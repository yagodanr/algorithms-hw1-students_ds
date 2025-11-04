#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "student.h"

std::vector<Student*> readStudents(std::string path) {
    std::vector<Student*> students;


    std::ifstream fin(path);
    if (!fin.is_open()) {
        std::cerr << "ERROR: Could not open file " << path << std::endl;
        return students;
    }

    std::string line;
    // Skip header line
    if (!std::getline(fin, line)) {
        fin.close();
        return students;
    }

    while (std::getline(fin >> std::ws, line)) {
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
            students.push_back(st);
        } catch (const std::exception& e) {
            std::cerr << "ERROR parsing line [" << line << "]: " << e.what() << std::endl;
        }
    }

    fin.close();
    return students;
}


void sortBuiltIn(std::vector<Student*>& students) {
    std::sort(students.begin(), students.end(), [](Student* a, Student* b) {
        return std::make_pair(a->m_surname, a->m_name) < std::make_pair(b->m_surname, b->m_name);
    });
}

void sortRadix(std::vector<Student*>& students) {
    if (students.empty()) return;

    // Find maximum lengths
    size_t max_name = 0, max_surname = 0;
    for (auto* s : students) {
        max_name = std::max(max_name, s->m_name.size());
        max_surname = std::max(max_surname, s->m_surname.size());
    }

    std::vector<Student*> temp(students.size());
    std::vector<size_t> count(256);

    // 1️⃣ Sort by NAME first (less significant)
    for (size_t pos = max_name; pos > 0; --pos) {
        size_t idx = pos - 1;
        std::fill(count.begin(), count.end(), 0);

        // Count
        for (auto* s : students) {
            unsigned char c = (idx < s->m_name.size()) ? static_cast<unsigned char>(s->m_name[idx]) : 0;
            ++count[c];
        }

        // Prefix sum
        for (size_t i = 1; i < 256; ++i)
            count[i] += count[i - 1];

        // Place backwards for stability
        for (auto it = students.rbegin(); it != students.rend(); ++it) {
            unsigned char c = (idx < (*it)->m_name.size()) ? static_cast<unsigned char>((*it)->m_name[idx]) : 0;
            temp[--count[c]] = *it;
        }

        students.swap(temp);
    }

    // 2️⃣ Sort by SURNAME (more significant)
    for (size_t pos = max_surname; pos > 0; --pos) {
        size_t idx = pos - 1;
        std::fill(count.begin(), count.end(), 0);

        for (auto* s : students) {
            unsigned char c = (idx < s->m_surname.size()) ? static_cast<unsigned char>(s->m_surname[idx]) : 0;
            ++count[c];
        }

        for (size_t i = 1; i < 256; ++i)
            count[i] += count[i - 1];

        for (auto it = students.rbegin(); it != students.rend(); ++it) {
            unsigned char c = (idx < (*it)->m_surname.size()) ? static_cast<unsigned char>((*it)->m_surname[idx]) : 0;
            temp[--count[c]] = *it;
        }

        students.swap(temp);
    }
}


void saveToCSV(std::vector<Student*> students, std::string path) {
    std::ofstream fout(path);
    if (!fout.is_open()) {
        std::cerr << "ERROR: Could not open file " << path << std::endl;
        return ;
    }
    fout << "name" << ',';
    fout << "surname" << ',';
    fout << "email" << ',';
    fout << "birth_year" << ',';
    fout << "birth_month" << ',';
    fout << "birth_day" << ',';
    fout << "group" << ',';
    fout << "rating" << ',';
    fout << "phone_number";
    for(auto &st: students) {
        fout << st->m_name << ',';
        fout << st->m_surname << ',';
        fout << st->m_email << ',';
        fout << st->m_birth_year << ',';
        fout << st->m_birth_month << ',';
        fout << st->m_birth_day << ',';
        fout << st->m_group << ',';
        fout << st->m_rating << ',';
        fout << st->m_phone_number;
        fout << '\n';
    }
    fout.close();
}