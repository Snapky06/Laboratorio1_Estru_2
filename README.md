# Student Records Manager

C++ CLI program that stores variable-length student records in a binary data file and uses a binary primary index.

JSON files are only used as input for adding or updating students.
==============================================================
## Compile

cd Laboratorio1_Estru_2
cmake -S . -B build
cmake --build build

Executable:

.\build\student.exe
==============================================================
## Commands

```powershell
.\build\student.exe --file <data_file.dat> [--index <index_file.idx>] add --student <student.json>
.\build\student.exe --file <data_file.dat> [--index <index_file.idx>] search <account>
.\build\student.exe --file <data_file.dat> [--index <index_file.idx>] delete <account>
.\build\student.exe --file <data_file.dat> [--index <index_file.idx>] update --student <student.json>
```
==============================================================
If `--index` is omitted, the index name is generated from the data file:

```text
students.dat -> students.idx
```
==============================================================
## Student JSON

Each JSON file contains one student:

```json
{
  "account": "2024000001",
  "name": "Ana Lopez",
  "telephone": "999988887777",
  "age": 20,
  "date": "20260201"
}
```
==============================================================
Fixed-size fields:

```text
account   = 10 bytes
telephone = 12 bytes
date      = 8 bytes, YYYYMMDD
```

Example JSON files are in `examples/`.
==============================================================
## Storage

The data file stores each student as:

account[10] + name_size + name + telephone[12] + age + date[8]
==============================================================
The index file stores entries with:

account + offset + size
==============================================================
At startup, the index is loaded into memory:

std::vector<index> indexes;
==============================================================
The index is kept ordered by account without using `std::sort`.
==============================================================
## Delete Strategy

This project uses hard delete.

When a student is deleted, its bytes are removed from the data file, its index entry is removed, and the offsets of later records are updated.
==============================================================
## Examples
==============================================================
.\build\student.exe --file students.dat add --student examples\student1.json
.\build\student.exe --file students.dat add --student examples\student2.json
.\build\student.exe --file students.dat search 2024000001
.\build\student.exe --file students.dat delete 2024000002
.\build\student.exe --file students.dat update --student examples\student3.json
==============================================================