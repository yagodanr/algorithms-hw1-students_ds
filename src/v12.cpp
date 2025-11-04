#include "v1.h"
#include <vector>
#include <map>


struct Group2 {
    void addStudent(Student& student) {
        students[student.m_email] = student;
        if(NSHashes[{student.m_name, student.m_surname}]++) {
            ++sameNamed;
        }
    }

    void deleteStudent(Student& student) {
        students.erase(student.m_email);
        if(--NSHashes[{student.m_name, student.m_surname}] >= 1) {
            --sameNamed;
        }
    }
    std::map<std::string, Student> students;
    std::map<std::pair<std::string, std::string>, int> NSHashes;
    unsigned int sameNamed = 0;
};



class MySolution2: public Solution{
public:
    std::vector<Student*> getStudentsByName(std::string name, std::string surname) override {
        std::vector<Student*> students;
        for(auto &gr: m_groups) {
            for(auto &st: gr.second.students) {
                if(st.second.m_name == name && st.second.m_surname == surname) {
                    students.push_back(&st.second);
                }
            }
        }
        return students;
    }

    std::vector<std::string> getGroupsWithEqualNames() override {
        std::vector<std::string> res_groups;
        for(const auto &gr: m_groups) {
            if(gr.second.sameNamed) {
                res_groups.push_back(gr.first);
            }
        }
        return res_groups;
    }

    void changeGroupByEmail(std::string email, std::string new_group) override {
        Group2& gr = m_mailMap[email];
        Student& st = gr.students[email];

        m_groups[new_group].addStudent(st);
        gr.deleteStudent(st);
    }

    void addStudent(Student& student) override {
        std::string s_group = student.m_group;
        m_groups[s_group].addStudent(student);
        m_mailMap[student.m_email] = m_groups[s_group];
    }

    void clear() override {

    }

private:
    std::map<std::string, Group2> m_groups;
    std::map<std::string, Group2> m_mailMap;
};
