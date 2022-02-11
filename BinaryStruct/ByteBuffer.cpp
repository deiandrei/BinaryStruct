#include "ByteBuffer.h"

ByteBuffer::ByteBuffer(uint4 size) {
	mPosition = 0;
	mSize = size;
	mByteContainer = nullptr;

	Expand(size);
}

ByteBuffer::ByteBuffer(byte1* buffer, uint4 size) : ByteBuffer(size) {
	memcpy(mByteContainer, buffer, size);
}

ByteBuffer::~ByteBuffer() {
	delete[] mByteContainer;
}

void ByteBuffer::Expand(uint4 bytesToAdd) {
	if (!bytesToAdd) return;

	if (mSize == 0) {
		mByteContainer = new byte1[bytesToAdd];
	}
	else {
		mByteContainer = (byte1*)realloc(mByteContainer, mSize + bytesToAdd);
	}

	mSize += bytesToAdd;
}

bool ByteBuffer::GetBitFromByte(int bitId, const byte1& byte) {
	return ((byte >> bitId) & 1);
}

void ByteBuffer::SetBitOfByte(int bitId, bool bit, byte1* byte) {
	if (bitId >= 8) return;

	*byte ^= (-bit ^ *byte) & (1UL << bitId);
}
