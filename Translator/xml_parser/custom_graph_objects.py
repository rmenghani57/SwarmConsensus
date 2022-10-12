class Node(object):
    def __init__(self, id, name, committed):
        self.id = id
        self.name = name
        self.committed = committed

class Template(object):
    def __init__(self, name, declarations=""):
        # Will contain nodes and their neighbors (only ids)
        self.name = name
        self.adjacency_list = {}
        self.nodes = {}
        self.declarations = ""
        self.start_id = ""
    
    # Helper function that adds [node_id : Node Object] (key, value) pairs 
    # and initializes the adjacency list of the added node
    def add_node(self, id, name, committed=False):
        self.nodes[id] = Node(id, name, committed)
        self.adjacency_list[id] = list()

    # Adds a directed edges between id1 and id2
    def add_directed_edge(self, id1, id2, labels):
        self.adjacency_list[id1].append([id2, labels])