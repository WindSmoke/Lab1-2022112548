#include "gtest/gtest.h"
#include "graph_analyzer.h" // 包含你的函数定义

// 你需要在你的 main.cpp 中定义 graph 变量，或者在这里直接定义（如果你的所有代码都在一起）
// 为了测试独立性，通常会在测试前 setup 图

// 假设你的 graph 变量是全局的，并且在 build_directed_graph 中被修改
// 在每个测试开始前，我们需要清除并重建图，以确保测试的独立性。
// 可以通过在测试夹具（Test Fixture）中实现 SetUp 方法来完成。

// 定义一个测试夹具
class QueryBridgeWordsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 在每个测试用例运行前执行
        // 清空全局图变量，然后构建一个简单的图用于测试
        graph.clear();
        std::string test_text = "new york new jersey new city";
        // 注意：build_directed_graph 返回一个引用，但我们这里只是调用它来填充全局的 graph 变量
        build_directed_graph(test_text);
    }

    void TearDown() override {
        // 在每个测试用例运行后执行（可选，这里不需要特别清理）
    }
};

// Test Case 1: Word1 not in graph
TEST_F(QueryBridgeWordsTest, Word1NotInGraph) {
    std::string word1 = "london"; // 不在图中的词
    std::string word2 = "york";   // 在图中的词
    std::vector<std::string> bridge_words = queryBridgeWords(word1, word2);
    ASSERT_TRUE(bridge_words.empty()); // 期望结果为空
}

// Test Case 2: Word2 not in graph
TEST_F(QueryBridgeWordsTest, Word2NotInGraph) {
    std::string word1 = "new";      // 在图中的词
    std::string word2 = "paris";    // 不在图中的词
    std::vector<std::string> bridge_words = queryBridgeWords(word1, word2);
    ASSERT_TRUE(bridge_words.empty()); // 期望结果为空
}

// Test Case 3: Both words in graph, but no bridge words
TEST_F(QueryBridgeWordsTest, NoBridgeWordsExist) {
    // 文本 "new york new jersey new city" 构建的图
    // new -> york (1)
    // york -> new (1)
    // new -> jersey (1)
    // jersey -> new (1)
    // new -> city (1)

    std::string word1 = "city";
    std::string word2 = "jersey";
    // 从 york 到 jersey 没有桥接词 (york -> X -> jersey)
    std::vector<std::string> bridge_words = queryBridgeWords(word1, word2);
    ASSERT_TRUE(bridge_words.empty()); // 期望结果为空
}

// Test Case 4: Single bridge word exists
TEST_F(QueryBridgeWordsTest, SingleBridgeWord) {
    // 文本 "new york new jersey new city"
    // new -> york, york -> new
    // new -> jersey, jersey -> new
    // new -> city

    std::string word1 = "new";
    std::string word2 = "new"; // 查找 "new -> X -> new"
    std::vector<std::string> bridge_words = queryBridgeWords(word1, word2);
    // 期望的桥接词是 "york" 和 "jersey"
    // 注意：queryBridgeWords 函数的输出顺序不确定，所以需要检查内容
    ASSERT_EQ(bridge_words.size(), 2);
    ASSERT_NE(std::find(bridge_words.begin(), bridge_words.end(), "york"), bridge_words.end());
    ASSERT_NE(std::find(bridge_words.begin(), bridge_words.end(), "jersey"), bridge_words.end());
}

// Test Case 5: Multiple bridge words exist (using a different text for clarity)
class QueryBridgeWordsMultipleTest : public ::testing::Test {
protected:
    void SetUp() override {
        graph.clear();
        std::string test_text = "life is good good life is beautiful";
        // 构建的图 (部分):
        // life -> is
        // is -> good
        // good -> good
        // good -> life
        // life -> is
        // is -> beautiful
        build_directed_graph(test_text);
    }
};

TEST_F(QueryBridgeWordsMultipleTest, MultipleBridgeWords) {
    std::string word1 = "is";
    std::string word2 = "life";
    // 期望的桥接词：is -> good -> life
    std::vector<std::string> bridge_words = queryBridgeWords(word1, word2);
    ASSERT_EQ(bridge_words.size(), 1);
    ASSERT_EQ(bridge_words[0], "good");
}

// Main function for running tests
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}