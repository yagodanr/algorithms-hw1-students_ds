#pragma once
#ifndef V3_HEADER
#define V3_HEADER

#include "v1.h"

class MySolution3 : public Solution {
private:
    std::vector<Student*> data;

public:
    std::vector<Student*> getStudentsByName(std::string name, std::string surname) override {
        std::vector<Student*> out;
        for (Student* s : data) {
            if (s->m_name == name && s->m_surname == surname) {
                out.push_back(s);
            }
        }
        return out;
    }

    std::vector<std::string> getGroupsWithEqualNames() override {
        std::vector<std::string> groups;

        for (Student* s : data) {
            const std::string& g = s->m_group;

            bool already = false;
            for (auto& x : groups) {
                if (x == g) {
                    already = true;
                    break;
                }
            }
            if (already) {
                continue;
            }

            bool found = false;
            for (Student* a : data) {
                if (a->m_group != g) {
                    continue;
                }
                for (Student* b : data) {
                    if (b == a) {
                        continue;
                    }
                    if (b->m_group != g) {
                        continue;
                    }
                    if (a->m_name == b->m_name && a->m_surname == b->m_surname) {
                        found = true;
                        break;
                    }
                }
                if (found) {
                    break;
                }
            }
            if (found) {
                groups.push_back(g);
            }
        }

        return groups;
    }

    void changeGroupByEmail(std::string email, std::string new_group) override {
        for (Student* s : data) {
            if (s->m_email == email) {
                s->m_group = new_group;
                return;
            }
        }
    }

    void addStudent(Student& student) override {
        data.push_back(&student);
    }
};

#endif