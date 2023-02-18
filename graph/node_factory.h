#ifndef JIMU_GRAPH_NODE_FACTORY_H
#define JIMU_GRAPH_NODE_FACTORY_H

#include <map>
#include <vector>
#include <memory>

#include "jimu_node.h"
#include "../tools/common_tools.h"

namespace jimu {
namespace graph {

template<typename GraphDataJimuNodeType>
class NodeBuilderBase {
public:
    virtual std::shared_ptr<GraphDataJimuNodeType> build_node() = 0;
};

template<typename NodeType, typename GraphDataJimuNodeType>
class NodeBuilder : public NodeBuilderBase<GraphDataJimuNodeType> {
public:
    NodeBuilder() {}
    ~NodeBuilder() {}

    virtual std::shared_ptr<GraphDataJimuNodeType>
        build_node() override {
        return jimu::tools::convert_ptr<NodeType,
            GraphDataJimuNodeType>(new NodeType());
    }
};

template<typename BuilderDataType, typename GraphDataType>
class NodeFactory {

    using JimuNodeBaseType =
        JimuNode<BuilderDataType, GraphDataType>;
    using NodeBuilderBaseSharedPtr =
        std::shared_ptr<NodeBuilderBase<JimuNodeBaseType>>;
    
    using NodeBuilderBaseType =
        NodeBuilderBase<JimuNodeBaseType>;

    std::map<std::string, NodeBuilderBaseSharedPtr> _node_factory_map;

public:
    NodeFactory() : _node_factory_map() {}
    ~NodeFactory() {}

    template<typename T>
    uint32_t register_node(std::string node_name) {
        if (_node_factory_map.count(node_name)) {
            return 1;
        }

        jimu::tools::map_insert(_node_factory_map, node_name,
            jimu::tools::convert_ptr<
            NodeBuilder<T, JimuNodeBaseType>,
            NodeBuilderBaseType>(
                new NodeBuilder<T, JimuNodeBaseType>()));

        return 0;
    }

    uint32_t build_node_map(std::map<std::string,
        std::shared_ptr<
        JimuNode<BuilderDataType, GraphDataType>>> &node_map) {
        for (auto & [name, node_ptr] : _node_factory_map) {
            jimu::tools::map_insert(node_map, name, node_ptr->build_node());
        }

        return 0;
    }
    
    uint32_t get_name_vec(std::vector<std::string> & name_vec) {
        name_vec.clear();

        for (auto & [name, node_ptr] : _node_factory_map) {
            name_vec.push_back(name);
        }

        return 0;
    }
};

}
}

#endif