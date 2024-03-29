#include "xheaacsuperframe.h"
#include "../util/CRC.h"

XHEAACSuperFrame::XHEAACSuperFrame():AudioSuperFrame (),numChannels(0),superFrameSize(0),payload(),borders(),frameSize()
{    
}

void
XHEAACSuperFrame::init(const CAudioParam& audioParam, unsigned frameSize)
{
    numChannels = audioParam.AM_MONO?1:2;
    superFrameSize = frameSize;
    payload.resize(0);
    borders.resize(0);
}

unsigned XHEAACSuperFrame::getSuperFrameDurationMilliseconds()
{
    return 0; // TODO is this variable or fixed?
}
        /*
     * 5.3.1.3 Transport of xHE-AAC audio frames within the payload section
    The USAC access unit encoder generates a continuous sequence of audio frames at a constant bit rate over the long term.
    The individual length of each audio frame in the continuous sequence is variable but constrained by the bit
    reservoir mechanism in the audio encoder to allow for improved audio quality. The encoder's bit reservoir buffer level is
    signalled to the decoder, to reduce required input buffer size and the extra tune-in delay to a minimum.

    Audio frame transport

    The xHE-AAC audio encoder generates a sequence of audio super frames (occupying one DRM logical frame for robustness modes A, B, C and D,
    or two DRM logical frames for robustness mode E). The audio frames as generated by the USAC access unit encoder are inserted into
    the Payload section of the audio super frame as a continuous byte sequence without any padding bytes in-between.
    Should padding be required to achieve the overall fixed bit rate and byte-alignment, it is inserted by the USAC access unit encoder
    into the audio frames themselves.

    The frame borders of audio frames do not need to be and typically will not be aligned with the audio super frame boundaries.
    Instead, audio frames are not synchronized to audio super frames; they continue from the current audio super frame into the Payload
    section of subsequent audio super frame(s). The frame borders within the Payload section of an audio super frame can be derived from
    the Header and Directory section of the audio super frame (there may be none!).

    An audio super frame shall not contaixn a Frame border description element without at least one byte of the corresponding audio frame data.
    If the available space in an audio super frame is not sufficient to hold at least 1 byte of the next audio frame in the Payload section
    plus the 2 bytes of the related extra Frame border description element in the Directory section, then the remaining space in the Payload
    section shall be filled with audio frame content, while the related Frame border description element is carried as the first Frame border
    description element in the Directory section of the following audio super frame (i.e. located at the end of the Directory section).
    The Frame border index value of such a delayed Frame border description element shall carry the special value 0xFFE or 0xFFF; with 0xFFF
    indicating the start of the audio frame at the last byte of the Payload section of the previous audio super frame. A decoder therefore
    always needs to buffer the last 2 bytes within the Payload section for a possible later processing along with the next audio super frame.
     */
    /*
    * The xHE-AAC audio super frame Header section has the following structure:
    • Frame border count
    • Bit reservoir level
    • Fixed header CRC
    The following definitions apply:
    4 bits. 4 bits. 8 bits
    * */
bool XHEAACSuperFrame::parse(CVectorEx<_BINARY>& asf)
{
    bool ok = true;
    unsigned frameBorderCount = asf.Separate(4);
    unsigned bitReservoirLevel = asf.Separate(4);
    CCRC CRCObject;
    CRCObject.Reset(8);
    CRCObject.AddByte((frameBorderCount << 4) | bitReservoirLevel);

    if(CRCObject.CheckCRC(asf.Separate(8))) {
        //cerr << endl << "superframe crc ok" << endl;
    }
    else {
        //cerr << endl << "superframe crc bad but will hope the frameBorderCount is OK" << endl;
    }
    // TODO handle reservoir
//    unsigned bitResLevel = (bitReservoirLevel+1) * 384 * numChannels;
    unsigned directory_offset = superFrameSize - 2*frameBorderCount;
    size_t start = payload.size();
    //cerr << start << " bytes left from previous superframe" << endl;
    //cerr << "payload start " << start << " bit reservoir level " << bitReservoirLevel << " bitResLevel " << bitResLevel << " superframe size " << superFrameSize << " directory offset " << 8*directory_offset << " bits " << directory_offset << " bytes" << endl;

    // get the payload
    for (size_t i=2; i < directory_offset; i++) {
        if (asf.Size() == 0) {
            printf("DRM xHE-AAC asf UNDERRUN 1\n");
            return false;
        }
        payload.push_back(asf.Separate(8));
    }
    borders.resize(frameBorderCount);
    frameSize.resize(frameBorderCount);


    //printf("xHE sfs=%d start=%zu diroff=%d fbc=%d ", superFrameSize, start, directory_offset, frameBorderCount);
    if (frameBorderCount > 0) {
        // get the directory
        for (int i = int(frameBorderCount-1); i >= 0; i--) {
            if (asf.Size() == 0) {
                printf("DRM xHE-AAC asf UNDERRUN 2\n");
                return false;
            }
            unsigned frameBorderIndex = asf.Separate(12);
            unsigned frameBorderCountRepeat = asf.Separate(4);
            if (frameBorderCountRepeat != frameBorderCount) {
                ok = false;
            }
            //cerr << "border " << i << " of " << frameBorderCountRepeat << "/" << frameBorderCount << " starts at " << hex << frameBorderIndex << dec << endl;
            borders[unsigned(i)] = frameBorderIndex;
            //printf("%d:%d ", i, frameBorderIndex);
        }
        //printf("\n");
        if (!ok) {
            //printf("xHE-FBC-repeat-BAD! ");
            return false;
        }

        // set the borders relative to the start including the payload bytes from previous superframes
        switch (borders[0]) {

            case 0xffe: // delayed from previous superframe
                //cerr << "first frame has two bytes in previous superframe" << endl;
                if (start < 2) {
                    //printf("xHE-0xffe! ");
                    return false;
                }
                borders[0] = start-2;
                frameSize[0] = borders[0];
                break;
    
            case 0xfff: // the start of the audio frame at the last byte of the Payload section of the previous audio super frame
                //cerr << "first frame has one byte in previous superframe" << endl;
                if (start < 1) {
                    //printf("xHE-0xfff! ");
                    return false;
                }
                borders[0] = start-1;
                frameSize[0] = borders[0];
                break;
    
            default: // boundary in this superframe
                borders[0] += start;
                #if 0
                    if (borders[0] < 2) return false;
                    borders[0] -= 2; // header not in payload
                #endif
                frameSize[0] = borders[0];
                //cerr << "border 0 is " << borders[0] << " bytes from start of payload" << endl;
                break;
        }

        for (unsigned i=1; i < borders.size(); i++) {
            borders[i] += start;
            #if 0
                borders[i] -= 2; // header not in payload
                unsigned bytes = borders[i] - borders[i-1];
            #else
                unsigned bytes = borders[i] - borders[i-1];
            #endif
            frameSize[i] = bytes;
            //cerr << "border " << i << " is " << borders[i] << " bytes from start of payload" << endl;
        }

        #if 0
            printf("xHE i:fb:fsize ");
            for (unsigned i=0; i < borders.size(); i++) {
                printf("%d:%d:%d ", i, borders[i], frameSize[i]);
            }
        #endif
    } else {
        // frameBorderCount == 0: spans entire ASF payload or bad data in header
        //printf("xHE-fullspan\n");
        audioFrame.resize(0);
        return ok;
    }

    size_t bytesInFrames = 0;
    for (size_t i=0; i < frameSize.size(); i++) bytesInFrames += frameSize[i];
    size_t next = payload.size() - bytesInFrames;
    //cerr << "payload is " << payload.size() << " bytes of which " << bytesInFrames << " are for this superframe and " << next << " are for the next superframe" << endl;

    //printf("xHE payload=%zu bytesInFrames=%zu next=%zu\n", payload.size(), bytesInFrames, next);

    // now copy into the audioFrames for simplicity
    size_t i=0;
    audioFrame.resize(frameBorderCount);
    audioFrame[i].resize(0);

    while (true) {
        #if 0
            if (i >= audioFrame.size()) {
                printf("audioFrame size: i=%d af_size=%d frameBorderCount=%d\n", (int) i, (int) audioFrame.size(), frameBorderCount);
                exit(-1);
            }
            int psize = payload.size();
            if (psize == 0) {
                printf("payload size: i=%d af_size=%d af_size[i]=%d frameSize=%d frameBorderCount=%d\n",
                    (int) i, (int) audioFrame.size(), audioFrame[i].size(), frameSize[i], frameBorderCount);
                //exit(-1);
            }
        #endif

        if (payload.empty()) {
            printf("DRM xHE-AAC payload UNDERRUN\n");
            return false;
        }
        audioFrame[i].push_back(payload.front());
        payload.pop_front();
        if (audioFrame[i].size() == frameSize[i]) {
            i++;
            if (i >= audioFrame.size()) break;
            audioFrame[i].resize(0);
        }
    }

    //cerr << "remaining payload is " << payload.size() << " bytes" << endl;
    //size_t allocated = 2 + 2*frameBorderCount; // bytes of the superframe for the header and directory
    //allocated += bytesInFrames - start; // do count bytes in frames but not the ones from previous superframes
    //allocated += next; // do count bytes for next superframe
    //cerr << "allocated " << allocated << " bytes out of " << superFrameSize << " in the superframe" << endl;
    return ok;
}

void
XHEAACSuperFrame::getFrame(std::vector<uint8_t>& frame, uint8_t& crc, unsigned i)
{
    frame = audioFrame[i];
}
