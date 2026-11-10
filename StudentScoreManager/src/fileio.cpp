// Read and write CSV files and binary backups.
#include "fileio.h"
#include <cerrno>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>

// Remove spaces at both ends of text in place. Return the trimmed text pointer.
static char* trim(char* text)
{
    while (std::isspace(static_cast<unsigned char>(*text)))
        ++text;
    char* end = text + std::strlen(text);
    while (end > text && std::isspace(static_cast<unsigned char>(end[-1])))
        --end;
    *end = '\0';
    return text;
}

// Check the basic fields of s before file I/O. Return true if valid.
static bool validFileStudent(const Student& s)
{
    if (s.id == 0 || s.name[0] == '\0' ||
        std::memchr(s.name, '\0', MAX_NAME_LEN) == nullptr)
        return false;
    for (int i = 0; i < NUM_SUBJECTS; ++i)
        if (!std::isfinite(s.scores[i]) || s.scores[i] < 0 || s.scores[i] > 10)
            return false;
    bool hasName = false;
    for (const char* ch = s.name; *ch != '\0'; ++ch)
        if (!std::isspace(static_cast<unsigned char>(*ch)))
            hasName = true;
    return hasName;
}

// Parse a CSV line into s. Require five fields with a valid ID, name and scores.
// Modify line and return false for invalid data. Quoted CSV fields are not supported.
static bool parseStudent(char* line, Student& s)
{
    char* fields[2 + NUM_SUBJECTS];
    fields[0] = line;
    for (int i = 1; i < 2 + NUM_SUBJECTS; ++i)
    {
        char* comma = std::strchr(fields[i - 1], ',');
        if (comma == nullptr)
            return false;
        *comma = '\0';
        fields[i] = comma + 1;
    }
    if (std::strchr(fields[1 + NUM_SUBJECTS], ',') != nullptr)
        return false;
    for (int i = 0; i < 2 + NUM_SUBJECTS; ++i)
    {
        if (i != 1) // Keep the name unchanged. Trim only number fields.
            fields[i] = trim(fields[i]);
    }

    // Allow only decimal digits in the ID. Reject signs and decimal points.
    if (*fields[0] == '\0')
        return false;
    for (const char* ch = fields[0]; *ch != '\0'; ++ch)
        if (!std::isdigit(static_cast<unsigned char>(*ch)))
            return false;
    errno = 0;
    char* end = nullptr;
    const unsigned long long id = std::strtoull(fields[0], &end, 10);
    if (errno == ERANGE || id == 0 || id > std::numeric_limits<std::uint32_t>::max())
        return false;
    s.id = static_cast<std::uint32_t>(id);

    const std::size_t length = std::strlen(fields[1]);
    if (length == 0 || length >= MAX_NAME_LEN)
        return false;
    std::memcpy(s.name, fields[1], length + 1);

    for (int i = 0; i < NUM_SUBJECTS; ++i)
    {
        errno = 0;
        const float score = std::strtof(fields[i + 2], &end);
        if (end == fields[i + 2] || *end != '\0' || errno == ERANGE)
            return false;
        s.scores[i] = score;
    }
    return validFileStudent(s);
}

// Load CSV from path into arr and update count/capacity. Return the number added, or -1 on error.
int loadCsv(const char* path, Student*& arr, int& count, int& capacity)
{
    TRACE("loadCsv");
    if (path == nullptr || *path == '\0' || count < 0 || capacity < count ||
        (arr == nullptr && capacity != 0))
        return -1;

    std::FILE* file = std::fopen(path, "r");
    if (file == nullptr)
    {
        std::fprintf(stderr, "Cannot open CSV: %s\n", path);
        return -1;
    }

    char line[1024];
    std::size_t lineNumber = 0;
    int added = 0;
    bool success = true;
    while (std::fgets(line, sizeof(line), file) != nullptr)
    {
        ++lineNumber;
        if (std::strchr(line, '\n') == nullptr)
        {
            // Skip the rest of a long line so it is not read as a new CSV row.
            int ch = std::fgetc(file);
            if (ch != EOF && ch != '\n')
            {
                do { ch = std::fgetc(file); } while (ch != EOF && ch != '\n');
                std::fprintf(stderr, "%s: line %zu: line is too long; skipped.\n", path, lineNumber);
                continue;
            }
        }

        char* text = line;
        while (std::isspace(static_cast<unsigned char>(*text)))
            ++text;
        if (*text == '\0' || *text == '#')
            continue;

        Student s{};
        if (!parseStudent(line, s))
        {
            std::fprintf(stderr, "%s: line %zu: invalid data; skipped.\n", path, lineNumber);
            continue;
        }
        if (findById(arr, 0, count - 1, s.id) != -1)
        {
            std::fprintf(stderr, "%s: line %zu: duplicate ID; skipped.\n", path, lineNumber);
            continue;
        }
        if (!addStudent(arr, count, capacity, s))
        {
            std::fprintf(stderr, "%s: line %zu: cannot add student.\n", path, lineNumber);
            success = false;
            break;
        }
        ++added;
    }

    if (std::ferror(file))
        success = false;
    if (std::fclose(file) != 0)
        success = false;
    if (!success)
    {
        std::fprintf(stderr, "Could not finish loading CSV: %s\n", path);
        return -1;
    }
    return added;
}

// Save count students from arr to CSV at path. Return false on invalid data or a file error.
bool saveCsv(const char* path, const Student* arr, int count)
{
    TRACE("saveCsv");
    if (path == nullptr || *path == '\0' || count < 0 || (arr == nullptr && count > 0))
        return false;

    // This simple CSV format does not support commas or line breaks in names.
    for (int i = 0; i < count; ++i)
        if (!validFileStudent(arr[i]) || std::strpbrk(arr[i].name, ",\r\n") != nullptr)
            return false;

    std::FILE* file = std::fopen(path, "w");
    if (file == nullptr)
    {
        std::fprintf(stderr, "Cannot open CSV for writing: %s\n", path);
        return false;
    }

    bool success = std::fprintf(file, "# id,name,cpp,mcu,english\n") >= 0;
    for (int i = 0; i < count && success; ++i)
    {
        // Nine significant digits preserve float scores when read back.
        success = std::fprintf(file, "%llu,%s,%.9g,%.9g,%.9g\n",
            static_cast<unsigned long long>(arr[i].id), arr[i].name,
            arr[i].scores[0], arr[i].scores[1], arr[i].scores[2]) >= 0;
    }
    if (std::fclose(file) != 0)
        success = false;
    if (!success)
        std::fprintf(stderr, "Error writing or closing CSV: %s\n", path);
    return success;
}

// Write count, then arr, to the binary file at path. Return true if writing and closing succeed.
// Use this simple backup format with the same program and platform.
bool saveBinary(const char* path, const Student* arr, int count)
{
    TRACE("saveBinary");
    if (path == nullptr || *path == '\0' || count < 0 || (arr == nullptr && count > 0))
        return false;
    for (int i = 0; i < count; ++i)
        if (!validFileStudent(arr[i]))
            return false;

    std::FILE* file = std::fopen(path, "wb");
    if (file == nullptr)
    {
        std::fprintf(stderr, "Cannot open binary for writing: %s\n", path);
        return false;
    }

    bool success = std::fwrite(&count, sizeof(count), 1, file) == 1;
    if (success && count > 0)
        success = std::fwrite(arr, sizeof(Student), count, file) == static_cast<std::size_t>(count);
    if (std::fclose(file) != 0)
        success = false;
    if (!success)
        std::fprintf(stderr, "Error writing or closing binary: %s\n", path);
    return success;
}

// Load the binary file at path. Replace arr/count/capacity only on success.
// Return the number loaded, or -1 on error. Keep the old list if loading fails.
int loadBinary(const char* path, Student*& arr, int& count, int& capacity)
{
    TRACE("loadBinary");
    if (path == nullptr || *path == '\0' || count < 0 || capacity < count ||
        (arr == nullptr && capacity != 0))
        return -1;

    std::FILE* file = std::fopen(path, "rb");
    if (file == nullptr)
    {
        std::fprintf(stderr, "Cannot open binary: %s\n", path);
        return -1;
    }

    // The file starts with the number of saved students.
    int fileCount = 0;
    bool success = std::fread(&fileCount, sizeof(fileCount), 1, file) == 1;
    if (fileCount < 0)
        success = false;

    Student* newArray = nullptr;
    int newCount = 0;
    int newCapacity = 0;
    for (int i = 0; i < fileCount && success; ++i)
    {
        Student s{};
        if (std::fread(&s, sizeof(s), 1, file) != 1 || !validFileStudent(s))
        {
            success = false;
            break;
        }

        // addStudent checks duplicate IDs, grows the array and calculates average/rank.
        success = addStudent(newArray, newCount, newCapacity, s);
    }

    // The file must contain exactly the number of students given in its header.
    if (success && std::fgetc(file) != EOF)
        success = false;
    if (std::ferror(file))
        success = false;
    if (std::fclose(file) != 0)
        success = false;

    if (!success)
    {
        delete[] newArray;
        std::fprintf(stderr, "Cannot load a valid backup: %s\n", path);
        return -1;
    }

    // Free the old array only after the whole file is loaded successfully.
    delete[] arr;
    arr = newArray;
    count = newCount;
    capacity = newCapacity;
    return count;
}
