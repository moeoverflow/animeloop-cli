//
// Created by shin on 12/2/17.
//

#ifndef ANIMELOOP_CLI_CHILD_PROCESS_H
#define ANIMELOOP_CLI_CHILD_PROCESS_H

#include <functional>

void child_process(std::function<void()> functor);

#endif //ANIMELOOP_CLI_CHILD_PROCESS_H
