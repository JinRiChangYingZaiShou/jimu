#ifndef JIMU_GRAPH_JIMU_NODE_H
#define JIMU_GRAPH_JIMU_NODE_H

#include <string>
#include <iostream>
#include <memory>

#include "graph_data.h"

namespace jimu {
namespace graph {

template<typename BuilderDataType, typename GraphDataType>
class JimuNodeBase {
    uint32_t node_id;
    std::string _node_name;

    GraphDataContainer<BuilderDataType, GraphDataType> _data_container;

public:
    JimuNodeBase() :
        node_id(0),
        _node_name(""),
        _data_container() {}

    ~JimuNodeBase() {}

    std::string get_node_name() {
        return _node_name;
    }

    uint32_t set_node_name(std::string node_name) {
        _node_name = node_name;
        return 0;
    }

    std::shared_ptr<GraphDataType> graph_data() {
        return _data_container.graph_data();
    }

    uint32_t set_graph_data(std::shared_ptr<GraphDataType> data) {
        return _data_container.set_graph_data(data);
    }

    std::shared_ptr<BuilderDataType> builder_data() {
        return _data_container.builder_data();
    }

    uint32_t set_builder_data(std::shared_ptr<BuilderDataType> data) {
        return _data_container.set_builder_data(data);
    }

    uint32_t get_node_id() {
        return node_id;
    }

    uint32_t set_node_id(uint32_t node_id) {
        this->node_id = node_id;
        return 0;
    }
};

template<typename BuilderDataType, typename GraphDataType>
class JimuNode : public JimuNodeBase<BuilderDataType, GraphDataType> {

public:
    JimuNode() : JimuNodeBase<BuilderDataType, GraphDataType>() {}
    ~JimuNode() {}

    virtual uint32_t init() {
        return 0;
    }

    virtual uint32_t process() = 0;

    virtual uint32_t reset() {
        return 0;
    }

};

}
}

#endif