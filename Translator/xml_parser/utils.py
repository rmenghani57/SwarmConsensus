import xml.etree.ElementTree as ET
from custom_graph_objects import Node, Template
import lxml.etree as etree
import yaml
import os

def extract_templates(filename):
    tree = ET.parse(filename)
    root = tree.getroot()
    templates = dict()
    for t in root.findall('template'):
        template_name = t.find('name').text
        template_declarations = t.find('declaration').text
        # Creating template object
        template = Template(template_name, template_declarations)
        # Extracting all nodes inside a template
        for n in t.findall('location'):
            node_id = n.attrib['id']
            node_name = n.find('name').text
            is_committed = False
            try:
                is_committed = n.find('committed').text
                is_committed = True
            except:
                pass
            # Adding the custom Node object into the custom Template object
            template.add_node(node_id, node_name, is_committed)
            if node_name == "Start" or node_name == "Idle":                                                # MODIFICATION for drone IDLE state in template
                template.start_id = node_id

        # Extracting all edges inside a template
        for e in t.findall('transition'):
            # Finding the nodes that need to be connected
            source = e.find('source').attrib['ref']
            target = e.find('target').attrib['ref']
            # Find all the (label_type, label_value) pairs associated with an edge
            labels = []
            for label in e.findall('label'):
                kind = label.attrib['kind']
                value = label.text
                labels.append([kind, value])
            # append [target, [labels]] to the key 'source' in the adjacency list inside the template object
            template.add_directed_edge(source, target, labels)

        template.declarations = template_declarations
        # Storing each of the templates / graphs in a dictionary, where the template's name is the key
        templates[template_name] = template
    return templates


def extract_text(filename, tag_name):
    tree = ET.parse(filename)
    root = tree.getroot()
    text = ""
    for g in root.findall(tag_name):
        text = g.text
    return text

def generate_ros_params(global_params):

    # Create folder structure to store ros params file
    path = "src/control/global_params/"
    try:
        os.makedirs(path, exist_ok=True)
    except OSError:
        print("Creation of ROS Global Parameters File in {0} has failed".format(path))
    else:
        print("Successfully Created ROS Global Parameters Directory Structure\n Proceeding with ROS Global Parameters File Generation...")

    
    assigned_lookup = dict()
    lines = [line.strip() for line in global_params.split("\n")]
    yaml_dict = dict()

    for line in lines:
        if(not line.startswith("//") and len(line)):
            parts = [x.strip() for x in line.split(" ")]
            processed_parts = parts
            for idx, part in enumerate(parts):
                if part == "//" or part.startswith("//"):
                    processed_parts = parts[:idx]
                    break
            # Remove semi-colons
            processed_parts[-1] = processed_parts[-1][:-1]

            # channels are not translated
            if "chan" in processed_parts:
                yaml_dict[processed_parts[-1]] = 0
            
            # storing variables with assigned values so that they can be accessed when
            # translating other lines (like using N to initialize an array to all 0s)
            elif "=" in processed_parts:
                for idx, part in enumerate(processed_parts):
                    if part == "=":
                        left = processed_parts[idx - 1]
                        right = " ".join(processed_parts[idx+1:])
                        processed_right = right.replace("{", "[")
                        processed_right = processed_right.replace("}", "]")
                        # to check if the we are dealing with an array or normal variable
                        if right != processed_right:
                            left = left.split("[")[0]
                            processed_right = processed_right[1:-1]
                            processed_right = [int(x) for x in processed_right.split(",")]
                        else:
                            processed_right = int(processed_right)
                        yaml_dict[left] = processed_right
                        assigned_lookup[left] = processed_right
            else:
                # Checking if the current declaration is an Array
                isArray = False
                for idx, part in enumerate(processed_parts):
                    if "[" in part:
                        isArray = True
                        part_type = processed_parts[idx - 1]
                        part_name = part.split("[")[0]
                        # To check if the length is a variable or a concrete value
                        try:
                            length = int(assigned_lookup[part.split("[")[1].split("]")[0]])
                        except:
                            length = int(part.split("[")[1].split("]")[0])
                        
                        if part_type == "int":
                            yaml_dict[part_name] = [0 for x in range(length)]    #this is filling int array with zeros
                        elif part_type == "bool":
                            yaml_dict[part_name] = [False for x in range(length)]  # this fils bool arrays with false 

                # This leaves us with the last case of an uninitialized variable
                if(not isArray):
                    part_type = processed_parts[0]
                    part_name = processed_parts[1]
                    if part_type == "int":
                        yaml_dict[part_name] = 0
                    elif part_type == "bool":
                        yaml_dict[part_name] = [False]

        with open(path + "params.yaml", "w") as f:
            f.write(yaml.dump(yaml_dict, default_flow_style=None))

def generate_ros_launch(system_declarations):

    # Cleaning up comments in the system declarations
    lines = [line.strip() for line in system_declarations.split("\n")]
    bad_chars = ' ;'

    # List that will hold all the node attributes necessary for ROS Nodes in ROS Launch File
    nodes = list()
    nodes_seen = dict()
    for line in lines:
        # Check if line is commented
        if not line.startswith("//"):
            # To trim spaces and semicolons for objects in systems declaration
            parts = ["".join(c for c in part if c not in bad_chars) for part in line.split("=")]
            # To check if given line is indeed an object instantiation line
            if(len(parts) == 2):
                # Construct Node dictionary
                node = {"type" : "dummy", "name" : "dummy"}

                node["type"] = parts[1].split("(")[0]
                if(node["type"] in nodes_seen):
                    nodes_seen[node["type"]] += 1
                else:
                    nodes_seen[node["type"]] = 0
                node["name"] = "{}{}".format(node["type"],nodes_seen[node["type"]])
                nodes.append(node)


    # Create folder to store ros launch file
    path = "src/control/launch/"
    try:
        os.makedirs(path, exist_ok=True)
    except OSError:
        print("Creation of ROS Launch File in {0} has failed".format(path))
    else:
        print("Successfully Created ROS Launch Directory Structure\n Proceeding with ROS Launch File Generation...")

    # Construct XML tree from nodes extracted from system declaration file
    ros_tree = ET.Element("launch")

    load_yaml = ET.SubElement(ros_tree, "rosparam", attrib={"command":"load", "file":"$(find control)/global_params/params.yaml"})

    for node in nodes:
        if nodes_seen[node["type"]] > 0:
            ros_group = ET.SubElement(ros_tree, "group")
            ros_group_node = ET.SubElement(ros_group, "node", attrib={"pkg":"control", "type":"{}".format(node["type"]), "name":"{}".format(node["name"]), "output":"screen", "ns":"/{}".format(node["name"])})
            ros_node_param1 = ET.SubElement(ros_group_node, "param", attrib={"name":"namespace", "value":"/{}".format(node["name"])})
            ros_node_param2 = ET.SubElement(ros_group_node, "param", attrib={"name":"use_sim_time", "value":"true"})
        else:
            ros_node = ET.SubElement(ros_tree, "node", attrib={"pkg":"control", "type":"{}".format(node["type"]), "name":"{}".format(node["name"])})
    
    # Dumping XML Tree String into interim ROS Launch file
    xml_string = ET.tostring(ros_tree)
    with open(path+"control.launch", "w") as f:
        f.write(xml_string.decode("utf-8"))
    
    # Parsing string dump into lxml etree object so that
    # we can leverage the pretty function from the lxml library (print xml with indents)
    x = etree.parse(path+"control.launch")
    with open(path+"control.launch", "w") as f:
        f.write(etree.tostring(x, pretty_print=True).decode("utf-8"))