#ifndef BYTEBUFFER_H
#define BYTEBUFFER_H

#include "defines.h"

class ByteBuffer {
	public:
		ByteBuffer(uint4 size = 0);
		ByteBuffer(byte1* buffer, uint4 size);
		~ByteBuffer();
		
		// Reader
		template<typename T>
		T Read() {
			if (sizeof(T) + mPosition > mSize) {
				mPosition = mSize;
				return 0;
			}

			T temp;
			memcpy(&temp, mByteContainer + mPosition, sizeof(T));
			mPosition += sizeof(T);
			return temp;
		}

		template<typename T>
		void Read(T* dest) {
			if (sizeof(T) + mPosition > mSize) {
				mPosition = mSize;
				return;
			}

			memcpy(dest, mByteContainer + mPosition, sizeof(T));
			mPosition += sizeof(T);
		}

		template<typename T>
		void ReadArray(T* destStartPtr, uint4 count) {
			if (count == 0) return;
			if ((sizeof(T) * count + mPosition) > mSize) {
				mPosition = mSize;
				return;
			}

			memcpy(destStartPtr, mByteContainer + mPosition, sizeof(T) * count);
			mPosition += sizeof(T) * count;
		}

		// Writer
		template<typename T>
		void Write(const T& data) {
			if (sizeof(T) + mPosition > mSize) {
				Expand((mPosition + sizeof(T)) - mSize);
			}

			memcpy(mByteContainer + mPosition, &data, sizeof(T));
			mPosition += sizeof(T);
		}

		template<typename T>
		void WriteArray(T* destStartPtr, uint4 count) {
			if (count == 0) return;
			if ((sizeof(T) * count + mPosition) > mSize) {
				Expand((mPosition + (sizeof(T) * count)) - mSize);
			}

			memcpy(mByteContainer + mPosition, destStartPtr, sizeof(T) * count);
			mPosition += sizeof(T) * count;
		}

		bool EndReached() { return mPosition >= mSize; }

		uint4& Position() { return mPosition; }
		uint4 Size() { return mSize; }
		uint4 BytesRemaining() { return mSize - mPosition; }

		byte1* Data() { return mByteContainer; }
		void Expand(uint4 bytesToAdd);

		void SavePosition() { mPositionHistory.push_back(mPosition); }
		void RestorePosition() { mPosition = mPositionHistory.back(); mPositionHistory.pop_back(); }
		void PreparePosition(uint4 newPos) { SavePosition(); mPosition = newPos; }

		static bool GetBitFromByte(int bitId, const byte1& byte);
		static void SetBitOfByte(int bitId, bool bit, byte1* byte);

	private:
		byte1* mByteContainer;
		uint4 mPosition, mSize;

		std::vector<uint4> mPositionHistory;

};

#endif
