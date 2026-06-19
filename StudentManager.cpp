#include "StudentManager.hpp"
#include <cstring>
#include <iostream>
#include "CRC.h"

StudentManager::StudentManager(std::string filename, std::string index)
{
    filename_ = filename;
    index_ = index;
}

bool StudentManager::open()
{
    std::fstream f(filename_,std::ios::binary|std::ios::in|std::ios::out);
    if(!f.is_open()){
        f.open(filename_,std::ios::binary | std::ios::out);
        if(!f.is_open())return false;
        f.close();
        f.open(filename_,std::ios::binary|std::ios::out|std::ios::in);
    }

    if(!f.is_open())return false;
    return loadIndex();
}

void StudentManager::close()
{
    saveIndex();
}   

bool StudentManager::addRegister(std::string& JSON)
{
    std::ifstream json(JSON);
    if (!json.is_open()) return false;

    nl::json j;
    json >> j;

    std::vector<Student> students;

    if (j.is_array())
    {
        for (int i = 0; i < j.size(); i++)
        {
            students.push_back(j[i].get<Student>());
        }
    }
    else
    {
        students.push_back(j.get<Student>());
    }

    if (students.empty()) return false;

    int page_data_size = PAGE_SIZE - sizeof(pageHeader);

    for (int i = 0; i < students.size(); i++)
    {
        std::string account(students[i].account, sizeof(students[i].account));

        if (findIndexPosition(account) != -1) return false;

        std::string record = serializeStudent(students[i]);
        if (record.size() > page_data_size) return false;

        for (int j = i + 1; j < students.size(); j++)
        {
            int cmp = memcmp(students[i].account,
                             students[j].account,
                             sizeof(students[i].account));

            if (cmp == 0) return false;
        }
    }

    std::fstream f(filename_, std::ios::binary | std::ios::in | std::ios::out);
    if (!f.is_open()) return false;

    f.seekg(0, std::ios::end);
    long file_size = static_cast<long>(f.tellg());

    if (file_size < 0) return false;
    if (file_size % PAGE_SIZE != 0) return false;

    char page[PAGE_SIZE] = {};
    pageHeader header = {};
    long page_start = 0;

    if (file_size == 0)
    {
        page_start = 0;
    }
    else
    {
        page_start = file_size - PAGE_SIZE;

        f.seekg(page_start, std::ios::beg);
        f.read(page, PAGE_SIZE);

        if (!f) return false;

        memcpy(&header, page, sizeof(header));

        if (header.record_count < 0 || header.used_bytes < 0 || header.used_bytes > page_data_size)
        {
            return false;
        }

        uint32_t current_crc = CRC::Calculate(page + sizeof(pageHeader),
                                              header.used_bytes,
                                              CRC::CRC_32());

        if (current_crc != header.crc) return false;
    }

    std::vector<index> new_indexes;

    for (int i = 0; i < students.size(); i++)
    {
        std::string record = serializeStudent(students[i]);
        int record_size = static_cast<int>(record.size());

        if (header.used_bytes + record_size > page_data_size)
        {
            memset(page + sizeof(pageHeader) + header.used_bytes,
                   0,
                   page_data_size - header.used_bytes);

            header.crc = CRC::Calculate(page + sizeof(pageHeader),
                                        header.used_bytes,
                                        CRC::CRC_32());

            memcpy(page, &header, sizeof(header));

            f.clear();
            f.seekp(page_start, std::ios::beg);
            f.write(page, PAGE_SIZE);

            if (!f) return false;

            page_start += PAGE_SIZE;
            memset(page, 0, PAGE_SIZE);
            header = {};
        }

        long offset = page_start + sizeof(pageHeader) + header.used_bytes;

        memcpy(page + sizeof(pageHeader) + header.used_bytes,
               record.data(),
               record_size);

        header.record_count++;
        header.used_bytes += record_size;

        index new_index;
        memcpy(new_index.account, students[i].account, sizeof(students[i].account));
        new_index.offset = offset;
        new_index.size = record_size;

        new_indexes.push_back(new_index);
    }

    memset(page + sizeof(pageHeader) + header.used_bytes,
           0,
           page_data_size - header.used_bytes);

    header.crc = CRC::Calculate(page + sizeof(pageHeader),
                                header.used_bytes,
                                CRC::CRC_32());

    memcpy(page, &header, sizeof(header));

    f.clear();
    f.seekp(page_start, std::ios::beg);
    f.write(page, PAGE_SIZE);

    if (!f) return false;

    for (int i = 0; i < new_indexes.size(); i++)
    {
        if (!insertIndexOrdered(new_indexes[i])) return false;
    }

    return saveIndex();
}

bool StudentManager::deleteStudent(std::string account)
{
    int pos = findIndexPosition(account);

    if (pos == -1)
    {
        return false;
    }

    std::vector<Student> students;

    for (int i = 0; i < indexes.size(); i++)
    {
        if (i == pos)
        {
            continue;
        }

        std::string current_account(indexes[i].account, sizeof(indexes[i].account));
        std::optional<Student> student = searchStudent(current_account);

        if (!student.has_value())
        {
            return false;
        }

        students.push_back(student.value());
    }

    return rebuildDataFile(students);
}

std::optional<Student> StudentManager::searchStudent(std::string& account)
{
    int pos = findIndexPosition(account);
    if(pos==-1)return std::nullopt;

    std::ifstream f(filename_,std::ios::binary);
    if(!f.is_open())return std::nullopt;

    long offset = indexes[pos].offset;
    int size = indexes[pos].size;

    long page_start = (offset / PAGE_SIZE) * PAGE_SIZE;

    char page[PAGE_SIZE] = {};

    f.seekg(page_start,std::ios::beg);
    f.read(page,PAGE_SIZE);

    if(!f)return std::nullopt;

    pageHeader header = {};
    memcpy(&header , page , sizeof(header));

    int page_data_size = PAGE_SIZE - sizeof(pageHeader);

    if(header.used_bytes < 0 || header.used_bytes > page_data_size){
        return std::nullopt;
    }

    uint32_t current_crc = CRC::Calculate(page + sizeof(pageHeader),
                                            header.used_bytes,
                                            CRC::CRC_32());

    if(current_crc != header.crc)return std::nullopt;

    long record_position_in_page = offset - page_start;

    if(record_position_in_page < sizeof(pageHeader)){
        return std::nullopt;
    }

    if (record_position_in_page + size > sizeof(pageHeader) + header.used_bytes)
    {
        return std::nullopt;
    }

    f.seekg(offset,std::ios::beg);

    Student s = deserializeStudent(f);
    if(!f)return std::nullopt;

    return s;
}

bool StudentManager::updateStudent(std::string& JSON)
{
    std::ifstream json_f(JSON);
    if (!json_f.is_open()) return false;

    nl::json j;
    json_f >> j;

    std::vector<Student> updates;

    if (j.is_array())
    {
        for (int i = 0; i < j.size(); i++)
        {
            updates.push_back(j[i].get<Student>());
        }
    }
    else
    {
        updates.push_back(j.get<Student>());
    }

    if (updates.empty()) return false;

    int page_data_size = PAGE_SIZE - sizeof(pageHeader);

    for (int i = 0; i < updates.size(); i++)
    {
        std::string account(updates[i].account, sizeof(updates[i].account));

        if (findIndexPosition(account) == -1) return false;

        std::string record = serializeStudent(updates[i]);
        if (record.size() > page_data_size) return false;

        for (int j = i + 1; j < updates.size(); j++)
        {
            int cmp = memcmp(updates[i].account,
                             updates[j].account,
                             sizeof(updates[i].account));

            if (cmp == 0) return false;
        }
    }

    std::vector<Student> students;

    for (int i = 0; i < indexes.size(); i++)
    {
        std::string current_account(indexes[i].account, sizeof(indexes[i].account));
        std::optional<Student> current = searchStudent(current_account);

        if (!current.has_value()) return false;

        Student student = current.value();

        for (int j = 0; j < updates.size(); j++)
        {
            int cmp = memcmp(student.account,
                             updates[j].account,
                             sizeof(student.account));

            if (cmp == 0)
            {
                student = updates[j];
            }
        }

        students.push_back(student);
    }

    return rebuildDataFile(students);
}

bool StudentManager::loadIndex()
{
    indexes.clear();

    std::ifstream f(index_,std::ios::binary);

    if(!f.is_open())
    {
        std::ofstream f(index_,std::ios::binary);
        return f.is_open();
    }

    index CurrentIndex;

    while(f.read(reinterpret_cast<char*>(&CurrentIndex),sizeof(index)))
    {
        indexes.push_back(CurrentIndex);
    }

    if(f.gcount() == 0)return true;
    f.close();
    return false;
}

bool StudentManager::saveIndex()
{
    std::ofstream f(index_, std::ios::binary | std::ios::trunc);

    if(!f.is_open())return false;

    for(int i = 0 ; i < indexes.size();i++){
        f.write(reinterpret_cast<char*>(&indexes[i]),sizeof(index));
        if(!f)return false;
    }

    f.close();
    return !f.fail();
}

bool StudentManager::rebuildDataFile(std::vector<Student>& students)
{
    int page_data_size = PAGE_SIZE - sizeof(pageHeader);

    for (int i = 0; i < students.size(); i++)
    {
        std::string record = serializeStudent(students[i]);

        if (record.size() > page_data_size)
        {
            return false;
        }
    }

    std::ofstream out(filename_, std::ios::binary | std::ios::trunc);
    if (!out.is_open()) return false;

    std::vector<index> new_indexes;

    char page[PAGE_SIZE] = {};
    pageHeader header = {};
    long page_start = 0;

    for (int i = 0; i < students.size(); i++)
    {
        std::string record = serializeStudent(students[i]);
        int record_size = static_cast<int>(record.size());

        if (header.used_bytes + record_size > page_data_size)
        {
            memset(page + sizeof(pageHeader) + header.used_bytes,
                   0,
                   page_data_size - header.used_bytes);

            header.crc = CRC::Calculate(page + sizeof(pageHeader),
                                        header.used_bytes,
                                        CRC::CRC_32());

            memcpy(page, &header, sizeof(header));

            out.write(page, PAGE_SIZE);
            if (!out) return false;

            page_start += PAGE_SIZE;
            memset(page, 0, PAGE_SIZE);
            header = {};
        }

        long offset = page_start + sizeof(pageHeader) + header.used_bytes;

        memcpy(page + sizeof(pageHeader) + header.used_bytes,
               record.data(),
               record_size);

        header.record_count++;
        header.used_bytes += record_size;

        index new_index;
        memcpy(new_index.account, students[i].account, sizeof(students[i].account));
        new_index.offset = offset;
        new_index.size = record_size;

        new_indexes.push_back(new_index);
    }

    if (header.record_count > 0)
    {
        memset(page + sizeof(pageHeader) + header.used_bytes,
               0,
               page_data_size - header.used_bytes);

        header.crc = CRC::Calculate(page + sizeof(pageHeader),
                                    header.used_bytes,
                                    CRC::CRC_32());

        memcpy(page, &header, sizeof(header));

        out.write(page, PAGE_SIZE);
        if (!out) return false;
    }

    indexes = new_indexes;

    return saveIndex();
}

std::string StudentManager::serializeStudent(Student &s)
{
    int name_size = static_cast<int>(s.name.size());

    std::string record;

    record.append(s.account, sizeof(s.account));
    record.append(reinterpret_cast<char*>(&name_size), sizeof(name_size));
    record.append(s.name.c_str(), name_size);
    record.append(s.telephone, sizeof(s.telephone));
    record.append(reinterpret_cast<char*>(&s.age), sizeof(s.age));
    record.append(s.date, sizeof(s.date));

    return record;
}

Student StudentManager::deserializeStudent(std::ifstream &f)
{
    Student s;
    int name_size = 0;

    f.read(s.account, sizeof(s.account));
    f.read(reinterpret_cast<char*>(&name_size), sizeof(name_size));

    s.name.resize(name_size);

    f.read(&s.name[0], name_size);
    f.read(s.telephone, sizeof(s.telephone));
    f.read(reinterpret_cast<char*>(&s.age), sizeof(s.age));
    f.read(s.date, sizeof(s.date));

    return s;
}

int StudentManager::findIndexPosition(std::string account)
{
    if(account.size() != 10)
    {
        return -1;
    }

    int left = 0;
    int right = static_cast<int>(indexes.size()) - 1;

    while (left <= right)
    {
        int mid = left + (right - left) / 2;
        int cmp = memcmp(indexes[mid].account, account.c_str(), sizeof(indexes[mid].account));

        if (cmp == 0)
        {
            return mid;
        }
        else if (cmp < 0)
        {
            left = mid + 1;
        }
        else
        {
            right = mid - 1;
        }
    }

    return -1;
}

bool StudentManager::insertIndexOrdered(index new_index)
{
    for(int i = 0;i< indexes.size(); i++){

        int cmp = memcmp(indexes[i].account, new_index.account, sizeof(new_index.account));
        
        if(cmp == 0)return false;

        if(cmp > 0){
            indexes.insert(indexes.begin() + i, new_index);
            return true;
        }
    }

    indexes.push_back(new_index);
    return  true;
}
