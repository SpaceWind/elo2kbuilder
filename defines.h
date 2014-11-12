//---------------------------------------------------------------------------

#ifndef definesH
#define definesH
#include <vcl.h>
#include "utils.h"
#include "filecrypt.h"

#define CRYPT_DEFINES_PASS      "112"



class Defines
{
public:
        Defines();
        Defines(String filename);
        ~Defines();
        String        Get(String key, String prefix="", String postfix="");
        void          Set(String key, String value);

        TStringList * GetList(String key);
        void          SetList(String key, TStringList * data);

        void          RemoveList(String key);
        bool          AddToList(String key, String data);
        void          RemoveFromList(String keyList, String data);

        bool          IsDefined(String key);
        void          Define(String key);
        void          Undef(String key);

        void   Load(String filename);
        void   Save(String filename);
        void   Save(){Save(_filename);}
        String _filename;
        bool encode;
private:
        TStringList * l;
};
//---------------------------------------------------------------------------
#endif

