#include "child_process.hpp"

using namespace std;

void child_process(string cli) {
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	DWORD exit_code = 0;
	LPSTR exec = const_cast<char *>(cli.c_str());

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// Start the child process. 
	if (!CreateProcess(
		NULL,           // No module name (use command line)
		exec,           // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi             // Pointer to PROCESS_INFORMATION structure
	)) {
		printf("CreateProcess failed (%d)\n", GetLastError());
		return;
	}

	// Wait until child process exits.
	WaitForSingleObject(pi.hProcess, INFINITE);

	// Get exit code
	GetExitCodeProcess(pi.hProcess, &exit_code);
	if (exit_code) {
		cerr << "The child was killed or segfaulted or something." << endl;
	}

	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}