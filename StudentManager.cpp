#include "StudentManager.hpp"
#include <cstring>

StudentManager::StudentManager(std::string filename, std::string index)
{
    filename_ = filename;
    index_ = index;
    open();
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
    return loadIndex;
    return false;
}

void StudentManager::close()
{
    std::ofstream f(filename_,std::ios::binary);
    saveIndex();
    
    if(f.is_open())f.close();
}

bool StudentManager::addRegister(std::string file)
{
    
}

std::optional<Student> StudentManager::searchStudent(std::string account)
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
    std::ofstream f(index_,std::ios::binary);

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
    int right = indexes.size() - 1;

    while(left<=right){
        int mid = left + (right - left)/2;
        int cmp = strcmp(indexes[mid].account,account.c_str());

        if(cmp == 0){
            return mid;
        }else if(cmp < 0){
            left = mid + 1;
        }else{
            right = mid - 1;
        }
    }
    return -1;
}

bool StudentManager::insertIndexOrdered(index new_index)
{
    for(int i = 0;i< indexes.size() - 1; i++){

        int cmp = strcmp(indexes[i].account,new_index.account);
        
        if(cmp == 0)return false;

        if(cmp > 0){
            indexes.insert(indexes.begin() + i, new_index);
            return true;
        }
    }

    indexes.push_back(new_index);
    return  false;
}
