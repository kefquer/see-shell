#include "shell.hpp"

#include <csignal>
#include <iostream>
#include <chrono>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

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

        std::cerr << argv[0] << ": cmd nf\n";
        _exit(127);
    } else if (pid > 0) {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            std::perror("Waitpid error");
            return 1;
        }
        return 1;
    } else {
        std::perror("Fork error");
        return 1;
    }
}

std::vector<std::string> Shell::parseArrg(std::string& input) {
    std::vector<std::string> argv;
    const char delimiters[] = " ";

    std::vector<char> buffer(input.begin(), input.end());
    buffer.push_back('\0');

    char *token = strtok(buffer.data(), delimiters);

    while (token != NULL ) {
        argv.push_back(token);
        token = strtok(NULL, delimiters);
    }
    return argv;
}

void Shell::executeCommand(std::string& command) {
    //const std::string cmd = parseCommand(command);
    const std::vector<std::string> argv = parseArrg(command);

    auto it = cmds.find(argv[0]);

    if (it != cmds.end()) {
        it->second(argv);
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
