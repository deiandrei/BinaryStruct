#include "BinaryStruct.h"

void BinaryStruct::AddEntryImpl(const std::string& name, EType type, byte1* data, uint4 dataSize) {
	if (HasEntry(name)) return;

	Entry entry;
	entry.Type = type;
	entry.DataSize = dataSize;
	entry.Data = new byte1[dataSize];
	memcpy(entry.Data, data, dataSize);

	mEntries.insert({ name, entry });
}

void BinaryStruct::AddEntryArrayImpl(const std::string& name, byte1* data, uint4 elemCount, uint4 elemSize) {
	if (HasEntry(name) || !elemSize) return;

	Entry entry;
	entry.Type = TYPE_ARRAY;
	entry.DataSize = elemSize * elemCount + 4;
	entry.Data = new byte1[entry.DataSize];
	memcpy(entry.Data, &elemCount, sizeof(uint4));
	if(elemCount) memcpy(entry.Data + sizeof(uint4), data, elemSize * elemCount);

	mEntries.insert({ name, entry });
}

void BinaryStruct::AddEntry(const std::string& name, int value) {
	AddEntryImpl(name, TYPE_INT, (byte1*)&value, sizeof(int));
}

void BinaryStruct::AddEntry(const std::string& name, float value) {
	AddEntryImpl(name, TYPE_FLOAT, (byte1*)&value, sizeof(float));
}

void BinaryStruct::AddEntry(const std::string& name, const std::string& value) {
	AddEntryImpl(name, TYPE_STRING, (byte1*)value.c_str(), (uint4)value.length());
}

void BinaryStruct::AddEntry(const std::string& name, byte1* data, uint4 dataSize) {
	AddEntryImpl(name, TYPE_BYTES, data, dataSize);
}

void BinaryStruct::CreateTable(byte1* data, uint4 dataSize) {
	ByteBuffer* bb = new ByteBuffer(data, dataSize);

	// build the <id, dataPosition> map
	std::map<uint4, uint4> idsMap;
	bb->Position() = 0;
	while (!bb->EndReached()) {
		bb->Position() += 4;
		uint4 id = bb->Read<uint4>();

		if (idsMap.find(id) != idsMap.end()) continue;
		else idsMap.insert({ id, bb->Position() - 8 }); // save the entry position 0, so subtract the last 8 bytes read

		uint4 size = bb->Read<uint4>();
		bb->Position() += size;
	}

	// read entry list
	bb->Position() = 12; // skip the type, id and size
	uint4 entryID, stringLen;
	char str[1024];
	uint4 rowCount = bb->Read<uint4>();
	for (uint4 i = 0; i < rowCount; ++i) {
		// read entry properties
		entryID = bb->Read<uint4>();

		if (idsMap.find(entryID) != idsMap.end()) {
			uint4 entryDataPosition = idsMap[entryID];

			stringLen = bb->Read<uint4>();
			bb->ReadArray<char>(&str[0], stringLen);
			str[stringLen] = 0;

			Entry entry;
			memcpy(&entry.Type, bb->Data() + (entryDataPosition), sizeof(uint4));
			memcpy(&entry.DataSize, bb->Data() + (entryDataPosition + 8), sizeof(uint4));
			entry.Data = new byte1[entry.DataSize];
			memcpy(entry.Data, bb->Data() + entryDataPosition + 12, entry.DataSize);

			mEntries.insert({ std::string(str), entry });
		}
		else bb->Position() += bb->Read<uint4>(); // skip this block data if the entry is not found
	}
}

BinaryStruct::BinaryStruct() {
	
}

BinaryStruct::~BinaryStruct() {

}

bool BinaryStruct::Load(const std::string& file) {
	std::ifstream in(file, std::ios::binary);

	uint4 size;
	in.seekg(0, std::ios::end);
	size = in.tellg();
	in.seekg(0);

	byte1* buf = new byte1[size];
	in.read((char*)&buf[0], size);
	in.close();
	bool status = Load(buf, size);
	delete buf;

	return status;
}

bool BinaryStruct::Load(byte1* data, uint4 size) {
	int4 type;
	memcpy(&type, data, sizeof(int4));
	if (type != TYPE_BINARYSTRUCT) return false;

	CreateTable(data, size);

	return true;
}

void BinaryStruct::Export(const std::string& file) {
	auto data = this->Export();

	std::ofstream out(file, std::ios::binary);
	out.write((char*)&data.first[0], data.second);
	out.close();
}

std::pair<byte1*, uint4> BinaryStruct::Export() {
	ByteBuffer* bb = new ByteBuffer;

	// write the BS header and then the data
	std::map<std::string, uint4> idsMap; // map key string to unique id

	bb->Write<uint4>(EType::TYPE_BINARYSTRUCT);
	bb->Write<uint4>(mID);
	bb->Write<uint4>(0); // temp size for now
	bb->Write<uint4>((uint4)mEntries.size());
	int entryCount = 0;
	for (auto itr = mEntries.begin(); itr != mEntries.end(); ++itr) {
		entryCount++;
		bb->Write<uint4>(entryCount);
		bb->Write<uint4>((uint4)itr->first.length());
		bb->WriteArray<char>((char*)itr->first.c_str(), (uint4)itr->first.length());

		idsMap.insert({ itr->first, entryCount });
	}

	// write the BS header size
	bb->PreparePosition(8);
	bb->Write<uint4>(bb->Size() - 12); // - (type + id + size)
	bb->RestorePosition();

	// write every entry and its data
	for (auto itr = mEntries.begin(); itr != mEntries.end(); ++itr) {
		bb->Write<uint4>(itr->second.Type);
		bb->Write<uint4>(idsMap[itr->first]);
		bb->Write<uint4>(itr->second.DataSize);
		bb->WriteArray<byte1>(itr->second.Data, itr->second.DataSize);
	}

	return { bb->Data(), bb->Size() };
}
