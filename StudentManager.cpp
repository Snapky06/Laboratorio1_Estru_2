#include "StudentManager.hpp"

bool StudentManager::addRegister(std::string file)
{
    
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
return !f.fail();
}
