#include "../deepwalk.h"
#include <iostream>
#include <algorithm>
#include <ctime>

int main()
{
    using namespace deepwalk;
    using namespace std;
    
    // const char *filename = "./data/sample.adjlist.txt";
    // unsigned int n_vertex = 34;
    // int start_idx = 1;
    
    const char *filename = "./data/sample.edgelist.txt";
    unsigned int n_vertex = 6301;
    int start_idx = 0;

    Graph<> g(filename, n_vertex, start_idx);
    std::vector<Vertex<unsigned int>*> data = g.GetData();
    // std::random_shuffle(data.begin(), data.end());

    for (auto p_v : data) {
        cout << p_v->id << endl;
        cout << "adj list size: " << p_v->adjacent_list.size() << endl;
        // cout << "adj list: ";
        // for (auto p_adj : p_v->adjacent_list) {
        //     cout << p_adj->id << "|";
        //     cout << p_adj->adjacent_list.size() << ",";
        // }
        // cout << endl;
        if (p_v->adjacent_list.size() == 0) {
            cout << "EMPTY!" << endl;
            break;
        }
    }
    cout << "----------------" << endl;
    int n_iter = 10;
    int n_step = 40;
    clock_t start = clock();
    g.GenRandomWalks(n_iter, n_step);
    clock_t end = clock();
    cout << "Cost " << static_cast<float>(end - start) / CLOCKS_PER_SEC * 1000 << " ms." << endl;
    // show random walks
    
    const char *walks_file = "random_walks.txt";
    // const char *walks_file_b = "random_walks.bin";
    g.SaveTxt(walks_file);
    // g.SaveBinary(walks_file_b);

    /*
    std::vector<std::vector<unsigned int> > paths = g.GetPaths();
    int cnt = 0;
    for (auto path : paths) {
        cnt++;
        if (path.size() != 41) {
            cout << "error. "  << cnt << ", path size: " << path.size() << endl;
            break;
        }
        for (auto id : path) {
            cout << id << ",";
        }
        cout << endl;
    }
    */
    return 0;
}

