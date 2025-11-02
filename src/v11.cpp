#include "v1.h"
#include <vector>
#include <unordered_map>

class Group {
public:
private:
    std::vector<Student> m_students;
    std::vector<int> m_NSHashes;

};



class MySolution: public virtual Solution{
    Student get_student_by_name(std::string name, std::string surname);

    std::vector<std::string> get_groups_with_equal_names(std::string name, std::string surname);

    void change_group_by_email(std::string email, std::string new_group);



private:
    std::vector<Group> m_groups;
    std::unordered_map<std::string, Group> m_mailMap;
};