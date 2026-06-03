#pragma once

#include "Records.hpp"

#include <fstream>
#include <string>
#include <vector>

class StudentManager 
{
public:
    StudentManager(std::string filename, std::string index);

    bool open();
    void addRegister();    
    bool deleteStudent(std::string account);
    Student searchStudent(std::string account);
    bool updateStudent(std::string account);
        
private:
    std::fstream in_out;
    std::string filename_;
    std::string index_;
    std::vector<index> indexes;
};