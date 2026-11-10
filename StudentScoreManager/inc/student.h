#ifndef STUDENT_H
#define STUDENT_H

#include <cstdint>
#include "config.h"

// Store one student and their results.
struct Student
{
    std::uint32_t id;
    char name[MAX_NAME_LEN];
    float scores[NUM_SUBJECTS];
    float average;
    char rank;
};

enum class SortKey
{
    AverageDescending,
    NameAscending
};

// Add s in ascending ID order. Update arr/count/capacity when the array grows.
// Return false if the data is invalid or the ID already exists.
bool addStudent(Student*& arr, int& count, int& capacity, const Student& s);
// Remove a student by ID and keep the order. Return false if not found.
bool removeStudent(Student* arr, int& count, std::uint32_t id);
// Update the score for subject (0..NUM_SUBJECTS-1) and recalculate average/rank.
// Return false if the ID, subject or score is invalid.
bool updateScore(Student* arr, int count, std::uint32_t id, int subject, float score);

// Use recursive binary search in [low, high]. The array must be sorted by ID.
// Return the index, or -1 if not found. low > high means an empty range.
int findById(const Student* arr, int low, int high, std::uint32_t id);
// Return the average of n scores. scores must point to at least n elements, with n > 0.
float calcAverage(const float* scores, int n = NUM_SUBJECTS);
// Print one student. No return value.
void printStudent(const Student& s);

// Print count students. arr may be nullptr when count is 0. No return value.
void printStudent(const Student* arr, int count);
// Swap all data of two students. No return value.
void swapStudent(Student& a, Student& b);

// Sort count pointers by key without changing student data. No return value.
// view may be nullptr when count is 0. Free view before changing arr.
void sortView(const Student** view, int count, SortKey key);

// Print the class average, highest/lowest averages and rank counts for count students. No return value.
void printStatistics(const Student* arr, int count);
// Create a heap view, sort, print and free it. Return false on invalid input or allocation failure.
bool printSortedStudents(const Student* arr, int count, SortKey key);

#endif // STUDENT_H
