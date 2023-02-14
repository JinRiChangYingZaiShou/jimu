#ifndef JIMU_GRAPH_JIMU_NODE_H
#define JIMU_GRAPH_JIMU_NODE_H

#include <string>
#include <iostream>
#include <memory>

#include "graph_data.h"

namespace jimu {
namespace graph {

class JimuNodeBase {
    uint32_t node_id;
    std::string _node_name;
    GraphDataBaseSharedPtr _graph_data;
    GraphDataBaseSharedPtr _builder_data;

public:
    JimuNodeBase();
    ~JimuNodeBase();

    std::string get_node_name();
    uint32_t set_node_name(std::string node_name);

    GraphDataBaseSharedPtr graph_data();
    uint32_t set_graph_data(GraphDataBaseSharedPtr data);

    GraphDataBaseSharedPtr builder_data();
    uint32_t set_builder_data(GraphDataBaseSharedPtr data);

    uint32_t get_node_id();
    uint32_t set_node_id(uint32_t node_id);
};

class JimuNode : public JimuNodeBase {

public:
    JimuNode();
    ~JimuNode();

    virtual uint32_t init();
    virtual uint32_t process() = 0;
    virtual uint32_t reset();

};

}
}

#endif