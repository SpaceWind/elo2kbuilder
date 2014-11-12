//---------------------------------------------------------------------------


#pragma hdrstop

#include "progressbars.h"
ProgressBar::ProgressBar(String filename, FastBitmap* context)
{
        min = 0; max = 100; position = 0; text = false;
        Defines d;
        d.Load(filename);
        PBMP bmpEmpty = new BMP();
        PBMP bmpFull  = new BMP();
        bmpEmpty->LoadFromFile("sprites\\"+d.Get("empty"));
        bmpFull->LoadFromFile("sprites\\"+d.Get("full"));

        FastBitmap *empty = new FastBitmap(bmpEmpty);
        FastBitmap *full  = new FastBitmap(bmpFull);
        cfb = new ComplexFastBitmap();
        picture.init(cfb->init(empty,full),context);

        String mode = d.Get("mode");
        if (mode == "alpha")
                picture.setRenderMode(RM_ALPHA_MASK, StrToInt(d.Get("color")));
        else if (mode == "overlay")
                picture.setRenderMode(RM_OVERLAY, StrToInt(d.Get("color")));
        h = d.IsDefined("horizontal");
        l = d.IsDefined("left");
        if (d.IsDefined("max"))
                max = StrToInt(d.Get("max"));
        if (d.IsDefined("min"))
                min = StrToInt(d.Get("min"));
        if (d.IsDefined("text"))
        {
                text= true;
                textPrefix = d.Get("textprefix");
                PBMP b = new BMP();
                b->Width = bmpFull->Width;
                b->Height = bmpFull->Height;
                FastBitmap * tfb = new FastBitmap(b);
                tfb->b->Canvas->Font->Size = 16;
                tfb->b->Canvas->Brush->Color = clWhite;
                if (d.IsDefined("fontcolor"))
                        tfb->b->Canvas->Font->Color = StrToInt(d.Get("fontcolor"));
                textRender.init(tfb,cfb->getPicture());
                textRender.setRenderMode(RM_ALPHA_MASK, clWhite);

        }


}
ProgressBar::~ProgressBar()
{
        if (cfb)
                delete cfb;
}
void ProgressBar::setMin(int m)
{
        if (m<=max)
                min = max;
        else
                min = m;
        if (position<min)
                position = min;
        update();
}
void ProgressBar::setMax(int m)
{
        max = 0;
        if (m<=min)
                max = min;
        else
                max = m;
        if (position > max)
                position = max;
        update();
}
void ProgressBar::setProgress(int p)
{
        if (p<min)
                position = min;
        else if (p>max)
                position = max;
        else
                position = p;
        update();
}
void ProgressBar::setProgress(double p)
{
        if (p<0 || p>1)
                setProgress(int(p));
        else
        position = (double(max-min))*p + double(min);
        update();
}
void ProgressBar::setPosition(int left, int top)
{
        picture.setPosition(left,top);
}
void ProgressBar::setAlign(bool horizontal, bool left)
{
        h = horizontal;
        l = left;
}
bool ProgressBar::update()
{
        cfb->complex(getProgressDouble(),h,l);
        return true;
}
void ProgressBar::render()
{
        if (text)
        {
                textRender.getPicture()->b->Canvas->FillRect(Rect(0,0,textRender.getPicture()->b->Width,textRender.getPicture()->b->Height));
                //textRender.getPicture()->b->Canvas->Font->Size = textRender.getPicture()->b->Height/2;
                textRender.getPicture()->b->Canvas->Font->Style = TFontStyles()<<fsBold;
                textRender.getPicture()->b->Canvas->TextOutA(
                        (textRender.getPicture()->b->Width - textRender.getPicture()->b->Canvas->TextExtent(textPrefix+IntToStr(int(position))).cx)/2,
                        (textRender.getPicture()->b->Height - textRender.getPicture()->b->Canvas->TextExtent(textPrefix+IntToStr(int(position))).cy)/2,
                        textPrefix+IntToStr(int(position)));
                textRender.render();
        }
        picture.render();
}
//---------------------------------------------------------------------------
SpellProgressBar::SpellProgressBar():ProgressBar()
{
}
SpellProgressBar::SpellProgressBar(String filename, FastBitmap * context) :ProgressBar(filename, context)
{
}
void SpellProgressBar::render()
{
        if (text)
        {
                String txt = textPrefix;
                int firstDigit = int(max-position)/1000;
                
                int lastDigit = (int(max-position)-firstDigit*1000)/100;
                if (firstDigit == 0 && lastDigit == 0)
                {
                        picture.render();
                        return;
                }
                txt += IntToStr(firstDigit)+"."+IntToStr(lastDigit);

                textRender.getPicture()->b->Canvas->FillRect(Rect(0,0,textRender.getPicture()->b->Width,textRender.getPicture()->b->Height));
                textRender.getPicture()->b->Canvas->Font->Size = textRender.getPicture()->b->Height/2-1;
                textRender.getPicture()->b->Canvas->Font->Style = TFontStyles()<<fsBold;
                textRender.getPicture()->b->Canvas->TextOutA(
                        (textRender.getPicture()->b->Width - textRender.getPicture()->b->Canvas->TextExtent(txt).cx)/2,
                        (textRender.getPicture()->b->Height - textRender.getPicture()->b->Canvas->TextExtent(txt).cy)/2,
                         txt);
                textRender.render();
        }
        picture.render();
}

//---------------------------------------------------------------------------

#pragma package(smart_init)
