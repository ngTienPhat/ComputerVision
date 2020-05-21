#include "common.hpp"

string getImageNameFromImageDir(string imageDir){
	int sLength = imageDir.size();
	int start = 0;
	string imageName= "";

	for(int j = sLength-1; j >= 0; --j){
		if (imageDir[j] == '/'){
			start = j+1;
			break;
		}
	}
	while(imageDir[start] != '.'){
		imageName += imageDir[start];
		start+=1;
	}

	return imageName;
}

float sumFunction(float a, float b){
	return a + b;
}
float multiplyFunction(float a, float b){
	return a*b;
}
float divideFunction(float a, float b){
	return a/b;
}
float substractFuntion(float a, float b){
	return a-b;
}