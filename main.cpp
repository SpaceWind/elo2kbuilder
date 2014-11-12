//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormCreate(TObject *Sender)
{

        PBMP bContext = new BMP();
        bContext->Width = ClientWidth;
        bContext->Height = ClientHeight;
        context = new FastBitmap(bContext);
        rm = new RenderManager();
        rm->setContext(context,Canvas,ClientWidth, ClientHeight, clWhite);
        DoubleBuffered=true;
        gm = new GameManager();
        gm->setRenderManager(rm);
        SpriteFabric *sf = SpriteFabric::getInstance();
        sc = 0;
        map = 0;
        SoundManager * sm = SoundManager::getInstance();
        sm->init(Button1);

        UserStats::getInstance()->loadFromFile("config/autoexec.txt");

}
//---------------------------------------------------------------------------
void __fastcall TForm1::Timer1Timer(TObject *Sender)
{

        if (!mmgr->isDone())
                mmgr->update();
        else
        {
                Timer1->Enabled = false;
                ShowMessage("system::mapIsOver");
                Application->Terminate();

        }
}
//---------------------------------------------------------------------------


void __fastcall TForm1::FormActivate(TObject *Sender)
{
        mmgr = new MapManager("config\\game.txt",gm);
        Timer1->Enabled=true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
        DeleteFile("sprites\\fbDefines.fabric");
}
//---------------------------------------------------------------------------


void __fastcall TForm1::TrackBar1Change(TObject *Sender)
{
        double leftV = double(100-TrackBar1->Position)/100.;
        double rightV = double(TrackBar1->Position)/100.;
        MPSetVolume(MediaPlayer1,leftV,rightV);
}
//---------------------------------------------------------------------------


