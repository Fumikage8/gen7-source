//======================================================================
/**
 * @file  DebugWinSystem.cpp
 * @brief 15/02/19
 * @author  ariizumi
 * @data  niji用デバッグウィンドウ周り
 */
//======================================================================
#include <ui/include/gfl2_UI.h>
#include <Debug/include/gfl2_DebugWinItemTypes.h>
#include <system/include/GflUse.h>
#include "sound/include/Sound.h"
#include "sound/include/BGMManager.h"
#include "debug/DebugWin/include/DebugWinSystem.h"
#include "Debug/DebugWin/include/DebugPause.h"
#include "System/include/HeapDefine.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "GameSys/include/NijiTime.h"
#include "print/include/SystemFont.h"
#include "print/include/PrintSystem.h"
#include "System/include/nijiAllocator.h"

//#include "FieldScript/include/EventScriptCall.h"
#include "system/include/Date/gfl2_Date.h"

#include <system/include/Timer/gfl2_PerformanceCounter.h> //max2重定義・・・

#include <Print/include/PrintSystem.h>

#include <Debug/include/DynamicHeader/gfl2_DebugDynamicHeader.h>

#include "System/include/PokemonVersion.h"


#include <arc_index/debug_message.gaix>

// メッセージのインデックス
#include "niji_conv_header/message/debug/msg_debug_menu.h"

//  きせかえデータベース
#include "KisekaeData/include/KisekaeDataBase.h"
#include <niji_conv_header/message/msg_kisekae_color.h>

// 通信デバッグ機能
#include "NetStatic/NetLib/include/Debug/NetDebug.h"

// EventFieldmapControl.h
#include "Field/FieldStatic/source/Event/EventFieldmapControl.h"

#include "ExtSavedata/include/NijiExtSaveSystem.h"
#include "ExtSavedata/include/NijiExtSaveData.h"

// PMSoundSystem
#include  "Sound/include/PMSoundSystem.h"


//Heap名称定義
#define HEAP_DEFINE(str) L ## #str,
static const wchar_t* const heapNameArr[HEAPID_CHILD_MAX] =
{
  L"HEAPID_SYSTEM",
  #include "System/include/HeapDefine.cdat"

};


#if USE_DEBUG_WIN
/** ---------------------------------------------------------------------
//! @brief  SINGLETONの前方参照宣言
// ---------------------------------------------------------------------*/
SINGLETON_INSTANCE_DEFINITION( Debug::DebugWin::DebugWinWork )
template class gfl2::base::SingletonAccessor<Debug::DebugWin::DebugWinWork>;

#endif


GFL_NAMESPACE_BEGIN(Debug)
GFL_NAMESPACE_BEGIN(DebugWin)

struct KEY_SET_WORK
{
  int mSeq;
  int *mKeyBit;
  bool mKeepMode;
};
struct DEBWIN_GROUP_WORK
{
  KEY_SET_WORK mTrgWork;
  KEY_SET_WORK mHoldWork;
};

extern void KujiraDebugWin_UpdateKeySet( void* userWork  , gfl2::debug::DebugWinItem *item );
extern wchar_t* KujiraDebgWin_DrawKeySet( void* userWork  , gfl2::debug::DebugWinItem *item );




class DebugWinWork;

extern wchar_t* DebugWin_DrawRomCreateDateTime( void* userWork  , gfl2::debug::DebugWinItem *item );

extern void DebugWin_UpdateTickBar( void* userWork  , gfl2::debug::DebugWinItem *item );
extern wchar_t* DebugWin_DrawTickBar( void* userWork  , gfl2::debug::DebugWinItem *item );
extern void DebugWin_UpdateTick( void* userWork  , gfl2::debug::DebugWinItem *item );
extern wchar_t* DebugWin_DrawTick( void* userWork  , gfl2::debug::DebugWinItem *item );
extern void DebugWin_ResetTick( void* userWork  , gfl2::debug::DebugWinItem *item );
extern void DebugWin_UpdateFs( void* userWork  , gfl2::debug::DebugWinItem *item );
extern wchar_t* DebugWin_DrawFs( void* userWork  , gfl2::debug::DebugWinItem *item );
extern void DebugWin_UpdateFsProg( void* userWork  , gfl2::debug::DebugWinItem *item );
extern wchar_t* DebugWin_DrawFsProg( void* userWork  , gfl2::debug::DebugWinItem *item );
extern void DebugWin_UpdateLyt( void* userWork  , gfl2::debug::DebugWinItem *item );
extern wchar_t* DebugWin_DrawLyt( void* userWork  , gfl2::debug::DebugWinItem *item );
extern wchar_t* DebugWin_DrawComBuf( void* userWork  , gfl2::debug::DebugWinItem *item );
extern wchar_t* DebugWin_DrawComReq( void* userWork  , gfl2::debug::DebugWinItem *item );

extern void DebugWin_UpdateSoundOutputMode( void* userWork  , gfl2::debug::DebugWinItem *item );
extern wchar_t* DebugWin_DrawBGMSwitch( void* userWork  , gfl2::debug::DebugWinItem *item );
extern void DebugWin_UpdateBGMSwitch( void* userWork  , gfl2::debug::DebugWinItem *item );
extern wchar_t* DebugWin_DrawBSoundOutputMode( void* userWork  , gfl2::debug::DebugWinItem *item );

extern void DebugWin_UpdateGflUsePublicRandDebug( void* userWork  , gfl2::debug::DebugWinItem *item );
extern wchar_t* DebugWin_DrawGflUsePublicRandDebug( void* userWork  , gfl2::debug::DebugWinItem *item );

extern void DebugWin_UpdateHeapClearSwitch( void* userWork  , gfl2::debug::DebugWinItem *item );
extern wchar_t* DebugWin_DrawHeapClearSwitch( void* userWork , gfl2::debug::DebugWinItem *item );

#if GFL_ENABLE_DYNAMIC_HEADER
static void DebugWin_AddDynamicHeaderWin(gfl2::heap::HeapBase* heap, gfl2::debug::DebugWinGroup* parent);
#endif

extern void DebugWin_UpdateGpuProfile( void* userWork  , gfl2::debug::DebugWinItem *item );
extern wchar_t* DebugWin_DrawGpuProfile( void* userWork  , gfl2::debug::DebugWinItem *item );
static void DebugWin_AddGpuProfileWin( gfl2::heap::HeapBase* heap,  gfl2::debug::DebugWinGroup* parent );

extern void DebugWin_UpdateGameManagerTickReset( void* userWork  , gfl2::debug::DebugWinItem *item );
extern wchar_t* DebugWin_DrawGameManagerTickName( void* userWork  , gfl2::debug::DebugWinItem *item );
extern wchar_t* DebugWin_DrawGameManagerTickValue( void* userWork  , gfl2::debug::DebugWinItem *item );
static void DebugWin_AddDrawGameManagerTick( gfl2::heap::HeapBase* heap,  gfl2::debug::DebugWinGroup* parent );

extern void DebugTestMenu_Script( gfl2::heap::HeapBase *heap );


static void KujiraDebgWin_OpenTimeGroup( void *userWork );
static void KujiraDebgWin_GetTime( void* userWork  , gfl2::debug::DebugWinItem *item );
static void KujiraDebgWin_SetTime( void* userWork  , gfl2::debug::DebugWinItem *item );
static void KujiraDebgWin_SetTimeSpeed( void* userWork  , gfl2::debug::DebugWinItem *item );
static void KujiraDebgWin_ResetTime( void* userWork  , gfl2::debug::DebugWinItem *item );

static void DebgWinFunc_SetVersoinSapphire( void* userWork  , gfl2::debug::DebugWinItem *item );
static void DebgWinFunc_SetVersoinRuby( void* userWork  , gfl2::debug::DebugWinItem *item );
static void DebgWinFunc_ResetVersoin( void* userWork  , gfl2::debug::DebugWinItem *item );

//  きせかえ用のコールバッグ
static void DebugWin_UpdateKisekaeFirstHave( void* userWork  , gfl2::debug::DebugWinItem *item );
static void DebugWin_UpdateKisekaeAllHave( void* userWork  , gfl2::debug::DebugWinItem *item );

//Heap系
void DebugWinFunc_Heap_UpdateAllDump( void* userWork  , gfl2::debug::DebugWinItem *item );
void DebugWinFunc_Heap_UpdateAllDumpLocal( void* userWork  , gfl2::debug::DebugWinItem *item );
void DebugWinFunc_Heap_UpdateTreeDump( void* userWork  , gfl2::debug::DebugWinItem *item );
void DebugWinFunc_Heap_UpdateDump( void* userWork  , gfl2::debug::DebugWinItem *item );
void DebugWinFunc_Heap_UpdateDumpLocal( void* userWork  , gfl2::debug::DebugWinItem *item );
static void DebugWinUpdate_MemoryVisualizerLog( void* userWork  , gfl2::debug::DebugWinItem *item );
static wchar_t * DebugWinDraw_MemoryVisualizerLog( void* userWork  , gfl2::debug::DebugWinItem *item );
static void DebugWinUpdate_HioLogInMemory( void* userWork  , gfl2::debug::DebugWinItem *item );
static wchar_t * DebugWinDraw_HioLogInMemory( void* userWork  , gfl2::debug::DebugWinItem *item );

static void DebugWinUpdate_HeapView( void* userWork  , gfl2::debug::DebugWinItem *item );
static wchar_t * DebugWinDraw_HeapView( void* userWork  , gfl2::debug::DebugWinItem *item );
static wchar_t * DebugWinDraw_HeapName( void* userWork  , gfl2::debug::DebugWinItem *item );
static void CreateGroup_HeapView( void *userWork, gfl2::debug::DebugWinItem *item );
static void CreateGroupAfter_HeapView( void *userWork, gfl2::debug::DebugWinItem *item );
static void DestroyGroup_HeapView( void *userWork, gfl2::debug::DebugWinItem *item );


//げんご
static void DebugWin_UpdateLanguage( void* userWork  , gfl2::debug::DebugWinItem *item );
static wchar_t* DebugWin_DrawLanguage( void* userWork  , gfl2::debug::DebugWinItem *item );

//テキスト速度
static void     DebugWin_UpdateTextSpdPutCountMax( void* userWork, gfl2::debug::DebugWinItem *item );
static void     DebugWin_UpdateTextSpdWaitFrame( void* userWork, gfl2::debug::DebugWinItem *item );
static wchar_t* DebugWin_DrawTextSpdWaitFrame( void* userWork, gfl2::debug::DebugWinItem *item );
static void     DebugWin_UpdateTextSpdPutCount( void* userWork, gfl2::debug::DebugWinItem *item );
static wchar_t* DebugWin_DrawTextSpdPutCount( void* userWork, gfl2::debug::DebugWinItem *item );
static void     DebugWin_UpdateTextSpdUsing( void* userWork, gfl2::debug::DebugWinItem *item );
static wchar_t* DebugWin_DrawTextSpdUsing( void* userWork, gfl2::debug::DebugWinItem *item );

#if USE_DEBUG_WIN
GFL_NAMESPACE_BEGIN(Item)

static void OpenKeyRecodeGroup( void *userWork )
{
  ::Debug::DebugWin::DebugWinWork *pDebugWinWork = GFL_SINGLETON_INSTANCE(::Debug::DebugWin::DebugWinWork);
  pDebugWinWork->mIsKeyReleaseForKeyRecode = false;
}

static void UpdateKeyLogRec( void* userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  gfl2::ui::Button *pButton = gfl2::debug::DebugWin_GetUiButton();

  ::Debug::DebugWin::DebugWinWork *pDebugWinWork = GFL_SINGLETON_INSTANCE(::Debug::DebugWin::DebugWinWork);

  // こうしないとデバッグメニューに入る操作の決定ボタンのリリース判定で誤作動するので。。。
  if( pDebugWinWork->mIsKeyReleaseForKeyRecode == false )
  {
    if( pButton->IsRelease( gfl2::ui::BUTTON_A ) )
    {
      pDebugWinWork->mIsKeyReleaseForKeyRecode = true;
    }
    return;
  }

  //： トリガーだと決定ボタンのリリース判定が入ってしまう為、リリースタイミングで決定する
  if( pButton->IsRelease( gfl2::ui::BUTTON_A ) || pButton->IsRelease( gfl2::ui::BUTTON_Y ) )
  {
    if( pGameManager->DebugUiDeviceIsLogRec() )
    {
      pGameManager->DebugUiDeviceEndLogRec();
    }
    else
    {
      pGameManager->DebugUiDeviceStartLogRec();
    }

    // ウィンドウを閉じる
    gfl2::debug::DebugWin_CloseWindow();
  }
}

static void UpdateKeyLogPlay( void* userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  gfl2::ui::Button *pButton = gfl2::debug::DebugWin_GetUiButton();
  
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    if( pGameManager->DebugUiDeviceIsLogRec() )
    {
      pGameManager->DebugUiDeviceEndPlayLog();
    }
    else
    {
      pGameManager->DebugUiDeviceStartPlayLog();
    }

    // ウィンドウを閉じる
    gfl2::debug::DebugWin_CloseWindow();
  }
}

GFL_NAMESPACE_END(Item)
#endif



//デバッグシステムの初期化
void InitializeSystem( gfl2::heap::HeapBase *parentHeap , gfl2::heap::HeapBase *parentDevHeap )
{
#if USE_DEBUG_WIN
  GameSys::GameManager *gMan = GFL_SINGLETON_INSTANCE(GameSys::GameManager);

  // @note 無駄があったので、ヒープをきらないようにした。　heap側は項目追加時にヒープをNULLにしていると使われるので注意。
#if 0
  gfl2::heap::HeapBase *heap = GFL_CREATE_HEAP( parentHeap , HEAPID_DEBUGWIN , 0x60000 , gfl2::heap::HEAP_TYPE_EXP , true );
  gfl2::heap::HeapBase *devHeap = GFL_CREATE_HEAP( parentDevHeap , HEAPID_DEBUGWIN_DEVICE , 0x80000 , gfl2::heap::HEAP_TYPE_EXP , true );
#endif
  gfl2::heap::HeapBase *heap = parentHeap;
  gfl2::heap::HeapBase *devHeap = parentDevHeap;

  gfl2::debug::DebugWin_Initialize(heap,devHeap, System::GflUse::GetResidentAllocator() ,GFL_SINGLETON_INSTANCE(print::SystemFont)->GetFont(),128);
  //デバイスマネージャーの設定(デバイスマネージャーが変わらなければ1回でOK
  gfl2::debug::DebugWin_SetDeviceManager( gMan->GetUiDeviceManager() );

  //デバッグ用のワーク
  DebugWinWork *debWork = GFL_NEW(heap) DebugWinWork();
  gfl2::debug::DebugWin_SetCallbackClass(debWork);
  
  // メッセージデータ
  gfl2::str::MsgData* pMsgData = GFL_NEW( heap ) gfl2::str::MsgData( print::GetMessageArcId_Debug(), GARC_debug_message_debug_menu_DAT, heap, gfl2::str::MsgData::LOAD_PART/*LOAD_FULL*/ );
  debWork->SetMessageData( pMsgData );
  debWork->InitializeMessageStrBuf( heap );


  //----------------------------------------------------
  //各メニューの追加



  // Debugグループ作成
  gfl2::debug::DebugWinGroup *pDebugGroup = gfl2::debug::DebugWin_AddGroup( heap , L"Debug" );


  //処理負荷
  gfl2::debug::DebugWinGroup *tickGroup = gfl2::debug::DebugWin_AddGroup( heap , pMsgData , msg_dmenu_tick_top, pDebugGroup );
  gfl2::debug::DebugWin_AddItemUpdateFunc( tickGroup , heap , L"" , NULL, DebugWin_UpdateTickBar , DebugWin_DrawTickBar );
  gfl2::debug::DebugWin_AddItemUpdateFunc( tickGroup , heap , pMsgData , msg_dmenu_tick_update , reinterpret_cast<void*>((int)DEBUGWIN_TICK_UPDATE) , DebugWin_UpdateTick , DebugWin_DrawTick );
  gfl2::debug::DebugWin_AddItemUpdateFunc( tickGroup , heap , pMsgData , msg_dmenu_tick_upnext , reinterpret_cast<void*>((int)DEBUGWIN_TICK_UPNEXT) , DebugWin_UpdateTick , DebugWin_DrawTick );
  gfl2::debug::DebugWin_AddItemUpdateFunc( tickGroup , heap , pMsgData , msg_dmenu_tick_draw   , reinterpret_cast<void*>((int)DEBUGWIN_TICK_DRAW)   , DebugWin_UpdateTick , DebugWin_DrawTick );
  gfl2::debug::DebugWin_AddItemUpdateFunc( tickGroup , heap , pMsgData , msg_dmenu_tick_gpu    , reinterpret_cast<void*>((int)DEBUGWIN_TICK_GPU)    , DebugWin_UpdateTick , DebugWin_DrawTick );
  gfl2::debug::DebugWin_AddItemUpdateFunc( tickGroup , heap , pMsgData , msg_dmenu_tick_cpu    , reinterpret_cast<void*>((int)DEBUGWIN_TICK_CPU)    , DebugWin_UpdateTick , DebugWin_DrawTick );
  gfl2::debug::DebugWin_AddItemUpdateFunc( tickGroup , heap , pMsgData , msg_dmenu_tick_reset  , NULL , DebugWin_ResetTick , NULL );
  gfl2::debug::DebugWinGroup *tickPrintGroup = gfl2::debug::DebugWin_AddGroup(heap, pMsgData, msg_dmenu_tickprint_top, tickGroup);
  gfl2::debug::DebugWin_AddItemUpdateFunc(tickGroup, heap, pMsgData, msg_dmenu_tick_com_buf, NULL, NULL, DebugWin_DrawComBuf);
  gfl2::debug::DebugWin_AddItemUpdateFunc( tickGroup , heap , pMsgData , msg_dmenu_tick_com_cnt    , NULL , NULL , DebugWin_DrawComReq );

  //処理負荷Print
  debWork->mDebugPrintTick = false;
  debWork->mDebugPrintTickCnt = 0;
  debWork->mDebugPrintTickInterval = 60;
  debWork->mDebugTickCheck = false;
  debWork->mDebugTickCheckSe = false;
  debWork->mDebugTickLimitCPU = 33333;
  debWork->mDebugTickLimitGPU = 33333;
  gfl2::debug::DebugWin_AddItemValueBool(tickPrintGroup, heap, pMsgData, msg_dmenu_tickprint_item_00, &debWork->mDebugPrintTick);
  gfl2::debug::DebugWin_AddItemValueInt(tickPrintGroup, heap, pMsgData, msg_dmenu_tickprint_item_01, &debWork->mDebugPrintTickInterval, 1, 6000);
  gfl2::debug::DebugWin_AddItemValueBool(tickPrintGroup, heap, pMsgData, msg_dmenu_tickprint_item_02, &debWork->mDebugTickCheck);
  gfl2::debug::DebugWin_AddItemValueBool(tickPrintGroup, heap, pMsgData, msg_dmenu_tickprint_item_03, &debWork->mDebugTickCheckSe);
  gfl2::debug::DebugWin_AddItemValueInt(tickPrintGroup, heap, pMsgData, msg_dmenu_tickprint_item_04, &debWork->mDebugTickLimitCPU, -1, 70000);
  gfl2::debug::DebugWin_AddItemValueInt(tickPrintGroup, heap, pMsgData, msg_dmenu_tickprint_item_05, &debWork->mDebugTickLimitGPU, -1, 70000);
  // ファイル
  {
    gfl2::debug::DebugWinGroup *fsGroup = gfl2::debug::DebugWin_AddGroup( heap , pMsgData , msg_dmenu_file_top, pDebugGroup );
    gfl2::debug::DebugWin_AddItemUpdateFunc( fsGroup , heap , pMsgData , msg_dmenu_file_pc       , reinterpret_cast<void*>(0) , DebugWin_UpdateFs , DebugWin_DrawFs );
    gfl2::debug::DebugWin_AddItemUpdateFunc( fsGroup , heap , pMsgData , msg_dmenu_file_garc_sys , reinterpret_cast<void*>(1) , DebugWin_UpdateFs , DebugWin_DrawFs );
    gfl2::debug::DebugWin_AddItemUpdateFunc( fsGroup , heap , pMsgData , msg_dmenu_file_garc     , reinterpret_cast<void*>(2) , DebugWin_UpdateFs , DebugWin_DrawFs );
    gfl2::debug::DebugWin_AddItemUpdateFunc( fsGroup , heap , pMsgData , msg_dmenu_file_sd_garc  , reinterpret_cast<void*>(3) , DebugWin_UpdateFs , DebugWin_DrawFs );
    gfl2::debug::DebugWin_AddItemUpdateFunc( fsGroup , heap , pMsgData , msg_dmenu_file_rom_access, reinterpret_cast<void*>(4) , DebugWin_UpdateFs , DebugWin_DrawFs );
    gfl2::debug::DebugWin_AddItemValueInt(   fsGroup , heap , L"SERomAccessCheck", &Sound::PMSoundSystem::m_bRomAccessCheckMode, 0, Sound::PMSoundSystem::DEBUG_ROMACCESS_MODE_ASSERT, 1);
 
    gfl2::debug::DebugWinGroup *fsProgGroup = gfl2::debug::DebugWin_AddGroup( heap , pMsgData , msg_dmenu_file_for_pg_top , fsGroup );
    gfl2::debug::DebugWin_AddItemUpdateFunc( fsProgGroup , heap , pMsgData , msg_dmenu_file_for_pg_rec , reinterpret_cast<void*>(0) , DebugWin_UpdateFsProg , DebugWin_DrawFsProg );
    gfl2::debug::DebugWin_AddItemUpdateFunc( fsProgGroup , heap , pMsgData , msg_dmenu_file_for_pg_clear  , reinterpret_cast<void*>(1) , DebugWin_UpdateFsProg , DebugWin_DrawFsProg );
    gfl2::debug::DebugWin_AddItemUpdateFunc( fsProgGroup , heap , pMsgData , msg_dmenu_file_for_pg_assert , reinterpret_cast<void*>(2) , DebugWin_UpdateFsProg , DebugWin_DrawFsProg );
    gfl2::debug::DebugWin_AddItemUpdateFunc( fsProgGroup , heap , pMsgData , msg_dmenu_file_for_pg_output , reinterpret_cast<void*>(3) , DebugWin_UpdateFsProg , DebugWin_DrawFsProg );
  }




  //Heap用デバッグ
  {
    ::Debug::DebugWin::DebugWinWork *work = GFL_SINGLETON_INSTANCE(::Debug::DebugWin::DebugWinWork);
    gfl2::debug::DebugWinGroup *heapGroup = gfl2::debug::DebugWin_AddGroup( heap , pMsgData ,msg_dmenu_heap_top, pDebugGroup );
    //全ヒープのダンプ
    gfl2::debug::DebugWin_AddItemUpdateFunc( heapGroup , heap , pMsgData , msg_dmenu_heap_all_dump       , NULL , DebugWinFunc_Heap_UpdateAllDump , NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( heapGroup , heap , pMsgData , msg_dmenu_heap_all_dump_local , NULL , DebugWinFunc_Heap_UpdateAllDumpLocal , NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( heapGroup , heap , pMsgData , msg_dmenu_heap_tree_dump      , NULL , DebugWinFunc_Heap_UpdateTreeDump , NULL );

    gfl2::debug::DebugWin_AddItemValueInt( heapGroup, heap, pMsgData , msg_dmenu_heap_dump_id        , &work->mHeapDumpId, 0, HEAPID_CHILD_MAX, 1);
    gfl2::debug::DebugWin_AddItemUpdateFunc( heapGroup , heap , pMsgData , msg_dmenu_heap_dump       , NULL , DebugWinFunc_Heap_UpdateDump , NULL );
    gfl2::debug::DebugWin_AddItemValueInt( heapGroup, heap, pMsgData , msg_dmenu_heap_dump_id_local  , &work->mLocalHeapDumpId, 0, HEAPID_LOCAL_MAX, 1);
    gfl2::debug::DebugWin_AddItemUpdateFunc( heapGroup , heap , pMsgData , msg_dmenu_heap_dump_local , NULL , DebugWinFunc_Heap_UpdateDumpLocal , NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( heapGroup , heap , pMsgData, msg_dmenu_heap_memfill , NULL, DebugWin_UpdateHeapClearSwitch , DebugWin_DrawHeapClearSwitch );

    gfl2::debug::DebugWin_AddItemValueInt( heapGroup, heap, L"VramLeakCheck", &Field::EventFieldmapOpen::m_s_DebugCheckVramClear, 0, 1, 1);

#if defined(GF_PLATFORM_CTR)
#if GFL_HIO_ENABLE
    gfl2::debug::DebugWin_AddItemUpdateFunc( heapGroup , heap , L"MemoryVisualizerLog", NULL , DebugWinUpdate_MemoryVisualizerLog , DebugWinDraw_MemoryVisualizerLog );
#endif
#ifdef GFL_DEBUG_HIO_LOG_IN_MEMORY
    gfl2::debug::DebugWin_AddItemUpdateFunc( heapGroup , heap , L"HioLogInMememory", NULL , DebugWinUpdate_HioLogInMemory , DebugWinDraw_HioLogInMemory );
#endif
#endif


    {
      gfl2::debug::DebugWinGroup* pGropWork = gfl2::debug::DebugWin_AddGroup( heap, L"HeapView", heapGroup, HEAPID_CHILD_MAX * 2 + 2);
      pGropWork->SetCreateDestroyGroupFunc( NULL, CreateGroup_HeapView, DestroyGroup_HeapView, CreateGroupAfter_HeapView );
      work->mHeapViewUnitType = 0;
    }

  }

  // time
  {
    gfl2::debug::DebugWinGroup* pDateTimeGroup;
    
    ::Debug::DebugWin::DebugWinWork *work = GFL_SINGLETON_INSTANCE(::Debug::DebugWin::DebugWinWork);
    pDateTimeGroup = gfl2::debug::DebugWin_AddGroup( heap, pMsgData , msg_dmenu_time_top, pDebugGroup );
    
    KujiraDebgWin_GetTime( NULL , NULL );
    pDateTimeGroup->SetOpenGroupFunc( KujiraDebgWin_OpenTimeGroup );
    
    gfl2::debug::DebugWin_AddItemValueInt( pDateTimeGroup, heap, pMsgData , msg_dmenu_time_mode , &work->mIsDeviceTime, 0, 1, 1);
    gfl2::debug::DebugWin_AddItemUpdateFunc( pDateTimeGroup , heap , pMsgData , msg_dmenu_time_get , NULL , KujiraDebgWin_GetTime , NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pDateTimeGroup , heap , pMsgData , msg_dmenu_time_set , NULL , KujiraDebgWin_SetTime , NULL );
    
    gfl2::debug::DebugWin_AddItemValueInt( pDateTimeGroup, heap, pMsgData , msg_dmenu_time_year, &work->mTimeYear, 1900, 2189, 1);
    gfl2::debug::DebugWin_AddItemValueInt( pDateTimeGroup, heap, pMsgData , msg_dmenu_time_month, &work->mTimeMonth, 1, 12, 1);
    gfl2::debug::DebugWin_AddItemValueInt( pDateTimeGroup, heap, pMsgData , msg_dmenu_time_day, &work->mTimeDay, 1, 31, 1);
    gfl2::debug::DebugWin_AddItemValueInt( pDateTimeGroup, heap, pMsgData , msg_dmenu_time_hour, &work->mTimeHour, 0, 23, 1);
    gfl2::debug::DebugWin_AddItemValueInt( pDateTimeGroup, heap, pMsgData , msg_dmenu_time_min, &work->mTimeMin, 0, 59, 1);
    gfl2::debug::DebugWin_AddItemValueInt( pDateTimeGroup, heap, pMsgData , msg_dmenu_time_sec, &work->mTimeSec, 0, 59, 1);
    gfl2::debug::DebugWin_AddItemValueInt( pDateTimeGroup, heap, pMsgData , msg_dmenu_time_speed, &work->mTimeSpeed, 0, 10000, 1);
    
    gfl2::debug::DebugWin_AddItemUpdateFunc( pDateTimeGroup, heap, pMsgData, msg_dmenu_time_set_spd, NULL, KujiraDebgWin_SetTimeSpeed, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pDateTimeGroup, heap, pMsgData, msg_dmenu_time_reset, NULL, KujiraDebgWin_ResetTime, NULL );
  }
  
  //PauseSystem
  //ポーズシステム
  {
    Debug::DebugWin::PauseSystem *pauseSys = GFL_SINGLETON_INSTANCE(Debug::DebugWin::PauseSystem);

    gfl2::debug::DebugWinGroup* pauseGroup = gfl2::debug::DebugWin_AddGroup( heap, pMsgData, msg_dmenu_pause_top, pDebugGroup );

    gfl2::debug::DebugWin_AddItemValueBool( pauseGroup , heap, pMsgData , msg_dmenu_pause_enable, pauseSys->GetIsEnablePtr() );
    gfl2::debug::DebugWin_AddItemValueBool( pauseGroup , heap, pMsgData , msg_dmenu_pause_dbg_win_check, pauseSys->GetIsCheckDebugWin() );
    {
      DEBWIN_GROUP_WORK *grpWork = static_cast<DEBWIN_GROUP_WORK*>(pauseGroup->CreateGroupWork(sizeof(DEBWIN_GROUP_WORK)));
      grpWork->mTrgWork.mKeyBit  = pauseSys->GetStepKey();
      grpWork->mHoldWork.mKeyBit = pauseSys->GetHoldKey();

      DebugWin_AddItemUpdateFunc( pauseGroup , heap, pMsgData , msg_dmenu_pause_hold, &grpWork->mHoldWork , KujiraDebugWin_UpdateKeySet , KujiraDebgWin_DrawKeySet );
      DebugWin_AddItemUpdateFunc( pauseGroup , heap, pMsgData , msg_dmenu_pause_step, &grpWork->mTrgWork , KujiraDebugWin_UpdateKeySet , KujiraDebgWin_DrawKeySet );
    }
  }

  // KeyRecordSystem
  {
    gfl2::debug::DebugWinGroup* pWorkGroup = gfl2::debug::DebugWin_AddGroup( heap, pMsgData, msg_dmenu_ui_rec_top, pDebugGroup );
    pWorkGroup->SetOpenGroupFunc( Item::OpenKeyRecodeGroup );

    DebugWin_AddItemUpdateFunc( pWorkGroup , heap , pMsgData, msg_dmenu_ui_rec_record, NULL, Item::UpdateKeyLogRec, NULL );
    DebugWin_AddItemUpdateFunc( pWorkGroup , heap , pMsgData, msg_dmenu_ui_rec_play, NULL, Item::UpdateKeyLogPlay, NULL );
    //DebugWin_AddItemUpdateFunc( pWorkGroup , heap , ""DebMenu_UI_Output, &grpWork->mTrgWork , KujiraDebugWin_UpdateKeyLogOutput, KujiraDebugWin_DrawKeyLog );
    //DebugWin_AddItemUpdateFunc( pWorkGroup , heap , DebMenu_UI_Load, &grpWork->mTrgWork , KujiraDebugWin_UpdateKeyLogLoad, KujiraDebugWin_DrawKeyLog );
    //DebugWin_AddItemUpdateFunc( pWorkGroup , heap , DebMenu_UI_TimeLimit, &grpWork->mTrgWork , KujiraDebugWin_UpdateKeyTimeLimit, KujiraDebugWin_DrawKeyTimeLimit );
    //DebugWin_AddItemUpdateFunc( pWorkGroup , L"Loop Count", &grpWork->mTrgWork , DebgWinFunc_None, DebugWinDrawFunc_Loop );
  }

  // GameManager Tick View追加
  DebugWin_AddDrawGameManagerTick( heap, pDebugGroup );

  // GPUProfile追加
  DebugWin_AddGpuProfileWin( heap, pDebugGroup );

#if GFL_ENABLE_DYNAMIC_HEADER
  // 動的ヘッダロードシステム制御メニュー
  DebugWin_AddDynamicHeaderWin( heap, pDebugGroup );
#endif

  // システムフラグ
  {
    gfl2::debug::DebugWinGroup* systemFlagGroup = gfl2::debug::DebugWin_AddGroup( heap, pMsgData, msg_dmenu_sysflag_top, pDebugGroup );
    gfl2::debug::DebugWin_AddItemValueBool( systemFlagGroup , heap, pMsgData, msg_dmenu_sysflag_resident_heap, gMan->GetGameEventManager()->GetDebugChkHeapDataAssertFlag() );
    gfl2::debug::DebugWin_AddItemUpdateFunc( systemFlagGroup , heap , pMsgData, msg_dmenu_sysflag_bgm_switch , reinterpret_cast<void*>(0) , DebugWin_UpdateBGMSwitch , DebugWin_DrawBGMSwitch );
    gfl2::debug::DebugWin_AddItemValueInt( systemFlagGroup , heap, pMsgData, msg_dmenu_sysflag_bgm_print_log, &(Sound::BGMManager::DebugPrintBGMLog), 0, 1 );
    gfl2::debug::DebugWin_AddItemUpdateFunc( systemFlagGroup , heap , pMsgData, msg_dmenu_sysflag_bgm_switch , reinterpret_cast<void*>(0) , DebugWin_UpdateSoundOutputMode , DebugWin_DrawBSoundOutputMode );
    gfl2::debug::DebugWin_AddItemValueBool( systemFlagGroup , heap, pMsgData, msg_dmenu_sysflag_ext_save_fatal_error, ExtSavedata::NijiExtSaveSystem::GetDebugExtSaveFatalErrorFlag() );

    gfl2::debug::DebugWin_AddItemUpdateFunc( systemFlagGroup , heap, L"GflUse::PublicRandDebug",  reinterpret_cast<void*>(0), DebugWin_UpdateGflUsePublicRandDebug, DebugWin_DrawGflUsePublicRandDebug );
  }

  // レイアウト
  {
    gfl2::debug::DebugWinGroup *lytGroup = gfl2::debug::DebugWin_AddGroup( heap , pMsgData, msg_dmenu_layout_top, pDebugGroup );
    gfl2::debug::DebugWin_AddItemUpdateFunc( lytGroup , heap , pMsgData, msg_dmenu_layout_assert , reinterpret_cast<void*>(0) , DebugWin_UpdateLyt , DebugWin_DrawLyt );
  }

  {
    gfl2::debug::DebugWinGroup* pRomVersionFake;
    
    //::Debug::DebugWin::DebugWinWork *work = GFL_SINGLETON_INSTANCE(::Debug::DebugWin::DebugWinWork);
    pRomVersionFake = gfl2::debug::DebugWin_AddGroup( heap, pMsgData, msg_dmenu_rom_ver_fake_top, pDebugGroup );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pRomVersionFake, heap, pMsgData, msg_dmenu_rom_ver_fake_sun, NULL, DebgWinFunc_SetVersoinSapphire, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pRomVersionFake, heap, pMsgData, msg_dmenu_rom_ver_fake_moon, NULL, DebgWinFunc_SetVersoinRuby, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pRomVersionFake, heap, pMsgData, msg_dmenu_rom_ver_fake_reset, NULL, DebgWinFunc_ResetVersoin, NULL );
  }

  //  きせかえ
  {
    gfl2::debug::DebugWinGroup *pKisekaeGroup = gfl2::debug::DebugWin_AddGroup( heap , pMsgData, msg_dmenu_kisekae_top, pDebugGroup );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pKisekaeGroup , heap , pMsgData, msg_dmenu_kisekae_first_have , reinterpret_cast<void*>(0) , DebugWin_UpdateKisekaeFirstHave , NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pKisekaeGroup , heap , pMsgData, msg_dmenu_kisekae_all_have , reinterpret_cast<void*>(0) , DebugWin_UpdateKisekaeAllHave , NULL );
  }

  // 通信
  {
    gfl2::debug::DebugWinGroup *pNetDebugGroup = gfl2::debug::DebugWin_AddGroup( heap , pMsgData, msg_dmenu_network_top, pDebugGroup );
    if( debWork->m_pNetDebug == NULL )
    {
      debWork->m_pNetDebug = GFL_NEW( heap ) NetLib::Debug::NetDebug( pNetDebugGroup );
    }
  }

  // 言語
  {
    gfl2::debug::DebugWinGroup * languageGroup = gfl2::debug::DebugWin_AddGroup( heap , pMsgData, msg_dmenu_language_00, pDebugGroup );
    gfl2::debug::DebugWin_AddItemUpdateFunc( languageGroup, heap, pMsgData, msg_dmenu_language_00, NULL, DebugWin_UpdateLanguage, DebugWin_DrawLanguage );

    // テキスト速度
    {
      gfl2::debug::DebugWinGroup * textSpdGroup = gfl2::debug::DebugWin_AddGroup( heap, pMsgData, msg_dmenu_text_speed_00, languageGroup );
      gfl2::debug::DebugWin_AddItemUpdateFunc( textSpdGroup, heap, pMsgData, msg_dmenu_text_speed_02, NULL, DebugWin_UpdateTextSpdPutCountMax, NULL );
      gfl2::debug::DebugWin_AddItemUpdateFunc( textSpdGroup, heap, pMsgData, msg_dmenu_text_speed_00, NULL, DebugWin_UpdateTextSpdWaitFrame, DebugWin_DrawTextSpdWaitFrame );
      gfl2::debug::DebugWin_AddItemUpdateFunc( textSpdGroup, heap, pMsgData, msg_dmenu_text_speed_00, NULL, DebugWin_UpdateTextSpdPutCount,  DebugWin_DrawTextSpdPutCount );
      gfl2::debug::DebugWin_AddItemUpdateFunc( textSpdGroup, heap, pMsgData, msg_dmenu_text_speed_00, NULL, DebugWin_UpdateTextSpdUsing,     DebugWin_DrawTextSpdUsing );
    }
  }

  // ROM生成日時
  gfl2::debug::DebugWin_AddItemUpdateFunc( pDebugGroup, heap , L"ROM create" , NULL, NULL, DebugWin_DrawRomCreateDateTime );

#endif //USE_DEBUG_WIN
}

//デバッグシステムの終了
void FinalizeSystem()
{
#if USE_DEBUG_WIN

  gfl2::debug::DebugWin_Finalize();
  DebugWinWork *debWork = GFL_SINGLETON_INSTANCE(DebugWinWork);
  if( debWork != NULL )
  {
    debWork->FinalizeMessageStrBuf();
    gfl2::str::MsgData * pMsgData = debWork->GetMessageData();
    GFL_SAFE_DELETE( pMsgData );
  }
  GFL_SAFE_DELETE( debWork );

#endif //USE_DEBUG_WIN
}

//デバッグシステムの更新(TickPrint用
void UpdateSystem()
{
#if USE_DEBUG_WIN
  ::Debug::DebugWin::DebugWinWork *work = GFL_SINGLETON_INSTANCE(::Debug::DebugWin::DebugWinWork);

  if (work->mDebugPrintTick)
  {
    work->mDebugPrintTickCnt++;
    if (work->mDebugPrintTickCnt > work->mDebugPrintTickInterval)
    {
      work->mDebugPrintTickCnt = 0;
      const int update = GFL_SINGLETON_INSTANCE(Debug::DebugWin::DebugWinWork)->GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_UPDATE);
      const int upnext = GFL_SINGLETON_INSTANCE(Debug::DebugWin::DebugWinWork)->GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_UPNEXT);
      const int draw = GFL_SINGLETON_INSTANCE(Debug::DebugWin::DebugWinWork)->GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_DRAW);
      const int gpu = GFL_SINGLETON_INSTANCE(Debug::DebugWin::DebugWinWork)->GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_GPU);
      GFL_RELEASE_PRINT("Tick[%6d][%6d][%6d][%6d]\n", update, upnext, draw, gpu);
    }
  }
  if (work->mDebugTickCheck ||
    work->mDebugTickCheckSe)
  {
    bool isOver = false;
    const int update = GFL_SINGLETON_INSTANCE(Debug::DebugWin::DebugWinWork)->GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_UPDATE);
    const int upnext = GFL_SINGLETON_INSTANCE(Debug::DebugWin::DebugWinWork)->GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_UPNEXT);
    const int draw = GFL_SINGLETON_INSTANCE(Debug::DebugWin::DebugWinWork)->GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_DRAW);
    const int gpu = GFL_SINGLETON_INSTANCE(Debug::DebugWin::DebugWinWork)->GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_GPU);
    if (work->mDebugTickLimitCPU != -1 &&
      update + upnext + draw >= work->mDebugTickLimitCPU)
    {
      isOver = true;
    }
    if (work->mDebugTickLimitGPU != -1 &&
      gpu >= work->mDebugTickLimitGPU)
    {
      isOver = true;
    }

    if (isOver == true)
    {
      if (work->mDebugTickCheck)
      {
        GFL_RELEASE_PRINT("TickOver![%6d][%6d][%6d][%6d]\n", update, upnext, draw, gpu);
      }
      if (work->mDebugTickCheckSe)
      {
        Sound::PlaySE(SEQ_SE_BEEP);
      }
    }
  }
#endif //USE_DEBUG_WIN
}


//デバッグウィンコールバック
//----------------------------------------------------------------------------
/**
 *  @brief  デバッグウィンドウ用サウンド再生
 *          gflibにサウンドを持たせるわけにもいかないので、アプリにお任せ。
 *
 *  @param  const DEBUGWIN_SOUND_TYPE サウンド種類
 */
//-----------------------------------------------------------------------------
#if USE_DEBUG_WIN
void DebugWinWork::SoundCallBack( const gfl2::debug::DEBUGWIN_SOUND_TYPE type )
{
  if( !Sound::IsInitialized() ) { return; }
  switch(type)
  {
  case gfl2::debug::SE_CURSOR_MOVE:
    Sound::PlaySE( SEQ_SE_SELECT1 );
    break;
  case gfl2::debug::SE_CHANGE_VALUE:
    Sound::PlaySE( SEQ_SE_SLIDE );
    break;
  case gfl2::debug::SE_GROUP_IN:
    Sound::PlaySE( SEQ_SE_DECIDE1 );
    break;
  case gfl2::debug::SE_GROUP_OUT:
    Sound::PlaySE( SEQ_SE_CANCEL1 );
    break;
  case gfl2::debug::SE_ACCEPT:
    Sound::PlaySE( SEQ_SE_DECIDE1 );
    break;
  case gfl2::debug::SE_CANCEL:
    Sound::PlaySE( SEQ_SE_CANCEL1 );
    break;
  case gfl2::debug::SE_ERROR:
    Sound::PlaySE( SEQ_SE_BEEP );
    break;
  case gfl2::debug::SE_WINDOW_OPEN:
    Sound::PlaySE( SEQ_SE_OPEN1 );
    break;
  case gfl2::debug::SE_WINDOW_CLOSE:
    Sound::PlaySE( SEQ_SE_CLOSE1 );
    break;
  }
}
#endif
//Tick計測用
void TickStart( int tickType )
{
#if USE_DEBUG_WIN
  GFL_SINGLETON_INSTANCE(DebugWinWork)->TickStart(tickType);
#endif //USE_DEBUG_WIN
}

void TickEnd( int tickType )
{
#if USE_DEBUG_WIN
  GFL_SINGLETON_INSTANCE(DebugWinWork)->TickEnd(tickType);
#endif //USE_DEBUG_WIN
}

void TickCalc( int tickType , s64 microSec )
{
#if USE_DEBUG_WIN
  GFL_SINGLETON_INSTANCE(DebugWinWork)->TickCalc(tickType,microSec);
#endif //USE_DEBUG_WIN
}



#if USE_DEBUG_WIN
//キー登録用関数
void KujiraDebugWin_UpdateKeySet( void* userWork  , gfl2::debug::DebugWinItem *item )
{
#if GFL_DEBUGWIN_USE
  GFL_UNUSED(item);
  KEY_SET_WORK *work = static_cast<KEY_SET_WORK*>(userWork);

  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();

  switch( work->mSeq )
  {
  case 0:
    if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
    {
      item->PlaySystemSound(gfl2::debug::SE_ACCEPT);
      work->mKeepMode = true;
      work->mSeq++;
    }
    break;
  case 1:
    int tempBit = 0;
    work->mKeepMode = true;
    if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
    {
      tempBit += gfl2::ui::BUTTON_A;
    }
    if( uiBtn->IsTrigger( gfl2::ui::BUTTON_B ) )
    {
      tempBit += gfl2::ui::BUTTON_B;
    }
    if( uiBtn->IsTrigger( gfl2::ui::BUTTON_X ) )
    {
      tempBit += gfl2::ui::BUTTON_X;
    }
    if( uiBtn->IsTrigger( gfl2::ui::BUTTON_Y ) )
    {
      tempBit += gfl2::ui::BUTTON_Y;
    }
    if( uiBtn->IsTrigger( gfl2::ui::BUTTON_R ) )
    {
      tempBit += gfl2::ui::BUTTON_R;
    }
    if( uiBtn->IsTrigger( gfl2::ui::BUTTON_L ) )
    {
      tempBit += gfl2::ui::BUTTON_L;
    }

    if( tempBit != 0 )
    {
      item->PlaySystemSound(gfl2::debug::SE_ACCEPT);
      *work->mKeyBit = tempBit;
      work->mSeq = 0;
      GFL_PRINT("[%d]\n",*work->mKeyBit);
    }
    break;
  }
#endif
}

wchar_t* KujiraDebgWin_DrawKeySet( void* userWork  , gfl2::debug::DebugWinItem *item )
{
#if GFL_DEBUGWIN_USE
  KEY_SET_WORK *work = static_cast<KEY_SET_WORK*>(userWork);

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  const int keyBit = *work->mKeyBit;

  switch( work->mSeq )
  {
  case 0:
    swprintf( workStr , gfl2::debug::DEBUGWIN_NAME_LEN
              ,L"%ls[%ls%ls%ls%ls%ls%ls] " ,item->GetName()
              ,(keyBit&gfl2::ui::BUTTON_A?L"A":L"-")
              ,(keyBit&gfl2::ui::BUTTON_B?L"B":L"-")
              ,(keyBit&gfl2::ui::BUTTON_X?L"X":L"-")
              ,(keyBit&gfl2::ui::BUTTON_Y?L"Y":L"-")
              ,(keyBit&gfl2::ui::BUTTON_L?L"L":L"-")
              ,(keyBit&gfl2::ui::BUTTON_R?L"R":L"-"));
    break;
  case 1:
    DebugWinWork* devWork = GFL_SINGLETON_INSTANCE( DebugWinWork );
    gfl2::debug::DebugWin* devWin = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin);
    gfl2::str::MsgData * pMsgData = devWork->GetMessageData();
    gfl2::str::StrBuf* workStrBuf =GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetWorkStrBuf();
    pMsgData->GetString( msg_dmenu_pause_wait_input, *workStrBuf );

    swprintf( workStr , gfl2::debug::DEBUGWIN_NAME_LEN
                ,L"%ls[%ls] " ,item->GetName(), workStrBuf->GetPtr() );
    if( work->mKeepMode )
    {
      work->mKeepMode = false;
    }
    else
    {
      work->mSeq = 0;
    }
    break;
  }
  return workStr;
#else
  return NULL;
#endif
}


//---------------------------------------------------------------------------
//ROM生成日時表示
wchar_t* DebugWin_DrawRomCreateDateTime( void* userWork  , gfl2::debug::DebugWinItem *item )
{
#if GFL_DEBUGWIN_USE
  //外から呼ぶから引数は使わないこと！
  GFL_UNUSED(item);
  GFL_UNUSED(userWork);

  DebugWinWork *work = GFL_SINGLETON_INSTANCE(DebugWinWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr , gfl2::debug::DEBUGWIN_NAME_LEN
              ,L"%ls date[%s] time[%s]", item->GetName(), __DATE__, __TIME__ );

  return workStr;
#else
  return NULL;
#endif

}



//---------------------------------------------------------------------------
//処理負荷デバッグメニュー用関数
void DebugWin_UpdateTickBar( void* userWork  , gfl2::debug::DebugWinItem *item )
{
#if GFL_DEBUGWIN_USE
  GFL_UNUSED(item);
  GFL_UNUSED(userWork);

  DebugWinWork *work = GFL_SINGLETON_INSTANCE(DebugWinWork);

  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();

  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    item->PlaySystemSound(gfl2::debug::SE_CHANGE_VALUE);
    work->SetMode60( !work->IsMode60() );
  }
#endif
}

wchar_t* DebugWin_DrawTickBar( void* userWork  , gfl2::debug::DebugWinItem *item )
{
#if GFL_DEBUGWIN_USE
  //外から呼ぶから引数は使わないこと！
  GFL_UNUSED(item);
  GFL_UNUSED(userWork);

  DebugWinWork *work = GFL_SINGLETON_INSTANCE(DebugWinWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  gfl2::debug::DebugWin* devWin = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin);

  gfl2::str::StrBuf* strMode = work->GetMessageStrBuf( DebugWinWork::DEBUG_WIN_STR_IDX_TICK_MODE );
  gfl2::str::StrBuf* strFrame = work->GetMessageStrBuf( DebugWinWork::DEBUG_WIN_STR_IDX_TICK_FRAME );

  swprintf( workStr , gfl2::debug::DEBUGWIN_NAME_LEN
              ,L"%ls %2d%ls この後ろにバーを書く予定", strMode->GetPtr(), (work->IsMode60()?60:30), strFrame->GetPtr() );

  //バーの描画
  static const s32 BarL = 64;
  static const s32 BarR = 288;
  static const s32 BarW = BarR-BarL;
  static const s32 BarT = *gfl2::debug::DebugWin_GetLineHeight();
  static const s32 BarB = *gfl2::debug::DebugWin_GetLineHeight()*2;
  static const s32 BarH = BarB-BarT;

  /*
  gfl2::grp::util::DrawUtil::MaterialInfo info;
  gfl2::grp::util::DrawUtil::SetMaterial( info );

  //色バー
  if( work->IsMode60() )
  {
    static const int maxSec = 16666;

    gfl2::math::VEC4 UpdateCol(1.0f,0.0f,0.0f,1.0f);
    gfl2::math::VEC4 DrawCol(0.0f,1.0f,0.0f,1.0f);
    gfl2::math::VEC4 GpuCol(1.0f,1.0f,0.0f,1.0f);

    s32 UpdateLen = work->GetTickNow( DEBUGWIN_TICK_UPDATE ) * BarW / maxSec;
    s32 DrawLen   = work->GetTickNow( DEBUGWIN_TICK_DRAW   ) * BarW / maxSec;
    s32 GpuLen    = work->GetTickNow( DEBUGWIN_TICK_GPU    ) * BarW / maxSec;

    gfl2::grp::util::DrawUtil::DrawRect( BarL , BarT , UpdateLen , BarH/2 , UpdateCol );
    gfl2::grp::util::DrawUtil::DrawRect( BarL + UpdateLen, BarT , DrawLen , BarH/2 , DrawCol );
    gfl2::grp::util::DrawUtil::DrawRect( BarL , BarT+BarH/2 , GpuLen , BarH/2 , GpuCol );

  }
  else
  {
    static const int maxSec = 16666;

    gfl2::math::VEC4 UpdateCol(1.0f,0.0f,0.0f,1.0f);
    gfl2::math::VEC4 UpNextCol(0.0f,0.0f,1.0f,1.0f);
    gfl2::math::VEC4 DrawCol(0.0f,1.0f,0.0f,1.0f);
    gfl2::math::VEC4 GpuCol(1.0f,1.0f,0.0f,1.0f);
    s32 UpdateLen = work->GetTickNow( DEBUGWIN_TICK_UPDATE ) * BarW / maxSec;
    s32 UpNextLen = work->GetTickNow( DEBUGWIN_TICK_UPNEXT ) * BarW / maxSec;
    s32 DrawLen   = work->GetTickNow( DEBUGWIN_TICK_DRAW   ) * BarW / maxSec;
    s32 GpuLen    = work->GetTickNow( DEBUGWIN_TICK_GPU    ) * BarW / 33333;

    gfl2::grp::util::DrawUtil::DrawRect( BarL , BarT , UpdateLen , BarH/3 , UpdateCol );
    gfl2::grp::util::DrawUtil::DrawRect( BarL , BarT+BarH/3 , UpNextLen , BarH/3 , UpNextCol );

    gfl2::grp::util::DrawUtil::DrawRect( BarL + UpNextLen, BarT+BarH/3 , DrawLen , BarH/3 , DrawCol );
    gfl2::grp::util::DrawUtil::DrawRect( BarL , BarT+(BarH/3*2) , GpuLen , BarH/3 , GpuCol );
  }
  //判定ライン
  gfl2::grp::util::DrawUtil::DrawRect( BarL,BarT,2,BarH );
  gfl2::grp::util::DrawUtil::DrawRect( BarL+BarW/2,BarT,2,BarH );
  gfl2::grp::util::DrawUtil::DrawRect( BarR,BarT,2,BarH );
  */

  return workStr;
#else
  return NULL;
#endif
}


void DebugWin_UpdateTick( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  GFL_UNUSED(item);
  GFL_UNUSED(userWork);
  //int type = reinterpret_cast<int>(userWork);
  //DebugWinWork *work = GFL_SINGLETON_INSTANCE(DebugWinWork);

}

wchar_t* DebugWin_DrawTick( void* userWork  , gfl2::debug::DebugWinItem *item )
{
#if GFL_DEBUGWIN_USE
  int type = reinterpret_cast<int>(userWork);
  DebugWinWork *work = GFL_SINGLETON_INSTANCE(DebugWinWork);


  gfl2::str::StrBuf* strNow = work->GetMessageStrBuf( DebugWinWork::DEBUG_WIN_STR_IDX_NOW );
  gfl2::str::StrBuf* strAve = work->GetMessageStrBuf( DebugWinWork::DEBUG_WIN_STR_IDX_AVE );
  gfl2::str::StrBuf* strMax = work->GetMessageStrBuf( DebugWinWork::DEBUG_WIN_STR_IDX_MAX );

  s64 ave = work->GetTickCalcAve(type);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr , gfl2::debug::DEBUGWIN_NAME_LEN
            ,L"%ls %ls[%6d]%ls[%6d]%ls[%6d]" ,item->GetName()
            ,strNow->GetPtr()
            ,(int)work->GetTickNow(type)
            ,strAve->GetPtr()
            ,(int)ave
            ,strMax->GetPtr()
            ,(int)work->GetTickMax(type) );

  return workStr;
#else
  return NULL;
#endif
}

void DebugWin_ResetTick( void* userWork  , gfl2::debug::DebugWinItem *item )
{
#if GFL_DEBUGWIN_USE
  GFL_UNUSED(item);
  GFL_UNUSED(userWork);

  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();
  DebugWinWork *work = GFL_SINGLETON_INSTANCE(DebugWinWork);

  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    item->PlaySystemSound(gfl2::debug::SE_ACCEPT);
    work->TickReset();
  }
#endif
}



void DebugWin_UpdateFs( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  GFL_UNUSED(item);

#if GFL_DEBUGWIN_USE
  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();

  enum
  {
    ITEM_INDEX_HIO_USE_FLAG                    = 0,
    ITEM_INDEX_ARCSRC_CREATE_FLAG              = 1,
    ITEM_INDEX_ARCSRC_USE_FLAG                 = 2,
    ITEM_INDEX_DIRECT_SDMC_ARC_FILE_USE_FLAG   = 3,
    ITEM_INDEX_INTERVAL_CHECK_LEVEL            = 4,
  };
  int itemIndex = reinterpret_cast<int>(userWork);

  GameSys::GameManager *gMan = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  gfl2::fs::AsyncFileManager* romAsyncFileManager = gMan->GetAsyncFileManager();

  switch(itemIndex)
  {
  case ITEM_INDEX_HIO_USE_FLAG:
    {
#if defined(GF_PLATFORM_CTR)
      bool reloadFlag = false;
      bool useFlag = romAsyncFileManager->GetHioUseFlag();
      if( uiBtn->IsTrigger( gfl2::ui::BUTTON_LEFT ) || uiBtn->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
      {
        useFlag = !useFlag;
        reloadFlag = true;
      }
      romAsyncFileManager->SetHioUseFlag(useFlag);
      if (reloadFlag)
      {
        System::GflUse::ReloadGarc();
      }
#endif
    }
    break;
  case ITEM_INDEX_ARCSRC_CREATE_FLAG:
    {
      bool createFlag = romAsyncFileManager->GetArcSrcCreateFlag();
      if( uiBtn->IsTrigger( gfl2::ui::BUTTON_LEFT ) || uiBtn->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
      {
        createFlag = !createFlag;
      }
      romAsyncFileManager->SetArcSrcCreateFlag(createFlag);
    }
    break;
  case ITEM_INDEX_ARCSRC_USE_FLAG:
    {
      bool useFlag = romAsyncFileManager->GetArcSrcUseFlag();
      if( uiBtn->IsTrigger( gfl2::ui::BUTTON_LEFT ) || uiBtn->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
      {
        useFlag = !useFlag;
      }
      romAsyncFileManager->SetArcSrcUseFlag(useFlag);
    }
    break;
  case ITEM_INDEX_DIRECT_SDMC_ARC_FILE_USE_FLAG:
    {
      bool useFlag = romAsyncFileManager->GetDirectSdmcArcFileUseFlag();
      if( uiBtn->IsTrigger( gfl2::ui::BUTTON_LEFT ) || uiBtn->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
      {
        useFlag = !useFlag;
      }
      romAsyncFileManager->SetDirectSdmcArcFileUseFlag(useFlag);
    }
    break;
  case ITEM_INDEX_INTERVAL_CHECK_LEVEL:
    {
      s32 level = romAsyncFileManager->GetIntervalCheckLevel();
      if( uiBtn->IsRepeat( gfl2::ui::BUTTON_LEFT ) ) --level;
      else if( uiBtn->IsTrigger( gfl2::ui::BUTTON_RIGHT ) ) ++level;
      romAsyncFileManager->SetIntervalCheckLevel(level);  // levelの値オーバーはこの関数内で直してくれる
    }
    break;
  }
#endif
}

wchar_t* DebugWin_DrawFs( void* userWork  , gfl2::debug::DebugWinItem *item )
{
#if GFL_DEBUGWIN_USE
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"" );

  enum
  {
    ITEM_INDEX_HIO_USE_FLAG                    = 0,
    ITEM_INDEX_ARCSRC_CREATE_FLAG              = 1,
    ITEM_INDEX_ARCSRC_USE_FLAG                 = 2,
    ITEM_INDEX_DIRECT_SDMC_ARC_FILE_USE_FLAG   = 3,
    ITEM_INDEX_INTERVAL_CHECK_LEVEL            = 4,
  };
  int itemIndex = reinterpret_cast<int>(userWork);

  GameSys::GameManager *gMan = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  gfl2::fs::AsyncFileManager* romAsyncFileManager = gMan->GetAsyncFileManager();

  switch(itemIndex)
  {
  case ITEM_INDEX_HIO_USE_FLAG:
    {
      bool useFlag = false;
#if defined(GF_PLATFORM_CTR)
      useFlag = romAsyncFileManager->GetHioUseFlag();
#elif defined(GF_PLATFORM_WIN32)
      useFlag = true;  // 「PCファイルを読む」という項目なので、Win32では常にtrueとなる。
#endif
      swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN,
        L"%ls [%d]",
        item->GetName(), useFlag?(1):(0) );
    }
    break;
  case ITEM_INDEX_ARCSRC_CREATE_FLAG:
    {
      bool createFlag = romAsyncFileManager->GetArcSrcCreateFlag();
      swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN,
        L"%ls [%d]",
        item->GetName(), createFlag?(1):(0) );
    }
    break;
  case ITEM_INDEX_ARCSRC_USE_FLAG:
    {
      bool useFlag = romAsyncFileManager->GetArcSrcUseFlag();
      swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN,
        L"%ls [%d]",
        item->GetName(), useFlag?(1):(0) );
    }
    break;
  case ITEM_INDEX_DIRECT_SDMC_ARC_FILE_USE_FLAG:
    {
      bool useFlag = romAsyncFileManager->GetDirectSdmcArcFileUseFlag();
      swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN,
        L"%ls [%d]",
        item->GetName(), useFlag?(1):(0) );
    }
    break;
  case ITEM_INDEX_INTERVAL_CHECK_LEVEL:
    {
      s32 level = romAsyncFileManager->GetIntervalCheckLevel();
      swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN,
        L"%ls [%d]",
        item->GetName(), level );
    }
    break;
  }

  return workStr;
#else
  return NULL;
#endif
}

void DebugWin_UpdateFsProg( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  GFL_UNUSED(item);

#if GFL_DEBUGWIN_USE
  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();

  enum
  {
    ITEM_INDEX_MATCH_CHECK_FLAG             = 0,
    ITEM_INDEX_MATCH_CHECK_CLEAR_REMAINING  = 1,
    ITEM_INDEX_MATCH_CHECK_ANNOUNCE_ASSERT  = 2,
    ITEM_INDEX_MATCH_CHECK_OUTPUT_REMAINING = 3,
  };
  int itemIndex = reinterpret_cast<int>(userWork);

  GameSys::GameManager *gMan = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  gfl2::fs::AsyncFileManager* romAsyncFileManager = gMan->GetAsyncFileManager();

  switch(itemIndex)
  {
  case ITEM_INDEX_MATCH_CHECK_FLAG:
    {
      bool flag = romAsyncFileManager->GetMatchCheckFlag();
      if( uiBtn->IsTrigger( gfl2::ui::BUTTON_LEFT ) || uiBtn->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
      {
        flag = !flag;
      }
      romAsyncFileManager->SetMatchCheckFlag(flag);
    }
    break;
  case ITEM_INDEX_MATCH_CHECK_CLEAR_REMAINING:
    {
      if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
      {
        romAsyncFileManager->ClearMatchCheckRemainingReq();
      }
    }
    break;
  case ITEM_INDEX_MATCH_CHECK_ANNOUNCE_ASSERT:
    {
      bool isAssert = romAsyncFileManager->IsMatchCheckAnnounceAssert();
      if( uiBtn->IsTrigger( gfl2::ui::BUTTON_LEFT ) || uiBtn->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
      {
        isAssert = !isAssert;
      }
      romAsyncFileManager->SetMatchCheckAnnounceAssert(isAssert);
    }
    break;
  case ITEM_INDEX_MATCH_CHECK_OUTPUT_REMAINING:
    {
      if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
      {
        romAsyncFileManager->OutputMatchCheckRemainingReq();
      }
    }
    break;
  }
#endif
}

wchar_t* DebugWin_DrawFsProg( void* userWork  , gfl2::debug::DebugWinItem *item )
{
#if GFL_DEBUGWIN_USE
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"" );

  enum
  {
    ITEM_INDEX_MATCH_CHECK_FLAG             = 0,
    ITEM_INDEX_MATCH_CHECK_CLEAR_REMAINING  = 1,
    ITEM_INDEX_MATCH_CHECK_ANNOUNCE_ASSERT  = 2,
    ITEM_INDEX_MATCH_CHECK_OUTPUT_REMAINING = 3,
  };
  int itemIndex = reinterpret_cast<int>(userWork);

  GameSys::GameManager *gMan = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  gfl2::fs::AsyncFileManager* romAsyncFileManager = gMan->GetAsyncFileManager();

  switch(itemIndex)
  {
  case ITEM_INDEX_MATCH_CHECK_FLAG:
    {
      bool flag = false;
      flag = romAsyncFileManager->GetMatchCheckFlag();
      swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN,
        L"%ls [%d]",
        item->GetName(), flag?(1):(0) );
    }
    break;
  case ITEM_INDEX_MATCH_CHECK_CLEAR_REMAINING:
    {
      swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN,
        L"%ls",
        item->GetName() );
    }
    break;
  case ITEM_INDEX_MATCH_CHECK_ANNOUNCE_ASSERT:
    {
      bool isAssert = romAsyncFileManager->IsMatchCheckAnnounceAssert();
      swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN,
        L"%ls [%d]",
        item->GetName(), isAssert?(1):(0) );
    }
    break;
  case ITEM_INDEX_MATCH_CHECK_OUTPUT_REMAINING:
    {
      swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN,
        L"%ls",
        item->GetName() );
    }
    break;
  }

  return workStr;
#else
  return NULL;
#endif
}



void DebugWin_UpdateLyt( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  GFL_UNUSED(item);

#if GFL_DEBUGWIN_USE
  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();

  enum
  {
    ITEM_INDEX_DRAWING_CHECK_FLAG = 0,
  };
  int itemIndex = reinterpret_cast<int>(userWork);

  GameSys::GameManager *gMan = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
//  gfl2::fs::AsyncFileManager* romAsyncFileManager = gMan->GetAsyncFileManager();

  switch(itemIndex)
  {
  case ITEM_INDEX_DRAWING_CHECK_FLAG:
    {
      bool isEnabled = gMan->IsDrawingLytWkManagerAdditionEnabled();
      if( uiBtn->IsTrigger( gfl2::ui::BUTTON_LEFT ) || uiBtn->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
      {
        isEnabled = !isEnabled;
      }
      gMan->SetDrawingLytWkManagerAdditionEnabled(isEnabled);
    }
    break;
  }
#endif
}
wchar_t* DebugWin_DrawLyt( void* userWork  , gfl2::debug::DebugWinItem *item )
{
#if GFL_DEBUGWIN_USE
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"" );

  enum
  {
    ITEM_INDEX_DRAWING_CHECK_FLAG = 0,
  };
  int itemIndex = reinterpret_cast<int>(userWork);

  GameSys::GameManager *gMan = GFL_SINGLETON_INSTANCE(GameSys::GameManager);

  switch(itemIndex)
  {
  case ITEM_INDEX_DRAWING_CHECK_FLAG:
    {
      bool isEnabled = gMan->IsDrawingLytWkManagerAdditionEnabled();
      swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN,
        L"%ls [%d]",
        item->GetName(), isEnabled?(1):(0) );
    }
    break;
  }

  return workStr;
#else
  return NULL;
#endif
}

wchar_t* DebugWin_DrawComBuf( void* userWork  , gfl2::debug::DebugWinItem *item )
{
#if GFL_DEBUGWIN_USE
  int type = reinterpret_cast<int>(userWork);
  DebugWinWork *work = GFL_SINGLETON_INSTANCE(DebugWinWork);

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr , gfl2::debug::DEBUGWIN_NAME_LEN
    ,L"%ls [%6d / %6d]" ,item->GetName()
    ,work->mCommandBufferUseSize
    ,work->mCommandBufferMaxSize );

  return workStr;
#else
  return NULL;
#endif
}

wchar_t* DebugWin_DrawComReq( void* userWork  , gfl2::debug::DebugWinItem *item )
{
#if GFL_DEBUGWIN_USE
  int type = reinterpret_cast<int>(userWork);
  DebugWinWork *work = GFL_SINGLETON_INSTANCE(DebugWinWork);

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr , gfl2::debug::DEBUGWIN_NAME_LEN
    ,L"%ls [%6d / %6d]" ,item->GetName()
    ,work->mCommandRequestCount
    ,work->mCommandRequestMax );

  return workStr;
#else
  return NULL;
#endif
}


#if GFL_ENABLE_DYNAMIC_HEADER

wchar_t* DebugWin_DynamicHeaderChangeEnableDraw( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  GFL_UNUSED(userWork);

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"" );

  gfl2::str::StrBuf* workStrBuf = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetWorkStrBuf();
  DebugWinWork *debWork = GFL_SINGLETON_INSTANCE(DebugWinWork);
  GFL_ASSERT( debWork );
  gfl2::str::MsgData * pMsgData = debWork->GetMessageData();
  
  bool isEnable = gfl2::debug::DynamicHeader::HeaderHashManager::GetIsEnable();

  pMsgData->GetString( isEnable ? msg_dmenu_dynaheader_state_ena : msg_dmenu_dynaheader_state_dis, *workStrBuf );

  swprintf(
    workStr,
    gfl2::debug::DEBUGWIN_NAME_LEN,
    L"%ls [%ls]",
    item->GetName(),
    workStrBuf->GetPtr()
  );

  return workStr;
}

void DebugWin_DynamicHeaderChangeEnableUpdate(void* userWork, gfl2::debug::DebugWinItem *item)
{
  GFL_UNUSED(userWork);
  GFL_UNUSED(item);

  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();

  bool isEnable = gfl2::debug::DynamicHeader::HeaderHashManager::GetIsEnable();
  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_LEFT ) || uiBtn->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    isEnable = !isEnable;
  }
  gfl2::debug::DynamicHeader::HeaderHashManager::SetIsEnable(isEnable);
}

void DebugWin_AddDynamicHeaderWin( gfl2::heap::HeapBase* heap, gfl2::debug::DebugWinGroup* parent )
{
  DebugWinWork *debWork = GFL_SINGLETON_INSTANCE(DebugWinWork);
  GFL_ASSERT( debWork );
  gfl2::str::MsgData * pMsgData = debWork->GetMessageData();
  gfl2::debug::DebugWinGroup *dynaHeaderGroup = gfl2::debug::DebugWin_AddGroup( heap, pMsgData, msg_dmenu_dynaheader_top, parent );  
  gfl2::debug::DebugWin_AddItemUpdateFunc( dynaHeaderGroup, heap , pMsgData, msg_dmenu_dynaheader_state_top, NULL, DebugWin_DynamicHeaderChangeEnableUpdate , DebugWin_DynamicHeaderChangeEnableDraw ); 
}

#endif //GFL_ENABLE_DYNAMIC_HEADER

#if defined(GFX_CTR_PROFILING_ENABLED)
//----------------------------------------------------------------------------
/**
  *   @brief Orcsファイル保存関数
  *
  *   @param userWork         ワークポインタ
  *   @param item             デバッグウィンドウ項目
*/
//-----------------------------------------------------------------------------
static void DebugWin_SaveOrcsFile( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  GFL_UNUSED(item);
  GFL_UNUSED(userWork);

  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();
  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    gfl2::gfx::GFGL::ExportOrcsFileRequest("gpu_profiler.orcs"); 
  }
}
#endif

void DebugWin_UpdateGpuProfile( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  GFL_UNUSED(item);
  GFL_UNUSED(userWork);
  //int type = reinterpret_cast<int>(userWork);
  //DebugWinWork *work = GFL_SINGLETON_INSTANCE(DebugWinWork);
}

wchar_t* DebugWin_DrawGpuProfile( void* userWork  , gfl2::debug::DebugWinItem *item )
{
#if GFL_DEBUGWIN_USE

#if defined(GFX_CTR_PROFILING_ENABLED)
  gfl2::gfx::ctr::CTRGPUProfiler::ProfilingResult result = gfl2::gfx::ctr::CTRGL::GetGPUProfilingResult();
  int index = reinterpret_cast<int>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr , gfl2::debug::DEBUGWIN_NAME_LEN,
    L"%ls [%u]" ,item->GetName(), result.values[index] );
         //↑%dだとwarningが出た。unsignedは%uがただしいらしい
  return workStr;
#else
  return NULL;
#endif // GFX_CTR_PROFILING_ENABLED

#else // GFL_DEBUGWIN_USE

  return NULL;

#endif // GFL_DEBUGWIN_USE
}

void DebugWin_AddGpuProfileWin( gfl2::heap::HeapBase* heap, gfl2::debug::DebugWinGroup* parent )
{
#if defined(GFX_CTR_PROFILING_ENABLED)
  DebugWinWork *debWork = GFL_SINGLETON_INSTANCE(DebugWinWork);
  GFL_ASSERT( debWork );
  gfl2::str::MsgData* pMsgData = debWork->GetMessageData();

  gfl2::debug::DebugWinGroup *gpuProfileGroup                     = gfl2::debug::DebugWin_AddGroup( heap , pMsgData, msg_dmenu_gpu_prof_top, parent ); // GPUProfileルートグループ
  gfl2::debug::DebugWinGroup *gpuProfileVertexShader01Group       = gfl2::debug::DebugWin_AddGroup( heap , pMsgData, msg_dmenu_gpu_prof_vtx_shader01_top, gpuProfileGroup );
  gfl2::debug::DebugWinGroup *gpuProfileVertexShader23Group       = gfl2::debug::DebugWin_AddGroup( heap , pMsgData, msg_dmenu_gpu_prof_vtx_shader23_top, gpuProfileGroup );
  gfl2::debug::DebugWinGroup *gpuProfileBusyPolygonFragmentGroup  = gfl2::debug::DebugWin_AddGroup( heap , pMsgData, msg_dmenu_gpu_prof_poly_frag_top   , gpuProfileGroup );
  gfl2::debug::DebugWinGroup *gpuProfileMemoryAccessGroup         = gfl2::debug::DebugWin_AddGroup( heap , pMsgData, msg_dmenu_gpu_prof_mem_access_top  , gpuProfileGroup );

#if defined(GFX_CTR_PROFILING_ENABLED)
  // 3DS_GPU_Debugger用orcsファイル出力
  {
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileGroup , heap , L"Save Orcs File" , NULL , DebugWin_SaveOrcsFile );
  }
#endif
  
  // VertexShader0-1グループ
  {
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader01Group , heap , pMsgData, msg_dmenu_gpu_prof_vtx_shader01_vtx0_pc    , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX0_PROGCOUNTER)      , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader01Group , heap , pMsgData, msg_dmenu_gpu_prof_vtx_shader01_vtx0_cmd   , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX0_CMD_STOLE)        , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader01Group , heap , pMsgData, msg_dmenu_gpu_prof_vtx_shader01_vtx0_mova  , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX0_MOVA_CMD_STOLE)   , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader01Group , heap , pMsgData, msg_dmenu_gpu_prof_vtx_shader01_vtx0_cmp   , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX0_CMP_CMD_STOLE)    , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader01Group , heap , pMsgData, msg_dmenu_gpu_prof_vtx_shader01_vtx0_fetch , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX0_FETCH_MISS_STOLE) , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );

    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader01Group , heap , pMsgData, msg_dmenu_gpu_prof_vtx_shader01_vtx1_pc    , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX1_PROGCOUNTER)      , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader01Group , heap , pMsgData, msg_dmenu_gpu_prof_vtx_shader01_vtx1_cmd   , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX1_CMD_STOLE)        , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader01Group , heap , pMsgData, msg_dmenu_gpu_prof_vtx_shader01_vtx1_mova  , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX1_MOVA_CMD_STOLE)   , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader01Group , heap , pMsgData, msg_dmenu_gpu_prof_vtx_shader01_vtx1_cmp   , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX1_CMP_CMD_STOLE)    , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader01Group , heap , pMsgData, msg_dmenu_gpu_prof_vtx_shader01_vtx1_fetch , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX1_FETCH_MISS_STOLE) , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
  }

  // VertexShader2-3グループ
  {
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader23Group , heap , pMsgData, msg_dmenu_gpu_prof_vtx_shader23_vtx2_pc    , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX2_PROGCOUNTER)      , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader23Group , heap , pMsgData, msg_dmenu_gpu_prof_vtx_shader23_vtx2_cmd   , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX2_CMD_STOLE)        , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader23Group , heap , pMsgData, msg_dmenu_gpu_prof_vtx_shader23_vtx2_mova  , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX2_MOVA_CMD_STOLE)   , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader23Group , heap , pMsgData, msg_dmenu_gpu_prof_vtx_shader23_vtx2_cmp   , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX2_CMP_CMD_STOLE)    , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader23Group , heap , pMsgData, msg_dmenu_gpu_prof_vtx_shader23_vtx2_fetch , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX2_FETCH_MISS_STOLE) , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );

    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader23Group , heap , pMsgData, msg_dmenu_gpu_prof_vtx_shader23_vtx3_pc    , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX3_PROGCOUNTER)      , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader23Group , heap , pMsgData, msg_dmenu_gpu_prof_vtx_shader23_vtx3_cmd   , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX3_CMD_STOLE)        , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader23Group , heap , pMsgData, msg_dmenu_gpu_prof_vtx_shader23_vtx3_mova  , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX3_MOVA_CMD_STOLE)   , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader23Group , heap , pMsgData, msg_dmenu_gpu_prof_vtx_shader23_vtx3_cmp   , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX3_CMP_CMD_STOLE)    , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader23Group , heap , pMsgData, msg_dmenu_gpu_prof_vtx_shader23_vtx3_fetch , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX3_FETCH_MISS_STOLE) , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
  }

  // BusyPolygonFragmentグループ
  {
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileBusyPolygonFragmentGroup , heap , pMsgData, msg_dmenu_gpu_prof_poly_frag_shader0    , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::BUSY_SHADER0)             , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileBusyPolygonFragmentGroup , heap , pMsgData, msg_dmenu_gpu_prof_poly_frag_vtx_load   , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::BUSY_COMMAND_VERTEX_LOAD) , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileBusyPolygonFragmentGroup , heap , pMsgData, msg_dmenu_gpu_prof_poly_frag_rasterize  , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::BUSY_RASTERIZE)           , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileBusyPolygonFragmentGroup , heap , pMsgData, msg_dmenu_gpu_prof_poly_frag_triangle   , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::BUSY_TRIANGLE_SETUP)      , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileBusyPolygonFragmentGroup , heap , pMsgData, msg_dmenu_gpu_prof_poly_frag_fragment   , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::BUSY_FRAGMENT)            , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileBusyPolygonFragmentGroup , heap , pMsgData, msg_dmenu_gpu_prof_poly_frag_tex_unit   , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::BUSY_TEXTURE_UNIT)        , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileBusyPolygonFragmentGroup , heap , pMsgData, msg_dmenu_gpu_prof_poly_frag_perfragment, reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::BUSY_PERFRAGMENT)         , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileBusyPolygonFragmentGroup , heap , pMsgData, msg_dmenu_gpu_prof_poly_frag_tex_comb   , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::BUSY_TEXTURE_COMB)        , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );

    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileBusyPolygonFragmentGroup , heap , pMsgData, msg_dmenu_gpu_prof_poly_frag_input_vtx  , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::POLYGON_INPUT_VERTEX)   , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileBusyPolygonFragmentGroup , heap , pMsgData, msg_dmenu_gpu_prof_poly_frag_input_poly , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::POLYGON_INPUT_POLYGON)  , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileBusyPolygonFragmentGroup , heap , pMsgData, msg_dmenu_gpu_prof_poly_frag_output_poly, reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::POLYGON_OUTPUT_POLYGON) , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );

    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileBusyPolygonFragmentGroup , heap , pMsgData, msg_dmenu_gpu_prof_poly_frag_input_fragment, reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::FRAGMENT_INPUT_FRAGMENT) , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
  }

  // MemoryAccessグループ
  {
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileMemoryAccessGroup , heap , pMsgData, msg_dmenu_gpu_prof_mem_access_vrama_read                   , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::ACCESS_VRAMA_READ)         , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileMemoryAccessGroup , heap , pMsgData, msg_dmenu_gpu_prof_mem_access_vrama_write                    , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::ACCESS_VRAMA_WRITE)        , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileMemoryAccessGroup , heap , pMsgData, msg_dmenu_gpu_prof_mem_access_vramb_read                   , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::ACCESS_VRAMB_READ)         , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileMemoryAccessGroup , heap , pMsgData, msg_dmenu_gpu_prof_mem_access_vramb_write              , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::ACCESS_VRAMB_WRITE)        , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileMemoryAccessGroup , heap , pMsgData, msg_dmenu_gpu_prof_mem_access_cmd_vertex     , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::ACCESS_CMD_VERTEX)         , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileMemoryAccessGroup , heap , pMsgData, msg_dmenu_gpu_prof_mem_access_tex_unit       , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::ACCESS_TEX_UNIT)           , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileMemoryAccessGroup , heap , pMsgData, msg_dmenu_gpu_prof_mem_access_depth_read     , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::ACCESS_DEPTH_READ)         , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileMemoryAccessGroup , heap , pMsgData, msg_dmenu_gpu_prof_mem_access_depth_write    , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::ACCESS_DEPTH_WRITE)        , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileMemoryAccessGroup , heap , pMsgData, msg_dmenu_gpu_prof_mem_access_color_read     , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::ACCESS_COLOR_READ)         , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileMemoryAccessGroup , heap , pMsgData, msg_dmenu_gpu_prof_mem_access_color_write    , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::ACCESS_COLOR_WRITE)        , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileMemoryAccessGroup , heap , pMsgData, msg_dmenu_gpu_prof_mem_access_lcd_up_read    , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::ACCESS_LCD_UP_DISP_READ)   , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileMemoryAccessGroup , heap , pMsgData, msg_dmenu_gpu_prof_mem_access_lcd_down_write , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::ACCESS_LCD_DOWN_DISP_READ) , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileMemoryAccessGroup , heap , pMsgData, msg_dmenu_gpu_prof_mem_access_post_read      , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::ACCESS_POST_READ)          , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileMemoryAccessGroup , heap , pMsgData, msg_dmenu_gpu_prof_mem_access_post_write     , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::ACCESS_POST_WRITE)         , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileMemoryAccessGroup , heap , pMsgData, msg_dmenu_gpu_prof_mem_access_memfill0_write , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::ACCESS_MEMFILL0_WRITE)     , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileMemoryAccessGroup , heap , pMsgData, msg_dmenu_gpu_prof_mem_access_memfill1_write , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::ACCESS_MEMFILL1_WRITE)     , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileMemoryAccessGroup , heap , pMsgData, msg_dmenu_gpu_prof_mem_access_read_pixel     , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::ACCESS_READPIXCEL)         , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileMemoryAccessGroup , heap , pMsgData, msg_dmenu_gpu_prof_mem_access_dma_vram_write , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::ACCESS_DMA_VRAM_WRITE)     , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
  }

#endif
}

// GameManagerTickが開かれたときの処理
void OpenGroupFunc_GameManagerTick( void *work )
{
#if GFL_DEBUGWIN_USE
  GAME_MANAGER_DEBUG_TICK_SET_FLAG( true );
  GAME_MANAGER_DEBUG_TICK_RESET();
#endif // #if GFL_DEBUGWIN_USE
}

void DebugWin_UpdateGameManagerTickReset( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  GFL_UNUSED(item);
  GFL_UNUSED(userWork);

#if GFL_DEBUGWIN_USE
  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();
  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    GAME_MANAGER_DEBUG_TICK_RESET();
  }
#endif
}

wchar_t* DebugWin_DrawGameManagerTickName( void* userWork  , gfl2::debug::DebugWinItem *item )
{
#if GFL_DEBUGWIN_USE
  u32 index = reinterpret_cast<u32>(userWork);
  DebugWinWork *debWork = GFL_SINGLETON_INSTANCE(DebugWinWork);
  GFL_ASSERT( debWork );
  gfl2::debug::DebugWin* devWin = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin);

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  gfl2::str::StrBuf* strNow = debWork->GetMessageStrBuf( DebugWinWork::DEBUG_WIN_STR_IDX_NOW );
  gfl2::str::StrBuf* strAve = debWork->GetMessageStrBuf( DebugWinWork::DEBUG_WIN_STR_IDX_AVE );
  
  swprintf( workStr , gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls %ls[%6d] %ls[%6d]",
    item->GetName(),
    strNow->GetPtr(),
    GAME_MANAGER_DEBUG_TICK_US_GET(index),
    strAve->GetPtr(),
    GAME_MANAGER_DEBUG_TICK_US_AVE_GET(index)
    );

  return workStr;

#else // GFL_DEBUGWIN_USE
  return NULL;
#endif // GFL_DEBUGWIN_USE
}

wchar_t* DebugWin_DrawGameManagerTickValue( void* userWork  , gfl2::debug::DebugWinItem *item )
{
#if GFL_DEBUGWIN_USE

  u32 index = reinterpret_cast<u32>(userWork);
  DebugWinWork *debWork = GFL_SINGLETON_INSTANCE(DebugWinWork);
  GFL_ASSERT( debWork );
  gfl2::debug::DebugWin* devWin = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin);

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  gfl2::str::StrBuf* strNow = debWork->GetMessageStrBuf( DebugWinWork::DEBUG_WIN_STR_IDX_NOW );
  gfl2::str::StrBuf* strAve = debWork->GetMessageStrBuf( DebugWinWork::DEBUG_WIN_STR_IDX_AVE );
  gfl2::str::StrBuf* strMax = debWork->GetMessageStrBuf( DebugWinWork::DEBUG_WIN_STR_IDX_MAX );

  swprintf( workStr , gfl2::debug::DEBUGWIN_NAME_LEN,
    L" %ls[%6d] %ls[%6d] %ls[%6d]",
    strNow->GetPtr(),
    GAME_MANAGER_DEBUG_TICK_US_GET(index),
    strAve->GetPtr(),
    GAME_MANAGER_DEBUG_TICK_US_AVE_GET(index),
    strMax->GetPtr(),
    GAME_MANAGER_DEBUG_TICK_US_MAX_GET(index)
    );

  return workStr;

#else // GFL_DEBUGWIN_USE
  return NULL;
#endif // GFL_DEBUGWIN_USE
}

void DebugWin_AddDrawGameManagerTick( gfl2::heap::HeapBase* heap, gfl2::debug::DebugWinGroup* parent )
{
  DebugWinWork *debWork = GFL_SINGLETON_INSTANCE(DebugWinWork);
  GFL_ASSERT( debWork );
  gfl2::str::MsgData * pMsgData = debWork->GetMessageData();

  gfl2::debug::DebugWinGroup *groupTop = gfl2::debug::DebugWin_AddGroup( heap , pMsgData, msg_dmenu_gmmgr_tick_top, parent );
  groupTop->SetOpenGroupFunc( OpenGroupFunc_GameManagerTick );

  for( u32 i = 0; i < GameSys::GameManager::TickDefine::MAX; ++i)
  {
    const u32 WORK_STR_LENGTH = 64;
    wchar_t workStr[WORK_STR_LENGTH];
#if defined(GF_PLATFORM_CTR)
    swprintf( workStr , WORK_STR_LENGTH, L"%s" , GameSys::GameManager::GetTickLabel( static_cast<GameSys::GameManager::TickDefine::Index>( i)));
#elif defined(GF_PLATFORM_WIN32)
    size_t ret;
    mbstowcs_s( &ret, workStr, WORK_STR_LENGTH, GameSys::GameManager::GetTickLabel( static_cast<GameSys::GameManager::TickDefine::Index>( i)), _TRUNCATE);
    swprintf( workStr , WORK_STR_LENGTH, L"%s" , workStr);
#endif

    // ラベル名表示
    gfl2::debug::DebugWin_AddItemUpdateFunc( 
      groupTop,
      heap,
      workStr,
      reinterpret_cast<void*>( i), 
      NULL,
      DebugWin_DrawGameManagerTickName
      );
#if 0
    // チック表示
    gfl2::debug::DebugWin_AddItemUpdateFunc( 
      groupTop,
      heap,
      workStr,
      reinterpret_cast<void*>( i), 
      NULL,
      DebugWin_DrawGameManagerTickValue
      );
#endif
  }

  // リセット
  gfl2::debug::DebugWin_AddItemUpdateFunc( 
    groupTop,
    heap,
    pMsgData,
    msg_dmenu_gmmgr_tick_reset,
    NULL,
    DebugWin_UpdateGameManagerTickReset,
    NULL
    );
}

//---------------------------------------------------------------------------
//デバッグウィンドウワーク
DebugWinWork::DebugWinWork()
  : mResetRequest(DEBUGWIN_TICK_MAX)
  , mIsDeviceTime(true)
  , mIsGetTime( false )
  , mTimeSpeed( 1 )
  , mIsKeyReleaseForKeyRecode( false )
  , mPlayerIDUpper( 0 )
  , mPlayerIDLower( 0 )
  , m_pNetDebug( NULL )
  , mTextSpeedWaitFrame(0)
  , mTextSpeedPutCount(1)
{
  mIs60Mode = false;
  TickReset();
}

DebugWinWork::~DebugWinWork()
{
  GFL_SAFE_DELETE(m_pNetDebug);
}

void DebugWinWork::TickReset(void)
{
  for( int i=0;i<DEBUGWIN_TICK_MAX;i++ )
  {
    this->TickReset(i);
  }
}
void DebugWinWork::TickReset( int tickType )
{
  mTickStart[tickType] = 0;
  mTickNow[tickType] = 0;
  mTickMax[tickType] = 0;
  for( int j=0;j<TICK_AVE_NUM;j++ )
  {
    mTickAve[tickType][j] = 0;
  }
  mTickAveCnt[tickType] = 0;
}

void DebugWinWork::TickResetRequest( int tickType )
{
  mResetRequest = tickType;
}

void DebugWinWork::TickStart( int tickType )
{
  if( mResetRequest == tickType )
  {
    this->TickReset( tickType );
    mResetRequest = DEBUGWIN_TICK_MAX;
  }

  mTickStart[tickType] = gfl2::system::Timer::PerformanceCounter::GetMicroSecond();
}

void DebugWinWork::TickEnd( int tickType )
{
  s64 end = gfl2::system::Timer::PerformanceCounter::GetMicroSecond();
  TickCalc(tickType,end - mTickStart[tickType]);
}
void DebugWinWork::TickCalc( int tickType , s64 microSec )
{
  mTickNow[tickType] = microSec;

  if( mTickNow[tickType] > mTickMax[tickType] )
  {
    mTickMax[tickType] = mTickNow[tickType];
  }

  mTickAve[tickType][mTickAveCnt[tickType]] = mTickNow[tickType];
  mTickAveCnt[tickType]++;

  if( mTickAveCnt[tickType] >= TICK_AVE_NUM )
  {
    mTickAveCnt[tickType] = 0;
  }
}

s64 DebugWinWork::GetTickCalcAve( int tickType )
{
  s64 ave = 0;
  for( int i=0;i<DebugWinWork::TICK_AVE_NUM;i++ )
  {
    ave += GetTickAve(tickType,i);
  }
  ave /= DebugWinWork::TICK_AVE_NUM;
  return ave;
}



// Strbufを生成
void DebugWinWork::InitializeMessageStrBuf( gfl2::heap::HeapBase * pHeap )
{
  static const u32 sc_DebugStr[] = 
  {
    msg_dmenu_tick_now,
    msg_dmenu_tick_ave,
    msg_dmenu_tick_max,
    msg_dmenu_tick_mode,
    msg_dmenu_tick_frame
  };
  GFL_ASSERT( GFL_NELEMS(sc_DebugStr) == DEBUG_WIN_STR_IDX_NUM );

  for( u32 i=0; i<DEBUG_WIN_STR_IDX_NUM; ++i )
  {
    mpStrTick[i] = GFL_NEW( pHeap ) gfl2::str::StrBuf( gfl2::debug::DEBUGWIN_NAME_LEN, pHeap );
    mpMsgData->GetString( sc_DebugStr[i], *mpStrTick[i] );
  }
}

// Strbufを破棄
void DebugWinWork::FinalizeMessageStrBuf( void )
{
  for( u32 i=0; i<DEBUG_WIN_STR_IDX_NUM; ++i )
  {
    GFL_SAFE_DELETE( mpStrTick[i] );
  }
}

// StrBufの取得
gfl2::str::StrBuf * DebugWinWork::GetMessageStrBuf( StrIndex index ) const
{
  if( index < DEBUG_WIN_STR_IDX_NUM )
  {
    return mpStrTick[ index ];
  }

  GFL_ASSERT( index < DEBUG_WIN_STR_IDX_NUM );
  return mpStrTick[ 0 ];
}



//------------------------------------------------------------------------------
/**
 * @func        KujiraDebgWin_OpenTimeGroup
 * @brief       KujiraDebgWin開くTimeGroup関数.
 * @author      N.Takeda
 * @date        2015/06/25(木) 19:13:44
 *
 * @param[in,out] void* userWork  .
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void KujiraDebgWin_OpenTimeGroup( void *userWork )
{
#if GFL_DEBUGWIN_USE
  ::Debug::DebugWin::DebugWinWork *debWin = GFL_SINGLETON_INSTANCE(::Debug::DebugWin::DebugWinWork);
  if( debWin->mIsGetTime == false )
  {
    debWin->mIsGetTime = true;
    KujiraDebgWin_GetTime( userWork , NULL );
  }
#endif
}



//------------------------------------------------------------------------------
/**
 * @func        KujiraDebgWin_GetTime
 * @brief       KujiraDebgWin_GetTime関数.
 * @author      N.Takeda
 * @date        2015/06/25(木) 18:37:00
 *
 * @param[in,out] void*                       userWork  .
 * @param[in,out] gfl2::debug::DebugWinItem*  item      .
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void KujiraDebgWin_GetTime( void* userWork  , gfl2::debug::DebugWinItem *item )
{
#if GFL_DEBUGWIN_USE
  //gfl::debug::DebugWinItem *item 使うな！！
  ::Debug::DebugWin::DebugWinWork *debWin = GFL_SINGLETON_INSTANCE(::Debug::DebugWin::DebugWinWork);


  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();
  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    GameSys::AlolaTime alola_time;
    GameSys::DeviceDate device_date;
    GameSys::INijiDate * pNijiDate;

    alola_time.Initialize();

    if(debWin->mIsDeviceTime)
    {
      pNijiDate = &device_date;
    }
    else
    {
      pNijiDate = &alola_time;
    }

    debWin->mTimeYear  = device_date.GetYear();
    debWin->mTimeMonth = device_date.GetMonth();
    debWin->mTimeDay   = device_date.GetDay();
    debWin->mTimeHour  = pNijiDate->GetHour();
    debWin->mTimeMin   = pNijiDate->GetMinute();
    debWin->mTimeSec   = pNijiDate->GetSecond();
  }
#endif
}


//------------------------------------------------------------------------------
/**
 * @func        KujiraDebgWin_SetTime
 * @brief       KujiraDebgWin_SetTime関数.
 * @author      N.Takeda
 * @date        2015/06/25(木) 18:36:57
 *
 * @param[in,out] void*                       userWork  .
 * @param[in,out] gfl2::debug::DebugWinItem*  item      .
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void KujiraDebgWin_SetTime( void* userWork  , gfl2::debug::DebugWinItem *item )
{
#if GFL_DEBUGWIN_USE
  ::Debug::DebugWin::DebugWinWork *debWin = GFL_SINGLETON_INSTANCE(::Debug::DebugWin::DebugWinWork);

  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();
  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    gfl2::system::Date  time;

    time.SetYear(debWin->mTimeYear);
    time.SetMonth(debWin->mTimeMonth);
    time.SetDay(debWin->mTimeDay);
    time.SetHour(debWin->mTimeHour);
    time.SetMinute(debWin->mTimeMin);
    time.SetSecond(debWin->mTimeSec);

    gfl2::system::Date::FixFakeDate( &time );
    if(!debWin->mIsDeviceTime)
    {
      GameSys::AlolaTime alola_time;
      alola_time.Initialize();
      alola_time.FixFakeTime( debWin->mTimeHour, debWin->mTimeMin );
    }
    
    u32 rate = gfl2::system::Date::GetFakeRate();
    gfl2::system::Date::StartFakeTime( rate );
  }
#endif
}


//------------------------------------------------------------------------------
/**
 * @func        KujiraDebgWin_SetTimeSpeed
 * @brief       KujiraDebgWin_SetTimeSpeed関数.
 * @author      N.Takeda
 * @date        2015/06/25(木) 18:36:55
 *
 * @param[in,out] void*                       userWork  .
 * @param[in,out] gfl2::debug::DebugWinItem*  item      .
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void KujiraDebgWin_SetTimeSpeed( void* userWork  , gfl2::debug::DebugWinItem *item )
{
#if GFL_DEBUGWIN_USE
  ::Debug::DebugWin::DebugWinWork *debWin = GFL_SINGLETON_INSTANCE(::Debug::DebugWin::DebugWinWork);

  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();
  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    gfl2::system::Date::StartFakeTime( debWin->mTimeSpeed );
  }
#endif
}

//------------------------------------------------------------------------------
/**
 * @func        KujiraDebgWin_ResetTime
 * @brief       KujiraDebgWin_ResetTime関数.
 * @author      N.Takeda
 * @date        2015/06/25(木) 18:36:53
 *
 * @param[in,out] void*                       userWork  .
 * @param[in,out] gfl2::debug::DebugWinItem*  item      .
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void KujiraDebgWin_ResetTime( void* userWork  , gfl2::debug::DebugWinItem *item )
{
#if GFL_DEBUGWIN_USE
  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();
  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    gfl2::system::Date::StopFakeTime();
  }
#endif
}

void DebgWinFunc_SetVersoinSapphire( void* userWork, gfl2::debug::DebugWinItem *item )
{
  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();
  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    item->PlaySystemSound(gfl2::debug::SE_ACCEPT);
  System::SetVersion( VERSION_SUN2 );
  }
}

void DebgWinFunc_SetVersoinRuby( void* userWork, gfl2::debug::DebugWinItem *item )
{
  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();
  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    item->PlaySystemSound(gfl2::debug::SE_ACCEPT);
  System::SetVersion( VERSION_MOON2 );
  }
}

void DebgWinFunc_ResetVersoin( void* userWork, gfl2::debug::DebugWinItem *item )
{
  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();
  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    item->PlaySystemSound(gfl2::debug::SE_ACCEPT);
  System::SetVersion( PM_VERSION );
  }
}

//  きせかえ用のコールバッグ

//  初期所持
void DebugWin_UpdateKisekaeFirstHave( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();
  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    //  きせかえ全所持
    Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();
    Savedata::Fashion* pFashionSaveData = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFashion();

    //  いったんすべてきせかえクリアする
    {
      pFashionSaveData->Clear( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE ) );
      //  デフォルト所持
      pFashionSaveData->InitializeDefaultFashionItem( ( pMyStatus->GetSex() == poke_3d::model::DressUpParam::SEX_FEMALE ) );
    }
  }
}

//  全所持
void DebugWin_UpdateKisekaeAllHave( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();
  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    //  きせかえ全所持
    Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();
    Savedata::Fashion* pFashionSaveData = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFashion();

    //  いったんすべてきせかえクリアする
    {
      pFashionSaveData->Clear( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE ) );
      //  デフォルト所持
      pFashionSaveData->InitializeDefaultFashionItem( ( pMyStatus->GetSex() == poke_3d::model::DressUpParam::SEX_FEMALE ) );
    }

    poke_3d::model::DressUpParam::Sex sex = (poke_3d::model::DressUpParam::Sex)pMyStatus->GetSex();

    gfl2::ro::RoManager* pRoMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
    gfl2::ro::Module* pModule = NULL;

    // @fix GFNMcat[2107] ジョインフェスタではKisekaeDataのDLLは読み込み済み
    if( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->IsNowJoinFestaFieldData() == false )
    {
      pModule = pRoMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "KisekaeData.cro");
      pRoMgr->StartModule( pModule, true );
    }

    for( u32 category_idx = 0; category_idx < Savedata::Fashion::CATEGORY_MAX; ++category_idx )
    {
      u32 goodsMax  = app::util::Kisekae::KisekaeDataBase::GetGoodsDataNum( (Savedata::Fashion::Category)category_idx );
      for( u32 goods_idx = 0; goods_idx < goodsMax; ++goods_idx )
      {
        const table_data_GoodsData* pGoodsData  = app::util::Kisekae::KisekaeDataBase::GetGoodsData( (Savedata::Fashion::Category)category_idx, goods_idx );
        if( pGoodsData->sex != sex )
        {
          continue;
        }

        if( pGoodsData->color_change == 1 )
        {
          //  色ではなく色テキストで判定
          //  ホワイトとライトグレーは追加
          if( ( pGoodsData->color_txt_id != kisekae_color_01_01 )
          &&  ( pGoodsData->color_txt_id != kisekae_color_06_03 ) )
          {
            continue;
          }
        }

        //  セーブに追加
        pFashionSaveData->Add( category_idx, pGoodsData->item_id );
      }
    }

    // @fix GFNMcat[2107] ジョインフェスタではKisekaeDataのDLLは読み込み済み
    if( pModule != NULL )
    {
      pRoMgr->DisposeModule( pModule );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @func        BGM関連
 * @author      Miyuki Iwasawa
 */
//------------------------------------------------------------------------------

void DebugWin_UpdateBGMSwitch( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  GFL_UNUSED(item);

#if GFL_DEBUGWIN_USE
  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();
  GameSys::GameManager *gMan = GFL_SINGLETON_INSTANCE(GameSys::GameManager);

  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    if( Sound::IsBGMControlEnable() )
    {
      Sound::PauseBGM(0);
      Sound::SetBGMControlEnable(false);
      Sound::PlaySE( SEQ_SE_KW_CANCEL );
    } 
    else
    {
      Sound::SetBGMControlEnable(true);
      Sound::RestartBGM(0);
      Sound::PlaySE( SEQ_SE_KW_AWAKE );
    }
  }
#endif
}
wchar_t* DebugWin_DrawBGMSwitch( void* userWork  , gfl2::debug::DebugWinItem *item )
{
#if GFL_DEBUGWIN_USE
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"" );

  bool isEnabled = Sound::IsBGMControlEnable();
  if (isEnabled)
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls [On]", item->GetName() );
  } 
  else
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls [Off]", item->GetName() );
  }
  

  return workStr;
#else
  return NULL;
#endif
}

void DebugWin_UpdateSoundOutputMode( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  GFL_UNUSED(item);

#if GFL_DEBUGWIN_USE
  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();
  GameSys::GameManager *gMan = GFL_SINGLETON_INSTANCE(GameSys::GameManager);

  bool trig = false;
  int mode = static_cast<int>(Sound::GetOutputMode());

  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
  {
    mode = (mode+2)%3;
    trig = true;
  } 
  else if( uiBtn->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    mode = (mode+1)%3;
    trig = true;
  }
  if( trig ){
    Sound::PlaySE( SEQ_SE_SELECT1 );
    switch(mode){
    case 0:
      Sound::SetOutputModeMono();
      break;
    case 1:
      Sound::SetOutputModeStereo();
      break;
    case 2:
      Sound::SetOutputModeSurround();
      break;
    }
  }
#endif
}
wchar_t* DebugWin_DrawBSoundOutputMode( void* userWork  , gfl2::debug::DebugWinItem *item )
{
#if GFL_DEBUGWIN_USE
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"" );

  gfl2::sound::SoundSystem::OutputMode mode = Sound::GetOutputMode();

  switch( mode ){
  case gfl2::sound::SoundSystem::OUTPUT_MODE_MONO:
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls [Mono]", item->GetName() );
    break;
  case gfl2::sound::SoundSystem::OUTPUT_MODE_STEREO:
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls [Stereo]", item->GetName() );
    break;
  default:
  case gfl2::sound::SoundSystem::OUTPUT_MODE_SURROUND:
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls [Surround]", item->GetName() );
    break;
  }

  return workStr;
#else
  return NULL;
#endif
}

void DebugWin_UpdateGflUsePublicRandDebug( void* userWork, gfl2::debug::DebugWinItem *item )
{
  // s32で操作する
  s32 targetWork = *System::GflUse::DebugGetRandDebugPointer();
  s32* target = &targetWork;
  s32 min = 0;
  s32 max = System::GflUse::RandDebug::MAX_EFFECTIVE_VAL;
  s32 step = 1;
  
  // 値変更
  {
    gfl2::ui::Button *uiButton = item->GetSysUiButton();
    gfl2::ui::VectorDevice *uiKey = item->GetSysUiCrossKey();
    
    if( uiButton->IsHold( gfl2::ui::BUTTON_R ) )
    {
      step *= 10;
    }
    if( uiButton->IsHold( gfl2::ui::BUTTON_L ) )
    {
      step *= 100;
    }
    
    if( uiKey->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
    {
      if( *target == min )
      {
        *target = max;
      }
      else
      if( *target - step < min )
      {
        *target = min;
      }
      else
      {
        *target -= step;
      }
      // item->Get->SoundCallBack(SE_CHANGE_VALUE);
    }
    else
    if( uiKey->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
    {
      if( *target == max )
      {
        *target = min;
      }
      else
      if( *target + step > max )
      {
        *target = max;
      }
      else
      {
        *target += step;
      }
      // debSys->SoundCallBack(SE_CHANGE_VALUE);
    }
  }

  // 値を反映
  *System::GflUse::DebugGetRandDebugPointer() = static_cast< System::GflUse::RandDebug::Value>( targetWork );
}

wchar_t* DebugWin_DrawGflUsePublicRandDebug( void* userWork  , gfl2::debug::DebugWinItem *item )
{
#if GFL_DEBUGWIN_USE
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"" );

  System::GflUse::RandDebug::Value val = *System::GflUse::DebugGetRandDebugPointer();

  gfl2::sound::SoundSystem::OutputMode mode = Sound::GetOutputMode();

  switch( val )
  {
  case System::GflUse::RandDebug::NONE:
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls [None]", item->GetName() );
    break;
  case System::GflUse::RandDebug::FIX_MAX:
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls [Fix_Max]", item->GetName() );
    break;
  case System::GflUse::RandDebug::FIX_MIN:
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls [Fix_Min]", item->GetName() );
    break;
  }

  return workStr;
#else
  return NULL;
#endif
}


void DebugWin_UpdateHeapClearSwitch( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  GFL_UNUSED(item);

#if GFL_DEBUGWIN_USE
  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();
  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    gfl2::heap::HeapBase::DebugFreeMemClearType clear_type = gfl2::heap::HeapBase::GetDebugFreeMemClear();
    switch ( clear_type )
    {
    case gfl2::heap::HeapBase::DEBUG_FREE_MEMC_NONE:     ///< クリアしない
      gfl2::heap::HeapBase::SetDebugFreeMemClear( gfl2::heap::HeapBase::DEBUG_FREE_MEMC_ZERO );
      break;
    case gfl2::heap::HeapBase::DEBUG_FREE_MEMC_ZERO:     ///< ゼロでクリア
      gfl2::heap::HeapBase::SetDebugFreeMemClear( gfl2::heap::HeapBase::DEBUG_FREE_MEMC_FF );
      break;
    case gfl2::heap::HeapBase::DEBUG_FREE_MEMC_FF:       ///< 0xFFでクリア
      gfl2::heap::HeapBase::SetDebugFreeMemClear( gfl2::heap::HeapBase::DEBUG_FREE_MEMC_NONE );
      break;
    } 
  }
#endif
}
wchar_t* DebugWin_DrawHeapClearSwitch( void* userWork , gfl2::debug::DebugWinItem *item )
{
#if GFL_DEBUGWIN_USE
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"" );
  gfl2::heap::HeapBase::DebugFreeMemClearType clear_type = gfl2::heap::HeapBase::GetDebugFreeMemClear();
  switch ( clear_type )
  {
  case gfl2::heap::HeapBase::DEBUG_FREE_MEMC_NONE:     ///< クリアしない
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls [none]", item->GetName() );
    break;
  case gfl2::heap::HeapBase::DEBUG_FREE_MEMC_ZERO:     ///< ゼロでクリア
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls [Ox00]", item->GetName() );
    break;
  case gfl2::heap::HeapBase::DEBUG_FREE_MEMC_FF:       ///< 0xFFでクリア
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls [0xff]", item->GetName() );
    break;
  } 
  return workStr;
#else
  return NULL;
#endif
}

//-------------------------------------------------------------------------------
//Heap関係
//-------------------------------------------------------------------------------
//通常ヒープのダンプ
void DebugWinFunc_Heap_UpdateAllDump( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  GFL_UNUSED(userWork);

  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();
  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    item->PlaySystemSound(gfl2::debug::SE_ACCEPT);

    GFL_RELEASE_PRINT("[ ID][-------------------------------HEAP NAME][TOTAL SIZE][FREE SIZE-][ALLOCATABLE][-------------CREATE FILE(LINE)]\n");
    for( int i=0;i<HEAPID_CHILD_MAX;i++ )
    {
      if( gfl2::heap::Manager::IsExistHeapId( i ) )
      {
        gfl2::heap::HeapBase *heap = gfl2::heap::Manager::GetHeapByHeapId(i);
        const int size = heap->GetTotalSize();
        const int free = heap->GetTotalFreeSize();
        const int allocate = heap->GetTotalAllocatableSize();
        const bool isFragment = ((free-allocate)>gfl2::heap::MEMORY_HEADER_SIZE);

        GFL_RELEASE_PRINT("[%3d][%40ls][0x%08x][0x%08x][ 0x%08x][%24s(%4d)]%s\n",i,heapNameArr[i],size,free,allocate,heap->GetCreateDataFilename(),heap->GetCreateDataLine(),(isFragment?"断片化":" "));

      }
    }
    GFL_RELEASE_PRINT("[ ID][-------------------------------HEAP NAME][TOTAL SIZE][FREE SIZE-][ALLOCATABLE][-------------CREATE FILE(LINE)]\n");
  }
}
//ローカルヒープのダンプ
void DebugWinFunc_Heap_UpdateAllDumpLocal( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  GFL_UNUSED(userWork);

  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();
  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    item->PlaySystemSound(gfl2::debug::SE_ACCEPT);

    GFL_RELEASE_PRINT("[ ID][-----------------------HEAP NAME][TOTAL SIZE][FREE SIZE-][ALLOCATABLE][-------------CREATE FILE(LINE)]\n");
    for( int i=0;i<HEAPID_LOCAL_MAX;i++ )
    {
      if( gfl2::heap::Manager::IsExistHeapId( GFL_HEAP_INDEX_TO_LOCAL_HEAP_ID(i) ) )
      {
        gfl2::heap::HeapBase *heap = gfl2::heap::Manager::GetHeapByHeapId(GFL_HEAP_INDEX_TO_LOCAL_HEAP_ID(i));
        const int size = heap->GetTotalSize();
        const int free = heap->GetTotalFreeSize();
        const int allocate = heap->GetTotalAllocatableSize();
        const bool isFragment = ((free-allocate)>gfl2::heap::MEMORY_HEADER_SIZE);

        GFL_RELEASE_PRINT("[%3d][%32s][0x%08x][0x%08x][ 0x%08x][%24s(%4d)]%s\n",i,heap->GetHeapName(),size,free,allocate,heap->GetCreateDataFilename(),heap->GetCreateDataLine(),(isFragment?"断片化":" "));

      }
    }
    GFL_RELEASE_PRINT("[ ID][-----------------------HEAP NAME][TOTAL SIZE][FREE SIZE-][ALLOCATABLE][-------------CREATE FILE(LINE)]\n");
  }
}
//ツリーダンプ
void DebugWinFunc_Heap_UpdateTreeDump( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  GFL_UNUSED(userWork);

  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();
  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    item->PlaySystemSound(gfl2::debug::SE_ACCEPT);

    gfl2::heap::HeapBase *workHeap;
    if( gfl2::heap::Manager::IsExistHeapId( HEAPID_DEBUG ) )
    {
      workHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
      gfl2::heap::Manager::DumpTree(workHeap);
    }
  }
}

//VisualizerLog
void DebugWinUpdate_MemoryVisualizerLog( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  GFL_UNUSED(userWork);

  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();
  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A )
    || uiBtn->IsTrigger( gfl2::ui::BUTTON_LEFT )
    || uiBtn->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
#if defined(GF_PLATFORM_CTR)
#if GFL_HIO_ENABLE
    if( gfl2::heap::Manager::IsHioDumpDefault() )
    {
      gfl2::heap::Manager::GetHeapByHeapId( HEAPID_FILEREAD )->EndHioDump();
      gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE )->EndHioDump();
      gfl2::heap::Manager::SetHioDumpDefault(false);  //この中で生きてるヒープのHioDump Abortされるので、先にEndを呼ぶ
    }
    else
    {
      gfl2::heap::Manager::SetHioDumpDefault(true);
      gfl2::heap::Manager::GetHeapByHeapId( HEAPID_FILEREAD )->StartHioDump();
      gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE )->StartHioDump();
    }
#endif  //HIO
#endif //CTR
  }
}


wchar_t* DebugWinDraw_MemoryVisualizerLog( void* userWork  , gfl2::debug::DebugWinItem *item )
{
#if GFL_DEBUGWIN_USE
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), gfl2::heap::Manager::IsHioDumpDefault() );
  return workStr;
#else
  return NULL;
#endif
}

//VisualizerLog
void DebugWinUpdate_HioLogInMemory( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  GFL_UNUSED(userWork);

  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();
  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A )
    || uiBtn->IsTrigger( gfl2::ui::BUTTON_LEFT )
    || uiBtn->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
#if defined(GF_PLATFORM_CTR)
#ifdef GFL_DEBUG_HIO_LOG_IN_MEMORY
    if( gfl2::heap::CTRExpHeap::GetDebugHioDumpInMemory() == 0 )
    {
      gfl2::heap::CTRExpHeap::SetDebugHioDumpInMemory( true, HEAPID_DEBUG_DEBUGGER );
    }
    else
    {
      gfl2::heap::CTRExpHeap::SetDebugHioDumpInMemory( false, HEAPID_DEBUG_DEBUGGER );
    }
#endif
#endif //CTR
  }
}

wchar_t* DebugWinDraw_HioLogInMemory( void* userWork  , gfl2::debug::DebugWinItem *item )
{
#if GFL_DEBUGWIN_USE
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
#ifdef GFL_DEBUG_HIO_LOG_IN_MEMORY
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), gfl2::heap::CTRExpHeap::GetDebugHioDumpInMemory() );
#endif 
  return workStr;
#else
  return NULL;
#endif
}

//個々のHeapのDump
void DebugWinFunc_Heap_UpdateDump( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  ::Debug::DebugWin::DebugWinWork *work = GFL_SINGLETON_INSTANCE(::Debug::DebugWin::DebugWinWork);
  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();
  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    item->PlaySystemSound(gfl2::debug::SE_ACCEPT);

    if( gfl2::heap::Manager::IsExistHeapId( work->mHeapDumpId ) )
    {
      gfl2::heap::HeapBase *heap = gfl2::heap::Manager::GetHeapByHeapId( work->mHeapDumpId );
      heap->Dump();
    }
    else
    {
      GFL_PRINT("Heap[%d] is not found!\n",work->mHeapDumpId);
    }
  }
}

void DebugWinFunc_Heap_UpdateDumpLocal( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  ::Debug::DebugWin::DebugWinWork *work = GFL_SINGLETON_INSTANCE(::Debug::DebugWin::DebugWinWork);
  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();
  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    item->PlaySystemSound(gfl2::debug::SE_ACCEPT);

    if( gfl2::heap::Manager::IsExistHeapId( GFL_HEAP_INDEX_TO_LOCAL_HEAP_ID(work->mLocalHeapDumpId)  ) )
    {
      gfl2::heap::HeapBase *heap = gfl2::heap::Manager::GetHeapByHeapId( GFL_HEAP_INDEX_TO_LOCAL_HEAP_ID(work->mLocalHeapDumpId) );
      heap->Dump();
    }
    else
    {
      GFL_PRINT("LocalHeap[%d] is not found!\n",work->mLocalHeapDumpId);
    }
  }
}

void DebugWinUpdate_HeapView( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  gfl2::heap::HeapBase* pHeap = reinterpret_cast<gfl2::heap::HeapBase*>( userWork );
  ::Debug::DebugWin::DebugWinWork *work = GFL_SINGLETON_INSTANCE(::Debug::DebugWin::DebugWinWork);
  s32 step = 1;
  s32* target = &work->mHeapViewUnitType;
  s32 max = 2;
  s32 min = 0;

  gfl2::ui::Button *uiButton = gfl2::debug::DebugWin_GetUiButton();
  gfl2::ui::VectorDevice *uiKey = item->GetSysUiCrossKey();

  if( uiButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pHeap->Dump();
  }

  if( uiButton->IsHold( gfl2::ui::BUTTON_R ) )
  {
    step *= 10;
  }
  if( uiButton->IsHold( gfl2::ui::BUTTON_L ) )
  {
    step *= 100;
  }

  if( uiKey->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    if( *target == min )
    {
      *target = max;
    }
    else
    {
      if( *target - step < min )
      {
        *target = min;
      }
      else
      {
        *target -= step;
      }
      item->PlaySystemSound( gfl2::debug::SE_CHANGE_VALUE );
    }
  }
  else
  {
    if( uiKey->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
    {
      if( *target == max )
      {
        *target = min;
      }
      else
      {
        if( *target + step > max )
        {
          *target = max;
        }
        else
        {
          *target += step;
        }
        item->PlaySystemSound( gfl2::debug::SE_CHANGE_VALUE );
      }
    }
  }
}

s32 Pow( s32 x, s32 y )
{
  s32 result = x;
  for( s32 i = 1; i < y; i++ )
  {
    result = result * x;
  }

  return result;
}

void IntToStr( s32 value, wchar_t* pOut, s32 max )
{
  if( max == 0 )
  {
    return;
  }

  u32 keta = 0;
  s32 work = value;
  do
  {
    keta++;
    work = work / 10;
  }
  while( work != 0 );

  // 数値 + , + \0がバッファサイズを超えているかチェック
  if( keta + (keta-1) / 3 + 1 >= max )
  {
    pOut = '\0';
    return;
  }

  s32 charIndex = 0;
  for( s32 i = keta; i > 0; i-- )
  {
    if( i == 1 )
    {
      pOut[ charIndex ] = '0' + ( value % 10 );
      charIndex++;
    }
    else
    {
      pOut[ charIndex ] = '0' + ( value / Pow( 10, i - 1 ) % 10 );
      charIndex++;

      if( i % 3 == 1 )
      {
        pOut[ charIndex ]  = ',';
        charIndex++;
      }
    }
  }

  pOut[ charIndex ] = '\0';
}

wchar_t * DebugWinDraw_HeapName( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  gfl2::heap::HeapBase* pHeap = reinterpret_cast<gfl2::heap::HeapBase*>( userWork );
  DebugWinWork *debWork = GFL_SINGLETON_INSTANCE(DebugWinWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN
    ,L"%2d %ls" , pHeap->GetHeapId(), item->GetName() );

  return workStr;
}



wchar_t * DebugWinDraw_HeapView( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  gfl2::heap::HeapBase* pHeap = reinterpret_cast<gfl2::heap::HeapBase*>( userWork );
  DebugWinWork *debWork = GFL_SINGLETON_INSTANCE(DebugWinWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  struct UNIT_INFO
  {
    s32 unit;
    const wchar_t* dispName;
  };

  UNIT_INFO unitInfoTable[] =
  {
    1, L"Byte",
    1024, L"KByte",
    1024*1024, L"MByte",
  };
  
  s32 unit = debWork->mHeapViewUnitType;
  s32 heapSizeView = pHeap->GetTotalFreeSize() / unitInfoTable[ unit ].unit;
  s32 heapSizeMaxView = pHeap->GetTotalSize() / unitInfoTable[ unit ].unit;

  wchar_t valueText[ 255 ];
  IntToStr( heapSizeView, valueText, 255 );   // ,をつける @note swprintfではつけられない
  
  wchar_t maxValueText[ 255 ];
  IntToStr( heapSizeMaxView, maxValueText, 255 );   // ,をつける @note swprintfではつけられない

  swprintf( workStr , gfl2::debug::DEBUGWIN_NAME_LEN
    ,L"       -[ %ls / %ls ](%ls)" , valueText, maxValueText, unitInfoTable[ unit ].dispName );

  return workStr;
}




void CreateGroup_HeapView( void *userWork, gfl2::debug::DebugWinItem *item )
{
  // FieldDebugMenu *pDebug = reinterpret_cast<FieldDebugMenu*>( userWork );
  gfl2::debug::DebugWinGroup *pGropWork = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pGropWork )
  {
    gfl2::heap::HeapBase *pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork, pHeap, L"HeapID HeapName [ TotalFreeSize / TotalSize ]", NULL, NULL, NULL )->SetIsSelectSkip( true );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork, pHeap, L"[A]Dump [Left/Right]ChangeUnit", NULL, NULL, NULL )->SetIsSelectSkip( true );


    b32 isFirstData = true;
    
    for( int i=0;i<HEAPID_CHILD_MAX;i++ )
    {
      if( gfl2::heap::Manager::IsExistHeapId( i ) )
      {
        gfl2::heap::HeapBase *heap = gfl2::heap::Manager::GetHeapByHeapId(i);
        const int size = heap->GetTotalSize();
        const int free = heap->GetTotalFreeSize();
        const int allocate = heap->GetTotalAllocatableSize();
        const bool isFragment = ((free-allocate)>gfl2::heap::MEMORY_HEADER_SIZE);

        gfl2::debug::DebugWinFunc* pItem = gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork, pHeap, heapNameArr[i], heap, DebugWinUpdate_HeapView, DebugWinDraw_HeapName );
        gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork, pHeap, heapNameArr[i], heap, DebugWinUpdate_HeapView, DebugWinDraw_HeapView )->SetIsSelectSkip( true );
 
        // データ部の先頭を選択
        if ( isFirstData )
        {
          gfl2::debug::DebugWin_SetCurrentItem( pItem );
          isFirstData = false;
        }
      
      }
    }

    // gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    // Field::EnvManager* pEnvManager = pDebug->GetEnvManager();
    // gfl2::debug::DebugWinItemValueU32  <u32>* itemU32   = NULL;
  }
}

void CreateGroupAfter_HeapView(void *userWork, gfl2::debug::DebugWinItem *item)
{
  gfl2::debug::DebugWinGroup *pGropWork = reinterpret_cast<gfl2::debug::DebugWinGroup*>(item);
  if (pGropWork)
  {
    gfl2::debug::DebugWin_SetCurrentItemByIndex(2);

  }
}

void DestroyGroup_HeapView( void *userWork, gfl2::debug::DebugWinItem *item )
{
  gfl2::debug::DebugWinGroup *pGropWork = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pGropWork )
  {
    pGropWork->RemoveAllItem();
  }
}

//げんご
void DebugWin_UpdateLanguage( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();


  s8 dir = 0;

  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
  {
    dir = -1;
  } 
  else if( uiBtn->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    dir = +1;
  }

  if( dir != 0 )
  {
    static const u8 Language[] =
    {
      POKEMON_LANG_JAPAN,
      POKEMON_LANG_ENGLISH,
      POKEMON_LANG_FRANCE,
      POKEMON_LANG_ITALY,
      POKEMON_LANG_GERMANY,
      POKEMON_LANG_SPAIN,
      POKEMON_LANG_KOREA,
      POKEMON_LANG_CHINA,
      POKEMON_LANG_TAIWAN,
    };

    for (int i = 0; i < GFL_NELEMS(Language); ++i )
    {
      if( System::GetLang() == Language[i])
      {
        int nextIndex = (i+dir);
        if( nextIndex < 0 )
        {
          nextIndex = GFL_NELEMS(Language) - 1;
        }
        else
        {
          nextIndex %= GFL_NELEMS(Language);
        }

        gfl2::heap::HeapBase *workHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
        System::SetLang( Language[nextIndex], workHeap );
        break;
      }
    }
  }
}
wchar_t* DebugWin_DrawLanguage( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  DebugWinWork *debWork = GFL_SINGLETON_INSTANCE(DebugWinWork);
  if( debWork != NULL )
  {
    wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
    gfl2::str::StrBuf* workStrBuf =GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetWorkStrBuf();
    gfl2::str::MsgData * pMsgData = debWork->GetMessageData();
    pMsgData->GetString( msg_dmenu_language_00 + System::GetLang(), *workStrBuf );

    swprintf( workStr , gfl2::debug::DEBUGWIN_NAME_LEN
                ,L"%ls[%ls] " ,item->GetName(), workStrBuf->GetPtr() );

    return workStr;
  }

  return NULL;
}


// テキスト速度
void DebugWin_UpdateTextSpdPutCountMax( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  DebugWinWork *debWork = GFL_SINGLETON_INSTANCE(DebugWinWork);
  if( debWork != NULL )
  {
    gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();

    if( uiBtn->IsTrigger(gfl2::ui::BUTTON_A) )
    {
      debWork->mTextSpeedPutCount = 255;
      print::MessageWindow::SetDebugMessageSpeed(
        debWork->mTextSpeedWaitFrame, debWork->mTextSpeedPutCount );
    }
  }
}

void DebugWin_UpdateTextSpdWaitFrame( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  DebugWinWork *debWork = GFL_SINGLETON_INSTANCE(DebugWinWork);
  if( debWork != NULL )
  {
    gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();

    u32 prevWaitFrame = debWork->mTextSpeedWaitFrame;

    if( uiBtn->IsTrigger(gfl2::ui::BUTTON_LEFT) )
    {
      if( debWork->mTextSpeedWaitFrame > 0 )
      {
        debWork->mTextSpeedWaitFrame--;
      }
    }
    else if( uiBtn->IsTrigger(gfl2::ui::BUTTON_RIGHT) )
    {
      if( debWork->mTextSpeedWaitFrame < 255 )
      {
        debWork->mTextSpeedWaitFrame++;
      }
    }

    if( prevWaitFrame != debWork->mTextSpeedWaitFrame )
    {
      print::MessageWindow::SetDebugMessageSpeed(
        debWork->mTextSpeedWaitFrame, debWork->mTextSpeedPutCount );
    }
  }
}

wchar_t* DebugWin_DrawTextSpdWaitFrame( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  DebugWinWork *debWork = GFL_SINGLETON_INSTANCE(DebugWinWork);
  if( debWork != NULL )
  {
    wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"文字出力間隔(0-255)[ %d ]", debWork->mTextSpeedWaitFrame );

    return workStr;
  }

  return NULL;
}

void DebugWin_UpdateTextSpdPutCount( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  DebugWinWork *debWork = GFL_SINGLETON_INSTANCE(DebugWinWork);
  if( debWork != NULL )
  {
    gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();

    u32 prevPutCount = debWork->mTextSpeedPutCount;

    if( uiBtn->IsRepeat(gfl2::ui::BUTTON_LEFT) )
    {
      if( debWork->mTextSpeedPutCount > 1 )
      {
        debWork->mTextSpeedPutCount--;
      }
    }
    else if( uiBtn->IsRepeat(gfl2::ui::BUTTON_RIGHT) )
    {
      if( debWork->mTextSpeedPutCount < 255 )
      {
        debWork->mTextSpeedPutCount++;
      }
    }

    if( prevPutCount != debWork->mTextSpeedPutCount )
    {
      print::MessageWindow::SetDebugMessageSpeed(
        debWork->mTextSpeedWaitFrame, debWork->mTextSpeedPutCount );
    }
  }
}

wchar_t* DebugWin_DrawTextSpdPutCount( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  DebugWinWork *debWork = GFL_SINGLETON_INSTANCE(DebugWinWork);
  if( debWork != NULL )
  {
    wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"出力文字数(1-255)[ %d ]", debWork->mTextSpeedPutCount );

    return workStr;
  }

  return NULL;
}

void DebugWin_UpdateTextSpdUsing( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  DebugWinWork *debWork = GFL_SINGLETON_INSTANCE(DebugWinWork);
  if( debWork != NULL )
  {
    gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();

    if( uiBtn->IsTrigger(gfl2::ui::BUTTON_LEFT) )
    {
      print::MessageWindow::SetDebugMessageSpeedUseFlag( false );
    }
    else if( uiBtn->IsTrigger(gfl2::ui::BUTTON_RIGHT) )
    {
      print::MessageWindow::SetDebugMessageSpeed( debWork->mTextSpeedWaitFrame, debWork->mTextSpeedPutCount );
      print::MessageWindow::SetDebugMessageSpeedUseFlag( true );
    }
  }
}

wchar_t* DebugWin_DrawTextSpdUsing( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"使用[ %d ]", print::MessageWindow::GetDebugMessageSpeedUseFlag() );

  return workStr;
}

#endif //USE_DEBUG_WIN

GFL_NAMESPACE_END(DebugWin)
GFL_NAMESPACE_END(Debug)
