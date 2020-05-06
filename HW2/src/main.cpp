#include "image.hpp"
#include "image_operator.hpp"
#include "kernel_generator.hpp"

int main(){
    string data_dir = "/Users/tienphat/Documents/HCMUS/Computer_Vision/ComputerVision/data";
    string image_dir = "1.jpg";

    //MyImage my_image = MyImage(data_dir + "/"+image_dir);
    //my_image.showImage();


    Mat laplaceGx = KernelGenerator::getLaplaceKernelGx();
    cout << laplaceGx << endl;

    for(int i= 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            cout << laplaceGx.at<float>(i, j) << endl;
        }
    }

    return 0;
}