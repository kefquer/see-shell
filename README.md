# See Shell

**See** – Seraphim Execution Environment. A simple command-line shell written in C++.
⚠️ Note: This project is in early development and may be unstable. It is not recommended to set it as your default shell until it becomes more mature.

## What it does

See Shell provides a basic interactive prompt with a few built-in commands and the ability to launch system programs.

## Built-in commands

- `help` – print a list of available commands
- `datetime` – show current date and time in locale format
- `date` – show current date (YYYY-MM-DD)
- `time` – show current time (HH:MM:SS)
- `clear` – clear the terminal screen (using ANSI escape codes)
- `echo <text>` – print the given text to the terminal
- `li <command>` – execute a system command (Linux Integration)
- `exit` – quit the shell

The shell uses **GNU Readline** for input handling, which gives you line editing, history navigation with arrow keys, and persistent history between sessions.

## Building

### Requirements

- C++20 compatible compiler (GCC 10+ or Clang 12+)
- GNU Readline library (`readline` on Arch, `libreadline-dev` on Debian/Ubuntu)
- GNU Make

### Build

```bash
git clone https://github.com/kefquer/see-shell.git
cd see-shell
make
```
The binary will be named see-shell.

## Installation (optional)

A script install.sh is provided for convenience. It will build the project, copy the binary to /usr/local/bin/see-shell, and add it to /etc/shells.

```bash
chmod +x install.sh
./install.sh
```

After that you can run it by typing see-shell in any terminal. To set it as your default login shell, use chsh -s /usr/local/bin/see-shell. It is strongly recommended to test the shell thoroughly before changing your default shell.

# License

MIT License. See LICENSE for details.
