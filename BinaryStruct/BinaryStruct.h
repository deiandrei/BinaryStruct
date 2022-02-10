#ifndef BINARYSTRUCT_H
#define BINARYSTRUCT_H

#include "ByteBuffer.h"
#include <map>

class BinaryStruct {
	public:
		enum EType {
			TYPE_BINARYSTRUCT = 0x00B8779E, // BS TYPE
			TYPE_INT		  = 0x00000001,
			TYPE_FLOAT		  = 0x00000100,
			TYPE_STRING		  = 0x00010000,
			TYPE_BYTES		  = 0x01000000
		};

		struct Entry {
			EType Type;
			byte1* Data;
			uint4 DataSize;

			template<class T>
			T GetValue() {
				return *((T*)Data);
			}

			template<>
			std::string GetValue() {
				return std::string((char*)Data, DataSize);
			}

			template<class T>
			void GetArray(T* arrayPtr) {
				memcpy(arrayPtr, Data, DataSize);
			}

		};

	public:
		BinaryStruct();
		BinaryStruct(const std::string& file) { Load(file); }
		~BinaryStruct();

		bool Load(const std::string& file);
		bool Load(byte1* data, uint4 size);
		void Export(const std::string& file);
		std::pair<byte1*, uint4> Export();

		bool HasEntry(const std::string& name) { return mEntries.find(name) != mEntries.end(); }
		Entry* GetEntry(const std::string& name) { if (HasEntry(name)) return &mEntries[name]; return nullptr; }

		void AddEntry(const std::string& name, int value);
		void AddEntry(const std::string& name, float value);
		void AddEntry(const std::string& name, const std::string& value);
		void AddEntry(const std::string& name, byte1* data, uint4 dataSize);

	protected:
		void CreateTable(byte1* data, uint4 dataSize);
		void AddEntryImpl(const std::string& name, EType type, byte1* data, uint4 dataSize);

	protected:
		uint4 mID;
		std::map<std::string, Entry> mEntries;

};

#endif
