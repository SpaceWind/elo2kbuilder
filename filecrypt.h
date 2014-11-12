//---------------------------------------------------------------------------

#ifndef filecryptH
#define filecryptH
#include <vcl.h>
#include <stdio.h>
#include <IdCoder.hpp>
#include <IdCoderMessageDigest.hpp>
#include <math.h>

#define LFSR_MODE_16BIT         0xAA000001
#define LFSR_MODE_17BIT         0xAA000002
#define LFSR_MODE_32BIT         0xAA000003
#define LFSR_MODE_MANUAL        0xAA0000FF
#define LFSR_DEFAULT_SEED       0x1F5E361B
#define LFG_NORMALIZE_VALUE_9   0x3B9ACA00
#define LFG_NORMALIZE_VALUE_M   0xFFFFFFFF


typedef unsigned int uint;
typedef unsigned char byte;

uint CreateSeedFromString(String keyWord);
uint CreateHashFromString(String keyWord);
void EncodeFile(String inFilename, String outFilename, String keyWord);
void EncodeFile(String inFilename, String outFilename, uint seed);

struct lags
{
        lags(){a=0; b=0;}
        lags (uint _a,uint _b) {a=_a; b=_b;}
        uint a,b;
};
class LFSRGenerator
{
public:
        uint s;
        virtual int NextBit()=0;
        virtual uint NextInt()=0;
};
class LFSRGenerator16:public LFSRGenerator
{
public:
        virtual int NextBit();
        virtual uint NextInt();
};
class LFSRGenerator17:public LFSRGenerator
{
public:
        virtual int NextBit();
        virtual uint NextInt();
};
class LFSRGenerator32:public LFSRGenerator
{
public:
        virtual int NextBit();
        virtual uint NextInt();
};


class LFSRManager
{
public:
        LFSRManager () {memset(this,0,sizeof(LFSRGenerator));}
        LFSRManager (uint mode, uint seed = LFSR_DEFAULT_SEED){memset(this,0,sizeof(LFSRGenerator)); this->mode=0; SetLFSRMode(mode, seed);}
        ~LFSRManager() {if (mode) delete mode;}
        uint s;
        uint taps[32];
        uint tapsCount;
        uint LFSRSize;

        void SetLFSRMode(uint mode, uint seed);

        byte * GenerateGamma(uint len);
        int NextBit();
        uint NextInt();
private:
        LFSRGenerator * mode;
};

class LFGManager
{
public:
        LFGManager(){memset(this, 0, sizeof(LFGManager));}
        ~LFGManager(){if (values) {delete[]values;delete[]normalized_values;}}
        lags l;
        uint current;
        uint size;
        void AllocateData(uint size, uint maxLag);
        bool AddStartValue(uint value, uint maxValue);
        bool AddStartValue(float value);
        void GenerateGamma(lags lg);
        void Normalize(uint maxValue);
        byte * GetArray();
        void SaveToFile(String filename);
private:
        float * values;
        byte * normalized_values;


};
//---------------------------------------------------------------------------
#endif
