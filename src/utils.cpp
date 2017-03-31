#include "utils.h"
#include <Shlwapi.h>
#include <ShlObj.h>
#include <PathCch.h>


static wchar_t PATH_BUFFER[MAX_PATH];

str_t path_strip_filename(str_t filename)
{
	filename.copy(PATH_BUFFER, MAX_PATH, 0);
	PathCchRemoveFileSpec(PATH_BUFFER, MAX_PATH);
	return str_t(PATH_BUFFER);
}

void split_path(const str_t &path, str_t &name, str_t &ext)
{
	wchar_t *cext;
	wchar_t *cname = PathFindFileName(path.c_str());
	PathCchFindExtension(cname, MAX_PATH, &cext);

	name = str_t(cname);
	ext = str_t(cext);
}

str_t strip_slash(const str_t &path)
{
	//path.copy(PATH_BUFFER, MAX_PATH);
	wcscpy_s(PATH_BUFFER, MAX_PATH, path.c_str());
	size_t size = path.size();
	if (S_OK == PathCchRemoveBackslash(PATH_BUFFER, MAX_PATH))
	{
		return str_t(PATH_BUFFER);
	}

	return path;
}

str_t add_slash(const str_t &path)
{
	//path.copy(PATH_BUFFER, MAX_PATH);
	wcscpy_s(PATH_BUFFER, path.c_str());
	if (S_OK == PathCchAddBackslash(PATH_BUFFER, MAX_PATH))
	{
		return str_t(PATH_BUFFER);
	}

	return path;
}

str_vector_t find_files_recursive(str_t path, int pathSize = -1)
{
	if (pathSize < 0) pathSize = path.size() - 1;
	
	str_t dirMask = path + L"*";

	WIN32_FIND_DATA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	hFind = FindFirstFile(dirMask.c_str(), &ffd);

	str_vector_t files;
	if (INVALID_HANDLE_VALUE == hFind) return files;

	do
	{
		wchar_t* name = ffd.cFileName;
		if (name[0] == '.' && (name[1] == '.' || name[1] == 0))
		{
			continue;
		}

		str_t filename = path + name;
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			filename += '\\';
			str_vector_t inner = find_files_recursive(filename, pathSize);
			files.insert(files.end(), inner.begin(), inner.end());
			continue;
		}

		files.emplace_back(filename.c_str() + pathSize + 1);
	}
	while (FindNextFile(hFind, &ffd) != 0);

	FindClose(hFind);

	return files;
}

str_vector_t find_files(str_t path)
{
	if (!is_dir(path)) return str_vector_t();
	return find_files_recursive(path);
}

void create_dir_recursive(str_t path)
{
	wchar_t fullPath[MAX_PATH];
	GetFullPathName(path.c_str(), MAX_PATH, fullPath, NULL);
	SHCreateDirectoryEx(NULL, fullPath, NULL);
}

str_t cstr_to_wstr(const char* cstr)
{
	size_t len = strlen(cstr);
	wchar_t* buf = new wchar_t[len + 1];

	size_t numChars;
	mbstowcs_s(&numChars, buf, len + 1, cstr, len);

	str_t wstr(buf);
	delete[] buf;
	return wstr;
}

std::ifstream::pos_type get_file_size(str_t filename)
{
	std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
	return in.tellg();
}
