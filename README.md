# Nim
A minimal terminal-based text editor written in C using raw mode and ANSI escape sequences.

This project is primarily a learning exercise to understand:

- Terminal raw mode (`termios`)
- Low-level input handling
- Cursor movement with escape codes
- Basic file I/O
- Simple editor state management

It currently supports basic typing, navigation, saving, and quitting directly inside the terminal.

## Super Basic Demo
https://github.com/user-attachments/assets/14f4808a-7a21-407d-ab10-37431448e0fe

## Features
- Raw terminal input (no enter required)
- Arrow key cursor movement
- Create new files or open existing ones
- Save using Ctrl + O
- Quit using Ctrl + Q
- Simple status bar (row, column, filename, last saved time)

## Installation

### Requirements

- Linux (or any POSIX system)
- GCC or Clang

### After cloning the repo, Usage

```bash
gcc main.c -o m
./m
```

You can also use cli arguments to open file directly
```bash
gcc main.c -o m
./m filename.txt
```

## ToDo
- [ ] Proper Cursor Movements
- [ ] 2D Buffer
- [ ] Scrolling Support
- [ ] Better Status Bar
- [ ] Basic Search




