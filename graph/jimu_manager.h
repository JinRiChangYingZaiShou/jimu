#ifndef JIMU_GRAPH_JIMU_MANAGER_H
#define JIMU_GRAPH_JIMU_MANAGER_H

#include <any>
#include <mutex>
#include <atomic>
#include <memory>
#include <condition_variable>

#include "jimu_graph.h"
#include "jimu_node.h"

namespace jimu {
namespace graph {

template<typename BuilderDataType,
    typename GraphDataType, typename AdapterType>
class JimuGraphManager {
public:
    using BuilderType = JimuGraphBuilder<BuilderDataType, GraphDataType>;
    using GraphType = JimuGraph<BuilderDataType, GraphDataType>;

private:
    uint32_t vec_size;
    std::shared_ptr<BuilderType> builder;
    std::vector<std::shared_ptr<GraphType>> graph_vec;
    jimu::tools::Counter<bool> graph_status_vec = jimu::tools::Counter<bool>();
    std::mutex graph_vec_mutex;
    std::condition_variable condition;

    std::pair<std::shared_ptr<GraphType>, int> scan_graph_vec() {
        for (int i = 0 ; i < vec_size; i++) {
            if (!graph_status_vec.load(i)) {
                graph_status_vec.store(i, true);
                return std::make_pair(graph_vec[i], i);
            }
        }

        std::shared_ptr<GraphType> temp(nullptr);
        return std::make_pair(temp, 0);
    }

    std::pair<std::shared_ptr<GraphType>, int> request_graph() {
        std::unique_lock<std::mutex> lock(graph_vec_mutex);

        auto scan_res = scan_graph_vec();

        if (scan_res.second != 0) {
            lock.unlock();
            return scan_res;
        }

        this->condition.wait(lock);

        lock.unlock();
        return scan_graph_vec();
    }

    uint32_t release_graph(std::shared_ptr<GraphType> graph, int id) {
        std::unique_lock<std::mutex> lock(graph_vec_mutex);
        graph_status_vec.store(id, false);
        condition.notify_one();
        lock.unlock();

        return 0;
    }

public:
    JimuGraphManager() :
        vec_size(0),
        builder(std::make_shared<BuilderType>()),
        graph_vec(),
        graph_vec_mutex(),
        condition() {}
    
    ~JimuGraphManager() {}

    std::shared_ptr<BuilderType> get_builder() {
        return builder;
    }

    uint32_t init(uint32_t graph_size, uint32_t graph_thread_number) {
        vec_size = graph_size;

        if (builder->init(std::make_shared<BuilderDataType>())) {
            return 1;
        }
        
        graph_vec.resize(graph_size);
        graph_status_vec.resize(graph_size, false);

        for(auto & ptr : graph_vec) {
            ptr = builder->make_graph();
            if (ptr->init(std::make_shared<GraphDataType>(), graph_thread_number)) {
                return 2;
            }
        }

        return 0;
    }

    uint32_t process(std::any request, std::any response) {
        auto graph_pair = request_graph();
        auto graph = graph_pair.first;
        auto adapter = std::make_shared<AdapterType>();
        
        adapter->set_data_container(graph->get_data_container());
        adapter->set_request(request);
        graph->run_graph();
        adapter->get_response(response);
        graph->reset();

        release_graph(graph, graph_pair.second);

        return 0;
    }

};

}
}

#endif