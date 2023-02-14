#include "jimu_graph.h"
#include "../tools/common_tools.h"

namespace jimu {
namespace graph {

JimuGraphBuilder::JimuGraphBuilder() :
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

JimuGraphBuilder::~JimuGraphBuilder() {}

uint32_t JimuGraphBuilder::add_edge(
    std::string pre, std::string suf) {
    if (pre != suf) {
        return 1;
    }

    edge_vec.emplace_back(pre, suf);

    return 0;
}

uint32_t JimuGraphBuilder::build_graph_vec(std::vector<std::string> name_vec) {
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
}

uint32_t JimuGraphBuilder::build_graph() {
    std::vector<std::string> name_vec;
    node_factory.get_name_vec(name_vec);

    if(build_graph_vec(name_vec)) {
        return 1;
    }

    return 0;
}

uint32_t JimuGraphBuilder::dfs(int x) {
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

uint32_t JimuGraphBuilder::check_circle() {
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

GraphDataBaseSharedPtr JimuGraphBuilder::builder_data() {
    return _builder_data;
}

uint32_t JimuGraphBuilder::set_builder_data(
    GraphDataBaseSharedPtr data) {
    _builder_data = data;
}

uint32_t JimuGraphBuilder::init(GraphDataBaseSharedPtr data) {

    set_builder_data(data);

    if (build_graph()) {
        return 1;
    }

    if (check_circle()) {
        return 2;
    }

    return 0;
}

uint32_t JimuGraphBuilder::build_node_map(
    std::map<std::string, std::shared_ptr<JimuNode>> &node_map,
    std::map<uint32_t, std::shared_ptr<JimuNode>> &node_id_map) {
    if (node_factory.build_node_map(node_map)) {
        return 1;
    }

    for (auto & [name, node] : node_map) {
        jimu::tools::map_insert(node_id_map, id_map_sti[name], node);
    }

    return 0;
}

JimuGraph::JimuGraph(JimuGraphBuilder * builder) :
    node_map(),
    node_id_map(),
    _graph_data(nullptr),
    start_count(),
    executer() {
    this->builder = builder;
    _builder_data = builder->builder_data();
}

JimuGraph::~JimuGraph() {}

GraphDataBaseSharedPtr JimuGraph::graph_data() {
    return _graph_data;
}

uint32_t JimuGraph::set_graph_data(GraphDataBaseSharedPtr data) {
    _graph_data = data;
}

GraphDataBaseSharedPtr JimuGraph::builder_data() {
    return _builder_data;
}

uint32_t JimuGraph::init(GraphDataBaseSharedPtr data,
    uint32_t thread_number) {
    set_graph_data(data);

    if (builder->build_node_map(node_map, node_id_map)) {
        return 1;
    }

    for (auto & [id, node] : node_id_map) {
        node->init();
    }

    start_count.resize(builder->node_size, 0);

    executer.init(thread_number);

    reset();
    return 0;
}

uint32_t JimuGraph::start_count_reset() {
    for (auto & i : start_count) {
        i.store(0);
    }

    return 0;
}

uint32_t JimuGraph::seamaphore_map_reset() {
    seamaphore_map.clear();

    for (int i = 0; i < builder->node_size; i++) {
        if (builder->out_edge_vec[i].empty()) {
            jimu::tools::map_insert(seamaphore_map, i,
                std::make_shared<std::counting_semaphore<1>>(0));
        }
    }

    return 0;
}

uint32_t JimuGraph::reset() {
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

void JimuGraph::run_node(uint32_t node_id) {
    auto node = node_id_map[node_id];

    node->process();

    if (builder->out_edge_vec[node_id].empty()) {
        seamaphore_map[node_id]->release(1);
        return;
    }

    for (auto next_node_id : builder->out_edge_vec[node_id]) {
        auto start_times = start_count[next_node_id].fetch_add(1);
        if (start_times == builder->in_deg_vec[next_node_id]) {
            executer.run(&JimuGraph::run_node, this, next_node_id);
        }
    }
}

uint32_t JimuGraph::run_graph() {
    std::vector<uint32_t> start_node_vec;
    for(int i = 0; i < builder->node_size; i++) {
        if (start_count[i] == builder->in_deg_vec[i]) {
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

}
}