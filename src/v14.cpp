#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <string>
#include <sstream>

#include "v1.h"

class MySolution4: public Solution{
public:
    std::vector<Student*> getStudentsByName(std::string name, std::string surname) override {
        std::vector<Student*> students;
        for(auto &st: m_students) {
            if(st->m_name == name && st->m_surname == surname) {
                students.push_back(st);
            }
        }
        return students;
    }

    std::vector<std::string> getGroupsWithEqualNames() override {
        std::unordered_map<std::string, std::unordered_set<std::pair<std::string, std::string>>> groups;
        std::vector<std::string> filteredGroups;
        for(const auto &st: m_students) {
            std::pair<std::string, std::string> nameSur{st->m_name, st->m_surname};
            if(groups[st->m_group].find(nameSur) != groups[st->m_group].end()) {
                filteredGroups.push_back(st->m_group);
            }
            groups[st->m_group].insert(nameSur);
        }

        return filteredGroups;
    }

    void changeGroupByEmail(std::string email, std::string new_group) override {
        Student* st;
        for(const auto& x: m_students) {
            if(x->m_email == email) {
                st = x;
                x->m_group = new_group;
                return ;
            }
        }
    }

    void addStudent(Student& student) override {
        m_students.push_back(&student);
    }

    void clear() override {
        m_students.clear();
    }

private:
    std::vector<Student*> m_students;
};