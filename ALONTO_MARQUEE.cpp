#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <stdexcept>
#ifdef _WIN32
#include <windows.h>
#endif

std::mutex coutMutex;                     // guards terminal output
std::mutex textMutex;                     // guards marqueeText
std::string marqueeText = "";             // default, changeable with "set_text"
std::atomic<bool> marqueeRunning(false);  // start/stop flag
std::thread marqueeThread;
size_t offset = 0; 

const int WIDTH = 30;                     // visible characters in the marquee
std::atomic<int> FRAME_MS(150);           // refresh delay in ms, changeable with "set_speed"


std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

void enableAnsi() {
#ifdef _WIN32
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (h != INVALID_HANDLE_VALUE && GetConsoleMode(h, &mode))
        SetConsoleMode(h, mode | 0x0004);  // enable ANSI on Windows
#endif
}

// Thread-safe print: lock_guard (RAII) holds coutMutex so threads don't interleave output.
// Ref: https://en.cppreference.com/w/cpp/thread/lock_guard
// Ref: https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#cp-concurrency
void say(const std::string& s) {
    std::lock_guard<std::mutex> lock(coutMutex);
    std::cout << s << std::flush;
}

// Clear screen, lock row 1 for the marquee, put cursor on row 2
void resetScreen() {
    say("\033[2J\033[2;r\033[2;1H");
}

void printHeader() {
    say("CSOPESY Section: S05\nGroup Developer: Alonto, Azzam\nVersion Date: 09/28/2026\n\n");
}

// Draw a string on row 1 without moving the user's cursor.
// Uses ANSI escape sequences (cursor save/restore, cursor position, erase line).
// Reference: https://en.wikipedia.org/wiki/ANSI_escape_code
// Standard: ECMA-48, https://ecma-international.org/publications-and-standards/standards/ecma-48/
void drawMarquee(const std::string& s) {
    std::lock_guard<std::mutex> lock(coutMutex);
    std::cout << "\0337" << "\033[1;1H" << "\033[2K" << s << "\0338" << std::flush;
}

void marqueeLoop() {
    while (marqueeRunning) {
        std::string text;
        {
            std::lock_guard<std::mutex> lock(textMutex);
            text = marqueeText;
        }
        std::string padded = text + "   ";  // gap before it wraps around
        std::string window;
        for (int i = 0; i < WIDTH; ++i)
            window += padded[(offset + i) % padded.size()];
        offset = (offset + 1) % padded.size();

        drawMarquee(window);
        std::this_thread::sleep_for(std::chrono::milliseconds(FRAME_MS.load()));
    }
}

bool startMarquee() {
    if (marqueeRunning.exchange(true)) return false;
    marqueeThread = std::thread(marqueeLoop);
    return true;
}

void stopMarquee() {
    if (!marqueeRunning.exchange(false)) return;
    if (marqueeThread.joinable()) marqueeThread.join();
}

int main() {
    enableAnsi();
    resetScreen();
    printHeader();

    std::string line;
    while (true) {
        say("Command> ");

        if (!std::getline(std::cin, line)) {
            say("\nInput closed. Terminating console...\n");
            break;
        }

        line = trim(line);
        if (line.empty()) continue;

        size_t spacePos = line.find(' ');
        std::string cmd = (spacePos == std::string::npos) ? line : line.substr(0, spacePos);
        std::string arg = (spacePos == std::string::npos) ? "" : trim(line.substr(spacePos + 1));

        if (cmd == "help") {
            say("help - displays the commands and its description\n"
                "set_text <text> - sets the marquee text\n"
                "set_speed <ms> - sets the marquee animation refresh in milliseconds\n"
                "start_marquee - starts the scrolling marquee\n"
                "stop_marquee - stops the marquee\n"
                "exit - terminates the console\n");
        }
        else if (cmd == "set_text") {
            if (arg.empty()) {
                say("Usage: set_text <your_string>\n");
            } else if (arg.size() > 1024) {
                say("Error: text too long (max 1024 characters)\n");
            } else {
                {
                    std::lock_guard<std::mutex> lock(textMutex);
                    marqueeText = arg;
                }
                say("Text saved for marquee: " + arg + "\n");
            }
        }
        else if (cmd == "set_speed") {
            try {
                size_t used = 0;
                int ms = std::stoi(arg, &used);
                if (arg.empty() || used != arg.size()) throw std::invalid_argument("bad");
                if (ms < 10 || ms > 1000) {
                    say("Error: speed must be between 10 and 1000 ms\n");
                } else {
                    FRAME_MS = ms;
                    say("Marquee refresh set to " + std::to_string(ms) + " ms\n");
                }
            } catch (...) {
                say("Usage: set_speed <milliseconds>  (e.g. set_speed 100)\n");
            }
        }
        else if (cmd == "start_marquee") {
            say(startMarquee() ? "Marquee started.\n" : "Marquee is already running.\n");
        }
        else if (cmd == "stop_marquee") {
            if (marqueeRunning) { stopMarquee(); say("Marquee stopped.\n"); }
            else                say("Marquee is not running.\n");
        }
        else if (cmd == "exit") {
            say("Terminating console...\n");
            break;
        }
        else {
            say("Unknown command: " + cmd + " (type 'help' for a list of commands)\n");
        }

        say("\n");
    }

    stopMarquee();
    say("\0337\033[1;1H\033[2K\0338");  // erase line 1, restore cursor
    say("\033[2J\033[3J\033[H");
    say("\033[r");
    return 0;
}