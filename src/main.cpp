#include "main.h"

#include <conio.h>

#include "GameData.h"
#include "script.h"
#include "utils.h"


/*
void process_subtitle(DatFileEntry* entry, char* outPath)
{
	char* buffer = new char[entry->Size];
	entry->Dat->ReadFile(entry->Index, buffer);

	subtitle_content* subtitle = (subtitle_content*)buffer;

	for (uint32_t i = 0; i < subtitle->numEntries; i++)
	{
		wprintf(L"%s : %s", subtitle->entries[i].id, subtitle->entries[i].text);
	}
}

void do_export_single(wchar_t* datPath, char* outPath, bool debug)
{
	DatFile dat(datPath);

	if (dat.NumEntries() == 0) return;

	char dat_path[MAX_PATH], dat_filename[MAX_PATH];
	split_path_file(dat_path, dat_filename, datPath);
	printf("DAT file %s is OK.\n\n", dat_filename);

	char fullPath[MAX_PATH];
	GetFullPathNameA(outPath, MAX_PATH, fullPath, NULL);
	SHCreateDirectoryExA(NULL, fullPath, NULL);

	char datOutPath[MAX_PATH];
	sprintf_s(datOutPath, "%s\\%.*s", fullPath, (int)strlen(dat_filename) - 4, dat_filename);
	CreateDirectoryA(datOutPath, NULL);

	char scriptOutPath[MAX_PATH];
	sprintf_s(scriptOutPath, "%s\\scripts", datOutPath);
	CreateDirectoryA(scriptOutPath, NULL);

	char subtitleOutPath[MAX_PATH];
	sprintf_s(subtitleOutPath, "%s\\subtitles", datOutPath);
	CreateDirectoryA(subtitleOutPath, NULL);

	DatFileEntry* entry;

	for (int i = 0; i < dat.NumEntries(); i++)
	{
		entry = dat[i];

		if (strcmp(entry->Extension, "bin") == 0)
		{
			process_script(entry, scriptOutPath, debug);
		}
		else if (strcmp(entry->Extension, "smd") == 0)
		{
			process_subtitle(entry, subtitleOutPath);
		}
		else
		{
			continue;
		}

		//if (strcmp(entry->Name, "p100_fa238e7c_scp.bin") == 0)
		//{
		//	char* buffer = new char[entry->Size];
		//	dat.ReadFile(i, buffer);

		//	int binSize;
		//	char* bin = script_import(buffer, "", &binSize);

		//	dat.Replace(i, bin, binSize);
		//	//dat.Save("p100.dat");

		//	delete buffer;
		//}

		printf("Done.\n");
	}

}
*/


int wmain(int argc, wchar_t *argv[], wchar_t *envp[])
{
	if (argc < 2)
	{
		DoHelp();
		return 0;
	}

	auto it = commands.find(argv[1]);
	if (it == commands.end())
	{
		DoHelp();
		return 0;
	}

	int ac = argc - 2;
	wchar_t ** av = argv + 2;
	it->second(ac, av);

	//while (!_kbhit()) {}
	return 0;
}

void DoHelp()
{
	wcout << std::endl << L"NieR: Automata Text Tools by @micktu" << std::endl;
	wcout << std::endl << L"  Available commands:" << std::endl;
	wcout << std::endl << L"  att list <PATH> [FILTER]" << std::endl;
	wcout << L"      Lists all files in a directory or .dat container specified by <PATH>." << std::endl;
}

void DoList(int &argc, wchar_t ** &argv)
{
	str_t path = add_slash(path_normalize(argv[0]));

	GameData gd(path);
	ReadGameData(gd, argc > 1 ? argv[1] : L"", L"Listing");

	for (GameFile& gf : gd)
	{
		wcout << gf.Path << gf.Filename << std::endl;
	}
}

void DoExtract(int &argc, wchar_t ** &argv)
{
	str_t path = add_slash(path_normalize(argv[0]));
	str_t outPath = add_slash(path_normalize(argv[1]));

	GameData gd(path);
	ReadGameData(gd, argc > 2 ? argv[2] : L"", L"Extracting");

	std::vector<DatFile>& datFiles = gd.GetDatFiles();

	for (GameFile& gf : gd)
	{
		if (gf.DatIndex != ~0)
		{
			str_t outDir = outPath + add_slash(path_strip_filename(strip_slash(gf.Path)));
			create_dir_recursive(outDir);
			DatFile& dat = datFiles[gf.DatIndex];
			dat.ExtractFile(&dat[gf.IndexInDat], outDir);
		}
	}
}

void DoExport(int &argc, wchar_t ** &argv)
{
	str_t path = add_slash(path_normalize(argv[0]));
	str_t outPath = add_slash(path_normalize(argv[1]));


	GameData gd(path);
	ReadGameData(gd, L"text", L"Exporting");

	std::vector<DatFile>& datFiles = gd.GetDatFiles();

	for (GameFile& gf : gd)
	{
		str_t outDir = outPath + add_slash(path_strip_filename(strip_slash(gf.Path)));
		create_dir_recursive(outDir);

		if (ext_equals(gf.Filename, L".bin"))
		{
				wcout << L"Processing script " << gf.Filename << L"... ";

				DatFileEntry& dat = datFiles[gf.DatIndex][gf.IndexInDat];

				char* bin = dat.ReadFile();
				ScriptContent* content = script_extract(bin);

				outDir += gf.Filename;

				//script_dump_debug(bin, outDir + L".debug.txt", content);

				if (nullptr != content)
				{
					script_export(content, outDir + L".txt");
					wcout << L"Done." << std::endl;
				}
				else
				{
					wcout << L"Failed to load script." << std::endl;
				}

				delete content;
				delete bin;
		}
	}

	wcout << std::endl << L"Fin";
}

void ReadGameData(GameData& gd, const str_t filter, const str_t verb)
{
	size_t numFiles = gd.GetFilenames().size();
	wcout << L"Checking " << numFiles << L" files in " << gd.GetBasePath() << L"... ";
	gd.Read(filter);

	size_t numGameFiles = gd.GetGameFiles().size();
	size_t numDatFiles = gd.GetDatFiles().size();
	wcout << L"Done." << std::endl;
	if (!verb.empty()) wcout << verb << L" ";
	wcout << numGameFiles << L" files in " << numDatFiles << L" archives." << std::endl;
	wcout << std::endl;
}