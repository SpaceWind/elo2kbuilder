//---------------------------------------------------------------------------

#ifndef rendersystemH
#define rendersystemH
#include <vcl.h>
#include <Classes.hpp>
#include <list>
#include <map>
#include "fastbitmap.h"
#include "defines.h"

#define RM_NORMAL       0
#define RM_ALPHA_MASK   1
#define RM_OVERLAY      2

#define SPRITE_SIGNATURE 0xB0BAAEEB

#define ZB_LOWEST       0
#define ZB_BACK         1
#define ZB_BOTTOM       16
#define ZB_BEHIND       256
#define ZB_NORMAL       4096
#define ZB_UPPER        32368
#define ZB_TOP          65536
#define ZB_INTERFACE    1048576
#define ZB_ERROR        16777215



class Renderable
{
public:
        virtual void render()=0;
        virtual bool update()=0;
};
class RenderObject : public Renderable
{
public:
        RenderObject();
        ~RenderObject();

        void assign(FastBitmap * fb);
        void assign(PBMP b);
        void assign(String file);
        void setRenderMode(uint Mode, uint clr=0);
        void setContext(FastBitmap * context);
        FastBitmap* getContext(){return context;}
        void setPosition(int left, int top);

        void init(FastBitmap * pic, FastBitmap *context);
        void init(PBMP pic,         FastBitmap *context);
        void init(String filename,  FastBitmap *context);
        void init(FastBitmap * pic, PBMP context);
        void init(PBMP pic,         PBMP context);
        void init(String filename,  PBMP context);

        void applyAlphaMask(FastBitmap * mask);
        void applyAlphaMask(PBMP mask);
        void applyAlphaMask(String filename);

        virtual void render();
        virtual bool update(){return false;}
        void render(FastBitmap * extContext);
        void render(PBMP extContext);
        void render(TCanvas * extContext);
        uint getMode(){return mode;}
        uint getColor(){return color;}
        void complexTo(RenderObject * r, double percent, bool horizontal, bool reverse=false){picture->complexTo(r->picture,percent,horizontal, !reverse);}
        FastBitmap * getPicture(){return picture;}
        void setPicture(FastBitmap * fb) {picture = fb;}
private:

        uint mode, color;
        bool selfMadeContext;
        FastBitmap * picture;
        FastBitmap * context;
        int left, top;

};

struct AnimationDescriptor
{
        AnimationDescriptor()
        {
                name="";
                firstFrame=lastFrame=0;
                animationTime=50;
        }
        AnimationDescriptor(AnimationDescriptor & c)
        {
                name=c.name;
                firstFrame=c.firstFrame;
                lastFrame = c.lastFrame;
                animationTime = c.animationTime;
        }
        AnimationDescriptor(String _name, uint ff, uint lf, uint t)
        {
                name = _name;
                firstFrame = ff;
                lastFrame = lf;
                animationTime = t;
        }
        ~AnimationDescriptor(){;}
        String name;
        uint firstFrame;
        uint lastFrame;
        uint animationTime;
};

class Sprite : public Renderable
{
public:
        friend class SpriteFabric;
        Sprite();
        ~Sprite();

        void loadFromFile(String filename);

        void setRenderMode(uint mode, uint color);
        void setContext(FastBitmap * c);
        FastBitmap * getContext();
        void addAnimation(String name, uint firstFrame, uint lastFrame, uint frameRate);
        void setAnimation(String name);
        void setAnimation(uint id);
        String getCurrentAnimationName();
        int getCurrentAnimationID();
        int getAnimationID(String name);
        bool isLastFrameInAnimation();


        bool update();
        void render();

        bool attach(String filename);
        void flush();

        TRect getRect();
        TDoubleRect getDoubleRect();
        void setPosition(double left, double top);

        bool isSpriteCollision(Sprite * s);
        bool isCursorCollision(int curX, int curY);

        bool isLessThan(Sprite * s) {return z<s->z;}
        uint getAnimationCount() {return animationCount;}
        uint getFramesCount() {return frCount;}

        Pointer2D getPosValues(){Pointer2D p; p.l=&left; p.t=&top; return p;}

        uint z, id;
        bool allowAttachment;
        bool animate;
        AnimationDescriptor * animations;

private:

        void applyEffects(Defines &defs, String effectsList, uint mode, uint color);
        void setFrame(int i);

        RenderObject picture;
        FastBitmap * context;
        FastBitmap frame;
        PBMP allFrames;
        bool ownFrames;

        uint currentAnimation;
        uint animationCount;
        uint currentFrame;
        uint lastTimeUpdated;
        uint frCount;
        int width, height;
        double left, top;
};

class SpriteFabric
{
public:
        static void scan();
        static Sprite * newSprite(String composition, FastBitmap * context);
        static Sprite * copySprite(Sprite * src);
        static Sprite * newInstanceOf(Sprite * src);
        static SpriteFabric * getInstance();
        Sprite * newCachedSprite(String composition, FastBitmap * context);

        void addSpriteToCache(String composition);
        bool spriteInCache(String composition);

private:
        SpriteFabric(){if (!FileExists("sprites\\fbDefines.fabric")) scan();
                       fbD.Load("sprites\\fbDefines.fabric");}
        ~SpriteFabric(){;}
        std::map<String, PBMP> cache;
        Defines fbD;
};

struct SpriteFabricInstanceStruct
{
        friend class SpriteFabric;
        SpriteFabricInstanceStruct(){instance = 0;}
        ~SpriteFabricInstanceStruct(){;}
private:
        SpriteFabric * instance;
}spriteFabricInstanceStruct;



class RenderManager
{
public:
        RenderManager(){id=0; context=0; output=0;}
        ~RenderManager(){;}

        void setContext(FastBitmap * ct, TCanvas* out, uint width, uint height, uint color);
        FastBitmap* getContext(){return context;}
        uint add(Sprite * s);
        uint add(Renderable * r);
        uint adduiObject(Renderable * r);
        void remove(uint id);
        void remove(Sprite *s);
        void remove(Renderable *ro){uiSprites.remove(ro); uiObjects.remove(ro);}
        uint render();
        uint scrollRectRender(TRect sr);

        
        uint getColor(){return c;}
        void clear(bool bsprites, bool bobjects){if (bsprites)sprites.clear(); if (bobjects) {uiSprites.clear(); uiObjects.clear();}}
private:
        uint getId();
        void sort();
        void renderStaticSprites();
        uint id;
        std::list<Sprite*> sprites;
        std::list<Renderable*> uiSprites;
        std::list<Renderable*> uiObjects;

        FastBitmap * context;
        TCanvas * output;
        uint w,h,c;
};



//---------------------------------------------------------------------------
#endif
