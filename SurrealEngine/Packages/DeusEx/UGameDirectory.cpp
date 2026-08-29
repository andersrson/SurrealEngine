
#include "Packages/DeusEx/UDeusExSaveInfo.h"
#include "Precomp.h"
#include "UGameDirectory.h"
#include "Utils/Logger.h"
#include "Engine.h"
#include "Package/PackageManager.h"
#include <filesystem>

void UDXGameDirectory::GetGameDirectory()
{
	if (GameDirectoryType() == EGameDirectoryTypes::GD_Maps)
	{
		currentDirectory = fs::path(engine->LaunchInfo.gameRootFolder) / "Maps";
		PopulateDirectoryList();
	}
	else
	{
		currentDirectory = engine->packages->GetSaveFolderPath();
		PopulateDirectoryList();
		PopulateSaveInfoPointers();
	}
}

int UDXGameDirectory::GetNewSaveFileIndex()
{
	const auto saveFolder = engine->packages->GetSaveFolderPath();

	// Save folders being formatted like Save0001 implies that the number can go up to 9999
	for (int i = 1; i < 10000; i++)
	{
		auto folderPath = saveFolder / GetSaveIndexFolderName(i);
		if (!fs::exists(folderPath) || (fs::exists(folderPath) && !fs::is_directory(folderPath)))
			return i;
	}

	return 0;
}

std::string UDXGameDirectory::GenerateSaveFilename(int saveIndex)
{
	return GetSaveIndexFolderName(saveIndex);
}

std::string UDXGameDirectory::GenerateNewSaveFileName(std::optional<int> newIndex)
{
	return GenerateSaveFilename(newIndex ? *newIndex : GetNewSaveFileIndex());
}

int UDXGameDirectory::GetDirCount()
{
	return (int)DirectoryList().size();
}

std::string UDXGameDirectory::GetDirFilename(int fileIndex)
{
#if 0
	return DirectoryList()[fileIndex];
#else
	return {};
#endif
}

void UDXGameDirectory::SetDirType(EGameDirectoryTypes newDirType)
{
	GameDirectoryType() = newDirType;
}

void UDXGameDirectory::SetDirFilter(const std::string& strFilter)
{
	CurrentFilter() = strFilter;
}

UDXSaveInfo* UDXGameDirectory::GetSaveInfo(int fileIndex)
{
	if (GameDirectoryType() != EGameDirectoryTypes::GD_SaveGames)
		// We're not in the Save folder
		return nullptr;

	auto pkg = engine->packages->GetSaveInfoPackage(GetSaveIndexFolderName(fileIndex));

	if (!pkg)
		return nullptr;

	return Cast<UDXSaveInfo>(pkg->GetUObject("DeusExSaveInfo", "MyDeusExSaveInfo"));
}

UDXSaveInfo* UDXGameDirectory::GetSaveInfoFromDirectoryIndex(int DirectoryIndex)
{
	auto dirs = DirectoryList();
	if (DirectoryIndex < 0 || (size_t)DirectoryIndex >= dirs.size())
		return nullptr;

	Package* pkg = engine->packages->GetSaveInfoPackage(dirs[DirectoryIndex]);
	if (!pkg)
		return nullptr;

	return Cast<UDXSaveInfo>(pkg->GetUObject("DeusExSaveInfo", "MyDeusExSaveInfo"));
}

UDXSaveInfo* UDXGameDirectory::GetTempSaveInfo()
{
	if(!TempSaveInfo())
	{
		auto cls = engine->packages->FindClass("DeusEx.DeusExSaveInfo");
		TempSaveInfo() = Cast<UDXSaveInfo>(engine->packages->GetTransientPackage()->NewObject("DeusExSaveInfo", cls, ObjectFlags::Transient));
	}
	return TempSaveInfo();
}

void UDXGameDirectory::DeleteSaveInfo(UDXSaveInfo& saveInfo)
{
	auto arr = LoadedSaveInfoPointers();
	for (size_t i = 0; i < arr.size(); i++)
	{
		if (arr[i] != &saveInfo)
			continue;
		for (size_t j = i; j + 1 < arr.size(); j++)
			arr[j] = arr[j + 1];
		arr.pop_back();
		return;
	}
}

void UDXGameDirectory::PurgeAllSaveInfo()
{
	if (GameDirectoryType() == EGameDirectoryTypes::GD_Maps)
		return;

	if (fs::exists(currentDirectory) && fs::is_directory(currentDirectory))
		for (auto& p : fs::directory_iterator(currentDirectory))
			// We're not using remove_all() as we don't want to remove the Save folder itself
			fs::remove(p.path());
}

int UDXGameDirectory::GetSaveFreeSpace()
{
	// Returns a value in KBs, which limits us to ~2TB of "free space" max.
	// Should be enough but still
	const auto freeSpaceInKBs = static_cast<int>(fs::space(currentDirectory).free / 1024);
	// Capped at 1TB
	return std::min(freeSpaceInKBs, 1000 * 1024 * 1024);
}

int UDXGameDirectory::GetSaveDirectorySize(int saveIndex)
{
	if (GameDirectoryType() != EGameDirectoryTypes::GD_SaveGames)
		// We're not in the Save folder
		return 0;

	int size = 0;

	for (auto& p : fs::directory_iterator(currentDirectory / GetSaveIndexFolderName(saveIndex)))
		size += (int)p.file_size();

	// script expects KB
	return (int)size / 1024;
}

std::string UDXGameDirectory::GetSaveIndexFolderName(int saveIndex)
{
	if(saveIndex == -1)
		return "QuickSave";

	std::string folderName = std::to_string(saveIndex);
	folderName.insert(0, 4 - folderName.length(), '0'); // Pad with 0s
	return "Save" + folderName;
}

void UDXGameDirectory::PopulateDirectoryList()
{
	auto list = DirectoryList();
	while (list.size() > 0)
		list.pop_back();

	const bool saveGames = (GameDirectoryType() == EGameDirectoryTypes::GD_SaveGames);

	for (auto& p : fs::directory_iterator(currentDirectory))
	{
		if (saveGames)
		{
			// Save folders: directories containing a SaveInfo file (same test as ScanSaveInfos)
			if (p.is_directory() &&
				fs::is_regular_file(p.path() / ("SaveInfo." + engine->packages->GetSaveExtension())))
				list.push_back(p.path().filename().string());
		}
		else if (p.is_regular_file())
		{
			list.push_back(p.path().filename().string());
		}
	}
}

void UDXGameDirectory::PopulateSaveInfoPointers()
{
	auto arr = LoadedSaveInfoPointers();
	while (arr.size() > 0)
		arr.pop_back();

	for (const auto& saveInfoPackage : engine->packages->GetSaveInfoPackages())
	{
		if (auto* info = Cast<UDXSaveInfo>(saveInfoPackage.second->GetUObject("DeusExSaveInfo", "MyDeusExSaveInfo")))
			arr.push_back(info);
	}
}
