#ifndef SERAPHIM_SHELL_HPP
#define SERAPHIM_SHELL_HPP

#include <functional>
#include <string>
#include <unistd.h>
#include <unordered_map>
#include <vector>
#include <filesystem>

class Shell {
public:
    Shell();
    void run();

    std::filesystem::path c_path = std::filesystem::current_path();
private:
    bool running_ = true;

    std::unordered_map<std::string, std::function<void(std::vector<std::string> argv)>> cmds;

    std::string getInput();
    void output(const std::string& message) const;

    void datetime() const;
    void date() const;
    void time() const;
    void clear() const;
    void echo(std::vector<std::string> argv);
    void cd(std::vector<std::string> argv);
    void pwd();
    void quit();

    std::vector<char*> to_c_args(const std::vector<std::string>& argv);

    int pipeline(std::vector<std::string> argv);
    int fork_exec(std::vector<std::string> argv);
    std::vector<std::string> parseArrg(std::string& input);
    void executeCommand(std::string& command);
};

#endif // SERAPHIM_SHELL_HPP
