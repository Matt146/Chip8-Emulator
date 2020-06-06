#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include "logger.h"

// global variable declaration
extern int memory_size;
extern int max_file_size;
extern int max_program_size;
extern int x_window_scale;
extern int y_window_scale;

// utility functions (should be accessible to everything)
unsigned char* read_file_binary(const char* fname);
char* read_file(const char* fname);

#endif // UTILS_H