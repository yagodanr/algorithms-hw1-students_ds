#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include "v1.h"
#include "v11.cpp"

#define SOLUT MySolution


SOLUT init(std::string path) {
    std::ifstream fin;
    fin.open(path);
    if(!fin.is_open()) {
        std::cerr << "ERROR" << std::endl;
    }

    std::vector<std::string> row;
    std::string line, word;

    SOLUT solution;
    fin >> std::ws && std::getline(fin, line);
    while (fin >> std::ws && std::getline(fin, line)) {
        row.clear();
        std::stringstream s(line);

        while (std::getline(s, word, ',')) {
            row.push_back(word);
        }

        Student st;
        st.m_name = row[0];
        st.m_surname = row[1];
        st.m_email = row[2];
        st.m_birth_year = std::stoi(row[3]);
        st.m_birth_month = std::stoi(row[4]);
        st.m_birth_day = std::stoi(row[5]);
        st.m_group = row[6];
        st.m_rating = std::stof(row[7]);
        st.m_phone_number = row[8];


        solution.addStudent(st);

   }

   fin.close();

   return solution;
}


int main() {
    SOLUT solution = init("../students.csv");
    solution.changeGroupByEmail("halyna.melnyk39@student.org", "VXJ-26");

    return 0;
}