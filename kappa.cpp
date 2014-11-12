//---------------------------------------------------------------------------


#pragma hdrstop

#include "kappa.h"


Kappa::Kappa(GameManager * gm, FastBitmap * c)
{
        manager = gm;
        context = c;
        Defines config;
        config.Load("config\\kappa.txt");

        SpriteFabric * sf = SpriteFabric::getInstance();

        kappaNormal = sprite = sf->SFM_CURRENT(config.Get("sprite"), context);
        kappaRotate = sf->SFM_CURRENT(config.Get("spriterotate"), context);
        maxhp=StrToInt(config.Get("hp"));
        hp = StrToInt(config.Get("hp"));
        speed=StrToInt(config.Get("speed"));
        jump =StrToInt(config.Get("jump"));
        speedDecreaseLand = StrToInt(config.Get("sdl"));
        speedDecreaseAir = StrToInt(config.Get("sda"));
        tAcceleration = StrToInt(config.Get("acceleration"));
        damage = StrToInt(config.Get("damage"));
        memset(arrowsPressed,0,4);
        lastUpdate = 0;

        state = STATE_KAPPA_INAIR | STATE_KAPPA_NORMAL | STATE_KAPPA_MOVING_PASSIVE;
        currentSpeedT = 1;
        currentSpeedL = 0;
        moveRequested = false;
        team = TEAM_PLAYER;

        hpBar = new ProgressBar("config\\text_hpbar.txt",c);
        hpBar->setPosition(0, 525);
        eloBar = new ProgressBar("config\\elobar.txt",c);
        eloBar->setPosition(0, 560);
        eloBar->setProgress(300);
        gm->getRenderManager()->adduiObject(hpBar);
        gm->getRenderManager()->adduiObject(eloBar);

        spellManager = new SpellManager(&keys, manager);
        KappaSpellDash * kappaDash = new KappaSpellDash();
        kappaDash->init(this,305, 535);
        spellManager->addSpell("dash",kappaDash);
}

Kappa::Kappa(GameManager * gm, FastBitmap * c, String spriteStr, String spriteRotateStr,
              int maxHP, int HP, int spd, int jumpP, int sdl, int sda, int acc, int dmg, int elo)
{
        manager = gm;
        context = c;
        SpriteFabric * sf = SpriteFabric::getInstance();

        kappaNormal = sprite = sf->SFM_CURRENT(spriteStr, context);
        kappaRotate = sf->SFM_CURRENT(spriteRotateStr, context);
        maxhp=maxHP;
        hp = HP;
        speed=spd;
        jump = jumpP;
        speedDecreaseLand = sdl;
        speedDecreaseAir = sda;
        tAcceleration = acc;
        damage = dmg;

        memset(arrowsPressed,0,4);
        lastUpdate = 0;

        state = STATE_KAPPA_INAIR | STATE_KAPPA_NORMAL | STATE_KAPPA_MOVING_PASSIVE;
        currentSpeedT = 1;
        currentSpeedL = 0;
        moveRequested = false;
        team = TEAM_PLAYER;

        hpBar = new ProgressBar("config\\text_hpbar.txt",c);
        hpBar->setPosition(0, 525);
        eloBar = new ProgressBar("config\\elobar.txt",c);
        eloBar->setPosition(0, 560);
        eloBar->setProgress(elo);
        gm->getRenderManager()->adduiObject(hpBar);
        gm->getRenderManager()->adduiObject(eloBar);

        spellManager = new SpellManager(&keys, manager);
        KappaSpellDash * kappaDash = new KappaSpellDash();
        kappaDash->init(this,305, 535);
        spellManager->addSpell("dash",kappaDash);


}
Kappa::~Kappa()
{
        manager->getRenderManager()->remove(hpBar);
        manager->getRenderManager()->remove(eloBar);
        delete hpBar;
        delete eloBar;
        delete kappaNormal;
        delete kappaRotate;
}
void Kappa::processMessage(IMessage *msg)
{
        double temp[8];
        switch (msg->type)
        {
        case MSG_MOVE_REQUEST:
                if ((state & 0xF000) == STATE_KAPPA_NORMAL)
                {
                        kappaNormal->setPosition(msg->left, msg->top);
                        kappaRotate->setPosition(msg->left, msg->top+60);
                }
                else
                {
                        kappaNormal->setPosition(msg->left, msg->top-60);
                        kappaRotate->setPosition(msg->left, msg->top);
                }
                if (((state & 0xF00) == STATE_KAPPA_ONGROUND) && sprite->getRect().bottom<500)
                        state = (state & 0xFF0FF) | STATE_KAPPA_INAIR;
                break;
        case MSG_COLLISION:
                if ((msg->super->type & 0xFF00) == TYPE_WALL)
                {
                        if (moveRequested)
                        {
                                TRect wRect = msg->super->sprite->getRect();
                                TRect sRect = sprite->getRect();
                                int col = rectCollision(convertRect(sRect),convertRect(wRect), false);
                                if (msg->super->type == TYPE_BOUND_WALL)
                                        if (msg->top<=0)
                                                col = COLLISION_UP;
                                        else
                                                col = COLLISION_DOWN;
                                switch (col)
                                {
                                case COLLISION_RIGHT:
                                        temp[0]=msg->super->sprite->getDoubleRect().left-kappaNormal->getDoubleRect().Width()-1.;
                                        temp[1]=msg->super->sprite->getDoubleRect().left-kappaRotate->getDoubleRect().Width()-1.;
                                        temp[2]=msg->top+kappaNormal->getDoubleRect().Height();
                                        temp[3]=msg->super->sprite->getDoubleRect().top;
                                        temp[4]=msg->top+kappaRotate->getDoubleRect().Height();
                                        if (((state & 0xF000) == STATE_KAPPA_NORMAL))
                                        {
                                                if (temp[2]-temp[3]< 10)
                                                {
                                                        kappaNormal->setPosition(msg->left, temp[3]-temp[2]+msg->top-1);
                                                        kappaRotate->setPosition(msg->left, temp[3]-temp[2]+msg->top+60-1);
                                                }
                                                else
                                                {
                                                        kappaNormal->setPosition(temp[0], msg->top);
                                                        kappaRotate->setPosition(temp[1], msg->top+60);
                                                        currentSpeedL = SPEED_MINIMAL;
                                                }
                                        }
                                        else
                                        {
                                                if (temp[4]-temp[3]< 10)
                                                {
                                                        kappaNormal->setPosition(msg->left, temp[3]-temp[4]+msg->top-1-60);
                                                        kappaRotate->setPosition(msg->left, temp[3]-temp[4]+msg->top-1);
                                                }
                                                else
                                                {
                                                        kappaNormal->setPosition(temp[0], msg->top-60);
                                                        kappaRotate->setPosition(temp[1], msg->top);
                                                        currentSpeedL = SPEED_MINIMAL;
                                                }
                                        }

                                        break;

                                case COLLISION_LEFT:
                                        temp[0]=msg->super->sprite->getDoubleRect().right+1.;
                                        temp[1]=msg->top+kappaNormal->getDoubleRect().Height();
                                        temp[2]=msg->top+kappaRotate->getDoubleRect().Height();
                                        temp[3]=msg->super->sprite->getDoubleRect().top;
                                        if (((state & 0xF000) == STATE_KAPPA_NORMAL))
                                        {
                                                if (temp[1]-temp[3]<10)
                                                {
                                                        kappaNormal->setPosition(msg->left, temp[3]-temp[1]+msg->top-1);
                                                        kappaRotate->setPosition(msg->left, temp[3]-temp[1]+msg->top+60-1);
                                                }
                                                else
                                                {
                                                        kappaNormal->setPosition(temp[0], msg->top);
                                                        kappaRotate->setPosition(temp[0], msg->top+60);
                                                        currentSpeedL = SPEED_MINIMAL;
                                                }
                                        }
                                        else
                                        {
                                                if (temp[2]-temp[3]< 10)
                                                {
                                                        kappaNormal->setPosition(msg->left, temp[3]-temp[2]+msg->top-1-60);
                                                        kappaRotate->setPosition(msg->left, temp[3]-temp[2]+msg->top-1);
                                                }
                                                else
                                                {
                                                        kappaNormal->setPosition(temp[0], msg->top-60);
                                                        kappaRotate->setPosition(temp[0], msg->top);
                                                        currentSpeedL = SPEED_MINIMAL;
                                                }
                                        }

                                        break;
                                case COLLISION_UP:
                                        if (currentSpeedT<0)
                                                currentSpeedT *=-0.8;
                                        if (msg->super->type == TYPE_BOUND_WALL)
                                        {
                                                kappaNormal->setPosition(msg->left, kappaNormal->getDoubleRect().top);
                                                kappaRotate->setPosition(msg->left, kappaRotate->getDoubleRect().top);
                                        }
                                        else
                                        {
                                                kappaNormal->setPosition(msg->left, msg->super->sprite->getDoubleRect().bottom+1);
                                                kappaRotate->setPosition(msg->left, msg->super->sprite->getDoubleRect().bottom+1);
                                        }
                                        break;
                                case COLLISION_DOWN:
                                        if (msg->super->type == TYPE_BOUND_WALL)
                                        {
                                                kappaNormal->setPosition(msg->left, kappaNormal->getDoubleRect().top);
                                                kappaRotate->setPosition(msg->left, kappaRotate->getDoubleRect().top);
                                        }
                                        else
                                        {
                                                kappaNormal->setPosition(msg->left, msg->super->sprite->getDoubleRect().top
                                                                                    - kappaNormal->getDoubleRect().Height());
                                                kappaRotate->setPosition(msg->left, msg->super->sprite->getDoubleRect().top
                                                                                    - kappaRotate->getDoubleRect().Height());
                                        }

                                        state = (state & 0xF0FF) | STATE_KAPPA_ONGROUND;
                                        currentSpeedT = 0;
                                        break;

                                default:
                                        if ((state & 0xF000) == STATE_KAPPA_NORMAL)
                                        {
                                                kappaNormal->setPosition(msg->left, msg->top);
                                                kappaRotate->setPosition(msg->left, msg->top+60);
                                        }
                                        else
                                        {
                                                kappaNormal->setPosition(msg->left, msg->top-60);
                                                kappaRotate->setPosition(msg->left, msg->top);
                                        }
                                        break;
                                }
                        }
                }
                break;
        default:
                break;
        }
        delete msg;
}
//---------------------------------------------------------------------------
void Kappa::update (uint t)
{
        if (lastUpdate == 0)
        {
                lastUpdate = t;
                return;
        }
        double timeL = double(t-lastUpdate)/1000.;

        spellManager->update();
        hpBar->setMax(maxhp);
        hpBar->setProgress(hp);

        if (((state &0x00F00) == STATE_KAPPA_INAIR) && sprite->getRect().bottom > 530)
        {
                state = (state &0xFF0FF)|STATE_KAPPA_ONGROUND;
                sprite->setPosition(sprite->getDoubleRect().left,530.-sprite->getDoubleRect().Height());
        }

        if (currentSpeedL>0 && ((state&0x00F0) == STATE_KAPPA_MOVING_PASSIVE))
        {
                if ((state & 0x0F00) == STATE_KAPPA_ONGROUND)
                        currentSpeedL-= timeL*speedDecreaseLand;
                else
                        currentSpeedL-= timeL*speedDecreaseAir;
                if (currentSpeedL<0)
                {
                        currentSpeedL = 0;
                        state = (state & 0xFFF0);
                }
        }
        else if (currentSpeedL<0 && ((state&0x00F0) == STATE_KAPPA_MOVING_PASSIVE))
        {
                if ((state & 0x0F00) == STATE_KAPPA_ONGROUND)
                        currentSpeedL+= timeL*speedDecreaseLand;
                else
                        currentSpeedL+= timeL*speedDecreaseAir;
                if (currentSpeedL>0)
                {
                        currentSpeedL = 0;
                        state = (state & 0xFFF0);
                }
        }
        if (((state&0x0F00) == STATE_KAPPA_INAIR) && ((state & 0x00F0) == STATE_KAPPA_MOVING_ACTIVE)
           && (currentSpeedL>0) && ((state&0xF000) == STATE_KAPPA_NORMAL))
        {
                currentSpeedL-= timeL*speedDecreaseAir*3.;
                if (currentSpeedL<0)
                        currentSpeedL = 0;
        }
        if (((state&0x0F00) == STATE_KAPPA_INAIR) && ((state & 0x00F0) == STATE_KAPPA_MOVING_ACTIVE)
           && (currentSpeedL<0) && ((state&0xF000) == STATE_KAPPA_NORMAL))
        {
                currentSpeedL+= timeL*speedDecreaseAir*3.;
                if (currentSpeedL>0)
                        currentSpeedL = 0;
        }

        double nextL=sprite->getDoubleRect().left, nextT=sprite->getDoubleRect().top;
        if ((state & 0x000F) != STATE_KAPPA_STAY)
                nextL+= timeL*currentSpeedL*
                (((state & 0xF000) == STATE_KAPPA_ROTATE)&&((state & 0x0F00) != STATE_KAPPA_INAIR)?0.5:1.);
        if ((state & 0x0F00) == STATE_KAPPA_INAIR)
        {
                nextT+= timeL*currentSpeedT;
                currentSpeedT+=timeL*tAcceleration* (((state & 0xF000) == STATE_KAPPA_ROTATE)?0.8:1.);
        }


        if ((state& 0x000F) == STATE_KAPPA_STAY)
                sprite->animate = false;
        else
                sprite->animate = true;
        if (((state & 0x000F) == STATE_KAPPA_MOVING_RIGHT) && (sprite->getCurrentAnimationName() != "go_right"))
                sprite->setAnimation("go_right");
        else if (((state & 0x000F) == STATE_KAPPA_MOVING_LEFT)&& (sprite->getCurrentAnimationName() != "go_left"))
                sprite->setAnimation("go_left");
        sprite->update();

        moveRequested = true;
        manager->sendMessage(IMessage::createMoveRequestMessage(this,nextL,nextT));
        lastUpdate = ::GetTickCount();
}

void Kappa::keyUpdate(uint spKey)
{
        keys.updateKey(spKey);
        updateArrowPressed(spKey);

        spellManager->check();

        switch(keys.getLastKey())
        {
        case VK_LEFT:
                if (keys.isLastKeyDown())
                {
                        state = (state&0xFF00)|STATE_KAPPA_MOVING_ACTIVE | STATE_KAPPA_MOVING_LEFT;
                        currentSpeedL = -speed;
                }
                else
                {
                        state = (state&0xFF0F)|STATE_KAPPA_MOVING_PASSIVE;
                        if (someArrowPressed())
                        {
                                state = findNextArrowState(state);
                                currentSpeedL = speed;
                        }
                }
                break;
        case VK_RIGHT:
                if (keys.isLastKeyDown())
                {
                        state = (state&0xFF00)|STATE_KAPPA_MOVING_ACTIVE | STATE_KAPPA_MOVING_RIGHT;
                        currentSpeedL = speed;
                }
                else
                {
                        state = (state&0xFF0F)|STATE_KAPPA_MOVING_PASSIVE;
                        if (someArrowPressed())
                        {
                                state = findNextArrowState(state);
                                currentSpeedL = -speed;
                        }
                }
                break;
        case VK_UP:
                if (keys.isLastKeyDown() && ((state&0x0F00)!=STATE_KAPPA_INAIR))
                {
                        state = (state&0xF0FF)|STATE_KAPPA_INAIR;
                        currentSpeedT = -jump;
                }
                break;
        case VK_DOWN:
                if (keys.isLastKeyDown())
                {
                        state = (state&0x0FFF)|STATE_KAPPA_ROTATE;
                        setKappaRotate();
                }
                else
                {
                        state = (state&0x0FFF)|STATE_KAPPA_NORMAL;
                        setKappaNormal();
                }
                break;

        }
}
void Kappa::setPosition(int left, int top)
{
        kappaNormal->setPosition(left,top);
        kappaRotate->setPosition(left,top+60);
}
int Kappa::findNextArrowState(int currentState)
{
        if (arrowsPressed[0])
                return (currentState & 0xFF00) | STATE_KAPPA_MOVING_ACTIVE | STATE_KAPPA_MOVING_LEFT;
        if (arrowsPressed[1])
                return (currentState & 0xFF00) | STATE_KAPPA_MOVING_ACTIVE | STATE_KAPPA_MOVING_RIGHT;
        return currentState;
}
void Kappa::updateArrowPressed(int spKey)
{
        bool isUp = (spKey&0xFF00) == KEY_UP;
        int key = spKey&0xFF;
        if (key == VK_LEFT)
                arrowsPressed[0] = !isUp;
        else if (key == VK_RIGHT)
                arrowsPressed[1] = !isUp;

}
void Kappa::setKappaNormal()
{
        sprite = kappaNormal;
        manager->getRenderManager()->remove(kappaRotate);
        manager->getRenderManager()->add(kappaNormal);
}
void Kappa::setKappaRotate()
{
        sprite = kappaRotate;
        manager->getRenderManager()->remove(kappaNormal);
        manager->getRenderManager()->add(kappaRotate);
}
void Kappa::render()
{
        sprite->render();
        spellManager->render();
} 

//---------------------------------------------------------------------------

#pragma package(smart_init)
