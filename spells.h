//---------------------------------------------------------------------------

#ifndef spellsH
#define spellsH
#include "gamesystem.h"
#include "progressbars.h"
#include "kappa.h"
class Spell:public Renderable
{
public:
        Spell(){gm=0; pb=0;}
        virtual ~Spell(){delete pb;}

        virtual void init(GameManager * gm, int left, int top)=0;
        virtual void activate()=0;
        virtual void deactivate()=0;
        virtual bool isReady()=0;
        virtual bool testCombo(const keyHistory& keys)=0;
        virtual void render()=0;
        virtual void setCombo(String cmb){combo=cmb;}
        virtual String getHint(){return hint;}
protected:
        SpellProgressBar * pb;
        GameManager * gm;
        String combo, hint;
};

class SpellManager
{
public:
        SpellManager(){keys=0;}
        ~SpellManager(){free();}

        SpellManager(keyHistory* keys, GameManager * gm);
        void setKeys(keyHistory* keys);
        void addSpell(String name, Spell * sp);
        void removeSpell(String name);
        void check();
        void update();
        void render();
        void free();
private:
        std::map<String, Spell*> spells;
        keyHistory * keys;
        GameManager* manager;
        Renderable * table;
};

class Kappa;
class KappaSpellDash : public Spell
{
public:
        KappaSpellDash();
        ~KappaSpellDash();

        void init(Kappa * kappa, int left, int top);
        void activate();
        void deactivate();
        bool isReady(){return rdy;}
        bool testCombo(const keyHistory& keys);
        virtual bool update();
        void render();
        void setCoolDown(uint ms){cooldown = ms;}
        uint getCoolDown(){return cooldown;}
        void delayCoolDown(uint value){ltActivated+=value;}
        void accelerateCoolDown(uint value){ltActivated-=value;}
private:
        void init(GameManager * gm, int left, int top);

        Kappa* kappa;
        uint ltActivated;
        uint cooldown;
        double speedIncrease;
        uint length;
        bool rdy;
        bool activated;


};
//---------------------------------------------------------------------------
#endif
