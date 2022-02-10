#include <iostream>
#include <fstream>
#include "BinaryStruct.h"

int main(int argc, char** argv) {
	BinaryStruct* bs = new BinaryStruct("testout.bin");

	//std::string strrr = "lol sa vedem daca merge chestia asta";
	//bs->AddEntry("str", strrr);

	float lol = bs->GetEntry("Lol")->GetValue<float>();
	int test = bs->GetEntry("Test")->GetValue<int>();
	
	std::string strtest = bs->GetEntry("str")->GetValue<std::string>();

	bs->Export("testout2.bin");

	return 0;
}

