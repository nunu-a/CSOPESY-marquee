# Marquee Console

A simple C++ console program with a scrolling marquee on row 1 and a command prompt below it.

## What it does

- Shows a header (section, developer, version date).
- Keeps a marquee running on row 1 without disturbing what you type.
- Accepts commands from the `Command>` prompt.

## Commands

| Command | Description |
|---------|-------------|
| `help` | Show all commands |
| `set_text <text>` | Set the marquee text (max 1024 chars) |
| `set_speed <ms>` | Set refresh delay in ms (10–1000) |
| `start_marquee` | Start the scrolling marquee |
| `stop_marquee` | Stop the marquee |
| `exit` | Quit the program |

## How to compile

### Windows (MinGW / g++)

```bash
g++ -std=c++17 -O2 -pthread main.cpp -o marquee.exe
```

### Windows (MSVC)

```bash
cl /std:c++17 /EHsc /O2 main.cpp
```

### Linux / macOS

```bash
g++ -std=c++17 -O2 -pthread main.cpp -o marquee
```

> The file must be saved as `main.cpp` (or change the name in the command to match yours).

## How to run

### Windows

```bash
marquee.exe
```

or

```bash
.\marquee.exe
```

### Linux / macOS

```bash
./marquee
```

## Example session

```
Command> set_text Hello, CSOPESY!
Text saved for marquee: Hello, CSOPESY!

Command> set_speed 100
Marquee refresh set to 100 ms

Command> start_marquee
Marquee started.

Command> stop_marquee
Marquee stopped.

Command> exit
Terminating console...
```

## Notes

- On Windows, run it in **Windows Terminal** for the smoothest animation. `cmd.exe` works but is slower.
- Default speed is `150 ms`. Lower = faster, higher = slower.
- Press `Ctrl+C` to force-quit. Type `exit` for a clean exit.

## AI Disclosure

In the interest of transparency, the following uses of AI are disclosed:

- **Formatting:** AI was used to format this `README.md` file.
- **References and suggestions:** AI was used to look for references regarding fixes to the choppy refresh rate and to suggest the best way to phrase / reduce the conflict between the two threads (the main thread and the marquee thread).
- **Authorship:** All analysis and code were written and verified by the student. AI did not author any part of the implementation; it was only used for formatting and reference lookup as described above.
