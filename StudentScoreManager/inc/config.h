// Shared constants and macros for Student Score Manager.
#ifndef CONFIG_H
#define CONFIG_H

const int MAX_NAME_LEN = 32;
const int NUM_SUBJECTS = 3;
const int INITIAL_CAPACITY = 4;

// Function macro: put parentheses around the argument and the whole expression.
#define SQUARE(value) ((value) * (value))

#ifdef DEBUG_LOG
#define TRACE(message) do { std::fprintf(stderr, "[%s:%d] %s\n", __func__, __LINE__, (message)); } while (0)
#else
#define TRACE(message) do { } while (0)
#endif

#endif // CONFIG_H
