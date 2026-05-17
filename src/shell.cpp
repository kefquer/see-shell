#include "shell.hpp"

#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <string>
#include <format>
#include <chrono>
#include <readline/readline.h>
#include <readline/history.h>

std::string getInput() {
    char* line = readline("> ");
    if (line == nullptr) {
        std::cout << std::endl;
        return "exit";
    }
    std::string input(line);
    free(line);
    if (!input.empty()) {
        add_history(input.c_str());
    }
    return input;
}

void Shell::output(const std::string& message) const {
    std::cout << message << std::endl;
}

void Shell::help() const {
    std::cout << "datetime - current date and time" << std::endl;
    std::cout << "date - current date (YYYY-MM-DD)" << std::endl;
    std::cout << "time - current time (HH:MM:SS)" << std::endl;
    std::cout << "clear - clear the terminal screen" << std::endl;
    std::cout << "echo <text> - print the given text" << std::endl;
    std::cout << "exit - выход из shell" << std::endl;
    std::cout << "li <command> - run any system command (Linux Integration)" << std::endl;
    std::cout << "--------------------" << std::endl;
    std::cout << "help - show this help" << std::endl;
}

void Shell::datetime() const {
    auto date = std::chrono::system_clock::now();
    std::string outdate = std::format("{:%c}", date);
    output(outdate);
}

void Shell::date() const {
    auto date = std::chrono::system_clock::now();
    std::string outdate = std::format("{:%F}", date);
    output(outdate);
}

void Shell::time() const {
    auto date = std::chrono::system_clock::now();
    std::string outdate = std::format("{:%T}", date);
    output(outdate);
}

void Shell::clear() const {
    std::cout<<"\033[2J\033[1;1H" << std::flush;
    brend();
}

void Shell::echo(const std::string& arrg) const {
    std::cout << arrg << std::endl;
}

void Shell::quit() { running_ = false; }

void Shell::brend() const {
    std::cout << "---XwX---" << std::endl;
    std::cout << "Seraphim Shell" << std::endl;
}

std::string Shell::parseCommand(const std::string& input) const {
    size_t spacePos = input.find(' ');
    if (spacePos != std::string::npos) {
        return input.substr(0, spacePos);
    }
    return input;
}

std::string Shell::parseArrg(const std::string& input) const {
    size_t spacePos = input.find(' ');
    if (spacePos != std::string::npos) {
        return input.substr(spacePos + 1);
    }
    return "";
}

void Shell::executeCommand(const std::string& command) {
    const std::string cmd = parseCommand(command);
    const std::string arrg = parseArrg(command);
    if (cmd == "help") {
        help();
    } else if (cmd == "datetime") {
        datetime();
    } else if (cmd == "date") {
        date();
    } else if (cmd == "time") {
        time();
    } else if (cmd == "clear") {
        clear();
    } else if (cmd == "echo") {
        echo(arrg);
    } else if (cmd == "li") {
        system(arrg.c_str());
    } else if (cmd == "exit") {
        quit();
    } else {
        std::cout << "Unknown command: " << cmd << std::endl;
    }
}

void Shell::run() {
    brend();
    std::string userInput;
    while (running_) {
        userInput = getInput();
        if (userInput.empty()) {
            continue;
        } else {
            executeCommand(userInput);
        }
    }
}
