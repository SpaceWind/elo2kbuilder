//---------------------------------------------------------------------------

#ifndef mapH
#define mapH
#include "scenes.h"
#include "gamesystem.h"
#include "kappa.h"
#include "walls.h"
#include "objectDefines.h"
#include "soundSystem.h"
#include "stats.h"

#define MAP_SIZE_WIDTH          900
#define MAP_SIZE_HEIGHT         525

#define COMPARE_METHOD_LESS     -1
#define COMPARE_METHOD_MORE     1
#define COMPARE_METHOD_EQUAL    0



GameObject * bindObject(String name, GameManager *mgr, FastBitmap * context);


struct SubMapDesc
{
        SubMapDesc(){active=false; operations=0;currentOperation=0;}
        SubMapDesc(String _name, TStringList * _operations, bool active=false);
        ~SubMapDesc(){if (operations) delete operations;}

        String nextOperation();
        String toString();
        void setOperations(TStringList * operations);
        String name;
        bool active;
        TStringList * operations;
        int currentOperation;
};
struct MapAnimate
{
        MapAnimate();
        MapAnimate(GameObject* obj, double destLeft, double destTop, double _speed, bool update){init(obj,destLeft,destTop,_speed, up);}
        ~MapAnimate(){;}

        void init(GameObject *object, double destLeft, double destTop, double _speed, bool up);
        bool update();
        void reverse();

        String onStart, onEnd, event;

        uint lastUpdate;

        double dLeft, dTop;
        double sLeft, sTop;
        double speed;
        GameObject * object;
        bool up;
        bool done;
        bool started;
        double dL, dT;
};

class MapEvent
{
public:
        friend class Map;
        MapEvent(){active=false; event = "";}
        MapEvent(bool _active, String _event){active = _active; event = _event;}
        virtual ~MapEvent(){;}
        virtual bool update()=0;
        virtual String getEvent(){return event;}
        virtual bool isActive(){return active;}
        String getType(){return type;}
protected:
        String event;
        bool active;
        String type;
};

struct PlayerRectEvent : public MapEvent
{
        PlayerRectEvent():MapEvent(){type = "playerRect";}
        PlayerRectEvent(GameObject * _object, TRect r, String event):MapEvent(true,event){object = _object; rect = r;type = "playerRect";}
        ~PlayerRectEvent(){;}

        bool update() {if (!active) return false; if (cRectCollision(object->sprite->getRect(), rect)) {active=false; return true;} return false;}
        bool consistOf(GameObject * obj) {return obj == object;}

private:
        TRect rect;
        GameObject * object;
};

struct MapScrollEvent : public MapEvent
{
        MapScrollEvent():MapEvent(){type = "mapScroll";}
        MapScrollEvent(int * _scrollValueLeft, int * _scrollValueTop,
                       int _endScrollValueLeft, int _endScrollValueTop,
                       int _compareMethod, String event);
        ~MapScrollEvent(){;}

        bool update();
private:
        int * currentScrollValueLeft;
        int * currentScrollValueTop;
        int endScrollValueLeft, endScrollValueTop;
        int compareMethod;

};


class Map
{
public:
        Map(){;}
        Map(String filename, GameManager * gm);
        ~Map();

        void update();
        bool isDone(){return done;}

        void load(String filename);
        void callSubMap(String name);
        void setScrollingBounds(int _scrollLeftBound, int _scrollTopBound){scrollLeftBound = _scrollLeftBound; scrollTopBound = _scrollTopBound;}
        void holdScrolling(bool left, bool top){holdScrollingLeft=left; holdScrollingTop=top;}
        void boundScrollingToObject(GameObject * object){scrollBoundObject = object;}
        bool isScrollingAllowed();
        void render();
private:
        bool processString(String s);
        SubMapDesc* findSubMap(String s);
        GameObject * buildObject(TStringList* params);
        void flush();

        Scene * scene;

        std::list<SubMapDesc*> subScenes;
        std::map <String, MapAnimate*> animations;
        std::map <String, GameObject*> objects;
        std::list<MapEvent*> events;
        GameManager * manager;
        GameObject * scrollBoundObject;


        bool started;
        bool done;

        int scrollLeftValue, scrollLeftBound;
        int scrollTopValue,  scrollTopBound;

        int scrollLeftNegativeBound;
        int scrollTopNegativeBound;

        int scrollSmoothVertical;
        int scrollSmoothHorizontal;

        int maxScrollLeft, minScrollLeft, maxScrollTop, minScrollTop;

        bool holdScrollingLeft, holdScrollingTop;
        bool scrollBoundsTop, scrollBoundsLeft;
        int w, h;



};
//---------------------------------------------------------------------------
#endif
