//---------------------------------------------------------------------------


#pragma hdrstop

#include "spells.h"

SpellManager::SpellManager(keyHistory* k, GameManager* gm)
{
        keys = k;
        manager = gm;
        Sprite * t = SpriteFabric::newSprite("spell_table",gm->getRenderManager()->getContext());
        t->setPosition(300, 529);
        gm->getRenderManager()->adduiObject(t);
        table = t;
}
void SpellManager::addSpell(String name, Spell * sp)
{
        std::map<String,Spell*>::iterator it = spells.find(name);
        if (it != spells.end())
                delete it->second;
        spells[name]=sp;
}

void SpellManager::removeSpell(String name)
{
        std::map<String,Spell*>::iterator it = spells.find(name);
        if (it != spells.end())
        {
                delete it->second;
                spells.erase(it);
        }
}
void SpellManager::check()
{
        map_foreach(String,Spell*,it,spells)
                if (it->second->testCombo(*keys) && it->second->isReady())
                        it->second->activate();
}
void SpellManager::update()
{
        map_foreach(String,Spell*,it,spells)
                it->second->update();
}
void SpellManager::render()
{
        map_foreach(String,Spell*,it,spells)
                it->second->render();
}
void SpellManager::free()
{
        map_foreach(String,Spell*,it,spells)
                delete it->second;
        spells.clear();
        manager->getRenderManager()->remove(table);
}
//---------------------------------------------------------------------------
KappaSpellDash::KappaSpellDash()
{
        gm = 0;
        pb = 0;
}
KappaSpellDash::~KappaSpellDash()
{
        if (pb)
                delete pb;
}
void KappaSpellDash::init(Kappa * k, int left, int top)
{
        kappa = k;
        init(k->manager, left, top);
}
void KappaSpellDash::init(GameManager * gm, int left, int top)
{
        pb = new SpellProgressBar("config\\spell_kappa_dash.txt",gm->getRenderManager()->getContext());
        pb->setPosition(left, top);
        pb->setProgress(0.);
        cooldown = pb->getMax();
        combo = UserStats::getInstance()->getGlobal("kappa_spell_dash_combo");
        speedIncrease = 7.5;
        length = 250;
        ltActivated = ::GetTickCount();
        rdy=false;
        gm->getRenderManager()->adduiObject(pb);
}
void KappaSpellDash::activate()
{
        int kappaMovingDest = kappa->state&0xF;
        kappa->state = STATE_KAPPA_MODE_DASH | STATE_KAPPA_ROTATE | (kappa->state & 0xF00) | STATE_KAPPA_MOVING_ACTIVE | kappaMovingDest;
        kappa->setKappaRotate();
        if (kappaMovingDest == STATE_KAPPA_MOVING_LEFT)
                kappa->currentSpeedL = -speedIncrease*kappa->speed;
        else if (kappaMovingDest == STATE_KAPPA_MOVING_RIGHT)
                kappa->currentSpeedL = speedIncrease*kappa->speed;
        kappa->hp-=5;
        UserStats::getInstance()->setGlobal("kappa_hp",IntToStr(kappa->hp));
        ltActivated = ::GetTickCount();          
        rdy=false;
        activated = true;
}
bool KappaSpellDash::update()
{
        int timeLapsed = ::GetTickCount()-ltActivated;
        pb->setMax(cooldown);
        int timeLeft = max(cooldown-timeLapsed, 0);
        double pos = 1.-double(timeLeft)/double(cooldown);
        pb->setProgress(pos);
        if (timeLeft == 0)
                rdy=true;
        else
                rdy=false;
        if ((timeLapsed >=length) && activated)
                deactivate();
        return true;

}
void KappaSpellDash::deactivate()
{
        kappa->currentSpeedL/=speedIncrease;
        kappa->state = STATE_KAPPA_NORMAL | (kappa->state&0xF0F)| STATE_KAPPA_MOVING_ACTIVE;
        kappa->setKappaNormal();
        activated = false;
}
bool KappaSpellDash::testCombo(const keyHistory& keys)
{
        return keys.testCombo(combo);
}
void KappaSpellDash::render()
{
        pb->render();
}

//---------------------------------------------------------------------------

#pragma package(smart_init)

