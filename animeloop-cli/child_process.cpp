#include "child_process.hpp"

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/wait.h>
#include <unistd.h>
#endif

#include <iostream>

using namespace std;

void child_process(string cli) {
    #ifdef _WIN32

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

    #else

    int pid = fork();

    if(pid == -1) {
        /* Failed to fork */
        cerr << "Fork failed" << endl;
        throw;
    } else if(pid == 0) {
        int status = system(cli.c_str());
        if (status) {
            cerr << "The child was killed or segfaulted or something." << endl;
        }
        exit(100);
    }

    /* Optionally, wait for the child to exit and get
       the exit status. */
    int status;
    waitpid(pid, &status, 0);
    if(! WIFEXITED(status)) {
        cerr << "The child was killed or segfaulted or something." << endl;
    }

    status = WEXITSTATUS(status);

    #endif


}