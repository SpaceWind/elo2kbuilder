//---------------------------------------------------------------------------

#ifndef progressbarsH
#define progressbarsH
#include <vcl.h>
#include "rendersystem.h"

class ProgressBar : public Renderable
{
public:
        ProgressBar(){cfb =0; min=0; max=100; position=0; h = true; l = true;}
        ProgressBar(String filename, FastBitmap * context);
        virtual ~ProgressBar();
        virtual void setMin(int m);
        virtual void setMax(int m);
        virtual int  getMin(){return min;}
        virtual int  getMax(){return max;}
        virtual void setProgress(int p);
        virtual void setProgress(double p);
        virtual int  getProgressInt(){return position;}
        virtual double getProgressDouble(){return double(position-min)/double(max-min);}
        virtual void setPosition(int left, int top);
        virtual void setAlign(bool horizontal, bool left);
        virtual void setContext(FastBitmap * context){picture.setContext(context);}
        FastBitmap* getContext(){return picture.getContext();}
        virtual void render();


        virtual void stepUp(int step=1) {setProgress(getProgressInt()+step);}
        virtual void stepDown(int step=1) {setProgress(getProgressInt()-step);}

        RenderObject picture;
protected:
        bool update();
        ComplexFastBitmap * cfb;
        RenderObject textRender;

        int min, max, position;

        bool h, l;
        bool text;
        String textPrefix;
};

class SpellProgressBar : public ProgressBar
{
public:
        SpellProgressBar();
        SpellProgressBar(String filename, FastBitmap * context);
        ~SpellProgressBar(){;}
        void render();

};
//---------------------------------------------------------------------------
#endif
