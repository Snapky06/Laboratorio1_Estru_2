#include "StudentManager.hpp"
#include <cstring>

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
    return false;
}

void StudentManager::close()
{
    saveIndex();
}

bool StudentManager::addRegister(std::string& JSON)
{
    std::ifstream json_f(JSON);
    if(!json_f.is_open())return false;

    nl::json j;
    json_f >> j;
    Student s = j.get<Student>();

    std::string account(s.account,sizeof(s.account));
    if(findIndexPosition(account)!=-1)return false;

    std::ofstream f(filename_,std::ios::binary|std::ios::app);
    if(!f.is_open())return false;

    f.seekp(0,std::ios::end);
    long offset = static_cast<long>(f.tellp());
    int name_size = static_cast<int>(s.name.size());

    int record_size = sizeof(s.account) + sizeof(name_size)
    + name_size + sizeof(s.telephone) + sizeof(s.age) + sizeof(s.date);

    f.write(s.account, sizeof(s.account));
    f.write(reinterpret_cast<char*>(&name_size), sizeof(name_size));
    f.write(s.name.c_str(), name_size);
    f.write(s.telephone, sizeof(s.telephone));
    f.write(reinterpret_cast<char*>(&s.age), sizeof(s.age));
    f.write(s.date, sizeof(s.date));

    f.close();

    index new_index;
    memcpy(new_index.account,s.account,sizeof(s.account));
    new_index.offset = offset;
    new_index.size = record_size;
    if(!insertIndexOrdered(new_index))return false;

    return saveIndex();
}

bool StudentManager::deleteStudent(std::string account)
{
    int pos = findIndexPosition(account);
    if(pos == -1)return false;

    long deleted_offset = indexes[pos].offset;
    int deleted_size = indexes[pos].size;

    std::ifstream f(filename_,std::ios::binary);
    if(!f.is_open())return false;

    f.seekg(0,std::ios::end);
    long file_size = static_cast<long>(f.tellg());
    f.seekg(0,std::ios::beg);

    std::vector<char> data(file_size);
    f.read(data.data(),file_size);
    f.close();

     if (deleted_offset < 0 || deleted_size < 0 || deleted_offset + deleted_size > file_size)
    {
        return false;
    }

    data.erase(
        data.begin() + deleted_offset,
        data.begin() + deleted_offset + deleted_size
    );

    std::ofstream f_out(filename_,std::ios::binary | std::ios::trunc);
    if(!f_out.is_open())return false;

    f_out.write(data.data(),data.size());
    f_out.close();

    if(!f_out)return false;

    indexes.erase(indexes.begin() + pos);
    for(int i  = 0 ; i < indexes.size() ; i++){

        if (indexes[i].offset > deleted_offset)
        {
            indexes[i].offset -= deleted_size;
        }
    }

    return saveIndex();
}

std::optional<Student> StudentManager::searchStudent(std::string& account)
{
    std::ifstream f(filename_,std::ios::binary);
    if(!f.is_open())return std::nullopt;

    int pos = findIndexPosition(account);   
    if(pos == -1)return std::nullopt;

    f.seekg(indexes[pos].offset,std::ios::beg);
    if(!f)return std::nullopt;

    Student s;
    int name_size = 0;

    f.read(reinterpret_cast<char*>(&s.account),sizeof(s.account));
    f.read(reinterpret_cast<char*>(&name_size),sizeof(name_size));

    s.name.resize(name_size);
    f.read(&s.name[0],name_size);

    f.read(reinterpret_cast<char*>(&s.telephone),sizeof(s.telephone));
    f.read(reinterpret_cast<char*>(&s.age),sizeof(s.age));
    f.read(reinterpret_cast<char*>(&s.date),sizeof(s.date));

    if(!f)return std::nullopt;
    f.close();
    return s;
}

bool StudentManager::updateStudent(std::string& JSON)
{
    std::ifstream json_f(JSON);

    if (!json_f.is_open())
    {
        return false;
    }

    nl::json j;
    json_f >> j;

    Student s = j.get<Student>();
    std::string account(s.account, sizeof(s.account));

    int pos = findIndexPosition(account);

    if (pos == -1)
    {
        return false;
    }

    int name_size = static_cast<int>(s.name.size());

    std::string new_record;
    new_record.append(s.account, sizeof(s.account));
    new_record.append(reinterpret_cast<char*>(&name_size), sizeof(name_size));
    new_record.append(s.name.c_str(), name_size);
    new_record.append(s.telephone, sizeof(s.telephone));
    new_record.append(reinterpret_cast<char*>(&s.age), sizeof(s.age));
    new_record.append(s.date, sizeof(s.date));

    long old_offset = indexes[pos].offset;
    int old_size = indexes[pos].size;
    int new_size = static_cast<int>(new_record.size());
    int difference = new_size - old_size;

    std::ifstream f(filename_, std::ios::binary);

    if (!f.is_open())
    {
        return false;
    }

    f.seekg(0, std::ios::end);
    long file_size = static_cast<long>(f.tellg());
    f.seekg(0, std::ios::beg);

    if (old_offset < 0 || old_size < 0 || old_offset + old_size > file_size)
    {
        return false;
    }

    std::vector<char> data(file_size);
    f.read(data.data(), file_size);
    f.close();

    if (!f)
    {
        return false;
    }

    data.erase(data.begin() + old_offset, data.begin() + old_offset + old_size);
    data.insert(data.begin() + old_offset, new_record.begin(), new_record.end());

    std::ofstream out(filename_, std::ios::binary | std::ios::trunc);

    if (!out.is_open())
    {
        return false;
    }

    out.write(data.data(), data.size());
    out.close();

    if (!out)
    {
        return false;
    }

    indexes[pos].size = new_size;

    for (int i = 0; i < indexes.size(); i++)
    {
        if (indexes[i].offset > old_offset)
        {
            indexes[i].offset += difference;
        }
    }

    return saveIndex();
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

int StudentManager::findIndexPosition(std::string account)
{
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
