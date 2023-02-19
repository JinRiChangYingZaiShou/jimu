#include "cstdio"
#include "iostream"

#include "../graph/jimu_manager.h"

#include "../third-party/httplib/httplib.h"

class BuilderDataTest : public jimu::graph::GraphDataBase {
public:

    std::atomic<uint32_t> count;

    BuilderDataTest() : jimu::graph::GraphDataBase() {}
    ~BuilderDataTest() {}

    virtual uint32_t init() override {
        std::cout << "BuilderDataTest init" << std::endl;
        count.store(0);
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
        return 0;
    }

    virtual uint32_t process() override {
        std::vector<uint32_t> vec;
        for (int i = 100; i > 0; i--) {
            vec.push_back(i);
        }
        std::sort(vec.begin(), vec.end());
        return 0;
    }

    virtual uint32_t reset() override {
        return 0;
    }
};

class StepSecondNode : jimu::graph::JimuNode<BuilderDataTest, GraphDataTest> {
public:
    StepSecondNode() {}
    ~StepSecondNode() {}

    virtual uint32_t init() override {
        return 0;
    }

    virtual uint32_t process() override {
        std::cout << "StepSecondNode run" << std::endl;
        std::cout << "request times:" << builder_data()->count.fetch_add(1) + 1 << std::endl;
        return 0;
    }

    virtual uint32_t reset() override {
        return 0;
    }
};

jimu::graph::JimuGraphManager
    <BuilderDataTest, GraphDataTest, GraphAdapterTest> manager_test;

void init() {
    std::cout << "jimu demo test" << std::endl;

    manager_test.get_builder()->register_node<StepFirstNode>("StepFirstNode");
    std::cout << "add StepFirstNode" << std::endl;

    manager_test.get_builder()->register_node<StepSecondNode>("StepSecondNode");
    std::cout << "add StepSecondNode" << std::endl;

    manager_test.get_builder()->add_edge("StepFirstNode", "StepSecondNode");
    std::cout << "add edge" << std::endl;

    manager_test.init(4, 1);
    std::cout << "maneger init done" << std::endl;
}

int main() {

    init();

    httplib::Server svr;

    svr.Get("/test", [](const httplib::Request &req, httplib::Response &res) {
        std::cout << "get request" << std::endl;
        manager_test.process(req, res);

        res.set_content("Hello World!", "text/plain");
    });

    svr.listen("127.0.0.1", 8080);

    return 0;
}