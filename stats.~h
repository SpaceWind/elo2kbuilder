//---------------------------------------------------------------------------

#ifndef statsH
#define statsH
#include <vcl.h>
#include <Controls.hpp>
#include <StdCtrls.hpp> 
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include "utils.h"
#include <map.h>



class UserStats
{
public:
        static UserStats* getInstance();
        void print(TLabel *livesCountLabel,  TLabel * scoresLabel, TLabel * lvlNameLabel, TLabel * gameOverLabel, TProgressBar * pb, TLabel* currenthp, TMemo* memo);

        void setGlobal(String key, String val);
        String getGlobal(String key);
        bool processString(String key, bool scene=false);
        String translateString(String s);

        void loadFromFile(String filename);
        void saveToFile(String filename);


        void damage(int i);
        int livesCount, scores;
        int enemyTotalCount;
        int maxhp, hp;
        bool gameOver;
        TStringList * system;
        String levelName;


private:
        UserStats();
        ~UserStats();

        uint lasttimedmg;
        std::map<String,String> globals;
};


struct UserStatsStatic
{
        friend UserStats;
private:
        UserStats * instance;
        bool init;
}usStatic;

//---------------------------------------------------------------------------
#endif
