#pragma once

#include "stdafx.h"


static const uint32_t DAT_MAGIC = '\x00TAD';

typedef uint32_t dat_offset_t, dat_size_t;
using dat_name_t = char[MAX_PATH];
using dat_ext_t = char[4];

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
	DatFileEntry() = default;
	DatFileEntry(class DatFile* dat, uint32_t index, wstr_t name, dat_size_t size, dat_offset_t offset);

	std::ifstream OpenFile();

	uint32_t Index;
	wstr_t Name;
	dat_size_t Size;
	dat_offset_t Offset;

	DatFile* Dat;

	char_vector_t ReadFile();
};

class DatFile
{
	wstr_t BasePath;
	wstr_t Filename;
	wstr_t FullPath;
	std::vector<DatFileEntry> _entries;
	dat_header _header;

public:

private:

public:
	static bool CheckFile(wstr_t &path);

	DatFile(wstr_t path, wstr_t filename);

	bool Read(wstr_t &path, wstr_t &filename);
	std::ifstream OpenFile(const DatFileEntry * entry);
	void ReadFile(const DatFileEntry& entry, char* buffer);
	void InjectFile(int index, char_vector_t buffer);
	const DatFileEntry * FindFile(wstr_t &name);
	void ExtractFile(DatFileEntry &entry, wstr_t outPath);
	void ExtractAll(wstr_t &outPath);

	FORCEINLINE const wstr_t &GetPath() const { return FullPath; }
	FORCEINLINE const wstr_t &GetFilename() const { return Filename; }
	FORCEINLINE size_t NumEntries() { return _entries.size(); }
	FORCEINLINE DatFileEntry& operator[](size_t index) { return _entries[index]; }

	auto begin() { return _entries.begin(); }
	auto end() { return _entries.end(); }
};
