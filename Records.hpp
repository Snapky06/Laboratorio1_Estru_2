#include <string>

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