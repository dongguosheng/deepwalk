#include "deepwalk.h"
#include <iostream>
#include <cstring>
#include <sys/time.h>
#include <cstdio>

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
    // std::vector<Vertex<unsigned int>*> data = g.GetData();
    int n_iter = atoi(argv[4]);
    int n_step = atoi(argv[5]);
    struct timeval st; gettimeofday( &st, NULL );
    g.GenRandomWalks(n_iter, n_step);
    struct timeval et; gettimeofday( &et, NULL );
    printf("timeval: %ld ms\n", (et.tv_sec - st.tv_sec) * 1000 + (et.tv_usec - st.tv_usec)/1000);

    const char *walks_file = argv[6];
    g.SaveTxt(walks_file);
    
    return 0;
}

