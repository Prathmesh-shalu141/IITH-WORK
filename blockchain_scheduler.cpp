#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <algorithm>
#include <random>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread> // Added for std::this_thread::sleep_for

struct Transaction {
    int id;
    std::unordered_set<int> readSet;
    std::unordered_set<int> writeSet;
    int gasFee;
};

struct ColorInfo {
    int color;
    int gasFee;
    int numTxs;
    int timeNeeded;
    std::vector<int> txIDs;
};

std::vector<std::vector<int>> buildConflictGraph(
    const std::vector<Transaction>& txs,
    double conflictPercentage
) {
    int n = txs.size();
    std::vector<std::vector<int>> graph(n);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 100);

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            bool conflict = false;

            for (int r : txs[i].readSet) {
                if (txs[j].writeSet.count(r)) conflict = true;
            }
            for (int r : txs[j].readSet) {
                if (txs[i].writeSet.count(r)) conflict = true;
            }
            for (int w : txs[i].writeSet) {
                if (txs[j].writeSet.count(w)) conflict = true;
            }

            if (conflict) {
                double randValue = dis(gen);
                if (randValue <= conflictPercentage) {
                    graph[i].push_back(j);
                    graph[j].push_back(i);
                }
            }
        }
    }

    return graph;
}

std::vector<int> greedyColoring(const std::vector<std::vector<int>>& graph) {
    int n = graph.size();
    std::vector<int> result(n, -1);
    std::vector<bool> used(n, false);

    for (int u = 0; u < n; ++u) {
        std::fill(used.begin(), used.end(), false);
        for (int v : graph[u]) {
            if (result[v] != -1) {
                used[result[v]] = true;
            }
        }
        for (int c = 0; c < n; ++c) {
            if (!used[c]) {
                result[u] = c;
                break;
            }
        }
    }

    return result;
}

int maximizeGasFeeWithThreads(
    const std::vector<Transaction>& txs,
    const std::vector<int>& colors,
    int remainingTime,
    int threads,
    std::vector<int>& selectedTransactions,
    std::unordered_map<int, std::vector<int>>& colorGroups,
    std::unordered_map<int, int>& colorGasFee
) {
    std::unordered_map<int, std::vector<int>> colorTxs;
    std::unordered_map<int, int> colorGas;

    for (int i = 0; i < txs.size(); ++i) {
        int color = colors[i];
        colorTxs[color].push_back(i);
        colorGas[color] += txs[i].gasFee;
    }

    std::vector<ColorInfo> colorInfos;

    for (auto& [color, txIDs] : colorTxs) {
        int numTxs = txIDs.size();
        int timeNeeded = std::ceil((double)numTxs / threads);
        int gas = colorGas[color];

        colorInfos.push_back({color, gas, numTxs, timeNeeded, txIDs});
    }

    std::sort(colorInfos.begin(), colorInfos.end(), [](const ColorInfo& a, const ColorInfo& b) {
        return a.gasFee > b.gasFee;
    });

    int totalFee = 0;
    int timeUsed = 0;

    for (const auto& group : colorInfos) {
        if (timeUsed + group.timeNeeded <= remainingTime) {
            timeUsed += group.timeNeeded;
            totalFee += group.gasFee;
            selectedTransactions.insert(selectedTransactions.end(), group.txIDs.begin(), group.txIDs.end());

            colorGroups[group.color] = group.txIDs;
            colorGasFee[group.color] = group.gasFee;
        }
    }

    return totalFee;
}

int main() {
    std::ifstream file("input.txt");
    if (!file.is_open()) {
        std::cerr << "Error opening file\n";
        return 1;
    }

    int threads, numTransactions, totalTime;
    double conflictPercentage;
    file >> threads >> conflictPercentage >> totalTime >> numTransactions;

    std::vector<Transaction> txs(numTransactions);

    for (int i = 0; i < numTransactions; ++i) {
        file >> txs[i].id;
        int readSetSize, writeSetSize;
        file >> readSetSize >> writeSetSize >> txs[i].gasFee;

        for (int j = 0; j < readSetSize; ++j) {
            int readElem;
            file >> readElem;
            txs[i].readSet.insert(readElem);
        }

        for (int j = 0; j < writeSetSize; ++j) {
            int writeElem;
            file >> writeElem;
            txs[i].writeSet.insert(writeElem);
        }
    }

    auto startGraphTime = std::chrono::high_resolution_clock::now();

    auto conflictGraph = buildConflictGraph(txs, conflictPercentage);
    auto colors = greedyColoring(conflictGraph);

    auto endGraphTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> graphBuildDuration = endGraphTime - startGraphTime;
    int graphBuildTime = std::ceil(graphBuildDuration.count());

    int remainingTime = totalTime - graphBuildTime;

    std::vector<int> selected;
    std::unordered_map<int, std::vector<int>> colorGroups;
    std::unordered_map<int, int> colorGasFee;

    int maxFee = maximizeGasFeeWithThreads(txs, colors, remainingTime, threads, selected, colorGroups, colorGasFee);

    // --- Simulate executing selected transactions with 0.1 second sleep ---
    std::cout << "Executing selected transactions...\n";
    for (int txID : selected) {
        std::this_thread::sleep_for(std::chrono::milliseconds(300)); // Sleep 0.1 seconds
        std::cout << "Executed transaction ID: " << txs[txID].id << "\n";
    }
    // -----------------------------------------------------------------------

    std::ofstream out("result.txt");
    out << "--- Result ---\n";
    out << "Total gas fee collected: " << maxFee << "\n";
    out << "Graph building time: " << graphBuildTime << " seconds\n";
    out << "Remaining time for execution: " << remainingTime << " seconds\n\n";

    std::unordered_map<int, std::vector<int>> allColorGroups;
    for (int i = 0; i < colors.size(); ++i) {
        allColorGroups[colors[i]].push_back(i);
    }

    out << "Transactions executed in parallel by color group:\n";
    for (const auto& group : allColorGroups) {
        int color = group.first;
        out << "c" << color << " = ";
        for (int txID : group.second) {
            out << "t" << txs[txID].id << " ";
        }

        int gasFee = 0;
        for (int txID : group.second) {
            gasFee += txs[txID].gasFee;
        }

        bool executed = colorGroups.find(color) != colorGroups.end();
        out << "| Total gas fee: " << gasFee << (executed ? " [executed]\n" : " [not executed]\n");
    }

    std::cout << "Output written to result.txt\n";
    file.close();
    return 0;
} 