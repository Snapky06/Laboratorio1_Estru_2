# Student Records Manager

Command-line program in C++ for storing student records in a binary data file with a binary primary index.

JSON files are used only as input for adding and updating students. The stored records are kept in `.dat` and `.idx` files.
=====================================================================================================================================
## Compile

cmake -S . -B build
cmake --build build

Run:

.\build\student.exe
=====================================================================================================================================
## File Names

The program adds extensions automatically:

--file student   uses student.dat
--index student  uses student.idx
--student data1  uses data1.json

If `--index` is not provided, the index uses the same base name as `--file`.
=====================================================================================================================================
## Commands
=====================================================================================================================================
### Add

Adds one student from a JSON file.

.\build\student.exe --file student add --student student1

This reads `student1.json`, writes the new record into `student.dat`, and inserts a new entry into `student.idx`. The account must not already exist.
=====================================================================================================================================
### Search

Searches for a student by account number.

.\build\student.exe --file student search 2024000001

This uses the index to find the record position in the data file and prints the student information.
=====================================================================================================================================
### Delete

Deletes a student by account number.

.\build\student.exe --file student delete 2024000001

This project uses hard delete, so the record is physically removed from the data file. After removing it, the index is updated so the remaining offsets stay correct.
=====================================================================================================================================
### Update

Updates an existing student using a JSON file.

.\build\student.exe --file student update --student student1

The account inside the JSON is used to find the record. The record is replaced with the new information, and the index is adjusted if the record size changes.
=====================================================================================================================================
## JSON Format

Each JSON file contains one student:

{
  "account": "2024000001",
  "name": "Ana Lopez",
  "telephone": "999988887777",
  "age": 20,
  "date": "20260201"
}

Fixed-size fields:

account   10 bytes
telephone 12 bytes
date      8 bytes, YYYYMMDD
=====================================================================================================================================
## Examples

.\build\student.exe --file student add --student student1
.\build\student.exe --file student add --student student2
.\build\student.exe --file student add --student student3

.\build\student.exe --file student search 2024000001
.\build\student.exe --file student delete 2024000002
.\build\student.exe --file student update --student student3
=====================================================================================================================================
## Storage

Each record is stored in the data file as:

account[10] + name_size + name + telephone[12] + age + date[8]

The index stores:

account + offset + size

The index is loaded into memory, kept ordered by account, and saved back to disk after changes.
=====================================================================================================================================