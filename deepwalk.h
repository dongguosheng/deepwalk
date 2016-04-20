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

namespace deepwalk {
    const static int THREAD_NUM = omp_get_max_threads();
    template<typename T = unsigned int>
    struct Vertex {
        T id;
        std::vector<Vertex*> adjacent_list;
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
                        data[v_id]->adjacent_list.push_back(data[tmp_id]);
                        n_edge++;
                    }
                    cnt++;
                }
            }
            n_edge /= 2;
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
                id_left -= start_idx;
                id_right -= start_idx;
                assert(id_left < data.size());
                assert(id_right < data.size());    
                if (data[id_left] == NULL)    data[id_left] = new Vertex<T>(id_left);
                if (data[id_right] == NULL)    data[id_right] = new Vertex<T>(id_right);
                data[id_left]->adjacent_list.push_back(data[id_right]);
                data[id_right]->adjacent_list.push_back(data[id_left]);
                n_edge++;
            }
            fin.close();
            std::cout << "Read edgelist complete. Total " << data.size() << " vertex, " << n_edge << " edges." << std::endl;
            return true;
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
        inline void Walk(const Vertex<T>* ptr_v, int n_step, std::vector<T> &path) {
            path[0] = ptr_v->id;
            size_t idx;
            int i = 1;
            const Vertex<T>* ptr_now = ptr_v;
            while (i <= n_step) {
                assert(ptr_now->adjacent_list.size() > 0);
                std::uniform_int_distribution<size_t> ud(0, ptr_now->adjacent_list.size() - 1);
                idx = ud(rng);
                ptr_now = ptr_now->adjacent_list[idx];
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
                for (auto id : path)    fout << id << " ";
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

        Graph(const Graph &other) {}
        Graph& operator=(const Graph &other) {}
    };
}


#endif /*DEEPWALK_H*/
