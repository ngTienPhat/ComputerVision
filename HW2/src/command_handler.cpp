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

    Mat _dummy = executeAlgorithmWithGivenCommand(imgDir, exeCommand);

    waitKey(0);
}

void CommandHandler::testAndSave(string saveDir){
    string imgDir = argv[1];
    string imgName = getImageNameFromImageDir(imgDir);
    cout << "processing " << imgName << " ..." << endl;

    Mat result = executeAlgorithmWithGivenCommand(imgDir, "detect_canny");

    int gaussSize = stoi(argv[3]);    
    float gaussStd = stof(argv[4]);
    int lowThres = stoi(argv[5]);
    int hightThres = stoi(argv[6]);

    MyImage sourceImage = MyImage(imgDir, IMREAD_GRAYSCALE);
    Mat opencvResult = opencvImageOperator::EdgeDetectCanny_opencv(sourceImage.getData(), lowThres, hightThres, gaussSize, gaussStd);

    ofstream resultFile;
    resultFile.open(saveDir+"/"+imgName+".txt", ios::out | ios::app);

    int tp = ImageOperator::calculateTruePositivePoints(result, opencvResult);
    int tn = ImageOperator::calculateTrueNegativePoints(result, opencvResult);
    int fp = ImageOperator::calculateFalsePositivePoints(result, opencvResult);
    int fn = ImageOperator::calculateFalseNegativePoints(result, opencvResult);
    
    float imageTotalPoints = getMatrixArea(sourceImage.getData());

    string inputParams = "gaussSize: "+ argv[3] + 
                            ", gaussSigma: " + argv[4] + 
                            ", low_thres: " + argv[5] + 
                            ", high_thres: " + argv[6];
    resultFile << inputParams <<endl;

    stringstream score;
    score << "\n true_positive: " << setprecision(2) <<(tp)
        << "\n true_negative: " << setprecision(2) <<(tn)
        << "\n false_positive: " << setprecision(2) <<(fp)
        << "\n false_negative: " << setprecision(2) <<(fn)
        << "\n false_total: " << (fp+fn)
        << "\n precision: " << setprecision(2) <<(100*float(tp)/(tp+fp)) << "%"
        << "\n recall: " << setprecision(2) <<(100*(float)tp/(tp+fn)) << "%"
        << "\n true negative rate: " << setprecision(2) <<(100*(float)tn/(tn+fp)) << "%";
        
    resultFile << score.str() << endl;
    resultFile << "------------------------------------------------" << endl;

    //print result images
    MyImage::saveImageFromMatrix(result, saveDir, "my_"+imgName); //save my result
    MyImage::saveImageFromMatrix(opencvResult, saveDir, "opencv_"+imgName); //save opencv result   

    cout << "Finish" << endl;
}

// --------------------------------------------------------
// PRIVATE AREA

Mat CommandHandler::executeAlgorithmWithGivenCommand(string imageDir, string commandName){
    if (commandName == "detect_sobel"){
        return executeSobelAlgorithm(imageDir);
    }
    else if (commandName == "detect_prewitt"){
        return executePrewittlAlgorithm(imageDir);
    }
    else if (commandName == "detect_laplacian"){
        return executeLaplacianAlgorithm(imageDir);
    }
    return executeCannyAlgorithm(imageDir);
}

void CommandHandler::writeResultLineToFile(ofstream outFile, string lineResult){
    outFile << lineResult << endl;
}

// Execute Algorithm given image dir
Mat CommandHandler::executeSobelAlgorithm(string imageDir){
    cout << "Detecting edge with Sobel algorithm..." << endl;
    MyImage inputImage = MyImage(imageDir, IMREAD_GRAYSCALE);

    int gaussSize = stoi(argv[3]);
    float gaussStd = stof(argv[4]);
    int pixelThres = stoi(argv[5]);

    Mat result = ImageOperator::EdgeDetectSobel(inputImage.getData(), gaussSize, gaussStd, pixelThres, true);
    return result;
}
Mat CommandHandler::executePrewittlAlgorithm(string imageDir){
    cout << "Detecting edge with Prewitt algorithm..." << endl;
    MyImage inputImage = MyImage(imageDir, IMREAD_GRAYSCALE);

    int gaussSize = stoi(argv[3]);
    float gaussStd = stof(argv[4]);
    int pixelThres = stoi(argv[5]);
    Mat result = ImageOperator::EdgeDetectPrewitt(inputImage.getData(), gaussSize, gaussStd, pixelThres, true);

    return result;
}
Mat CommandHandler::executeLaplacianAlgorithm(string imageDir){
    cout << "Detecting edge with Laplacian algorithm..." << endl;
    MyImage inputImage = MyImage(imageDir, IMREAD_GRAYSCALE);

    int gaussSize = stoi(argv[3]);
    float gaussStd = stof(argv[4]);
    float thresPrecent = stof(argv[5]);

    Mat result;
    if (thresPrecent > 1){
        result = ImageOperator::EdgeDetectLaplacian(inputImage.getData(), gaussSize, gaussStd);
    }
    else
        result = ImageOperator::EdgeDetectLaplacian(inputImage.getData(), gaussSize, gaussStd, thresPrecent);
    return result;
}
Mat CommandHandler::executeCannyAlgorithm(string imageDir){
    //cout << "Detecting edge with Canny algorithm..." << endl;
    MyImage inputImage = MyImage(imageDir, IMREAD_GRAYSCALE);

    int gaussSize = stoi(argv[3]);
    float gaussStd = stof(argv[4]);
    int lowThres = stoi(argv[5]);
    int hightThres = stoi(argv[6]);

    Mat result;
    if (hightThres < lowThres){
        result = ImageOperator::EdgeDetectCanny(inputImage.getData(), gaussSize, gaussStd);
    }
    else
        result = ImageOperator::EdgeDetectCanny(inputImage.getData(), gaussSize, gaussStd, lowThres, hightThres, false);
    
    return result;
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
        cout << i+1 << "." << patternCommands[i];
        cout << " <gauss_size:[int]>" << " <gauss_std:[float]>";
        if (patternCommands[i] == "detect_laplacian"){
            cout << " <threshold:[float]>";
        }
        else if (patternCommands[i] == "detect_canny"){
            cout << " <low_thres:[int]>" << " <high_thres:[int]>";
        }
        else 
            cout << " <edge_pixel_thres:[int]>";
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