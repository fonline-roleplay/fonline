#ifndef FONLINE_LOOK_DATA
#define FONLINE_LOOK_DATA

#ifndef ANGELSCRIPT_H 
// Avoid having to inform include path if header is already include before
#include "AngelScript/angelscript.h"
#endif

#ifdef FONLINE_SERVER
class Critter;
class Map;
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

#ifdef FONLINE_SERVER
    void InitLook(Critter& critter);
    void InitHide(Critter& critter);
    void InitCritter(Critter& critter);
    void InitMap(Map& map);
#endif

    LookData GetMixed(LookData& other);


    void AddRef() const;
    void Release() const;

    static LookData* Create();

    static void ScriptRegistration(asIScriptEngine* engine);


    static unsigned char GetWallMaterialHearMultiplier(unsigned index);
    static void SetWallMaterialHearMultiplier(unsigned index, unsigned char value);
};
#endif // !FONLINE_LOOK_DATA

