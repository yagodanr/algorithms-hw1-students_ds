#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "v1.h"

class MySolution4: public Solution{
public:
    std::vector<Student*>& getStudentsByName(std::string name, std::string surname) override {
        m_tempStudentsByName.clear();
        for(auto &st: m_students) {
            if(st->m_name == name && st->m_surname == surname) {
                m_tempStudentsByName.push_back(st);
            }
        }
        return m_tempStudentsByName;
    }

    std::vector<std::string>& getGroupsWithEqualNames() override {
        m_tempGroupsList.clear();
        std::unordered_map<std::string, std::unordered_set<std::pair<std::string, std::string>>> groups;

        for(const auto &st: m_students) {
            std::pair<std::string, std::string> nameSur{st->m_name, st->m_surname};
            if(groups[st->m_group].find(nameSur) != groups[st->m_group].end()) {
                m_tempGroupsList.push_back(st->m_group);
            }
            groups[st->m_group].insert(nameSur);
        }

        return m_tempGroupsList;
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
    std::vector<Student*>& getStudents() override {
        m_tempStudentList.clear();
        for(auto &st: m_students) {
            m_tempStudentList.push_back(st);
        }
        return m_tempStudentList;
    }

    void clear() override {
        m_students.clear();
    }

private:
    std::vector<Student*> m_students;
    std::vector<Student*> m_tempStudentsByName;
    std::vector<std::string> m_tempGroupsList;
    std::vector<Student*> m_tempStudentList;
};