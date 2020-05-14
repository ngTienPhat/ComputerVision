#ifndef COMMAND_HANDLER_HPP__
#define COMMAND_HANDLER_HPP__


#include "image.hpp"
#include "image_operator.hpp"
#include "kernel_generator.hpp"
#include "image_operator_opencv.hpp"

/*
This class is used to handle command line commands and execute them.
*/

class CommandHandler{
private:
    int argc;
    bool valid;
    vector<string> argv;

    vector<string> patternCommands;

public:
    //CommandHandler();
    CommandHandler(int argc, char** argv);

    /*
    Main function to execute command line program and show result on user's display
    */
    void execute();
    
    /*
    This is used for testing with Canny algorithm only
    */
    void testAndSave(string saveDir);
// HELPER FUNCTIONS
private:
    /*
    Group of helper functions apply edge detection algorithms based on user 
    command line arguments

    Input: string of image path
    Output: Matrix of result
    */
    Mat executeSobelAlgorithm(string imageDir);
    Mat executePrewittlAlgorithm(string imageDir);
    Mat executeLaplacianAlgorithm(string imageDir);
    Mat executeCannyAlgorithm(string imageDir);

    /*
    Group of functions check command line validity
    */
    bool isValidCommands();
    void initPatternCommands();
    void printPatternCommands();
    bool isCommandInPattern(const string &checkCommand);

    // These functions are used to test only
    Mat executeAlgorithmWithGivenCommand(string imageDir, string commandName);
    void writeResultLineToFile(ofstream outFile, string lineResult);
};

#endif //COMMAND_HANDLER_HPP__
