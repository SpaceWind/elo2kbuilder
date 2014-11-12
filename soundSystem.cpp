//---------------------------------------------------------------------------


#pragma hdrstop

#include "soundSystem.h"

void MPSetVolume(TMediaPlayer* mp, int vol, int channel)
{
        MCI_DGV_SETAUDIO_PARMS p;
        p.dwCallback = 0;
        p.dwItem = MCI_DGV_SETAUDIO_VOLUME;
        p.dwValue = vol;
        p.dwOver = 0;
        p.lpstrAlgorithm=0;
        p.lpstrQuality=0;

        mciSendCommand(mp->DeviceID,MCI_SETAUDIO,MCI_DGV_SETAUDIO_VALUE | channel | MCI_DGV_SETAUDIO_ITEM,  __int32(&p));
}
int MPGetVolume(TMediaPlayer* mp)
{
        MCI_STATUS_PARMS p;
        p.dwCallback = 0;
        p.dwItem = MCI_DGV_STATUS_VOLUME;
        mciSendCommand(mp->DeviceID, MCI_STATUS, MCI_STATUS_ITEM, __int32(&p)) ;
        return p.dwReturn;
}

void MPSetVolume(TMediaPlayer*mp, double left, double right)
{
        int leftV = left*double(1024);
        int rightV = right*double(1024);
        MPSetVolume(mp,leftV,CHANNEL_LEFT);
        MPSetVolume(mp,rightV,CHANNEL_RIGHT);
}

//---------------------------------------------------------------------------
SoundManager* SoundManager::getInstance()
{
        return soundManagerSingletonStruct.init?soundManagerSingletonStruct.instance:new SoundManager();
}
SoundManager::SoundManager()
{
        soundManagerSingletonStruct.init=true;
        soundManagerSingletonStruct.instance = this;
        parent = 0;
}
SoundManager::~SoundManager()
{
        binds.clear();
        map_foreach(String,TMediaPlayer*,it,resources)
                delete it->second;
        resources.clear();
}
void SoundManager::init(TObject * p)
{
        parent = p;
        loopNotify = LoopNotifyMethod;
}
void __fastcall SoundManager::LoopNotifyMethod(TObject *Sender)
{
        TMediaPlayer *mp = (TMediaPlayer*)Sender;
        if (mp->NotifyValue == nvSuccessful && mp->Mode != mpStopped)
        {
                mp->Rewind();
                mp->Play();
                mp->Notify=true;
        }
}
void SoundManager::addResource(String name, String filename, bool loop)
{
        std::map<String, TMediaPlayer*>::iterator it = resources.find(name);
        if (it != resources.end())
                delete it->second;
        TMediaPlayer * mp = new TMediaPlayer(parent);
        mp->Parent = (TWinControl*)parent;
        mp->Visible = false;
        mp->Wait = false;
        resources[name] = mp;
        if (loop)
        {
                mp->Notify = true;
                mp->OnNotify = loopNotify;
        }
        mp->FileName = filename;
        mp->Open();


}
void SoundManager::setResourceVolume(String name, double left, double right)
{
        std::map<String, TMediaPlayer*>::iterator it = resources.find(name);
        if (it != resources.end())
                MPSetVolume(it->second,left,right);
}
void SoundManager::setResourceVolume(String name, TDoubleRect& screenRect, TDoubleRect& itemsRect, TDoubleRect& coefs)
{
        std::map<String, TMediaPlayer*>::iterator it = resources.find(name);
        if (it == resources.end())
                return;
        double balance = 0;
        double masterVolume=1;

        double width = screenRect.Width();
        double height = screenRect.Height();
        double centerPointLeft = screenRect.left+width/2.;
        double centerPointTop  = screenRect.top+height/2.;

        double srcLeft=itemsRect.left;
        double srcTop =itemsRect.top;

        double listenerLeft = itemsRect.right;
        double listenerTop =  itemsRect.bottom;

        double itemBalanceCoef = ((srcLeft-listenerLeft)*coefs.left/width) +
                                 ((srcLeft-centerPointLeft)*coefs.right/width);
        balance = (max(min(itemBalanceCoef,1.),-1.)+1.)/2.;

        double itemVolume = (max(1. - (sqrt(pow(srcLeft-listenerLeft,2.)+pow(srcTop-listenerTop,2))/
                             sqrt(width*width+height*height)*coefs.top),0.));
        double scrVolume  = (max(1. - (sqrt(pow(srcLeft-centerPointLeft,2.)+pow(srcTop-centerPointTop,2))/
                             sqrt(width*width+height*height)*coefs.bottom),0.));
        masterVolume  = itemVolume*scrVolume;


        MPSetVolume(it->second, (1.-balance)*masterVolume, balance*masterVolume);

}
void SoundManager::bindResource(String name, soundBindDescriptor sbd)
{
        binds[name] = sbd;
}
void SoundManager::unbindResource(String name)
{
        std::map<String, soundBindDescriptor>::iterator it = binds.find(name);
        if (it != binds.end())
                binds.erase(it);
}
void SoundManager::updateBinds()
{
        map_foreach(String,soundBindDescriptor,it,binds)
        {
                TDoubleRect itemsRect;
                itemsRect.left = *(it->second.src.l);
                itemsRect.top = *(it->second.src.t);
                itemsRect.right = *(it->second.listener.l);
                itemsRect.bottom = *(it->second.listener.t);
                setResourceVolume(it->first, it->second.screenRect, itemsRect, it->second.coefs);
        }
}
void SoundManager::setBindScreenRect(TDoubleRect screen)
{
        map_foreach(String,soundBindDescriptor,it,binds)
                it->second.screenRect = screen;
}
void SoundManager::Play(String res)
{
        std::map<String, TMediaPlayer *>::iterator it = resources.find(res);
        if (it != binds.end())
                it->second->Play();
}
void SoundManager::Stop(String res)
{
        std::map<String, TMediaPlayer *>::iterator it = resources.find(res);
        if (it != binds.end())
                it->second->Stop();
}
void SoundManager::freeBinds()
{
        binds.clear();
}
void SoundManager::freeResource(String name)
{
        std::map<String, TMediaPlayer *>::iterator it = resources.find(name);
        std::map<String, soundBindDescriptor>::iterator bindIt = binds.find(name);
        if (it != resources.end())
        {
                delete it->second;
                resources.erase(it);
        }
        if (bindIt != binds.end())
                binds.erase(bindIt);

}
void SoundManager::freeAll()
{
        binds.clear();
        map_foreach(String,TMediaPlayer*,it,resources)
                delete it->second;
        resources.clear();
}
void SoundManager::setVolume(String res, double left, double right)
{
        std::map<String, TMediaPlayer *>::iterator it = resources.find(res);
        if (it != resources.end())
                MPSetVolume(it->second,left,right);
}



//---------------------------------------------------------------------------

#pragma package(smart_init)
