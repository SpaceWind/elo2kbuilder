//---------------------------------------------------------------------------

#ifndef scenesH
#define scenesH
#include <vcl.h>
#include <mmsystem.h>
#include "gamesystem.h"
#include "progressbars.h"
#include "stats.h"

struct SceneTimer
{
        SceneTimer();
        SceneTimer(int delay, String operations){init(delay,operations);}
        SceneTimer(int delay){init(delay,0);}
        ~SceneTimer(){;}

        bool update(){if(::GetTickCount()>lastTime) done = true; return done;}
        bool isDone () {return done;}
        void init(int delay, String operation) {lastTime=::GetTickCount()+delay; ops=operation; done=false;}

        uint lastTime;
        String ops;
        bool done;
};

struct AnimateValue
{
        AnimateValue(){start=current=dest=speed=0; done = false; lastUpdate=0;}
        ~AnimateValue(){;}

        bool update();
        double start, current, dest, speed;
        bool done;
        int lastUpdate;
};

struct BindedProgressBar
{
        BindedProgressBar(){pb=0;}
        ~BindedProgressBar(){if (pb) delete pb;}
        ProgressBar * pb;
        AnimateValue * av;
        void update(){pb->setProgress(av->current);}
};

struct SceneAnimate
{
        SceneAnimate();
        SceneAnimate(Sprite * s, double destLeft, double destTop, double _speed, bool isLinear=true){init(s,destLeft,destTop,_speed,isLinear);}
        ~SceneAnimate(){;}

        void init(Sprite *s, double destLeft, double destTop, double _speed, bool isLinear=true);
        bool update();
        void reverse();

        String onStart, onEnd, event;

        uint lastUpdate;

        double dLeft, dTop;
        double sLeft, sTop;
        double speed;
        Sprite * sprite;
        bool linear;
        bool done;
        bool started;
        double dL, dT;
};

struct SubSceneDesc
{
        SubSceneDesc(){active=false; operations=0;currentOperation=0;}
        SubSceneDesc(String _name, TStringList * _operations, bool active=false);
        ~SubSceneDesc(){if (operations) delete operations;}

        String nextOperation();
        String toString();
        String name;
        bool active;
        TStringList * operations;
        int currentOperation;
};

class Scene
{
public:
        friend class Map;
        Scene(){rm=0; started=false; done=false;}
        Scene(String filename, RenderManager * rm);
        ~Scene();

        void update();
        bool isDone(){return done;}
        void load(String filename);
        void callSubScene(String name);
private:
        bool processString(String s);
        SubSceneDesc* findSubScene(String s);

        std::list<SubSceneDesc*> subScenes;
        std::list<SceneTimer*>   timers;
        std::map <String, SceneAnimate*> animations;
        std::map <String, Sprite*> sprites;
        std::map <String, AnimateValue> values;
        std::map <String, BindedProgressBar*> progressBars;

        RenderManager * rm;

        bool started;
        bool done;



};
//---------------------------------------------------------------------------
#endif
