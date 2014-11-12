//---------------------------------------------------------------------------


#pragma hdrstop

#include "scenes.h"
SceneAnimate::SceneAnimate()
{
        init(0,0,0,0,true);
}
void SceneAnimate::init(Sprite *s, double destLeft, double destTop, double _speed, bool isLinear)
{
        sprite=s;
        sLeft = s?s->getDoubleRect().left:0;
        sTop  = s?s->getDoubleRect().top:0;
        dLeft = destLeft;
        dTop  = destTop;
        speed = _speed;
        linear = isLinear;
        done = (sLeft == dLeft) && (sTop == dTop);
        dL = (dLeft>sLeft)? 1 : ((dLeft<sLeft)? -1: 0);
        dT = (dTop>sTop)? 1 : ((dTop<sTop)? -1: 0);
        started = false;
        lastUpdate = 0;
}

void SceneAnimate::reverse()
{
        done = false;
        double temp = sLeft;
        sLeft = dLeft;
        dLeft = temp;
        lastUpdate=0;
        temp = sTop;
        sTop = dTop;
        dTop = temp;
        dL *= -1;
        dT *= -1;
        started=false;
}

bool SceneAnimate::update()
{
        if (sprite==0)
                return done;
        if (lastUpdate == 0)
        {
                started = true;
                event = onStart;
                lastUpdate = ::GetTickCount();
                return done;
        }
        if (done)
                return true;

        double cLeft = sprite->getDoubleRect().left;
        double cTop  = sprite->getDoubleRect().top;

        double totalRange = fabs(dLeft-sLeft)+fabs(dTop-sTop);
        totalRange = (totalRange==0)?1:totalRange;

        cLeft += fabs((dLeft-sLeft)/totalRange)*speed/1000.
                 *double(::GetTickCount()-lastUpdate)*dL
                 *(linear?1:2-fabs(4.*fabs(dLeft-cLeft)/fabs(dLeft-sLeft)-2));
        cTop  += fabs((dTop-sTop)/totalRange)*speed/1000.
                 *double(::GetTickCount()-lastUpdate)*dT
                 *(linear?1:2-fabs(4.*fabs(dTop-cTop)/fabs(dTop-sTop)-2));
        this->sprite->setPosition(cLeft,cTop);

        event = "";


        if (((dL > 0) && (cLeft >= dLeft)) ||
            ((dT > 0) && (cTop  >= dTop))  ||
            ((dL < 0) && (cLeft <= dLeft)) ||
            ((dT < 0) && (cTop  <= dTop)))
        {
                event = onEnd;
                done  = true;
                return true;
        }

        lastUpdate = ::GetTickCount();
        return done;
}
//---------------------------------------------------------------------------
SubSceneDesc::SubSceneDesc(String _name, TStringList * _operations, bool _active)
{
        name = _name;
        operations = _operations;
        currentOperation = 0;
        active = _active;
}
String SubSceneDesc::nextOperation()
{
        if (operations)
                if (operations->Count < currentOperation)
                {
                        currentOperation++;
                        if (operations->Count == currentOperation)
                                active = false;
                        return operations->Strings[currentOperation-1];
                }
        return "";
}
String SubSceneDesc::toString()
{
        String s;
        if (operations)
                for (int i=0; i<operations->Count; i++)
                {
                        s = s + operations->Strings[i];
                        if (i != operations->Count-1)
                                s = s + ";";
                }
        return s;
}
//---------------------------------------------------------------------------
bool AnimateValue::update()
{
        if (done)
                return true;
        if (lastUpdate == 0)
        {
                lastUpdate = ::GetTickCount();
                current = start;
                return false;
        }
        double coef = 0;
        if (dest>start)
                coef = 1;
        else if (dest<start)
                coef = -1;
        current += double(::GetTickCount()-lastUpdate)/1000.*speed*coef;
        if (coef == 1)
                if (current > dest)
                {
                        done = true;
                        return true;
                }
        if (coef == -1)
                if (current < dest)
                {
                        done = true;
                        return true;
                }
        lastUpdate = ::GetTickCount();
        return false;

}
//---------------------------------------------------------------------------
Scene::Scene(String filename, RenderManager * _rm)
{
        rm = _rm;
        Defines d;
        d.Load(filename);
        TStringList * subScenesList = d.GetList(".");
        for (int i=0; i<subScenesList->Count; i++)
                subScenes.push_back(new SubSceneDesc(subScenesList->Strings[i],
                                                     d.GetList(subScenesList->Strings[i]),
                                                     (subScenesList->Strings[i].LowerCase()=="start")?true:false
                                                     ));
        done = false;
        started = false;
        delete subScenesList;
}
Scene::~Scene()
{
        list_foreach(SceneTimer*,it,timers)
                delete (*it);
        timers.clear();


        list_foreach(SubSceneDesc*,it,subScenes)
                delete (*it);
        subScenes.clear();

        map_foreach(String,SceneAnimate*,it,animations)
                delete it->second;
        animations.clear();

        map_foreach(String,Sprite*,it,sprites)
        {
                rm->remove(it->second);
                delete it->second;
        }
        map_foreach(String,BindedProgressBar*,it,progressBars)
        {
                rm->remove(&(it->second->pb->picture));
                delete it->second;
        }
        rm->clear(false, true);
        sprites.clear();
}

SubSceneDesc* Scene::findSubScene(String s)
{
        list_foreach(SubSceneDesc*,it,subScenes)
        {
                SubSceneDesc * ssd = (*it);
                if (ssd->name == s)
                        return (*it);
        }
        return 0;
}
bool Scene::processString(String s)
{
        s=StringReplace(s," ","^",TReplaceFlags()<<rfReplaceAll);
        TStringList * splitOperations = new TStringList();
        splitOperations->Delimiter = ';';
        splitOperations->DelimitedText = s;
        for (int i=0; i<splitOperations->Count; i++)
                splitOperations->Strings[i] = StringReplace(splitOperations->Strings[i], "^", " ",TReplaceFlags()<<rfReplaceAll);

        bool processed = false;
        for (int i=0; i<splitOperations->Count; i++)
        {
                String op = StringReplace(splitOperations->Strings[i],",",":",TReplaceFlags()<<rfReplaceAll);

                UserStats * us = UserStats::getInstance();
                if (us->processString(op, true))
                        continue;
                op = us->translateString(op);

                TStringList * opParts = new TStringList();
                opParts->DelimitedText = op;

                if (opParts->Strings[0].LowerCase() == "addsprite")
                {
                        SpriteFabric * sf = SpriteFabric::getInstance();
                        Sprite * s = sf->SFM_CURRENT(opParts->Strings[1],rm->getContext());
                        sprites[opParts->Strings[2]] = s;
                        rm->add(s);
                        processed = true;
                }
                if (opParts->Strings[0].LowerCase() == "addstaticsprite")
                {
                        SpriteFabric * sf = SpriteFabric::getInstance();
                        Sprite * s = sf->newSprite(opParts->Strings[1],rm->getContext());
                        sprites[opParts->Strings[2]] = s;
                        Renderable * r = s;
                        rm->add(r);
                        processed = true;
                }
                else if (opParts->Strings[0].LowerCase() == "mval")
                {
                        AnimateValue av;
                        av.start = StrToFloat(StringReplace(opParts->Strings[2],":",",",TReplaceFlags()<<rfReplaceAll));
                        av.dest  = StrToFloat(StringReplace(opParts->Strings[3],":",",",TReplaceFlags()<<rfReplaceAll));
                        av.speed = StrToFloat(StringReplace(opParts->Strings[4],":",",",TReplaceFlags()<<rfReplaceAll));
                        values[opParts->Strings[1]] = av;
                        processed = true;
                }
                else if (opParts->Strings[0].LowerCase() == "addprogressbar")
                {
                        ProgressBar * pb = new ProgressBar("config\\"+opParts->Strings[2],rm->getContext());
                        pb->setPosition(StrToInt(opParts->Strings[3]), StrToInt(opParts->Strings[4]));
                        AnimateValue * av;
                        map_foreach(String,AnimateValue,it,values)
                                if (it->first == opParts->Strings[5])
                                {
                                        av = &(it->second);
                                        break;
                                }
                        BindedProgressBar * bpb = new BindedProgressBar();
                        bpb->pb = pb;
                        bpb->av = av;
                        progressBars[opParts->Strings[1]] = bpb;
                        rm->add(pb);
                }
                else if (opParts->Strings[0].LowerCase() == "movesprite")
                {
                        std::map<String, Sprite *>::iterator it = sprites.find(opParts->Strings[1]);
                        if (it != sprites.end())
                        {
                                Sprite * s = it->second;
                                s->setPosition(StrToInt(opParts->Strings[2]),StrToInt(opParts->Strings[3]));
                                processed = true;
                        }
                        else    processed = false;
                }
                else if (opParts->Strings[0].LowerCase() == "animate")
                {
                        String name = opParts->Strings[1];
                        String spriteName = opParts->Strings[2];
                        int dLeft = StrToInt(opParts->Strings[3]), dTop = StrToInt(opParts->Strings[4]);
                        int speed = StrToInt(opParts->Strings[5]);
                        bool linear = (opParts->Strings[6].LowerCase() == "linear")?true:false;

                        std::map<String, Sprite *>::iterator it = sprites.find(spriteName);
                        if (it != sprites.end())
                        {
                                animations[name] = new SceneAnimate(it->second,dLeft,dTop,speed,linear);
                                processed = true;
                                done = false;
                        }
                        else    processed = false;
                }
                else if (opParts->Strings[0].LowerCase() == "waitcall")
                {
                        SceneTimer * st = new SceneTimer(StrToInt(opParts->Strings[1]), opParts->Strings[2]);
                        timers.push_back(st);
                        processed = true;
                }
                else if (opParts->Strings[0].LowerCase() == "removesprite")
                {
                        std::map<String, Sprite *>::iterator it = sprites.find(opParts->Strings[1]);
                        if (it != sprites.end())
                        {
                                Sprite * s = it->second;
                                sprites.erase(it);
                                rm->remove(it->second);
                                delete s;
                                processed = true;
                        }
                }
                else if (opParts->Strings[0].LowerCase() == "removestaticsprite")
                {
                        std::map<String, Sprite *>::iterator it = sprites.find(opParts->Strings[1]);
                        if (it != sprites.end())
                        {
                                Sprite * s = it->second;
                                sprites.erase(it);
                                Renderable * r = s;
                                rm->remove(r);
                                delete s;
                                processed = true;
                        }
                }
                else if (opParts->Strings[0].c_str()[0] == ':')
                {
                        SubSceneDesc * ssd = findSubScene(StringReplace(opParts->Strings[0],":","",TReplaceFlags()<<rfReplaceAll));
                        if (ssd)
                        {
                                processString(ssd->toString());
                                processed = true;
                        }
                }
                else if (opParts->Strings[0].LowerCase() == "playsound")
                {
                        PlaySound(String("sounds\\"+opParts->Strings[1]+".wav").c_str(),NULL,SND_ASYNC);
                        processed = true;
                }
                else if (opParts->Strings[0].LowerCase() == "playsoundand")
                {
                        PlaySound(String("sounds\\"+opParts->Strings[1]+".wav").c_str(),NULL,SND_ASYNC);
                        Defines sd;
                        sd.Load("sounds\\!len.def");
                        timers.push_back(new SceneTimer(StrToInt(sd.Get(opParts->Strings[1]+".wav")),opParts->Strings[2]));

                        processed = true;
                }
                else if (opParts->Strings[0].LowerCase() == "setanimation")
                {
                        std::map<String, Sprite *>::iterator it = sprites.find(opParts->Strings[1]);
                        if (it != sprites.end())
                        {
                                Sprite * s = it->second;
                                if (opParts->Strings[2] == "!start" || opParts->Strings[2] == "!stop")
                                        s->animate = (opParts->Strings[2] == "!start")? true:false;
                                else
                                        s->setAnimation(opParts->Strings[2]);
                                processed = true;
                        }
                        else    processed = false;
                }
                else if (opParts->Strings[0].LowerCase().Pos("animation")==1)
                {
                        TStringList * params = new TStringList();
                        params->Delimiter = '.';
                        params->DelimitedText = opParts->Strings[0];
                        String aName   = params->Strings[1];
                        String onEvent = params->Strings[2].LowerCase();

                        std::map <String, SceneAnimate*>::iterator it =  animations.find(aName);
                        if (it != animations.end())
                        {
                                SceneAnimate * sa = it->second;
                                if (onEvent == "onstart")
                                        sa->onStart = opParts->Strings[1];
                                else if (onEvent == "onend")
                                        sa->onEnd = opParts->Strings[1];
                                processed = true;
                        }
                        delete params;
                }
                else if (opParts->Strings[0].LowerCase() == "changescene")
                {
                        if (FileExists(opParts->Strings[1]))
                        {
                                load(opParts->Strings[1]);
                                return true;
                        }
                }
		else if (opParts->Strings[0].LowerCase() == "scenedone")
		{
			done = true;
			return true;
		}
                delete opParts;
        }
        delete splitOperations;
        return processed;
}
void Scene::load(String filename)
{
        for (std::list<SceneTimer*>::iterator it = timers.begin(); it != timers.end(); it++)
                delete (*it);
        timers.clear();


        for (std::list<SubSceneDesc*>::iterator it = subScenes.begin(); it != subScenes.end(); it++)
                delete (*it);
        subScenes.clear();


        for (std::map<String, SceneAnimate*>::iterator it = animations.begin(); it != animations.end(); it++)
                delete it->second;
        animations.clear();

        for (std::map<String, Sprite*>::iterator it = sprites.begin(); it != sprites.end(); it++)
        {
                rm->remove(it->second);
                delete it->second;
        }
        sprites.clear();
        
        Defines d;
        d.Load(filename);
        TStringList * subScenesList = d.GetList(".");
        for (int i=0; i<subScenesList->Count; i++)
                subScenes.push_back(new SubSceneDesc(subScenesList->Strings[i],
                                                     d.GetList(subScenesList->Strings[i]),
                                                     (subScenesList->Strings[i].LowerCase()=="start")?true:false
                                                     ));
        delete subScenesList;
        done = started = false;
        delete subScenesList;
}
void Scene::update()
{
   	if (done)
   		return;
        if (!started)
        {
                processString(findSubScene("start")->toString());
                started = true;
        }
        bool noTimers = false, noAnimations = true;

        list_foreach(SceneTimer*,it,timers)
        {
                if (!(*it)->isDone())
                if ((*it)->update())
                {
                        if ((*it)->ops != "")
                                processString((*it)->ops);
                        delete (*it);
                        timers.remove(*it);
                        it = timers.begin();
                }
        }
        noTimers = timers.empty();
        map_foreach(String,SceneAnimate*,it,animations)
        {
                if (done)
                        return;
                if (!it->second->done)
                {
                        it->second->update();
                        noAnimations = false;
                        if (it->second->event != "")
                                processString(it->second->event);
                }
        }
        map_foreach(String,AnimateValue,it,values)
                it->second.update();
        map_foreach(String,BindedProgressBar*,it,progressBars)
                it->second->update();
        done = noTimers && noAnimations;
}
void Scene::callSubScene(String name)
{
        SubSceneDesc * ssd = findSubScene(StringReplace(name,":","",TReplaceFlags()<<rfReplaceAll));
        if (ssd)
                processString(ssd->toString());
}

//---------------------------------------------------------------------------

#pragma package(smart_init)

