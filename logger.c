#include "logger.h"

void Log(const char* entry, int entry_type) {
    switch (entry_type) {
        case 0:
            printf("[Info] %s\n", entry);
            break;
        case 1:
            printf("[Warning] %s\n", entry);
            break;
        case 2:
            printf("[Error] %s\n", entry);
            break;
        case 3:
            printf("[Fatal] %s\n", entry);
            break;
        default:
            printf("[Info] %s\n", entry);
    }
}