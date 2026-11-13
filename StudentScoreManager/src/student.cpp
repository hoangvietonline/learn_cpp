// Student operations, searching and sorting.
#include "student.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <limits>
#include <new>

static inline char rankFromAverage(float average)
{
    if (average >= 8.5f)
        return 'A';
    if (average >= 7.0f)
        return 'B';
    if (average >= 5.0f)
        return 'C';
    return 'F';
}

static bool isValidStudent(const Student &s)
{
    if (s.id == 0 || s.name[0] == '\0' ||
        std::memchr(s.name, '\0', MAX_NAME_LEN) == nullptr)
        return false;

    for (const float *score = s.scores; score != s.scores + NUM_SUBJECTS; ++score)
    {
        if (!std::isfinite(*score) || *score < 0.0f || *score > 10.0f)
            return false;
    }
    return true;
}

bool addStudent(Student *&arr, int &count, int &capacity, const Student &s)
{
    TRACE("addStudent");
    if (count < 0 || capacity < 0 || count > capacity ||
        (arr == nullptr && capacity != 0) || !isValidStudent(s))
        return false;

    for (int i = 0; i < count; ++i)
    {
        if (arr[i].id == s.id)
        {
            return false;
        }
    }

    Student newStudent = s;
    newStudent.average = calcAverage(newStudent.scores);
    newStudent.rank = rankFromAverage(newStudent.average);

    if (count == capacity)
    {
        if (capacity > std::numeric_limits<int>::max() / 2)
            return false;

        int newCapacity = capacity == 0 ? INITIAL_CAPACITY : capacity * 2;
        Student *newArray = new (std::nothrow) Student[newCapacity];
        if (newArray == nullptr)
            return false;

        for (int i = 0; i < count; ++i)
        {
            newArray[i] = arr[i];
        }

        delete[] arr;
        arr = newArray;
        capacity = newCapacity;
    }

    int position = count;
    while (position > 0 && arr[position - 1].id > newStudent.id)
    {
        arr[position] = arr[position - 1];
        --position;
    }

    arr[position] = newStudent;
    ++count;
    return true;
}

bool removeStudent(Student *arr, int &count, std::uint32_t id)
{
    TRACE("removeStudent");
    if (arr == nullptr || count <= 0 || id == 0)
        return false;

    int positionRemove = -1;

    for (int i = 0; i < count; ++i)
    {
        if (arr[i].id == id)
        {
            positionRemove = i;
            break;
        }
    }

    if (positionRemove > -1)
    {
        for (int i = positionRemove; i < count - 1; ++i)
        {
            arr[i] = arr[i + 1];
        }

        --count;
        return true;
    }
    else
    {
        return false;
    }
}

void printStudent(const Student *arr, int count)
{
    if (arr == nullptr || count <= 0)
    {
        std::cout << "The student list is empty.\n";
        return;
    }

    for (int i = 0; i < count; ++i)
    {
        printStudent(arr[i]);
    }
}

void printStudent(const Student &s)
{
    std::cout << "ID: " << s.id << ", ";
    std::cout << "Name: " << s.name << ", ";
    std::cout << "C++: " << s.scores[0] << ", ";
    std::cout << "MCU: " << s.scores[1] << ", ";
    std::cout << "English: " << s.scores[2];
    std::cout << ", Average: " << std::fixed << std::setprecision(2)
              << s.average << ", Rank: " << s.rank << std::endl;
}

void swapStudent(Student &a, Student &b)
{
    Student temp = a;
    a = b;
    b = temp;
}

bool updateScore(Student *arr, int count, std::uint32_t id, int subject, float score)
{
    TRACE("updateScore");
    if (arr == nullptr || count <= 0 || id == 0 ||
        subject < 0 || subject >= NUM_SUBJECTS ||
        !std::isfinite(score) || score < 0.0f || score > 10.0f)
    {
        return false;
    }

    for (int i = 0; i < count; ++i)
    {
        if (arr[i].id == id)
        {
            arr[i].scores[subject] = score;
            arr[i].average = calcAverage(arr[i].scores);

            arr[i].rank = rankFromAverage(arr[i].average);

            return true;
        }
    }

    return false;
}

int findById(const Student *arr, int low, int high, std::uint32_t id)
{
    TRACE("findById");
    if (arr == nullptr || low < 0 || low > high)
        return -1;

    int mid = low + (high - low) / 2;

    if (arr[mid].id == id)
        return mid;

    if (id < arr[mid].id)
    {
        return findById(arr, low, mid - 1, id);
    }

    return findById(arr, mid + 1, high, id);
}

float calcAverage(const float *scores, int n)
{
    if (scores == nullptr || n <= 0)
    {
        return 0.0f;
    }

    float total = 0;

    for(const float* score = scores; score != scores+ n; ++score ){
        total += *score;
    }

    return total / n;
}

static bool compareAverage(const Student *a, const Student *b)
{
    return a->average > b->average;
}

// Split a name into surname, middle name and given name.
// Assume words are separated by one space, with no spaces at either end.
static void splitName(char* name, const char*& surname,
                      const char*& middle, const char*& given)
{
    surname = "";
    middle = "";
    given = name;

    char* lastSpace = std::strrchr(name, ' ');
    if (lastSpace == nullptr)
        return; // Only one word

    *lastSpace = '\0';
    given = lastSpace + 1;
    surname = name;

    char* firstSpace = std::strchr(name, ' ');
    if (firstSpace != nullptr)
    {
        *firstSpace = '\0';
        middle = firstSpace + 1;
    }
}

static bool compareName(const Student* a, const Student* b)
{
    char nameA[MAX_NAME_LEN];
    char nameB[MAX_NAME_LEN];

    std::strcpy(nameA, a->name);
    std::strcpy(nameB, b->name);

    const char *surnameA, *middleA, *givenA;
    const char *surnameB, *middleB, *givenB;

    splitName(nameA, surnameA, middleA, givenA);
    splitName(nameB, surnameB, middleB, givenB);

    int result = std::strcmp(givenA, givenB);
    if (result != 0)
        return result < 0;

    result = std::strcmp(middleA, middleB);
    if (result != 0)
        return result < 0;

    return std::strcmp(surnameA, surnameB) < 0;
}

void sortView(const Student **view, int count, SortKey key)
{
    TRACE("sortView");
    if (view == nullptr || count < 2)
    {
        return;
    }

    if (key == SortKey::AverageDescending)
    {
        std::sort(view, view + count, compareAverage);
    }
    else if (key == SortKey::NameAscending)
    {
        std::sort(view, view + count, compareName);
    }
}

void printStatistics(const Student *arr, int count)
{
    TRACE("printStatistics");
    if (arr == nullptr || count <= 0)
    {
        std::cout << "The student list is empty.\n";
        return;
    }

    double total = 0.0;
    float lowest = arr[0].average;
    float highest = arr[0].average;
    int ranks[4] = {}; // A, B, C, F
    for (const Student *student = arr; student != arr + count; ++student)
    {
        total += student->average;
        if (student->average < lowest)
            lowest = student->average;
        if (student->average > highest)
            highest = student->average;
        switch (rankFromAverage(student->average))
        {
        case 'A':
            ++ranks[0];
            break;
        case 'B':
            ++ranks[1];
            break;
        case 'C':
            ++ranks[2];
            break;
        default:
            ++ranks[3];
            break;
        }
    }

    std::cout << std::fixed << std::setprecision(2)
              << "Class average: " << total / count << '\n'
              << "Highest average: " << highest << '\n'
              << "Lowest average: " << lowest << '\n'
              << "A: " << ranks[0] << ", B: " << ranks[1]
              << ", C: " << ranks[2] << ", F: " << ranks[3] << std::endl;
}

bool printSortedStudents(const Student *arr, int count, SortKey key)
{
    TRACE("printSortedStudents");
    if (count < 0 || (arr == nullptr && count > 0) ||
        (key != SortKey::AverageDescending && key != SortKey::NameAscending))
        return false;
    if (count == 0)
    {
        printStudent(arr, count);
        return true;
    }

    const Student **view = new (std::nothrow) const Student *[count];
    if (view == nullptr)
    {
        return false;
    }

    for (size_t i = 0; i < count; ++i)
    {
        view[i] = &arr[i];
    }
    sortView(view, count, key);
    for (int i = 0; i < count; ++i)
        printStudent(*view[i]);

    delete[] view;
    return true;
}
