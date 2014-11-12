//---------------------------------------------------------------------------

#ifndef mainH
#define mainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <mmsystem.h>
#include "gamesystem.h"
#include "scenes.h"
#include "progressbars.h"
#include "kappa.h"
#include "map.h"
#include "mapManager.h"
#include "soundSystem.h"
#include <ExtCtrls.hpp>
#include <MPlayer.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
        TTimer *Timer1;
        TMediaPlayer *MusicPlayer;
        TButton *Button1;
        TMediaPlayer *MediaPlayer1;
        TTrackBar *TrackBar1;
        TButton *Button2;
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall FormActivate(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall TrackBar1Change(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TForm1(TComponent* Owner);

        //-------------------------------------------------------------------
        RenderManager * rm;
        GameManager * gm;
        FastBitmap * context;
        SpriteFabric * sf;
        Sprite * test1;
        Sprite * test2;
        Scene * sc;
        Map * map;

        MapManager * mmgr;

        ProgressBar * pb;


};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
