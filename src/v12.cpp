#include <iostream>
#include <vector>
#include <map>
#include <set>

#include "v1.h"

struct Group2 {
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
    ~Group2() {
        // Note: Group2 doesn't own Student pointers, so we don't delete them here
    }

    std::map<std::string, Student*> students;
    std::map<std::pair<std::string, std::string>, int> NSHashes;
    unsigned int sameNamed = 0;
};

class MySolution2: public Solution{
public:
    std::vector<Student*> getStudentsByName(std::string name, std::string surname) override {
        return m_students[{name, surname}];
    }

    std::vector<std::string> getGroupsWithEqualNames() override {
        return {m_duplicateGroups.begin(), m_duplicateGroups.end()};
    }

    void changeGroupByEmail(std::string email, std::string new_group) override {
        // Check if email exists in mailMap
        auto it = m_mailMap.find(email);
        if(it == m_mailMap.end()) {
            std::cerr << "Error: Email not found: " << email << std::endl;
            return;
        }

        std::string old_group = it->second;
        Group2* old_gr = m_groups[old_group];
        if(old_gr->students.find(email) == old_gr->students.end()) {
            std::cerr << "Error: Student not found: " << email << std::endl;
            return;

        }
        Student* st = old_gr->students[email];


        // Create new group if it doesn't exist
        if(m_groups.find(new_group) == m_groups.end()) {
            m_groups[new_group] = new Group2();
        }

        old_gr->deleteStudent(st);
        if(!old_gr->sameNamed) {
            m_duplicateGroups.erase(old_group);
        }

        m_mailMap[email] = new_group;
        st->m_group = new_group;
        addStudent(*st);
    }

    void addStudent(Student& student) override {
        std::string s_group = student.m_group;

        // Create new group if it doesn't exist
        if(m_groups.find(s_group) == m_groups.end()) {
            m_groups[s_group] = new Group2();
        }

        m_groups[s_group]->addStudent(&student);
        if(m_groups[s_group]->sameNamed) {
            m_duplicateGroups.insert(s_group);
        }

        m_mailMap[student.m_email] = s_group;
        m_students[{student.m_name, student.m_surname}].push_back(&student);
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
    ~MySolution2() {
        clear();
    }

private:
    std::map<std::string, Group2*> m_groups;
    std::map<std::string, std::string> m_mailMap;  // Changed: email -> group_name
    std::set<std::string> m_duplicateGroups;
    std::map<std::pair<std::string, std::string>, std::vector<Student*>> m_students;
};