#include "LookData.h"
#include "Common.h"
#include "Defines.h"

#ifdef FONLINE_SERVER
#include "Critter.h"
#include "Map.h"
#endif

static asALLOCFUNC_t userAlloc = asAllocMem;
static asFREEFUNC_t  userFree = asFreeMem;

int QST_VISION = 702;
int QST_INVIS = 701;

unsigned char LookData::WallMaterialHearMultiplier[MATERIALS_COUNT];

LookData::LookData() : refcounter(1)
{
    ViewDirMultiplier[0] = 80;
    ViewDirMultiplier[1] = 100;
    ViewDirMultiplier[2] = 80;
    ViewDirMultiplier[3] = 80;

    HearDirMultiplier[0] = 100;
    HearDirMultiplier[1] = 80;
    HearDirMultiplier[2] = 50;
    HearDirMultiplier[3] = 40;

    RunningNoiseMultiplier = 100;
    RunningHearMultiplier = 100;

    MaxView = 0;
    MaxHear = 0;

    Vision = 0;
    Invis = 0;

    HideViewDirMultiplier[0] = 100;
    HideViewDirMultiplier[1] = 100;
    HideViewDirMultiplier[2] = 100;
    HideViewDirMultiplier[3] = 100;
    HideViewDirMultiplier[4] = 100;
    HideViewDirMultiplier[5] = 100;

    HideHearDirMultiplier[0] = 100;
    HideHearDirMultiplier[1] = 100;
    HideHearDirMultiplier[2] = 100;
    HideHearDirMultiplier[3] = 100;
    HideHearDirMultiplier[4] = 100;
    HideHearDirMultiplier[5] = 100;

    HideViewMultiplier = 100;
    HideHearMultiplier = 100;
}

#ifdef FONLINE_SERVER
void LookData::InitLook(Critter& critter)
{
    int perception = critter.GetParam(ST_PERCEPTION);
    MaxView = 10 + perception * 5;
    MaxHear = 5 + perception * 2;

    Vision = critter.GetParam(QST_VISION);
}

void LookData::InitHide(Critter& critter)
{
    Invis = critter.GetParam(QST_INVIS);
}

void LookData::InitCritter(Critter& critter)
{
    dir = critter.GetDir();
    hexx = critter.GetHexX();
    hexy = critter.GetHexY();
    isplayer = critter.IsPlayer();

    if (isplayer)
        access = ((Client*)&critter)->Access;
    else access = 0;

    isruning = critter.IsRuning;

    //InitLook(critter);
    //InitHide(critter);
}

void LookData::InitMap(Map& map)
{
    ViewDirMultiplier[0] = 100;
    ViewDirMultiplier[1] = 100;
    ViewDirMultiplier[2] = 100;
    ViewDirMultiplier[3] = 100;

    HearDirMultiplier[0] = 100;
    HearDirMultiplier[1] = 100;
    HearDirMultiplier[2] = 100;
    HearDirMultiplier[3] = 100;

    RunningNoiseMultiplier = 100;
    RunningHearMultiplier = 100;
}
#endif

LookData LookData::GetMixed(LookData& other)
{
    LookData result = *this;

    result.ViewDirMultiplier[0] *= (uint) (0.01 * other.ViewDirMultiplier[0]);
    result.ViewDirMultiplier[1] *= (uint)(0.01 * other.ViewDirMultiplier[1]);
    result.ViewDirMultiplier[2] *= (uint)(0.01 * other.ViewDirMultiplier[2]);
    result.ViewDirMultiplier[3] *= (uint)(0.01 * other.ViewDirMultiplier[3]);

    result.HearDirMultiplier[0] *= (uint)(0.01 * other.HearDirMultiplier[0]);
    result.HearDirMultiplier[1] *= (uint)(0.01 * other.HearDirMultiplier[1]);
    result.HearDirMultiplier[2] *= (uint)(0.01 * other.HearDirMultiplier[2]);
    result.HearDirMultiplier[3] *= (uint)(0.01 * other.HearDirMultiplier[3]);

    result.RunningNoiseMultiplier *= (uint)(0.01 * other.RunningNoiseMultiplier);
    result.RunningHearMultiplier *= (uint)(0.01 * other.RunningHearMultiplier);

    result.MaxView += other.MaxView;
    result.MaxHear += other.MaxHear;

    return result;
}

void LookData::AddRef() const
{
    //asAtomicInc(refcounter);
}

void LookData::Release() const
{
    /*if (asAtomicDec(refcounter) == 0)
    {
        this->~LookData();
        userFree(const_cast<LookData*>(this));
    }*/
}

unsigned char LookData::GetWallMaterialHearMultiplier(unsigned index)
{
    if (index >= MATERIALS_COUNT)
        return 0;

    return WallMaterialHearMultiplier[index];
}

void LookData::SetWallMaterialHearMultiplier(unsigned index, unsigned char value)
{
    if (index >= MATERIALS_COUNT)
        return;

    WallMaterialHearMultiplier[index] = value;
}

LookData* LookData::Create()
{
    void* mem = userAlloc(sizeof(LookData));
    if (mem == 0)
    {
        asIScriptContext* ctx = asGetActiveContext();
        if (ctx)
            ctx->SetException("Out of memory");

        return 0;
    }

    LookData* result = new(mem) LookData();
    return result;
}

void LookData::ScriptRegistration( asIScriptEngine* engine )
{

}