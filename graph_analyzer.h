// graph_analyzer.h
#ifndef GRAPH_ANALYZER_H
#define GRAPH_ANALYZER_H

#include <string>
#include <unordered_map>
#include <vector>

// 声明全局图变量
extern std::unordered_map<std::string, std::unordered_map<std::string, int>> graph;

// 声明需要的函数
std::string text_pre_process(const std::string& text);
std::string read_file(const std::string& file_path);
std::unordered_map<std::string, std::unordered_map<std::string, int>>& build_directed_graph(const std::string& text);
std::vector<std::string> queryBridgeWords(const std::string& word1, const std::string& word2);

#endif // GRAPH_ANALYZER_H