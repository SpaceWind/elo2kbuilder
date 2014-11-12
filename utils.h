//---------------------------------------------------------------------------

#ifndef logicH
#define logicH
#include <vcl.h>
#include <list>
#include <math.h>
#include <map>


#define PBMP Graphics::TBitmap *
#define BMP  Graphics::TBitmap

#define KEY_UP          0x0100
#define KEY_DOWN        0x0200

#define KB_STATE_WINAPI_FORMAT          0
#define KB_STATE_BOOLEAN_FORMAT         1


#define NO_COLLISION            0
#define COLLISION_RIGHT         1//rect1 -> rect2
#define COLLISION_UP            2//rect1 \  rect2
#define COLLISION_DOWN          3//rect1 /  rect2
#define COLLISION_LEFT          4//rect2 <- rect1

#define KH_BUFFER_SIZE                          16
#define KH_BUFFER_BOUND                         KH_BUFFER_SIZE-1

#define list_foreach(t,n,e) for(std::list<t>::iterator n = e.begin(); n!=e.end(); ++n)
#define map_foreach(k,v,n,e) for(std::map<k,v>::iterator n = e.begin(); n!=e.end(); ++n)


typedef unsigned int uint;
typedef unsigned char byte;
template <class T> const T& min ( const T& a, const T& b ) {
  return (a>b)?b:a;
}
template <class T> const T& max ( const T& a, const T& b ) {
  return (a>b)?a:b;
}



min (int a, int b){
        return a>b?b:a;
}
max (int a, int b){
        return a>b?a:b;
}

struct Pointer2D
{
        double *l;
        double *t;
};
struct TDoubleRect
{
        double left, top, right, bottom;
        double Width(){return right-left;}
        double Height(){return bottom-top;}
};
struct kbState
{
        kbState(){memset(state,0,256);}
        static kbState get(int format);
        byte state[256];
        byte operator[](int index) {return state[index];}
};
bool valueInRange(int value, int min, int max);
TRect convertRect(TRect r, bool toRightBottomFmt=false);
int rectCollision(TRect A, TRect B, bool dontCalculate=true);
int cRectCollision(TRect A, TRect B, bool dontCalculate=true) {return rectCollision(convertRect(A),convertRect(B),dontCalculate);}
bool rectInRect (TRect A, TRect B);

int rectRange(TRect A, TRect B);


class KeyListener
{
public:
        KeyListener(){;}
        ~KeyListener(){;}
        virtual void keyUpdate(uint spKey)=0;
};

class KeyUpdateProducer
{
public:
        KeyUpdateProducer();
        ~KeyUpdateProducer();
        void update();
        void addListener(KeyListener * l);
        void removeListener(KeyListener * l);
        void clear();
private:
        std::list<KeyListener*> listeners;
        kbState prevResult;

};
struct keyHistory
{
        keyHistory(){memset(this,0,sizeof(keyHistory));}
        ~keyHistory(){;}
        void updateKey(uint spKey);
        int getLastKey(){return keys[max(index==KH_BUFFER_BOUND?KH_BUFFER_BOUND:index-1,0)];}
        bool isLastKeyUp(){return keyUp[max(index==KH_BUFFER_BOUND?KH_BUFFER_BOUND:index-1,0)];}
        bool isLastKeyDown(){return !isLastKeyUp();}

        int getSKey(int i) const {return (keyUp[i]?KEY_UP:KEY_DOWN) + keys[i];}

        bool testCombo(String combo) const;

        int keys[KH_BUFFER_SIZE];
        uint keyTime[KH_BUFFER_SIZE];
        bool keyUp[KH_BUFFER_SIZE];
        int index;
};


struct PhysForce
{
        PhysForce();
        ~PhysForce();

        static PhysForce create(double f, double p, double i, double fVecLeft, double fVecTop, double pVecLeft, double pVecTop);
        void init(double f, double p, double i, double fVecLeft, double fVecTop, double pVecLeft, double pVecTop);


        double getVl();
        double getVt();

        void stop();
        void start();


        double power, impulse, inertia;
        double fVecLeft, fVecTop, pVecLeft, pVecTop, iVecLeft, iVecTop;
        bool active;
        bool done;
        uint initTime;
        uint stopTime;
};
PhysForce operator+ (const PhysForce& pf1, const PhysForce& pf2);

class PhysObject
{
public:
        PhysObject(){;}
        ~PhysObject(){;}

        void addForce(String name, double f, double p, double i, double fVecLeft, double fVecTop, double pVecLeft, double pVecTop);
        void stopForce(String name);
        void startForce(String name);
        void invert(String name, bool horForce, bool vertForce);
        void removeForce(String name);

        double getVl();
        double getVt();




private:
        std::map<String, PhysForce> forces;
};




//---------------------------------------------------------------------------
#endif
