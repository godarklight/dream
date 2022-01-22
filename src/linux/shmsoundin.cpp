/******************************************************************************\
 * British Broadcasting Corporation
 * Copyright (c) 2007
 *
 * Author(s):
 *	Julian Cable
 *
 * Decription:
 * Sound in interface using POSIX shared memory
 *
 ******************************************************************************
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
\******************************************************************************/

#define _POSIX_C_SOURCE 199309
#include <time.h>
#include <sys/types.h>
#include <iostream>
# include <sys/mman.h>
# include <fcntl.h>
# include <unistd.h>
# include <sys/stat.h>
# include "shmsoundin.h"
using namespace std;

CShmSoundIn::CShmSoundIn():ringBuffer(NULL),
        shmid(-1),shm(NULL),shm_path(),name("shm input"),shmChannels(1),wantedChannels(2)
{
}

CShmSoundIn::~CShmSoundIn()
{
    Close();
}

/* iNewBufferSize is the number of samples to read, e.g. stereo is
 * 2 samples per frame
 */
void
CShmSoundIn::Init(int iNewBufferSize, bool bNewBlocking)
{
    // TODO decide if we should use the parameters
    (void)iNewBufferSize;
    (void)bNewBlocking;
    shmid = shm_open(shm_path.c_str(), O_RDWR, 0666);
    if (shmid == -1)
    {
        perror("shm_open");
        return;
    }
    struct stat s;
    fstat(shmid, &s);
    shm = mmap(0, s.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, shmid, 0);
    if ((void*)shm == (void*)-1)
    {
        perror("mmap");
        return;
    }
    ringBuffer = (PaUtilShmRingBuffer*)shm;
}

void
CShmSoundIn::Enumerate(vector < string > &choices)
{
    choices.clear();
    if (shmid==-1)
        return;
    choices.push_back(name);
}

void
CShmSoundIn::SetDev(int iNewDevice)
{
    (void)iNewDevice;
}

int
CShmSoundIn::GetDev()
{
    if (shmid==-1)
        return -1;
    return 0;
}

bool
CShmSoundIn::Read(CVector<short>& psData)
{
    if (ringBuffer==NULL)
        return false;

    size_t frames;
    if (wantedChannels==2)
        frames = psData.Size()/2;
    else
        frames = psData.Size();

    size_t samples,bytes;
    if (shmChannels==2)
        samples = 2*frames;
    else
        samples = frames;
    bytes = sizeof(short)*samples;


    while (PaUtil_GetShmRingBufferReadAvailable(ringBuffer)<int(bytes))
    {
        timespec ts;
        ts.tv_sec=0;
        ts.tv_nsec = 10000000; // 10 ms
        nanosleep(&ts, NULL);
    }

    if (shmChannels==wantedChannels)
    {
        PaUtil_ReadShmRingBuffer(ringBuffer, &psData[0], bytes);
    }
    else
    {
        short buf[samples];
        PaUtil_ReadShmRingBuffer(ringBuffer, &buf[0], bytes);
        for (size_t i=0; i<frames; i++)
        {
            if (wantedChannels==2) /* mono to stereo */
            {
                psData[2*i] = buf[i]/2;
                psData[2*i+1] = buf[i]/2;
            }
            else /* stereo to mono */
            {
                psData[i] = (buf[2*i] + buf[2*i+1])/2;
            }
        }
    }

    return true;
}

void
CShmSoundIn::Close()
{
    struct stat s;
    fstat(shmid, &s);
    munmap(shm, s.st_size);
    shm_unlink(shm_path.c_str());
    cout << "capture close" << endl;
}
