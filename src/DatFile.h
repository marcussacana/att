#pragma once

#include "stdafx.h"

#define DAT_MAGIC '\x00TAD'

typedef uint32_t dat_offset_t, dat_size_t;

typedef char dat_name_t[MAX_PATH];
typedef char dat_ext_t[4];

struct dat_header
{
	uint32_t magic;
	uint32_t num_files;
	dat_offset_t files;
	dat_offset_t extensions;
	dat_offset_t names;
	dat_offset_t sizes;
	dat_offset_t unknown1;
	uint32_t unknown2;
};

struct DatFileEntry
{
	uint32_t Index;
	dat_ext_t Extension;
	dat_name_t Name;
	dat_size_t Size;
	dat_offset_t Offset;

	class DatFile* Dat;
};

class DatFile
{
	const wchar_t* filename;
	int _numEntries;
	DatFileEntry* _entries;
	dat_header _header;

public:

private:

public:
	static bool CheckFile(const wchar_t* path);
	
	DatFile() = default;
	DatFile(const DatFile &obj) = default;
	DatFile(const wchar_t* filename);
	~DatFile();

	bool Read(const wchar_t* filename);
	void ReadFile(int index, char* buffer);
	void InjectFile(int index, char * buffer, uint32_t numBytes);
	int FindFile(const char* name);
	void ExtractFile(int index, const char* outPath);
	void ExtractAll(const char* outPath);
	void PrintFiles();

	__forceinline int NumEntries() const { return _numEntries; }
	__forceinline DatFileEntry* operator[](int index) { return _entries + index; }
};
