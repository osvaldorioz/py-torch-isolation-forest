#include <torch/torch.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>
#include <random>

namespace py = pybind11;

// Estructura para un nodo de un árbol de Isolation Forest
struct IsolationTree {
    int feature;
    double split_value;
    IsolationTree* left;
    IsolationTree* right;

    IsolationTree() : feature(-1), split_value(0), left(nullptr), right(nullptr) {}
};

// Generador de números aleatorios
std::mt19937 rng(std::random_device{}());

// Construcción de un árbol de aislamiento
IsolationTree* build_tree(const std::vector<std::vector<double>>& data, int depth, int max_depth) {
    if (data.empty() || depth >= max_depth) return nullptr;

    int num_features = data[0].size();
    std::uniform_int_distribution<int> feature_dist(0, num_features - 1);
    int feature = feature_dist(rng);

    double min_value = data[0][feature], max_value = data[0][feature];
    for (const auto& row : data) {
        min_value = std::min(min_value, row[feature]);
        max_value = std::max(max_value, row[feature]);
    }

    if (min_value == max_value) return nullptr;

    std::uniform_real_distribution<double> split_dist(min_value, max_value);
    double split_value = split_dist(rng);

    std::vector<std::vector<double>> left_data, right_data;
    for (const auto& row : data) {
        if (row[feature] < split_value) left_data.push_back(row);
        else right_data.push_back(row);
    }

    IsolationTree* node = new IsolationTree();
    node->feature = feature;
    node->split_value = split_value;
    node->left = build_tree(left_data, depth + 1, max_depth);
    node->right = build_tree(right_data, depth + 1, max_depth);

    return node;
}

// Estima la profundidad esperada de un punto en el árbol
double path_length(IsolationTree* node, const std::vector<double>& point, int depth = 0) {
    if (!node || (!node->left && !node->right)) return depth;
    if (point[node->feature] < node->split_value) return path_length(node->left, point, depth + 1);
    return path_length(node->right, point, depth + 1);
}

// Clase Isolation Forest
class IsolationForest {
public:
    int num_trees;
    int max_depth;
    std::vector<IsolationTree*> trees;

    IsolationForest(int num_trees, int max_depth) : num_trees(num_trees), max_depth(max_depth) {}

    void fit(const std::vector<std::vector<double>>& data) {
        for (int i = 0; i < num_trees; i++) {
            trees.push_back(build_tree(data, 0, max_depth));
        }
    }

    std::vector<double> predict(const std::vector<std::vector<double>>& data) {
        std::vector<double> scores(data.size(), 0.0);
        for (size_t i = 0; i < data.size(); i++) {
            double total_depth = 0.0;
            for (auto tree : trees) {
                total_depth += path_length(tree, data[i]);
            }
            scores[i] = total_depth / num_trees; // Promedio de profundidad
        }
        return scores;
    }
};

// Enlazar con Pybind11
PYBIND11_MODULE(isolation_forest, m) {
    py::class_<IsolationForest>(m, "IsolationForest")
        .def(py::init<int, int>())
        .def("fit", &IsolationForest::fit)
        .def("predict", &IsolationForest::predict);
}
