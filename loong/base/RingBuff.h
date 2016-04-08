/*
*   File name: RingBuff.h
*	 Describe:
*  Created on: 2015-2-12
*      Author: dengxiudong
*/

#ifndef _MRING_BUFF_RING_BUFF_H_
#define _MRING_BUFF_RING_BUFF_H_


#include <stdint.h>

namespace loong
{
	enum ReadWriteLogic
	{
		ANY,			//!< Reads as much data as there is in buffer, writes what ever data will fit in buffer.
		ALL				//!< Will only read if specified num. of elements present,
						//!< will only write data if there is enough space for all data in buffer.
	};

	class RingBuff
	{

	public:

		//! @brief		Enumeration of different ways to read/write data.
		//! @details	Used with RingBuff::Write() and RingBuff::Read().

		//! @brief		Initialises the buffer (incl. malloc()ing memory for buffer), ready for use.
		//! @details	If initialisation fails, IsInitComplete() will return false. This method is used,
		//!				because exceptions are not supported (designed to be compatible with embedded
		//!				microcontrollers)
		//! @param		m_capacity	The size (in bytes) of the buffer.
		RingBuff(uint32_t capacity);

		//! @brief		Destructor. Frees memory allocated in constructor.
		~RingBuff();

		//! @brief		Writes a number of bytes to the ring buffer.
		//! @details	Will return early if there is no more space left in the buffer. Does
		//!				not write over contents. Returns 0 if IsInitComplete()
		//!				is false.
		//! @param		buff		Pointer to data in memory to read from (the stuff that will be
		//!							written to the ring buffer).
		//! @param		numBytes	The maximum number of bytes to write.
		//!	@param		writeLogic	If set to ANY, ring buffer will be written with as much data as possible,
		//!							even if all the data cannot fit. If set to ALL, data will be written
		//!							only if there is enough space for all of it.
		uint32_t Write(const uint8_t* buff, uint32_t numBytes, ReadWriteLogic writeLogic);

		//! @brief		Simplified overload of Write(const uint8_t* buff, uint32_t numBytes, WriteLogic writeLogic).
		//!	@details	writeLogic set to ALL
		uint32_t Write(const uint8_t* buff, uint32_t numBytes);

		//! @brief		Writes a nullptr-terminated string to the buffer. Does not write the nullptr character
		//!				to the buffer. Will not write anything (a returns 0) if nullptr-character cannot
		//!				be found or string is too large for buffer.
		//! @details	This looks for the nullptr character and then calls
		//!				uint32_t Write(const uint8_t* buff, uint32_t numBytes).
		//! @param		string		The nullptr-terminated string to write to the buffer.
		//!	@param		writeLogic	If set to ANY, ring buffer will be written with as much data as possible,
		//!							even if the whole string cannot fit. If set to ALL, data will be written
		//!							only if there is enough space for all of it.
		uint32_t Write(const char *string, ReadWriteLogic writeLogic);

		//! @brief		Simplified overload of Write(const char *string, WriteLogic writeLogic).
		//!	@details	writeLogic set to ALL.
		uint32_t Write(const char *string);

		//! @brief		Writes a single char to the ring buffer.
		//! @returns	True if write was successful, otherwise false (i.e. buffer was full, or buffer
		//!				did not initialise successfully).
		bool Write(char charToWrite);

		//! @brief		Reads (but DOESN'T remove) up to numBytes from the ring buffer.
		//! @details	Will read numBytes of data, unless there is not enough data to read, in which case
		//!				returns early.
		//! @returns	Number of bytes read (which could be from 0 to numBytes). Returns 0 if IsInitComplete()
		//!				is false.
		//! @sa			Read().
		uint32_t Peek(uint8_t* buff, uint32_t numBytes);

		//! @brief		Reads and removes up to #numBytes from the ring buffer.
		//! @details	Will read numBytes of data, unless there is not enough data to read, in which case
		//!				returns early.
		//! @returns	Number of bytes read (which could be from 0 to numBytes). Returns 0 if IsInitComplete()
		//!				is false.
		uint32_t Read(uint8_t* buff, uint32_t numBytes);

		//! @brief		Reads and removes a single element from the ring buffer, returns 0 if no element exists.
		//! @returns	The value of the data element read from the buffer, or 0 if no data exists
		//!				(i.e. buffer is empty).
		//! @warning	There is no way to tell from calling Read() if it had retrieved an element of
		//!				value '0' from the buffer, or it returned 0 because there was no data in the
		//!				buffer to read. For this reason if it best to check that there is data in the
		//!				buffer by calling IsData() before calling this method.
		uint8_t Read();

		//! @brief		Clears all data in the ring buffer.
		//! @details	Does not actually 0 data as this is not required, just sets tailPos = m_headPos.
		void Clear();

		//! @brief		Returns the size (capacity) of the buffer.
		//! 			This is NOT how many bytes are currently in the buffer, see NumElements().
		uint32_t Capacity() const;

		//! @brief		Returns the number of elements currently in the buffer. capacity() returns the m_capacity of
		//!				the buffer.
		uint32_t NumElements() const;

		//! @brief		Allows the user to quickly check if there is any data in the buffer.
		//! @returns	True is there is 1 or more data elements in the buffer, otherwise false.
		//! @sa			NumElements().
		bool IsData() const;

		//! @brief		Allows the user to quickly check if there is any space in the data to write
		//!				more elements.
		//! @returns	True if there is at least one element available to write to, otherwise false. Returns
		//!				false regardless if buffer initialisation wasn't successful.
		//! @sa			IsData().
		bool IsSpace() const;

		//! @brief		Attempts to resize the buffer memory.
		//! @details	If the buffer size is increased, all current data will be preserved. If the buffer
		//!				size is decreased, the largest amount of data possible will be preserved, starting
		//!				with the newest data written to the buffer.
		//! @param		newCapacity		The new m_capacity you wish to resize the buffer memory to.
		//! @returns	True is resize was successful (and m_capacity() will return new value), otherwise false.
		//!				Returns false regardless if buffer initialisation wasn't successful.
		bool Resize(uint32_t newCapacity);


	private:

		//! @brief		Shifts all the data in the buffer memory so that is "starts from the start", i.e.
		//!				tailPos = 0.
		//! @details	This is useful to do before resizing the buffer memory using realloc(). Used in
		//!				Resize().
		void ShiftElementsSoTailPosIsZero();

		//! @brief		Pointer to buffer. Memory allocated in constructor.
		uint8_t* m_buffMemPtr;

		//! @brief		The size of the buffer (in bytes). Set by Buffer().
		uint32_t m_capacity;

		//! @brief		The head position, measured in bytes from the start of the buffer (bufPtr).
		//! @details	This is the next element to write to.
		uint32_t m_headPos;

		//! @brief		The tail position, measured in bytes from the start of the buffer (bufPtr).
		//! @details	This is the next element to read from.
		uint32_t m_tailPos;

		//! @brief		Keeps track of the number of elements in the memory buffer.
		//! @details	Incremented by 1 for every element written to the buffer, decremented
		//!				by 1 for every element read from the buffer.
		uint32_t m_numElements;

	};
}

#endif 

