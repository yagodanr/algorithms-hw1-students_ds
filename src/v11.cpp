#include <vector>
#include <unordered_map>


#include "v1.h"

struct Group {
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
    std::unordered_map<std::string, Student> students;
    std::unordered_map<std::pair<std::string, std::string>, int> NSHashes;
    unsigned int sameNamed = 0;
};



class MySolution: public Solution{
public:
    std::vector<Student*>& getStudentsByName(std::string name, std::string surname) override {
        m_tempStudentsByName.clear();
        for(auto &gr: m_groups) {
            for(auto &st: gr.second.students) {
                if(st.second.m_name == name && st.second.m_surname == surname) {
                    m_tempStudentsByName.push_back(&st.second);
                }
            }
        }
        return m_tempStudentsByName;
    }

    std::vector<std::string>& getGroupsWithEqualNames() override {
        m_tempGroupsList.clear();
        for(const auto &gr: m_groups) {
            if(gr.second.sameNamed) {
                m_tempGroupsList.push_back(gr.first);
            }
        }
        return m_tempGroupsList;
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

    std::vector<Student*>& getStudents() override {
        m_tempStudentList.clear();
        for(auto &gr: m_groups) {
            for(auto &st: gr.second.students) {
                m_tempStudentList.push_back(&st.second);
            }
        }
        return m_tempStudentList;
    }

    void clear() override {

    }


private:
    std::unordered_map<std::string, Group> m_groups;
    std::unordered_map<std::string, Group> m_mailMap;
    std::vector<Student*> m_tempStudentsByName;
    std::vector<std::string> m_tempGroupsList;
    std::vector<Student*> m_tempStudentList;
};