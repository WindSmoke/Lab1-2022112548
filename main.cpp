#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <regex>
#include <random>
#include <queue>
#include <limits>
#include <iomanip>
#include <algorithm>
#include <map>
#include <cmath>
#include <set>
//NOW i am make some changes
using namespace std;

// 全局图变量
unordered_map<string, unordered_map<string, int>> graph;

// 文本预处理函数
string text_pre_process(const string& text) {
    // 将换行符和回车符替换为空格
    string processed_text = regex_replace(text, regex("[\n\r]"), " ");

    // 使用正则表达式替换所有标点符号为空格，并删除非字母字符
    processed_text = regex_replace(processed_text, regex("[^a-zA-Z\\s]"), " ");

    // 去除多余的空格（例如多个空格被替换为一个空格）
    processed_text = regex_replace(processed_text, regex("\\s+"), " ");

    // 去除首尾空格并将所有字符转换为小写
    processed_text = regex_replace(processed_text, regex("^\\s+|\\s+$"), "");
    transform(processed_text.begin(), processed_text.end(), processed_text.begin(), ::tolower);
    processed_text = regex_replace(processed_text, regex("[^\\w\\s]"), "");

    return processed_text;
}

// 读取文件内容函数
string read_file(const string& file_path) {
    ifstream file(file_path);
    if (file.is_open()) {
        stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        return buffer.str();
    } else {
        return "Error: The file at " + file_path + " was not found.";
    }
}
unordered_map<string, unordered_map<string, int>>& build_directed_graph(const string& text) {
    string processed_text = text_pre_process(text);
    stringstream ss(processed_text);
    string word;
    vector<string> words;
    while (ss >> word) {
        words.push_back(word);
    }

    graph.clear(); // 清空之前的图

    // 遍历单词列表，构建有向图
    for (size_t i = 0; i < words.size() - 1; ++i) {
        string word_a = words[i];
        string word_b = words[i + 1];

        // 确保每个单词都被添加为图的节点
        graph[word_a]; // 如果 word_a 没有出现在图中，它将被初始化为一个空的 unordered_map
        graph[word_b]; // 同上

        // 增加A->B的边权重
        graph[word_a][word_b]++;
    }

    return graph;
}

// 展示有向图 (需要第三方库，此处简化为打印节点和边)
void showDirectedGraph(const unordered_map<string, unordered_map<string, int>>& graph) {
    cout << "Directed Graph:" << endl;
    for (const auto& pair : graph) {
        cout << "Node: " << pair.first << " -> ";
        for (const auto& neighbor_pair : pair.second) {
            cout << neighbor_pair.first << "(" << neighbor_pair.second << ") ";
        }
        cout << endl;
    }
}

// 查询桥接词函数
vector<string> queryBridgeWords(const string& word1, const string& word2) {
    if (graph.find(word1) == graph.end() || graph.find(word2) == graph.end()) {
        cout << "No word1 in the graph!" << endl;
        return {};
    }
    if (graph.find(word1) == graph.end() || graph.find(word2) == graph.end()) {
        cout << " word2 in the graph!" << endl;
        return {};
    }

    vector<string> bridge_words;

    // 遍历图，查找满足条件的桥接词
    for (const auto& pair : graph) {
        const string& word3 = pair.first;
        if (graph.at(word1).count(word3) > 0 && graph.count(word3) > 0 && graph.at(word3).count(word2) > 0) {
            bridge_words.push_back(word3);
        }
    }

    // 根据找到的桥接词数量输出结果
    if (!bridge_words.empty()) {
        cout << "The bridge words from " << word1 << " to " << word2 << " are: ";
        for (size_t i = 0; i < bridge_words.size(); ++i) {
            cout << bridge_words[i] << (i == bridge_words.size() - 1 ? "." : ", ");
        }
        cout << endl;
    } else {
        cout << "No bridge words from " << word1 << " to " << word2 << "!" << endl;
    }

    return bridge_words;
}

// 生成新文本函数
string generateNewText(const string& inputText) {
    stringstream ss(inputText);
    string word;
    vector<string> words;
    while (ss >> word) {
        words.push_back(word);
    }

    vector<string> new_words;
    random_device rd;
    mt19937 gen(rd());

    // 遍历相邻的单词
    for (size_t i = 0; i < words.size() - 1; ++i) {
        string word1 = words[i];
        string word2 = words[i + 1];

        // 查找这对单词的桥接词
        vector<string> bridge_words = queryBridgeWords(word1, word2);

        // 如果存在桥接词，随机选择一个并插入
        if (!bridge_words.empty()) {
            uniform_int_distribution<> distrib(0, bridge_words.size() - 1);
            string bridge_word = bridge_words[distrib(gen)];
            new_words.push_back(word1);
            new_words.push_back(bridge_word);
        } else {
            new_words.push_back(word1);
        }

        // 对于最后一个单词，直接添加到新文本中
        if (i == words.size() - 2) {
            new_words.push_back(word2);
        }
    }

    // 将处理后的单词列表重新拼接成文本
    stringstream result_ss;
    for (const string& w : new_words) {
        result_ss << w << " ";
    }
    string result = result_ss.str();
    if (!result.empty()) {
        result.pop_back(); // Remove trailing space
    }
    return result;
}

// Dijkstra算法函数
pair<vector<string>, int> dijkstra(const string& start, const string& end) {
    unordered_map<string, int> distances;
    unordered_map<string, string> previous_nodes;
    priority_queue<pair<int, string>, vector<pair<int, string>>, greater<pair<int, string>>> priority_queue;

    for (const auto& node_pair : graph) {
        distances[node_pair.first] = numeric_limits<int>::max();
    }
    distances[start] = 0;
    priority_queue.push({0, start});

    while (!priority_queue.empty()) {
        int current_distance = priority_queue.top().first;
        string current_node = priority_queue.top().second;
        priority_queue.pop();

        if (current_distance > distances[current_node]) {
            continue;
        }

        if (graph.count(current_node)) {
            for (const auto& neighbor_pair : graph.at(current_node)) {
                string neighbor = neighbor_pair.first;
                int weight = neighbor_pair.second;
                int distance = current_distance + weight;
                if (distance < distances[neighbor]) {
                    distances[neighbor] = distance;
                    previous_nodes[neighbor] = current_node;
                    priority_queue.push({distance, neighbor});
                }
            }
        }
    }

    if (distances[end] == numeric_limits<int>::max()) {
        return {{}, -1}; // 返回空路径和-1表示不可达
    }

    vector<string> path;
    string current = end;
    while (current != "") {
        path.push_back(current);
        if (previous_nodes.find(current) == previous_nodes.end()) {
            break;
        }
        current = previous_nodes[current];
    }
    reverse(path.begin(), path.end());
    return {path, distances[end]};
}

// 计算最短路径函数
void calcShortestPath(const string& word1, const string& word2) {
    if (graph.find(word1) == graph.end() || graph.find(word2) == graph.end()) {
        cout << "No word1 or word2 in the graph!" << endl;
        return;
    }

    pair<vector<string>, int> result = dijkstra(word1, word2);
    const vector<string>& path = result.first;
    int length = result.second;

    if (length == -1) {
        cout << "There is no path from '" << word1 << "' to '" << word2 << "'!" << endl;
    } else {
        cout << "The shortest path from '" << word1 << "' to '" << word2 << "' is: ";
        for (size_t i = 0; i < path.size(); ++i) {
            cout << path[i] << (i == path.size() - 1 ? "" : " -> ");
        }
        cout << endl;
        cout << "The length of the path is: " << length << endl;
    }
}


// 计算PageRank函数，处理出度为0的节点
double calPageRank(const string& word) {
    double d = 0.85;
    int max_iter = 100;
    double tol = 1e-6;

    unordered_set<string> nodes_set;
    for (const auto& pair : graph) {
        nodes_set.insert(pair.first);
        for (const auto& neighbor_pair : pair.second) {
            nodes_set.insert(neighbor_pair.first);
        }
    }
    vector<string> nodes(nodes_set.begin(), nodes_set.end());
    int N = nodes.size();

    if (N == 0) {
        return 0.0;
    }

    unordered_map<string, double> pr;
    unordered_map<string, int> out_degree;

    for (const string& node : nodes) {
        pr[node] = 1.0 / N;
        out_degree[node] = graph.count(node) ? graph.at(node).size() : 0;
    }

    for (int iter = 0; iter < max_iter; ++iter) {
        unordered_map<string, double> new_pr;
        unordered_map<string, double> dangling_pr_sum; // 存储出度为0的节点的PR值

        // 计算每个节点的入链贡献和悬挂节点的PR值
        for (const string& node : nodes) {
            double inbound_pr_sum = 0;
            for (const string& neighbor : nodes) {
                if (graph.count(neighbor) && graph.at(neighbor).count(node)) {
                    inbound_pr_sum += pr[neighbor] / out_degree[neighbor];
                }
            }
            new_pr[node] = (1 - d) / N + d * inbound_pr_sum;
        }

        double sum_dangling_pr = 0;
        for (const string& node : nodes) {
            if (out_degree[node] == 0) {
                sum_dangling_pr += pr[node];
            }
        }

        // 将悬挂节点的PR值均分给所有节点
        for (const string& node : nodes) {
            new_pr[node] += d * sum_dangling_pr / N;
        }

        bool converged = true;
        for (const string& node : nodes) {
            if (abs(new_pr[node] - pr[node]) > tol) {
                converged = false;
                break;
            }
        }
        pr = new_pr;
        if (converged) {
            break;
        }
    }

    return pr.count(word) ? pr.at(word) : 0.0;
}
#include <ctime>
#include <utility> 

// 随机游走函数
void randomWalk() {
    string output_file = "walk_output.txt";
    set<string> nodes_set;
    for (const auto& pair : graph) {
        nodes_set.insert(pair.first);
    }
    vector<string> nodes(nodes_set.begin(), nodes_set.end());

    if (nodes.empty()) {
        cout << "The graph is empty, cannot perform random walk." << endl;
        return;
    }

    // 使用当前时间作为随机种子
    unsigned seed = static_cast<unsigned>(time(0));  // 获取当前时间的时间戳作为种子
    mt19937 gen(seed);  // 使用当前时间作为随机数生成器的种子
    uniform_int_distribution<> distrib_node(0, nodes.size() - 1);
    string current_node = nodes[distrib_node(gen)];

    vector<string> visited_nodes;
    set<pair<string, string>> visited_edges;

    ofstream f(output_file);
    if (f.is_open()) {
        f << "Random walk traversal:" << endl;
        f << "Starting from node: " << current_node << endl;

        // 限制随机游走的步数，防止死循环
        for (int i = 0; i < 1000000000000000000; ++i) { // 可以根据需要调整步数
            visited_nodes.push_back(current_node);
            f << current_node << " -> ";

            // 检查当前节点是否有出边
            if (!graph.count(current_node) || graph.at(current_node).empty()) {
                f << "\nNo outgoing edges from the current node. Stopping the walk." << endl;
                break;
            }

            // 获取当前节点的所有邻居
            vector<string> neighbors;
            for (const auto& neighbor_pair : graph.at(current_node)) {
                neighbors.push_back(neighbor_pair.first);
            }

            // 随机选择一个邻居节点作为下一个节点
            uniform_int_distribution<> distrib_neighbor(0, neighbors.size() - 1);
            string next_node = neighbors[distrib_neighbor(gen)];
            pair<string, string> edge = {current_node, next_node};

            // 如果这个边已经访问过，停止随机游走
            if (visited_edges.count(edge)) {
                f << "\nFound a duplicate edge. Stopping the walk." << endl;
                break;
            } else {
                visited_edges.insert(edge);
                current_node = next_node; // 移动到下一个节点
            }
        }

        // 记录访问过的节点
        f << "\nVisited nodes: ";
        for (size_t i = 0; i < visited_nodes.size(); ++i) {
            f << visited_nodes[i] << (i == visited_nodes.size() - 1 ? "" : " -> ");
        }
        f << endl;

        // 记录访问过的边
        f << "Visited edges: ";
        int count = 0;
        for (const auto& edge : visited_edges) {
            f << "(" << edge.first << ", " << edge.second << ")" << (++count == visited_edges.size() ? "" : ", ");
        }
        f << endl;

        cout << "Random walk completed. Traversal result has been written to " << output_file << endl;
        f.close();
    } else {
        cout << "Unable to open file: " << output_file << endl;
    }
}






// 计算 TF (Term Frequency)
unordered_map<string, double> calculateTF(const string& text) {
    unordered_map<string, int> term_counts;
    stringstream ss(text_pre_process(text));
    string word;
    int total_words = 0;
    while (ss >> word) {
        term_counts[word]++;
        total_words++;
    }
    unordered_map<string, double> tf_scores;
    if (total_words > 0) {
        for (const auto& pair : term_counts) {
            tf_scores[pair.first] = static_cast<double>(pair.second) / total_words;
        }
    }
    return tf_scores;
}

// 计算 IDF (Inverse Document Frequency)
unordered_map<string, double> calculateIDF(const vector<string>& documents) {
    unordered_map<string, int> doc_counts;
    int num_documents = documents.size();
    unordered_set<string> unique_words_in_doc;

    for (const string& doc : documents) {
        unique_words_in_doc.clear();
        stringstream ss(text_pre_process(doc));
        string word;
        while (ss >> word) {
            unique_words_in_doc.insert(word);
        }
        for (const string& w : unique_words_in_doc) {
            doc_counts[w]++;
        }
    }

    unordered_map<string, double> idf_scores;
    if (num_documents > 0) {
        for (const auto& pair : doc_counts) {
            idf_scores[pair.first] = log(static_cast<double>(num_documents) / (1 + pair.second));
        }
    }
    return idf_scores;
}

// 计算 TF-IDF 权重
unordered_map<string, double> calculateTFIDF(const string& document, const vector<string>& all_documents) {
    unordered_map<string, double> tf_scores = calculateTF(document);
    unordered_map<string, double> idf_scores = calculateIDF(all_documents);
    unordered_map<string, double> tfidf_scores;
    for (const auto& pair : tf_scores) {
        if (idf_scores.count(pair.first)) {
            tfidf_scores[pair.first] = pair.second * idf_scores[pair.first];
        } else {
            tfidf_scores[pair.first] = 0.0; // If a term is not in any other document
        }
    }
    return tfidf_scores;
}

// 计算PageRank函数，为重要单词分配更高的初始 PR 值 (使用 TF-IDF)
double calPageRankWithInitialWeighting(const string& target_word, const vector<string>& all_documents) {
    double d = 0.85;
    int max_iter = 100;
    double tol = 1e-6;

    unordered_set<string> nodes_set;
    for (const auto& pair : graph) {
        nodes_set.insert(pair.first);
        for (const auto& neighbor_pair : pair.second) {
            nodes_set.insert(neighbor_pair.first);
        }
    }
    vector<string> nodes(nodes_set.begin(), nodes_set.end());
    int N = nodes.size();

    if (N == 0) {
        return 0.0;
    }

    // 计算所有节点（单词）的 TF-IDF 权重
    unordered_map<string, double> initial_weights;
    for (const string& node : nodes) {
        initial_weights[node] = 0.0;
    }

    for (const string& doc : all_documents) {
        unordered_map<string, double> tfidf = calculateTFIDF(doc, all_documents);
        for (const auto& pair : tfidf) {
            if (initial_weights.count(pair.first)) {
                initial_weights[pair.first] = max(initial_weights[pair.first], pair.second); // 取文档中出现的最大 TF-IDF 值作为初始权重
            }
        }
    }

    // 初始化 PR 值，使用 TF-IDF 作为初始权重
    unordered_map<string, double> pr;
    unordered_map<string, int> out_degree;
    double sum_weights = 0.0;
    for (const string& node : nodes) {
        sum_weights += initial_weights[node];
    }

    for (const string& node : nodes) {
        pr[node] = (sum_weights > 0) ? initial_weights[node] / sum_weights : 1.0 / N;
        out_degree[node] = graph.count(node) ? graph.at(node).size() : 0;
    }

    for (int iter = 0; iter < max_iter; ++iter) {
        unordered_map<string, double> new_pr;
        double sum_dangling_pr = 0;

        for (const string& node : nodes) {
            double inbound_pr_sum = 0;
            for (const string& neighbor : nodes) {
                if (graph.count(neighbor) && graph.at(neighbor).count(node)) {
                    inbound_pr_sum += pr[neighbor] / out_degree[neighbor];
                }
            }
            new_pr[node] = (1 - d) / N + d * inbound_pr_sum;
            if (out_degree[node] == 0) {
                sum_dangling_pr += pr[node];
            }
        }

        for (const string& node : nodes) {
            new_pr[node] += d * sum_dangling_pr / N;
        }

        bool converged = true;
        for (const string& node : nodes) {
            if (abs(new_pr[node] - pr[node]) > tol) {
                converged = false;
                break;
            }
        }
        pr = new_pr;
        if (converged) {
            break;
        }
    }

    return pr.count(target_word) ? pr.at(target_word) : 0.0;
}


// Dijkstra算法变体，计算从起始单词到所有其他单词的最短路径
unordered_map<string, pair<int, string>> dijkstraFromWord(const string& start_word) {
    unordered_map<string, int> distances;
    unordered_map<string, string> previous_nodes;
    priority_queue<pair<int, string>, vector<pair<int, string>>, greater<pair<int, string>>> pq;

    // 初始化所有节点的距离为无穷大，起始节点的距离为 0
    unordered_set<string> all_nodes;
    for (const auto& entry : graph) {
        all_nodes.insert(entry.first);
        for (const auto& neighbor : entry.second) {
            all_nodes.insert(neighbor.first);
        }
    }
    for (const string& node : all_nodes) {
        distances[node] = numeric_limits<int>::max();
        previous_nodes[node] = "";
    }
    distances[start_word] = 0;
    pq.push({0, start_word});

    while (!pq.empty()) {
        int current_distance = pq.top().first;
        string current_word = pq.top().second;
        pq.pop();

        if (current_distance > distances[current_word]) {
            continue;
        }

        if (graph.count(current_word)) {
            for (const auto& neighbor_pair : graph.at(current_word)) {
                string neighbor_word = neighbor_pair.first;
                int weight = neighbor_pair.second;
                int distance = current_distance + weight;
                if (distance < distances[neighbor_word]) {
                    distances[neighbor_word] = distance;
                    previous_nodes[neighbor_word] = current_word;
                    pq.push({distance, neighbor_word});
                }
            }
        }
    }

    unordered_map<string, pair<int, string>> results;
    for (const auto& node : all_nodes) {
        results[node] = {distances[node], previous_nodes[node]};
    }
    return results;
}

// 根据前驱节点信息重建最短路径
vector<string> reconstructPath(const unordered_map<string, string>& previous_nodes, const string& target_word) {
    vector<string> path;
    string current = target_word;
    while (!current.empty()) {
        path.push_back(current);
        current = previous_nodes.at(current);
    }
    reverse(path.begin(), path.end());
    return path;
}

// 计算并展示从给定单词到图中其他所有单词的最短路径
void showShortestPathsFromWord(const string& start_word) {
    unordered_map<string, pair<int, string>> dijkstra_results = dijkstraFromWord(start_word);
    unordered_map<string, int> distances;
    unordered_map<string, string> previous_nodes;
    unordered_set<string> all_nodes;

    for (const auto& result_pair : dijkstra_results) {
        all_nodes.insert(result_pair.first);
        distances[result_pair.first] = result_pair.second.first;
        previous_nodes[result_pair.first] = result_pair.second.second;
    }

    cout << "Shortest paths from '" << start_word << "':" << endl;
    for (const string& target_word : all_nodes) {
        if (target_word == start_word) {
            continue;
        }

        if (distances[target_word] == numeric_limits<int>::max()) {
            cout << "To '" << target_word << "': No path exists." << endl;
        } else {
            vector<string> path = reconstructPath(previous_nodes, target_word);
            cout << "To '" << target_word << "' (Distance: " << distances[target_word] << "): ";
            for (size_t i = 0; i < path.size(); ++i) {
                cout << path[i] << (i == path.size() - 1 ? "" : " -> ");
            }
            cout << endl;
        }
    }
}

// 可选的默认 DOT 文件路径
const std::string defaultDotPath = "text_graph.dot";
// 可选的默认图像文件路径
const std::string defaultImagePath = "text_graph.png";

bool exportGraphToDOT(const std::string& dotFilePath = "") {
    std::string filePath = dotFilePath.empty() ? defaultDotPath : dotFilePath;
    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Unable to open DOT file: " << filePath << std::endl;
        return false;
    }

    file << "digraph TextGraph {" << std::endl;
    file << "  node [shape=ellipse, style=filled, fillcolor=lightblue];" << std::endl;
    file << "  edge [color=gray];" << std::endl;

    // Write vertices (unique words in the graph)
    std::unordered_set<std::string> vertices;
    for (const auto& entry : graph) {
        vertices.insert(entry.first);
        for (const auto& neighbor : entry.second) {
            vertices.insert(neighbor.first);
        }
    }
    for (const auto& vertex : vertices) {
        file << "  \"" << vertex << "\";" << std::endl;
    }

    // Write edges
    for (const auto& source_node : graph) {
        const std::string& source = source_node.first;
        for (const auto& target_node : source_node.second) {
            const std::string& target = target_node.first;
            int weight = target_node.second;
            file << "  \"" << source << "\" -> \"" << target
                 << "\" [label=\"" << weight << "\", weight=" << weight << "];" << std::endl;
        }
    }

    file << "}" << std::endl;
    file.close();

    std::cout << "Graph exported to DOT file: " << filePath << std::endl;
    return true;
}

bool generateGraphImage(const std::string& dotFilePath = "", const std::string& imageFilePath = "") {
    std::string dotPath = dotFilePath.empty() ? defaultDotPath : dotFilePath;
    std::string imgPath = imageFilePath.empty() ? defaultImagePath : imageFilePath;

    // Run Graphviz command to convert .dot to image
    std::string command = "dot -Tpng \"" + dotPath + "\" -o \"" + imgPath + "\"";

    std::cout << "Executing command: " << command << std::endl;
    int result = std::system(command.c_str());

    if (result == 0) {
        std::cout << "Graph image saved at: " << imgPath << std::endl;
        return true;
    } else {
        std::cerr << "Failed to generate graph image. Make sure Graphviz is installed and in the PATH." << std::endl;
        return false;
    }
}

// 组合导出 DOT 文件和生成图像的函数
void visualizeGraph(const std::string& dotFilePath = "", const std::string& imageFilePath = "") {
    if (exportGraphToDOT(dotFilePath)) {
        generateGraphImage(dotFilePath, imageFilePath);
    }
}
void extal_part(string text){
        cout << "\nSelect an option from the menu:" << endl;
        cout << "1. Display the directed graph" << endl;    
        cout << "2. Find all the shortest path for one word" << endl;
        cout << "3. Calculate PageRank for a word with TF-IDF" << endl;

        string choice;
        cout << "Enter the option number: ";
        getline(cin, choice);

        if (choice == "1") {
            visualizeGraph();
        }   else if (choice == "2") {
            string word1;
            cout << "Enter the  word: ";
            getline(cin, word1);
            showShortestPathsFromWord(word1);
           
        } else if (choice == "3") {
            std::vector<std::string> all_documents;
            all_documents.push_back(text);
            string word;
            cout << "Enter the word for PageRank calculation: ";
            getline(cin, word);
            cout << "PageRank value of '" << word << "': " << fixed << setprecision(6) << calPageRankWithInitialWeighting(word, all_documents) << endl;
        }  else {
            cout << "Invalid option. Please try again." << endl;
        }

}


int main(int argc, char* argv[]) {
    string filepath = "1.txt";
    if (argc > 1) {
        filepath = argv[1];
    }

    string text = read_file(filepath);
    build_directed_graph(text);

    while (true) {
        
        cout << "\nSelect an option from the menu:" << endl;
        cout << "1. Display the directed graph" << endl;
        cout << "2. Find the bridge words between two nodes" << endl;
        cout << "3. Generate new text based on bridge words" << endl;
        cout << "4. Find the shortest path between two nodes" << endl;
        cout << "5. Calculate PageRank for a word" << endl;
        cout << "6. Perform a random walk" << endl;
        cout << "7. Extal Part" << endl;
        cout << "8. Exit" << endl;

        string choice;
        cout << "Enter the option number: ";
        getline(cin, choice);

        if (choice == "1") {
            showDirectedGraph(graph);
        } else if (choice == "2") {
            string word1, word2;
            cout << "Enter the first word: ";
            getline(cin, word1);
            cout << "Enter the second word: ";
            getline(cin, word2);
            queryBridgeWords(word1, word2);
        } else if (choice == "3") {
            string new_text;
            cout << "Enter the text: ";
            getline(cin, new_text);
            cout << "Generated text: " << generateNewText(new_text) << endl;
        } else if (choice == "4") {
            string word1, word2;
            cout << "Enter the first word: ";
            getline(cin, word1);
            cout << "Enter the second word: ";
            getline(cin, word2);
            calcShortestPath(word1, word2);
        } else if (choice == "5") {
            string word;
            cout << "Enter the word for PageRank calculation: ";
            getline(cin, word);
            cout << "PageRank value of '" << word << "': " << fixed << setprecision(6) << calPageRank(word) << endl;
        } else if (choice == "6") {
            randomWalk();
        } else if(choice == "7") {

            extal_part(text);
            
        }else if(choice == "8") {
            cout << "Exiting the program." << endl;
            break;
        } else {
            cout << "Invalid option. Please try again." << endl;
        }
    }

    return 0;
}