#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

/* Entry types:
 * - 0: info
 * - 1: warning
 * - 2: error (recoverable)
 * - 3: error (fatal)
*/
void Log(const char* entry, int entry_type);

#endif // LOGGER_H