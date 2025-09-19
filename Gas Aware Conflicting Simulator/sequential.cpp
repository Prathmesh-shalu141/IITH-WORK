#include <iostream>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <fstream>
#include <thread>       // for sleep
#include <chrono>       // for time tracking
#include <cmath>        // for ceil

struct Transaction {
    int id;
    std::unordered_set<int> readSet;
    std::unordered_set<int> writeSet;
    int gasFee;
};

// Comparator to sort by gas fee descending
bool compareByGasFee(const Transaction& a, const Transaction& b) {
    return a.gasFee > b.gasFee;
}

int main() {
    std::ifstream file("input.txt");
    if (!file.is_open()) {
        std::cerr << "Error opening input file\n";
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

    file.close();

    // Sort transactions by gas fee (highest first)
    std::sort(txs.begin(), txs.end(), compareByGasFee);

    // Simulate sequential execution using actual time tracking
    int totalGasFee = 0;
    int transactionsExecuted = 0;
    double sleepDuration = 0.3; // seconds

    std::ofstream out("sequential_result.txt");
    out << "--- Sequential Execution Result ---\n";

    auto start = std::chrono::steady_clock::now();

    for (const auto& tx : txs) {
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = now - start;

        if (elapsed.count() + sleepDuration > totalTime) {
            break;  // Not enough time left for another transaction
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        totalGasFee += tx.gasFee;
        ++transactionsExecuted;
        out << "Executed transaction ID: t" << tx.id << " | Gas Fee: " << tx.gasFee << "\n";
    }

    double totalTimeUsed = transactionsExecuted * sleepDuration;

    out << "\nTotal transactions executed: " << transactionsExecuted << "\n";
    out << "Total gas fee collected: " << totalGasFee << "\n";
    out << "Total time consumed: " << totalTimeUsed << " seconds\n";
    out << "Time limit provided: " << totalTime << " seconds\n";

    out.close();

    std::cout << "Sequential execution completed. Output written to sequential_result.txt\n";

    return 0;
}