//---------------------------------------------------------------------------

#ifndef soundSystemH
#define soundSystemH
#include <vcl.h>
#include <mmsystem.h>
#include <MPlayer.hpp>
#include <map>

#include "utils.h"

#define MCI_SETAUDIO                    0x0873
#define MCI_DGV_SETAUDIO_VOLUME         0x4002
#define MCI_DGV_SETAUDIO_ITEM           0x00800000
#define MCI_DGV_SETAUDIO_VALUE          0x01000000
#define MCI_DGV_SETAUDIO_LEFT           0x00200000
#define MCI_DGV_SETAUDIO_RIGHT          0x00400000
#define MCI_DGV_STATUS_VOLUME           0x4019

#define CHANNEL_LEFT                    MCI_DGV_SETAUDIO_LEFT
#define CHANNEL_RIGHT                   MCI_DGV_SETAUDIO_RIGHT


struct MCI_DGV_SETAUDIO_PARMS
{
        DWORD dwCallback;
        DWORD dwItem;
        DWORD dwValue;
        DWORD dwOver;
        LPSTR lpstrAlgorithm;
        LPSTR lpstrQuality;
};



void MPSetVolume(TMediaPlayer* mp, int vol, int channel=0);
void MPSetVolume(TMediaPlayer*mp, double left, double right);
int  MPGetVolume(TMediaPlayer* mp);


struct soundBindDescriptor
{
        Pointer2D listener, src;
        TDoubleRect screenRect, coefs;
};

class SoundManager
{
public:
        static SoundManager* getInstance();
        ~SoundManager();
        void init(TObject * parent);
        void addResource(String name, String filename, bool loop);
        void setResourceVolume(String name, double left, double right);

        /*
                name - resource name;
                screenRect {scrollLeft, scrollTop, scrollLeft+width, scrollTop+height}
                itemsRect  {srcLeft, srcTop, listenersLeft, listenersTop}
                coefs {left = rangeLeft{0..1} per screen width. //balance |
                       top  = rangeLeft{0..1} per screen width. //volume  | in screen
                       right = rangeLeft{0..} per screen width  //balance |
                       bottom = rangeLeft{0..} per screen width //volume  | outside of screen
        */
        void setResourceVolume(String name, TDoubleRect& screenRect,
                                            TDoubleRect& itemsRect,
                                            TDoubleRect& coefs);

        //looping closure
        TNotifyEvent loopNotify;
        void __fastcall LoopNotifyMethod(TObject *Sender);

        void bindResource(String name, soundBindDescriptor sbd);
        void unbindResource(String name);
        void updateBinds();
        void setBindScreenRect(TDoubleRect screen);

        void Play(String res);
        void Stop(String res);
        void setVolume(String res, double left, double right);

        void freeBinds();
        void freeResource(String name);
        void freeAll();
private:
        SoundManager();

        std::map<String, TMediaPlayer*> resources;
        std::map<String, soundBindDescriptor> binds;
        TObject * parent;

};

struct SoundManagerSingleton
{
        friend SoundManager;
private:
        bool init;
        SoundManager * instance;
} soundManagerSingletonStruct;



//---------------------------------------------------------------------------
#endif
