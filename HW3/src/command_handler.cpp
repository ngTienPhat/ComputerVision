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
    
    string exeCommand = argv[1];
    string imgDir = argv[2];

    if (exeCommand == "--help"){
        printPatternCommands();
        return;
    }

    executeAlgorithmWithGivenCommand(imgDir, exeCommand);

    waitKey(0);
}

void CommandHandler::testAndSave(string saveDir){
    // string imgDir = argv[1];
    // string imgName = getImageNameFromImageDir(imgDir);
    // cout << "processing " << imgName << " ..." << endl;

    // Mat result = executeAlgorithmWithGivenCommand(imgDir, "detect_canny");

    // int gaussSize = stoi(argv[3]);    
    // float gaussStd = stof(argv[4]);
    // int lowThres = stoi(argv[5]);
    // int hightThres = stoi(argv[6]);
    // int isPrint = stoi(argv[7]);

    // MyImage sourceImage = MyImage(imgDir, IMREAD_GRAYSCALE);
    // Mat opencvResult = opencvImageOperator::EdgeDetectCanny_opencv(sourceImage.getData(), lowThres, hightThres, gaussSize, gaussStd);

    // ofstream resultFile;
    // resultFile.open(saveDir+"/"+imgName+".txt", ios::out | ios::app);

    // int tp = ImageOperator::calculateTruePositivePoints(result, opencvResult);
    // int tn = ImageOperator::calculateTrueNegativePoints(result, opencvResult);
    // int fp = ImageOperator::calculateFalsePositivePoints(result, opencvResult);
    // int fn = ImageOperator::calculateFalseNegativePoints(result, opencvResult);
    
    // float imageTotalPoints = getMatrixArea(sourceImage.getData());

    // string inputParams = "gaussSize: "+ argv[3] + 
    //                         ", gaussSigma: " + argv[4] + 
    //                         ", low_thres: " + argv[5] + 
    //                         ", high_thres: " + argv[6];
    // resultFile << inputParams <<endl;

    // stringstream score;
    // score << "\n true_positive: " << setprecision(2) <<(tp)
    //     << "\n true_negative: " << setprecision(2) <<(tn)
    //     << "\n false_positive: " << setprecision(2) <<(fp)
    //     << "\n false_negative: " << setprecision(2) <<(fn)
    //     << "\n false_total: " << (fp+fn)
    //     << "\n precision: " << setprecision(2) <<(100*float(tp)/(tp+fp)) << "%"
    //     << "\n recall: " << setprecision(2) <<(100*(float)tp/(tp+fn)) << "%"
    //     << "\n true negative rate: " << setprecision(2) <<(100*(float)tn/(tn+fp)) << "%";
        
    // resultFile << score.str() << endl;
    // resultFile << "------------------------------------------------" << endl;

    // //print result images
    // MyImage::saveImageFromMatrix(result, saveDir, "my_"+imgName); //save my result
    // MyImage::saveImageFromMatrix(opencvResult, saveDir, "opencv_"+imgName); //save opencv result   

    // cout << "Finish" << endl;
}

// --------------------------------------------------------
// PRIVATE AREA

void CommandHandler::executeAlgorithmWithGivenCommand(string imageDir, string commandName){
    if (commandName == "detect_harris"){
        executeHarrisAlgorithm(imageDir);
    }
    else if (commandName == "detect_blob"){
        executeBloblAlgorithm(imageDir);
    }
    else if (commandName == "detect_blob_dog"){
        executeBlobDOGAlgorithm(imageDir);
    }
    else if (commandName == "detect_sift"){
        executeSiftAlgorithm(imageDir);
    }
    else if (commandName == "matching_images"){
        string trainImg = imageDir;
        string testImg = argv[3];
        executeSiftMatchingImages(trainImg, testImg);
    }
}

void CommandHandler::writeResultLineToFile(ofstream outFile, string lineResult){
    outFile << lineResult << endl;
}

// Execute Algorithm given image dir
void CommandHandler::executeHarrisAlgorithm(string imageDir){
    cout << "Detecting corners with Harris algorithm..." << endl;
    MyImage inputImage = MyImage(imageDir);

    float Rthreshold = stof(argv[3]);
    float empiricalConstant = stof(argv[4]);

    CornerDetector::harisCornerDetect(inputImage.getData(), Rthreshold, empiricalConstant);
}

void CommandHandler::executeBloblAlgorithm(string imageDir){
    cout << "Detecting blobs with Blob algorithm..." << endl;
    MyImage inputImage = MyImage(imageDir);

    float startSigma = stof(argv[3]);
    int nLayers = stoi(argv[4]);

    BlobDetector::detectBlob_LoG(inputImage.getData(), startSigma, nLayers);
}

void CommandHandler::executeBlobDOGAlgorithm(string imageDir){
    cout << "Detecting blobs with DOG-Blob algorithm..." << endl;
    MyImage inputImage = MyImage(imageDir);

    float startSigma = stof(argv[3]);
    int nLayers = stoi(argv[4]);

    BlobDetector::detectBlob_DoG(inputImage.getData(), startSigma, nLayers);
}

void CommandHandler::executeSiftAlgorithm(string imageDir){
    cout << "Detecting keypoints with Sift algorithm..." << endl;
    MyImage inputImage = MyImage(imageDir);

    float startSigma = stof(argv[3]);
    int numOctave = stoi(argv[4]);
    int numScalesPerOctave = stoi(argv[5]);

    Sift siftDetector(startSigma, numOctave, numScalesPerOctave);

    siftDetector.execute(inputImage.getData());
}

void CommandHandler::executeSiftMatchingImages(string trainImage, string testDir){
    cout << "Matching keypoints using Sift algorithm..." << endl;
    KeypointsMatcher myMatcher;

	myMatcher.knnMatchTwoImages(trainImage, testDir);
}

// Check valid commands
bool CommandHandler::isValidCommands(){
    if (this->argc < 3)
        return false;
    
    return isCommandInPattern(this->argv[1]);
}

void CommandHandler::initPatternCommands(){
    patternCommands.push_back("detect_harris");
    patternCommands.push_back("detect_blob");
    patternCommands.push_back("detect_blob_dog");
    patternCommands.push_back("detect_sift");
    patternCommands.push_back("matching_images");
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
    for(int i = 0; i < patternCommands.size(); i++){
        if (checkCommand == patternCommands[i])
            return true;
    }
    return false;
}