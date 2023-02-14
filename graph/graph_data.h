#ifndef JIMU_GRAPH_GRAPH_DATA_H
#define JIMU_GRAPH_GRAPH_DATA_H

#include <memory>

namespace jimu {
namespace graph {

class GraphDataBase {
public:
    virtual uint32_t init() = 0;
    virtual uint32_t reset() = 0;
    virtual uint32_t destory() = 0;
};

using GraphDataBaseSharedPtr = std::shared_ptr<GraphDataBase>;

}
}

#endif