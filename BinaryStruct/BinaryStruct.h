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
			TYPE_BYTES		  = 0x01000000,
			TYPE_ARRAY		  = 0x11000000
		};

		struct Entry {
			EType Type;
			byte1* Data;
			uint4 DataSize;

			// Universal getters
			template<typename T>
			T GetValue() {
				return *((T*)Data);
			}

			template<typename T>
			void GetBytes(T* arrayPtr) {
				memcpy(arrayPtr, Data, DataSize);
			}

			// Array and misc helpers
			template<>
			std::string GetValue() {
				return std::string((char*)Data, DataSize);
			}

			int GetArraySize() {
				if (Type != TYPE_ARRAY) return -1;

				return GetValue<uint4>();
			}

			int GetArrayElementSize() {
				if (Type != TYPE_ARRAY) return -1;
				else {
					int arrSize = GetArraySize();

					if (arrSize == 0) return 0;
					else return (DataSize - 4) / arrSize;
				}
			}

			template<typename T>
			void GetArray(T* arrayPtr) {
				if (Type != TYPE_ARRAY) return;

				int elemSize = GetArrayElementSize();
				if (elemSize != sizeof(T)) return; // fails even if the array is empty, also no cpp datatype has 0 bytes obviously

				memcpy(arrayPtr, Data + 4, DataSize - 4);
			}

			template<typename T>
			void GetArray(std::vector<T>& arr) {
				if (Type != TYPE_ARRAY) return;

				int elemSize = GetArrayElementSize();
				if (elemSize != sizeof(T)) return; // element incompatibility, same as above

				int arrSize = GetArraySize();
				if ((uint4)arr.size() < arrSize) arr.resize(arrSize);

				memcpy(&arr[0], Data + 4, DataSize - 4);
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

		template<typename T>
		void AddEntryArray(const std::string& name, T* arrPtr, uint4 elementsCount) {
			AddEntryArrayImpl(name, (byte1*)arrPtr, elementsCount, sizeof(T));
		}

		template<typename T>
		void AddEntryArray(const std::string& name, std::vector<T>& arr) {
			AddEntryArrayImpl(name, arr.empty() ? nullptr : (byte1*)&arr[0], (uint4)arr.size(), sizeof(T));
		}

	protected:
		void CreateTable(byte1* data, uint4 dataSize);
		void AddEntryImpl(const std::string& name, EType type, byte1* data, uint4 dataSize);
		void AddEntryArrayImpl(const std::string& name, byte1* data, uint4 elemCount, uint4 elemSize);

	protected:
		uint4 mID;
		std::map<std::string, Entry> mEntries;

};

#endif
