# deepwalk
weighted deepwalk implementation in c++.

# Build
`make`

# Usage
1. `python scripts/validate_graph.py graph_file`    
2. `./walk graph_file num_vertex start_index(0/1) num_iter num_step output_file [idx_list_file]`    
`graph_file`: edge or adj file.     
`start_index`: graph node start index, 0 or 1.  
`num_iter`: iteration number of all nodes.  
`num_step`: max step number of each walk.   
`output_file`:  output walks file.  
`idx_list_file`: graph node name and idx list, `name\tindex\n`.   

## example
`python scripts/validate_graph.py data/sample.edgelist.weight.txt`  
you will get output: `(valid) start index: 0, total 6301 vertex.`   
`./walk data/sample.edgelist.weight.txt 6301 0 25 40 random_walks.txt`    

# Requirements
`C++11`  
`OpenMP`    
