#include <string>
#include <nlohmann/json.hpp>

namespace nl = nlohmann;

struct index
{
char account[10];
long offset;
int size;
};

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
    std::string account = j.at("account").get<std::string>();
    std::string telephone = j.at("telephone").get<std::string>();
    std::string date = j.at("date").get<std::string>();

    memcpy(s.account, account.c_str(), sizeof(s.account));
    s.name = j.at("name").get<std::string>();
    memcpy(s.telephone, telephone.c_str(), sizeof(s.telephone));
    s.age = j.at("age").get<int>();
    memcpy(s.date, date.c_str(), sizeof(s.date));
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