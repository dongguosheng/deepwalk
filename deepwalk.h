#ifndef DEEPWALK_H
#define DEEPWALK_H

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <algorithm>
#include <random>
#include <ctime>
#include <omp.h>
#include <cstring>
#include <utility>
#include <unordered_map>

namespace deepwalk {
    const static int THREAD_NUM = omp_get_max_threads();
    template<typename T = unsigned int>
    struct Vertex {
        T id;
        std::vector<std::pair<Vertex*, size_t> > adjacent_list;
        std::vector<size_t> cum_table;
        Vertex(T _id) : id(_id) {}
        Vertex() {}
    };

    template<typename T = unsigned int>
    class Graph {
    public:
        Graph() {}
        Graph(const char *filename, T n_vertex, int start_idx) {
            //TODO: load different type of graph file
            data = std::vector<Vertex<T>*>(n_vertex, NULL);
            bool rs = false;
            n_edge = 0;

            if (strstr(filename, "adj") != NULL)    rs = LoadAdjList(filename, start_idx);
            else if (strstr(filename, "edge") != NULL)    rs = LoadEdgeList(filename, start_idx);
            else {
                std::cerr << "file format not supported yet." << std::endl;
            }
            if (!rs) std::cerr << "load graph file error." << std::endl;
            rng.seed(static_cast<int>(time(NULL)));
            MakeCumTable();
        }
        virtual ~Graph() {
            for (auto ptr_v : data) {
                delete ptr_v;
                ptr_v = NULL;
            }
        }
        inline bool LoadAdjList(const char *filename, int start_idx) {
            std::ifstream fin(filename);
            if (fin.fail()) {
                std::cerr << filename << " read error." << std::endl;
                return false;
            }
            std::string line;
            while (std::getline(fin, line)) {
                std::istringstream ss(line);
                T tmp_id, v_id;
                int cnt = 0;
                while (!ss.eof()) {
                    if (!(ss >> tmp_id))    break;
                    tmp_id -= start_idx;
                    assert(tmp_id < data.size());
                    if (cnt == 0) {
                        v_id = tmp_id;
                        if(data[v_id] == NULL)    data[v_id] = new Vertex<T>(v_id);
                    }
                    else {
                        assert(data[v_id] != NULL);
                        if (data[tmp_id] == NULL)    data[tmp_id] = new Vertex<T>(tmp_id);
                        // adj list with weight not supported yet.
                        data[v_id]->adjacent_list.push_back(std::make_pair(data[tmp_id], 1));
                        n_edge++;
                    }
                    cnt++;
                }
            }
            n_edge /= 2;
            fin.close();
            std::cout << "Read adjlist complete. Total " << data.size() << " vertex, " << n_edge << " edges." << std::endl;
            return true;
        }
        inline bool LoadEdgeList(const char *filename, int start_idx) {
            std::ifstream fin(filename);
            if (fin.fail()) {
                std::cerr << filename << " read error." << std::endl;
                return false;
            }
            std::string line;
            while (std::getline(fin, line)) {
                std::istringstream ss(line);
                T id_left, id_right;
                ss >> id_left >> id_right;
                size_t weight = 1;
                ss >> weight;
                id_left -= start_idx;
                id_right -= start_idx;
                assert(id_left < data.size());
                assert(id_right < data.size());    
                if (data[id_left] == NULL)    data[id_left] = new Vertex<T>(id_left);
                if (data[id_right] == NULL)    data[id_right] = new Vertex<T>(id_right);
                data[id_left]->adjacent_list.push_back(std::make_pair(data[id_right], weight));
                data[id_right]->adjacent_list.push_back(std::make_pair(data[id_left], weight));
                n_edge++;
            }
            fin.close();
            std::cout << "Read edgelist complete. Total " << data.size() << " vertex, " << n_edge << " edges." << std::endl;
            return true;
        }
        inline bool LoadVertexName(const char *filename) {
            std::ifstream fin(filename);
            if (fin.fail()) {
                std::cerr << filename << " read error." << std::endl;
                return false;
            }
            std::string line;
            while (std::getline(fin, line)) {
                std::istringstream ss(line);
                std::string vname;
                T vid;
                ss >> vname >> vid;
                vidtoname[vid] = vname;
            }
            std::cout << "Load Vertex List Complete." << std::endl;
            fin.close();
            return true;
        }
        inline void MakeCumTable() {
            typedef std::pair<Vertex<T>*, size_t> edge_t;
            for(auto v_ptr : data) {
                // sort adjlist, higher weight with lower index
                std::sort((v_ptr->adjacent_list).begin(), (v_ptr->adjacent_list).end(), [](const edge_t left, const edge_t right){ return left.second > right.second; });
                (v_ptr->cum_table).reserve(v_ptr->adjacent_list.size());
                size_t weight_sum = 0;
                for(auto edge : v_ptr->adjacent_list) {
                    weight_sum += edge.second;
                    (v_ptr->cum_table).push_back(weight_sum);
                }
            }
        }
        inline void GenRandomWalks(int n_iter, int n_step) {
            paths = std::vector<std::vector<T> >(n_iter * data.size(), std::vector<T>());
            for (int i = 0; i < n_iter; ++i) {
                std::random_shuffle(data.begin(), data.end());
                int n_vertex = static_cast<int>(data.size());
                # pragma omp parallel for num_threads(THREAD_NUM)
                for (int j = 0; j < n_vertex; ++ j) {
                    if (data[j]->adjacent_list.empty())    continue;
                    paths[i*data.size() + j] = Walk(data[j], n_step);
                    // Walk(data[j], n_step, paths[i*data.size() + j]);
                }
            }
        }
        inline std::vector<T> Walk(const Vertex<T>* ptr_v, int n_step) {
            std::vector<T> path(n_step + 1, 0);
            Walk(ptr_v, n_step, path);
            return path;
        }
        inline size_t BinarySearch(const std::vector<size_t> &cum_table, size_t rand_num) {
            size_t idx;
            size_t left = 0, right = cum_table.size() - 1;
            while(left <= right) {
                idx = left + (right - left) / 2;
                if(idx == 0 || idx == cum_table.size() - 1) break;
                if (rand_num > cum_table[idx-1] && rand_num <= cum_table[idx])  break;
                else if (rand_num > cum_table[idx] && rand_num <= cum_table[idx+1]) { idx++; break; }
                else if (rand_num > cum_table[idx+1]) left = idx + 1;
                else    right = idx - 1;
            }
            return idx;
        }
        inline void Walk(const Vertex<T>* ptr_v, int n_step, std::vector<T> &path) {
            path[0] = ptr_v->id;
            size_t rand_num, idx;
            int i = 1;
            const Vertex<T>* ptr_now = ptr_v;
            while (i <= n_step) {
                assert(ptr_now->adjacent_list.size() > 0 && ptr_now->adjacent_list.size() == ptr_now->cum_table.size());
                std::uniform_int_distribution<size_t> ud(1, ptr_now->cum_table[(ptr_now->cum_table).size() - 1]);
                rand_num = ud(rng);
                idx = BinarySearch(ptr_now->cum_table, rand_num);
                ptr_now = ptr_now->adjacent_list[idx].first;
                path[i] = ptr_now->id;
                i++;
            }
        }
        inline bool SaveTxt(const char *filename) {
            //TODO: Too Slow ...
            std::ofstream fout(filename);
            if (fout.fail()) {
                std::cerr << filename << " open error." << std::endl;
                return false;
            }
            for (auto path : paths) {
                for (auto id : path) {
                    if (vidtoname.size() > 0)   { assert(vidtoname.find(id) != vidtoname.end()); fout << vidtoname[id] << " "; }
                    else    fout << id << " ";
                }
                fout << std::endl;
            }
            fout.close();
            return true;
        }
        inline bool SaveBinary(const char *filename) {
            std::ofstream fout(filename, std::ios::out | std::ios::binary);
            if (fout.fail()) {
                std::cerr << filename << " open error." << std::endl;
                return false;
            }
            for (auto path : paths) {
                fout.write(reinterpret_cast<const char*>(&path[0]), path.size() * sizeof(T));
            }
            fout.close();
        }
        inline std::vector<Vertex<T>*> GetData() const {
            return data;
        }
        inline std::vector<std::vector<T> > GetPaths() const {
            return paths;
        }
        inline Vertex<T>* operator[](const T idx) const {
            return data[idx];
        }
        inline size_t GetDegree(const T idx) const {
            return data[idx].size();
        }
    private:
        std::vector<Vertex<T>*> data;
        unsigned long n_edge;
        std::vector<std::vector<T> > paths;
        std::mt19937 rng;
        std::unordered_map<T, std::string> vidtoname;

        Graph(const Graph &other) {}
        Graph& operator=(const Graph &other) {}
    };
}


#endif /*DEEPWALK_H*/
