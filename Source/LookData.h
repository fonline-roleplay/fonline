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
    unsigned char ViewDirMultiplier[4]; // * 0.1
    unsigned char HearDirMultiplier[4]; // * 0.1

    unsigned short RunningNoiseMultiplier; // * 0.1
    unsigned char RunningHearMultiplier;    // * 0.1

    char MaxView;
    char MaxHear;

    unsigned char Vision;
    unsigned char Invis;

    unsigned char HideViewDirMultiplier[6]; // * 0.1
    unsigned char HideHearDirMultiplier[6]; // * 0.1

    unsigned char HideViewMultiplier;
    unsigned char HideHearMultiplier;

    static unsigned char WallMaterialHearMultiplier[MATERIALS_COUNT]; // * 0.1

    // WorkData
    unsigned char dir;
    unsigned short hexx;
    unsigned short hexy;

    bool isplayer;
    bool isruning;

    unsigned char access;

    LookData();

#ifdef FONLINE_SERVER
    void InitLook(Critter& critter);
    void InitHide(Critter& critter);
    void InitCritter(Critter& critter);
    void InitMap(Map& map);
#endif

    LookData GetMixed(LookData& other);

    //script
    mutable int refcounter;

    void AddRef() const;
    void Release() const;

    static LookData* Create();

    static void ScriptRegistration(asIScriptEngine* engine);


    static unsigned char GetWallMaterialHearMultiplier(unsigned index);
    static void SetWallMaterialHearMultiplier(unsigned index, unsigned char value);
};
#endif // !FONLINE_LOOK_DATA

