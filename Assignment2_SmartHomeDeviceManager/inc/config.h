#ifndef CONFIG_H
#define CONFIG_H

// Build with -DTRACE_LIFECYCLE to enable constructor/destructor logs.
#ifdef TRACE_LIFECYCLE
#include <iostream>
#define TRACE(message) do { std::cout << message << '\n'; } while (0)
#else
#define TRACE(message) do { } while (0)
#endif

#endif // CONFIG_H
