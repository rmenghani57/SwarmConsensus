from pycparser import parse_file, c_generator, c_parser, c_ast
from CodeGen import *
import yaml
from ast_to_json import *
import json
import os

def load_yaml(filepath):
    with open(filepath) as f:
        data = yaml.load(f, Loader=yaml.FullLoader)
    return data

def json_extract(obj, var_set, key):
    # Recursively search for values of key in JSON tree.
    if isinstance(obj, dict):
        for k, v in obj.items():
            if isinstance(v, (dict, list)):
                json_extract(v, var_set, key)
            elif k == key:
                var_set.add(v)
    elif isinstance(obj, list):
        for item in obj:
            json_extract(item, var_set, key)
    return var_set

def extract_global_refs_helper(body_ast):
    json_data = to_json(body_ast, sort_keys=True, indent=4)
    json_dict = json.loads(json_data)
    var_set = set()
    var_set = json_extract(json_dict, var_set, 'name')
    return var_set
    

def extract_global_refs(node, template_func):
    # Loading previously constructed yaml file to check for global references
    yaml_data = load_yaml('src/control/global_params/params.yaml')
    template_func["global_refs"] = dict()
    var_set = extract_global_refs_helper(node.body)
    for var in var_set:
        if var in yaml_data:
            template_func["global_refs"][var] = [type(yaml_data[var])]
            if isinstance(yaml_data[var], list):
                template_func["global_refs"][var].append(type(yaml_data[var][0]))
    return template_func

def extract_data_from_ast(ast):
    template_vars = list()
    template_funcs = list()

    for node in ast:
        if type(node) == c_ast.Decl:
            template_var = dict()
            if(type(node.type) == c_ast.TypeDecl):
                template_var["decl"] = "type_decl"
                template_var["name"] = node.type.declname
                template_var["type"] = node.type.type.names[0]
                if(node.init is not None):
                    template_var["value"] = node.init.value
                else:
                    template_var["value"] = "NA"
            elif(type(node.type) == c_ast.ArrayDecl):
                template_var["decl"] = "array_decl"
                template_var["name"] = node.type.type.declname
                template_var["type"] = node.type.type.type.names[0]
                try:
                    template_var["array_dim"] = node.type.dim.name
                except:
                    template_var["array_dim"] = node.type.dim.value
                array_init = list()
                if(node.init is not None):
                    for item in node.init.exprs:
                        if template_var["type"] == "int":
                            array_init.append(int(item.value))
                        elif template_var["type"] == "bool":
                            if item.name == "true":
                                array_init.append(1)
                            else:
                                array_init.append(0)
                        else:
                            array_init.append(item.name)
                    template_var["value"] = array_init
                else:
                    template_var["value"] = "NA"
                    
            template_vars.append(template_var)

        elif type(node) == c_ast.FuncDef:
            template_func = dict()
            template_func["name"] = node.decl.name
            template_func["return_type"] = node.decl.type.type.type.names[0]
            template_func["arguments"] = list()
            try:
                for param in node.decl.type.args.params:
                    template_func["arguments"].append([param.type.type.names[0], param.name])
            except:
                # Some Exception, or function has no arguments
                pass
            # extract global references in function body
            template_func = extract_global_refs(node, template_func)
            
            # function body
            generator = c_generator.CGenerator()
            func_body = generator.visit(node.body)
            func_body_lines = func_body.split("\n")
            processed_body_lines = list()
            for line in func_body_lines[1:-2]:
                processed_body_lines.append(line)
            template_func["body"] = processed_body_lines

            # Append this custom function dictionary into list holding all functions inside the template
            template_funcs.append(template_func)
    return [template_vars, template_funcs]


def construct_abstract_syntax_tree(template):
    template_declarations = [line for line in template.declarations.split("\n")]
    with open("interim_base_file.c", "w") as f:
        f.write("typedef enum { false, true } bool;\n")
        for line in template_declarations:
            temp_line = "".join(c for c in line if c != " ")
            if "for(" in temp_line:
                loop_var = temp_line.split("(")[1][0]
                loop_var_type = temp_line.split(":")[1].split("[")[0]

                loop_range_line = temp_line.split("[")[1]
                loop_range_line = loop_range_line.split("]")[0]
                loop_start = loop_range_line.split(",")[0]
                loop_end = loop_range_line.split(",")[1]
                f.write("for({} {}={}; {} < {}; {}++)\n".format(loop_var_type, loop_var, loop_start, loop_var, loop_end, loop_var))
            else:
                f.write(line + "\n")
    
    ast = parse_file("interim_base_file.c", use_cpp=True)
    # We don't need the intermediate c file anymore since we've already built the AST
    os.remove("interim_base_file.c")
    return ast

def generate_ros_base_class(template, template_vars, template_funcs):
    
    template_name = template.name
    file_path = "src/control/src/base_classes/"
    try:
        os.makedirs(file_path, exist_ok=True)
    except OSError:
        print("Creation of {} Base Class in {} has failed".format(template_name, file_path))
    else:
        print("Successfully Created {} Base Class Directory Structure\n Proceeding with {} Base Class Generation...".format(template_name, template_name))

    file_name = "{}{}_base_class.cpp".format(file_path, template_name)
    cpp = CppFile(file_name)
    cpp("#include <bits/stdc++.h>")
    cpp("#include <gnc_functions.hpp>")
    cpp("")
    with cpp.subs(class_name=template_name):
        with cpp.block("class $class_name$", ";"):
            cpp.label("private")
            for var_dict in template_vars:
                cpp("{} {};".format(var_dict["type"], var_dict["name"]))
            cpp.label("public")
            # Constructor code generation
            with cpp.block("$class_name$()"):
                cpp("nh = ros::NodeHandle(\"~\");")
                for var_dict in template_vars:
                    if(var_dict["value"] != "NA"):
                        if(var_dict["decl"] == "array_decl"):
                            temp_val = "{"
                            for idx, item in enumerate(var_dict["value"]):
                                if(var_dict["type"] == "bool"):
                                    if item:
                                        temp_val += "true"
                                    else:
                                        temp_val += "false"
                                else:
                                    temp_val += str(item)
                                if idx != len(var_dict["value"]) - 1:
                                    temp_val += ","
                                else:
                                    temp_val += "}"
                            var_dict["value"] = temp_val
                        cpp("this->{} = {};".format(var_dict["name"], var_dict["value"]))
            
            # Function code generation
            for func in template_funcs:
                args = "("
                for idx, arg in enumerate(func["arguments"]):
                    args += arg[0] + " " + arg[1]
                    if idx != len(func["arguments"]) - 1:
                        args += ","
                args += ")"
                block_name = "{} {}{}".format(func["return_type"], func["name"], args)
                with cpp.block(block_name):
                    for global_ref in func["global_refs"]:
                        # Global ref is not an array
                        val = func["global_refs"][global_ref]
                        if len(val) == 1:
                            if issubclass(val[0], int):
                                cpp("int {};".format(global_ref))
                            elif issubclass(val[0], bool):
                                cpp("bool {};".format(global_ref))
                        elif len(val) == 2:
                            if issubclass(val[1], int):
                                cpp("vector<int> {};".format(global_ref))
                            elif issubclass(val[1], float):
                                cpp("vector<double> {};".format(global_ref))
                            elif issubclass(val[1], bool):
                                cpp("vector<bool> {};".format(global_ref))
                        cpp("nh.getParam(\"/{}\", {});".format(global_ref, global_ref))
                    # printing out the function body
                    for line in func["body"]:
                        cpp(line)
    cpp.close()

# Here 'g' is the graph object
# def generate_ros_node(g):