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

template<typename BuilderDataType, typename GraphDataType>
class JimuGraph;

template<typename BuilderDataType, typename GraphDataType>
class JimuGraphBuilder {
    NodeFactory<BuilderDataType, GraphDataType> node_factory;
    
    GraphDataContainer<BuilderDataType, GraphDataType>
        data_container;

    std::vector<std::tuple<std::string, std::string>> edge_vec;

public:

    uint32_t node_size;
    std::map<std::string, uint32_t> id_map_sti;
    std::map<uint32_t, std::string> id_map_its;

    std::vector<uint32_t> in_deg_vec;
    std::vector<std::vector<uint32_t>> out_edge_vec;
    
    std::vector<uint32_t> start_node_vec;
    std::vector<uint32_t> end_node_vec;

    JimuGraphBuilder() :
        data_container(),
        edge_vec(),
        node_factory(),
        node_size(0),
        id_map_its(),
        id_map_sti(),
        in_deg_vec(),
        out_edge_vec(),
        start_node_vec(),
        end_node_vec(),
        dfn(),
        low(),
        tarjan_cnt(0),
        stk(),
        in_stk(),
        find_cir(0) {}

    ~JimuGraphBuilder() {}

    uint32_t add_edge(std::string pre, std::string suf) {
        if (pre != suf) {
            return 1;
        }

        edge_vec.emplace_back(pre, suf);

        return 0;
    }
    
    template<typename T>
    uint32_t register_node(std::string node_name) {
        return node_factory.template register_node<T>(node_name);
    }

    uint32_t init(std::shared_ptr<BuilderDataType> data) {
        set_builder_data(data);

        if (build_graph()) {
            return 1;
        }

        if (check_circle()) {
            return 2;
        }

        return 0;
    }

    std::shared_ptr<BuilderDataType> builder_data() {
        return data_container.builder_data();
    }

    uint32_t build_node_map(
        std::map<std::string, std::shared_ptr<
        JimuNode<BuilderDataType, GraphDataType>>> &node_map,
        std::map<uint32_t, std::shared_ptr<
        JimuNode<BuilderDataType, GraphDataType>>> &node_id_map) {

        if (node_factory.build_node_map(node_map)) {
            return 1;
        }

        for (auto & [name, node] : node_map) {
            jimu::tools::map_insert(node_id_map, id_map_sti[name], node);
        }

        return 0;
    }

    std::shared_ptr<
    JimuGraph<BuilderDataType, GraphDataType>> make_graph() {
        std::cout << "make graph" << std::endl;
        return std::make_shared<
            JimuGraph<BuilderDataType, GraphDataType>>(this);
    }

private:
    uint32_t build_graph_vec(std::vector<std::string> name_vec) {
        for (const auto & name : name_vec) {
            
            jimu::tools::map_insert(id_map_sti, name, node_size);
            jimu::tools::map_insert(id_map_its, node_size, name);

            ++node_size;
        }

        in_deg_vec.resize(node_size, 0);
        out_edge_vec.resize(node_size);

        for (const auto & edge : edge_vec) {
            auto pre = std::get<0>(edge);
            auto suf = std::get<1>(edge);

            if (!id_map_sti.count(pre) || !id_map_sti.count(suf)) {
                return 1;
            }

            if (pre == suf) {
                return 2;
            }

            auto pre_id = id_map_sti[pre];
            auto suf_id = id_map_sti[suf];

            in_deg_vec[suf_id] = in_deg_vec[suf_id] + 1;
            out_edge_vec[pre_id].push_back(suf_id);
        }

        start_node_vec.clear();
        end_node_vec.clear();

        for (int i = 0; i < in_deg_vec.size(); i++) {
            if (!in_deg_vec[i]) {
                start_node_vec.push_back(i);
            }
            
            if (out_edge_vec[i].empty()) {
                end_node_vec.push_back(i);
            }
        }

        return 0;
    }

    uint32_t build_graph() {
        std::vector<std::string> name_vec;
        node_factory.get_name_vec(name_vec);

        if(build_graph_vec(name_vec)) {
            return 1;
        }

        return 0;
    }

    uint32_t set_builder_data(std::shared_ptr<BuilderDataType> data) {
        return data_container.set_builder_data(data);
    }

    std::vector<uint32_t> dfn;
    std::vector<uint32_t> low;
    uint32_t tarjan_cnt;
    std::stack<uint32_t> stk;
    std::vector<uint32_t> in_stk;
    uint32_t find_cir;

    uint32_t check_circle() {
        dfn.resize(node_size, node_size);
        low.resize(node_size, node_size);
        in_stk.resize(node_size, 0);
        
        for (int i = 0; i < node_size; i++) {
            if (dfn[i] >= node_size) {
                dfs(i);
                if (find_cir) {
                    return 1;
                }
            }
        }

        return 0;
    }

    uint32_t dfs(int x) {
        dfn[x] = tarjan_cnt;
        low[x] = tarjan_cnt;
        ++tarjan_cnt;
        stk.push(x);
        in_stk[x] = 1;

        for (auto edge : out_edge_vec[x]) {
            if (dfn[edge] >= node_size) {
                dfs(edge);
                low[x] = std::min(low[x], low[edge]);
            }
            else if (in_stk[edge]) {
                low[x] = std::min(low[x], dfn[edge]);
            }
        }

        if (dfn[x] == low[x]) {
            if (stk.top() != x) {
                find_cir = 1;
            }

            while (stk.top() != x) {
                stk.pop();
            }

            stk.pop();
        }

        return 0;
    }
};

template<typename BuilderDataType, typename GraphDataType>
class JimuGraph {
public:
    using BuilderType = JimuGraphBuilder<BuilderDataType, GraphDataType>;
    using NodeType = JimuNode<BuilderDataType, GraphDataType>;

private:

    BuilderType *builder;

    std::map<std::string, std::shared_ptr<NodeType>> node_map;
    std::map<uint32_t, std::shared_ptr<NodeType>> node_id_map;

    std::map<uint32_t, std::shared_ptr<std::counting_semaphore<1> > >
        seamaphore_map;
    
    std::shared_ptr<GraphDataContainer<BuilderDataType, GraphDataType> >
        data_container =
            std::make_shared<GraphDataContainer<BuilderDataType, GraphDataType>>();

    jimu::tools::Counter<uint32_t> start_count = jimu::tools::Counter<uint32_t>();

    JimuExecuter executer;

    uint32_t start_count_reset() {
        start_count.reset_all(0);
        return 0;
    }

    uint32_t seamaphore_map_reset() {
        seamaphore_map.clear();

        for (int i = 0; i < builder->node_size; i++) {
            if (builder->out_edge_vec[i].empty()) {
                jimu::tools::map_insert(seamaphore_map, i,
                    std::make_shared<std::counting_semaphore<1>>(0));
            }
        }

        return 0;
    }

    uint32_t set_graph_data(std::shared_ptr<GraphDataType> data) {
        return data_container->set_graph_data(data);
    }

    void run_node(uint32_t node_id) {
        auto node = node_id_map[node_id];

        node->process();

        if (builder->out_edge_vec[node_id].empty()) {
            seamaphore_map[node_id]->release(1);
            return;
        }

        for (auto next_node_id : builder->out_edge_vec[node_id]) {
            auto start_times = start_count.at(next_node_id).fetch_add(1);
            if (start_times == builder->in_deg_vec[next_node_id]) {
                executer.run(&JimuGraph::run_node, this, next_node_id);
            }
        }
    }

public:

    explicit JimuGraph(BuilderType *builder) :
        node_map(),
        node_id_map(),
        executer(),
        seamaphore_map() {
        this->builder = builder;
        data_container->set_builder_data(builder->builder_data());
        std::cout << "create graph done" << std::endl;
    }

    std::shared_ptr<GraphDataContainer<BuilderDataType, GraphDataType>>
        get_data_container() {
        return data_container;
    }

    ~JimuGraph() {}

    std::shared_ptr<GraphDataType> graph_data() {
        return data_container->graph_data();
    }

    std::shared_ptr<BuilderDataType> builder_data() {
        return data_container->builder_data();
    }

    uint32_t init(std::shared_ptr<GraphDataType> data,
        uint32_t thread_number) {
        set_graph_data(data);

        std::cout << "builde node map" << std::endl;

        if (builder->build_node_map(node_map, node_id_map)) {
            return 1;
        }

        std::cout << "node init" << std::endl;
        for (auto & [id, node] : node_id_map) {
            node->init();
        }

        start_count.resize(builder->node_size, 0);

        executer.init(thread_number);

        reset();
        return 0;
    }

    uint32_t reset() {
        if (graph_data() != nullptr) {
            graph_data()->reset();
        }

        start_count_reset();
        seamaphore_map_reset();

        for (auto & [id, node] : node_id_map) {
            node->reset();
        }

        return 0;
    }

    uint32_t run_graph() {
        std::vector<uint32_t> start_node_vec;
        for(int i = 0; i < builder->node_size; i++) {
            if (start_count.load(i) == builder->in_deg_vec[i]) {
                start_node_vec.push_back(i);
            }
        }

        for (auto node_id : start_node_vec) {
            executer.run(&JimuGraph::run_node, this, node_id);
        }

        for (auto & [key, value] : seamaphore_map) {
            value->acquire();
        }

        return 0;
    }
    
};

}
}

#endif