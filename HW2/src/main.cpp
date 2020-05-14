#include "command_handler.hpp"

int main(int argc, char** argv) {
	CommandHandler executor = CommandHandler(argc, argv);
    executor.execute();
	//executor.testAndSave("../result");
	
	return 0;
}