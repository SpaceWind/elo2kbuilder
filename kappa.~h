//---------------------------------------------------------------------------

#ifndef kappaH
#define kappaH
#include "gamesystem.h"
#include "spells.h"
#include "objectDefines.h"

#define STATE_KAPPA_STAY                        0x00000
#define STATE_KAPPA_MOVING_LEFT                 0x00001
#define STATE_KAPPA_MOVING_RIGHT                0x00002
#define STATE_KAPPA_MOVING_PASSIVE              0x00010
#define STATE_KAPPA_MOVING_ACTIVE               0x00020
#define STATE_KAPPA_ONGROUND                    0x00100
#define STATE_KAPPA_INAIR                       0x00200
#define STATE_KAPPA_NORMAL                      0x01000
#define STATE_KAPPA_ROTATE                      0x02000
#define STATE_KAPPA_MODE_DASH                   0x10000


#define COMBO_DASH_RIGHT                       "1000- . > !> > !> !."
#define COMBO_DASH_LEFT                        "1000- . < !< < !< !."
//#define COMBO_DASH_LEFT                         "200- A !A"


#define SPEED_MINIMAL                           0.1


class SpellManager;
class Kappa : public GameObject
{
        friend class KappaSpellDash;
public:
        Kappa(){;}
        Kappa(GameManager * gm, FastBitmap * context);
        Kappa(GameManager * gm, FastBitmap * context,
              String sprite, String spriteRotate,
              int maxHP, int HP, int speed, int jumpP, int sdl, int sda, int acc, int dmg, int elo);
        ~Kappa();

        virtual void keyUpdate(uint spKey);
        virtual void processMessage(IMessage *msg);
        virtual void update (uint t);
        virtual void render();
        void setPosition(int left, int top);

protected:
        bool someArrowPressed() {return (arrowsPressed[0]||arrowsPressed[1]);}
        void updateArrowPressed(int spKey);
        int  findNextArrowState(int currentState);

        void setKappaNormal();
        void setKappaRotate();

        double speed;
        double jump;
        double speedDecreaseLand, speedDecreaseAir;
        double currentSpeedL, currentSpeedT;
        double tAcceleration;

        SpellManager * spellManager;
        ProgressBar * hpBar;
        ProgressBar * eloBar;

        int maxhp,hp;
        int damage;

        keyHistory keys;
        bool arrowsPressed[2];

        int lastUpdate;
        bool moveRequested;
        Sprite * kappaNormal;
        Sprite * kappaRotate;

};
//---------------------------------------------------------------------------
#endif
