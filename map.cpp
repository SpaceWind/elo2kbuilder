//---------------------------------------------------------------------------


#pragma hdrstop

#include "map.h"


GameObject * bindObject(String name, GameManager *mgr, FastBitmap * context)
{
        if (name.LowerCase() == "kappa")
                return new Kappa(mgr,context);
        return 0;


}

SubMapDesc::SubMapDesc(String _name, TStringList * _operations, bool _active)
{
        name = _name;
        operations = _operations;
        currentOperation = 0;
        active = _active;
}
String SubMapDesc::nextOperation()
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
String SubMapDesc::toString()
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
void SubMapDesc::setOperations(TStringList * ops)
{
        TStringList * filteredOperations = new TStringList();
        filteredOperations->Add("call "+name);
        for (int i=0; i<ops->Count; i++)
        {
                if (ops->Strings[i].LowerCase().Pos("addobject") ||
                    ops->Strings[i].LowerCase().Pos("moveobject") ||
                    ops->Strings[i].LowerCase().Pos("setobjectanimation") ||
                    ops->Strings[i].LowerCase().Pos("animateobject") ||
                    ops->Strings[i].LowerCase().Pos("removeobject") ||
                    ops->Strings[i].LowerCase().Pos("onobjectrect") ||
                    ops->Strings[i].LowerCase().Pos("onmapscroll"))
                {
                        filteredOperations->Add(ops->Strings[i]);
                }

        }
        delete ops;
        operations = filteredOperations;
}
//---------------------------------------------------------------------------
MapAnimate::MapAnimate()
{
        init(0,0,0,0,false);
}
void MapAnimate::init(GameObject *s, double destLeft, double destTop, double _speed, bool upd)
{
        object=s;
        sLeft = s?s->sprite->getDoubleRect().left:0;
        sTop  = s?s->sprite->getDoubleRect().top:0;
        dLeft = destLeft;
        dTop  = destTop;
        speed = _speed;
        up = upd;
        done = (sLeft == dLeft) && (sTop == dTop);
        dL = (dLeft>sLeft)? 1 : ((dLeft<sLeft)? -1: 0);
        dT = (dTop>sTop)? 1 : ((dTop<sTop)? -1: 0);
        started = false;
        lastUpdate = 0;
}

void MapAnimate::reverse()
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

bool MapAnimate::update()
{
        if (object==0)
                return true;
        if (lastUpdate == 0)
        {
                started = true;
                event = onStart;
                lastUpdate = ::GetTickCount();
                return done;
        }
        if (done)
                return true;

        double cLeft = object->sprite->getDoubleRect().left;
        double cTop  = object->sprite->getDoubleRect().top;

        double totalRange = fabs(dLeft-sLeft)+fabs(dTop-sTop);
        totalRange = (totalRange==0)?1:totalRange;

        cLeft += fabs((dLeft-sLeft)/totalRange)*speed/1000.
                 *double(::GetTickCount()-lastUpdate)*dL;
        cTop  += fabs((dTop-sTop)/totalRange)*speed/1000.
                 *double(::GetTickCount()-lastUpdate)*dT;
        object->sprite->setPosition(cLeft,cTop);
        if (up)
                object->update(::GetTickCount());

        event = "";


        bool leftDone = ((dL>0) && (cLeft > dLeft)) || ((dL < 0) && (cLeft < dLeft));
        bool topDone  = ((dT > 0) && (cTop  > dTop))|| ((dT < 0) && (cTop  < dTop));
        if (leftDone || topDone)
        {
                cLeft = dLeft;
                cTop  = dTop;
                object->sprite->setPosition(cLeft,cTop);
                event = onEnd;
                done  = true;
                return true;
        }

        lastUpdate = ::GetTickCount();
        return done;
}
//---------------------------------------------------------------------------
MapScrollEvent::MapScrollEvent(int * _scrollValueLeft, int * _scrollValueTop,
                               int _endScrollValueLeft, int _endScrollValueTop,
                               int _compareMethod, String event) : MapEvent(true,event)
{
        currentScrollValueLeft = _scrollValueLeft;
        currentScrollValueTop  = _scrollValueTop;
        endScrollValueLeft = _endScrollValueLeft;
        endScrollValueTop  = _endScrollValueTop;
        compareMethod      = _compareMethod;
        type = "mapScroll";
}
bool MapScrollEvent::update()
{
        if (!active)
                return false;
        switch (compareMethod)
        {
        case COMPARE_METHOD_MORE:
                if (((*currentScrollValueLeft) >= endScrollValueLeft) ||
                    ((*currentScrollValueTop)  >= endScrollValueTop))
                {
                        active = false;
                        return false;
                }
                break;
        case COMPARE_METHOD_LESS:
                if (((*currentScrollValueLeft) <= endScrollValueLeft) ||
                    ((*currentScrollValueTop)  <= endScrollValueTop))
                {
                        active = false;
                        return false;
                }
                break;
        default:
        case COMPARE_METHOD_EQUAL:
                if (((*currentScrollValueLeft) == endScrollValueLeft) ||
                    ((*currentScrollValueTop)  == endScrollValueTop))
                {
                        active = false;
                        return false;
                }
                break;
        }
        return true;
}


//---------------------------------------------------------------------------
Map::Map(String filename, GameManager * gm)
{
        manager = gm;
        Defines d;
        d.Load(filename);

        holdScrollingTop  = d.IsDefined("holdscrollingtop");
        holdScrollingLeft = d.IsDefined("holdscrollingleft");
        scrollBoundsTop   = d.IsDefined("scrollboundstop");
        scrollBoundsLeft  = d.IsDefined("scrollboundsleft");

        scrollSmoothVertical = d.IsDefined("smoothscrollingv")?StrToInt(d.Get("smoothscrollingv")):1024;
        scrollSmoothHorizontal = d.IsDefined("smoothscrollingh")?StrToInt(d.Get("smoothscrollingh")):1024;

        if (!holdScrollingLeft)
        {
                this->scrollLeftBound = d.IsDefined("leftrightbound")?
                                                StrToInt(d.Get("leftrightbound")):0;
                this->scrollLeftNegativeBound = d.IsDefined("leftleftbound")?
                                                StrToInt(d.Get("leftleftbound")):0;
        }
        else
                scrollLeftBound = scrollLeftNegativeBound = 0;
        if (!holdScrollingTop)
        {
                this->scrollTopBound = d.IsDefined("topbottombound")?
                                                StrToInt(d.Get("topbottombound")):0;
                this->scrollTopNegativeBound = d.IsDefined("toptopbound")?
                                                StrToInt(d.Get("toptopbound")):0;
        }
        else
                scrollTopBound = scrollTopNegativeBound = 0;


        if (scrollBoundsLeft && (!holdScrollingLeft))
        {
                maxScrollLeft = d.IsDefined("maxscrollleft")?
                                        StrToInt(d.Get("maxscrollleft")):0;
                minScrollLeft = d.IsDefined("minscrollleft")?
                                        StrToInt(d.Get("minscrollleft")):0;
        }
        else
                maxScrollLeft = minScrollLeft = 0;

        if (scrollBoundsTop && (!holdScrollingTop))
        {
                maxScrollTop = d.IsDefined("maxscrolltop")?
                                        StrToInt(d.Get("maxscrolltop")):0;
                minScrollTop = d.IsDefined("minscrolltop")?
                                        StrToInt(d.Get("minscrolltop")):0;
        }
        else
                maxScrollTop = minScrollTop = 0;
        scrollLeftValue = d.IsDefined("startscrollleft")?StrToInt(d.Get("startscrollleft")):0;
        scrollTopValue =  d.IsDefined("startscrolltop")? StrToInt(d.Get("startscrolltop")) :0;




        TStringList * subScenesList = d.GetList(".");
        for (int i=0; i<subScenesList->Count; i++)
                subScenes.push_back(new SubMapDesc(subScenesList->Strings[i],
                                                     d.GetList(subScenesList->Strings[i]),
                                                     (subScenesList->Strings[i].LowerCase()=="start")?true:false
                                                     ));
        done = false;
        started = false;
        delete subScenesList;

        if (!gm)
                scene = new Scene(filename, 0);
        else
                scene = new Scene(filename, gm->getRenderManager());
        scrollBoundObject = 0;

}
Map::~Map()
{
        flush();
}
void Map::flush()
{
      if (scene)
                delete scene;
        list_foreach(SubMapDesc*,it,subScenes)
                delete (*it);
        map_foreach(String, MapAnimate*, it, animations)
                delete it->second;
        map_foreach(String, GameObject*, it, objects)
        {
                manager->getRenderManager()->remove(it->second->sprite);
                manager->removeObject(it->second);
                delete it->second;
        }
        list_foreach(MapEvent*,it,events)
                delete (*it);
}
void Map::load(String filename)
{
        flush();
        Defines d;
        d.Load(filename);
        TStringList * subScenesList = d.GetList(".");
        for (int i=0; i<subScenesList->Count; i++)
                subScenes.push_back(new SubMapDesc(subScenesList->Strings[i],
                                                   d.GetList(subScenesList->Strings[i]),
                                                   (subScenesList->Strings[i].LowerCase()=="start")?true:false
                                                   ));
        done = false;
        started = false;
        delete subScenesList;

        if (!manager)
                scene = new Scene(filename, 0);
        else
                scene = new Scene(filename, manager->getRenderManager());
}
void Map::callSubMap(String name)
{
        SubMapDesc * smd = findSubMap(StringReplace(name,":","",TReplaceFlags()<<rfReplaceAll));
        if (smd)
                processString(smd->toString());
}

SubMapDesc* Map::findSubMap(String s)
{
        list_foreach(SubMapDesc *, it, subScenes)
        {
                if ((*it)->name == s)
                        return (*it);
        }
        return 0;
}
bool Map::processString(String s)
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

                TStringList * opParts = new TStringList();
                opParts->DelimitedText = op;
                UserStats * us = UserStats::getInstance();

                if (opParts->Strings[0].LowerCase() == "getobjectparam")
                {

                        std::map<String, GameObject *>::iterator it = objects.find(opParts->Strings[2]);
                        if (it != objects.end())
                        {
                                int value;
                                if (opParts->Strings[3].LowerCase() == "left")
                                        value = it->second->sprite->getRect().left;
                                else if (opParts->Strings[3].LowerCase() == "top")
                                        value = it->second->sprite->getRect().top;
                                else if (opParts->Strings[3].LowerCase() == "width")
                                        value = it->second->sprite->getRect().Width();
                                else if (opParts->Strings[3].LowerCase() == "height")
                                        value = it->second->sprite->getRect().Height();

                                us->setGlobal(opParts->Strings[1], IntToStr(value));
                                delete opParts;
                                continue;

                        }
                }

                if (us->processString(op))
                        continue;
                op = us->translateString(op);

                opParts->DelimitedText = op;

                if (opParts->Strings[0].LowerCase() == "addobject")
                {
                        GameObject * obj = buildObject(opParts);
                        if (obj)
                        {
                                objects[opParts->Strings[1].LowerCase()] = obj;
                                manager->sendMessage(obj->getSelfAddMessage());
                                processed = true;
                        }
                }
                else if (opParts->Strings[0].LowerCase() == "moveobject")
                {
                        std::map<String, GameObject *>::iterator it = objects.find(opParts->Strings[1]);
                        if (it != objects.end())
                        {
                                GameObject * obj = it->second;
                                obj->sprite->setPosition(StrToInt(opParts->Strings[2]),
                                                         StrToInt(opParts->Strings[3]));
                                processed = true;
                        }
                }
                else if (opParts->Strings[0].LowerCase() == "setobjectanimation")
                {
                        std::map<String, GameObject *>::iterator it = objects.find(opParts->Strings[1]);
                        if (it != objects.end())
                        {
                                GameObject * obj = it->second;
                                if (opParts->Strings[2].LowerCase() == "!start")
                                        obj->sprite->animate = true;
                                else if (opParts->Strings[2].LowerCase() == "!stop")
                                        obj->sprite->animate = false;
                                else
                                        obj->sprite->setAnimation(opParts->Strings[2]);
                                processed = true;
                        }
                }
                else if (opParts->Strings[0].LowerCase() == "animateobject")
                {
                        std::map<String, GameObject *>::iterator it = objects.find(opParts->Strings[2]);
                        if (it != objects.end())
                        {
                                GameObject * obj = it->second;

                                MapAnimate * mapAnimate = new MapAnimate(obj,
                                                                         StrToInt(opParts->Strings[3]),
                                                                         StrToInt(opParts->Strings[4]),
                                                                         StrToInt(opParts->Strings[5]),
                                                                         false);
                                animations[opParts->Strings[1].LowerCase()] = mapAnimate;
                                processed = true;
                        }
                }
                //objectanimation.animationName.onEnd :callfunc
                else if (opParts->Strings[0].LowerCase().Pos("objectanimation")==1)
                {
                        TStringList * params = new TStringList();
                        params->Delimiter = '.';
                        params->DelimitedText = opParts->Strings[0];
                        String aName   = params->Strings[1];
                        String onEvent = params->Strings[2].LowerCase();

                        std::map<String, MapAnimate *>::iterator it = animations.find(aName);
                        if (it != animations.end())
                        {
                                if (onEvent == "onend")
                                        it->second->onEnd = opParts->Strings[1];
                                else if (onEvent == "onstart")
                                        it->second->onStart = opParts->Strings[1];
                        }
                }
                else if (opParts->Strings[0].LowerCase() == "removeobject")
                {
                        std::map<String, GameObject *>::iterator it = objects.find(opParts->Strings[1]);
                        if (it != objects.end())
                        {
                                GameObject * obj = it->second;
                                objects.erase(it);
                                manager->removeObject(obj);
                                if (obj == this->scrollBoundObject)
                                        this->scrollBoundObject = 0;
                                processed = true;
                        }
                }
                else if (opParts->Strings[0].LowerCase() == "removeevents")
                {
                        list_foreach(MapEvent*,it, events)
                                delete (*it);
                        events.clear();
                }
                else if (opParts->Strings[0].LowerCase() == "onobjectrect")
                {
                        std::map<String, GameObject *>::iterator it = objects.find(opParts->Strings[1]);
                        if (it != objects.end())
                        {
                                GameObject * obj = it->second;
                                MapEvent * mapEvent = new PlayerRectEvent(obj,
                                                                          TRect(
                                                                          StrToInt(opParts->Strings[2]),
                                                                          StrToInt(opParts->Strings[3]),
                                                                          StrToInt(opParts->Strings[4]),
                                                                          StrToInt(opParts->Strings[5])),
                                                                          opParts->Strings[6]);
                                events.push_back(mapEvent);
                                processed = true;
                        }
                }
                else if (opParts->Strings[0].LowerCase() == "onmapscroll")
                {
                        MapEvent * mapEvent = new MapScrollEvent(&scrollLeftValue, &scrollTopValue,
                                                                StrToInt(opParts->Strings[1]),
                                                                StrToInt(opParts->Strings[2]),
                                                                ((opParts->Strings[3].LowerCase()=="more") ?
                                                                        COMPARE_METHOD_MORE :
                                                                        ((opParts->Strings[3].LowerCase() == "less")?
                                                                                COMPARE_METHOD_LESS: COMPARE_METHOD_EQUAL)),

                                                                opParts->Strings[4]);
                        events.push_back(mapEvent);
                        processed = true;
                }
                else if (opParts->Strings[0].c_str()[0] == ':')
                {
                        SubMapDesc * smd = findSubMap(StringReplace(opParts->Strings[0],":","",TReplaceFlags()<<rfReplaceAll));
                        if (smd)
                        {
                                scene->callSubScene(StringReplace(opParts->Strings[0],":","",TReplaceFlags()<<rfReplaceAll));
                                processString(smd->toString());

                                processed = true;
                        }
                }
                else if (opParts->Strings[0].LowerCase() == "done")
                {
                        done = true;
                        processed = true;
                }
                else if (opParts->Strings[0].LowerCase() == "bindscrollingto")
                {
                        std::map<String, GameObject *>::iterator it = objects.find(opParts->Strings[1]);
                        if (it != objects.end())
                        {
                                GameObject * obj = it->second;
                                scrollBoundObject = obj;
                                processed = true;
                        }
                }
                else if (opParts->Strings[0].LowerCase() == "unbindscrolling")
                {
                        scrollBoundObject = 0;
                        processed = true;
                }
                else if (opParts->Strings[0].LowerCase() == "_loadsound")
                {
                        SoundManager *sm = SoundManager::getInstance();
                        sm->addResource(opParts->Strings[1], opParts->Strings[2], opParts->Strings[3].LowerCase()=="true");
                        processed = true;
                }
                else if (opParts->Strings[0].LowerCase() == "_setsoundvolume")
                {
                        SoundManager *sm = SoundManager::getInstance();
                        sm->setResourceVolume(opParts->Strings[1], double(StrToInt(opParts->Strings[2]))/1024.,double(StrToInt(opParts->Strings[3]))/1024.);
                        processed = true;
                }
                else if (opParts->Strings[0].LowerCase() == "_playsound")
                {
                        SoundManager *sm = SoundManager::getInstance();
                        sm->Play(opParts->Strings[1]);
                        processed = true;
                }
                else if (opParts->Strings[0].LowerCase() == "_stopsound")
                {
                        SoundManager *sm = SoundManager::getInstance();
                        sm->Stop(opParts->Strings[1]);
                        processed = true;
                }
                else if (opParts->Strings[0].LowerCase() == "_unbindsound")
                {
                        SoundManager *sm = SoundManager::getInstance();
                        sm->unbindResource(opParts->Strings[1]);
                        processed = true;
                }
                else if (opParts->Strings[0].LowerCase() == "_freesound")
                {
                        SoundManager *sm = SoundManager::getInstance();
                        sm->freeResource(opParts->Strings[1]);
                        processed = true;
                }
                else if (opParts->Strings[0].LowerCase() == "_freeall")
                {
                        SoundManager *sm = SoundManager::getInstance();
                        sm->freeAll();
                        processed = true;
                }
                else if (opParts->Strings[0].LowerCase() == "_unbind")
                {
                        SoundManager *sm = SoundManager::getInstance();
                        sm->freeBinds();
                        processed = true;
                }
                else if (opParts->Strings[0].LowerCase() == "_bindsound")
                {
                        SoundManager *sm = SoundManager::getInstance();
                        //bindSound hryuhryu hohol1 kappa1 280 250 600 600
                        Sprite* srcObject = 0;
                        Sprite* listenerObject = 0;
                        std::map<String,GameObject*>::iterator srcit = objects.find(opParts->Strings[2]);
                        if (srcit == objects.end())
                        {
                                std::map<String, Sprite*>::iterator ssrcit = scene->sprites.find(opParts->Strings[2]);
                                if (ssrcit != scene->sprites.end())
                                        srcObject = ssrcit->second;
                        }
                        else
                                srcObject = srcit->second->sprite;
                        std::map<String,GameObject*>::iterator listit = objects.find(opParts->Strings[3]);
                        if (listit != objects.end())
                                listenerObject = listit->second->sprite;
                        if (srcObject && listenerObject)
                        {
                                 soundBindDescriptor sbd;
                                 sbd.src = srcObject->getPosValues();
                                 sbd.listener = listenerObject->getPosValues();
                                 TDoubleRect scr;
                                 scr.left = scrollLeftValue;
                                 scr.top  = scrollTopValue;
                                 scr.right= scrollLeftValue+MAP_SIZE_WIDTH;
                                 scr.bottom=scrollTopValue+MAP_SIZE_HEIGHT;
                                 sbd.screenRect = scr;
                                 TDoubleRect coefs;
                                 coefs.left = double(StrToInt(opParts->Strings[4]))/1024.;
                                 coefs.top  = double(StrToInt(opParts->Strings[5]))/1024.;
                                 coefs.right= double(StrToInt(opParts->Strings[6]))/1024.;
                                 coefs.bottom=double(StrToInt(opParts->Strings[7]))/1024.;
                                 sbd.coefs = coefs;
                                 sm->bindResource(opParts->Strings[1], sbd);
                        }
                        processed = true;
                }




                delete opParts;
        }
        delete splitOperations;
        return processed;
}

void Map::update()
{
        if (!started)
        {
                processString(findSubMap("start")->toString());
                started = true;
        }

        scene->update();


        map_foreach(String, MapAnimate*, it, animations)
        {
                it->second->update();
                if (it->second->event != "")
                {
                        scene->processString(it->second->event);
                        processString(it->second->event);
                        it->second->event="";
                }
        }


        if (scrollBoundObject)
        {
                TRect r = scrollBoundObject->sprite->getRect();
                r.left  -= scrollLeftValue;
                r.right -= scrollLeftValue;
                r.top   -= scrollTopValue;
                r.bottom -= scrollTopValue;

                if ((r.right > scrollLeftBound) && (holdScrollingLeft == false))
                        scrollLeftValue += max(min(r.right-scrollLeftBound,scrollSmoothHorizontal),-scrollSmoothHorizontal);
                if ((r.bottom > scrollTopBound) && (holdScrollingTop == false))
                        scrollTopValue += max(min(r.bottom-scrollTopBound,scrollSmoothVertical),-scrollSmoothVertical);
                if ((r.left < scrollLeftNegativeBound) && (holdScrollingLeft == false))
                        scrollLeftValue += max(min(r.left - scrollLeftNegativeBound,scrollSmoothHorizontal),-scrollSmoothHorizontal);
                if ((r.top < scrollTopNegativeBound) && (holdScrollingTop == false))
                        scrollTopValue += max(min(r.top - scrollTopNegativeBound,scrollSmoothVertical),-scrollSmoothVertical);

                if (scrollBoundsLeft)
                        if (scrollLeftValue > maxScrollLeft)
                                scrollLeftValue = maxScrollLeft;
                        else if (scrollLeftValue < minScrollLeft)
                                scrollLeftValue = minScrollLeft;
                if (scrollBoundsTop)
                        if (scrollTopValue > maxScrollTop)
                                scrollTopValue = maxScrollTop;
                        else if (scrollTopValue < minScrollTop)
                                scrollTopValue = minScrollTop;

        }


        list_foreach(MapEvent*, it, events)
        {
                if ((*it)->update())
                {
                        scene->processString((*it)->event);
                        processString((*it)->event);
                }
        }




        SoundManager * sm = SoundManager::getInstance();
        TDoubleRect scr;
        scr.left = this->scrollLeftValue;
        scr.top  = this->scrollTopValue;
        scr.right= this->scrollLeftValue + MAP_SIZE_WIDTH;
        scr.bottom=this->scrollTopValue + MAP_SIZE_HEIGHT;
        sm->setBindScreenRect(scr);
        sm->updateBinds();

}
bool Map::isScrollingAllowed()
{
        if (holdScrollingLeft && holdScrollingTop)
                return false;
        if ((scrollLeftValue == 0) && (scrollTopValue == 0))
                return false;
        return true;
}
void Map::render()
{
        if (isScrollingAllowed())
        {
                TRect r = TRect(scrollLeftValue,scrollTopValue,
                          scrollLeftValue+MAP_SIZE_WIDTH,scrollTopValue+MAP_SIZE_HEIGHT);

                manager->getRenderManager()->scrollRectRender(r);
        }
        else
                manager->getRenderManager()->render();

}

GameObject * Map::buildObject(TStringList* params)
{
        UserStats *us = UserStats::getInstance();
        if (params->Strings[2].LowerCase() == "kappa")
                return new Kappa(manager, manager->getRenderManager()->getContext());
        if (params->Strings[2].LowerCase() == "customkappa")
                return new Kappa(manager, manager->getRenderManager()->getContext(),
                           us->getGlobal("kappa_sprite"), us->getGlobal("kappa_rotate_sprite"),
                           StrToInt(us->getGlobal("kappa_maxhp")), StrToInt(us->getGlobal("kappa_hp")),
                           StrToInt(us->getGlobal("kappa_speed")), StrToInt(us->getGlobal("kappa_jump")),
                           StrToInt(us->getGlobal("kappa_sdl")),   StrToInt(us->getGlobal("kappa_sda")),
                           StrToInt(us->getGlobal("kappa_acc")),   StrToInt(us->getGlobal("kappa_damage")),
                           StrToInt(us->getGlobal("kappa_elo")));
        else if (params->Strings[2].LowerCase() == "boombox")
                return new Wall(manager, manager->getRenderManager()->getContext(),
                                "boombox","");
        else if ((params->Strings[2].LowerCase() == "box") && (params->Count == 5))
                return new Wall(manager,manager->getRenderManager()->getContext(),
                                "box_"+params->Strings[3]+"_"+params->Strings[4],"");
        else if ((params->Strings[2].LowerCase() == "wall") && (params->Count == 4))
                return new Wall(manager, manager->getRenderManager()->getContext(),
                                params->Strings[3],"");
        return 0;

}




//---------------------------------------------------------------------------

#pragma package(smart_init)
