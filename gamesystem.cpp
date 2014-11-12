//---------------------------------------------------------------------------


#pragma hdrstop

#include "gamesystem.h"

IMessage* IMessage::createFreeMessage()
{
        IMessage *m = new IMessage();
        m->type = MSG_FREE;
        return m;
}
IMessage* IMessage::createMoveRequestMessage(GameObject * s, double l, double t)
{
        IMessage *m = new IMessage();
        m->type = MSG_MOVE_REQUEST;
        m->sender = s;
        m->left = l;
        m->top = t;
        return m;
}
IMessage* IMessage::createStatsIncreaseMessage(uint value)
{
        IMessage *m = new IMessage();
        m->type = MSG_STATS_INCREASE;
        m->value = value;
        return m;
}
IMessage* IMessage::createShowMessageMessage(String message)
{
        IMessage *m = new IMessage();
        m->type = MSG_SHOW_MESSAGE;
        m->msg = message;
        return m;
}
IMessage* IMessage::createDestroyMeMessage(GameObject *s)
{
        IMessage *m = new IMessage();
        m->type = MSG_DESTROY_ME;
        m->sender = s;
        return m;
}
IMessage* IMessage::createGameoverMessage()
{
        IMessage *m = new IMessage();
        m->type = MSG_GAMEOVER;
        return m;
}
IMessage* IMessage::createBroadcastMessage(IMessage* message)
{
        IMessage *m = new IMessage();
        m->type = MSG_BROADCAST;
        m->broadcast = message;
        return m;
}
IMessage* IMessage::createAddMessage(GameObject * sp)
{
        IMessage *m = new IMessage();
        m->type = MSG_ADD;
        m->super = sp;
        return m;
}
IMessage* IMessage::createAddMeMessage(GameObject * s, bool messageList, bool renderList, bool collisionList)
{
        IMessage *m = new IMessage();
        m->type = MSG_ADD_ME | (messageList ? MSG_ML:0) | (renderList ? MSG_RL:0) | (collisionList ? MSG_CL:0);
        m->sender = s;
        return m;
}
IMessage* IMessage::createHideMeMessage(GameObject * s, bool messageList, bool renderList, bool collisionList)
{
        IMessage *m = new IMessage();
        m->type = MSG_HIDE_ME | (messageList ? MSG_ML:0) | (renderList ? MSG_RL:0) | (collisionList ? MSG_CL:0);
        m->sender = s;
        return m;
}

IMessage* IMessage::createCollisionMessage(GameObject * sp, int l, int t)
{
        IMessage *m = new IMessage();
        m->type = MSG_COLLISION;
        m->super = sp;
        m->left = l;
        m->top = t;
        return m;
}
IMessage* IMessage::createDieMessage()
{
        IMessage *m = new IMessage();
        m->type = MSG_DIE;
        return m;
}
//---------------------------------------------------------------------------
GameManager::GameManager()
{
        renderManager = 0;
}
GameManager::~GameManager()
{
        ;
}
void GameManager::setRenderManager(RenderManager * rm)
{
        renderManager = rm;
}
void GameManager::sendMessage(IMessage * m)
{
        messageQueue.push(m);
}
void GameManager::testCollisions()
{
        for (std::list<GameObject*>::iterator i=collisionList.begin(); i!=collisionList.end(); i++)
                for (std::list<GameObject*>::iterator j=i; j!=collisionList.end(); j++)
                {
                        if (i==j)
                                continue;
                        if ((*i)->sprite->isSpriteCollision((*j)->sprite))
                        {
                                (*i)->processMessage(IMessage::createCollisionMessage(*j, (*i)->sprite->getRect().left,(*i)->sprite->getRect().top));
                                (*j)->processMessage(IMessage::createCollisionMessage(*i,(*j)->sprite->getRect().left,(*j)->sprite->getRect().top));
                        }
                }
}
void GameManager::update()
{
        keyUpdater.update();
        for (std::list<GameObject*>::iterator i=listenersList.begin(); i!=listenersList.end(); i++)
                (*i)->update(::GetTickCount());

}
void GameManager::removeObject(GameObject * obj)
{
        listenersList.remove(obj);
        collisionList.remove(obj);
        keyUpdater.removeListener(obj);
        renderManager->remove(obj->sprite);
}

void GameManager::processMessages()
{
        while(!messageQueue.empty())
        {
                IMessage * msg = messageQueue.front();
                UserStats * us;

                TRect senderRect;
                bool isCollision=false;

                switch (msg->type)
                {
                case MSG_MOVE_REQUEST:
                        senderRect = convertRect(msg->sender->sprite->getRect());
                        senderRect.left = msg->left;
                        senderRect.top = msg->top;
                        isCollision=false;
                        for (std::list<GameObject*>::iterator i=collisionList.begin(); i!=collisionList.end(); i++)
                                if ((*i)!=msg->sender && rectCollision(convertRect((*i)->sprite->getRect()), senderRect))
                                {
                                        msg->sender->processMessage(IMessage::createCollisionMessage(*i,msg->left,msg->top));
                                        isCollision = true;
                                        break;
                                }
                        if (!isCollision)
                        {
                                if ((senderRect.top+senderRect.bottom >= 530))
                                {
                                        SilentObject boundWall;
                                        boundWall.type = TYPE_BOUND_WALL;
                                        msg->sender->processMessage(IMessage::createCollisionMessage(&boundWall,msg->left,msg->top));
                                }
                                else
                                        msg->sender->processMessage(IMessage::createMoveRequestMessage(0,msg->left,msg->top));
                        }
                        break;
                case MSG_DESTROY_ME:
                        listenersList.remove(msg->sender);
                        renderManager->remove(msg->sender->sprite);
                        keyUpdater.removeListener(msg->sender);
                        collisionList.remove(msg->sender);
                        delete msg->sender;
                        break;
                case MSG_BROADCAST:
                        for (std::list<GameObject*>::iterator i=listenersList.begin(); i!=listenersList.end(); i++)
                                (*i)->processMessage(msg->broadcast);
                        break;
                case MSG_ADD:
                        sendMessage(msg->super->getSelfAddMessage());
                        break;
                case MSG_ADD_ME_ALL:
                case MSG_ADD_ME | MSG_ML:
                case MSG_ADD_ME | MSG_RL:
                case MSG_ADD_ME | MSG_CL:
                case MSG_ADD_ME | MSG_ML | MSG_RL:
                case MSG_ADD_ME | MSG_ML | MSG_CL:
                case MSG_ADD_ME | MSG_RL | MSG_CL:
                        if (msg->type & MSG_ML)
                        {
                                listenersList.push_back(msg->sender);
                                keyUpdater.addListener(msg->sender);
                        }
                        if (msg->type & MSG_RL)
                                renderManager->add(msg->sender->sprite);
                        if (msg->type & MSG_CL)
                                collisionList.push_back(msg->sender);
                        break;
                case MSG_HIDE_ME_ALL:
                case MSG_HIDE_ME | MSG_ML:
                case MSG_HIDE_ME | MSG_RL:
                case MSG_HIDE_ME | MSG_CL:
                case MSG_HIDE_ME | MSG_ML | MSG_RL:
                case MSG_HIDE_ME | MSG_ML | MSG_CL:
                case MSG_HIDE_ME | MSG_RL | MSG_CL:
                        if (msg->type & MSG_ML)
                        {
                                listenersList.remove(msg->sender);
                                keyUpdater.removeListener(msg->sender);
                        }
                        if (msg->type & MSG_RL)
                                renderManager->remove(msg->sender->sprite);
                        if (msg->type & MSG_CL)
                                collisionList.remove(msg->sender);
                        break;
                case MSG_STATS_INCREASE:
                        us = UserStats::getInstance();
                        us->scores+=msg->value;
                        break;
                case MSG_GAMEOVER:
                        us = UserStats::getInstance();
                        us->gameOver=true;
                        break;
                }
                messageQueue.pop();
                delete msg;
        }
}
void GameManager::render()
{
        renderManager->render();
}

void GameManager::launchGameCycle(int mask)
{
        if (mask & GM_OP_PROCESS_COLLISION)
                testCollisions();
        if (mask & GM_OP_UPDATE)
                update();
        if (mask & GM_OP_PROCESS_MESSAGES)
                processMessages();
        if (mask & GM_OP_RENDER)
                render();
}
void GameManager::clear()
{
        while(!messageQueue.empty()) messageQueue.pop();
        listenersList.clear();
        collisionList.clear();
        keyUpdater.clear();
}
void GameManager::generateMap(GameFieldDesc * gfd, uint w, uint h)
{
        gfd->width=w;
        gfd->height=h;
        list_foreach(GameObject*,it,collisionList)
        {
                gfd->addObject((*it)->sprite->getRect().left/gfd->tileSize,
                               (*it)->sprite->getRect().top/gfd->tileSize,
                               (*it)->type);
        }
}
std::list<GameObject*> GameManager::objectsInRect(TRect r, int type)
{
        std::list<GameObject*> res;
        list_foreach(GameObject*,it,listenersList)
        {
                if (cRectCollision(r,(*it)->sprite->getRect(),true) &&
                    ((type == (*it)->type) || (type == 0)))
                        res.push_back((*it));
        }
        return res;
}
std::list<GameObject*> GameManager::objectsInRange(TRect r, int range, int type)
{
        std::list<GameObject*> res;
        list_foreach(GameObject*,it,listenersList)
        {
                if ((rectRange(r,(*it)->sprite->getRect())<=range) &&
                    ((type == (*it)->type) || (type == 0)))
                        res.push_back((*it));
        }
        return res;
}
//--------------------------------------------------------------------------
bool operator==(const TypePoint& p1, const TypePoint& p2)
{
        return p1.left==p2.left?(p1.top==p2.top?(p1.type==p2.type?1:0):0):0;
}

bool operator==(const AstarPoint& p1, const AstarPoint& p2)
{
        return p1.p==p2.p;
}

std::list<TypePoint> AstarPoint::reconstructPath()
{
        std::list<TypePoint> path;
        AstarPoint *p = this;
        path.push_back(this->p);
        while(p->parent)
        {
                path.push_back(p->parent->p);
                p = p->parent;
        }
        return path;
}
GameFieldDesc::GameFieldDesc()
{
        width = 0;
        height = 0;
        map = 0;
}
GameFieldDesc::GameFieldDesc(uint w, uint h, uint ts)
{
        if (ts==0)
                ts=defaultTileSize;
        width = w;
        height = h;
        uint wCount = w/ts;
        uint hCount = h/ts;
        map = new mapEntry*[hCount];
        for (uint i=0; i<hCount; i++)
                map[i] = new mapEntry[wCount];
        tileSize = ts;

}
GameFieldDesc::~GameFieldDesc()
{
        if (map)
        {
                for (uint i=0; i<height/tileSize; i++)
                        delete[] map[i];
                delete[] map;
        }
}
void GameFieldDesc::init (uint w, uint h, uint ts)
{
        if (map)
        {
                for (uint i=0;  i<width/tileSize+1; i++)
                        delete[] map[i];
                delete[] map;
        }
        if (ts==0)
                ts=defaultTileSize;
        width = w;
        height = h;
        uint wCount = w/ts;
        uint hCount = h/ts;
        map = new mapEntry*[hCount];
        for (uint i=0; i<hCount; i++)
                map[i] = new mapEntry[wCount];
        tileSize = ts;
}
void GameFieldDesc::addObject(int left, int top, uint id)
{
        if (!map)   return;
        if (top<0)  return;
        if (left<0) return;

        if (top<=int(height/tileSize))
        if (left<=int(width/tileSize))
                map[top][left].add(TypePoint(left,top,id));
}

void GameFieldDesc::clear()
{
        if (!map)
                for(uint i=0; i<height/tileSize; i++)
                for(uint j=0; j<width/tileSize; j++)
                        map[i][j].clear();
}
std::list<TypePoint> GameFieldDesc::findObjects(const TRect& area, uint id, bool wholeMap, int bitCheck)
{
        std::list<TypePoint> objects;
        TRect normalizedArea;
        normalizedArea.left = wholeMap?0:area.left/tileSize;
        normalizedArea.top = wholeMap?0:area.top/tileSize;
        normalizedArea.right = wholeMap?width/tileSize:area.right/tileSize;
        normalizedArea.bottom = wholeMap?height/tileSize:area.bottom/tileSize;

        for (int i=max(min(height/tileSize, uint(normalizedArea.top)),uint(0)); i<min(height/tileSize, uint(normalizedArea.bottom)); i++)
        for (int j=max(min(width/tileSize, uint(normalizedArea.left)),uint(0)); j<min(width/tileSize, uint(normalizedArea.right)); j++)
        for (int k=0; k<map[i][j].count; k++)
        {
                if (bitCheck==1)
                        if (map[i][j].objects[k].type&id)
                                objects.push_back(map[i][j].objects[k]);
                        else continue;
                else if (bitCheck==2)
                        if (map[i][j].objects[k].type==id)
                                objects.push_back(map[i][j].objects[k]);
                        else continue;
                else if (bitCheck==0)
                        objects.push_back(map[i][j].objects[k]);
        }
        return objects;
}


double GameFieldDesc::h(const TypePoint& current, const TypePoint& finish)
{
        int cL = current.left;
        int cT = current.top;
        int fL = finish.left;
        int fT = finish.top;
        return abs(cL-fL)+abs(cT-fT);
}

std::list<TypePoint> GameFieldDesc::findWay(const TRect& started, uint destLeft, uint destTop)
{
        std::list<AstarPoint*> closedPoints;
        std::list<AstarPoint*> openedPoints;
        std::list<TypePoint>  pathPoints;

        AstarPoint start;
        start.p.left=started.left/tileSize;
        start.p.top=started.top/tileSize;
        start.p.type=0;

        AstarPoint finish;
        finish.p.left = destLeft/tileSize;
        finish.p.top = destTop/tileSize;
        finish.p.type = 0;

        start.g = 0;
        start.h = h(start.p,finish.p);
        start.f = start.g+start.h;

        openedPoints.push_back(&start);
        while(!openedPoints.empty())
        {
                AstarPoint * t = new AstarPoint();
                t->f+=1;
                for (std::list<AstarPoint*>::iterator it=openedPoints.begin(); it!=openedPoints.end(); it++)
                        if ((*it)->f<=t->f)
                        {
                                if (t->isDefault())
                                        delete t;
                                t = (*it);
                        }

                openedPoints.remove(t);
                closedPoints.push_back(t);
                std::list<AstarPoint*> nearPoints;
                if (t->p.left)
                if (!map[t->p.top][t->p.left-1].containsObjectType(TYPE_WALL))
                {
                        AstarPoint * tp=new AstarPoint;
                        tp->p.left = t->p.left-1;
                        tp->p.top  = t->p.top;
                        nearPoints.push_back(tp);
                }
                if (t->p.left<width/tileSize-1)
                if (!map[t->p.top][t->p.left+1].containsObjectType(TYPE_WALL))
                {
                        AstarPoint * tp=new AstarPoint;
                        tp->p.left = t->p.left+1;
                        tp->p.top  = t->p.top;
                        nearPoints.push_back(tp);
                }
                if (t->p.top!=0)
                if (!map[t->p.top-1][t->p.left].containsObjectType(TYPE_WALL))
                {
                        AstarPoint * tp=new AstarPoint;
                        tp->p.left = t->p.left;
                        tp->p.top  = t->p.top-1;
                        nearPoints.push_back(tp);
                }
                if (t->p.top<height/tileSize-1)
                if (!map[t->p.top+1][t->p.left].containsObjectType(TYPE_WALL))
                {
                        AstarPoint * tp=new AstarPoint;
                        tp->p.left = t->p.left;
                        tp->p.top  = t->p.top+1;
                        nearPoints.push_back(tp);
                }

                for (std::list<AstarPoint*>::iterator it = nearPoints.begin(); it!=nearPoints.end(); it++)
                {
                        bool inClosedPoints=false;
                        for(std::list<AstarPoint*>::iterator cl = closedPoints.begin(); cl!=closedPoints.end(); cl++)
                                if ((*(*it)) == (*(*cl)))
                                {
                                        inClosedPoints=true;
                                        delete *it;
                                        break;
                                }
                        if (inClosedPoints)
                                continue;

                        double bestG = t->g + 1;
                        bool isBetter = false;
                        bool isOpened=false;
                        for (std::list<AstarPoint*>::iterator op = openedPoints.begin(); op!=openedPoints.end(); op++)
                                if ((*(*it)) == (*(*op)))
                                {
                                        isOpened = true;
                                        delete *it;
                                        (*it) = (*op);
                                        break;
                                }
                        if (!isOpened)
                        {
                                openedPoints.push_back(*it);
                                isBetter = true;
                        }
                        else
                        {
                                if (bestG < (*it)->g)
                                        isBetter = true;
                                else
                                        isBetter = false;
                        }
                        if (isBetter)
                        {
                                (*it)->parent = t;
                                (*it)->g = bestG;
                                (*it)->h = h(finish.p,(*it)->p);
                                (*it)->f = (*it)->g + (*it)->h;
                        }
                        if ((*it)->p==finish.p)
                        {
                                pathPoints = t->reconstructPath();
                                for(std::list<AstarPoint*>::iterator d = closedPoints.begin(); d!=closedPoints.end(); d++)
                                        delete *d;
                                for(std::list<AstarPoint*>::iterator d = openedPoints.begin(); d!=openedPoints.end(); d++)
                                        delete *d;
                                return pathPoints;
                        }
                }

        }
        std::list<TypePoint> noWay;
        for(std::list<AstarPoint*>::iterator d = closedPoints.begin(); d!=closedPoints.end(); d++)
                delete *d;                   
        for(std::list<AstarPoint*>::iterator d = openedPoints.begin(); d!=openedPoints.end(); d++)
                delete *d;
        return noWay;
}

//---------------------------------------------------------------------------

#pragma package(smart_init)
