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

    // result dir
    string result_dir="./result";

public:
    //CommandHandler();
    CommandHandler(int argc, char** argv);

    /*
    Main function to execute command line program and show result on user's display
    */
    void execute();
    

// HELPER FUNCTIONS
private:
    /*
    Group of helper functions to apply algorithms based on user 
    command line arguments

    Input: string of image path
    */
    void executeHarrisAlgorithm(string imageDir);
    void executeBloblAlgorithm(string imageDir);
    void executeBlobDOGAlgorithm(string imageDir);
    void executeSiftAlgorithm(string imageDir);
    void executeSiftMatchingImages(string trainImage, string testDir);

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
