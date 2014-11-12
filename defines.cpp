//---------------------------------------------------------------------------


#pragma hdrstop

#include "defines.h"
Defines::Defines()
{
        l = new TStringList();
        encode = false;
}
Defines::~Defines()
{
        delete l;
}
Defines::Defines(String filename)
{
        l = new TStringList();
        l->LoadFromFile(filename);
}
void Defines::Load(String filename)
{
        if (encode)
                EncodeFile(filename, filename+".dc", CRYPT_DEFINES_PASS);
        l->LoadFromFile(encode?filename+".dc":filename);
        _filename = filename;
        if (encode)
                DeleteFile(filename+".dc");
}
void Defines::Save(String filename)
{
        l->SaveToFile(filename);
        if (encode)
                EncodeFile(filename, filename, CRYPT_DEFINES_PASS);
}
String Defines::Get(String key, String prefix, String postfix)
{
        int index = -1;
        for (int i=0; i<l->Count; i++)
                if (l->Strings[i]==String("#")+key)
                {
                        index = i+1;
                        break;
                }
        if (index == -1)
                return "";
        return prefix+l->Strings[index]+postfix;
}
void Defines::Set(String key, String value)
{
        int index = -1;
        for (int i=0; i<l->Count; i++)
                if (l->Strings[i]==String("#")+key)
                {
                        index = i+1;
                        break;
                }
        if (index==-1)
        {
                l->Add("#"+key);
                l->Add(value);
        }else
                l->Strings[index]=value;
}
bool Defines::IsDefined(String key)
{
        return (Get(key)!=String(""));
}
void Defines::Define(String key)
{
        if (!IsDefined(key))
                Set(key,"!");
}
void Defines::Undef(String key)
{
        Set(key,"");
}


void Defines::SetList(String key, TStringList * data)
{
        int indexS = -1, indexF = -1;
        for (int i=0; i<l->Count; i++)
        {
                switch(indexS)
                {
                case -1:
                        if (l->Strings[i]==String("@")+key)
                                indexS = i;
                        break;
                default:
                        if (l->Strings[i]==String("/")+key)
                        {
                                indexF = i;
                                i = l->Count;
                        }
                        break;
                }
        }

        if (indexS == -1)
        {
                l->Add("@"+key);
                for (int i=0; i<data->Count; i++)
                        l->Add(data->Strings[i]);
                l->Add("/"+key);
        }else
        {
                for (int i=0; i<(indexF-indexS+1); i++)
                        l->Delete(indexS);
                l->Add("@"+key);
                for (int i=0; i<data->Count; i++)
                        l->Add(data->Strings[i]);
                l->Add("/"+key);
        }
}
TStringList * Defines::GetList(String key)
{
        int indexS = -1, indexF = -1;
        for (int i=0; i<l->Count; i++)
        {
                switch(indexS)
                {
                case -1:
                        if (l->Strings[i]==String("@")+key)
                                indexS = i;
                        break;
                default:
                        if (l->Strings[i]==String("/")+key)
                        {
                                indexF = i;
                                i = l->Count;
                        }
                        break;
                }
        }
        if (indexS !=-1 && indexF!=-1)
        {
                TStringList * out = new TStringList();
                for (int i=0; i<(indexF-indexS-1); i++)
                        out->Add(l->Strings[indexS+1+i]);
                return out;
        }
        else
                return 0;
}
void Defines::RemoveList(String key)
{
        int iStart=0;
        int iStop=-1;
        for (int i=0; i<l->Count; i++)
                if (l->Strings[i]=="@"+key)
                        iStart=i;
                else if (l->Strings[i]=="/"+key)
                        iStop=i;
        for (int i=iStart; i<=iStop; i++)
                l->Strings[i]="";
}
bool Defines::AddToList(String key, String data)
{
        TStringList *s = GetList(key);
        if (s==0)
                return 0;
        s->Add(data);
        SetList(key,s);
        delete s;
        return 1;
}
void Defines::RemoveFromList(String keyList, String data)
{
        TStringList *s = GetList(keyList);
        if (s==0)
                return;
        int i=s->IndexOf(data);
        if (i>=0)
                s->Delete(i);
        SetList(keyList,s);
        delete s;
}

//---------------------------------------------------------------------------

#pragma package(smart_init)

