#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <string>
#include <sstream>

#include "v1.h"

struct Group3 {
    void addStudent(Student* student) {
        if (!student) return; // Safety check

        students[student->m_email] = student;
        auto key = std::make_pair(student->m_name, student->m_surname);

        // Only increment sameNamed if this is the second student with this name
        if (NSHashes[key] >= 2) {
            ++sameNamed;
        }
        NSHashes[key]++;
    }

    void deleteStudent(Student* student) {
        if (!student) return; // Safety check

        students.erase(student->m_email);
        auto key = std::make_pair(student->m_name, student->m_surname);


        // Only decrement sameNamed if this was the last duplicate
        if (NSHashes[key] >= 2) {
            --sameNamed;
            --NSHashes[key];
        } else if (NSHashes[key] == 1) {
            NSHashes.erase(NSHashes.find(key));
        }
    }
    ~Group3() {
        // Note: Group3 doesn't own Student pointers, so we don't delete them here
    }

    std::unordered_map<std::string, Student*> students;
    std::unordered_map<std::pair<std::string, std::string>, int> NSHashes;
    unsigned int sameNamed = 0;
};

class MySolution3: public Solution{
public:
    std::vector<Student*> getStudentsByName(std::string name, std::string surname) override {
        std::vector<Student*> students;
        for(auto &gr: m_groups) {
            for(auto &st: gr.second->students) {
                if(st.second->m_name == name && st.second->m_surname == surname) {
                    students.push_back(st.second);
                }
            }
        }
        return students;
    }

    std::vector<std::string> getGroupsWithEqualNames() override {
        std::vector<std::string> res_groups;
        for(const auto &gr: m_groups) {
            if(gr.second->sameNamed) {
                res_groups.push_back(gr.first);
            }
        }
        return res_groups;
    }

    void changeGroupByEmail(std::string email, std::string new_group) override {
        // Check if email exists in mailMap
        auto it = m_mailMap.find(email);
        if(it == m_mailMap.end()) {
            std::cerr << "Error: Email not found: " << email << std::endl;
            return;
        }

        std::string old_group = it->second;
        Group3* old_gr = m_groups[old_group];
        if(old_gr->students.find(email) == old_gr->students.end()) {
            std::cerr << "Error: Student not found: " << email << std::endl;
            return;

        }
        Student* st = old_gr->students[email];


        // Create new group if it doesn't exist
        if(m_groups.find(new_group) == m_groups.end()) {
            m_groups[new_group] = new Group3();
        }

        old_gr->deleteStudent(st);
        m_groups[new_group]->addStudent(st);

        m_mailMap[email] = new_group;  // Store group name, not pointer
        st->m_group = new_group;
    }

    void addStudent(Student& student) override {
        std::string s_group = student.m_group;

        // Create new group if it doesn't exist
        if(m_groups.find(s_group) == m_groups.end()) {
            m_groups[s_group] = new Group3();
        }

        m_groups[s_group]->addStudent(&student);
        m_mailMap[student.m_email] = s_group;  // Store group name, not pointer
    }
    std::vector<Student*> getStudents() override {
        std::vector<Student*> students;
        for(auto &gr: m_groups) {
            for(auto &st: gr.second->students) {
                students.push_back(st.second);
            }
        }
        return students;
    }



    void clear() override {
        // Clean up dynamically allocated groups
        for(auto &gr: m_groups) {
            delete gr.second;
        }
        m_groups.clear();
        m_mailMap.clear();
    }
    ~MySolution3() {
        clear();
    }

private:
    std::unordered_map<std::string, Group3*> m_groups;
    std::unordered_map<std::string, std::string> m_mailMap;  // Changed: email -> group_name
};