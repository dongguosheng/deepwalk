#include "DeepWalk.h"
#include <iostream>
#include <cstring>

int main(int argc, const char *argv[])
{
	using namespace deepwalk;
	using namespace std;
    // args parse
    if (argc < 2) {
        cout << "./run_Main graph_file num_vertex start_index(0/1) num_iter num_step output_file" << endl;
        return -1;
    }
	const char *filename = argv[1];
	unsigned int n_vertex = atoi(argv[2]);
	int start_idx = atoi(argv[3]);

	Graph<> g(filename, n_vertex, start_idx);
	std::vector<Vertex<unsigned int>*> data = g.GetData();
	int n_iter = atoi(argv[4]);
	int n_step = atoi(argv[5]);
	g.GenRandomWalks(n_iter, n_step);
	
	const char *walks_file = argv[6];
	g.SaveTxt(walks_file);
    
    return 0;
}

