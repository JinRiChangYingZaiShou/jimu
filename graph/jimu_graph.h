#ifndef JIMU_GRAPH_JIMU_GRAPH_H
#define JIMU_GRAPH_JIMU_GRAPH_H

#include <map>
#include <tuple>
#include <vector>
#include <stack>
#include <atomic>
#include <semaphore>

#include "graph_data.h"
#include "jimu_node.h"
#include "node_factory.h"
#include "jimu_executer.h"

namespace jimu {
namespace graph {

class JimuGraph;

class JimuGraphBuilder {
    NodeFactory node_factory;
    GraphDataBaseSharedPtr _builder_data;

    std::vector<std::tuple<std::string, std::string>> edge_vec;

public:

    uint32_t node_size;
    std::map<std::string, uint32_t> id_map_sti;
    std::map<uint32_t, std::string> id_map_its;

    std::vector<uint32_t> in_deg_vec;
    std::vector<std::vector<uint32_t>> out_edge_vec;
    
    std::vector<uint32_t> start_node_vec;
    std::vector<uint32_t> end_node_vec;

    JimuGraphBuilder();
    ~JimuGraphBuilder();

    uint32_t add_edge(std::string pre, std::string suf);
    
    template<typename T>
    uint32_t register_node(std::string node_name) {
        return node_factory.register_node<T>(node_name);
    }

    uint32_t init(GraphDataBaseSharedPtr data);

    GraphDataBaseSharedPtr builder_data();

    uint32_t build_node_map(
        std::map<std::string, std::shared_ptr<JimuNode>> &node_map,
        std::map<uint32_t, std::shared_ptr<JimuNode>> &node_id_map);

private:
    uint32_t build_graph_vec(std::vector<std::string> name_vec);
    uint32_t build_graph();

    uint32_t set_builder_data(GraphDataBaseSharedPtr data);

    std::vector<uint32_t> dfn;
    std::vector<uint32_t> low;
    uint32_t tarjan_cnt;
    std::stack<uint32_t> stk;
    std::vector<uint32_t> in_stk;
    uint32_t find_cir;

    uint32_t check_circle();
    uint32_t dfs(int x);
};

class JimuGraph {
    JimuGraphBuilder *builder;
    std::map<std::string, std::shared_ptr<JimuNode>> node_map;
    std::map<uint32_t, std::shared_ptr<JimuNode>> node_id_map;

    std::map<uint32_t, std::shared_ptr<std::counting_semaphore<1>>>
        seamaphore_map;
    
    GraphDataBaseSharedPtr _graph_data;
    GraphDataBaseSharedPtr _builder_data;

    std::vector<std::atomic<uint32_t>> start_count;

    JimuExecuter executer;
    
    uint32_t set_graph_data(GraphDataBaseSharedPtr data);

    uint32_t start_count_reset();
    uint32_t seamaphore_map_reset();

    void run_node(uint32_t node_id);

public:
    JimuGraph(JimuGraphBuilder *builder);
    ~JimuGraph();

    GraphDataBaseSharedPtr graph_data();
    GraphDataBaseSharedPtr builder_data();

    uint32_t init(GraphDataBaseSharedPtr data, uint32_t thread_number);
    uint32_t reset();
    uint32_t run_graph();
    
};

}
}

#endif