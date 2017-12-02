//
// Created by shin on 12/2/17.
//
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

#include "child_process.hpp"

using namespace std;

void child_process(function<void()> functor) {
    int pid = fork();

    if(pid == -1) {
        /* Failed to fork */
        cerr << "Fork failed" << endl;
        throw;
    } else if(pid == 0) {
        functor();
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
}