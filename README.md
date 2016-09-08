# deepwalk
weighted deepwalk implementation in c++.

# Build
`make`

# Usage
1. `python scripts/validate_graph.py graph_file`    
2. `./walk graph_file max_vertex_idx start_index(0/1) num_iter num_step output_file [idx_list_file]`    
`graph_file`: something.edge or something.adj or something.edge.directed. Only left -> right is an edge in edge.directed file.      
`max_vertex_idx`: max graph node index.     
`start_index`: graph node start index, 0 or 1.  
`num_iter`: iteration number of all nodes.  
`num_step`: max step number of each walk.   
`output_file`:  output walks file.  
`idx_list_file`: graph node name and idx list, `name\tindex\n`.   

## example
`python scripts/validate_graph.py data/sample.edgelist.weight.txt`  
you will get output: `(valid) start index: 0, max vertex idx: 6300, total 6301 vertex.`   
`./walk data/sample.edgelist.weight.txt 6300 0 25 40 random_walks.txt`    

# Requirements
`C++11`  
`OpenMP`    
