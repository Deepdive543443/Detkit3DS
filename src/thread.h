#ifndef THREAD_H
#define THREAD_H

#include "sections.h"

void main_loop_locker();
void hang_err(const char *message);
void HALinit();
void HAL_cleanup();

#endif  // THREAD_H