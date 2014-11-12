//---------------------------------------------------------------------------


#pragma hdrstop

#include "walls.h"

Wall::Wall(GameManager *gm, FastBitmap * fb, String spriteComp, String config)
{
        this->manager = gm;
        this->context = context;
        this->team = TEAM_PEACEFUL;
        this->type = TYPE_SIMPLE_BOX;

        SpriteFabric * sf = SpriteFabric::getInstance();
        sprite = sf->SFM_CURRENT(spriteComp,fb);

}


//---------------------------------------------------------------------------

#pragma package(smart_init)
