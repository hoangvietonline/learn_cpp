#ifndef FILEIO_H
#define FILEIO_H

#include "student.h"

// Add valid CSV rows to arr. Skip invalid rows and report their line numbers to stderr.
// Return the number added, or -1 on error. Keep students already added if a later error occurs.
int loadCsv(const char* path, Student*& arr, int& count, int& capacity);
// Save ID, name and three scores to CSV. Return true on success. Names cannot contain commas or line breaks.
bool saveCsv(const char* path, const Student* arr, int count);
// Write count (int), then the Student array, as a binary backup for the same platform. Return true on success.
bool saveBinary(const char* path, const Student* arr, int count);
// Replace the list after loading binary successfully. Keep the old list on error.
// Return the number loaded, or -1 on error.
// Recalculate average and rank from scores.
int loadBinary(const char* path, Student*& arr, int& count, int& capacity);

#endif // FILEIO_H
