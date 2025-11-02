#include "v1.h"
#include <vector>
#include <unordered_map>

namespace std {
    template <>
    struct hash<std::pair<std::string, std::string>> {
        size_t operator()(const std::pair<std::string, std::string>& p) const {
            return hash<string>{}(p.first) + hash<string>{}(p.second);
        }
    };
}

struct Group {
    void addStudent(Student& student) {
        students[student.m_email] = student;
        NSHashes.push_back(h({student.m_name, student.m_surname}));
    }

    void deleteStudent(Student& student) {
        students.erase(student.m_email);
        size_t h_st = h({student.m_name, student.m_surname});
        for(int i=0, sz=NSHashes.size(); i<sz; ++i) {
            if(NSHashes[i] == h_st) {
                NSHashes.erase(NSHashes.begin() + i);
                return;
            }
        }

    }
    std::hash<std::pair<std::string, std::string>> h;
    std::unordered_map<std::string, Student> students;
    std::vector<size_t> NSHashes;
};



class MySolution: public virtual Solution{
public:
    Student getStudentByName(std::string name, std::string surname) override {
        return *new Student();
    }

    std::vector<std::string> getGroupsWithEqualNames() override {
        return std::vector<std::string>();
    }

    void changeGroupByEmail(std::string email, std::string new_group) override {
        Group& gr = m_mailMap[email];
        Student& st = gr.students[email];

        m_groups[new_group].addStudent(st);
        gr.deleteStudent(st);
    }

    void addStudent(Student& student) override {
        std::string s_group = student.m_group;
        m_groups[s_group].addStudent(student);
        m_mailMap[student.m_email] = m_groups[s_group];
    }



private:
    std::unordered_map<std::string, Group> m_groups;
    std::unordered_map<std::string, Group> m_mailMap;
};