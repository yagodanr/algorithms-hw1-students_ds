#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>

#include <sys/resource.h>
#include <iomanip>
#include <map>
#include <memory>
#include <functional>

#include <chrono>

#include "v1.h"
#include "v11.cpp"
#include "v12.cpp"
#include "v13.cpp"
#include "v14.cpp"

using HRClock = std::chrono::high_resolution_clock;

#define SOLUT MySolution4
#define OPERATION1 2
#define OPERATION2 10
#define OPERATION3 30
#define SECONDS_TEST 10
#define EPOCHS_STEP_TEST 1


struct BenchmarkResult {
    std::string solution_name;
    size_t n;
    long long operations;
    size_t peak_memory_kb;
    size_t final_memory_kb;
};

// Get current memory usage in KB
size_t getCurrentMemoryKB() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize / 1024;
    }
    return 0;
#else
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    #ifdef __APPLE__
        return usage.ru_maxrss / 1024;
    #else
        return usage.ru_maxrss;
    #endif
#endif
}




void init(std::string path, Solution& solution, int max_lines = -1) {
    solution.clear(); // Reset solution state

    std::ifstream fin(path);
    if (!fin.is_open()) {
        std::cerr << "ERROR: Could not open file " << path << std::endl;
        return ;
    }

    std::string line;
    // Skip header line
    if (!std::getline(fin, line)) {
        fin.close();
        return ;
    }

    int i = 0;
    while ((max_lines < 0 || i < max_lines) && std::getline(fin >> std::ws, line)) {
        std::vector<std::string> row;
        std::stringstream s(line);
        std::string word;

        while (std::getline(s, word, ',')) {
            row.push_back(word);
        }

        // Validate row has enough columns
        if (row.size() < 9) {
            std::cerr << "WARNING: Skipping invalid line (columns=" << row.size() << "): " << line << std::endl;
            continue;
        }

        try {
            Student* st = new Student{
                row[0],                     // m_name
                row[1],                     // m_surname
                row[2],                     // m_email
                std::stoi(row[3]),          // m_birth_year
                std::stoi(row[4]),          // m_birth_month
                std::stoi(row[5]),          // m_birth_day
                row[6],                     // m_group
                std::stof(row[7]),          // m_rating
                row[8]                      // m_phone_number
            };

            solution.addStudent(*st);
            ++i;
        } catch (const std::exception& e) {
            std::cerr << "ERROR parsing line [" << line << "]: " << e.what() << std::endl;
        }
    }

    fin.close();
}

long long test(Solution& solution, std::vector<Student*>& students, size_t epochs) {
    long long cnt_operations = 0;
    size_t n = students.size();
    if(n == 0) {
        return 0;
    }

    for(; epochs > 0; --epochs, ++cnt_operations) {
        int operation_type = rand() % (OPERATION1+OPERATION2+OPERATION3);

        if(operation_type < OPERATION1) {
            size_t student_ind = rand() % n;
            auto x = solution.getStudentsByName(students[student_ind]->m_name, students[student_ind]->m_surname);
            if(x.empty()) {
                std::cerr << "Empty result of get students" << std::endl;
                return cnt_operations;
            }
        }
        else if(operation_type < OPERATION1 + OPERATION2) {
            auto x = solution.getGroupsWithEqualNames();
            if(x.empty()) {
                // std::cerr << "Empty result of get groups" << std::endl;
                // return cnt_operations;

            }
        }
        else {
            size_t student_ind = rand() % n;
            size_t group_ind = rand() % n;

            solution.changeGroupByEmail(students[student_ind]->m_email, students[group_ind]->m_group);
        }

    }

    return cnt_operations;
}

long long testSeconds(Solution& solution, std::vector<Student*>& students,
                      size_t sec=10, size_t epochsStep=100,
                      size_t* peak_memory_out=nullptr) {
    size_t peak_memory = 0;
    auto start = HRClock::now();
    std::chrono::seconds timestep(sec);
    long long cnt_operations = 0;

    while(true) {
        auto delta_time = HRClock::now() - start;
        auto timer = std::chrono::duration_cast<std::chrono::seconds>(delta_time);

        if(timer >= timestep) {
            break;
        }

        cnt_operations += test(solution, students, epochsStep);

        size_t current_memory = getCurrentMemoryKB();
        if(current_memory > peak_memory) {
            peak_memory = current_memory;
        }
    }

    if(peak_memory_out) {
        *peak_memory_out = peak_memory;
    }

    return cnt_operations;
}

std::vector<BenchmarkResult> runTestsForSolution(
    const std::string& solution_name,
    std::function<Solution*()> solution_factory,
    std::string path,
    size_t max_n=100000,
    size_t sec=10,
    size_t epochsStep=100) {

    std::vector<BenchmarkResult> measures;

    for(size_t i=100; i<=max_n; i*=10) {
        size_t baseline_memory = getCurrentMemoryKB();

        // Create solution instance
        std::unique_ptr<Solution> solution(solution_factory());

        // Initialize solution with data
        init(path, *solution, (int)i);

        // Get students from solution (assuming Solution has a method to get students)
        // You might need to adjust this based on your actual Solution interface
        std::vector<Student*> students = solution->getStudents();

        size_t peak_memory = 0;
        long long ops = testSeconds(*solution, students, sec, epochsStep, &peak_memory);

        size_t final_memory = getCurrentMemoryKB();

        measures.push_back({
            solution_name,
            i,
            ops,
            peak_memory > baseline_memory ? peak_memory - baseline_memory : peak_memory,
            final_memory > baseline_memory ? final_memory - baseline_memory : final_memory
        });

        // solution and students will be cleaned up automatically
    }

    return measures;
}

std::vector<BenchmarkResult> runAllTests(std::string path,
                                         size_t max_n=100000,
                                         size_t sec=10,
                                         size_t epochsStep=100) {
    std::vector<BenchmarkResult> all_results;

    // Test MySolution
    std::cout << "Running MySolution..." << std::endl;
    auto results1 = runTestsForSolution(
        "MySolution",
        []() -> Solution* { return new MySolution(); },
        path, max_n, sec, epochsStep);
    all_results.insert(all_results.end(), results1.begin(), results1.end());

    // Test MySolution2
    std::cout << "Running MySolution2..." << std::endl;
    auto results2 = runTestsForSolution(
        "MySolution2",
        []() -> Solution* { return new MySolution2(); },
        path, max_n, sec, epochsStep);
    all_results.insert(all_results.end(), results2.begin(), results2.end());

    // Test MySolution3
    std::cout << "Running MySolution3..." << std::endl;
    auto results3 = runTestsForSolution(
        "MySolution3",
        []() -> Solution* { return new MySolution3(); },
        path, max_n, sec, epochsStep);
    all_results.insert(all_results.end(), results3.begin(), results3.end());

    // Test MySolution4
    std::cout << "Running MySolution4..." << std::endl;
    auto results4 = runTestsForSolution(
        "MySolution4",
        []() -> Solution* { return new MySolution4(); },
        path, max_n, sec, epochsStep);
    all_results.insert(all_results.end(), results4.begin(), results4.end());

    return all_results;
}

void printBenchmarkResults(const std::vector<BenchmarkResult>& results) {
    std::cout << "\n=== Benchmark Results ===\n\n";
    std::cout << std::left
              << std::setw(15) << "Solution"
              << std::setw(10) << "n"
              << std::setw(15) << "Operations"
              << std::setw(18) << "Peak Mem (KB)"
              << std::setw(18) << "Final Mem (KB)" << "\n";
    std::cout << std::string(76, '-') << "\n";

    for(const auto& result : results) {
        std::cout << std::left
                  << std::setw(15) << result.solution_name
                  << std::setw(10) << result.n
                  << std::setw(15) << result.operations
                  << std::setw(18) << result.peak_memory_kb
                  << std::setw(18) << result.final_memory_kb << "\n";
    }
}

void printComparisonTable(const std::vector<BenchmarkResult>& results) {
    std::map<size_t, std::map<std::string, BenchmarkResult>> organized;

    for(const auto& result : results) {
        organized[result.n][result.solution_name] = result;
    }

    std::cout << "\n=== Performance Comparison by Input Size ===\n";

    for(const auto& [n, solutions] : organized) {
        std::cout << "\nn = " << n << ":\n";
        std::cout << std::string(70, '-') << "\n";

        for(const auto& [name, result] : solutions) {
            std::cout << std::left << std::setw(15) << name
                      << " | Ops: " << std::setw(10) << result.operations
                      << " | Peak: " << std::setw(8) << result.peak_memory_kb << " KB"
                      << " | Final: " << std::setw(8) << result.final_memory_kb << " KB\n";
        }
    }

    // Find best performer for each metric
    std::cout << "\n=== Best Performers ===\n";
    for(const auto& [n, solutions] : organized) {
        // Best by operations (throughput)
        auto best_ops = std::max_element(solutions.begin(), solutions.end(),
            [](const auto& a, const auto& b) {
                return a.second.operations < b.second.operations;
            });

        // Best by memory efficiency (lowest peak memory)
        auto best_mem = std::min_element(solutions.begin(), solutions.end(),
            [](const auto& a, const auto& b) {
                return a.second.peak_memory_kb < b.second.peak_memory_kb;
            });

        std::cout << "n = " << n << ":\n";
        std::cout << "  Fastest: " << best_ops->first
                  << " (" << best_ops->second.operations << " ops)\n";
        std::cout << "  Most Memory Efficient: " << best_mem->first
                  << " (" << best_mem->second.peak_memory_kb << " KB)\n";
    }

    // Calculate speedup ratios
    std::cout << "\n=== Speedup Ratios (relative to MySolution) ===\n";
    for(const auto& [n, solutions] : organized) {
        if(solutions.find("MySolution") == solutions.end()) continue;

        long long baseline_ops = solutions.at("MySolution").operations;
        std::cout << "n = " << n << ":\n";

        for(const auto& [name, result] : solutions) {
            if(name == "MySolution") continue;
            double speedup = static_cast<double>(result.operations) / baseline_ops;
            std::cout << "  " << name << ": " << std::fixed << std::setprecision(2)
                      << speedup << "x";
            if(speedup > 1.0) std::cout << " (faster)";
            else if(speedup < 1.0) std::cout << " (slower)";
            std::cout << "\n";
        }
    }
}

// Alternative: If init returns students separately
std::vector<BenchmarkResult> runTestsForSolutionAlt(
    const std::string& solution_name,
    std::function<Solution*()> solution_factory,
    std::function<std::vector<Student*>(std::string, Solution&, int)> init_func,
    std::string path,
    size_t max_n=100000,
    size_t sec=10,
    size_t epochsStep=100) {

    std::vector<BenchmarkResult> measures;

    for(size_t i=100; i<=max_n; i*=10) {
        size_t baseline_memory = getCurrentMemoryKB();

        std::unique_ptr<Solution> solution(solution_factory());
        std::vector<Student*> students = init_func(path, *solution, (int)i);

        size_t peak_memory = 0;
        long long ops = testSeconds(*solution, students, sec, epochsStep, &peak_memory);

        size_t final_memory = getCurrentMemoryKB();

        measures.push_back({
            solution_name,
            i,
            ops,
            peak_memory > baseline_memory ? peak_memory - baseline_memory : peak_memory,
            final_memory > baseline_memory ? final_memory - baseline_memory : final_memory
        });
    }

    return measures;
}

// TODO
Student* getStByEmail(std::vector<Student*>& students, std::string email) {
    for(auto &st: students) {
        if(st->m_email == email) {
            return st;
        }
    }
    return nullptr;
}

int main() {
    srand(time(0));

    // MySolution s;
    // std::vector<Student*> students;

    // Student* st = new Student{
    //             "n",                     // m_name
    //             "s",                     // m_surname
    //             "1@students.org",                     // m_email
    //             2006,          // m_birth_year
    //             1,           // m_birth_month
    //             1,          // m_birth_day
    //             "PKN-24",                     // m_group
    //             1.0f,          // m_rating
    //             "+2929"                      // m_phone_number
    //         };
    // s.addStudent(*st);
    // students.push_back(st);

    // st = new Student{
    //             "n",                     // m_name
    //             "s",                     // m_surname
    //             "2@students.org",                     // m_email
    //             2006,          // m_birth_year
    //             1,           // m_birth_month
    //             1,          // m_birth_day
    //             "PKN-24",                     // m_group
    //             1.0f,          // m_rating
    //             "+2929"                      // m_phone_number
    //         };
    // s.addStudent(*st);
    // students.push_back(st);

    // st = new Student{
    //             "n",                     // m_name
    //             "s",                     // m_surname
    //             "3@students.org",                     // m_email
    //             2006,          // m_birth_year
    //             1,           // m_birth_month
    //             1,          // m_birth_day
    //             "PKN-24",                     // m_group
    //             1.0f,          // m_rating
    //             "+2929"                      // m_phone_number
    //         };
    // s.addStudent(*st);
    // students.push_back(st);
    // test(s, students, 10000);
    // auto [solution, students] = init("../students.csv");
    // auto s = getStByEmail(students, "halyna.melnyk39@student.org");
    // solution.changeGroupByEmail("halyna.melnyk39@student.org", "VXJ-26");

    // auto start = HRClock::now();
    // test(solution, students, EPOCHS);
    // auto duration = std::chrono::duration_cast<std::chrono::microseconds>(HRClock::now() - start);
    // std::cout << EPOCHS << " number of requests processed in " << duration << " microseconds" << std::endl;

    // std::cout << "Executed " <<  testSeconds(solution, students, SECONDS_TEST, EPOCHS_STEP_TEST) << " operations in " << SECONDS_TEST << " seconds.";

    std::vector<BenchmarkResult> measures = runAllTests("../students.csv", 100000, SECONDS_TEST, EPOCHS_STEP_TEST);
    printBenchmarkResults(measures);

    return 0;
}