#include "HappyMath/StackHeap.h"

using namespace HappyMath;

StackHeap::StackHeap()
{
	this->memoryBuffer = nullptr;
	this->memoryBufferSize = 0;
	this->blockSize = 0;
}

/*virtual*/ StackHeap::~StackHeap()
{
}

bool StackHeap::SetManagedMemory(uint8_t* memoryBuffer, uint64_t memoryBufferSize, uint64_t blockSize)
{
	this->memoryBuffer = memoryBuffer;
	this->memoryBufferSize = memoryBufferSize;
	this->blockSize = blockSize;

	if (this->blockSize > this->memoryBufferSize)
		return false;

	if (this->memoryBufferSize % this->blockSize != 0)
		return false;

	this->blockStack.clear();
	uint64_t numBlocks = this->memoryBufferSize / this->blockSize;
	this->blockStack.reserve(numBlocks);
	for (uint64_t i = 1; i <= numBlocks; i++)
		this->blockStack.push_back(this->memoryBufferSize - i * this->blockSize);

	return true;
}

uint8_t* StackHeap::AllocateBlock()
{
	if (this->blockStack.size() == 0)
		return nullptr;

	uint64_t blockOffset = this->blockStack.back();
	uint8_t* block = &this->memoryBuffer[blockOffset];
	this->blockStack.pop_back();
	return block;
}

bool StackHeap::DeallocateBlock(uint8_t* block)
{
	uint64_t blockOffset = block - this->memoryBuffer;
	if (blockOffset + this->blockSize > this->memoryBufferSize || blockOffset % this->blockSize != 0)
		return false;

	uint64_t numBlocks = this->memoryBufferSize / this->blockSize;
	if (this->blockStack.size() == numBlocks)
		return false;

	this->blockStack.push_back(blockOffset);
	return true;
}

uint64_t StackHeap::NumFreeBlocks() const
{
	return this->blockStack.size();
}