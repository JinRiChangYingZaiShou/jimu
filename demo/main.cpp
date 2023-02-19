#include "cstdio"
#include "iostream"

#include "../graph/jimu_manager.h"

class BuilderDataTest : public jimu::graph::GraphDataBase {
public:
    BuilderDataTest() : jimu::graph::GraphDataBase() {}
    ~BuilderDataTest() {}

    virtual uint32_t init() override {
        std::cout << "BuilderDataTest init" << std::endl;
        return 0;
    }

    virtual uint32_t reset() override {
        std::cout << "BuilderDataTest reset" << std::endl;
        return 0;
    }

    virtual uint32_t destory() override {
        std::cout << "BuilderDataTest destory" << std::endl;
        return 0;
    }
};

class GraphDataTest : public jimu::graph::GraphDataBase {
public:
    GraphDataTest() : jimu::graph::GraphDataBase() {}
    ~GraphDataTest() {}

    virtual uint32_t init() override {
        std::cout << "GraphDataTest init" << std::endl;
        return 0;
    }

    virtual uint32_t reset() override {
        std::cout << "GraphDataTest reset" << std::endl;
        return 0;
    }

    virtual uint32_t destory() override {
        std::cout << "GraphDataTest destory" << std::endl;
        return 0;
    }
};

class GraphAdapterTest : public
    jimu::graph::GraphDataAdapter<BuilderDataTest, GraphDataTest> {
public:
    GraphAdapterTest() {}
    ~GraphAdapterTest() {}

    virtual uint32_t set_request(std::any request) {
        std::cout << "adapter set request" << std::endl;
        return 0;
    }

    virtual uint32_t get_response(std::any response) {
        std::cout << "adapter get response" << std::endl;
        return 0;
    }
};

class StepFirstNode : jimu::graph::JimuNode<BuilderDataTest, GraphDataTest> {
public:
    StepFirstNode() {}
    ~StepFirstNode() {}

    virtual uint32_t init() override {
        std::cout << "step first node init" << std::endl;
        return 0;
    }

    virtual uint32_t process() override {
        std::cout << "step first node process" << std::endl;
        return 0;
    }

    virtual uint32_t reset() override {
        std::cout << "step first node reset" << std::endl;
        return 0;
    }
};

class StepSecondNode : jimu::graph::JimuNode<BuilderDataTest, GraphDataTest> {
public:
    StepSecondNode() {}
    ~StepSecondNode() {}

    virtual uint32_t init() override {
        std::cout << "step second node init" << std::endl;
        return 0;
    }

    virtual uint32_t process() override {
        std::cout << "step second node process" << std::endl;
        return 0;
    }

    virtual uint32_t reset() override {
        std::cout << "step second node reset" << std::endl;
        return 0;
    }
};

jimu::graph::JimuGraphManager
    <BuilderDataTest, GraphDataTest, GraphAdapterTest> manager_test;

int main() {
    std::cout << "jimu demo test" << std::endl;

    manager_test.get_builder()->register_node<StepFirstNode>("StepFirstNode");

    std::cout << "add StepFirstNode" << std::endl;

    manager_test.get_builder()->register_node<StepSecondNode>("StepSecondNode");

    std::cout << "add StepSecondNode" << std::endl;

    manager_test.get_builder()->add_edge("StepFirstNode", "StepSecondNode");

    std::cout << "add edge" << std::endl;

    manager_test.init(4, 1);

    std::cout << "maneger init done" << std::endl;

    while (true) {
        manager_test.process(nullptr, nullptr);
    }
    std::cout << "manager process done" << std::endl;

    return 0;
}