#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <random>

#include <chrono>
#include "v1.h"
#include "v11.cpp"
#include "v12.cpp"
#include "v13.cpp"

using HRClock = std::chrono::high_resolution_clock;

#define SOLUT MySolution
#define OPERATION1 2
#define OPERATION2 10
#define OPERATION3 30
#define SECONDS_TEST 10
#define EPOCHS_STEP_TEST 1

std::pair<SOLUT&, std::vector<Student*>&>& init(std::string path, int max_lines = -1) {
    static SOLUT solution;
    static std::vector<Student*> students;
    static std::pair<SOLUT&, std::vector<Student*>&> result(solution, students);

    // Clean up previous allocations
    for (Student* st : students) {
        delete st;
    }
    students.clear();
    solution.clear(); // Reset solution state

    std::ifstream fin(path);
    if (!fin.is_open()) {
        std::cerr << "ERROR: Could not open file " << path << std::endl;
        return result;
    }

    std::string line;
    // Skip header line
    if (!std::getline(fin, line)) {
        fin.close();
        return result;
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
            students.push_back(st);
            ++i;
        } catch (const std::exception& e) {
            std::cerr << "ERROR parsing line [" << line << "]: " << e.what() << std::endl;
        }
    }

    fin.close();
    return result;
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

long long testSeconds(Solution& solution, std::vector<Student*>& students, size_t sec=10, size_t epochsStep=100) {
    size_t n = students.size();

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
    }

    return cnt_operations;
}

std::vector<std::pair<size_t, long long>> runTests(std::string path, size_t max_n=100000, size_t sec=10, size_t epochsStep=100) {
    std::vector<std::pair<size_t, long long>> measures;
    for(size_t i=10; i<=max_n; i*=10) {
        auto [solution, students] = init(path, (int)i);
        long long ops = testSeconds(solution, students, sec, epochsStep);
        measures.push_back({i, ops});
        // Students and solution destroyed together - OK
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

    std::vector<std::pair<size_t, long long>> measures = runTests("../students.csv", 100000, SECONDS_TEST, EPOCHS_STEP_TEST);

    for(auto &[n, op]: measures) {
        std::cout << "n=" << n << "\t" << op << " per " << SECONDS_TEST << " seconds" << std::endl;
    }

    return 0;
}