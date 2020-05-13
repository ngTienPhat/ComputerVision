#ifndef COMMAND_HANDLER_HPP__
#define COMMAND_HANDLER_HPP__


#include "image.hpp"
#include "image_operator.hpp"
#include "kernel_generator.hpp"
#include "image_operator_opencv.hpp"
#include <filesystem>

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
    void executeAndTest(string dataDir, string saveDir);

// HELPER FUNCTIONS
private:
    Mat executeSobelAlgorithm(string imageDir);
    Mat executePrewittlAlgorithm(string imageDir);
    Mat executeLaplacianAlgorithm(string imageDir);
    Mat executeCannyAlgorithm(string imageDir);

    bool isValidCommands();
    void initPatternCommands();
    void printPatternCommands();
    bool isCommandInPattern(const string &checkCommand);

    void testAllAlgorithmsOnSingleImage(string imageDir, string saveDir);
    Mat executeAlgorithmWithGivenCommand(string imageDir, string commandName);
};

#endif //COMMAND_HANDLER_HPP__
