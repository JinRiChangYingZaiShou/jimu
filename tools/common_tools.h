#ifndef JIMU_TOOLS_COMMON_TOOLS_H
#define JIMU_TOOLS_COMMON_TOOLS_H

#include <map>
#include <memory>
#include <iostream>

namespace jimu {
namespace tools {

template<typename K, typename V, typename KC, typename VC>
uint32_t map_insert(std::map<K, V> & _map, KC && key, VC && value) {
    return _map.insert(std::map<K, V>::value_type(key, value));
}

}
}

#endif