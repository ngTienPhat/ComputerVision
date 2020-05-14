#include "command_handler.hpp"

int main(int argc, char** argv) {
    //string data_dir = "/Users/tienphat/Documents/HCMUS/Computer_Vision/ComputerVision/data";
	//string image_dir = data_dir+"/lena.jpg";

	CommandHandler executor = CommandHandler(argc, argv);
    executor.execute();
	executor.testAndSave("../result");

	return 0;
}