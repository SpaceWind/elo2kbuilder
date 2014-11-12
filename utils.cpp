//---------------------------------------------------------------------------


#pragma hdrstop

#include "utils.h"
bool valueInRange(int value, int min, int max)
{
        return (value >= min) && (value <= max);
}
TRect convertRect(TRect r, bool toRightBottomFmt)
{
        TRect res;
        res.left=r.left;
        res.top=r.top;
        if (toRightBottomFmt)
        {
                r.left = -r.left;
                r.top = -r.top;
        }
        res.right = r.right-r.left;
        res.bottom = r.bottom-r.top;
        return res;
}

int rectCollision(TRect A, TRect B, bool dontCalculate)
{
    bool xOverlap = valueInRange(A.left, B.left, B.left + B.right) ||
                    valueInRange(B.left, A.left, A.left + A.right);

    bool yOverlap = valueInRange(A.top, B.top, B.top + B.bottom) ||
                    valueInRange(B.top, A.top, A.top + A.bottom);

    if ((xOverlap == false) || (yOverlap == false))
        return NO_COLLISION;
    if (dontCalculate)
        return 1;

    TRect overlap;
    bool horizontal;
    overlap.left = valueInRange(A.left, B.left, B.left + B.right)?A.left:B.left;
    overlap.top  = valueInRange(A.top, B.top, B.top + B.bottom)?A.top:B.top;
    overlap.right= min(B.left+B.right,A.left+A.right);
    overlap.bottom = min(B.top+B.bottom, A.top+A.bottom);
    if (overlap.Width()<overlap.Height())
        horizontal = true;
    else
        horizontal = false;
    if (horizontal)
        if (A.left < B.left)
                return COLLISION_RIGHT;
        else
                return COLLISION_LEFT;
    else if (A.top < B.top)
                return COLLISION_DOWN;
         else
                return COLLISION_UP;
}

bool rectInRect (TRect A, TRect B)
{
        int dW = -(A.Width() - B.Width());
        int dH = -(A.Height() - B.Height());
        if ((dW<=0) || (dH<=0))
                return false;
        bool lOverlap = false;
        bool tOverlap = false;
        if ((A.left>=B.left) && ((A.left-B.left)<=dW))
                lOverlap = true;
        if ((A.top>=B.top) && ((A.top-B.top)<=dH))
                tOverlap = true;
        if (lOverlap && tOverlap)
                return true;
        else
                return false;

}
int rectRange(TRect A, TRect B)
{
        return sqrt((A.left-B.left)*(A.left-B.left) + (A.top-B.top)*(A.top-B.top));
}
//---------------------------------------------------------------------------
KeyUpdateProducer::KeyUpdateProducer()
{
}
KeyUpdateProducer::~KeyUpdateProducer()
{
        ;
}
void KeyUpdateProducer::addListener(KeyListener * l)
{
        listeners.push_back(l);
}
void KeyUpdateProducer::removeListener(KeyListener * l)
{
        listeners.remove(l);
}
void KeyUpdateProducer::clear()
{
        listeners.clear();
}
void KeyUpdateProducer::update()
{
        kbState newResult = kbState::get(KB_STATE_BOOLEAN_FORMAT);
        for (int i=0; i<256; i++)
        {
                if (newResult[i]!=prevResult[i])
                {
                        if (newResult[i] == 0)
                        {
                                int a;
                                a = a+3;
                        }
                        uint vk = ((newResult[i]==1) ? KEY_DOWN : KEY_UP)+ i;
                        for (std::list<KeyListener*>::iterator i = listeners.begin(); i!=listeners.end(); i++)
                                (*i)->keyUpdate(vk);
                }
        }
        prevResult = newResult;
}

kbState kbState::get(int format)
{
        kbState out;
        GetKeyboardState(out.state);
        if (format == KB_STATE_WINAPI_FORMAT);
        else if (format == KB_STATE_BOOLEAN_FORMAT)
        {
                for (int i=0; i<256; i++)
                        out.state[i] = (out.state[i]&128)?1:0;
                return out;
        }
        return out;
}
//---------------------------------------------------------------------------

void keyHistory::updateKey(uint spKey)
{
        bool up = (spKey&0xFF00) == KEY_UP;
        if (index == KH_BUFFER_BOUND)
        {
                for (int i=0; i<KH_BUFFER_BOUND; i++)
                {
                        keys[i]=keys[i+1];
                        keyTime[i]=keyTime[i+1];
                        keyUp[i] = keyUp[i+1];
                }
                keys[KH_BUFFER_BOUND] = spKey&0xFF;
                keyUp[KH_BUFFER_BOUND]= up;
                keyTime[KH_BUFFER_BOUND] = ::GetTickCount();
        }
        else
        {
                keys[index] = spKey&0xFF;
                keyUp[index]= up;
                keyTime[index] = ::GetTickCount();
                index++;
        }
}
bool keyHistory::testCombo(String c) const
{
        String combo =  StringReplace(
                        StringReplace(c," ","",TReplaceFlags()<<rfReplaceAll<<rfIgnoreCase),
                        ".","*",TReplaceFlags()<<rfReplaceAll<<rfIgnoreCase);

        int _keys [16]; memset(_keys,0,16);
        int lastkey = 0;
        TStringList * dcomp = new TStringList();
        dcomp->Delimiter = '-';
        dcomp->DelimitedText = combo;
        uint maxLen = StrToInt(dcomp->Strings[0]);
        combo = dcomp->Strings[1];
        bool mode = 0;
        if (index>8)
        {
                int a=2;
                a = 2+2+a;
        }
        for (int i=0; i<combo.Length(); i++)
        {
                if (combo.c_str()[i] == '!')
                {
                        mode = 1;
                        continue;
                }
                else    if (i!=0)
                                if (combo.c_str()[i-1] != '!')
                                        mode = 0;
                _keys[lastkey] = mode?KEY_UP:KEY_DOWN;
                if (combo.c_str()[i] == '>')
                        _keys[lastkey] += VK_RIGHT;
                else if (combo.c_str()[i] == '<')
                        _keys[lastkey] += VK_LEFT;
                else if (combo.c_str()[i] == '^')
                        _keys[lastkey] += VK_UP;
                else if (combo.c_str()[i] == '*')
                        _keys[lastkey] += VK_DOWN;
                else
                        _keys[lastkey] += combo.c_str()[i];
                lastkey++;
        }
        bool comboFound = false;
        for (int ki = 0; ki<KH_BUFFER_SIZE+1-lastkey; ki++)
        {
                if (getSKey(ki) == 0)
                        break;
                for (int ci=0; ci<lastkey;ci++)
                {
                        if (getSKey(ki+ci) != _keys[ci])
                        {
                                comboFound = false;
                                break;
                        }
                        else if ((ci == lastkey-1) && (getSKey(ki+ci) == _keys[ci]))
                        {
                                if ((keyTime[ki+ci] - keyTime[ki]) < maxLen)
                                if (::GetTickCount()-keyTime[ki+ci] < 1000)
                                {
                                        comboFound = true;
                                        break;
                                }
                        }
                }
                if (comboFound)
                {
                        int a=2+1;
                        a = a+2;
                        break;
                }
        }
        return comboFound;

}
//---------------------------------------------------------------------------
PhysForce operator+ (const PhysForce& pf1, const PhysForce& pf2)
{
        double sfVecL = pf1.power*pf1.fVecLeft + pf2.power*pf2.fVecLeft;
        double sfVecT = pf1.power*pf1.fVecTop + pf2.power*pf2.fVecTop;
        double sfp = fabs(sfVecL) + fabs(sfVecT);
        sfVecL/=sfp; sfVecT/=sfp;
        double spVecL = pf1.impulse*pf1.pVecLeft + pf2.impulse*pf2.pVecLeft;
        double spVecT = pf1.impulse*pf1.pVecTop + pf2.impulse*pf2.pVecTop;
        double spp = fabs(spVecL) + fabs(spVecT);
        spVecL/=spp; spVecT/=spp;
        double sip = sfp+spp;

        return PhysForce::create(sfp,spp,sip,sfVecL,sfVecT,spVecL,sfVecT);

}
PhysForce::PhysForce()
{
        init(0.,0.,0.,0.,0.,0.,0.);
}
PhysForce::~PhysForce()
{
;
}
void PhysForce::init(double f, double p, double i, double fVecL, double fVecT, double pVecL, double pVecT)
{
        power=f;
        impulse=p;
        inertia=i;
        fVecLeft = fVecL;
        fVecTop  = fVecT;
        pVecLeft = pVecL;
        pVecTop  = pVecL;
        iVecLeft = iVecTop = 0.;
        initTime = ::GetTickCount();
        active = true;
}
PhysForce PhysForce::create(double f, double p, double i, double fVecL, double fVecT, double pVecL, double pVecT)
{
        PhysForce pf;
        pf.init(f,p,i,fVecL,fVecT,pVecL,pVecT);
        return pf;
}
double PhysForce::getVl()
{
        if (done)
                return 0.;
        if (active)
                return power*fVecLeft*double(::GetTickCount())/1000. + impulse*pVecLeft;
        else
        {
                double inertiaForce = inertia*double(::GetTickCount()-stopTime)/1000.;
                if (inertiaForce >= impulse)
                {
                        done = true;
                        return 0.;
                }
                return impulse*pVecLeft - inertiaForce*iVecLeft;
        }
}
double PhysForce::getVt()
{
        if (done)
                return 0.;
        if (active)
                return power*fVecTop*double(::GetTickCount())/1000. + impulse*pVecTop;
        else
        {
                double inertiaForce = inertia*double(::GetTickCount()-stopTime)/1000.;
                if (inertiaForce >= impulse)
                {
                        done = true;
                        return 0.;
                }
                return impulse*pVecTop - inertiaForce*iVecTop;
        }
}
void PhysForce::stop()
{
        active = false;

        pVecLeft += power*fVecLeft*double(::GetTickCount())/1000.;
        pVecTop  += power*fVecTop*double(::GetTickCount())/1000.;
        double pVecCoef = fabs(pVecLeft) + fabs(pVecTop);
        impulse  *= pVecCoef;
        pVecLeft /= pVecCoef;
        pVecTop  /= pVecTop;

        iVecLeft = -pVecLeft;
        iVecTop  = -pVecTop;
        stopTime = ::GetTickCount();
}
void PhysForce::start()
{
        active = true;
        initTime = ::GetTickCount();
}
//---------------------------------------------------------------------------
void PhysObject::addForce(String name, double f, double p, double i, double fVecLeft, double fVecTop, double pVecLeft, double pVecTop)
{
        PhysForce pf = PhysForce::create(f,p,i,fVecLeft, fVecTop, pVecLeft, pVecTop);
        forces[name]=pf;
}
void PhysObject::stopForce(String name)
{
        std::map<String,PhysForce>::iterator it = forces.find(name);
        if (it != forces.end())
                it->second.stop();
}
void PhysObject::startForce(String name)
{
        std::map<String,PhysForce>::iterator it = forces.find(name);
        if (it != forces.end())
                it->second.start();
}
void PhysObject::invert(String name, bool horForce, bool vertForce)
{
        std::map<String,PhysForce>::iterator it = forces.find(name);
        if (it != forces.end())
        {
                if (horForce)
                {
                        it->second.fVecLeft *= -1.;
                        it->second.pVecLeft *= -1.;
                        it->second.iVecLeft *= -1.;
                }
                if (vertForce)
                {
                        it->second.fVecTop *= -1.;
                        it->second.pVecTop *= -1.;
                        it->second.iVecTop *= -1.;
                }
        }
}
void PhysObject::removeForce(String name)
{
        std::map<String,PhysForce>::iterator it = forces.find(name);
        if (it != forces.end())
                forces.erase(it);
}
double PhysObject::getVl()
{
        double vl=0;
        map_foreach(String,PhysForce,it,forces)
                vl+=it->second.getVl();
        return vl;
}
double PhysObject::getVt()
{
        double vt=0;
        map_foreach(String,PhysForce,it,forces)
                vt+=it->second.getVt();
        return vt;
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------

#pragma package(smart_init)
