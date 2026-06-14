#pragma once

#include "Records.hpp"
#include <fstream>
#include <optional>
#include <string>
#include <vector>

class StudentManager
{
public:
    StudentManager(std::string filename, std::string index);

    bool open();
    void close();

    bool addRegister(std::string& JSON);
    bool deleteStudent(std::string account);
    std::optional<Student> searchStudent(std::string& account);
    bool updateStudent(std::string& JSON);

private:
    std::string filename_;
    std::string index_;
    std::vector<index> indexes;

    bool loadIndex();
    bool saveIndex();

    std::string serializeStudent(Student& s);
    Student deserializeStudent(std::ifstream& f);
    int findIndexPosition(std::string account);
    bool insertIndexOrdered(index new_index);
};