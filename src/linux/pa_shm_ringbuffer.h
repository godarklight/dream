#ifndef PA_SHM_RINGBUFFER_H
#define PA_SHM_RINGBUFFER_H
/*
 * $Id: pa_shm_ringbuffer.h,v 1.3 2009/12/23 14:37:22 jcable Exp $
 * Portable Audio I/O Library
 * Ring Buffer utility.
 *
 * Author: Phil Burk, http://www.softsynth.com
 * modified for SMP safety on OS X by Bjorn Roche.
 * also allowed for const where possible.
 * Note that this is safe only for a single-thread reader
 * and a single-thread writer.
 *
 * This program is distributed with the PortAudio Portable Audio Library.
 * For more information see: http://www.portaudio.com
 * Copyright (c) 1999-2000 Ross Bencina and Phil Burk
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * The text above constitutes the entire PortAudio license; however,
 * the PortAudio community also makes the following non-binding requests:
 *
 * Any person wishing to distribute modifications to the Software is
 * requested to send the modifications to the original developer so that
 * they can be incorporated into the canonical version. It is also
 * requested that these non-binding requests be included along with the
 * license above.
 */

/** @file
 @ingroup common_src
*/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    typedef struct PaUtilShmRingBuffer
    {
        long   bufferSize; /* Number of bytes in FIFO. Power of 2. Set by PaUtil_InitShmRingBuffer. */
        long   writeIndex; /* Index of next writable byte. Set by PaUtil_AdvanceShmRingBufferWriteIndex. */
        long   readIndex;  /* Index of next readable byte. Set by PaUtil_AdvanceShmRingBufferReadIndex. */
        long   bigMask;    /* Used for wrapping indices with extra bit to distinguish full/empty. */
        long   smallMask;  /* Used for fitting indices to buffer. */
    } PaUtilShmRingBuffer;

    /** Initialize Ring Buffer.

     @param rbuf The ring buffer.

     @param numBytes The number of bytes in the buffer and must be power of 2.

     @param dataPtr A pointer to a previously allocated area where the data
     will be maintained.  It must be numBytes long.

     @return -1 if numBytes is not a power of 2, otherwise 0.
    */
    long PaUtil_InitializeShmRingBuffer( PaUtilShmRingBuffer *rbuf, long numBytes, void *dataPtr );

    /** Clear buffer. Should only be called when buffer is NOT being read.

     @param rbuf The ring buffer.
    */
    void PaUtil_FlushShmRingBuffer( PaUtilShmRingBuffer *rbuf );

    /** Retrieve the number of bytes available in the ring buffer for writing.

     @param rbuf The ring buffer.

     @return The number of bytes available for writing.
    */
    long PaUtil_GetShmRingBufferWriteAvailable( PaUtilShmRingBuffer *rbuf );

    /** Retrieve the number of bytes available in the ring buffer for reading.

     @param rbuf The ring buffer.

     @return The number of bytes available for reading.
    */
    long PaUtil_GetShmRingBufferReadAvailable( PaUtilShmRingBuffer *rbuf );

    /** Write data to the ring buffer.

     @param rbuf The ring buffer.

     @param data The address of new data to write to the buffer.

     @param numBytes The number of bytes to be written.

     @return The number of bytes written.
    */
    long PaUtil_WriteShmRingBuffer( PaUtilShmRingBuffer *rbuf, const void *data, long numBytes );

    /** Read data from the ring buffer.

     @param rbuf The ring buffer.

     @param data The address where the data should be stored.

     @param numBytes The number of bytes to be read.

     @return The number of bytes read.
    */
    long PaUtil_ReadShmRingBuffer( PaUtilShmRingBuffer *rbuf, void *data, long numBytes );

    /** Get address of region(s) to which we can write data.

     @param rbuf The ring buffer.

     @param numBytes The number of bytes desired.

     @param dataPtr1 The address where the first (or only) region pointer will be
     stored.

     @param sizePtr1 The address where the first (or only) region length will be
     stored.

     @param dataPtr2 The address where the second region pointer will be stored if
     the first region is too small to satisfy numBytes.

     @param sizePtr2 The address where the second region length will be stored if
     the first region is too small to satisfy numBytes.

     @return The room available to be written or numBytes, whichever is smaller.
    */
    long PaUtil_GetShmRingBufferWriteRegions( PaUtilShmRingBuffer *rbuf, long numBytes,
            void **dataPtr1, long *sizePtr1,
            void **dataPtr2, long *sizePtr2 );

    /** Advance the write index to the next location to be written.

     @param rbuf The ring buffer.

     @param numBytes The number of bytes to advance.

     @return The new position.
    */
    long PaUtil_AdvanceShmRingBufferWriteIndex( PaUtilShmRingBuffer *rbuf, long numBytes );

    /** Get address of region(s) from which we can write data.

     @param rbuf The ring buffer.

     @param numBytes The number of bytes desired.

     @param dataPtr1 The address where the first (or only) region pointer will be
     stored.

     @param sizePtr1 The address where the first (or only) region length will be
     stored.

     @param dataPtr2 The address where the second region pointer will be stored if
     the first region is too small to satisfy numBytes.

     @param sizePtr2 The address where the second region length will be stored if
     the first region is too small to satisfy numBytes.

     @return The number of bytes available for reading.
    */
    long PaUtil_GetShmRingBufferReadRegions( PaUtilShmRingBuffer *rbuf, long numBytes,
            void **dataPtr1, long *sizePtr1,
            void **dataPtr2, long *sizePtr2 );

    /** Advance the read index to the next location to be read.

     @param rbuf The ring buffer.

     @param numBytes The number of bytes to advance.

     @return The new position.
    */
    long PaUtil_AdvanceShmRingBufferReadIndex( PaUtilShmRingBuffer *rbuf, long numBytes );

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* PA_RINGBUFFER_H */
