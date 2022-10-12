import utils
import c_utils

if __name__ == '__main__':
    templates = utils.extract_templates('Swarm_consensus_final.xml')
    global_params = utils.extract_text('Swarm_consensus_final.xml', 'declaration')
    system_declarations = utils.extract_text('Swarm_consensus_final.xml', 'system')
 
    # Code Generation for ROS Global Params File
    utils.generate_ros_params(global_params)
    # Code Generation for ROS Launch File
    utils.generate_ros_launch(system_declarations)

    # Do the following steps for each of the extracted XML Templates
    for t in templates:
        # Dump UPPAAL code into a while and transform into C code. This C code shall
        # then be parsed by the pycgenerator to build and return an abstract syntax tree (AST)
        abstract_syntax_tree = c_utils.construct_abstract_syntax_tree(templates[t])
        # Use this AST to extract local_variable declarations and function declerations inside the template
        template_vars, template_funcs = c_utils.extract_data_from_ast(abstract_syntax_tree)
        # And Finally, use the refined data contained in template_vars and template_funcs to generate a ROS base class
        c_utils.generate_ros_base_class(templates[t], template_vars, template_funcs)