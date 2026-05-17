#ifndef SERAPHIM_SHELL_HPP
#define SERAPHIM_SHELL_HPP

#include <string>

class Shell {
public:
    void run();
private:
    bool running_ = true;

    std::string getInput();
    void output(const std::string& message) const;
    void help() const;
    void datetime() const;
    void date() const;
    void time() const;
    void clear() const;
    void echo(const std::string& message) const;
    void quit();
    void brend() const;
    std::string parseCommand(const std::string& input) const;
    std::string parseArrg(const std::string& input) const;
    void executeCommand(const std::string& command);
};

#endif // SERAPHIM_SHELL_HPP
