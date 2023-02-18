#ifndef JIMU_TOOLS_COMMON_TOOLS_H
#define JIMU_TOOLS_COMMON_TOOLS_H

#include <map>
#include <vector>
#include <atomic>
#include <memory>
#include <iostream>

namespace jimu {
namespace tools {

template<typename K, typename V, typename KC, typename VC>
uint32_t map_insert(std::map<K, V> & _map, KC && key, VC && value) {
    return !_map.insert(typename std::map<K, V>::value_type(key, value)).second;
}

template<typename T>
class AtomicWrapper {

public:
    std::atomic<T> data;

    AtomicWrapper() : data() {}

    AtomicWrapper(const T & x) : data(x) {}
    AtomicWrapper(const std::atomic<T> & x) : data(x.load()) {}

    AtomicWrapper(const AtomicWrapper<T> &x) : data(x.data.load()) {}

    AtomicWrapper &operator=(const AtomicWrapper<T> &other) {
        data.store(other.data.load());
        return *this;
    }
};

template<typename T>
class Counter {
    std::vector<AtomicWrapper<T>> data = std::vector<AtomicWrapper<T>>();

public:
    void resize(uint32_t size, const T &x) {
        data.resize(size, x);
    }

    void store(uint32_t pos, const T &x) {
        data[pos].data.store(x);
    }

    T load(uint32_t pos) {
        return data[pos].data.load();
    }

    void reset_all(const T& x) {
        for (int i = 0; i < data.size(); i++) {
            data[i].data.store(x);
        }
    }

    std::atomic<T>& at(uint32_t pos) {
        return data[pos].data;
    }

};

template<typename Type, typename BaseType>
std::shared_ptr<BaseType> convert_ptr(Type *base_ptr) {
    return std::shared_ptr<BaseType>((BaseType*)base_ptr);
}

}
}

#endif