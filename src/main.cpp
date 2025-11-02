#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <random>

#include <chrono>
#include "v1.h"
#include "v11.cpp"

using HRClock = std::chrono::high_resolution_clock;

#define SOLUT MySolution
#define OPERATION1 2
#define OPERATION2 10
#define OPERATION3 30
#define EPOCHS 100
#define SECONDS_TEST 10
#define EPOCHS_STEP_TEST 100

std::pair<SOLUT, std::vector<Student>> init(std::string path, size_t max_lines=-1) {
    std::ifstream fin;
    fin.open(path);
    if(!fin.is_open()) {
        std::cerr << "ERROR" << std::endl;
    }

    std::vector<std::string> row;
    std::string line, word;

    SOLUT solution;
    std::vector<Student> students;
    fin >> std::ws && std::getline(fin, line);
    for (size_t i=0; i<max_lines && fin >> std::ws && std::getline(fin, line); ++i) {
        row.clear();
        std::stringstream s(line);

        while (std::getline(s, word, ',')) {
            row.push_back(word);
        }

        Student st;
        st.m_name = row[0];
        st.m_surname = row[1];
        st.m_email = row[2];
        st.m_birth_year = std::stoi(row[3]);
        st.m_birth_month = std::stoi(row[4]);
        st.m_birth_day = std::stoi(row[5]);
        st.m_group = row[6];
        st.m_rating = std::stof(row[7]);
        st.m_phone_number = row[8];


        solution.addStudent(st);
        students.push_back(st);
   }

   fin.close();

   return {solution, students};
}


long long test(Solution& solution, std::vector<Student>& students, size_t epochs) {
    long long cnt_operations = 0;
    size_t n = students.size();

    for(; epochs > 0; --epochs, ++cnt_operations) {
        int operation_type = rand() % (OPERATION1+OPERATION2+OPERATION3);

        if(operation_type < OPERATION1) {
            size_t student_ind = rand() % n;
            solution.getStudentByName(students[student_ind].m_name, students[student_ind].m_surname);
        }
        else if(operation_type < OPERATION1 + OPERATION2) {
            solution.getGroupsWithEqualNames();
        }
        else {
            size_t student_ind = rand() % n;
            size_t group_ind = rand() % n;

            solution.changeGroupByEmail(students[student_ind].m_email, students[group_ind].m_group);
        }

    }

    return cnt_operations;
}

long long testSeconds(Solution& solution, std::vector<Student>& students, size_t sec=10, size_t epochsStep=100) {
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

        test(solution, students, epochsStep);
        cnt_operations += epochsStep;
    }

    return cnt_operations;
}


int main() {
    srand(time(0));
    auto [solution, students] = init("../students.csv", EPOCHS);
    solution.changeGroupByEmail("halyna.melnyk39@student.org", "VXJ-26");

    // auto start = HRClock::now();
    // test(solution, students, EPOCHS);
    // auto duration = std::chrono::duration_cast<std::chrono::microseconds>(HRClock::now() - start);
    // std::cout << EPOCHS << " number of requests processed in " << duration << " microseconds" << std::endl;


    std::cout << "Executed " <<  testSeconds(solution, students, SECONDS_TEST, EPOCHS_STEP_TEST) << " operations in " << SECONDS_TEST << " seconds.";

    return 0;
}