//---------------------------------------------------------------------------


#pragma hdrstop

#include "stats.h"

UserStats::UserStats()
{
        livesCount=scores=enemyTotalCount=gameOver=maxhp=hp=0;
        levelName = "";
        usStatic.init = true;
        usStatic.instance = this;
        lasttimedmg=0;
        system = new TStringList();
}
UserStats::~UserStats()
{
        delete system;
}
UserStats* UserStats::getInstance()
{
        return usStatic.init?usStatic.instance:new UserStats();
}
void UserStats::print(TLabel *livesCountLabel,  TLabel * scoresLabel, TLabel * lvlNameLabel, TLabel * gameOverLabel, TProgressBar * pb, TLabel* currenthp, TMemo* memo)
{
        livesCountLabel->Caption = "Lives Left: "+IntToStr(livesCount);
        scoresLabel->Caption = "Scores: "+IntToStr(scores);
        lvlNameLabel->Caption = "Level: "+levelName;
        if (gameOver)
                gameOverLabel->Visible=true;
        else
                gameOverLabel->Visible=false;
        pb->Max = maxhp;
        pb->Position=hp;
        currenthp->Caption = "HP: "+IntToStr(hp);
        memo->Clear();
        for (int i=0; i<system->Count; i++)
                memo->Lines->Add(system->Strings[i]);

}
void UserStats::damage(int i)
{
        if (::GetTickCount()-lasttimedmg>250)
        {
                hp-=i;
                if (hp<0)
                        hp=0;
                if (hp>maxhp)
                        hp=maxhp;
                lasttimedmg=::GetTickCount();
        }
}
void UserStats::setGlobal(String key, String val)
{
        globals[key] = val;
}
String UserStats::getGlobal(String key)
{
        std::map<String, String>::iterator it = globals.find(key);
        if (it != globals.end())
                return it->second;
        return "";
}
String UserStats::translateString(String s)
{
        map_foreach(String,String,it,globals)
                s = StringReplace(s, "$"+it->first, it->second, TReplaceFlags()<<rfReplaceAll);
        return s;
}
bool UserStats::processString(String s, bool isScene)
{
        s = translateString(s);

        TStringList * list = new TStringList();
        list->DelimitedText = s;
        if (list->Count ==0)
        {
                delete list;
                return false;
        }
        if ((list->Strings[0].LowerCase() == "set" && isScene) ||
            (list->Strings[0].LowerCase() == "mapset" && (!isScene))||
            (list->Strings[0].LowerCase() == "_set" && (!isScene)))
        {
                String value;
                for (int i=2; i<list->Count; i++)
                {
                        value = value + list->Strings[i];
                        if (i!=list->Count-1)
                                value = value + " ";
                }
                setGlobal(list->Strings[1], value);

                delete list;
                return true;
        }
        else if ((list->Strings[0].LowerCase() == "add" && isScene) ||
                 (list->Strings[0].LowerCase() == "mapadd" && (!isScene))||
                 (list->Strings[0].LowerCase() == "_add" && (!isScene)))
        {
                int val, add;
                TryStrToInt(getGlobal(list->Strings[1]), val);
                TryStrToInt(list->Strings[2], add);
                setGlobal(list->Strings[1], IntToStr(val+add));

                delete list;
                return true;
        }
        else if ((list->Strings[0].LowerCase() == "cat" && isScene) ||
                 (list->Strings[0].LowerCase() == "mapcat" && (!isScene))||
                 (list->Strings[0].LowerCase() == "_cat" && (!isScene)))
        {
                for (int i = 2; i<list->Count; i++)
                       setGlobal(list->Strings[1], getGlobal(list->Strings[1])+list->Strings[i]);
                delete list;
                return true;
        }
        else if ((list->Strings[0].LowerCase() == "sub" && isScene) ||
                 (list->Strings[0].LowerCase() == "mapsub" && (!isScene))||
                 (list->Strings[0].LowerCase() == "_sub" && (!isScene)))
        {
                int val, sub;
                TryStrToInt(getGlobal(list->Strings[1]), val);
                TryStrToInt(list->Strings[2], sub);
                setGlobal(list->Strings[1], IntToStr(val-sub));


                delete list;
                return true;
        }
         else if ((list->Strings[0].LowerCase() == "mul" && isScene) ||
                  (list->Strings[0].LowerCase() == "mapmul" && (!isScene))||
                  (list->Strings[0].LowerCase() == "_mul" && (!isScene)))
        {
                int val, mul;
                TryStrToInt(getGlobal(list->Strings[1]), val);
                TryStrToInt(list->Strings[2], mul);
                setGlobal(list->Strings[1], IntToStr(val*mul));


                delete list;
                return true;
        }
         else if ((list->Strings[0].LowerCase() == "div" && isScene) ||
                  (list->Strings[0].LowerCase() == "mapdiv" && (!isScene))||
                  (list->Strings[0].LowerCase() == "_div" && (!isScene)))
        {
                int val, div;
                TryStrToInt(getGlobal(list->Strings[1]), val);
                TryStrToInt(list->Strings[2], div);
                setGlobal(list->Strings[1], IntToStr(val/div));

                delete list;
                return true;
        }


        delete list;
        return false;
}
void UserStats::loadFromFile(String filename)
{
        if (!FileExists(filename))
                return;
        TStringList * list = new TStringList();
        list->LoadFromFile(filename);
        for (int i=0; i<list->Count; i++)
                processString(list->Strings[i]);
        delete list;
}
void UserStats::saveToFile(String filename)
{
        TStringList * list = new TStringList();
        map_foreach(String,String,it,globals)
                list->Add("set "+it->first+" "+it->second);
        list->SaveToFile(filename);
        delete list;
}

//---------------------------------------------------------------------------

#pragma package(smart_init)
