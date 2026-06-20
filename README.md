# Student Records Manager

Command-line C++ program for storing student records in a binary data file with a binary primary index.

JSON files are used as input for adding and updating students. The real stored data is kept in `.dat` pages, and the primary index is stored in a `.idx` file.

## Build

cmake -S . -B build
cmake --build build

The executable is created as:

.\build\student.exe

## File Names

Extensions are added automatically:

--file student   -> student.dat
--index index    -> index.idx
--student data   -> data.json

If `--index` is not provided, the index uses the same base name as `--file`.

## Commands

Add students from a JSON file:

.\build\student.exe --file student add --student student1

Search a student by account:

.\build\student.exe --file student search 2024000001

Delete a student by account:

.\build\student.exe --file student delete 2024000001

Update students from a JSON file:

.\build\student.exe --file student update --student student1

## JSON Input

`add` and `update` accept either one student object or an array of students.

Single student:

{
  "account": "2024000001",
  "name": "Ana Lopez",
  "telephone": "999988887777",
  "age": 20,
  "date": "20260201"
}

Multiple students:

[
  {
    "account": "2024000001",
    "name": "Ana Lopez",
    "telephone": "999988887777",
    "age": 20,
    "date": "20260201"
  },
  {
    "account": "2024000002",
    "name": "Luis Martinez",
    "telephone": "888877776666",
    "age": 21,
    "date": "20260315"
  }
]

Field sizes:

account   10 characters
telephone 12 characters
date      8 characters, YYYYMMDD

## Storage

Each student record is serialized as:

account[10] + name_size + name + telephone[12] + age + date[8]

The data file is divided into fixed-size pages:

PAGE_SIZE = 512 bytes

Each page starts with:

struct pageHeader
{
    int record_count;
    int used_bytes;
    uint32_t crc;
};

Page layout:

[pageHeader][used record bytes][zero padding]

The unused space inside a page is internal fragmentation. Those bytes are kept as zeroes.

## CRC

Each page stores a CRC-32 value in its header.

The CRC is calculated only over the used record bytes:

used record bytes only

It does not include:

pageHeader
zero padding

When searching, the page CRC is checked before reading the record. If the CRC does not match, the search fails.

## Index

The primary index stores:

struct index
{
    char account[10];
    long offset;
    int size;
};

Meaning:

account -> primary key
offset  -> byte where the record starts in the .dat file
size    -> real size of the serialized record

The index is loaded into memory, kept ordered by account, and saved back to disk after changes.

## Delete And Update

The project uses hard delete.

Because the data file is paged and every page has a CRC, delete and update rebuild the data file instead of cutting bytes directly.

The rebuild process is:

read valid students
apply delete or update
rewrite pages
recalculate CRC
rebuild index offsets
save index

This keeps page sizes, CRC values, zero padding, and index offsets correct.
