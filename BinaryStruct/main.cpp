#include <iostream>
#include <fstream>
#include "BinaryStruct.h"

int main(int argc, char** argv) {
	BinaryStruct* bs = new BinaryStruct("testout2.bin");

	//std::string strrr = "lol sa vedem daca merge chestia ast22a";
	//bs->AddEntry("str", strrr);

	float lol = bs->GetEntry("Lol")->GetValue<float>();
	int test = bs->GetEntry("Test")->GetValue<int>();
	
	std::string strtest = bs->GetEntry("str")->GetValue<std::string>();

	//int testarr[4] = { 4, 8, 10, 14 };
	//bs->AddEntryArray("arrai", testarr, 4);

	std::vector<int> testarrin;
	bs->GetEntry("arrai")->GetArray(testarrin);

	std::vector<float> testarr2;
	bs->AddEntryArray("arrai2", testarr2);

	bs->AddEntryArray<float>("arrai3nullptr", nullptr, 0);


	bs->Export("testout2.bin");

	int xx = bs->GetEntry("arrai3nullptr")->GetArrayElementSize();

	return 0;
}

