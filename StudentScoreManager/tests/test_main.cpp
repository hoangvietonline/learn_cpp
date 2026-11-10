// TC-01: load 10 valid students and check their results.
#include "fileio.h"
#include <cmath>
#include <cstdio>
#include <cstring>

// Create a valid student for tests that add records.
static Student makeStudent(std::uint32_t id)
{
    Student student{};
    student.id = id;
    std::snprintf(student.name, sizeof(student.name), "Student %u", id);
    student.scores[0] = 8.0f;
    student.scores[1] = 7.0f;
    student.scores[2] = 9.0f;
    return student;
}

#define CHECK(condition) do { \
    if (!(condition)) { \
        ++failures; \
    } \
} while (0)

// Run TC-01. Return the number of failed checks.
static int testTC01()
{
    int failures = 0;
    Student* arr = nullptr;
    int count = 0;
    int capacity = 0;

    const int loaded = loadCsv("tc01/students.csv", arr, count, capacity);
    CHECK(loaded == 10);
    CHECK(count == 10);

    // Expected values are written separately from the calculation being tested.
    const float averages[10] = {
        8.1666667f, 6.3333333f, 8.8333333f, 5.0f, 4.0f,
        8.0f, 8.5f, 7.0f, 0.0f, 10.0f
    };
    const char ranks[10] = {'B', 'C', 'A', 'C', 'F', 'B', 'A', 'B', 'F', 'A'};
    const char* names[10] = {
        "Nguyen Van An", "Tran Thi Binh", "Le Van Cuong", "Pham Thi Dung",
        "Hoang Van Huy", "Nguyen Dinh Hoang Viet", "Do Thi Lan",
        "Bui Van Minh", "Vu Thi Ngoc", "Dang Van Phuc"
    };

    if (arr != nullptr && count == 10)
    {
        for (int i = 0; i < count; ++i)
        {
            CHECK(arr[i].id == static_cast<std::uint32_t>(1001 + i));
            CHECK(std::strcmp(arr[i].name, names[i]) == 0);
            CHECK(std::fabs(arr[i].average - averages[i]) < 0.0001f);
            CHECK(arr[i].rank == ranks[i]);
        }
    }

    delete[] arr;
    std::puts(failures == 0 ? "PASS TC-01" : "FAIL TC-01");
    return failures;
}

// Run TC-02. Check that a missing CSV leaves an empty list and does not crash.
static int testTC02()
{
    int failures = 0;
    Student* arr = nullptr;
    int count = 0;
    int capacity = 0;

    const int loaded = loadCsv("tc02/students.csv",
                               arr, count, capacity);
    CHECK(loaded == -1);
    CHECK(arr == nullptr);
    CHECK(count == 0);
    CHECK(capacity == 0);

    delete[] arr;
    std::puts(failures == 0 ? "PASS TC-02" : "FAIL TC-02");
    return failures;
}

// Run TC-03. Skip three invalid CSV rows and load the valid row.
static int testTC03()
{
    int failures = 0;
    Student* arr = nullptr;
    int count = 0;
    int capacity = 0;

    const int loaded = loadCsv("students_invalid.csv", arr, count, capacity);
    CHECK(loaded == 1);
    CHECK(count == 1);
    if (arr != nullptr && count == 1)
    {
        CHECK(arr[0].id == 1001);
        CHECK(std::strcmp(arr[0].name, "Nguyen Van An") == 0);
        CHECK(std::fabs(arr[0].average - 8.1666667f) < 0.0001f);
        CHECK(arr[0].rank == 'B');
    }

    delete[] arr;
    std::puts(failures == 0 ? "PASS TC-03" : "FAIL TC-03");
    return failures;
}

// Run TC-04. Check that capacity doubles from 4 to 32 without losing records.
static int testTC04()
{
    int failures = 0;
    Student* arr = nullptr;
    int count = 0;
    int capacity = 0;

    CHECK(loadCsv("empty.csv", arr, count, capacity) == 0);
    CHECK(count == 0);
    for (int i = 0; i < 20; ++i)
    {
        Student student = makeStudent(static_cast<std::uint32_t>(100 + i));
        CHECK(addStudent(arr, count, capacity, student));
        const int expectedCapacity = i < 4 ? 4 : i < 8 ? 8 : i < 16 ? 16 : 32;
        CHECK(capacity == expectedCapacity);
    }

    CHECK(count == 20);
    if (arr != nullptr && count == 20)
    {
        for (int i = 0; i < count; ++i)
            CHECK(arr[i].id == static_cast<std::uint32_t>(100 + i));
    }

    delete[] arr;
    std::puts(failures == 0 ? "PASS TC-04" : "FAIL TC-04");
    return failures;
}

// Run TC-05. Reject a duplicate ID and keep the original student unchanged.
static int testTC05()
{
    int failures = 0;
    Student* arr = nullptr;
    int count = 0;
    int capacity = 0;
    Student original = makeStudent(1001);
    Student duplicate = makeStudent(1001);
    std::strcpy(duplicate.name, "Different Name");
    duplicate.scores[0] = 1.0f;

    CHECK(addStudent(arr, count, capacity, original));
    const int oldCapacity = capacity;
    CHECK(!addStudent(arr, count, capacity, duplicate));
    CHECK(count == 1);
    CHECK(capacity == oldCapacity);
    if (arr != nullptr && count == 1)
    {
        CHECK(arr[0].id == 1001);
        CHECK(std::strcmp(arr[0].name, "Student 1001") == 0);
        CHECK(arr[0].scores[0] == 8.0f);
    }

    delete[] arr;
    std::puts(failures == 0 ? "PASS TC-05" : "FAIL TC-05");
    return failures;
}


// Run TC-06. Reject invalid scores and check a valid score update.
static int testTC06()
{
    int failures = 0;
    Student* arr = nullptr;
    int count = 0, capacity = 0;
    Student student = makeStudent(2001);
    CHECK(addStudent(arr, count, capacity, student));
    CHECK(!updateScore(arr, count, 2001, 0, -1.0f));
    CHECK(!updateScore(arr, count, 2001, 1, 10.5f));
    CHECK(!updateScore(arr, count, 2001, 3, 5.0f));
    CHECK(!updateScore(arr, count, 9999, 0, 5.0f));
    CHECK(arr[0].scores[0] == 8.0f);
    CHECK(updateScore(arr, count, 2001, 0, 10.0f));
    CHECK(arr[0].scores[0] == 10.0f);
    CHECK(std::fabs(arr[0].average - (26.0f / 3.0f)) < 0.0001f);
    CHECK(arr[0].rank == 'A');
    delete[] arr;
    std::puts(failures == 0 ? "PASS TC-06" : "FAIL TC-06");
    return failures;
}

// Run TC-07. Check recursive ID search for existing and missing IDs.
static int testTC07()
{
    int failures = 0;
    Student* arr = nullptr;
    int count = 0, capacity = 0;
    for (int i = 0; i < 7; ++i)
        CHECK(addStudent(arr, count, capacity, makeStudent(static_cast<std::uint32_t>(300 + i * 2))));
    for (int i = 0; i < count; ++i)
        CHECK(findById(arr, 0, count - 1, arr[i].id) == i);
    CHECK(findById(arr, 0, count - 1, 301) == -1);
    CHECK(findById(arr, 0, -1, 300) == -1);
    delete[] arr;
    std::puts(failures == 0 ? "PASS TC-07" : "FAIL TC-07");
    return failures;
}

// Run TC-08. Remove students at the start, middle and end of the sorted array.
static int testTC08()
{
    int failures = 0;
    Student* arr = nullptr;
    int count = 0, capacity = 0;
    for (int i = 1; i <= 5; ++i)
        CHECK(addStudent(arr, count, capacity, makeStudent(static_cast<std::uint32_t>(400 + i))));
    CHECK(removeStudent(arr, count, 401));
    CHECK(removeStudent(arr, count, 403));
    CHECK(removeStudent(arr, count, 405));
    CHECK(!removeStudent(arr, count, 999));
    CHECK(count == 2);
    if (count == 2) CHECK(arr[0].id == 402 && arr[1].id == 404);
    delete[] arr;
    std::puts(failures == 0 ? "PASS TC-08" : "FAIL TC-08");
    return failures;
}

// Run TC-09. Check A/B/C/F thresholds and display class statistics.
static int testTC09()
{
    int failures = 0;
    Student* arr = nullptr;
    int count = 0, capacity = 0;
    const float values[] = {9.0f, 7.0f, 5.0f, 4.0f};
    const char ranks[] = {'A', 'B', 'C', 'F'};
    for (int i = 0; i < 4; ++i)
    {
        Student s = makeStudent(static_cast<std::uint32_t>(500 + i));
        for (int j = 0; j < NUM_SUBJECTS; ++j) s.scores[j] = values[i];
        CHECK(addStudent(arr, count, capacity, s));
        CHECK(arr[i].average == values[i] && arr[i].rank == ranks[i]);
    }
    delete[] arr;
    std::puts(failures == 0 ? "PASS TC-09" : "FAIL TC-09");
    return failures;
}

// Run TC-10. Sort a pointer view both ways without changing the original array.
static int testTC10()
{
    int failures = 0;
    Student students[] = {makeStudent(601), makeStudent(602), makeStudent(603), makeStudent(604)};
    std::strcpy(students[0].name, "Charlie Name");
    std::strcpy(students[1].name, "Alpha Name");
    std::strcpy(students[2].name, "Delta Name");
    std::strcpy(students[3].name, "Bravo Name");
    students[0].average = 6; students[1].average = 9;
    students[2].average = 7; students[3].average = 8;
    const Student* view[] = {&students[0], &students[1], &students[2], &students[3]};
    sortView(view, 4, SortKey::AverageDescending);
    CHECK(view[0]->id == 602 && view[1]->id == 604 && view[2]->id == 603 && view[3]->id == 601);
    sortView(view, 4, SortKey::NameAscending);
    CHECK(view[0]->id == 602 && view[1]->id == 604 && view[2]->id == 601 && view[3]->id == 603);
    for (int i = 0; i < 4; ++i) CHECK(students[i].id == static_cast<std::uint32_t>(601 + i));
    std::puts(failures == 0 ? "PASS TC-10" : "FAIL TC-10");
    return failures;
}

// Run TC-11. Save CSV, reload it and compare all student fields.
static int testTC11()
{
    int failures = 0;
    const char* path = "tc11_roundtrip.csv";
    Student *original = nullptr, *loaded = nullptr;
    int count = 0, capacity = 0, loadedCount = 0, loadedCapacity = 0;
    CHECK(loadCsv("tc01/students.csv", original, count, capacity) == 10);
    CHECK(saveCsv(path, original, count));
    CHECK(loadCsv(path, loaded, loadedCount, loadedCapacity) == count);
    CHECK(loadedCount == count);
    if (original && loaded && count == 10 && loadedCount == 10)
        for (int i = 0; i < count; ++i)
        {
            CHECK(original[i].id == loaded[i].id);
            CHECK(std::strcmp(original[i].name, loaded[i].name) == 0);
            CHECK(original[i].average == loaded[i].average && original[i].rank == loaded[i].rank);
            for (int j = 0; j < NUM_SUBJECTS; ++j) CHECK(original[i].scores[j] == loaded[i].scores[j]);
        }
    std::remove(path);
    delete[] original; delete[] loaded;
    std::puts(failures == 0 ? "PASS TC-11" : "FAIL TC-11");
    return failures;
}

// Run TC-12. Restore a binary backup after changing the current list.
static int testTC12()
{
    int failures = 0;
    const char* path = "tc12_backup.bin";
    Student* arr = nullptr;
    int count = 0, capacity = 0;
    CHECK(loadCsv("tc01/students.csv", arr, count, capacity) == 10);
    CHECK(saveBinary(path, arr, count));
    CHECK(removeStudent(arr, count, 1001));
    CHECK(count == 9);
    CHECK(loadBinary(path, arr, count, capacity) == 10);
    CHECK(count == 10);
    if (arr && count == 10)
    {
        CHECK(arr[0].id == 1001 && std::strcmp(arr[0].name, "Nguyen Van An") == 0);
        CHECK(std::fabs(arr[0].average - 8.1666667f) < 0.0001f && arr[0].rank == 'B');
    }
    std::remove(path);
    delete[] arr;
    std::puts(failures == 0 ? "PASS TC-12" : "FAIL TC-12");
    return failures;
}

// Run TC-13. Check that empty CSV data is safe for list operations.
static int testTC13()
{
    int failures = 0;
    Student* arr = nullptr;
    int count = 0, capacity = 0;
    CHECK(loadCsv("empty.csv", arr, count, capacity) == 0);
    CHECK(count == 0);
    CHECK(findById(arr, 0, count - 1, 1) == -1);
    delete[] arr;
    std::puts(failures == 0 ? "PASS TC-13" : "FAIL TC-13");
    return failures;
}

// Run TC-14. Allocate, use and release the dynamic student array.
static int testTC14()
{
    int failures = 0;
    Student* arr = nullptr;
    int count = 0, capacity = 0;
    CHECK(loadCsv("tc01/students.csv", arr, count, capacity) == 10);
    CHECK(updateScore(arr, count, 1001, 0, 9.0f));
    delete[] arr;
    std::puts(failures == 0 ? "PASS TC-14" : "FAIL TC-14");
    return failures;
}

// Run all test functions and return 0 if they pass.
int main()
{
    const int failures = testTC01() + testTC02() + testTC03() + testTC04() + testTC05() +
                         testTC06() + testTC07() + testTC08() + testTC09() + testTC10() +
                         testTC11() + testTC12() + testTC13() + testTC14();
    if (failures > 0)
        std::fprintf(stderr, "Tests failed with %d error(s).\n", failures);
    return failures == 0 ? 0 : 1;
}
