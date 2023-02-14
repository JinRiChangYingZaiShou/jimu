#include "jimu_node.h"

namespace jimu {
namespace graph {

JimuNodeBase::JimuNodeBase() :
    node_id(0),
    _node_name(""),
    _graph_data(nullptr) {}

JimuNodeBase::~JimuNodeBase() {}

std::string JimuNodeBase::get_node_name() {
    return _node_name;
}

uint32_t JimuNodeBase::set_node_name(std::string node_name) {
    _node_name = node_name;
    return 0;
}

GraphDataBaseSharedPtr JimuNodeBase::graph_data() {
    return _graph_data;
}

uint32_t JimuNodeBase::set_graph_data(GraphDataBaseSharedPtr data) {
    _graph_data = data;
    return 0;
}

GraphDataBaseSharedPtr JimuNodeBase::builder_data() {
    return _builder_data;
}

uint32_t JimuNodeBase::set_builder_data(GraphDataBaseSharedPtr data) {
    _builder_data = data;
    return 0;
}

uint32_t JimuNodeBase::get_node_id() {
    return node_id;
}

uint32_t JimuNodeBase::set_node_id(uint32_t node_id) {
    this->node_id = node_id;
    return 0;
}

JimuNode::JimuNode() {}

JimuNode::~JimuNode() {}

uint32_t JimuNode::init() {
    return 0;
}

uint32_t JimuNode::reset() {
    return 0;
}

}
}