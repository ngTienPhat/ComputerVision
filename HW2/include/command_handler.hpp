#ifndef COMMAND_HANDLER_HPP__
#define COMMAND_HANDLER_HPP__

#include "image.hpp"
#include "image_operator.hpp"
#include "kernel_generator.hpp"

class CommandHandler{
private:
    int argc;
    bool valid;
    vector<string> argv;

    vector<string> patternCommands;

public:
    CommandHandler();
    CommandHandler(int argc, char** argv);

    void execute();
    void executeAndTest();

// HELPER FUNCTIONS
private:
    void executeSobelAlgorithm(string imageDir);
    void executePrewittlAlgorithm(string imageDir);
    void executeLaplacianAlgorithm(string imageDir);
    void executeCannyAlgorithm(string imageDir);

    bool isValidCommands();
    void initPatternCommands();
    void printPatternCommands();
    bool isCommandInPattern(const string &checkCommand);
};

#endif //COMMAND_HANDLER_HPP__
