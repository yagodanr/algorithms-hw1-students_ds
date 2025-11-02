#pragma once
#ifndef V1_HEADER
#define V1_HEADER

#include "student.h"
#include <vector>

class Solution {
public:
    virtual Student get_student_by_name(std::string name, std::string surname);
    virtual std::vector<std::string> get_groups_with_equal_names(std::string name, std::string surname);
    virtual void change_group_by_email(std::string email, std::string new_group);
};

#endif