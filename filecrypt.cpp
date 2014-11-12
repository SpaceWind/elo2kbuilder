//---------------------------------------------------------------------------


#pragma hdrstop

#include "filecrypt.h"
int LFSRGenerator16::NextBit()
{
        s = ((( (s>>15)^(s>>14)^(s>>12)^(s>>3)) & 1 ) << 15 ) | (s>>1);
        return s & 1;
}
uint LFSRGenerator16::NextInt()
{
        uint temp = 0;
        for (int i = 0; i<sizeof (uint)*8; i++)
                temp = temp | NextBit()<<i;
        return temp;
}

int LFSRGenerator17::NextBit()
{
        s = ((((s>>16)^(s>>13)) & 1 ) << 16 ) | (s>>1);
        return s & 1;
}
uint LFSRGenerator17::NextInt()
{
        uint temp = 0;
        for (int i = 0; i<sizeof (uint)*8; i++)
                temp = temp | NextBit()<<i;
        return temp;
}
int LFSRGenerator32::NextBit()
{
        s = ((((s>>31)^(s>>21)^(s>>1)^s) & 1 ) << 31 ) | (s>>1);
        return s&1;
}
uint LFSRGenerator32::NextInt()
{
        uint temp = 0;
        for (int i = 0; i<sizeof (uint)*8; i++)
                temp = temp | NextBit()<<i;
        return temp;
}
void LFSRManager::SetLFSRMode(uint m, uint seed)
{
        switch (m)
        {
        case LFSR_MODE_MANUAL:
        default:
                if (mode)
                        delete mode;
                mode = 0;
                s = seed;
                break;
        case LFSR_MODE_16BIT:
                if (mode)
                        delete mode;
                mode = new LFSRGenerator16();
                mode->s = seed;
                break;
        case LFSR_MODE_17BIT:
                if (mode)
                        delete mode;
                mode = new LFSRGenerator17();
                mode->s = seed;
                break;
        case LFSR_MODE_32BIT:
                if (mode)
                        delete mode;
                mode = new LFSRGenerator32();
                mode->s = seed;
        }
}
int LFSRManager::NextBit()
{
        if (mode)
                return mode->NextBit();
        uint reverseBit=0;
        for (uint i=0; i<tapsCount; i++)
                reverseBit^=s>>taps[i];
        reverseBit&=1;
        reverseBit<<=LFSRSize-1;
        return (s = reverseBit | (s>>1))&1;
}
uint LFSRManager::NextInt()
{
        uint temp = 0;
        for (int i = 0; i<sizeof (uint)*8; i++)
                temp = temp | NextBit()<<i;
        return temp;
}
byte * LFSRManager::GenerateGamma(uint len)
{
        uint cnt = len/sizeof(uint)+1;
        uint * data = new uint[cnt];
        for (uint i=0; i<cnt; i++)
                data[i]=this->NextInt();
        return (byte*)data;
}
//---------------------------------------------------------------------------
void LFGManager::AllocateData(uint size, uint maxLag)
{
        if (values)
                delete [] values;
        if (normalized_values)
                delete [] normalized_values;
        values = new float [size/4+maxLag+1];
        normalized_values = new byte [size];
        this->size=size;
        l.a=maxLag;
        current = 0;
}
bool LFGManager::AddStartValue(uint value, uint maxValue)
{
        if (current>=l.a)
                return 0;
        values [current] = double(value)/double(maxValue);
        current++;
        return 1;
}
bool LFGManager::AddStartValue(float value)
{
        if (current>=l.a)
                return 0;
        values [current] = value;
        current++;
        return 1;
}
void LFGManager::GenerateGamma(lags lg)
{
        l = lg;
        for (;current<size/4+l.a+1; current++)
        {
                if (values[current-l.a] >= values[current-l.b])
                        values [current] = values[current-l.a]-values[current-l.b];
                else
                        values [current] = values[current-l.b]-values[current-l.a];
        }
}
void LFGManager::Normalize(uint maxValue)
{
        for (uint i = l.a; i<size/4+l.a+1; i++)
        {
                uint t = (pow(values[i]*double(maxValue), 2) + sqrt(values[i]*double(maxValue)))/2. + ((i%0xffffff)<<8);
                memcpy(&(normalized_values[(i-l.a)*sizeof(uint)]), &t, sizeof(uint));
        }
}
byte * LFGManager::GetArray()
{
        return normalized_values;
}
void LFGManager::SaveToFile(String filename)
{
        FILE * f = fopen (filename.c_str(), "w+b");
        fwrite (normalized_values, 1, size*sizeof(size), f);
        fclose(f);
}
//---------------------------------------------------------------------------
uint CreateSeedFromString(String keyWord)
{
        TIdCoderMD5 * c = new TIdCoderMD5(0);
        c->AutoCompleteInput = true;
        String hash;
        hash = c->CodeString(keyWord);
        char str[16];
        memcpy(str, hash.c_str(),16);
        for (int i=0; i<12; i++)
                str[i%4] ^= str[(i+4)%16];
        uint t;
        memcpy(&t, str, sizeof(uint));
        delete c;
        return t;
}
uint CreateHashFromString(String keyWord)
{
        TIdCoderMD5 * c = new TIdCoderMD5(0);
        c->AutoCompleteInput = true;
        String hash;
        hash = c->CodeString(keyWord);
        char str[16];
        memcpy(str, hash.c_str(),16);
        for (uint i=4; i<16; i++)
                str[i%8] ^= str[(i+4)%16];
        uint t;
        memcpy(&t, str, sizeof(uint));
        delete c;
        return t;
}
void EncodeFile(String inFileName, String outFileName, String keyWord)
{
        EncodeFile(inFileName, outFileName,CreateSeedFromString(keyWord));
}

void EncodeFile(String inFilename, String outFilename, uint seed)
{
        FILE * f = fopen(inFilename.c_str(), "rb");
        fseek(f,0,2);
        uint len = ftell(f);
        byte * Data = new byte [len];
        fseek(f,0,0);
        fread(Data,1,len,f);
        fclose(f);
        LFSRManager * lfsrm = new LFSRManager(LFSR_MODE_32BIT, seed);

        byte * gamma = lfsrm->GenerateGamma(len);
        for (uint i=0; i<len; i++)
                Data[i]^=gamma[i];
        f = fopen (outFilename.c_str(), "wb");
        fwrite(Data, 1, len, f);
        fclose(f);
        delete [] Data;
        delete [] gamma;
        delete lfsrm;
}
//---------------------------------------------------------------------------

#pragma package(smart_init)
