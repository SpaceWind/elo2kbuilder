//---------------------------------------------------------------------------

#ifndef mapManagerH
#define mapManagerH
#include <vcl.h>
#include "map.h"
#include "stats.h"

class MapManager
{
public:
        MapManager(String filename, GameManager * gm);
        ~MapManager();

        bool isDone(){return done;}
        String getCurrentLevelName(){return currentName;}
        void update();
private:
        bool loadNext();
        TStringList * levels;
        Scene * scene;
        Map * map;
        GameManager * manager;

        bool done;
        int levelIndex;
        String currentName;
};
//---------------------------------------------------------------------------
#endif

