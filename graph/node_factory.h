#ifndef JIMU_GRAPH_NODE_FACTORY_H
#define JIMU_GRAPH_NODE_FACTORY_H

#include <map>
#include <vector>
#include <memory>

#include "jimu_node.h"

namespace jimu {
namespace graph {

using JimuNodeSharedPtr = std::shared_ptr<JimuNode>;

class NodeBuilderBase {
public:
    virtual JimuNodeSharedPtr build_node() = 0;
};

template<typename T>
class NodeBuilder : NodeBuilderBase {
public:
    virtual JimuNodeSharedPtr build_node() override {
        return std::make_shared<JimuNode>(new T());
    }
};

class NodeFactory {

    using NodeBuilderBaseSharedPtr = std::shared_ptr<NodeBuilderBase>;

    std::map<std::string, NodeBuilderBaseSharedPtr> _node_factory_map;

public:
    NodeFactory();
    ~NodeFactory();

    template<typename T>
    uint32_t register_node(std::string node_name) {
        if (_node_factory_map.count(node_name)) {
            return 1;
        }

        _node_factory_map.insert(node_name,
            std::make_shared<NodeBuilderBase>(new NodeBuilderBase<T>());

        return 0;
    }

    uint32_t build_node_map(std::map<std::string,
        std::shared_ptr<JimuNode>> &node_map);
    
    uint32_t get_name_vec(std::vector<std::string> & name_vec);
};

}
}

#endif