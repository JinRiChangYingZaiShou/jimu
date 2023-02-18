#ifndef JIMU_GRAPH_GRAPH_DATA_H
#define JIMU_GRAPH_GRAPH_DATA_H

#include <any>
#include <memory>

namespace jimu {
namespace graph {

class GraphDataBase {
public:
    virtual uint32_t init() = 0;
    virtual uint32_t reset() = 0;
    virtual uint32_t destory() = 0;
};

template<typename BuilderDataType, typename GraphDataType>
class GraphDataContainer{

public:
    using BuilderDataPtrType = std::shared_ptr<BuilderDataType>;
    using GraphDataPtrType = std::shared_ptr<GraphDataType>;

private:
    BuilderDataPtrType _builder_data;
    GraphDataPtrType _graph_data;

public:

    GraphDataContainer() :
        _builder_data(nullptr),
        _graph_data(nullptr) {}

    ~GraphDataContainer() {}

    BuilderDataPtrType builder_data() {
        return _builder_data;
    }

    uint32_t set_builder_data(BuilderDataPtrType data) {
        _builder_data = data;
        return 0;
    }

    GraphDataPtrType graph_data() {
        return _graph_data;
    }

    uint32_t set_graph_data(GraphDataPtrType data) {
        _graph_data = data;
        return 0;
    }

};

template<typename BuilderDataType, typename GraphDataType>
class GraphDataAdapter {
public:
    using DataContainerType =
        std::shared_ptr<GraphDataContainer<BuilderDataType, GraphDataType>>;

private:
    DataContainerType data_container;
    
public:
    GraphDataAdapter() :
        data_container(nullptr) { }
    
    ~GraphDataAdapter() {}

    uint32_t set_data_container(DataContainerType container) {
        data_container = container;
        return 0;
    }

    DataContainerType get_data_container() {
        return data_container;
    }

    virtual uint32_t set_request(std::any request) = 0;
    virtual uint32_t get_response(std::any request) = 0;
};

}
}

#endif