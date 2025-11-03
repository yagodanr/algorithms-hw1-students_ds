#pragma once
#ifndef V1_HEADER
#define V1_HEADER

#include "student.h"
#include <vector>


namespace std {
    // template <>
    // struct hash<std::pair<std::string, std::string>> {
    //     size_t operator()(const std::pair<std::string, std::string>& p) const {
    //         return hash<string>{}(p.first) + hash<string>{}(p.second);
    //     }
    // };
    template <>
    struct hash<std::pair<std::string, std::string>> {
        size_t operator()(const std::pair<std::string, std::string>& p) const {
            return hash<string>{}(p.first+'\n'+p.second);
        }
    };
}

class Solution {
public:
    virtual std::vector<Student*> getStudentsByName(std::string name, std::string surname) = 0;
    virtual std::vector<std::string> getGroupsWithEqualNames() = 0;
    virtual void changeGroupByEmail(std::string email, std::string new_group) = 0;
    virtual void addStudent(Student& student) = 0;
};

#endif