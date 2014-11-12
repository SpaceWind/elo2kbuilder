//---------------------------------------------------------------------------

#ifndef wallsH
#define wallsH

#include "gamesystem.h"
#include "objectDefines.h"
class Wall : public GameObject
{
public:
        Wall(GameManager *gm, FastBitmap * fb, String spriteComp, String config);
        virtual void processMessage(IMessage *msg){delete msg;}
        virtual void update (uint t){sprite->update();}
        virtual void render () {sprite->render();}
        virtual void keyUpdate(uint){;}

        ~Wall(){delete sprite;}

};
//---------------------------------------------------------------------------
#endif
