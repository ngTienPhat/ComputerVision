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
    cout << "----------------------------------------------"  << endl;
    cout << "Detecting corners with Harris algorithm..." << endl;
    MyImage inputImage = MyImage(imageDir);

    float Rthreshold = stof(argv[3]);
    float empiricalConstant = stof(argv[4]);

    Mat res = CornerDetector::harisCornerDetect(inputImage.getData(), Rthreshold, empiricalConstant);
    MyImage::saveImageFromMatrix(res, result_dir, "Harris_"+getImageNameFromImageDir(imageDir));
}

void CommandHandler::executeBloblAlgorithm(string imageDir){
    cout << "----------------------------------------------"  << endl;
    cout << "Detecting blobs with Blob algorithm..." << endl;
    MyImage inputImage = MyImage(imageDir);
    Mat imgMatrix = inputImage.getData();

    float startSigma = stof(argv[3]);
    int nLayers = stoi(argv[4]);

    vector<Blob> res = BlobDetector::detectBlob_LoG(imgMatrix, startSigma, nLayers);
    Mat visResult = BlobDetector::visualizeResult(imgMatrix, res);
    MyImage::saveImageFromMatrix(visResult, result_dir, "LOG-blob_"+getImageNameFromImageDir(imageDir));
}

void CommandHandler::executeBlobDOGAlgorithm(string imageDir){
    cout << "----------------------------------------------"  << endl;
    cout << "Detecting blobs with DOG-Blob algorithm..." << endl;
    MyImage inputImage = MyImage(imageDir);
    Mat imgMatrix = inputImage.getData();

    float startSigma = stof(argv[3]);
    int nLayers = stoi(argv[4]);

    vector<Blob> res = BlobDetector::detectBlob_DoG(imgMatrix, startSigma, nLayers);
    Mat visResult = BlobDetector::visualizeResult(imgMatrix, res);
    MyImage::saveImageFromMatrix(visResult, result_dir, "DOG-blob_"+getImageNameFromImageDir(imageDir));
}

void CommandHandler::executeSiftAlgorithm(string imageDir){
    cout << "----------------------------------------------"  << endl;
    cout << "Detecting keypoints with Sift algorithm..." << endl;

    float startSigma = stof(argv[3]);
    int numOctave = stoi(argv[4]);
    int numScalesPerOctave = stoi(argv[5]);

    Sift siftDetector(startSigma, numOctave, numScalesPerOctave);

    vector<Extrema> kps = siftDetector.extractKeypoints(imageDir);
    Mat resMat = siftDetector.visualizeKeypoints(kps, imageDir);
    MyImage::saveImageFromMatrix(resMat, result_dir, "Sift_"+getImageNameFromImageDir(imageDir));
}

void CommandHandler::executeSiftMatchingImages(string trainImage, string testDir){
    cout << "----------------------------------------------"  << endl;
    cout << "Matching keypoints using Sift algorithm..." << endl;
    KeypointsMatcher myMatcher;

	Mat visResult = myMatcher.knnMatchTwoImages(trainImage, testDir);
    MyImage::saveImageFromMatrix(visResult, result_dir, "Sift-matching_"+
                                                            getImageNameFromImageDir(trainImage)+"_"+
                                                            getImageNameFromImageDir(testDir));
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