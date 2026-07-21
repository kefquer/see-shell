#include "shell.hpp"

#include <algorithm>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <readline/readline.h>
#include <readline/history.h>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <sstream>

Shell::Shell() {
    std::signal(SIGINT, SIG_IGN);
    cmds = {
        { "cd", [this](auto argv){ this->cd(argv); } },
        { "pwd", [this](auto ){ this->pwd(); } },
        { "exit", [this](auto ){ this->quit(); } },
        { "datetime", [this](auto ){ this->datetime(); } },
        { "date", [this](auto ){ this->date(); } },
        { "time", [this](auto ){ this->time(); } },
        { "clear", [this](auto ){ this->clear(); } },
        { "echo", [this](auto argv){ this->echo(argv); } },
    };
}

std::string Shell::getInput() {
    char username[_SC_LOGIN_NAME_MAX];
    char hostname[_SC_HOST_NAME_MAX];
    if (getlogin_r(username,  sizeof(username)) == 0 && gethostname(hostname, sizeof(hostname)) == 0) {
        std::cout << "\u001b[33m" << username << "\u001b[37m" << "@" << hostname;
    }

    std::cout << " " << c_path.string();
    char* line = readline("\u001b[33m👁 >\u001b[37m ");
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
}

void Shell::echo(std::vector<std::string> argv) {
    std::cout << argv[1] << std::endl;
}

void Shell::cd(std::vector<std::string> argv) {
    chdir(argv[1].c_str());
    c_path = std::filesystem::current_path();
}

void Shell::pwd() {
    std::cout << c_path.string() << std::endl;
}

void Shell::quit() { running_ = false; }

int Shell::fork_exec(std::vector<std::string> argv) {
    std::vector<char*> c_args;
    for (const auto& arg : argv) {
        c_args.push_back(const_cast<char*>(arg.c_str()));
    }
    c_args.push_back(nullptr);

    pid_t pid = fork();

    if (pid < 0) {
        std::cerr << "Failed to create process\n";
        return 1;
    }

    if (pid == 0) {
        execvp(c_args[0], c_args.data());

        std::cerr << argv[0] << ": cmd not found\n";
        _exit(127);
    }
    int status;
    if (waitpid(pid, &status, 0) == -1) {
        std::perror("Waitpid error");
        return 1;
    }

    return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
}

std::vector<char*> Shell::to_c_args(const std::vector<std::string>& argv) {
    std::vector<char*> c_args;
    c_args.reserve(argv.size() + 1);
    for (const auto& arg : argv) {
        c_args.push_back(const_cast<char*>(arg.c_str()));
    }
    c_args.push_back(nullptr);

    return c_args;
}

int Shell::pipeline(std::vector<std::string> argv) {
    auto it = std::find(argv.begin(), argv.end(), "|");
    if (it == argv.end()) return 1;

    std::vector<std::string> cmd_args(argv.begin(), it);
    std::vector<std::string> cmd1_args(it + 1, argv.end());

    if (cmd_args.empty() || cmd1_args.empty()) {
        std::cerr << "Syntax error\n";
        return 1;
    }

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();
    if (pid == 0) {
        dup2(pipefd[1], STDOUT_FILENO);

        close(pipefd[1]);
        close(pipefd[0]);

        auto c_args = to_c_args(cmd_args);
        execvp(c_args[0], c_args.data());

        perror(cmd_args[0].c_str());
        exit(1);
    }

    pid_t pid1 = fork();
    if (pid1 == 0) {
        dup2(pipefd[0], STDIN_FILENO);

        close(pipefd[0]);
        close(pipefd[1]);

        auto c_args1 = to_c_args(cmd1_args);
        execvp(c_args1[0], c_args1.data());

        perror(cmd1_args[0].c_str());
        exit(1);
    }

    close(pipefd[0]);
    close(pipefd[1]);

    int status, status1;
    waitpid(pid, &status, 0);
    waitpid(pid1, &status1, 0);

    return WIFEXITED(status1) ? WEXITSTATUS(status1) : 1;
}

std::vector<std::string> Shell::parseArrg(std::string& input) {
    std::vector<std::string> argv;
    std::stringstream ss(input);
    std::string token;

    while (ss >> token) {
        argv.push_back(token);
    }

    // const char delimiters[] = " ";

    // std::vector<char> buffer(input.begin(), input.end());
    // buffer.push_back('\0');

    // char *token = strtok(buffer.data(), delimiters);

    // while (token != NULL ) {
    //     argv.push_back(token);
    //     token = strtok(NULL, delimiters);
    // }
    return argv;
}

void Shell::executeCommand(std::string& command) {
    const std::vector<std::string> argv = parseArrg(command);

    auto it = cmds.find(argv[0]);

    if (it != cmds.end()) {
        it->second(argv);
        return;
    }

    auto pipe_it = std::find(argv.begin(), argv.end(), "|");
    if (pipe_it != argv.end()) {
        pipeline(argv);
    } else {
        fork_exec(argv);
    }
}

void Shell::run() {
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
