#include "node_factory.h"
#include "../tools/common_tools.h"

namespace jimu {
namespace graph {

NodeFactory::NodeFactory() : _node_factory_map() {}

NodeFactory::~NodeFactory() {}

uint32_t NodeFactory::build_node_map(std::map<std::string,
        JimuNodeSharedPtr> &node_map) {
    for (auto & [name, node_ptr] : _node_factory_map) {
        jimu::tools::map_insert(node_map, name, node_ptr->build_node());
    }

    return 0;
}

uint32_t NodeFactory::get_name_vec(std::vector<std::string> & name_vec) {
    name_vec.clear();

    for (auto & [name, node_ptr] : _node_factory_map) {
        name_vec.push_back(name);
    }

    return 0;
}

}
}