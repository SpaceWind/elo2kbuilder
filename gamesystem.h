//---------------------------------------------------------------------------

#ifndef gamesystemH
#define gamesystemH
#include <queue>
#include "utils.h"
#include "rendersystem.h"
#include "stats.h"
#include "objectDefines.h"


#define MSG_MOVE_REQUEST        0x1
#define MSG_STATS_INCREASE      0x2
#define MSG_SHOW_MESSAGE        0x3
#define MSG_DESTROY_ME          0x4
#define MSG_GAMEOVER            0x5
#define MSG_BROADCAST           0x6
#define MSG_ADD                 0x7
#define MSG_ADD_ME              0x8
#define MSG_HIDE_ME             0x9
#define MSG_KILL_ME             0xA

#define MSG_COLLISION           0x101
#define MSG_DIE                 0x102
#define MSG_FREE                0x0


#define MSG_ADD_ME_ALL          MSG_ADD_ME | MSG_ML | MSG_RL | MSG_CL
#define MSG_HIDE_ME_ALL         MSG_HIDE_ME| MSG_ML | MSG_RL | MSG_CL

#define MSG_ML                  0x200
#define MSG_RL                  0x400
#define MSG_CL                  0x800
#define IS_MSG_ADD_ME(message)  ((message & 0xFF)==MSG_ADD_ME)
#define IS_MSG_HIDE_ME(message) ((message & 0xFF)==MSG_HIDE_ME)



#define TEAM_PLAYER             0x1
#define TEAM_AI                 0x2
#define TEAM_HOSTILE            0x3     //Enemy for everybody. Everybody are enemies for him.
#define TEAM_TEAMLESS           0x4     //Enemy for AI. Everybody are neutral for him.
#define TEAM_NEUTRAL            0x5     //Not enemy for all. Theres enemy for him, till they attack.
#define TEAM_PEACEFUL           0x6     //Not enemy for all. Theres no enemy for him in any case

#define TEAM_CUSTOM             0x100   //Custom team.


#define BIT_CHECK_AND           0x1
#define BIT_CHECK_EQ            0x2


#define GM_OP_PROCESS_COLLISION         1
#define GM_OP_UPDATE                    2
#define GM_OP_PROCESS_MESSAGES          4
#define GM_OP_RENDER                    8
//
#define GM_OP_NOTHING                   0
#define GM_OP_ALL                       GM_OP_PROCESS_COLLISION | GM_OP_UPDATE | GM_OP_PROCESS_MESSAGES | GM_OP_RENDER
#define GM_OP_DONT_RENDER               GM_OP_PROCESS_COLLISION | GM_OP_UPDATE | GM_OP_PROCESS_MESSAGES


class GameObject;
struct TypePoint
{
        TypePoint () {left=top=type=0;}
        TypePoint (uint l, uint t, uint tp){left=l; top=t; type=tp;}
        ~TypePoint(){;}
        bool empty(){return left?0:top?0:type?0:1;}
        uint left, top, type;
};

struct AstarPoint
{
        AstarPoint () {g=0xFFFFFF; h=0; f=0xFFFFFF; parent=0;}
        ~AstarPoint() {;}
        bool isDefault() {return ((g==0xFFFFFF) && (h==0) && (f==0xFFFFFF+1) && (parent==0))?true:false;}
        TypePoint p;
        double g, h, f;
        AstarPoint * parent;

        std::list<TypePoint> reconstructPath();
};
bool operator==(const TypePoint& p1, const TypePoint& p2);
bool operator==(const AstarPoint& p1, const AstarPoint& p2);
class GameObject;
class GameManager;
struct IMessage;

struct IMessage
{
        static IMessage* createFreeMessage();
        static IMessage* createMoveRequestMessage(GameObject * s, double l, double t);
        static IMessage* createStatsIncreaseMessage(uint value);
        static IMessage* createShowMessageMessage(String message);
        static IMessage* createDestroyMeMessage(GameObject *s);
        static IMessage* createGameoverMessage();
        static IMessage* createBroadcastMessage(IMessage* message);
        static IMessage* createAddMessage(GameObject * sp);
        static IMessage* createAddMeMessage(GameObject * s, bool messageList=true, bool renderList=true, bool collisionList=true);
        static IMessage* createHideMeMessage(GameObject *s, bool messageList=true, bool renderList=true, bool collisionList=true);
        static IMessage* createCollisionMessage(GameObject * sp, int l, int t);
        static IMessage* createDieMessage();

        uint type;

        GameObject * sender;
        GameObject * super;

        IMessage * broadcast;

        double left, top;

        uint value;
        double dValue;

        String msg;
};


struct mapEntry
{
        mapEntry(){memset(this,0,sizeof(mapEntry));}
        ~mapEntry(){;}
        bool empty(){for (int i=0; i<16; i++)if (!objects[i].empty()) return false; return true;}
        bool containsObjectType(uint type){for (int i=0; i<16; i++) if (objects[16].type & type) return true; return false;}
        bool containsObject(uint type){for (int i=0; i<16; i++) if (objects[16].type == type) return true; return false;}
        void add(TypePoint p) {if (count<16) {objects[count]=p; count++;}}
        void clear(){memset(this,0,sizeof(mapEntry));}
        int getValue(){int v=0; for (int i=0; i<16; i++) v+=objects[i].type; return v;}
        TypePoint objects[16];
        int count;
};

struct GameFieldDesc
{
        GameFieldDesc();
        GameFieldDesc(uint width, uint height, uint tileSize=0);
        virtual ~GameFieldDesc();

        void init (uint width, uint height, uint tileSize);
        void addObject(int left, int top, uint id);
        void clear();
        std::list<TypePoint> findObjects(const TRect& area, uint id, bool wholeMap=false, int bitCheck=0);
        std::list<TypePoint> findWay(const TRect& started, uint destLeft, uint destTop);


        uint tileSize;

        mapEntry ** map;
        int width, height;
        static const defaultTileSize=32;
private:
        double h(const TypePoint& current, const TypePoint& finish);
};

class GameManager
{
public:
        GameManager();
        ~GameManager();

        void setRenderManager(RenderManager * rm);
        RenderManager* getRenderManager(){return renderManager;}
        void sendMessage(IMessage * m);
        void generateMap(GameFieldDesc * gfd, uint w, uint h);

        std::list<GameObject*> objectsInRect(TRect r, int type=0);
        std::list<GameObject*> objectsInRange(TRect r, int range, int type=0);

        void testCollisions();
        void update();
        void processMessages();
        void render();
        void clear();
        void removeObject(GameObject * obj);
        void launchGameCycle(int mask=GM_OP_ALL);
private:
        std::queue<IMessage*>   messageQueue;
        std::list<GameObject*>  listenersList;
        std::list<GameObject*>  collisionList;
        RenderManager * renderManager;
        KeyUpdateProducer keyUpdater;
};


class GameObject: public KeyListener
{
public:
        GameObject(){ph = new PhysObject();}
        virtual ~GameObject(){delete ph;}
        Sprite * sprite;
        GameManager * manager;
        FastBitmap * context;
        uint type, team;
        uint state;

        virtual void processMessage(IMessage *msg)=0;
        virtual void update (uint t)=0;
        virtual void render ()=0;
        virtual IMessage * getSelfAddMessage(){return IMessage::createAddMeMessage(this);}
protected:
        PhysObject * ph;
};

class SilentObject : public GameObject
{
public:
        SilentObject(){;}
        ~SilentObject(){;}

        virtual void processMessage(IMessage *msg){;}
        virtual void update (uint t){;}
        virtual void render () {;}
        virtual void keyUpdate(uint t) {;}
        virtual IMessage * getSelfAddMessage(){return IMessage::createAddMeMessage(this,false,false,false);}
};

//---------------------------------------------------------------------------
#endif
