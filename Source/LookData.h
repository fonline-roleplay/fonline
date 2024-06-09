#ifndef FONLINE_LOOK_DATA
#define FONLINE_LOOK_DATA

#ifndef ANGELSCRIPT_H 
// Avoid having to inform include path if header is already include before
#include "AngelScript/angelscript.h"
#endif

#ifdef FONLINE_SERVER
class Critter;
class Map;
class Item;
#endif

#define MATERIALS_COUNT (50)

class LookData
{
public:
    unsigned char ViewDirMultiplier[4]; // 4
    unsigned char HearDirMultiplier[4]; // +4 = 8

    unsigned short RunningNoiseMultiplier; // +2 = 10
    unsigned char RunningHearMultiplier;    // +1 = 11

    char MaxView; // +1 = 12
    char MaxHear; // +1 = 13

    unsigned char Vision; // +1=14
    unsigned char Invis; // +1=15

    unsigned char HideViewDirMultiplier[6]; // +6=21
    unsigned char HideHearDirMultiplier[6]; // +6=27

    unsigned char HideViewMultiplier; // +1=28
    unsigned char HideHearMultiplier; // +1=29

    static unsigned char WallMaterialHearMultiplier[MATERIALS_COUNT]; // * 0.1

    // WorkData
    unsigned char dir;
    unsigned short hexx;
    unsigned short hexy;

    bool isplayer;
    bool isruning;

    unsigned char access;

    //script
    mutable int refcounter;
    LookData();

    LookData& operator=( const LookData& other );

    struct Result
    {
        union
        {
            bool IsLook;
            struct
            {
                bool IsView : 1;
                bool IsHear : 1;
            };
        };
    };

    static LookData ScriptLookData0, ScriptLookData1, ItemLookData, ItemLightLookData;

#ifdef FONLINE_SERVER
    void InitCritter( const Critter& critter);
    void InitMap( const Map& map);
    void InitItem( const Item& item );
    static Result CheckLook( Map& map, LookData& look, LookData& hide, bool isDebug = false );
#endif

    void GetMixed( const LookData& other, LookData& outData );


    void AddRef() const;
    void Release() const;

    static LookData* Create();

    static void ScriptRegistration(asIScriptEngine* engine);


    static unsigned char GetWallMaterialHearMultiplier(unsigned index);
    static void SetWallMaterialHearMultiplier(unsigned index, unsigned char value);

};
#endif // !FONLINE_LOOK_DATA

