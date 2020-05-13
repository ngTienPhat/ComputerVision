#include "command_handler.hpp"

CommandHandler::CommandHandler(int argc, char** argv){
    this->argc = argc;
    for(int i = 0; i < argc; i++){
        this->argv.push_back(argv[i]);
    }
    initPatternCommands();
    valid = isValidCommands();
}

void CommandHandler::execute(){
    if (valid == false){
        cout <<"command is not valid"<<endl;
        printPatternCommands();
        return;
    }
    
    string exeCommand = argv[2];
    string imgDir = argv[1];

    if (exeCommand == "--help"){
        printPatternCommands();
        return;
    }

    if (exeCommand == "detect_sobel"){
        executeSobelAlgorithm(imgDir);
    }
    else if (exeCommand == "detect_prewitt"){
        executePrewittlAlgorithm(imgDir);
    }
    else if (exeCommand == "detect_laplacian"){
        executeLaplacianAlgorithm(imgDir);
    }
    else if (exeCommand == "detect_canny"){
        executeCannyAlgorithm(imgDir);
    }

    waitKey(0);
}

void CommandHandler::executeAndTest(){
}
// --------------------------------------------------------
// PRIVATE AREA


// Execute Algorithm given image dir
void CommandHandler::executeSobelAlgorithm(string imageDir){
    MyImage inputImage = MyImage(imageDir, IMREAD_GRAYSCALE);

    int gaussSize = stoi(argv[3]);
    float gaussStd = stof(argv[4]);

    Mat result = ImageOperator::EdgeDetectSobel(inputImage.getData(), gaussSize, gaussStd, true);
}
void CommandHandler::executePrewittlAlgorithm(string imageDir){
    MyImage inputImage = MyImage(imageDir, IMREAD_GRAYSCALE);

    int gaussSize = stoi(argv[3]);
    float gaussStd = stof(argv[4]);
    Mat result = ImageOperator::EdgeDetectPrewitt(inputImage.getData(), gaussSize, gaussStd, true);
}
void CommandHandler::executeLaplacianAlgorithm(string imageDir){
    MyImage inputImage = MyImage(imageDir, IMREAD_GRAYSCALE);

    int gaussSize = stoi(argv[3]);
    float gaussStd = stof(argv[4]);

    Mat result = ImageOperator::EdgeDetectLaplacian(inputImage.getData(), gaussSize, gaussStd);
}
void CommandHandler::executeCannyAlgorithm(string imageDir){
    MyImage inputImage = MyImage(imageDir, IMREAD_GRAYSCALE);

    int gaussSize = stoi(argv[3]);
    float gaussStd = stof(argv[4]);
    Mat result = ImageOperator::EdgeDetectCanny(inputImage.getData(), gaussSize, gaussStd);
}


// Check valid commands
bool CommandHandler::isValidCommands(){
    if (this->argc < 3)
        return false;
    
    return isCommandInPattern(this->argv[2]);
}

void CommandHandler::initPatternCommands(){
    patternCommands.push_back("detect_sobel");
    patternCommands.push_back("detect_prewitt");
    patternCommands.push_back("detect_laplacian");
    patternCommands.push_back("detect_canny");
}

void CommandHandler::printPatternCommands(){
    cout << "valid commands: " << endl;
    for(int i = 0; i < patternCommands.size(); i++){
        cout << i+1 << ".  " << patternCommands[i];
        cout << " <gauss_size:[int]>" << "  <gauss_std:[float]>";
        cout << endl;
    }
}

bool CommandHandler::isCommandInPattern(const string &checkCommand){
    bool flag = false;
    for(int i = 0; i < patternCommands.size(); i++){
        if (checkCommand == patternCommands[i])
            flag = true;
    }
    return flag;
}