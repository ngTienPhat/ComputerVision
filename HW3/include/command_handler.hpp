#pragma once

#include "image.hpp"
#include "blob_detector.hpp"
#include "corner_detector.hpp"
#include "keypoints_matcher.hpp"
// /*
// This class is used to handle command line commands and execute them.
// */

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
    void executeHarrisAlgorithm(string imageDir);
    void executeBloblAlgorithm(string imageDir);
    void executeBlobDOGAlgorithm(string imageDir);
    void executeSiftAlgorithm(string imageDir);

    /*
    Group of functions check command line validity
    */
    bool isValidCommands();
    void initPatternCommands();
    void printPatternCommands();
    bool isCommandInPattern(const string &checkCommand);

    // These functions are used to test only
    void executeAlgorithmWithGivenCommand(string imageDir, string commandName);
    void writeResultLineToFile(ofstream outFile, string lineResult);
};
