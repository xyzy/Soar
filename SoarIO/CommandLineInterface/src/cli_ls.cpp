#include "cli_CommandLineInterface.h"

#ifdef WIN32
#include <windows.h>
#include <winbase.h>
#endif // WIN32

#include "sml_Names.h"

using namespace cli;
using namespace sml;

// ____                     _     ____
//|  _ \ __ _ _ __ ___  ___| |   / ___|
//| |_) / _` | '__/ __|/ _ \ |   \___ \
//|  __/ (_| | |  \__ \  __/ |___ ___) |
//|_|   \__,_|_|  |___/\___|_____|____/
//
bool CommandLineInterface::ParseLS(gSKI::IAgent* pAgent, std::vector<std::string>& argv) {
	unused(pAgent);

	// No arguments
	if (argv.size() != 1) {
		return HandleSyntaxError(Constants::kCLILS, Constants::kCLITooManyArgs);
	}
	return DoLS();
}

// ____        _     ____
//|  _ \  ___ | |   / ___|
//| | | |/ _ \| |   \___ \
//| |_| | (_) | |___ ___) |
//|____/ \___/|_____|____/
//
bool CommandLineInterface::DoLS() {

#ifdef WIN32

	// Windows-specific directory listing
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	// Open a file find using the universal dos wildcard *.*
	hFind = FindFirstFile("*.*", &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) {
		
		// Not a single file, or file system error, we'll just assume no files
		return true;	
	}

	// At least one file found, concatinate additional ones with newlines
	do {
		if (m_RawOutput) {
			// TODO: Columns and stats
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				m_Result += '[';
			}
			m_Result += FindFileData.cFileName;
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				m_Result += ']';
			}
			m_Result += '\n';
		} else {
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				AppendArgTagFast(sml_Names::kParamDirectory, sml_Names::kTypeString, FindFileData.cFileName);
			} else {
				AppendArgTagFast(sml_Names::kParamFilename, sml_Names::kTypeString, FindFileData.cFileName);
			}
		}

	} while (FindNextFile(hFind, &FindFileData));

	// Close the file find
	FindClose(hFind);
	return true;

#else // WIN32
	return HandleError("TODO: ls on non-windows platforms");
#endif // WIN32
}

