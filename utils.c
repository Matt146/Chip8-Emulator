#include "utils.h"

int memory_size = 4096;
int max_file_size = 65536;
int max_program_size = 1024;
int x_window_scale = 10;
int y_window_scale = 10;

unsigned char* read_file_binary(const char* fname) {
    // Open the file. If an error occurs,
    // just log it and return NULL.
    Log("Opening and reading binary file...", 0);
    FILE* fp = fopen(fname, "r");
    if (fp == NULL) {
        Log("Unable to open file!", 3);
        return NULL;
    }

    // Allocate a buffer in memory and read the file
    unsigned char* buff = (unsigned char*)malloc(sizeof(unsigned char) * max_file_size);
    buff[max_file_size-1] = '\0';
    size_t buff_size = fread(buff, sizeof(unsigned char), max_file_size, fp);
    if (buff_size < max_file_size) {
        buff[buff_size] = '\0';
    }
    fclose(fp);

    Log("Successfully opened and read binary file", 0);

    // Now return the buffer
    return buff;
}

char* read_file(const char* fname) {
    // Open the file. If an error occurs,
    // just log it and return NULL.
    Log("Opening and reading binary file...", 0);
    FILE* fp = fopen(fname, "r");
    if (fp == NULL) {
        Log("Unable to open file!", 3);
        return NULL;
    }

    // Allocate a buffer in memory and read the file
    char* buff = (char*)malloc(sizeof(char) * max_file_size);
    buff[max_file_size-1] = '\0';
    size_t buff_size = fread(buff, sizeof(char), max_file_size, fp);
    if (buff_size < max_file_size) {
        buff[buff_size] = '\0';
    }
    fclose(fp);

    Log("Successfully opened and read binary file", 0);

    // Now return the buffer
    return buff;
}