//---------------------------------------------------------------------------


#pragma hdrstop

#include "mapManager.h"

MapManager::MapManager(String filename, GameManager * gm)
{
        Defines d;
        d.Load(filename);
        levels = d.GetList("levels");
        manager = gm;
        scene =0; map = 0;
        levelIndex = 0;
        if (!loadNext())
                done = true;
        else
                done = false;
}
MapManager::~MapManager()
{
        if (scene)
                delete scene;
        if (map)
                delete map;
}
bool MapManager::loadNext()
{
        if (!levels)
                return false;
        if (levels->Count <= levelIndex)
                return false;

        while((levels->Strings[levelIndex] == "")||
              (levels->Strings[levelIndex].c_str()[0]=='/'))
        {
                levelIndex++;
                if (levels->Count <= levelIndex)
                        return false;
        }

        TStringList * params = new TStringList();
        if (levelIndex == 0)
        {
                params->DelimitedText = levels->Strings[levelIndex];
                levelIndex++;
        }
        else
        {
                UserStats * us = UserStats::getInstance();
                params->DelimitedText = us->getGlobal("next");
        }
        if (scene)
        {
                delete scene;
                scene = 0;
        }
        if (map)
        {
                delete map;
                map = 0;
        }
        if (params->Strings[0].LowerCase() == "scene")
        {
                scene = new Scene("scenes\\"+params->Strings[1],manager->getRenderManager());
                Defines d;
                d.Load("scenes\\"+params->Strings[1]);
                currentName = d.Get("name");
        }
        else if (params->Strings[0].LowerCase() == "map")
        {
                map = new Map("maps\\"+params->Strings[1],manager);
                Defines d;
                d.Load("maps\\"+params->Strings[1]);
                currentName = d.Get("name");
        }
        return true;
}
void MapManager::update()
{
        if (map)
        {
                if (!map->isDone())
                {
                        map->update();
                        manager->launchGameCycle(GM_OP_DONT_RENDER);
                        map->render();
                }
                else if (!loadNext())
                {
                        done = true;
                        return;
                }
        }
        else if (scene)
        {
                if (!scene->isDone())
                {
                        scene->update();
                        manager->getRenderManager()->render();
                }
                else if (!loadNext())
                {
                        done = true;
                        return;
                }
        }

}

//---------------------------------------------------------------------------

#pragma package(smart_init)

