#if PM_DEBUG


//==============================================================================
/**
 @file    ResortDebugWinImpl.cpp
 @author  k.ohno
 @date    2015.08.11
 @brief   リゾートのデバッグウィンドウ
 */
//==============================================================================


// gfl2のインクルード
#include <str/include/gfl2_StrBuf.h>

#include <debug/include/gfl2_DebugWinSystem.h>
#include <debug/include/gfl2_DebugWinItemTypes.h>

// nijiのデータのヘッダーファイルのインクルード
#include <niji_conv_header/poke_lib/monsno_def.h>

// nijiのインクルード
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <Savedata/include/ResortSave.h>
#include <Savedata/include/ZukanSave.h>
#include <System/include/PokemonVersion.h>
#include <System/include/GflUse.h>
#include <Sound/include/Sound.h>
#include <Print/include/WordSet.h>

#include <Debug/DebugWin/include/DebugWinSystem.h>
// メッセージのインデックス
#include "niji_conv_header/message/debug/msg_debug_menu.h"

#include "ResortDebugWin.h"

#include "ResortDebugInfo.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)


//==============================================================================
/**
 @class     ResortDebugWinImpl
 */
//==============================================================================
class ResortDebugWinImpl
{
  GFL_FORBID_COPY_AND_ASSIGN(ResortDebugWinImpl);

public:
  ResortDebugWinImpl(gfl2::heap::HeapBase* heap, gfl2::debug::DebugWinGroup* parentGroup, gfl2::str::MsgData* pMsgData);
  virtual ~ResortDebugWinImpl();

private:
  static void     Initialize(ResortDebugWinImpl* inst);
  static void     Terminate(void);

  static void     IslandFlag_Update(void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* IslandFlag_Draw  (void* userWork, gfl2::debug::DebugWinItem* item);

  static void     camera_Update(void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* camera_Draw  (void* userWork, gfl2::debug::DebugWinItem* item);

  static void     fullmode_Update(void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* fullmode_Draw  (void* userWork, gfl2::debug::DebugWinItem* item);
    
  //LV
  static void     friendshiplevel_Update(void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* friendshiplevel_Draw  (void* userWork, gfl2::debug::DebugWinItem* item);

  static void     gimlevel_Update(void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* gimlevel_Draw  (void* userWork, gfl2::debug::DebugWinItem* item);

  static void     treasurelevel_Update(void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* treasurelevel_Draw  (void* userWork, gfl2::debug::DebugWinItem* item);

  static void     farmlevel_Update(void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* farmlevel_Draw  (void* userWork, gfl2::debug::DebugWinItem* item);

  static void     hotspalevel_Update(void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* hotspalevel_Draw  (void* userWork, gfl2::debug::DebugWinItem* item);

  const wchar_t* GetResisterNumberPtr( u32 id, u32 val, u32 keta );

  // タイマー
  void SetTimerWork( u8 id, u8 h, u8 m, u8 s );
  void CreateTimerGroup( gfl2::debug::DebugWinGroup * parent, u32 str, u8 id, u8 h, u8 m, u8 s );
  void CreateTimerGroup( gfl2::debug::DebugWinGroup * parent, const wchar_t * str, u8 id, u8 h, u8 m, u8 s );
  static void timer_Update(void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* timer_Draw(void* userWork, gfl2::debug::DebugWinItem* item);
  static void timerdecide_Update(void* userWork, gfl2::debug::DebugWinItem* item);

  void CreatePokeClearGroup( gfl2::debug::DebugWinGroup * parent, u32 str, u8 id, u8 max );
  static void pokeclear_Update(void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* pokeclear_Draw(void* userWork, gfl2::debug::DebugWinItem* item);
  static void pokecleardecide_Update(void* userWork, gfl2::debug::DebugWinItem* item);

  static void beanstype_Update(void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* beanstype_Draw(void* userWork, gfl2::debug::DebugWinItem* item);
  static void beansnum_Update(void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* beansnum_Draw(void* userWork, gfl2::debug::DebugWinItem* item);
  static void beansbottle_Update(void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* beansbottle_Draw(void* userWork, gfl2::debug::DebugWinItem* item);
  static void beansdecide_Update(void* userWork, gfl2::debug::DebugWinItem* item);

  void CreatePokeEventGroup( gfl2::debug::DebugWinGroup * parent, u32 str, u8 id, u8 start, u8 max );
  static wchar_t* pokeevent_Draw(void* userWork, gfl2::debug::DebugWinItem* item);


  // その他
  static s32 Util_S32_Update(s32 currVal, s32 minVal, s32 maxVal, bool isLoop, bool isJump);

  static void speedtype_Update(void* userWork, gfl2::debug::DebugWinItem* /*item*/);
  static wchar_t* speedtype_Draw(void* userWork, gfl2::debug::DebugWinItem* item);
  static void speedtype_UpdateX(void* userWork, gfl2::debug::DebugWinItem* /*item*/,int num);
  static wchar_t* speedtype_DrawX(void* userWork, gfl2::debug::DebugWinItem* item,int num);
  static void speedtype_Update2(void* userWork, gfl2::debug::DebugWinItem* /*item*/);
  static wchar_t* speedtype_Draw2(void* userWork, gfl2::debug::DebugWinItem* item);
  static void speedtype_Update3(void* userWork, gfl2::debug::DebugWinItem* /*item*/);
  static wchar_t* speedtype_Draw3(void* userWork, gfl2::debug::DebugWinItem* item);
  static void speedtype_Update4(void* userWork, gfl2::debug::DebugWinItem* /*item*/);
  static wchar_t* speedtype_Draw4(void* userWork, gfl2::debug::DebugWinItem* item);
  static void tutorial_Update(void* userWork, gfl2::debug::DebugWinItem* /*item*/);
  static wchar_t* tutorial_Draw(void* userWork, gfl2::debug::DebugWinItem* item);

  static void BeansCounter_Update(void* userWork, gfl2::debug::DebugWinItem* /*item*/);
  static wchar_t* BeansCounter_Draw(void* userWork, gfl2::debug::DebugWinItem* item);
  
  static void fureaiPoke_Update(void* userWork, gfl2::debug::DebugWinItem* /*item*/);
  static wchar_t* fureaiPoke_Draw(void* userWork, gfl2::debug::DebugWinItem* item);

  static void     debug_info_Update(void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* debug_info_Draw  (void* userWork, gfl2::debug::DebugWinItem* item);
  
  static void UpdateGimSetTamago( void * userWork, gfl2::debug::DebugWinItem * item );
  static void UpdateGimSetDameTamago( void * userWork, gfl2::debug::DebugWinItem * item );
  static void UpdateHotspaSetDameTamago( void * userWork, gfl2::debug::DebugWinItem * item );
  
public:
  int GetCameraDebugMode(void){ return s_inst->mbDebugCameraMode; }
  bool GetDebugFullMode(void){ return s_inst->mbDebugFullMode; }

  int GetSpeedVal( void ){	return s_inst->mSpeedWork.val; }

  
private:



  static ResortDebugWinImpl* s_inst;

private:
  static const s32 MONSNO_END_TEMP = MONSNO_DEOKISISU;

  // タイマーワークID
  enum
  {
    // ふれあい
    TIMER_FRIEND_SHIP_WILD_POKEMON = 0,       // 野生ポケモン抽選用カウントをためる時間  mWildPokemonTime
    TIMER_FRIEND_SHIP_WILD_POKEMON_SERVICE,   // 野生ポケモンの抽選確率が２倍になるカウンター   mWildPokemonServiceTime
    TIMER_FRIEND_SHIP_TIME,                   // 次に豆やポケモンの抽選を行うまでの時間  mFriendShipTime

    // 木のみリゾート
    TIMER_FARM_TIME,        // 収穫の時間 x 18       mFarmTime[ RESORT_FARM_MAX ][ DOWNTIMER_BUFF ]
    TIMER_FARM_FERTILIZER = TIMER_FARM_TIME + Savedata::ResortSave::RESORT_FARM_MAX,  // こやしの効果持続時間  mFarmFertilizerTime

    // たんけんリゾート
    TIMER_ADVENTURE_COUNT,  // アイテム抽選権のタイマーカウント   mAdventureCount
    TIMER_ADVENTURE_BOUNUS, // トロッコの効果持続時間             mAdventureBonusTime

    // うんどうリゾート
    TIMER_GIM_BOUNUS,       // うんどう装置の効果持続時間  mGimBonusTime
    TIMER_GIM_END,          // うんどうの残り時間 x 3      mGimEndTime[RESORT_GIM_KIND_MAX][DOWNTIMER_BUFF];

    // おんせんリゾート
    TIMER_HOT_SPA_BOUNUS = TIMER_GIM_END + Savedata::ResortSave::RESORT_GIM_KIND_MAX,   // 入浴剤の効果持続時間           mHotSpaBonusTime
    TIMER_HOT_SPA_POKE,                       // タイマーはトレイぶん３つ x 3   mHotSpaPokeTime[DOWNTIMER_BUFF][DOWNTIMER_BUFF];

    TIMER_HOT_SPA_NOBOSE = TIMER_HOT_SPA_POKE + Savedata::ResortSave::RESORT_HOTSPA_KIND_MAX,   // のぼせ時間           mHotSpaBonusTime
    // その他
    TIMER_POKEMON_EVENT = TIMER_HOT_SPA_NOBOSE + Savedata::ResortSave::RESORT_HOTSPA_KIND_MAX,    // ポケモンのイベントを再度抽選するタイマー    mPokemonEventTime
    //    TIMER_BOX_POKEMON_POS = 0,    // ???                                         boxPokemonPos

    // まめびん  // マメビン  // まめ瓶  // beansbottle  // beans_bottle  // beans bottle
    TIMER_BEANS_BOTTLE_EVENT,  // マメビンイベントタイマー    mBeansBottleTime

    TIMER_WORK_MAX
  };

  // ポケモン初期化ワークID
  enum
  {
    POKE_CLEAR_FIRIEND_SHIP = 0,
    POKE_CLEAR_FARM,
    POKE_CLEAR_ADVENTURE,
    POKE_CLEAR_WILD,
    POKE_CLEAR_WORK_MAX,
  };

  // ポケモンイベントワークID
  enum
  {
    POKE_EV_GROUP_FRIEND_SHIP = 0,
    POKE_EV_GROUP_WILD,
    POKE_EV_GROUP_FARM,
    POKE_EV_GROUP_ADVENTURE,
    POKE_EV_GROUP_GIM,
    POKE_EV_GROUP_HOT_SPA,
    POKE_EV_GROUP_MAX,

    POKE_EV_FRIEND_SHIP = 0,
    POKE_EV_WILD        = POKE_EV_FRIEND_SHIP + Savedata::ResortSave::STOCKPOKEMON_MAX,
    POKE_EV_FARM        = POKE_EV_WILD + Savedata::ResortSave::RESORT_FRIENDSHIP_PLAYPOKE_MAX,
    POKE_EV_ADVENTURE   = POKE_EV_FARM + Savedata::ResortSave::STOCKPOKEMON_MAX,
    POKE_EV_GIM         = POKE_EV_ADVENTURE + Savedata::ResortSave::STOCKPOKEMON_MAX,
    POKE_EV_HOT_SPA     = POKE_EV_GIM + Savedata::ResortSave::STOCKPOKEMON_MAX,
    POKE_EV_WORK_MAX    = POKE_EV_HOT_SPA + Savedata::ResortSave::STOCKPOKEMON_MAX,
  };


private:
  gfl2::heap::HeapBase*       m_heap;
  gfl2::debug::DebugWinGroup* m_parentGroup;

  gfl2::debug::DebugWinGroup* mResortGroup;
  gfl2::debug::DebugWinGroup* mCameraGroup;
  gfl2::debug::DebugWinGroup* mFriendShipGroup; // ふれあい
  gfl2::debug::DebugWinGroup* mTreasureGroup;   // たんけん
  gfl2::debug::DebugWinGroup* mGimGroup;        // うんどう
  gfl2::debug::DebugWinGroup* mHotspaGroup;     // おんせん
  gfl2::debug::DebugWinGroup* mFarmGroup;       // きのみ
  gfl2::debug::DebugWinGroup* mEtcGroup;        // その他

  gfl2::debug::DebugWinGroup* mTimerGroup[TIMER_WORK_MAX];
  gfl2::debug::DebugWinGroup* mClearPokeGroup[POKE_CLEAR_WORK_MAX];
  gfl2::debug::DebugWinGroup* mEventPokeGroup[POKE_EV_GROUP_MAX];
  gfl2::debug::DebugWinGroup* mBeansGroup;
  gfl2::debug::DebugWinGroup* mSpeedGroup;
  gfl2::debug::DebugWinGroup* mTutorialGroup;
  gfl2::debug::DebugWinGroup* mFullGroup;
  gfl2::debug::DebugWinGroup* mFureaiPokeGroup;
  gfl2::debug::DebugWinGroup* mBeansCounterGroup;

  gfl2::str::StrBuf* m_strBuf;

  gfl2::str::MsgData* m_pMsgData;

  int mbDebugCameraMode;
  bool mbDebugFullMode;

  print::WordSet * m_pWordSet;

  struct TimerCount
  {
    u16 val;
    u16 max;
  };
  struct TimerWork
  {
    u16 id;
    TimerCount count[Savedata::ResortSave::DOWNTIMER_BUFF];
  };
  TimerWork mTimerWork[TIMER_WORK_MAX];

  struct PokeClearWork
  {
    u16 id;
    u8 index;
    u8 max;
    pml::pokepara::CoreParam * cp;
  };
  PokeClearWork mPokeClearWork[POKE_CLEAR_WORK_MAX];
  pml::pokepara::CoreParam * m_pClearCoreParam;

  struct BeansWork
  {
    u16 index;
    u16 val;
    u16 onoff;
  };
  BeansWork mBeansWork;


  struct SpeedWork
  {
    u16 val;
  };
  SpeedWork mSpeedWork;

  struct TutorialWork
  {
    u16 val;
  };
  TutorialWork mTutorialWork;

  struct BeansCounterWork
  {
    u16 val;
  };
  BeansCounterWork mBeansCounterWork;

  
  struct FureaiPokeWork
  {
    pml::pokepara::CoreParam * cp;
    u16 box;
    u16 pos;
    u16 mons;
    u16 form;
  };
  FureaiPokeWork mFureaiPokeWork;


  
  struct PokeEventWork
  {
    u16 id;
    u16 index;
    pml::pokepara::CoreParam * cp;
  };
  PokeEventWork mPokeEventWork[POKE_EV_WORK_MAX];
  pml::pokepara::CoreParam * m_pTmpCoreParam;
};




//==============================================================================
//
//  ResortDebugWinImpl
//
//==============================================================================
ResortDebugWinImpl::ResortDebugWinImpl(gfl2::heap::HeapBase* heap, gfl2::debug::DebugWinGroup* parentGroup, gfl2::str::MsgData* pMsgData)
   : m_heap(heap),
     m_parentGroup(parentGroup),
     mResortGroup(NULL),
     m_strBuf(NULL),
     m_pMsgData(pMsgData)
{
  Initialize(this);

  m_pWordSet = GFL_NEW(m_heap) print::WordSet( m_heap );
  m_strBuf = GFL_NEW(m_heap) gfl2::str::StrBuf(512, m_heap);
  m_pClearCoreParam = GFL_NEW(m_heap) pml::pokepara::CoreParam( m_heap, MONSNO_NULL, 0, 0 );
  m_pTmpCoreParam = GFL_NEW(m_heap) pml::pokepara::CoreParam( m_heap, MONSNO_NULL, 0, 0 );

  mSpeedWork.val = 1;
  
  // 親
  mResortGroup     = gfl2::debug::DebugWin_AddGroup(m_heap, m_pMsgData, msg_dmenu_resort_top, parentGroup);

  // 各種
  {
    mCameraGroup  = gfl2::debug::DebugWin_AddGroup(m_heap, m_pMsgData, msg_dmenu_resort_camera,   mResortGroup);
    mFriendShipGroup = gfl2::debug::DebugWin_AddGroup(m_heap, m_pMsgData, msg_dmenu_resort_friendship, mResortGroup);
    mFarmGroup  = gfl2::debug::DebugWin_AddGroup(m_heap, m_pMsgData, msg_dmenu_resort_farm,   mResortGroup);
    mHotspaGroup  = gfl2::debug::DebugWin_AddGroup(m_heap, m_pMsgData, msg_dmenu_resort_hotspa,   mResortGroup);
    mGimGroup  = gfl2::debug::DebugWin_AddGroup(m_heap, m_pMsgData, msg_dmenu_resort_gim,   mResortGroup);
    mTreasureGroup  = gfl2::debug::DebugWin_AddGroup(m_heap, m_pMsgData, msg_dmenu_resort_treasure,   mResortGroup);
    mEtcGroup = gfl2::debug::DebugWin_AddGroup(m_heap, m_pMsgData, msg_dmenu_resort_etc_00, mResortGroup);
  }

  //カメラモード
  {
    gfl2::debug::DebugWin_AddItemUpdateFunc(mCameraGroup, m_heap, m_pMsgData, msg_dmenu_resort_camera_mode, NULL , camera_Update , camera_Draw);
  }
  //ふれあい
  {
    gfl2::debug::DebugWin_AddItemUpdateFunc(mFriendShipGroup, m_heap, m_pMsgData, msg_dmenu_resort_friendship_lv, NULL , friendshiplevel_Update , friendshiplevel_Draw);

    { // タイマー関連
      u8 h, m, s;

      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave()->GetWildPokemonTime( &h, &m, &s );
      CreateTimerGroup( mFriendShipGroup, msg_dmenu_resort_fs_time_00, TIMER_FRIEND_SHIP_WILD_POKEMON, h, m, s );

      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave()->GetWildPokemonServiceTime( &h, &m, &s );
      CreateTimerGroup( mFriendShipGroup, msg_dmenu_resort_fs_time_01, TIMER_FRIEND_SHIP_WILD_POKEMON_SERVICE, h, m, s );

      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave()->GetFriendShipTime( &h, &m, &s );
      CreateTimerGroup( mFriendShipGroup, msg_dmenu_resort_fs_time_02, TIMER_FRIEND_SHIP_TIME, h, m, s );
    }
    // ポケモン消去
    CreatePokeClearGroup( mFriendShipGroup, msg_dmenu_resort_clear_pokemon_00, POKE_CLEAR_FIRIEND_SHIP, Savedata::ResortSave::STOCKPOKEMON_MAX );
    // ポケモンイベント
    CreatePokeEventGroup( mFriendShipGroup, msg_dmenu_resort_etc_04, POKE_EV_GROUP_FRIEND_SHIP, POKE_EV_FRIEND_SHIP, Savedata::ResortSave::STOCKPOKEMON_MAX );

    // マメカウンタ
///    CreatePokeEventGroup( mFriendShipGroup, msg_dmenu_resort_beans_c, POKE_EV_GROUP_FRIEND_SHIP, POKE_EV_FRIEND_SHIP, Savedata::ResortSave::STOCKPOKEMON_MAX );

    mBeansCounterGroup = gfl2::debug::DebugWin_AddGroup( m_heap, m_pMsgData, msg_dmenu_resort_beans_c, mFriendShipGroup );
    gfl2::debug::DebugWin_AddItemUpdateFunc( mBeansCounterGroup, m_heap, m_pMsgData, msg_dmenu_resort_beans_c, &mBeansCounterWork, BeansCounter_Update, BeansCounter_Draw );

  }
  // きのみ
  {
    gfl2::debug::DebugWin_AddItemUpdateFunc(mFarmGroup, m_heap, m_pMsgData, msg_dmenu_resort_farm_lv, NULL, farmlevel_Update , farmlevel_Draw );

    { // タイマー関連
      u8 h, m, s;

      for( u32 i=0; i<Savedata::ResortSave::RESORT_FARM_MAX; i++ )
      {
        GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave()->GetFarmTime( i, &h, &m, &s );
        CreateTimerGroup( mFarmGroup, GetResisterNumberPtr(msg_dmenu_resort_farm_time_00,i,2), TIMER_FARM_TIME+i, h, m, s );
      }

      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave()->GetFarmFertilizerTime( &h, &m, &s );
      CreateTimerGroup( mFarmGroup, msg_dmenu_resort_farm_time_01, TIMER_FARM_FERTILIZER, h, m, s );
    }
    // ポケモン消去
    CreatePokeClearGroup( mFarmGroup, msg_dmenu_resort_clear_pokemon_00, POKE_CLEAR_FARM, Savedata::ResortSave::STOCKPOKEMON_MAX );
    // ポケモンイベント
    CreatePokeEventGroup( mFarmGroup, msg_dmenu_resort_etc_04, POKE_EV_GROUP_FARM, POKE_EV_FARM, Savedata::ResortSave::STOCKPOKEMON_MAX );
  }
  //たんけん
  {
    gfl2::debug::DebugWin_AddItemUpdateFunc(mTreasureGroup, m_heap, m_pMsgData, msg_dmenu_resort_treasure_lv, NULL, treasurelevel_Update , treasurelevel_Draw );

    { // タイマー関連
      u8 h, m, s;

      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave()->GetAdventureTime( &h, &m, &s );
      CreateTimerGroup( mTreasureGroup, msg_dmenu_resort_adv_time_00, TIMER_ADVENTURE_COUNT, h, m, s );

      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave()->GetAdventureBonusTime( &h, &m, &s );
      CreateTimerGroup( mTreasureGroup, msg_dmenu_resort_adv_time_01, TIMER_ADVENTURE_BOUNUS, h, m, s );
    }
    // ポケモン消去
    CreatePokeClearGroup( mTreasureGroup, msg_dmenu_resort_clear_pokemon_00, POKE_CLEAR_ADVENTURE, Savedata::ResortSave::STOCKPOKEMON_MAX );
    // ポケモンイベント
    CreatePokeEventGroup( mTreasureGroup, msg_dmenu_resort_etc_04, POKE_EV_GROUP_ADVENTURE, POKE_EV_ADVENTURE, Savedata::ResortSave::STOCKPOKEMON_MAX );
  }
  //うんどう
  {
    gfl2::debug::DebugWin_AddItemUpdateFunc(mGimGroup, m_heap, m_pMsgData, msg_dmenu_resort_gim_lv, NULL, gimlevel_Update , gimlevel_Draw );

    { // タイマー関連
      u8 h, m, s;

      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave()->GetGimBonusTime( &h, &m, &s );
      CreateTimerGroup( mGimGroup, msg_dmenu_resort_gim_time_00, TIMER_GIM_BOUNUS, h, m, s );

      for( u32 i=0; i<Savedata::ResortSave::RESORT_GIM_KIND_MAX; i++ )
      {
        GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave()->GetGimEndTime( i, &h, &m, &s );
        CreateTimerGroup( mGimGroup, GetResisterNumberPtr(msg_dmenu_resort_gim_time_01,i,2), TIMER_GIM_END+i, h, m, s );
      }
    }
    // ポケモンイベント
    CreatePokeEventGroup( mGimGroup, msg_dmenu_resort_etc_04, POKE_EV_GROUP_GIM, POKE_EV_GIM, Savedata::ResortSave::STOCKPOKEMON_MAX );
    // タマゴ
    gfl2::debug::DebugWin_AddItemUpdateFunc( mGimGroup, m_heap, m_pMsgData, msg_dmenu_resort_egg, NULL, UpdateGimSetTamago, NULL );
    // ダメタマゴ
    gfl2::debug::DebugWin_AddItemUpdateFunc( mGimGroup, m_heap, m_pMsgData, msg_dmenu_resort_ngegg, NULL, UpdateGimSetDameTamago, NULL );
  }
  //おんせん
  {
    gfl2::debug::DebugWin_AddItemUpdateFunc(mHotspaGroup, m_heap, m_pMsgData, msg_dmenu_resort_hotspa_lv, NULL, hotspalevel_Update , hotspalevel_Draw );

    { // タイマー関連
      u8 h, m, s;

      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave()->GetHotSpaBonusTime( &h, &m, &s );
      CreateTimerGroup( mHotspaGroup, msg_dmenu_resort_hotspa_time_00, TIMER_HOT_SPA_BOUNUS, h, m, s );

      for( u32 i=0; i<Savedata::ResortSave::DOWNTIMER_BUFF; i++ )
      {
        GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave()->GetHotSpaPokemonTime( i, &h, &m, &s );
        CreateTimerGroup( mHotspaGroup, GetResisterNumberPtr(msg_dmenu_resort_hotspa_time_01,i,2), TIMER_HOT_SPA_POKE+i, h, m, s );
      }
			for( u32 i=0; i<Savedata::ResortSave::DOWNTIMER_BUFF; i++ )
			{
	      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave()->GetSpaNoboseTime( i, &h, &m, &s );
				CreateTimerGroup( mHotspaGroup, GetResisterNumberPtr(msg_dmenu_resort_hotspa_time_01,i+3,2), TIMER_HOT_SPA_POKE+i+3, h, m, s );
			}

    }
    // ポケモンイベント
    CreatePokeEventGroup( mHotspaGroup, msg_dmenu_resort_etc_04, POKE_EV_GROUP_HOT_SPA, POKE_EV_HOT_SPA, Savedata::ResortSave::STOCKPOKEMON_MAX );
    // ダメタマゴ
    gfl2::debug::DebugWin_AddItemUpdateFunc( mHotspaGroup, m_heap, m_pMsgData, msg_dmenu_resort_ngegg, NULL, UpdateHotspaSetDameTamago, NULL );
  }
  // その他
  {
    { // タイマー関連
      u8 h, m, s;

      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave()->GetPokemonEventTime( &h, &m, &s );
      CreateTimerGroup( mEtcGroup, msg_dmenu_resort_etc_02, TIMER_POKEMON_EVENT, h, m, s );

      //      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave()->GetHotSpaBonusTime( &h, &m, &s );
      //      CreateTimerGroup( mEtcGroup, msg_dmenu_resort_etc_01, TIMER_BOX_POKEMON_POS, h, m, s );
    }
    // 野生ポケモン消去
    CreatePokeClearGroup( mEtcGroup, msg_dmenu_resort_etc_03, POKE_CLEAR_WILD, Savedata::ResortSave::RESORT_FRIENDSHIP_PLAYPOKE_MAX );
    // 野生ポケモンイベント
    CreatePokeEventGroup( mEtcGroup, msg_dmenu_resort_etc_06, POKE_EV_GROUP_WILD, POKE_EV_WILD, Savedata::ResortSave::RESORT_FRIENDSHIP_PLAYPOKE_MAX );
    // マメ
    mBeansGroup = gfl2::debug::DebugWin_AddGroup( m_heap, m_pMsgData, msg_dmenu_resort_mame_00, mEtcGroup );
    gfl2::debug::DebugWin_AddItemUpdateFunc( mBeansGroup, m_heap, m_pMsgData, msg_dmenu_resort_mame_01, &mBeansWork, beanstype_Update, beanstype_Draw );
    gfl2::debug::DebugWin_AddItemUpdateFunc( mBeansGroup, m_heap, m_pMsgData, msg_dmenu_resort_mame_02, &mBeansWork, beansnum_Update, beansnum_Draw );
    gfl2::debug::DebugWin_AddItemUpdateFunc( mBeansGroup, m_heap, m_pMsgData, msg_dmenu_resort_name_03, &mBeansWork, beansbottle_Update, beansbottle_Draw );
    gfl2::debug::DebugWin_AddItemUpdateFunc( mBeansGroup, m_heap, m_pMsgData, msg_dmenu_resort_decide, &mBeansWork, beansdecide_Update, NULL );
    // 倍速
    mSpeedGroup = gfl2::debug::DebugWin_AddGroup( m_heap, m_pMsgData, msg_dmenu_resort_speedup, mEtcGroup );
    gfl2::debug::DebugWin_AddItemUpdateFunc( mSpeedGroup, m_heap, m_pMsgData, msg_dmenu_resort_speedup_00, &mSpeedWork, speedtype_Update, speedtype_Draw );
    gfl2::debug::DebugWin_AddItemUpdateFunc( mSpeedGroup, m_heap, m_pMsgData, msg_dmenu_resort_speedup_00, &mSpeedWork, speedtype_Update2, speedtype_Draw2 );
    gfl2::debug::DebugWin_AddItemUpdateFunc( mSpeedGroup, m_heap, m_pMsgData, msg_dmenu_resort_speedup_00, &mSpeedWork, speedtype_Update3, speedtype_Draw3 );
    gfl2::debug::DebugWin_AddItemUpdateFunc( mSpeedGroup, m_heap, m_pMsgData, msg_dmenu_resort_speedup_00, &mSpeedWork, speedtype_Update4, speedtype_Draw4 );

    // チュートリアル
    mTutorialGroup = gfl2::debug::DebugWin_AddGroup( m_heap, m_pMsgData, msg_dmenu_resort_tutorial_data, mEtcGroup );
    gfl2::debug::DebugWin_AddItemUpdateFunc( mTutorialGroup, m_heap, m_pMsgData, msg_dmenu_resort_tutorial_data, &mTutorialWork, tutorial_Update, speedtype_Draw );

    // フルモード
    mFullGroup = gfl2::debug::DebugWin_AddGroup( m_heap, m_pMsgData, msg_dmenu_resort_full, mEtcGroup );
    gfl2::debug::DebugWin_AddItemUpdateFunc( mFullGroup, m_heap, m_pMsgData, msg_dmenu_resort_full, &mTutorialWork, fullmode_Update, fullmode_Draw );

    // ＢＯＸのポケモンをアサイン
    mFureaiPokeGroup = gfl2::debug::DebugWin_AddGroup( m_heap, m_pMsgData, msg_dmenu_resort_fureai_poke_ins, mEtcGroup );
    gfl2::debug::DebugWin_AddItemUpdateFunc( mFureaiPokeGroup, m_heap, m_pMsgData, msg_dmenu_resort_fureai_poke_ins, &mFureaiPokeWork, fureaiPoke_Update, fureaiPoke_Draw );
    mFureaiPokeWork.cp = m_pTmpCoreParam;

    {
      // まめびん  // マメビン  // まめ瓶  // beansbottle  // beans_bottle  // beans bottle
      u8 h, m, s;

      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave()->GetBeansBottleServiceTime( &h, &m, &s );
      CreateTimerGroup( mEtcGroup, msg_dmenu_resort_mame_bin_timer, TIMER_BEANS_BOTTLE_EVENT, h, m, s );
    }

    {
      // デバッグ情報
      gfl2::debug::DebugWin_AddItemUpdateFunc( mEtcGroup, m_heap, m_pMsgData, msg_dmenu_resort_debug_info, NULL, debug_info_Update, debug_info_Draw );
    }
  }
}

ResortDebugWinImpl::~ResortDebugWinImpl()
{
  gfl2::debug::DebugWin_RemoveGroup(mSpeedGroup);
  gfl2::debug::DebugWin_RemoveGroup(mBeansGroup);
  for( u32 i=0; i<POKE_EV_GROUP_MAX; i++ )
  {
    gfl2::debug::DebugWin_RemoveGroup( mEventPokeGroup[i] );
  }
  for( u32 i=0; i<POKE_CLEAR_WORK_MAX; i++ )
  {
    gfl2::debug::DebugWin_RemoveGroup( mClearPokeGroup[i] );
  }
  for( u32 i=0; i<TIMER_WORK_MAX; i++ )
  {
    gfl2::debug::DebugWin_RemoveGroup( mTimerGroup[i] );
  }
  gfl2::debug::DebugWin_RemoveGroup(mEtcGroup);
  gfl2::debug::DebugWin_RemoveGroup(mCameraGroup);
  gfl2::debug::DebugWin_RemoveGroup(mTreasureGroup);
  gfl2::debug::DebugWin_RemoveGroup(mGimGroup);
  gfl2::debug::DebugWin_RemoveGroup(mHotspaGroup);
  gfl2::debug::DebugWin_RemoveGroup(mFarmGroup);
  gfl2::debug::DebugWin_RemoveGroup(mFriendShipGroup);
  gfl2::debug::DebugWin_RemoveGroup(mResortGroup);

  GFL_SAFE_DELETE(m_pTmpCoreParam);
  GFL_SAFE_DELETE(m_pClearCoreParam);
  GFL_SAFE_DELETE(m_strBuf);
  GFL_SAFE_DELETE(m_pWordSet);

  Terminate();
}



ResortDebugWinImpl* ResortDebugWinImpl::s_inst = NULL;

void     ResortDebugWinImpl::Initialize(ResortDebugWinImpl* inst)
{
  Savedata::ResortSave*  pResortSave   = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave();

  s_inst = inst;
}
void     ResortDebugWinImpl::Terminate(void)
{}

void     ResortDebugWinImpl::IslandFlag_Update(void* userWork, gfl2::debug::DebugWinItem* /*item*/)
{
}

wchar_t* ResortDebugWinImpl::IslandFlag_Draw  (void* userWork, gfl2::debug::DebugWinItem* item)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"  " );

  return workStr;
}


const wchar_t* ResortDebugWinImpl::GetResisterNumberPtr( u32 str_id, u32 val, u32 keta )
{
  gfl2::str::StrBuf * wk_str = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetWorkStrBuf();
  m_pMsgData->GetString( str_id, *wk_str );
  m_pWordSet->RegisterNumber( 0, val, keta, print::NUM_DISP_ZERO, print::NUM_CODE_DEFAULT );
  m_pWordSet->Expand( m_strBuf, wk_str );
  return m_strBuf->GetPtr();
}




void ResortDebugWinImpl::camera_Update(void* /*userWork*/, gfl2::debug::DebugWinItem* /*item*/)
{
  gfl2::ui::Button *uiButton = gfl2::debug::DebugWin_GetUiButton();
  gfl2::heap::HeapBase* heap       = s_inst->m_heap->GetLowerHandle();

  if( uiButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
  {
    if(s_inst->mbDebugCameraMode>=2) s_inst->mbDebugCameraMode=0;
    else s_inst->mbDebugCameraMode++;
  }
  else if( uiButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    if(s_inst->mbDebugCameraMode<=0) s_inst->mbDebugCameraMode=2;
    else s_inst->mbDebugCameraMode--;
  }
}

wchar_t* ResortDebugWinImpl::camera_Draw (void* /*userWork*/, gfl2::debug::DebugWinItem* item)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ %d ]", item->GetName(), s_inst->mbDebugCameraMode );
  return workStr;
}






void ResortDebugWinImpl::fullmode_Update(void* /*userWork*/, gfl2::debug::DebugWinItem* /*item*/)
{
  gfl2::ui::Button *uiButton = gfl2::debug::DebugWin_GetUiButton();
  gfl2::heap::HeapBase* heap       = s_inst->m_heap->GetLowerHandle();

  if( uiButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
  {
    s_inst->mbDebugFullMode = !s_inst->mbDebugFullMode;
  }
  else if( uiButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    s_inst->mbDebugFullMode = !s_inst->mbDebugFullMode;
  }
}

wchar_t* ResortDebugWinImpl::fullmode_Draw (void* /*userWork*/, gfl2::debug::DebugWinItem* item)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"FULL[ %d ]",  s_inst->mbDebugFullMode );
  return workStr;
}





void ResortDebugWinImpl::treasurelevel_Update(void* /*userWork*/, gfl2::debug::DebugWinItem* /*item*/)
{
  Savedata::ResortSave*  pResortSave   = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave();
  gfl2::ui::Button *uiButton = gfl2::debug::DebugWin_GetUiButton();
  gfl2::heap::HeapBase* heap       = s_inst->m_heap->GetLowerHandle();
  int lv = pResortSave->GetResortLevel(Savedata::ResortSave::RESORT_KIND_ADVENTURE);
  if( uiButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
  {
    lv--;
    if(lv<0){
      lv=0;
    }
  }
  else if( uiButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    lv++;
    if(lv>=4){
      lv=3;
    }
  }
  pResortSave->SetResortLevel(Savedata::ResortSave::RESORT_KIND_ADVENTURE,lv);
}

wchar_t* ResortDebugWinImpl::treasurelevel_Draw (void* /*userWork*/, gfl2::debug::DebugWinItem* item)
{
  Savedata::ResortSave*  pResortSave   = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave();
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ %d ]", item->GetName(), pResortSave->GetResortLevel(Savedata::ResortSave::RESORT_KIND_ADVENTURE) );
  return workStr;
}


void ResortDebugWinImpl::farmlevel_Update(void* /*userWork*/, gfl2::debug::DebugWinItem* /*item*/)
{
  Savedata::ResortSave*  pResortSave   = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave();
  gfl2::ui::Button *uiButton = gfl2::debug::DebugWin_GetUiButton();
  gfl2::heap::HeapBase* heap       = s_inst->m_heap->GetLowerHandle();
  int lv = pResortSave->GetResortLevel(Savedata::ResortSave::RESORT_KIND_FARM);
  if( uiButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
  {
    lv--;
    if(lv<0){
      lv=0;
    }
  }
  else if( uiButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    lv++;
    if(lv>=4){
      lv=3;
    }
  }
  pResortSave->SetResortLevel(Savedata::ResortSave::RESORT_KIND_FARM,lv);
}

wchar_t* ResortDebugWinImpl::farmlevel_Draw (void* /*userWork*/, gfl2::debug::DebugWinItem* item)
{
  Savedata::ResortSave*  pResortSave   = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave();
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ %d ]", item->GetName(), pResortSave->GetResortLevel(Savedata::ResortSave::RESORT_KIND_FARM) );
  return workStr;
}



void ResortDebugWinImpl::hotspalevel_Update(void* /*userWork*/, gfl2::debug::DebugWinItem* /*item*/)
{
  Savedata::ResortSave*  pResortSave   = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave();
  gfl2::ui::Button *uiButton = gfl2::debug::DebugWin_GetUiButton();
  gfl2::heap::HeapBase* heap       = s_inst->m_heap->GetLowerHandle();
  int lv = pResortSave->GetResortLevel(Savedata::ResortSave::RESORT_KIND_HOTSPA);
  if( uiButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
  {
    lv--;
    if(lv<0){
      lv=0;
    }
  }
  else if( uiButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    lv++;
    if(lv>=4){
      lv=3;
    }
  }
  pResortSave->SetResortLevel(Savedata::ResortSave::RESORT_KIND_HOTSPA,lv);
}

wchar_t* ResortDebugWinImpl::hotspalevel_Draw (void* /*userWork*/, gfl2::debug::DebugWinItem* item)
{
  Savedata::ResortSave*  pResortSave   = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave();
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ %d ]", item->GetName(), pResortSave->GetResortLevel(Savedata::ResortSave::RESORT_KIND_HOTSPA) );
  return workStr;
}


void ResortDebugWinImpl::gimlevel_Update(void* /*userWork*/, gfl2::debug::DebugWinItem* /*item*/)
{
  Savedata::ResortSave*  pResortSave   = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave();
  gfl2::ui::Button *uiButton = gfl2::debug::DebugWin_GetUiButton();
  gfl2::heap::HeapBase* heap       = s_inst->m_heap->GetLowerHandle();
  int lv = pResortSave->GetResortLevel(Savedata::ResortSave::RESORT_KIND_GIM);
  if( uiButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
  {
    lv--;
    if(lv<0){
      lv=0;
    }
  }
  else if( uiButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    lv++;
    if(lv>=4){
      lv=3;
    }
  }
  pResortSave->SetResortLevel(Savedata::ResortSave::RESORT_KIND_GIM,lv);
}

wchar_t* ResortDebugWinImpl::gimlevel_Draw (void* /*userWork*/, gfl2::debug::DebugWinItem* item)
{
  Savedata::ResortSave*  pResortSave   = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave();
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ %d ]", item->GetName(), pResortSave->GetResortLevel(Savedata::ResortSave::RESORT_KIND_GIM) );
  return workStr;
}


void ResortDebugWinImpl::friendshiplevel_Update(void* /*userWork*/, gfl2::debug::DebugWinItem* /*item*/)
{
  Savedata::ResortSave*  pResortSave   = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave();
  gfl2::ui::Button *uiButton = gfl2::debug::DebugWin_GetUiButton();
  gfl2::heap::HeapBase* heap       = s_inst->m_heap->GetLowerHandle();
  int lv = pResortSave->GetResortLevel(Savedata::ResortSave::RESORT_KIND_FRIENDSHIP);
  if( uiButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
  {
    lv--;
    if(lv<0){
      lv=0;
    }
  }
  else if( uiButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    lv++;
    if(lv>=4){
      lv=3;
    }
  }
  pResortSave->SetResortLevel(Savedata::ResortSave::RESORT_KIND_FRIENDSHIP,lv);
}

wchar_t* ResortDebugWinImpl::friendshiplevel_Draw (void* /*userWork*/, gfl2::debug::DebugWinItem* item)
{
  Savedata::ResortSave*  pResortSave   = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave();
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ %d ]", item->GetName(), pResortSave->GetResortLevel(Savedata::ResortSave::RESORT_KIND_FRIENDSHIP) );
  return workStr;
}


void ResortDebugWinImpl::SetTimerWork( u8 id, u8 h, u8 m, u8 s )
{
  TimerWork * wk = &mTimerWork[id];
  wk->id = id;
  wk->count[0].val = h;
  wk->count[0].max = 99;
  wk->count[1].val = m;
  wk->count[1].max = 59;
  wk->count[2].val = s;
  wk->count[2].max = 59;
}

void ResortDebugWinImpl::CreateTimerGroup( gfl2::debug::DebugWinGroup * parent, u32 str, u8 id, u8 h, u8 m, u8 s )
{
  SetTimerWork( id, h, m, s );

  mTimerGroup[id] = gfl2::debug::DebugWin_AddGroup( m_heap, m_pMsgData, str, parent );

  TimerWork * wk = &mTimerWork[id];
  gfl2::debug::DebugWin_AddItemUpdateFunc( mTimerGroup[id], m_heap, m_pMsgData, msg_dmenu_time_hour, &wk->count[0], timer_Update, timer_Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( mTimerGroup[id], m_heap, m_pMsgData, msg_dmenu_time_min,  &wk->count[1], timer_Update, timer_Draw );
//  gfl2::debug::DebugWin_AddItemUpdateFunc( mTimerGroup[id], m_heap, m_pMsgData, msg_dmenu_time_sec,  &wk->count[2], timer_Update, timer_Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( mTimerGroup[id], m_heap, m_pMsgData, msg_dmenu_resort_decide, wk, timerdecide_Update, NULL );
}

void ResortDebugWinImpl::CreateTimerGroup( gfl2::debug::DebugWinGroup * parent, const wchar_t * str, u8 id, u8 h, u8 m, u8 s )
{
  SetTimerWork( id, h, m, s );

  mTimerGroup[id] = gfl2::debug::DebugWin_AddGroup( m_heap, str, parent );

  TimerWork * wk = &mTimerWork[id];
  gfl2::debug::DebugWin_AddItemUpdateFunc( mTimerGroup[id], m_heap, m_pMsgData, msg_dmenu_time_hour, &wk->count[0], timer_Update, timer_Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( mTimerGroup[id], m_heap, m_pMsgData, msg_dmenu_time_min,  &wk->count[1], timer_Update, timer_Draw );
//  gfl2::debug::DebugWin_AddItemUpdateFunc( mTimerGroup[id], m_heap, m_pMsgData, msg_dmenu_time_sec,  &wk->count[2], timer_Update, timer_Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( mTimerGroup[id], m_heap, m_pMsgData, msg_dmenu_resort_decide, wk, timerdecide_Update, NULL );
}

void ResortDebugWinImpl::timer_Update(void* userWork, gfl2::debug::DebugWinItem* /*item*/)
{
  TimerCount * count = static_cast<TimerCount*>( userWork );

  gfl2::ui::Button * button = gfl2::debug::DebugWin_GetUiButton();

  if( button->IsRepeat(gfl2::ui::BUTTON_LEFT) )
  {
    if( count->val == 0 )
    {
      count->val = count->max;
    }
    else
    {
      count->val--;
    }
  }
  else if( button->IsRepeat(gfl2::ui::BUTTON_RIGHT) )
  {
    if( count->val >= count->max )
    {
      count->val = 0;
    }
    else
    {
      count->val++;
    }
  }
}

wchar_t* ResortDebugWinImpl::timer_Draw(void* userWork, gfl2::debug::DebugWinItem* item)
{
  s8 * wk = static_cast<s8 *>( userWork );
  wchar_t * work_str = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( work_str, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ %d ]", item->GetName(), *wk );
  return work_str;
}

void ResortDebugWinImpl::timerdecide_Update(void* userWork, gfl2::debug::DebugWinItem* /*item*/)
{
  if( gfl2::debug::DebugWin_GetUiButton()->IsTrigger(gfl2::ui::BUTTON_A) )
  {
    TimerWork * wk = static_cast<TimerWork *>( userWork );
    Savedata::ResortSave * save = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave();

    if( wk->id == TIMER_FRIEND_SHIP_WILD_POKEMON )
    {
      save->SetWildPokemonTime( wk->count[0].val, wk->count[1].val, wk->count[2].val );
    }
    else if( wk->id == TIMER_FRIEND_SHIP_WILD_POKEMON_SERVICE )
    {
      save->SetWildPokemonServiceTime( wk->count[0].val, wk->count[1].val, wk->count[2].val );
    }
    else if( wk->id == TIMER_FRIEND_SHIP_TIME )
    {
      save->SetFriendShipTime( wk->count[0].val, wk->count[1].val, wk->count[2].val );
    }
    else if( wk->id >= TIMER_FARM_TIME && wk->id < TIMER_FARM_FERTILIZER )
    {
      save->SetFarmTime( wk->id-TIMER_FARM_TIME, wk->count[0].val, wk->count[1].val, wk->count[2].val );
    }
    else if( wk->id == TIMER_FARM_FERTILIZER )
    {
      save->SetFarmFertilizerTime( wk->count[0].val, wk->count[1].val, wk->count[2].val );
    }
    else if( wk->id == TIMER_ADVENTURE_COUNT )
    {
      save->SetAdventureTime( wk->count[0].val, wk->count[1].val, wk->count[2].val );
    }
    else if( wk->id == TIMER_ADVENTURE_BOUNUS )
    {
      save->SetAdventureBonusTime( wk->count[0].val, wk->count[1].val, wk->count[2].val );
    }
    else if( wk->id == TIMER_GIM_BOUNUS )
    {
      save->SetGimBonusTime( wk->count[0].val, wk->count[1].val, wk->count[2].val );
    }
    else if( wk->id >= TIMER_GIM_END && wk->id < TIMER_HOT_SPA_BOUNUS )
    {
      save->SetGimEndTime( wk->id-TIMER_GIM_END, wk->count[0].val, wk->count[1].val, wk->count[2].val );
    }
    else if( wk->id == TIMER_HOT_SPA_BOUNUS )
    {
      save->SetHotSpaBonusTime( wk->count[0].val, wk->count[1].val, wk->count[2].val );
    }
    else if( wk->id >= TIMER_HOT_SPA_POKE && wk->id < TIMER_HOT_SPA_NOBOSE )
    {
      save->SetHotSpaPokemonTime( wk->id-TIMER_HOT_SPA_POKE, wk->count[0].val, wk->count[1].val, wk->count[2].val );
    }
    else if( wk->id >= TIMER_HOT_SPA_NOBOSE && wk->id < TIMER_POKEMON_EVENT )
    {
      save->SetSpaNoboseTime( wk->id-TIMER_HOT_SPA_NOBOSE, wk->count[0].val, wk->count[1].val, wk->count[2].val );
    }
    else if( wk->id == TIMER_POKEMON_EVENT )
    {
      save->SetPokemonEventTime( wk->count[0].val, wk->count[1].val, wk->count[2].val );
    }
    /*
    else if( wk->id == TIMER_BOX_POKEMON_POS )
    {
    }
     */
    else if( wk->id == TIMER_BEANS_BOTTLE_EVENT )
    {
      save->SetBeansBottleServiceTime( wk->count[0].val, wk->count[1].val, wk->count[2].val );
    }
  }
}

void ResortDebugWinImpl::CreatePokeClearGroup( gfl2::debug::DebugWinGroup * parent, u32 str, u8 id, u8 max )
{
  mClearPokeGroup[id] = gfl2::debug::DebugWin_AddGroup( m_heap, m_pMsgData, str, parent );

  PokeClearWork * wk = &mPokeClearWork[id];
  wk->id  = id;
  wk->max = max;
  wk->cp  = m_pClearCoreParam;
  gfl2::debug::DebugWin_AddItemUpdateFunc( mClearPokeGroup[id], m_heap, m_pMsgData, msg_dmenu_resort_clear_pokemon_01, wk, pokeclear_Update, pokeclear_Draw );
  gfl2::debug::DebugWin_AddItemUpdateFunc( mClearPokeGroup[id], m_heap, m_pMsgData, msg_dmenu_resort_decide, wk, pokecleardecide_Update, NULL );
}

void ResortDebugWinImpl::pokeclear_Update(void* userWork, gfl2::debug::DebugWinItem* /*item*/)
{
  PokeClearWork * wk = static_cast<PokeClearWork*>( userWork );

  gfl2::ui::Button * button = gfl2::debug::DebugWin_GetUiButton();

  if( button->IsTrigger(gfl2::ui::BUTTON_LEFT) )
  {
    if( wk->index == 0 )
    {
      wk->index = wk->max-1;
    }
    else
    {
      wk->index--;
    }
  }
  else if( button->IsTrigger(gfl2::ui::BUTTON_RIGHT) )
  {
    if( wk->index >= (wk->max-1) )
    {
      wk->index = 0;
    }
    else
    {
      wk->index++;
    }
  }
}

wchar_t* ResortDebugWinImpl::pokeclear_Draw(void* userWork, gfl2::debug::DebugWinItem* item)
{
  PokeClearWork * wk = static_cast<PokeClearWork*>( userWork );
  wchar_t * work_str = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( work_str, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ %d ]", item->GetName(), wk->index );
  return work_str;
}

void ResortDebugWinImpl::pokecleardecide_Update(void* userWork, gfl2::debug::DebugWinItem* /*item*/)
{
  if( gfl2::debug::DebugWin_GetUiButton()->IsTrigger(gfl2::ui::BUTTON_A) )
  {
    PokeClearWork * wk = static_cast<PokeClearWork*>( userWork );
    Savedata::ResortSave * save = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave();

    switch( wk->id )
    {
    case POKE_CLEAR_FIRIEND_SHIP:
      save->SetFriendShipPokemon( wk->index, wk->cp,0,0 );
      break;
    case POKE_CLEAR_FARM:
      save->SetFarmPokemon( wk->index, wk->cp,0,0 );
      break;
    case POKE_CLEAR_ADVENTURE:
      save->SetAdventurePokemon( wk->index, wk->cp,0,0 );
      break;
    case POKE_CLEAR_WILD:
      save->SetWildPokemon( wk->index, wk->cp );
      break;
    }
  }
}

void ResortDebugWinImpl::beanstype_Update(void* userWork, gfl2::debug::DebugWinItem* /*item*/)
{
  BeansWork * wk = static_cast<BeansWork*>( userWork );

  gfl2::ui::Button * button = gfl2::debug::DebugWin_GetUiButton();

  if( button->IsTrigger(gfl2::ui::BUTTON_LEFT) )
  {
    if( wk->index == 0 )
    {
      wk->index = Savedata::ResortSave::POKEBEANS_KIND_MAX-1;
    }
    else
    {
      wk->index--;
    }
  }
  else if( button->IsTrigger(gfl2::ui::BUTTON_RIGHT) )
  {
    if( wk->index >= (Savedata::ResortSave::POKEBEANS_KIND_MAX-1) )
    {
      wk->index = 0;
    }
    else
    {
      wk->index++;
    }
  }
}

wchar_t* ResortDebugWinImpl::beanstype_Draw(void* userWork, gfl2::debug::DebugWinItem* item)
{
  BeansWork * wk = static_cast<BeansWork*>( userWork );
  wchar_t * work_str = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( work_str, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ %d ]", item->GetName(), wk->index );
  return work_str;
}

void ResortDebugWinImpl::beansnum_Update(void* userWork, gfl2::debug::DebugWinItem* /*item*/)
{
  BeansWork * wk = static_cast<BeansWork*>( userWork );

  gfl2::ui::Button * button = gfl2::debug::DebugWin_GetUiButton();

  if( button->IsTrigger(gfl2::ui::BUTTON_LEFT) )
  {
    if( wk->val == 0 )
    {
      wk->val = 255;
    }
    else
    {
      wk->val--;
    }
  }
  else if( button->IsTrigger(gfl2::ui::BUTTON_RIGHT) )
  {
    if( wk->val >= 255 )
    {
      wk->val = 0;
    }
    else
    {
      wk->val++;
    }
  }
}

wchar_t* ResortDebugWinImpl::beansnum_Draw(void* userWork, gfl2::debug::DebugWinItem* item)
{
  BeansWork * wk = static_cast<BeansWork*>( userWork );
  wchar_t * work_str = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( work_str, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ %d ]", item->GetName(), wk->val );
  return work_str;
}




void ResortDebugWinImpl::beansbottle_Update(void* userWork, gfl2::debug::DebugWinItem* /*item*/)
{
  BeansWork * wk = static_cast<BeansWork*>( userWork );

  gfl2::ui::Button * button = gfl2::debug::DebugWin_GetUiButton();

  if( button->IsTrigger(gfl2::ui::BUTTON_LEFT) )
  {
    Savedata::ResortSave * save = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave();
    for(int i=0;i<5;i++){
      save->SetBeansBottleGround( static_cast<Savedata::ResortSave::PokeResortKind>(i),  true);
      wk->onoff = true;
    }
  }
  else if( button->IsTrigger(gfl2::ui::BUTTON_RIGHT) )
  {
    Savedata::ResortSave * save = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave();
    for(int i=0;i<5;i++){
      save->SetBeansBottleGround( static_cast<Savedata::ResortSave::PokeResortKind>(i),  false);
      wk->onoff = false;
    }
  }
}

wchar_t* ResortDebugWinImpl::beansbottle_Draw(void* userWork, gfl2::debug::DebugWinItem* item)
{
  BeansWork * wk = static_cast<BeansWork*>( userWork );
  wchar_t * work_str = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( work_str, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ %d ]", item->GetName(), wk->onoff );
  return work_str;
}





void ResortDebugWinImpl::beansdecide_Update(void* userWork, gfl2::debug::DebugWinItem* /*item*/)
{
  if( gfl2::debug::DebugWin_GetUiButton()->IsTrigger(gfl2::ui::BUTTON_A) )
  {
    BeansWork * wk = static_cast<BeansWork*>( userWork );
    Savedata::ResortSave * save = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave();

    int add = static_cast<int>(wk->val) - static_cast<int>(save->GetPokeBeansNum(static_cast<Savedata::ResortSave::PokeBeansEnum>(wk->index)));
    if( add != 0 )
    {
      save->SetPokeBeansNum(static_cast<Savedata::ResortSave::PokeBeansEnum>(wk->index), add );
    }
  }
}

void ResortDebugWinImpl::CreatePokeEventGroup( gfl2::debug::DebugWinGroup * parent, u32 str, u8 id, u8 start, u8 max )
{
  mEventPokeGroup[id] = gfl2::debug::DebugWin_AddGroup( m_heap, m_pMsgData, str, parent );

  PokeEventWork * wk = &mPokeEventWork[start];
  for( u32 i=0; i<max; i++ )
  {
    wk[i].id    = id;
    wk[i].index = i;
    wk[i].cp    = m_pTmpCoreParam;
    gfl2::debug::DebugWin_AddItemUpdateFunc( mEventPokeGroup[id], m_heap, m_pMsgData, msg_dmenu_resort_etc_05, &wk[i], NULL, pokeevent_Draw );
  }
}

wchar_t* ResortDebugWinImpl::pokeevent_Draw(void* userWork, gfl2::debug::DebugWinItem* item)
{
  PokeEventWork * wk = static_cast<PokeEventWork*>( userWork );
  Savedata::ResortSave * save = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave();
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  switch( wk->id )
  {
  case POKE_EV_GROUP_FRIEND_SHIP:
    save->GetFriendShipPokemon( wk->index, wk->cp );
    break;
  case POKE_EV_GROUP_WILD:
    save->GetWildPokemon( wk->index, wk->cp );
    break;
  case POKE_EV_GROUP_FARM:
    save->GetFarmPokemon( wk->index, wk->cp );
    break;
  case POKE_EV_GROUP_ADVENTURE:
    save->GetAdventurePokemon( wk->index, wk->cp );
    break;
  case POKE_EV_GROUP_GIM:
    save->GetGimPokemon( wk->index, wk->cp );
    break;
  case POKE_EV_GROUP_HOT_SPA:
    save->GetHotSpaPokemon( wk->index, wk->cp );
    break;
  }

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"[%d] %ls[ %d ] [ %d ]", wk->index, item->GetName(), wk->cp->GetMonsNo(), wk->cp->GetResortEventStatus() );

  return workStr;
}


s32 ResortDebugWinImpl::Util_S32_Update(s32 currVal, s32 minVal, s32 maxVal, bool isLoop, bool isJump)
{
  gfl2::ui::Button* uiButton = gfl2::debug::DebugWin_GetUiButton();

  s32 retVal = currVal;

  s32 delta = 1;
  if(isJump)
  {
    if( uiButton->IsHold( gfl2::ui::BUTTON_R ) )
    {
      delta = 100;
    }
    else if( uiButton->IsHold( gfl2::ui::BUTTON_L ) )
    {
      delta = 10;
    }
  }

  if(currVal == minVal)
  {
    if( uiButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
    {
      if(isLoop)
      {
        retVal = maxVal;
      }
    }
    else if( uiButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
    {
      retVal = currVal + delta;
    }
  }
  else if(currVal == maxVal)
  {
    if( uiButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
    {
      if(isLoop)
      {
        retVal = minVal;
      }
    }
    else if( uiButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
    {
      retVal = currVal - delta;
    }
  }
  else
  {
    if( uiButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
    {
      retVal = currVal + delta;
    }
    else if( uiButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
    {
      retVal = currVal - delta;
    }
  }

  if(retVal < minVal)
  {
    retVal = minVal;
  }
  else if(retVal > maxVal)
  {
    retVal = maxVal;
  }

  return retVal;
}




void ResortDebugWinImpl::speedtype_Update(void* userWork, gfl2::debug::DebugWinItem* /*item*/)
{
  SpeedWork * wk = static_cast<SpeedWork*>( userWork );

  gfl2::ui::Button * button = gfl2::debug::DebugWin_GetUiButton();

  if( button->IsTrigger(gfl2::ui::BUTTON_LEFT) )
  {
    if( wk->val == 0 )
    {
      wk->val = 5000;
    }
    else
    {
      wk->val-=100;
    }
  }
  else if( button->IsTrigger(gfl2::ui::BUTTON_RIGHT) )
  {
    if( wk->val >= 5000 )
    {
      wk->val = 0;
    }
    else
    {
      wk->val+=100;
    }
  }
}

wchar_t* ResortDebugWinImpl::speedtype_Draw(void* userWork, gfl2::debug::DebugWinItem* item)
{
  SpeedWork * wk = static_cast<SpeedWork*>( userWork );
  wchar_t * work_str = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( work_str, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ %d ]", item->GetName(), wk->val );
  return work_str;
}


void ResortDebugWinImpl::speedtype_UpdateX(void* userWork, gfl2::debug::DebugWinItem* /*item*/, int num)
{
  SpeedWork * wk = static_cast<SpeedWork*>( userWork );
  gfl2::ui::Button * button = gfl2::debug::DebugWin_GetUiButton();
  if( button->IsTrigger(gfl2::ui::BUTTON_A) )
  {
    wk->val = num;
  }
}

wchar_t* ResortDebugWinImpl::speedtype_DrawX(void* userWork, gfl2::debug::DebugWinItem* item, int num)
{
  SpeedWork * wk = static_cast<SpeedWork*>( userWork );
  wchar_t * work_str = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( work_str, gfl2::debug::DEBUGWIN_NAME_LEN, L"[ %d ]", num );
  return work_str;
}




void ResortDebugWinImpl::speedtype_Update2(void* userWork, gfl2::debug::DebugWinItem* /*item*/)
{
  speedtype_UpdateX(userWork, NULL, 3600);
}

wchar_t* ResortDebugWinImpl::speedtype_Draw2(void* userWork, gfl2::debug::DebugWinItem* item)
{
  return speedtype_DrawX(userWork, item, 3600);
}


void ResortDebugWinImpl::speedtype_Update3(void* userWork, gfl2::debug::DebugWinItem* /*item*/)
{
  speedtype_UpdateX(userWork, NULL, 60);
}

wchar_t* ResortDebugWinImpl::speedtype_Draw3(void* userWork, gfl2::debug::DebugWinItem* item)
{
  return speedtype_DrawX(userWork, item, 60);
}

void ResortDebugWinImpl::speedtype_Update4(void* userWork, gfl2::debug::DebugWinItem* /*item*/)
{
  speedtype_UpdateX(userWork, NULL, 1);
}

wchar_t* ResortDebugWinImpl::speedtype_Draw4(void* userWork, gfl2::debug::DebugWinItem* item)
{
  return speedtype_DrawX(userWork, item, 1);
}






void ResortDebugWinImpl::tutorial_Update(void* userWork, gfl2::debug::DebugWinItem* /*item*/)
{
  TutorialWork * wk = static_cast<TutorialWork*>( userWork );

  gfl2::ui::Button * button = gfl2::debug::DebugWin_GetUiButton();
  Savedata::ResortSave*  pResortSave   = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave();

  if( button->IsTrigger(gfl2::ui::BUTTON_LEFT) )
  {
    wk->val = 1;
    pResortSave->DebugSetResortProgress(0xffffffff);
  }
  else if( button->IsTrigger(gfl2::ui::BUTTON_RIGHT) )
  {
    wk->val = 0;
    pResortSave->DebugSetResortProgress(0);
  }
}

wchar_t* ResortDebugWinImpl::tutorial_Draw(void* userWork, gfl2::debug::DebugWinItem* item)
{
  TutorialWork * wk = static_cast<TutorialWork*>( userWork );
  wchar_t * work_str = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( work_str, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ %d ]", item->GetName(), wk->val );
  return work_str;
}




void ResortDebugWinImpl::BeansCounter_Update(void* /*userWork*/, gfl2::debug::DebugWinItem* /*item*/)
{
  gfl2::ui::Button * button = gfl2::debug::DebugWin_GetUiButton();
  Savedata::ResortSave*  pResortSave   = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave();

  if( button->IsTrigger(gfl2::ui::BUTTON_LEFT) )
  {
    u8 mame = pResortSave->GetBeansGetCount();
    mame++;
    pResortSave->SetBeansGetCount(mame);
  }
  else if( button->IsTrigger(gfl2::ui::BUTTON_RIGHT) )
  {
    u8 mame = pResortSave->GetBeansGetCount();
    mame--;
    pResortSave->SetBeansGetCount(mame);
  }
}

wchar_t* ResortDebugWinImpl::BeansCounter_Draw(void* userWork, gfl2::debug::DebugWinItem* item)
{
  wchar_t * work_str = gfl2::debug::DebugWin_GetWorkStr();
  Savedata::ResortSave*  pResortSave   = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave();
  u8 mame = pResortSave->GetBeansGetCount();

  swprintf( work_str, gfl2::debug::DEBUGWIN_NAME_LEN, L"count[ %d ]", mame );
  return work_str;
}






void ResortDebugWinImpl::fureaiPoke_Update(void* userWork, gfl2::debug::DebugWinItem* item)
{
  FureaiPokeWork * wk = static_cast<FureaiPokeWork*>( userWork );

  gfl2::ui::Button * button = gfl2::debug::DebugWin_GetUiButton();
  Savedata::ResortSave*  pResortSave   = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetResortSave();
  Savedata::BoxPokemon*  pBoxSave   = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBoxPokemon();
  bool bupdate=false;

  if( button->IsTrigger(gfl2::ui::BUTTON_R) )
  {
    if(wk->box!=31)   wk->box++;
    else wk->box = 0;
    bupdate=true;
  }
  if( button->IsTrigger(gfl2::ui::BUTTON_L) )
  {
    if(wk->box!=0)   wk->box--;
    else wk->box = 31;
    bupdate=true;
  }
  if( button->IsTrigger(gfl2::ui::BUTTON_RIGHT) )
  {
    if(wk->pos!=29)   wk->pos++;
    else wk->pos = 0;
    bupdate=true;
  }
  if( button->IsTrigger(gfl2::ui::BUTTON_LEFT) )
  {
    if(wk->pos!=0)   wk->pos--;
    else wk->pos = 29;
    bupdate=true;
  }

  if(bupdate){
    pBoxSave->GetPokemon( wk->cp, wk->box, wk->pos );
    wk->mons = wk->cp->GetMonsNo();
    wk->form = wk->cp->GetFormNo();
  }
  
  if( button->IsTrigger(gfl2::ui::BUTTON_A) )
  {
    pBoxSave->GetPokemon( wk->cp, wk->box, wk->pos );

    switch(pResortSave->GetSelectResortPos()){
    case Savedata::ResortSave::RESORT_KIND_FRIENDSHIP:
      pResortSave->SetFriendShipPokemon(0, wk->cp, 0,0);
      break;
    case Savedata::ResortSave::RESORT_KIND_FARM:
      pResortSave->SetFarmPokemon(0, wk->cp,0,0);
      break;
    case Savedata::ResortSave::RESORT_KIND_ADVENTURE:
      pResortSave->SetAdventurePokemon(0, wk->cp,0,0);
      break;
    case Savedata::ResortSave::RESORT_KIND_GIM:
      pResortSave->SetGimPokemon(0, wk->cp);
      break;
    case Savedata::ResortSave::RESORT_KIND_HOTSPA:
      pResortSave->SetHotSpaPokemon(0, wk->cp);
			break;
    }
  }

}

wchar_t* ResortDebugWinImpl::fureaiPoke_Draw(void* userWork, gfl2::debug::DebugWinItem* item)
{
  FureaiPokeWork * wk = static_cast<FureaiPokeWork*>( userWork );
  wchar_t * work_str = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( work_str, gfl2::debug::DEBUGWIN_NAME_LEN, L"box %d %d - mons %d form %d", wk->box, wk->pos,wk->mons,wk->form );
  return work_str;
}


void     ResortDebugWinImpl::debug_info_Update (void* /*userWork*/, gfl2::debug::DebugWinItem* /*item*/)
{
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
  gfl2::ui::Button *uiButton = gfl2::debug::DebugWin_GetUiButton();
  s32 dispVal = ResortDebugInfo::s_ResortDebugInfo.GetUserVal();
  if( uiButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    --dispVal;
    if(dispVal < 0) dispVal = ResortDebugInfo::TIME_PHASE_RESORT_MAX -1;
  }
  else if( uiButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    ++dispVal;
    if(dispVal >= ResortDebugInfo::TIME_PHASE_RESORT_MAX) dispVal = 0;
  }
  ResortDebugInfo::s_ResortDebugInfo.SetUserVal(dispVal);
#endif
}
wchar_t* ResortDebugWinImpl::debug_info_Draw   (void* /*userWork*/, gfl2::debug::DebugWinItem* item)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  s32 dispVal    = 0;
  s64 dispNumber = 0;

#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
  dispVal    = ResortDebugInfo::s_ResortDebugInfo.GetUserVal();
  dispNumber = ResortDebugInfo::s_ResortDebugInfo.GetTime(dispVal);
#endif

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]%lld", item->GetName(), dispVal, dispNumber );
  return workStr;
}


// タマゴ・ダメタマゴ
// タマゴ：うんどう
void ResortDebugWinImpl::UpdateGimSetTamago( void * userWork, gfl2::debug::DebugWinItem * item )
{
  GFL_UNUSED( userWork );
  GFL_UNUSED( item );
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::Button * button = gm->GetUiDeviceManager()->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );

  if( button->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    Savedata::ResortSave * sv_resort = gm->GetGameData()->GetResortSave();
    gfl2::heap::HeapBase * heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );

    for( u32 i=0; i<Savedata::ResortSave::RESORT_GIM_KIND_MAX; i++ )
    {
      pml::pokepara::CoreParam * cp = GFL_NEW_LOW(heap) pml::pokepara::CoreParam( heap );
      sv_resort->GetGimPokemon( i*Savedata::ResortSave::RESORT_GIM_KIND_NUM_MAX, cp );
      MonsNo mons = cp->GetMonsNo();
      if( mons != MONSNO_NULL && mons != MONSNO_TAMAGO && mons != MONSNO_DAMETAMAGO )
      {
        cp->ChangeEgg( heap );
        sv_resort->SetGimPokemon( i*Savedata::ResortSave::RESORT_GIM_KIND_NUM_MAX, cp );
      }
      GFL_DELETE cp;
    }
  }
#endif
}

// ダメタマゴ：うんどう
void ResortDebugWinImpl::UpdateGimSetDameTamago( void * userWork, gfl2::debug::DebugWinItem * item )
{
  GFL_UNUSED( userWork );
  GFL_UNUSED( item );
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::Button * button = gm->GetUiDeviceManager()->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );

  if( button->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    Savedata::ResortSave * sv_resort = gm->GetGameData()->GetResortSave();
    gfl2::heap::HeapBase * heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );

    for( u32 i=0; i<Savedata::ResortSave::RESORT_GIM_KIND_MAX; i++ )
    {
      pml::pokepara::CoreParam * cp = GFL_NEW_LOW(heap) pml::pokepara::CoreParam( heap );
      sv_resort->GetGimPokemon( i*Savedata::ResortSave::RESORT_GIM_KIND_NUM_MAX, cp );
      MonsNo mons = cp->GetMonsNo();
      if( mons != MONSNO_NULL && mons != MONSNO_TAMAGO && mons != MONSNO_DAMETAMAGO )
      {
        cp->SetFuseiTamagoFlag();
        sv_resort->SetGimPokemon( i*Savedata::ResortSave::RESORT_GIM_KIND_NUM_MAX, cp );
      }
      GFL_DELETE cp;
    }
  }
#endif
}

// ダメタマゴ：おんせん
void ResortDebugWinImpl::UpdateHotspaSetDameTamago( void * userWork, gfl2::debug::DebugWinItem * item )
{
  GFL_UNUSED( userWork );
  GFL_UNUSED( item );
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::Button * button = gm->GetUiDeviceManager()->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );

  if( button->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    Savedata::ResortSave * sv_resort = gm->GetGameData()->GetResortSave();
    gfl2::heap::HeapBase * heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );

    for( u32 i=0; i<Savedata::ResortSave::RESORT_HOTSPA_KIND_MAX; i++ )
    {
      pml::pokepara::CoreParam * cp = GFL_NEW_LOW(heap) pml::pokepara::CoreParam( heap );
      sv_resort->GetHotSpaPokemon( i*6, cp );
      MonsNo mons = cp->GetMonsNo();
      if( mons != MONSNO_NULL && mons != MONSNO_TAMAGO && mons != MONSNO_DAMETAMAGO )
      {
        cp->SetFuseiTamagoFlag();
        sv_resort->SetHotSpaPokemon( i*6, cp );
      }
      GFL_DELETE cp;
    }
  }
#endif
}



//==============================================================================
//
//  ResortDebugWin
//
//==============================================================================
ResortDebugWin::ResortDebugWin(gfl2::heap::HeapBase* heap, gfl2::debug::DebugWinGroup* parentGroup)
   : m_heap(heap),
     m_parentGroup(parentGroup),
     m_impl(NULL),
     m_pMsgData(NULL)
{
  ::Debug::DebugWin::DebugWinWork* devWork = GFL_SINGLETON_INSTANCE( ::Debug::DebugWin::DebugWinWork );
  if( devWork ) m_pMsgData = devWork->GetMessageData();
  m_impl = GFL_NEW(heap) ResortDebugWinImpl(m_heap, m_parentGroup, m_pMsgData);
}
ResortDebugWin::~ResortDebugWin()
{
  GFL_SAFE_DELETE(m_impl);
}

int ResortDebugWin::GetDebugCameraMode(void)
{
  return m_impl->GetCameraDebugMode();
}

bool ResortDebugWin::GetDebugFullMode(void)
{
  return m_impl->GetDebugFullMode();
}


int ResortDebugWin::GetSpeedWork(void)
{
  return m_impl->GetSpeedVal();
}



GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)


#endif  // PM_DEBUG

