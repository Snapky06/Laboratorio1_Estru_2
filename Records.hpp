#pragma once
#include <cstring>
#include <string>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <cstdint>

namespace nl = nlohmann;

struct index
{
char account[10];
long offset;
int size;
};

const int PAGE_SIZE = 512;

struct PageHeader{
int record_count;
int used_bytes;
uint32_t crc;
}
struct Student
{
    char account[10]; //Llave primaria
    std::string name;
    char telephone[12];
    int age;
    char date[8];
};

inline void to_json(nl::json& j, const Student& s)
{
    j = {
        {"account", std::string(s.account, sizeof(s.account))},
        {"name", s.name},
        {"telephone", std::string(s.telephone, sizeof(s.telephone))},
        {"age", s.age},
        {"date", std::string(s.date, sizeof(s.date))}
    };
}

inline void from_json(const nl::json& j, Student& s)
{
    if (!j.contains("account") || !j.contains("name") || !j.contains("telephone") ||
        !j.contains("age") || !j.contains("date"))
    {
        throw std::runtime_error("Missing required student field");
    }

    std::string account = j.at("account").get<std::string>();
    std::string name = j.at("name").get<std::string>();
    std::string telephone = j.at("telephone").get<std::string>();
    int age = j.at("age").get<int>();
    std::string date = j.at("date").get<std::string>();

    if (account.size() != sizeof(s.account))
    {
        throw std::runtime_error("Account must have exactly 10 characters");
    }

    if (telephone.size() != sizeof(s.telephone))
    {
        throw std::runtime_error("Telephone must have exactly 12 characters");
    }

    if (date.size() != sizeof(s.date))
    {
        throw std::runtime_error("Date must have exactly 8 characters");
    }

    if (name.empty())
    {
        throw std::runtime_error("Name cannot be empty");
    }

    if (age < 0)
    {
        throw std::runtime_error("Age cannot be negative");
    }

    std::memcpy(s.account, account.c_str(), sizeof(s.account));
    s.name = name;
    std::memcpy(s.telephone, telephone.c_str(), sizeof(s.telephone));
    s.age = age;
    std::memcpy(s.date, date.c_str(), sizeof(s.date));
}

/*
Linea De Comando
-Agregar Registros (JSON)
-Eliminar Resgistros (Por numero de cuenta)
-Buscar Registros (Por numero de cuenta)
-Actualizar Informacion (JSON)

Indice Primario Simple
-Cuenta   ->
-Ofsset   -> Campos
-Size     ->
-Requerimientos
 -Ordenado (No es valido utilizar std::sort)
 -Llaves Unicas
 -
 
-Si utiliza soft delete debera implementar una funcion (Compact) para removerlos de el archivo*/