#include "RingBuff.h"

#include <assert.h>
#include <stdint.h>		// uint32_t
#include <stdlib.h>		// calloc();


namespace loong
{
	RingBuff::RingBuff(uint32_t capacity) :
		m_capacity(capacity),
		m_headPos(0),
		m_tailPos(0),
		m_numElements(0)
	{
		// Create space for buffer, also 0 buffer (although
		// not strictly needed)
		//this->m_buffMemPtr = (uint8_t*)calloc(capacity, sizeof(char));
		this->m_buffMemPtr = new uint8_t[100];

		// In an embedded environment no exception may be thrown for bad alloc
		assert(this->m_buffMemPtr);
	}

	RingBuff::~RingBuff()
	{
		// Free memory allocated in constructor
		delete[] this->m_buffMemPtr;
	}

	uint32_t RingBuff::Write(const uint8_t *buff, uint32_t numBytes, ReadWriteLogic writeLogic)
	{
		//std::cout << "numBytes = '" << numBytes << "' .numElements = '" << this->m_numElements << "'." << std::endl;

		// Check whether user only wants to write data if all data
		// will fit in buffer
		if (writeLogic == ALL)
		{
			if (numBytes > (this->m_capacity - this->m_numElements))
			{
				// Not enough space in buffer to write all elements,
				// return 0
				//! @debug
				//std::cout << "numBytes '" << numBytes << "' > numElements '" << this->m_numElements << "'." << std::endl;
				return 0;
			}
		}

		const uint8_t * currPos;
		currPos = buff;

		for (uint32_t i = 0; i < numBytes; i++)
		{
			if (this->m_numElements >= this->m_capacity)
			{
				// We have run out of space! Return how many bytes
				// we managed to write
				//std::cout << "Run out of space!" << std::endl;
				return i;
			}

			// Write one byte to buffer
			m_buffMemPtr[this->m_headPos] = *currPos++;

			//! @debug
			//std::cout << "Element '" << m_buffMemPtr[this->m_headPos] << "' written to buff pos '" << this->m_headPos << "'." << std::endl;

			// Increment the head
			this->m_headPos++;

			// Increment the number of elements
			this->m_numElements++;

			// Check for wrap-around
			if (this->m_headPos == this->m_capacity)
			{
				m_headPos = 0;
			}

		}

		// All bytes where written
		return numBytes;
	}

	uint32_t RingBuff::Write(const uint8_t *buff, uint32_t numBytes)
	{
		// Simplified overload of Write(const uint8_t *buff, uint32_t numBytes, ReadWriteLogic writeLogic),
		// default behaviour is to only write to buffer if all elements will fit
		return this->Write(buff, numBytes, ALL);
	}

	uint32_t RingBuff::Write(const char *string, ReadWriteLogic writeLogic)
	{
		bool nullFound = false;
		uint32_t x;

		// nullptr character can be one more element past capacity, since
		// it won't be written to the buffer
		for (x = 0; x < this->m_capacity + 1; x++)
		{
			// Look for nullptr-terminating string.
			if (string[x] == '\0')
			{
				nullFound = true;
				break;
			}
		}

		if (nullFound)
		{
			// nullptr has been found with limits of buffer capacity, valid string, write to buffer
			//std::cout << "nullptr found! Writing '" << x << "' elements." << std::endl;
			return this->Write((const uint8_t*)string, x, writeLogic);
		}
		else
		{
			// nullptr has not be found within limits of buffer capacity, either string was not nullptr-terminated, or was too large
			// to fit in buffer completely.
			//std::cout << "nullptr not found!" << std::endl;

			// In this case, only write stuff if ReadWriteLogic is ANY
			if (writeLogic == ANY)
				return this->Write((const uint8_t*)string, this->m_capacity, writeLogic);
			else if (writeLogic == ALL)
				// Since we know the string cannot fit in the buffer, return 0 here
				return 0;
		}

		// If code gets here something bad has happened
		return 0;
	}

	uint32_t RingBuff::Write(const char *string)
	{
		// Simplified overload of Write(const uint8_t *buff, uint32_t numBytes, ReadWriteLogic writeLogic),
		// default behaviour is to only write to buffer if all elements will fit
		return this->Write(string, ALL);
	}

	bool RingBuff::Write(char charToWrite)
	{
		// Write a single byte to the buffer, if there is space.
		if (this->Write((uint8_t*)&charToWrite, 1, ALL) == 1)
			// Byte was written to buffer successfully
			return true;
		else
			// Byte was not written to buffer successfully
			return false;
	}

	uint32_t RingBuff::Peek(uint8_t *buff, uint32_t numBytes)
	{
		uint32_t i;

		// Remembers the current position in buff, the buffer to write to
		uint32_t currPosInBuff = 0;

		// Remebers where we are currently reading from in the RingBuff
		uint32_t peekTailPos = this->m_tailPos;

		// We don't want to adjust the tail as we would do in the case of
		// a Read().
		for (i = 0; i < numBytes; i++)
		{

			// Check if there is any (more) data is available
			if (i >= this->m_numElements)
			{
				// No more data available, return the number of elements
				// we managed to read
				return i;
			}

			// Read one byte from the FIFO buffer
			buff[currPosInBuff++] = this->m_buffMemPtr[peekTailPos++];

			// Decrement the number of elements
			//this->m_numElements--;

			// Increment the peek tail pos
			//peekTailPos++;

			 // Increment the tail
			//this->m_tailPos++;

			// Check for wrap-around
			if (peekTailPos == this->m_capacity)
			{
				// Reset peek tail
				peekTailPos = 0;
			}
		}

		// All bytes were read
		return numBytes;
	}

	uint32_t RingBuff::Read(uint8_t *buff, uint32_t numBytes)
	{
		uint32_t i;
		uint8_t * currPos;
		currPos = buff;

		for (i = 0; i < numBytes; i++)
		{
			//
			// Check if any data is available
			if (this->m_numElements == 0)
			{
				// No more data available, return the number of elements
				// we managed to read
				return i;
			}

			// Read one byte from the FIFO buffer
			*currPos++ = this->m_buffMemPtr[this->m_tailPos];

			// Decrement the number of elements
			this->m_numElements--;

			// Increment the tail
			this->m_tailPos++;

			// Check for wrap-around
			if (this->m_tailPos == this->m_capacity)
			{
				// Reset tail
				this->m_tailPos = 0;
			}
		}

		// All bytes were read
		return numBytes;
	}

	uint8_t RingBuff::Read()
	{
		if (!this->IsData())
			return 0;

		// There is data present, so read out
		// 1 element
		uint8_t data;
		RingBuff::Read(&data, 1);

		// Return data element (note that this could be 0)
		return data;
	}

	void RingBuff::Clear()
	{
		// Does not 0 data, as this does not matter,
		// Just resets tail, head and number of elements
		this->m_tailPos = 0;
		this->m_headPos = 0;
		this->m_numElements = 0;
	}

	uint32_t RingBuff::Capacity() const
	{
		// Just return the saved size of the buffer
		return this->m_capacity;
	}


	uint32_t RingBuff::NumElements() const
	{
		return this->m_numElements;
	}

	bool RingBuff::IsData() const
	{
		// Return true if there are any data elements currently in the buffer
		if (this->m_numElements > 0)
			return true;
		else
			return false;
	}

	bool RingBuff::IsSpace() const
	{
		if (this->m_numElements < this->m_capacity)
			// There is at least one element free in buffer
			return true;
		else
			// There are no elements free in buffer
			return false;

	}

	bool RingBuff::Resize(uint32_t newCapacity)
	{
		// First, shuffle all data backwards so that tailPos is at 0
		// this makes resizing easier
		this->ShiftElementsSoTailPosIsZero();

		// Now realloc() the memory. realloc() is guaranteed to preserve as much data as possible
		// Since elements have been shifted to start of buffer this should work as intended
		this->m_buffMemPtr = (uint8_t*)realloc(this->m_buffMemPtr, newCapacity);

		// Make sure realloc() was successful
		if (this->m_buffMemPtr != nullptr)
		{
			// Update headPos if old headpos is larger than new capacity, as headPos will be
			// currently out of range
			if (this->m_numElements > newCapacity)
			{
				// Update headpos to end of memory (memory is full with data)
				this->m_headPos = newCapacity - 1;
				this->m_numElements = newCapacity;
			}

			// Update capacity
			this->m_capacity = newCapacity;

			return true;
		}
		else
			// realloc failed! Don't update capacity and return false
			return false;

	}

	void RingBuff::ShiftElementsSoTailPosIsZero()
	{
		uint32_t currNumElements = this->NumElements();

		// Create a temp buffer memory space for copying
		// existing data into
		uint8_t tempBuffMem[currNumElements];

		// Read all data into temp memory
		this->Read(tempBuffMem, currNumElements);

		// Reset the head and tail positions back to 0, numElements does not change.
		this->m_headPos = 0;
		this->m_tailPos = 0;

		// Now write all data back, tailPos should stay at 0
		this->Write(tempBuffMem, currNumElements);

		// Done!
	}
}

