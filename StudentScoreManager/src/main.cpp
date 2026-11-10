// Student menu: load CSV at startup and save or load files from the menu.
#include "student.h"
#include "fileio.h"
#include <cerrno>
#include <cctype>
#include <cmath>
#include <iostream>
#include <limits>

// Show menu options. No parameters or return value.
static void showMenu()
{
    std::cout << "\n===== STUDENT SCORE MANAGER =====\n"
              << "1. List all\n"
              << "2. Add a student\n"
              << "3. Update a score\n"
              << "4. Remove a student\n"
              << "5. Find by ID\n"
              << "6. View statistics\n"
              << "7. View sorted students\n"
              << "8. Save CSV\n"
              << "9. Save/load binary backup\n"
              << "0. Exit\n";
}

// Read a line into buffer (size characters). Retry long input; return false on EOF or a stream error.
static bool readLine(const char *prompt, char *buffer, int size)
{
    while (true)
    {
        std::cout << prompt;
        if (std::cin.getline(buffer, size))
            return true;
        if (std::cin.eof() || std::cin.bad())
            return false;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Input is too long. Please try again.\n";
    }
}

// Read a finite number in [min, max] into value. If integer is true, require a whole number. Return false when input ends.
static bool readNumber(const char *prompt, double min, double max,
                       bool integer, double &value)
{
    char line[256];
    while (readLine(prompt, line, sizeof(line)))
    {
        char *end = nullptr;
        errno = 0;
        const double parsed = std::strtod(line, &end);
        if (end == line)
        {
            std::cout << "Please enter a valid number.\n";
            continue;
        }
        while (std::isspace(static_cast<unsigned char>(*end)))
            ++end;
        if (errno == ERANGE || *end != '\0' || !std::isfinite(parsed) ||
            parsed < min || parsed > max || (integer && std::trunc(parsed) != parsed))
        {
            std::cout << "Invalid value. Enter "
                      << (integer ? "an integer" : "a number")
                      << " in [" << min << ", " << max << "].\n";
            continue;
        }
        value = parsed;
        return true;
    }
    return false;
}

// Read a positive ID into id. Return false when input ends.
static bool readId(std::uint32_t &id)
{
    double value;
    if (!readNumber("Enter ID: ", 1, std::numeric_limits<std::uint32_t>::max(), true, value))
        return false;
    id = static_cast<std::uint32_t>(value);
    return true;
}

// Read ID, name and three scores into s. Return false on EOF. addStudent calculates average and rank.
static bool inputStudent(Student &s)
{
    if (!readId(s.id))
        return false;
    while (true)
    {
        if (!readLine("Enter name (up to 31 bytes): ", s.name, MAX_NAME_LEN))
            return false;
        bool hasText = false;
        for (const char *ch = s.name; *ch != '\0'; ++ch)
            if (!std::isspace(static_cast<unsigned char>(*ch)))
                hasText = true;
        if (hasText)
            break;
        std::cout << "Name cannot be empty.\n";
    }

    const char *prompts[NUM_SUBJECTS] = {"C++ score: ", "MCU score: ", "English score: "};
    for (int i = 0; i < NUM_SUBJECTS; ++i)
    {
        double value;
        if (!readNumber(prompts[i], 0, 10, false, value))
            return false;
        s.scores[i] = static_cast<float>(value);
    }
    return true;
}

// Use argv[1] as the CSV path (default: students.csv). Run the menu and return 0 on exit.
int main(int argc, char* argv[])
{
    const char* csvPath = argc > 1 ? argv[1] : "students.csv";
    const char* binaryPath = "students.bin";
    Student *students = nullptr;
    int count = 0;
    int capacity = 0;
    bool running = true;
    int loaded = loadCsv(csvPath, students, count, capacity);
    if (loaded >= 0)
        std::cout << "Loaded " << loaded << " students from " << csvPath << ".\n";
    else
        std::cout << "Could not finish loading CSV; continuing with " << count << " students in the current list.\n";

    while (running)
    {
        showMenu();
        double choice;
        if (!readNumber("Choose an option: ", 0, 9, true, choice))
            break;

        switch (static_cast<int>(choice))
        {
        case 0:
            running = false;
            break;
        case 1:
            printStudent(students, count);
            break;
        case 2:
        {
            Student s{};
            if (!inputStudent(s))
            {
                running = false;
                break;
            }
            std::cout << (addStudent(students, count, capacity, s)
                              ? "Student added.\n"
                              : "Cannot add student: duplicate ID, invalid data or not enough memory.\n");
            break;
        }
        case 3:
        {
            std::uint32_t id;
            if (!readId(id))
            {
                running = false;
                break;
            }
            if (findById(students, 0, count - 1, id) == -1)
            {
                std::cout << "Student not found.\n";
                break;
            }
            double subject, score;
            if (!readNumber("Subject (1 C++, 2 MCU, 3 English): ", 1, NUM_SUBJECTS, true, subject) ||
                !readNumber("New score: ", 0, 10, false, score))
            {
                running = false;
                break;
            }
            std::cout << (updateScore(students, count, id, static_cast<int>(subject) - 1,
                                      static_cast<float>(score))
                              ? "Score updated.\n"
                              : "Cannot update the score.\n");
            break;
        }
        case 4:
        {
            std::uint32_t id;
            if (!readId(id))
            {
                running = false;
                break;
            }
            std::cout << (removeStudent(students, count, id)
                              ? "Student removed.\n"
                              : "Student not found.\n");
            break;
        }
        case 5:
        {
            std::uint32_t id;
            if (!readId(id))
            {
                running = false;
                break;
            }
            const int position = findById(students, 0, count - 1, id);
            if (position == -1)
                std::cout << "Student not found.\n";
            else
                printStudent(students[position]);
            break;
        }
        case 6:
            printStatistics(students, count);
            break;
        case 7:
        {
            double order;
            if (!readNumber("Sort by (1 average descending, 2 name ascending): ", 1, 2, true, order))
            {
                running = false;
                break;
            }
            const SortKey key = order == 1 ? SortKey::AverageDescending : SortKey::NameAscending;
            if (!printSortedStudents(students, count, key))
                std::cout << "Cannot create the sorted view.\n";
            break;
        }
        case 8:
            std::cout << (saveCsv(csvPath, students, count)
                              ? "CSV saved: " : "Cannot save CSV: ")
                      << csvPath << '\n';
            break;
        case 9:
        {
            double action;
            if (!readNumber("Binary (1 save, 2 load and replace the current list, 0 back): ",
                            0, 2, true, action))
            {
                running = false;
                break;
            }
            if (action == 1)
            {
                std::cout << (saveBinary(binaryPath, students, count)
                                  ? "Binary saved: " : "Cannot save binary: ")
                          << binaryPath << '\n';
            }
            else if (action == 2)
            {
                loaded = loadBinary(binaryPath, students, count, capacity);
                if (loaded >= 0)
                    std::cout << "Loaded " << loaded << " students from " << binaryPath << ".\n";
                else
                    std::cout << "Cannot load binary. The current list is unchanged.\n";
            }
            break;
        }
        }
    }

    delete[] students;
    std::cout << "Exited. Unsaved changes will be lost.\n";
    return 0;
}
