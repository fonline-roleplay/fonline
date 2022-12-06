#include "LookData.h"
#include "Common.h"
#include "Defines.h"

#ifdef FONLINE_SERVER
#include "Critter.h"
#include "Map.h"
#include "Access.h"
#include "MapManager.h"
#include "ItemManager.h"
#endif

static asALLOCFUNC_t userAlloc = asAllocMem;
static asFREEFUNC_t  userFree = asFreeMem;

unsigned char LookData::WallMaterialHearMultiplier[MATERIALS_COUNT];

LookData LookData::ScriptLookData0, LookData::ScriptLookData1, LookData::ItemLookData, LookData::ItemLightLookData;

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

void LookData::InitCritter( const Critter& critter)
{
    dir = critter.GetDir();
    hexx = critter.GetHexX();
    hexy = critter.GetHexY();
    isplayer = critter.IsPlayer();

    if (isplayer)
        access = ((Client*)&critter)->Access;
    else access = 0;

    isruning = critter.IsRuning;
}

void LookData::InitMap( const Map& map)
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

void LookData::InitItem( const Item& item )
{
    if( item.Accessory == ITEM_ACCESSORY_HEX )
    {
        dir = 0; // critter.GetDir( );
        hexx = item.AccHex.HexX;
        hexy = item.AccHex.HexY;
        isplayer = false;
        access = 0;
        isruning = false;
    }
}
#endif

void LookData::GetMixed( const LookData& other, LookData& outData )
{
    outData = *this;

    outData.ViewDirMultiplier[0] *= (uint)(0.01 * other.ViewDirMultiplier[0]);
    outData.ViewDirMultiplier[1] *= (uint)(0.01 * other.ViewDirMultiplier[1]);
    outData.ViewDirMultiplier[2] *= (uint)(0.01 * other.ViewDirMultiplier[2]);
    outData.ViewDirMultiplier[3] *= (uint)(0.01 * other.ViewDirMultiplier[3]);

    outData.HearDirMultiplier[0] *= (uint)(0.01 * other.HearDirMultiplier[0]);
    outData.HearDirMultiplier[1] *= (uint)(0.01 * other.HearDirMultiplier[1]);
    outData.HearDirMultiplier[2] *= (uint)(0.01 * other.HearDirMultiplier[2]);
    outData.HearDirMultiplier[3] *= (uint)(0.01 * other.HearDirMultiplier[3]);

    outData.RunningNoiseMultiplier *= (uint)(0.01 * other.RunningNoiseMultiplier);
    outData.RunningHearMultiplier *= (uint)(0.01 * other.RunningHearMultiplier);

    outData.MaxView += other.MaxView;
    outData.MaxHear += other.MaxHear;
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

LookData& LookData::operator=( const LookData& other )
{
    memcpy( this, &other, OFFSETOF( LookData, dir ) );
    return *this;
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

#ifdef FONLINE_SERVER
LookData::Result LookData::CheckLook( Map& map, LookData& look, LookData& hide )
{
    /*
    if( true )
    {
        LookData::Result result = { false };
        result.IsView = true;
        result.IsHear = true;
        return result;
    }*/
    LookData::Result result = { false };
    result.IsView = true;
    result.IsHear = true;

    if( look.access >= ACCESS_MODER && look.Vision > 0 )
        return result;

    uint dist = DistGame( look.hexx, look.hexy, hide.hexx, hide.hexy );
    if( look.Vision >= dist && hide.Invis <= dist )
        return result;

    if( hide.Invis > 0 && hide.Invis <= dist )
    {
        result.IsView = false;
        result.IsHear = false;
        return result;
    }

    if( hide.Invis > dist || look.Vision >= dist )
        return result;

    uint max_view = look.MaxView;
    uint max_hear = look.MaxHear;

    uchar start_dir = GetFarDir( look.hexx, look.hexy, hide.hexx, hide.hexy );
    uchar look_dir = ( look.dir > start_dir ? look.dir - start_dir : start_dir - look.dir );

    if( look_dir > 3 )
        look_dir = 6 - look_dir;

    double hear_mul = ( 0.01 * look.HearDirMultiplier[ look_dir ] ) * ( 0.01 * hide.HideHearMultiplier ) * ( 0.01 * hide.HideHearDirMultiplier[ start_dir ] ),
        view_mul = ( 0.01 * look.ViewDirMultiplier[ look_dir ] ) * ( 0.01 * hide.HideViewMultiplier ) * ( 0.01 * hide.HideViewDirMultiplier[ start_dir ] );

    if( hide.isruning )
        hear_mul *= 0.01 * hide.RunningNoiseMultiplier;

    if( look.isruning )
        hear_mul *= 0.01 * look.RunningHearMultiplier;

    max_view = ( uint )( max_view * view_mul );
    uint tmp_max_hear = ( uint )( max_hear * hear_mul );

    // new optimization: return early if distance larger than max_view and max_hear
    if( dist > max_view && dist > tmp_max_hear )
    {
        result.IsView = false;
        result.IsHear = false;
        return result;
    }

    static TraceData trace;
    static UShortPair block;
    //if( !trace.Walls ) trace.Walls = new SceneryClRefVec( );
    // if( !trace.Items ) trace.Items = new ItemPtrVec( );
    //if( !trace.Block ) trace.Block = &block;

    block.first = 0;
    block.second = 0;
    trace.ForceFullTrace = true;
    trace.TraceMap = &map;
    trace.BeginHx = look.hexx;
    trace.BeginHy = look.hexy;
    trace.EndHx = hide.hexx;
    trace.EndHy = hide.hexy;
    trace.Dist = dist;
    MapMngr.TraceBullet( trace ); 

    if( trace.Walls )
    {
        ProtoItem* protoItem = nullptr;
        for( auto it = trace.Walls->begin( ), end = trace.Walls->end( ); it != end; ++it )
        {
            protoItem = ItemMngr.GetProtoItem( ( *it )->ProtoId );
            if( protoItem && !protoItem->IsPassed( ) )
            {
                uint disttoitem = DistGame( look.hexx, look.hexy, ( *it )->MapX, ( *it )->MapY );
                if( disttoitem < max_hear )
                {
                    max_hear = disttoitem + ( uint )( ( max_hear - disttoitem ) * LookData::WallMaterialHearMultiplier[ protoItem->Material ] * 0.01 );
                }
            }
        }
        trace.Walls->clear( );
    }
    
    if( trace.Items )
    {
        Item* traceItem = nullptr;
        bool isHex = false;
        for( auto it = trace.Items->begin( ), end = trace.Items->end( ); it != end; ++it )
        {
            traceItem = *it;
            isHex = ( look.hexx == traceItem->AccHex.HexX && look.hexy == traceItem->AccHex.HexY );

            if( traceItem->IsViewBlocks( ) )
            {
                if( isHex )
                    view_mul *= 0.01 * traceItem->Proto->FORPData.Look_Block;
                else
                {
                    uint disttoitem = DistGame( look.hexx, look.hexy, traceItem->AccHex.HexX, traceItem->AccHex.HexY );
                    if( disttoitem >= max_view )
                        break;

                    uint distchange = max_view - disttoitem;
                    max_view = disttoitem + ( uint )( distchange * traceItem->Proto->FORPData.Look_BlockDir[ GetFarDir( look.hexx, look.hexy, traceItem->AccHex.HexX, traceItem->AccHex.HexY ) ] * 0.01 );
                }
            }

            if( traceItem->IsHearBlocks( ) )
            {
                if( isHex )
                    hear_mul *= 0.01 * traceItem->Proto->FORPData.Hear_Block;
                else
                {
                    uint disttoitem = DistGame( look.hexx, look.hexy, traceItem->AccHex.HexX, traceItem->AccHex.HexY );
                    if( disttoitem >= max_hear )
                        break;

                    uint distchange = max_hear - disttoitem;
                    max_hear = disttoitem + ( uint )( distchange * traceItem->Proto->FORPData.Hear_BlockDir[ GetFarDir( look.hexx, look.hexy, traceItem->AccHex.HexX, traceItem->AccHex.HexY ) ] * 0.01 );
                }
            }
        }
        trace.Items->clear( );
    }
    
    if( dist > max_view  || ( !trace.IsFullTrace && ( !trace.Block || ( block.first != hide.hexx && block.second != hide.hexy ) ) ) )
        result.IsView = false;

    if( dist > ( uint )( max_hear * hear_mul ) )
        result.IsHear = false;

    return result;
}
#endif
