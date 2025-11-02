#pragma once
#ifndef V1_HEADER
#define V1_HEADER

#include "student.h"
#include <vector>

class Solution {
public:
    virtual Student getStudentByName(std::string name, std::string surname) = 0;
    virtual std::vector<std::string> getGroupsWithEqualNames() = 0;
    virtual void changeGroupByEmail(std::string email, std::string new_group) = 0;
    virtual void addStudent(Student& student) = 0;
};

#endif