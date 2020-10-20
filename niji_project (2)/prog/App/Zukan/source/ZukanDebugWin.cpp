#if PM_DEBUG


//==============================================================================
/**
 @file    ZukanDebugWinImpl.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.11
 @brief   図鑑のデバッグウィンドウ
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
#include <Savedata/include/ZukanSave.h>
#include <System/include/PokemonVersion.h>
#include <System/include/GflUse.h>
#include <Sound/include/Sound.h>

// メッセージのインデックス
#include <Debug/DebugWin/include/DebugWinSystem.h>
#include "niji_conv_header/message/debug/msg_debug_menu.h"

// 図鑑のインクルード
#include "ZukanPrismParam.h"  // NIJI_APP_ZUKAN_PRISM_DEVELOPの部分だけ使用している

#include "ZukanDebugWin.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanDebugWinImpl
 @brief     図鑑のデバッグウィンドウの実装
 */
//==============================================================================
class ZukanDebugWinImpl
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanDebugWinImpl);

public:
  ZukanDebugWinImpl(gfl2::heap::HeapBase* heap, gfl2::debug::DebugWinGroup* parentGroup, gfl2::str::MsgData* pMsgData);
  virtual ~ZukanDebugWinImpl();

private:
  static void     Initialize(ZukanDebugWinImpl* inst);
  static void     Terminate(void);

  // 島nフラグ
  static void     IslandFlag_Update(void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* IslandFlag_Draw  (void* userWork, gfl2::debug::DebugWinItem* item);

  // 図鑑うめ
  static void     Fill_Exec_Update(void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* Fill_Exec_Draw  (void* userWork, gfl2::debug::DebugWinItem* item);
  static void     Fill_RegisterType_Update(void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* Fill_RegisterType_Draw  (void* userWork, gfl2::debug::DebugWinItem* item);
  static void     Fill_NumberType_Update(void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* Fill_NumberType_Draw  (void* userWork, gfl2::debug::DebugWinItem* item);
  static void     Fill_Range_Update(void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* Fill_Range_Draw  (void* userWork, gfl2::debug::DebugWinItem* item);
  static void     Fill_TargetType_Update(void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* Fill_TargetType_Draw  (void* userWork, gfl2::debug::DebugWinItem* item);
  static void     Fill_Sex_Update(void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* Fill_Sex_Draw  (void* userWork, gfl2::debug::DebugWinItem* item);
  static void     Fill_Rare_Update(void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* Fill_Rare_Draw  (void* userWork, gfl2::debug::DebugWinItem* item);
  static void     Fill_Form_Update(void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* Fill_Form_Draw  (void* userWork, gfl2::debug::DebugWinItem* item);
  static void     Fill_Lang_Update(void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* Fill_Lang_Draw  (void* userWork, gfl2::debug::DebugWinItem* item);

  // であったかいすう
  static void     See_Poke_Update(void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* See_Poke_Draw  (void* userWork, gfl2::debug::DebugWinItem* item);
  static void     See_Count_Update(void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* See_Count_Draw  (void* userWork, gfl2::debug::DebugWinItem* item);

  // その他
  static s32 Util_S32_Update(s32 currVal, s32 minVal, s32 maxVal, bool isLoop, bool isJump);

private:
  // 図鑑うめ
  enum
  {
    FILL_REGISTER_TYPE_GET,
    FILL_REGISTER_TYPE_SEE,
    FILL_REGISTER_TYPE_PAST,
    FILL_REGISTER_TYPE_UNREGISTER,
    FILL_REGISTER_TYPE_RANDOM,
    FILL_REGISTER_TYPE_MAX,
  };
  enum
  {
    FILL_NUMBER_TYPE_ZENKOKU,
    FILL_NUMBER_TYPE_CHIHOU,
    FILL_NUMBER_TYPE_ISLAND1,
    FILL_NUMBER_TYPE_ISLAND2,
    FILL_NUMBER_TYPE_ISLAND3,
    FILL_NUMBER_TYPE_ISLAND4,
    FILL_NUMBER_TYPE_MAX,
  };
  enum
  {
    FILL_TARGET_TYPE_ALL,
//    FILL_TARGET_TYPE_RANDOM,
    FILL_TARGET_TYPE_10pc,
    FILL_TARGET_TYPE_30pc,
    FILL_TARGET_TYPE_50pc,
    FILL_TARGET_TYPE_70pc,
    FILL_TARGET_TYPE_90pc,
    FILL_TARGET_TYPE_95pc,
    FILL_TARGET_TYPE_MAX,
  };
  enum
  {
    FILL_SEX_ALL,
    FILL_SEX_MALE,
    FILL_SEX_FEMALE,
    FILL_SEX_RANDOM,
    FILL_SEX_MAX,
  };
  enum
  {
    FILL_RARE_NORMAL,
    FILL_RARE_RARE,
    FILL_RARE_BOTH,
    FILL_RARE_RANDOM,
    FILL_RARE_MAX,
  };
  enum
  {
    FILL_FORM_ALL,
    FILL_FORM_NORMAL,
    FILL_FORM_RANDOM,
    FILL_FORM_MAX,
  };
  enum
  {
    FILL_LANG_ALL,
    FILL_LANG_JPN,
    FILL_LANG_ENG,
    FILL_LANG_FRA,
    FILL_LANG_ITA,
    FILL_LANG_GER,
    FILL_LANG_SPA,
    FILL_LANG_KOR,
    FILL_LANG_CHN,
    FILL_LANG_TWN,
    FILL_LANG_RANDOM,
    FILL_LANG_MAX,
  };

  static s32 s_fill_registerType;
  static s32 s_fill_numberType;
  static s32 s_fill_rangeMin;  // MonsNo
  static s32 s_fill_rangeMax;  // MonsNo
  static s32 s_fill_targetType;
  static s32 s_fill_sex;
  static s32 s_fill_rare;
  static s32 s_fill_form;
  static s32 s_fill_lang;

  // であったかいすう
  static s32 s_see_poke;    // MonsNo
  static s32 s_see_count;

  static ZukanDebugWinImpl* s_inst;

private:
  static const s32 MONSNO_END_TEMP = MONSNO_END;  // 開発中にセーブデータを破壊しないように途中で止めたいときに使う定義。本来はMONSNO_ENDをそのまま使えばよい。

private:
  gfl2::heap::HeapBase*       m_heap;
  gfl2::debug::DebugWinGroup* m_parentGroup;
  gfl2::str::MsgData*         m_pMsgData;

  gfl2::debug::DebugWinGroup* m_zukanGroup;
  gfl2::debug::DebugWinGroup* m_zukanFillGroup;  // 図鑑うめ
  gfl2::debug::DebugWinGroup* m_zukanSeeGroup;   // であったかいすう

  gfl2::str::StrBuf* m_strBuf;
};




//==============================================================================
//
//  ZukanDebugWinImpl
//
//==============================================================================
ZukanDebugWinImpl::ZukanDebugWinImpl(gfl2::heap::HeapBase* heap, gfl2::debug::DebugWinGroup* parentGroup, gfl2::str::MsgData* pMsgData)
  : m_heap(heap),
    m_parentGroup(parentGroup),
    m_zukanGroup(NULL),
    m_zukanFillGroup(NULL),
    m_zukanSeeGroup(NULL),
    m_strBuf(NULL),
    m_pMsgData(pMsgData)
{
  Initialize(this);
  
  // 親
  m_zukanGroup     = gfl2::debug::DebugWin_AddGroup(m_heap, m_pMsgData, msg_dmenu_zukan_top, parentGroup);
  
  // 図鑑そうさ
  {
    m_zukanFillGroup = gfl2::debug::DebugWin_AddGroup(m_heap, m_pMsgData, msg_dmenu_zukan_fill_top , m_zukanGroup);
    m_zukanSeeGroup  = gfl2::debug::DebugWin_AddGroup(m_heap, m_pMsgData, msg_dmenu_zukan_see_top, m_zukanGroup);
 
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanGroup, m_heap, m_pMsgData, msg_dmenu_zukan_isle1_flg, reinterpret_cast<void*>(0), IslandFlag_Update, IslandFlag_Draw);
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanGroup, m_heap, m_pMsgData, msg_dmenu_zukan_isle2_flg, reinterpret_cast<void*>(1), IslandFlag_Update, IslandFlag_Draw);
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanGroup, m_heap, m_pMsgData, msg_dmenu_zukan_isle3_flg, reinterpret_cast<void*>(2), IslandFlag_Update, IslandFlag_Draw);
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanGroup, m_heap, m_pMsgData, msg_dmenu_zukan_isle4_flg, reinterpret_cast<void*>(3), IslandFlag_Update, IslandFlag_Draw);
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanGroup, m_heap, m_pMsgData, msg_dmenu_zukan_rotom_flg, reinterpret_cast<void*>(4), IslandFlag_Update, IslandFlag_Draw);
  }

  // 図鑑うめ
  {
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanFillGroup, m_heap, m_pMsgData, msg_dmenu_zukan_fill_exec, NULL                      , Fill_Exec_Update        , Fill_Exec_Draw        );
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanFillGroup, m_heap, m_pMsgData, msg_dmenu_zukan_fill_register_type, NULL                      , Fill_RegisterType_Update, Fill_RegisterType_Draw);
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanFillGroup, m_heap, m_pMsgData, msg_dmenu_zukan_fill_number       , NULL                      , Fill_NumberType_Update  , Fill_NumberType_Draw  );
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanFillGroup, m_heap, m_pMsgData, msg_dmenu_zukan_fill_range_min    , reinterpret_cast<void*>(0), Fill_Range_Update       , Fill_Range_Draw       );
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanFillGroup, m_heap, m_pMsgData, msg_dmenu_zukan_fill_range_max    , reinterpret_cast<void*>(1), Fill_Range_Update       , Fill_Range_Draw       );
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanFillGroup, m_heap, m_pMsgData, msg_dmenu_zukan_fill_target_type  , NULL                      , Fill_TargetType_Update  , Fill_TargetType_Draw  );
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanFillGroup, m_heap, m_pMsgData, msg_dmenu_zukan_fill_sex                   , NULL                      , Fill_Sex_Update         , Fill_Sex_Draw         );
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanFillGroup, m_heap, m_pMsgData, msg_dmenu_zukan_fill_rare                  , NULL                      , Fill_Rare_Update        , Fill_Rare_Draw        );
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanFillGroup, m_heap, m_pMsgData, msg_dmenu_zukan_fill_form            , NULL                      , Fill_Form_Update        , Fill_Form_Draw        );
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanFillGroup, m_heap, m_pMsgData, msg_dmenu_zukan_fill_lang                   , NULL                      , Fill_Lang_Update        , Fill_Lang_Draw        );
  }

  // であったかいすう
  {
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanSeeGroup, m_heap, m_pMsgData, msg_dmenu_zukan_see_pokemon, NULL, See_Poke_Update , See_Poke_Draw );
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanSeeGroup, m_heap, m_pMsgData, msg_dmenu_zukan_see_count, NULL, See_Count_Update, See_Count_Draw);
  }

  m_strBuf = GFL_NEW(m_heap) gfl2::str::StrBuf(24, m_heap);
}
ZukanDebugWinImpl::~ZukanDebugWinImpl()
{
  GFL_SAFE_DELETE(m_strBuf);

  gfl2::debug::DebugWin_RemoveGroup(m_zukanSeeGroup);
  gfl2::debug::DebugWin_RemoveGroup(m_zukanFillGroup);
  gfl2::debug::DebugWin_RemoveGroup(m_zukanGroup);
  
  Terminate();
}

s32 ZukanDebugWinImpl::s_fill_registerType = 0;
s32 ZukanDebugWinImpl::s_fill_numberType   = 0;
s32 ZukanDebugWinImpl::s_fill_rangeMin     = 0;  // MonsNo
s32 ZukanDebugWinImpl::s_fill_rangeMax     = 0;  // MonsNo
s32 ZukanDebugWinImpl::s_fill_targetType   = 0;
s32 ZukanDebugWinImpl::s_fill_sex          = 0;
s32 ZukanDebugWinImpl::s_fill_rare         = 0;
s32 ZukanDebugWinImpl::s_fill_form         = 0;
s32 ZukanDebugWinImpl::s_fill_lang         = 0;

s32 ZukanDebugWinImpl::s_see_poke          = 0;    // MonsNo
s32 ZukanDebugWinImpl::s_see_count         = 0;

ZukanDebugWinImpl* ZukanDebugWinImpl::s_inst = NULL;

void     ZukanDebugWinImpl::Initialize(ZukanDebugWinImpl* inst)
{
  Savedata::ZukanData*  zukanData   = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();

  s_fill_registerType = FILL_REGISTER_TYPE_GET;
  s_fill_numberType   = FILL_NUMBER_TYPE_ZENKOKU;
  s_fill_rangeMin     = MONSNO_HUSIGIDANE;  // MonsNo
  s_fill_rangeMax     = MONSNO_END_TEMP;//BORUKENION;  // MonsNo
  s_fill_targetType   = FILL_TARGET_TYPE_ALL;
  s_fill_sex          = FILL_SEX_ALL;
  s_fill_rare         = FILL_RARE_BOTH;
  s_fill_form         = FILL_FORM_ALL;
  s_fill_lang         = FILL_LANG_ALL;

  s_see_poke          = MONSNO_HUSIGIDANE;    // MonsNo
  s_see_count         = zukanData->GetPokeSeeAmount(static_cast<MonsNo>(s_see_poke));

  s_inst = inst;
}
void     ZukanDebugWinImpl::Terminate(void)
{}

void     ZukanDebugWinImpl::IslandFlag_Update(void* userWork, gfl2::debug::DebugWinItem* /*item*/)
{
  gfl2::ui::Button *uiButton = gfl2::debug::DebugWin_GetUiButton();
  Field::EventWork* svEventWork = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork();

  enum
  {
    ITEM_INDEX_ISLAND1 = 0,
    ITEM_INDEX_ISLAND2 = 1,
    ITEM_INDEX_ISLAND3 = 2,
    ITEM_INDEX_ISLAND4 = 3,
    ITEM_INDEX_ROTOM   = 4,
  };
  s32 itemIndex = reinterpret_cast<s32>(userWork);

  bool flag = false;
  switch(itemIndex)
  {
  case ITEM_INDEX_ISLAND1: flag = svEventWork->CheckEventFlag(SYS_FLAG_ZUKAN_ISLAND1_OPEN); break;
  case ITEM_INDEX_ISLAND2: flag = svEventWork->CheckEventFlag(SYS_FLAG_ZUKAN_ISLAND2_OPEN); break;
  case ITEM_INDEX_ISLAND3: flag = svEventWork->CheckEventFlag(SYS_FLAG_ZUKAN_ISLAND3_OPEN); break;
  case ITEM_INDEX_ISLAND4: flag = svEventWork->CheckEventFlag(SYS_FLAG_ZUKAN_ISLAND4_OPEN); break;
  case ITEM_INDEX_ROTOM  : flag = svEventWork->CheckEventFlag(SYS_FLAG_BOUKEN_NAVI_GET);    break;
  }

  if( uiButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) || uiButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    flag = !flag;
  }

  switch(itemIndex)
  {
  case ITEM_INDEX_ISLAND1: if(flag) svEventWork->SetEventFlag(SYS_FLAG_ZUKAN_ISLAND1_OPEN); else svEventWork->ResetEventFlag(SYS_FLAG_ZUKAN_ISLAND1_OPEN); break;
  case ITEM_INDEX_ISLAND2: if(flag) svEventWork->SetEventFlag(SYS_FLAG_ZUKAN_ISLAND2_OPEN); else svEventWork->ResetEventFlag(SYS_FLAG_ZUKAN_ISLAND2_OPEN); break;
  case ITEM_INDEX_ISLAND3: if(flag) svEventWork->SetEventFlag(SYS_FLAG_ZUKAN_ISLAND3_OPEN); else svEventWork->ResetEventFlag(SYS_FLAG_ZUKAN_ISLAND3_OPEN); break;
  case ITEM_INDEX_ISLAND4: if(flag) svEventWork->SetEventFlag(SYS_FLAG_ZUKAN_ISLAND4_OPEN); else svEventWork->ResetEventFlag(SYS_FLAG_ZUKAN_ISLAND4_OPEN); break;
  case ITEM_INDEX_ROTOM  : if(flag) svEventWork->SetEventFlag(SYS_FLAG_BOUKEN_NAVI_GET);    else svEventWork->ResetEventFlag(SYS_FLAG_BOUKEN_NAVI_GET);    break;
  }
}
wchar_t* ZukanDebugWinImpl::IslandFlag_Draw  (void* userWork, gfl2::debug::DebugWinItem* item)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  const Field::EventWork* svEventWork = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork();

  enum
  {
    ITEM_INDEX_ISLAND1 = 0,
    ITEM_INDEX_ISLAND2 = 1,
    ITEM_INDEX_ISLAND3 = 2,
    ITEM_INDEX_ISLAND4 = 3,
    ITEM_INDEX_ROTOM   = 4,
  };
  s32 itemIndex = reinterpret_cast<s32>(userWork);

  bool flag = false;
  switch(itemIndex)
  {
  case ITEM_INDEX_ISLAND1: flag = svEventWork->CheckEventFlag(SYS_FLAG_ZUKAN_ISLAND1_OPEN); break;
  case ITEM_INDEX_ISLAND2: flag = svEventWork->CheckEventFlag(SYS_FLAG_ZUKAN_ISLAND2_OPEN); break;
  case ITEM_INDEX_ISLAND3: flag = svEventWork->CheckEventFlag(SYS_FLAG_ZUKAN_ISLAND3_OPEN); break;
  case ITEM_INDEX_ISLAND4: flag = svEventWork->CheckEventFlag(SYS_FLAG_ZUKAN_ISLAND4_OPEN); break;
  case ITEM_INDEX_ROTOM  : flag = svEventWork->CheckEventFlag(SYS_FLAG_BOUKEN_NAVI_GET);    break;
  }

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), flag?(1):(0) );

  return workStr;
}

void     ZukanDebugWinImpl::Fill_Exec_Update(void* /*userWork*/, gfl2::debug::DebugWinItem* /*item*/)
{
  Savedata::ZukanData*  zukanData   = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();
  
  gfl2::ui::Button *uiButton = gfl2::debug::DebugWin_GetUiButton();
  
  gfl2::heap::HeapBase* heap       = s_inst->m_heap->GetLowerHandle();
  
  if( uiButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    const u32 default_language[] =  // System/include/PokemonVersion.h
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


    /*
      図鑑埋め対象テーブル
    */
    static const u16  pokedexMonsNo[] = {
      722,    /*  モクロー        */
      723,    /*  フクスロー      */
      724,    /*  ジュナイパー    */
      725,    /*  ニャビー        */
      726,    /*  ニャヒート      */
      727,    /*  ガオガエン      */
      728,    /*  アシマリ        */
      729,    /*  オシャマリ      */
      730,    /*  アシレーヌ      */
      731,    /*  ツツケラ        */
      732,    /*  ケララッパ      */
      733,    /*  ドデカバシ      */
//      734,    /*  ヤングース      */    /*  序盤のポケモンを捕まえたら目標達成できるようにしておく  */
//      735,    /*  デカグース      */    /*  ビンゴにもならないように                                */
//       19,    /*  コラッタ        */
       20,    /*  ラッタ          */
//       10,    /*  キャタピー      */
//       11,    /*  トランセル      */
//       12,    /*  バタフリー      */
      165,    /*  レディバ        */
      166,    /*  レディアン      */
      167,    /*  イトマル        */
      168,    /*  アリアドス      */
//      427,    /*  ミミロル        */
//      428,    /*  ミミロップ      */
      686,    /*  マーイーカ      */
      687,    /*  カラマネロ      */
      570,    /*  ゾロア          */
      571,    /*  ゾロアーク      */
      676,    /*  トリミアン      */
      172,    /*  ピチュー        */
       25,    /*  ピカチュウ      */
       26,    /*  ライチュウ      */
      736,    /*  アゴジムシ      */
      737,    /*  デンヂムシ      */
      738,    /*  クワガノン      */
      438,    /*  ウソハチ        */
      185,    /*  ウソッキー      */
      440,    /*  ピンプク        */
      113,    /*  ラッキー        */
      242,    /*  ハピナス        */
      446,    /*  ゴンベ          */
      143,    /*  カビゴン        */
       79,    /*  ヤドン          */
       80,    /*  ヤドラン        */
      199,    /*  ヤドキング      */
      278,    /*  キャモメ        */
      279,    /*  ペリッパー      */
       63,    /*  ケーシィ        */
       64,    /*  ユンゲラー      */
       65,    /*  フーディン      */
       52,    /*  ニャース        */
       53,    /*  ペルシアン      */
       81,    /*  コイル          */
       82,    /*  レアコイル      */
      462,    /*  ジバコイル      */
       88,    /*  ベトベター      */
       89,    /*  ベトベトン      */
      439,    /*  マネネ          */
      122,    /*  バリヤード      */
       23,    /*  アーボ          */
       24,    /*  アーボック      */
      206,    /*  ノコッチ        */
       58,    /*  ガーディ        */
       59,    /*  ウインディ      */
       96,    /*  スリープ        */
       97,    /*  スリーパー      */
      296,    /*  マクノシタ      */
      297,    /*  ハリテヤマ      */
      235,    /*  ドーブル        */
      739,    /*  マケンカニ      */
      740,    /*  ケケンカニ      */
       92,    /*  ゴース          */
       93,    /*  ゴースト        */
       94,    /*  ゲンガー        */
      425,    /*  フワンテ        */
      426,    /*  フワライド      */
      198,    /*  ヤミカラス      */
      430,    /*  ドンカラス      */
       41,    /*  ズバット        */
       42,    /*  ゴルバット      */
      169,    /*  クロバット      */
      714,    /*  オンバット      */
      715,    /*  オンバーン      */
       50,    /*  ディグダ        */
       51,    /*  ダグトリオ      */
       21,    /*  オニスズメ      */
       22,    /*  オニドリル      */
      627,    /*  ワシボン        */
      628,    /*  ウォーグル      */
      629,    /*  バルチャイ      */
      630,    /*  バルジーナ      */
       56,    /*  マンキー        */
       57,    /*  オコリザル      */
      225,    /*  デリバード      */
      701,    /*  ルチャブル      */
      741,    /*  オドリドリ      */
      742,    /*  アブリー        */
      743,    /*  アブリボン      */
      669,    /*  フラベベ        */
      670,    /*  フラエッテ      */
      671,    /*  フラージェス    */
      548,    /*  チュリネ        */
      549,    /*  ドレディア      */
      546,    /*  モンメン        */
      547,    /*  エルフーン      */
       54,    /*  コダック        */
       55,    /*  ゴルダック      */
      238,    /*  ムチュール      */
      124,    /*  ルージュラ      */
      129,    /*  コイキング      */
      130,    /*  ギャラドス      */
      339,    /*  ドジョッチ      */
      340,    /*  ナマズン        */
       86,    /*  パウワウ        */
       87,    /*  ジュゴン        */
       66,    /*  ワンリキー      */
       67,    /*  ゴーリキー      */
       68,    /*  カイリキー      */
      524,    /*  ダンゴロ        */
      525,    /*  ガントル        */
      526,    /*  ギガイアス      */
      703,    /*  メレシー        */
      302,    /*  ヤミラミ        */
      303,    /*  クチート        */
      744,    /*  イワンコ        */
      745,    /*  ルガルガン      */
      327,    /*  パッチール      */
       72,    /*  メノクラゲ      */
       73,    /*  ドククラゲ      */
      456,    /*  ケイコウオ      */
      457,    /*  ネオラント      */
      746,    /*  ヨワシ          */
      370,    /*  ラブカス        */
      222,    /*  サニーゴ        */
      747,    /*  ヒドイデ        */
      748,    /*  ドヒドイデ      */
       90,    /*  シェルダー      */
       91,    /*  パルシェン      */
      366,    /*  パールル        */
      367,    /*  ハンテール      */
      368,    /*  サクラビス      */
      223,    /*  テッポウオ      */
      224,    /*  オクタン        */
      458,    /*  タマンタ        */
      226,    /*  マンタイン      */
      371,    /*  タツベイ        */
      372,    /*  コモルー        */
      373,    /*  ボーマンダ      */
      506,    /*  ヨーテリー      */
      507,    /*  ハーデリア      */
      508,    /*  ムーランド      */
      133,    /*  イーブイ        */
      134,    /*  シャワーズ      */
      135,    /*  サンダース      */
      136,    /*  ブースター      */
      196,    /*  エーフィ        */
      197,    /*  ブラッキー      */
      470,    /*  リーフィア      */
      471,    /*  グレイシア      */
      700,    /*  ニンフィア      */
      179,    /*  メリープ        */
      180,    /*  モココ          */
      181,    /*  デンリュウ      */
      749,    /*  ドロバンコ      */
      750,    /*  バンバドロ      */
      174,    /*  ププリン        */
       39,    /*  プリン          */
       40,    /*  プクリン        */
      128,    /*  ケンタロス      */
      241,    /*  ミルタンク      */
      283,    /*  アメタマ        */
      284,    /*  アメモース      */
      751,    /*  シズクモ        */
      752,    /*  オニシズクモ    */
      753,    /*  カリキリ        */
      754,    /*  ラランテス      */
      755,    /*  ネマシュ        */
      756,    /*  マシェード      */
       46,    /*  パラス          */
       47,    /*  パラセクト      */
       60,    /*  ニョロモ        */
       61,    /*  ニョロゾ        */
       62,    /*  ニョロボン      */
      186,    /*  ニョロトノ      */
      118,    /*  トサキント      */
      119,    /*  アズマオウ      */
      550,    /*  バスラオ        */
      349,    /*  ヒンバス        */
      350,    /*  ミロカロス      */
      594,    /*  ママンボウ      */
      661,    /*  ヤヤコマ        */
      662,    /*  ヒノヤコマ      */
      663,    /*  ファイアロー    */
      757,    /*  ヤトウモリ      */
      758,    /*  エンニュート    */
      104,    /*  カラカラ        */
      105,    /*  ガラガラ        */
      115,    /*  ガルーラ        */
      240,    /*  ブビィ          */
      126,    /*  ブーバー        */
      467,    /*  ブーバーン      */
      636,    /*  メラルバ        */
      637,    /*  ウルガモス      */
      759,    /*  ヌイコグマ      */
      760,    /*  キテルグマ      */
      761,    /*  アマカジ        */
      762,    /*  アママイコ      */
      763,    /*  アマージョ      */
      764,    /*  キュワワー      */
      127,    /*  カイロス        */
      163,    /*  ホーホー        */
      164,    /*  ヨルノズク      */
      352,    /*  カクレオン      */
      765,    /*  ヤレユータン    */
      766,    /*  ナゲツケサル    */
      704,    /*  ヌメラ          */
      705,    /*  ヌメイル        */
      706,    /*  ヌメルゴン      */
      351,    /*  ポワルン        */
      767,    /*  コソクムシ      */
      768,    /*  グソクムシャ    */
      120,    /*  ヒトデマン      */
      121,    /*  スターミー      */
      769,    /*  スナバァ        */
      770,    /*  シロデスナ      */
      138,    /*  オムナイト      */
      139,    /*  オムスター      */
      140,    /*  カブト          */
      141,    /*  カブトプス      */
      345,    /*  リリーラ        */
      346,    /*  ユレイドル      */
      347,    /*  アノプス        */
      348,    /*  アーマルド      */
      408,    /*  ズガイドス      */
      409,    /*  ラムパルド      */
      410,    /*  タテトプス      */
      411,    /*  トリデプス      */
      566,    /*  アーケン        */
      567,    /*  アーケオス      */
      564,    /*  プロトーガ      */
      565,    /*  アバゴーラ      */
      696,    /*  チゴラス        */
      697,    /*  ガチゴラス      */
      698,    /*  アマルス        */
      699,    /*  アマルルガ      */
      246,    /*  ヨーギラス      */
      247,    /*  サナギラス      */
      248,    /*  バンギラス      */
      708,    /*  ボクレー        */
      709,    /*  オーロット      */
      177,    /*  ネイティ        */
      178,    /*  ネイティオ      */
      299,    /*  ノズパス        */
      476,    /*  ダイノーズ      */
      771,    /*  ナマコブシ      */
      170,    /*  チョンチー      */
      171,    /*  ランターン      */
      772,    /*  タイプ：ヌル    */
      773,    /*  シルヴァディ    */
      803,    /*  ベベノム        */
      804,    /*  アーゴヨン      */
      718,    /*  ジガルデ        */
      568,    /*  ヤブクロン      */
      569,    /*  ダストダス      */
      572,    /*  チラーミィ      */
      573,    /*  チラチーノ      */
      204,    /*  クヌギダマ      */
      205,    /*  フォレトス      */
      227,    /*  エアームド      */
      132,    /*  メタモン        */
      173,    /*  ピィ            */
       35,    /*  ピッピ          */
       36,    /*  ピクシー        */
      605,    /*  リグレー        */
      606,    /*  オーベム        */
      774,    /*  メテノ          */
      374,    /*  ダンバル        */
      375,    /*  メタング        */
      376,    /*  メタグロス      */
      137,    /*  ポリゴン        */
      233,    /*  ポリゴン２      */
      474,    /*  ポリゴンＺ      */
      674,    /*  ヤンチャム      */
      675,    /*  ゴロンダ        */
      775,    /*  ネッコアラ      */
      324,    /*  コータス        */
      776,    /*  バクガメス      */
      228,    /*  デルビル        */
      229,    /*  ヘルガー        */
      702,    /*  デデンネ        */
      777,    /*  トゲデマル      */
      309,    /*  ラクライ        */
      310,    /*  ライボルト      */
      239,    /*  エレキッド      */
      125,    /*  エレブー        */
      466,    /*  エレキブル      */
       74,    /*  イシツブテ      */
       75,    /*  ゴローン        */
       76,    /*  ゴローニャ      */
      551,    /*  メグロコ        */
      552,    /*  ワルビル        */
      553,    /*  ワルビアル      */
      328,    /*  ナックラー      */
      329,    /*  ビブラーバ      */
      330,    /*  フライゴン      */
      443,    /*  フカマル        */
      444,    /*  ガバイト        */
      445,    /*  ガブリアス      */
      343,    /*  ヤジロン        */
      344,    /*  ネンドール      */
      622,    /*  ゴビット        */
      623,    /*  ゴルーグ        */
      707,    /*  クレッフィ      */
      778,    /*  ミミッキュ      */
      353,    /*  カゲボウズ      */
      354,    /*  ジュペッタ      */
      592,    /*  プルリル        */
      593,    /*  ブルンゲル      */
      779,    /*  ハギギシリ      */
      780,    /*  ジジーロン      */
      359,    /*  アブソル        */
      361,    /*  ユキワラシ      */
      362,    /*  オニゴーリ      */
      478,    /*  ユキメノコ      */
      215,    /*  ニューラ        */
      461,    /*  マニューラ      */
       27,    /*  サンド          */
       28,    /*  サンドパン      */
       37,    /*  ロコン          */
       38,    /*  キュウコン      */
      582,    /*  バニプッチ      */
      583,    /*  バニリッチ      */
      584,    /*  バイバニラ      */
      559,    /*  ズルッグ        */
      560,    /*  ズルズキン      */
      624,    /*  コマタナ        */
      625,    /*  キリキザン      */
      209,    /*  ブルー          */
      210,    /*  グランブル      */
      422,    /*  カラナクシ      */
      423,    /*  トリトドン      */
      369,    /*  ジーランス      */
      781,    /*  ダダリン        */
      318,    /*  キバニア        */
      319,    /*  サメハダー      */
      690,    /*  クズモー        */
      691,    /*  ドラミドロ      */
      692,    /*  ウデッポウ      */
      693,    /*  ブロスター      */
      320,    /*  ホエルコ        */
      321,    /*  ホエルオー      */
      131,    /*  ラプラス        */
      357,    /*  トロピウス      */
      102,    /*  タマタマ        */
      103,    /*  ナッシー        */
      341,    /*  ヘイガニ        */
      342,    /*  シザリガー      */
      619,    /*  コジョフー      */
      620,    /*  コジョンド      */
      782,    /*  ジャラコ        */
      783,    /*  ジャランゴ      */
      784,    /*  ジャラランガ    */
      587,    /*  エモンガ        */
      123,    /*  ストライク      */
      212,    /*  ハッサム        */
      214,    /*  ヘラクロス      */
      190,    /*  エイパム        */
      424,    /*  エテボース      */
      667,    /*  シシコ          */
      668,    /*  カエンジシ      */
      200,    /*  ムウマ          */
      429,    /*  ムウマージ      */
      621,    /*  クリムガン      */
      108,    /*  ベロリンガ      */
      463,    /*  ベロベルト      */
      447,    /*  リオル          */
      448,    /*  ルカリオ        */
      147,    /*  ミニリュウ      */
      148,    /*  ハクリュー      */
      149,    /*  カイリュー      */
      142,    /*  プテラ          */
      785,    /*  カプ・コケコ    */
      786,    /*  カプ・テテフ    */
      787,    /*  カプ・ブルル    */
      788,    /*  カプ・レヒレ    */
      789,    /*  コスモッグ      */
      790,    /*  コスモウム      */
      791,    /*  ソルガレオ      */
      792,    /*  ルナアーラ      */
      793,    /*  ウツロイド      */
      805,    /*  ツンデツンデ    */
      806,    /*  ズガドーン      */
      794,    /*  マッシブーン    */
      795,    /*  フェローチェ    */
      796,    /*  デンジュモク    */
      797,    /*  テッカグヤ      */
      798,    /*  カミツルギ      */
      799,    /*  アクジキング    */
      800,    /*  ネクロズマ      */
      /*  対象外  */
//      801,    /*  マギアナ        */
//      802,    /*  マーシャドー    */
//      807,    /*  ゼラオラ        */
    };

    const u32 pokedexMonsNoElems  = GFL_NELEMS(pokedexMonsNo);
    const u32 pokeDexComplete     = 400;    /**<  実際のコンプリート目標  */
    const u32 pokeDex10pc         = (pokeDexComplete * 10 / 100) - 1;
    const u32 pokeDex30pc         = (pokeDexComplete * 30 / 100) - 1;
    const u32 pokeDex50pc         = (pokeDexComplete * 50 / 100) - 1;
    const u32 pokeDex70pc         = (pokeDexComplete * 70 / 100) - 1;
    const u32 pokeDex90pc         = (pokeDexComplete * 90 / 100) - 1;
    const u32 pokeDex95pc         = (pokeDexComplete * 95 / 100) - 1;

    pml::personal::PersonalData *personal_data  = GFL_NEW(heap) pml::personal::PersonalData( heap );
    pml::pokepara::CoreParam * core_param = GFL_NEW(heap) pml::pokepara::CoreParam( heap );


    switch( s_fill_numberType )
    {
    case FILL_NUMBER_TYPE_ZENKOKU:
      s_fill_rangeMin = gfl2::math::Max( s_fill_rangeMin, 1 );
      s_fill_rangeMax = gfl2::math::Min( s_fill_rangeMax, static_cast<s32>(MONSNO_END_TEMP) );
      break;
    default:
      GFL_ASSERT(0);
    }

    for( s32 i = s_fill_rangeMin; i <= s_fill_rangeMax; ++i )
    {
      
      MonsNo monsNo = static_cast<MonsNo>(i);
      
      switch( s_fill_numberType )
      {
      case FILL_NUMBER_TYPE_ZENKOKU:
        monsNo = static_cast<MonsNo>(i);
        break;
      default:
        GFL_ASSERT(0);
      }

      /*
        図鑑埋め
      */
#if 0
      if( s_fill_targetType == FILL_TARGET_TYPE_RANDOM )
      {
        if( System::GflUse::GetPublicRand(2) == 0 )
        {
          continue;
        }
      }
#else
      if( s_fill_targetType != FILL_TARGET_TYPE_ALL )
      {
        u32   loopMax = 0;
        bool  bAllow  = false;
        
        switch(s_fill_targetType)
        {
          case  FILL_TARGET_TYPE_10pc:  loopMax = pokeDex10pc;   break;
          case  FILL_TARGET_TYPE_30pc:  loopMax = pokeDex30pc;   break;
          case  FILL_TARGET_TYPE_50pc:  loopMax = pokeDex50pc;   break;
          case  FILL_TARGET_TYPE_70pc:  loopMax = pokeDex70pc;   break;
          case  FILL_TARGET_TYPE_90pc:  loopMax = pokeDex90pc;   break;
          case  FILL_TARGET_TYPE_95pc:  loopMax = pokeDex95pc;   break;
        }
        
        for(u32 index = 0; index < loopMax; ++index)
        {
          if(monsNo == pokedexMonsNo[index])
          {
            bAllow  = true;
            break;
          }
        }
        
        if(!bAllow) continue;
      }
#endif


      personal_data->LoadData( monsNo, 0 );
      

      s32 reg = s_fill_registerType;

      if( s_fill_registerType == FILL_REGISTER_TYPE_RANDOM )
      {
        reg = System::GflUse::GetPublicRand(FILL_REGISTER_TYPE_RANDOM);
      }

      u32 form_max = 0;
      switch( s_fill_form )
      {
      case FILL_FORM_NORMAL:
        form_max  = 1;
        break;
      case FILL_FORM_ALL:
      case FILL_FORM_RANDOM:
        form_max = zukanData->GetFormMax( monsNo );
        break;
      default:
        GFL_ASSERT(0);
      }

      for( u32 j = 0; j < form_max; ++j )
      {
        if( s_fill_form == FILL_FORM_RANDOM )
        {
          if( System::GflUse::GetPublicRand(2) == 0 )
          {
            continue;
          }
        }

        // GFL_PRINT( "MonsNo %d FormNo %d FormMaxZukan %d", monsNo, j, zukanData->GetFormMax( monsNo ) );
        // GFL_PRINT( "FormMaxPer %d\n", personal_data->GetParam( pml::personal::PARAM_ID_form_max ) );

        core_param->ChangeMonsNo( monsNo, j );  //ここのフォルムにいれても前のフォルムが同じならば変更にならない
        core_param->ChangeFormNo( j );

        pml::Sex  sex_table[ 2 ];
        s32 sex_table_num = 0;
        pml::personal::SexVector sex_vec = static_cast<pml::personal::SexVector>( personal_data->GetParam( pml::personal::PARAM_ID_sex ) );

        if( sex_vec == pml::personal::SEX_VECTOR_ONLY_MALE )
        {
          sex_table[0]  = pml::SEX_MALE;
          sex_table_num = 1;
        }
        else if( sex_vec == pml::personal::SEX_VECTOR_ONLY_FEMALE )
        {
          sex_table[0]  = pml::SEX_FEMALE;
          sex_table_num = 1;
        }
        else if( sex_vec == pml::personal::SEX_VECTOR_UNKNOWN )
        {
          sex_table[0]  = pml::SEX_UNKNOWN;
          sex_table_num = 1;
        }
        else
        {
          s32 sex_type = s_fill_sex;
          if( s_fill_sex >= FILL_SEX_RANDOM )
          {
            sex_type = System::GflUse::GetPublicRand(FILL_SEX_RANDOM);
          }

          switch( sex_type )
          {
          default:
            GFL_ASSERT(0);
          case FILL_SEX_ALL:
            sex_table[0]  = pml::SEX_MALE;
            sex_table[1]  = pml::SEX_FEMALE;
            sex_table_num = 2;
            break;
          case FILL_SEX_MALE:
            sex_table[0]  = pml::SEX_MALE;
            sex_table_num = 1;
            break;
          case FILL_SEX_FEMALE:
            sex_table[0]  = pml::SEX_FEMALE;
            sex_table_num = 1;
            break;
          }

        }
        for( s32 k = 0; k < sex_table_num; ++ k )
        {
          core_param->ChangeSex( sex_table[ k ] );
          
          
          s32 rare_start  = 0;
          s32 rare_end    = 2;
          
          s32 rare  = s_fill_rare;
          if( s_fill_rare == FILL_RARE_RANDOM )
          {
            rare = System::GflUse::GetPublicRand(FILL_RARE_RANDOM);
          }
          switch( rare )
          {
          case FILL_RARE_NORMAL:
            rare_start  = 0;
            rare_end  = 1;
            break;
          case FILL_RARE_RARE:
            rare_start  = 1;
            rare_end  = 2;
            break;
          case FILL_RARE_BOTH:
            rare_start  = 0;
            rare_end  = 2;
            break;
          }
          for( s32 l = rare_start; l < rare_end; ++l )
          {
            if( l == 0 )
            {
              core_param->SetNotRare();
            }
            else
            {
              core_param->SetRare();
            }
            u32 language[9];
            u32 lang_max;
            switch( s_fill_lang )
            {
            case FILL_LANG_JPN:
            case FILL_LANG_ENG:
            case FILL_LANG_FRA:
            case FILL_LANG_ITA:
            case FILL_LANG_GER:
            case FILL_LANG_SPA:
            case FILL_LANG_KOR:
            case FILL_LANG_CHN:
            case FILL_LANG_TWN:
              lang_max = 1;
              language[0] = default_language[ s_fill_lang ];
              break;
            case FILL_LANG_ALL:
              lang_max = GFL_NELEMS(default_language);
              gfl2::std::MemCopy( default_language, language, sizeof(u32)*lang_max );
              break;
            case FILL_LANG_RANDOM:
              
              lang_max  = System::GflUse::GetPublicRand( GFL_NELEMS(default_language)  );
              for( u32 lran = 0; lran < lang_max; ++lran )
              {
                u32 rand = System::GflUse::GetPublicRand( GFL_NELEMS(default_language) );
                language[lran]  = default_language[ rand ];
              }
              break;
            default:
              GFL_ASSERT(0);
            }
            for( u32 m = 0; m < lang_max; ++m )
            {
              core_param->SetLangId( language[m] );
              
              switch( reg )
              {
              case FILL_REGISTER_TYPE_GET:
                core_param->SetCassetteVersion( System::GetVersion() );  // System/include/PokemonVersion.h
                zukanData->DebugSetPokeGetForce( *core_param );
                break;
              case FILL_REGISTER_TYPE_PAST:
                if( core_param->GetMonsNo() <= MONSNO_BORUKENION )  //今回のポケモンは過去フラグつかない
                { 
                  core_param->SetCassetteVersion( VERSION_X );
                  zukanData->DebugSetPokeGetForce( *core_param );
                }
                else
                {
                  core_param->SetCassetteVersion( System::GetVersion() );
                  zukanData->DebugSetPokeGetForce( *core_param );
                }
                break;
                
              case FILL_REGISTER_TYPE_UNREGISTER:
                zukanData->DebugSetPokeDeleteForce( *core_param );
                break;
                
              case FILL_REGISTER_TYPE_SEE:
                zukanData->DebugSetPokeSeeForce( *core_param );
                break;
              }
            }
          }
        }
      }
    }

    GFL_SAFE_DELETE( core_param );
    GFL_SAFE_DELETE( personal_data );
    
    Sound::PlaySE( SEQ_SE_SAVE );
  }
  else if( uiButton->IsTrigger( gfl2::ui::BUTTON_X ) )
  {
    // セーブデータの内容を確認するために出力する
    for(s32 monsNo=1; monsNo<=MONSNO_END_TEMP; ++monsNo)
    {
      bool getFlag = zukanData->GetPokeGetFlag(static_cast<MonsNo>(monsNo));
      bool seeFlag = zukanData->GetPokeSeeFlag(static_cast<MonsNo>(monsNo));
      if(getFlag || seeFlag)
      {
        pml::Sex     sex;
        bool         rare;
        pml::FormNo  formNo;
        zukanData->GetDrawData(static_cast<MonsNo>(monsNo), &sex, &rare, &formNo);
        GFL_PRINT("%03d: get=%d, see=%d, sex=%d, rare=%d, formNo=%d\n", monsNo, getFlag, seeFlag, sex, rare, formNo);
      }
      else
      {
        GFL_PRINT("%03d: get=0, see=0\n", monsNo);
      }
    }
  }
}
wchar_t* ZukanDebugWinImpl::Fill_Exec_Draw  (void* /*userWork*/, gfl2::debug::DebugWinItem* item)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls", item->GetName() );
  return workStr;
}
void     ZukanDebugWinImpl::Fill_RegisterType_Update(void* /*userWork*/, gfl2::debug::DebugWinItem* /*item*/)
{
  s_fill_registerType = Util_S32_Update( s_fill_registerType, 0, FILL_REGISTER_TYPE_MAX-1, true, false );
}
wchar_t* ZukanDebugWinImpl::Fill_RegisterType_Draw  (void* /*userWork*/, gfl2::debug::DebugWinItem* item)
{
  static const u32 fill_registerTypeStrId[] =
  {
    msg_dmenu_zukan_fill_register_type_capture, // "ほかく"
    msg_dmenu_zukan_fill_register_type_see,     // "みた"
    msg_dmenu_zukan_fill_register_type_brink,   // "もってきた"
    msg_dmenu_zukan_fill_register_type_delete,  // "さくじょ"
    msg_dmenu_zukan_fill_register_type_random,  // "ランダム"
  };


  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  gfl2::str::StrBuf* workStrBuf = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetWorkStrBuf();
  s_inst->m_pMsgData->GetString(fill_registerTypeStrId[s_fill_registerType], *workStrBuf);
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%ls]", item->GetName(), workStrBuf->GetPtr() );

  return workStr;
}
void     ZukanDebugWinImpl::Fill_NumberType_Update(void* /*userWork*/, gfl2::debug::DebugWinItem* /*item*/)
{
}
wchar_t* ZukanDebugWinImpl::Fill_NumberType_Draw  (void* /*userWork*/, gfl2::debug::DebugWinItem* item)
{
  static const u32 fill_numberTypeStrId[] =
  {
    msg_dmenu_zukan_fill_number_allover, // "全国番号"
  };

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  gfl2::str::StrBuf* workStrBuf = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetWorkStrBuf();
  s_inst->m_pMsgData->GetString(fill_numberTypeStrId[s_fill_numberType], *workStrBuf);
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%ls]", item->GetName(), workStrBuf->GetPtr() );
  return workStr;
}
void     ZukanDebugWinImpl::Fill_Range_Update(void* userWork, gfl2::debug::DebugWinItem* /*item*/)
{
  enum
  {
    ITEM_INDEX_RANGE_MIN = 0,
    ITEM_INDEX_RANGE_MAX = 1,
  };
  s32 itemIndex = reinterpret_cast<s32>(userWork);

  switch(itemIndex)
  {
  case ITEM_INDEX_RANGE_MIN:
    s_fill_rangeMin = Util_S32_Update( s_fill_rangeMin, 1, MONSNO_END_TEMP, true, true );
    break;
  case ITEM_INDEX_RANGE_MAX:
    s_fill_rangeMax = Util_S32_Update( s_fill_rangeMax, 1, MONSNO_END_TEMP, true, true );
    break;
  }
}
wchar_t* ZukanDebugWinImpl::Fill_Range_Draw  (void* userWork, gfl2::debug::DebugWinItem* item)
{
  enum
  {
    ITEM_INDEX_RANGE_MIN = 0,
    ITEM_INDEX_RANGE_MAX = 1,
  };
  s32 itemIndex = reinterpret_cast<s32>(userWork);

  s32 val = s_fill_rangeMin;
  switch(itemIndex)
  {
  case ITEM_INDEX_RANGE_MIN:
    val = s_fill_rangeMin;
    break;
  case ITEM_INDEX_RANGE_MAX:
    val = s_fill_rangeMax;
    break;
  }

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  pml::personal::GetMonsName(s_inst->m_strBuf, static_cast<MonsNo>(val));
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%03d:%ls]", item->GetName(), val, s_inst->m_strBuf->GetPtr() );
  return workStr;
}
void     ZukanDebugWinImpl::Fill_TargetType_Update(void* /*userWork*/, gfl2::debug::DebugWinItem* /*item*/)
{
  s_fill_targetType = Util_S32_Update( s_fill_targetType, 0, FILL_TARGET_TYPE_MAX-1, true, false );
}
wchar_t* ZukanDebugWinImpl::Fill_TargetType_Draw  (void* /*userWork*/, gfl2::debug::DebugWinItem* item)
{
  static const u32 fill_targetTypeStrId[] =
  {
    msg_dmenu_zukan_fill_target_type_all,     // "全部"
//    msg_dmenu_zukan_fill_target_type_random,  // "ランダム"
    msg_dmenu_zukan_fill_target_type_10_percent,  // "10%"
    msg_dmenu_zukan_fill_target_type_30_percent,  // "30%"
    msg_dmenu_zukan_fill_target_type_50_percent,  // "50%"
    msg_dmenu_zukan_fill_target_type_70_percent,  // "70%"
    msg_dmenu_zukan_fill_target_type_90_percent,  // "90%"
    msg_dmenu_zukan_fill_target_type_95_percent,  // "95%"
  };
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  gfl2::str::StrBuf* workStrBuf = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetWorkStrBuf();
  s_inst->m_pMsgData->GetString(fill_targetTypeStrId[s_fill_targetType], *workStrBuf);
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%ls]", item->GetName(), workStrBuf->GetPtr() );
  return workStr;
}
void     ZukanDebugWinImpl::Fill_Sex_Update(void* /*userWork*/, gfl2::debug::DebugWinItem* /*item*/)
{
  s_fill_sex = Util_S32_Update( s_fill_sex, 0, FILL_SEX_MAX-1, true, false );
}
wchar_t* ZukanDebugWinImpl::Fill_Sex_Draw  (void* /*userWork*/, gfl2::debug::DebugWinItem* item)
{
  static const u32 fill_sexStrId[] =
  {
    msg_dmenu_zukan_fill_sex_all,     // "全部"
    msg_dmenu_zukan_fill_sex_male,    // "オス"
    msg_dmenu_zukan_fill_sex_female,  // "メス"
    msg_dmenu_zukan_fill_sex_random,  // "ランダム"
  };
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  gfl2::str::StrBuf* workStrBuf = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetWorkStrBuf();
  s_inst->m_pMsgData->GetString(fill_sexStrId[s_fill_sex], *workStrBuf);
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%ls]", item->GetName(), workStrBuf->GetPtr() );
  return workStr;
}
void     ZukanDebugWinImpl::Fill_Rare_Update(void* /*userWork*/, gfl2::debug::DebugWinItem* /*item*/)
{
  s_fill_rare = Util_S32_Update( s_fill_rare, 0, FILL_RARE_MAX-1, true, false );
}
wchar_t* ZukanDebugWinImpl::Fill_Rare_Draw  (void* /*userWork*/, gfl2::debug::DebugWinItem* item)
{
  static const u32 fill_rareStrId[] =
  {
    msg_dmenu_zukan_fill_rare_except,   // "レアなし"
    msg_dmenu_zukan_fill_rare_only,     // "レアのみ"
    msg_dmenu_zukan_fill_rare_both,     // "両方"
    msg_dmenu_zukan_fill_rare_random,   // "ランダム"
  };
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  gfl2::str::StrBuf* workStrBuf = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetWorkStrBuf();
  s_inst->m_pMsgData->GetString(fill_rareStrId[s_fill_rare], *workStrBuf);
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%ls]", item->GetName(), workStrBuf->GetPtr() );
  return workStr;
}
void     ZukanDebugWinImpl::Fill_Form_Update(void* /*userWork*/, gfl2::debug::DebugWinItem* /*item*/)
{
  s_fill_form = Util_S32_Update( s_fill_form, 0, FILL_FORM_MAX-1, true, false );
}
wchar_t* ZukanDebugWinImpl::Fill_Form_Draw  (void* /*userWork*/, gfl2::debug::DebugWinItem* item)
{
  static const u32 fill_formStrId[] =
  {
    msg_dmenu_zukan_fill_form_all,    // "全部"
    msg_dmenu_zukan_fill_form_normal, // "通常のみ"
    msg_dmenu_zukan_fill_form_random,  // "ランダム"
  };
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  gfl2::str::StrBuf* workStrBuf = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetWorkStrBuf();
  s_inst->m_pMsgData->GetString(fill_formStrId[s_fill_form], *workStrBuf);
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%ls]", item->GetName(), workStrBuf->GetPtr() );
  return workStr;
}
void     ZukanDebugWinImpl::Fill_Lang_Update(void* /*userWork*/, gfl2::debug::DebugWinItem* /*item*/)
{
  s_fill_lang = Util_S32_Update( s_fill_lang, 0, FILL_LANG_MAX-1, true, false );
}
wchar_t* ZukanDebugWinImpl::Fill_Lang_Draw  (void* /*userWork*/, gfl2::debug::DebugWinItem* item)
{
  static const u32 fill_langStrId[] =
  {
    msg_dmenu_zukan_fill_lang_all,    // "全部"
    msg_dmenu_zukan_fill_lang_jpn,    // "日本語"
    msg_dmenu_zukan_fill_lang_eng,    // "英語"
    msg_dmenu_zukan_fill_lang_fra,    // "フランス語"
    msg_dmenu_zukan_fill_lang_ita,    // "イタリア語"
    msg_dmenu_zukan_fill_lang_ger,    // "ドイツ語"
    msg_dmenu_zukan_fill_lang_spa,    // "スペイン語"
    msg_dmenu_zukan_fill_lang_kor,    // "韓国語"
    msg_dmenu_zukan_fill_lang_chn,    // "簡体字(中国)"
    msg_dmenu_zukan_fill_lang_twn,    // "繁体字(台湾)"
    msg_dmenu_zukan_fill_lang_random, // "ランダム"
  };
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  gfl2::str::StrBuf* workStrBuf = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetWorkStrBuf();
  s_inst->m_pMsgData->GetString(fill_langStrId[s_fill_lang], *workStrBuf);
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%ls]", item->GetName(), workStrBuf->GetPtr() );
  return workStr;
}

void     ZukanDebugWinImpl::See_Poke_Update(void* /*userWork*/, gfl2::debug::DebugWinItem* /*item*/)
{
  Savedata::ZukanData*  zukanData   = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();
  s_see_poke = Util_S32_Update( s_see_poke, 1, MONSNO_END_TEMP, true, true );
  s_see_count = zukanData->GetPokeSeeAmount(static_cast<MonsNo>(s_see_poke));
}
wchar_t* ZukanDebugWinImpl::See_Poke_Draw  (void* /*userWork*/, gfl2::debug::DebugWinItem* item)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  pml::personal::GetMonsName(s_inst->m_strBuf, static_cast<MonsNo>(s_see_poke));
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%03d:%ls]", item->GetName(), s_see_poke, s_inst->m_strBuf->GetPtr() );
  return workStr;
}
void     ZukanDebugWinImpl::See_Count_Update(void* /*userWork*/, gfl2::debug::DebugWinItem* /*item*/)
{
  Savedata::ZukanData*  zukanData   = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();
  s32 val = Util_S32_Update( zukanData->GetPokeSeeAmount(static_cast<MonsNo>(s_see_poke)), 0, 999, true, true );
  if(val != s_see_count)
  {
    zukanData->Debug_SetPokeSeeAmount( static_cast<MonsNo>(s_see_poke), val );
  }
  s_see_count = zukanData->GetPokeSeeAmount(static_cast<MonsNo>(s_see_poke));
}
wchar_t* ZukanDebugWinImpl::See_Count_Draw  (void* /*userWork*/, gfl2::debug::DebugWinItem* item)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), s_see_count );
  return workStr;
}

s32 ZukanDebugWinImpl::Util_S32_Update(s32 currVal, s32 minVal, s32 maxVal, bool isLoop, bool isJump)
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












#if NIJI_APP_ZUKAN_PRISM_DEVELOP

//==============================================================================
/**
 @class     ZukanDebugWinPrismImpl
 @brief     図鑑のデバッグウィンドウの実装
 */
//==============================================================================
class ZukanDebugWinPrismImpl
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanDebugWinPrismImpl);

public:
  ZukanDebugWinPrismImpl(gfl2::heap::HeapBase* heap, gfl2::debug::DebugWinGroup* parentGroup, gfl2::str::MsgData* pMsgData);
  virtual ~ZukanDebugWinPrismImpl();

public:
  // 分布チェックmode(0ならオフ、1ならオン)
  static s32 GetMapIconCheckMode(void);
  // ポケモンモデルSRT調整mode(0ならオフ、1ならオン)
  static s32 GetPokeModelSRTModifyMode(void);
  // QR範囲外チェック(0ならオフ、1以上ならオン)
  static s32 GetQROutOfRangeVal(void);
  // QR全ポケモン表示(0ならオフ、1ならオン)
  static s32 GetQRAllDispFlag(void);

private:
  static void     Initialize(ZukanDebugWinPrismImpl* inst);
  static void     Terminate(void);

  // キラキラ
  static void     Prism_Val_Update(void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* Prism_Val_Draw  (void* userWork, gfl2::debug::DebugWinItem* item);
  static void     Prism_Decide_Update(void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* Prism_Decide_Draw  (void* userWork, gfl2::debug::DebugWinItem* item);

  static void     GetPrismItem(s32 itemIndex, s32** itemVar, s32* itemValMin, s32* itemValMax);
  
  // ReduceParam
  static void     PrismReduceParam_Val_Update(void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* PrismReduceParam_Val_Draw  (void* userWork, gfl2::debug::DebugWinItem* item);

  // その他開発向けmode
  static void     OtherDevMode_Val_Update(void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* OtherDevMode_Val_Draw  (void* userWork, gfl2::debug::DebugWinItem* item);
  static void     QROutOfRangeVal_Update(void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* QROutOfRangeVal_Draw  (void* userWork, gfl2::debug::DebugWinItem* item);
  static void     QRAllDispFlag_Update(void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* QRAllDispFlag_Draw  (void* userWork, gfl2::debug::DebugWinItem* item);

  static s32      UpdateS32Val(s32 currVal, s32 minVal, s32 maxVal, bool isLoop, bool isJump);

private:
  enum
  {
    ITEM_INDEX_TARGET_PLACE          =  0,
    ITEM_INDEX_TARGET_COLOR          =  1,
    ITEM_INDEX_MODE                  =  2,
    ITEM_INDEX_CONST_COLOR           =  3,
    ITEM_INDEX_GYRO_ANGLE            =  4,
    ITEM_INDEX_FUNC_TYPE             =  5,
    ITEM_INDEX_SHINPUKU_MIN          =  6,
    ITEM_INDEX_SHINPUKU_MAX          =  7,
    ITEM_INDEX_SYUUKI                =  8,
    ITEM_INDEX_ISOU                  =  9,
    ITEM_INDEX_ISOU_X                = 10,
    ITEM_INDEX_ISOU_Y                = 11,
    ITEM_INDEX_GYRO_ANGLE_PRINT_FLAG = 12,
  };

  // ReduceParam
  enum
  {
    REDUCE_PARAM_MOVE_ANGLE_BIG         = 0,
    REDUCE_PARAM_MOVE_ANGLE_SMALL       = 1,
    REDUCE_PARAM_REDUCE_RATE_BIG        = 2,
    REDUCE_PARAM_REDUCE_RATE_SMALL      = 3,
    REDUCE_PARAM_REDUCE_SPEED_TO_DARK   = 4,
    REDUCE_PARAM_REDUCE_SPEED_FROM_DARK = 5,
  }; 

  enum
  {
    OTHER_DEV_MODE_NONE                    = 0,
    OTHER_DEV_MODE_MAP_ICON_CHECK          = 1,  // 分布画面にてマップ上にアイコンを全部出す
    OTHER_DEV_MODE_POKE_MODEL_SRT_MODIFY   = 2,  // 姿/鳴き声画面にて中央配置のポケモンモデルの位置を調整する
    OTHER_DEV_MODE_MAX                     = 3,
  };

private:
  static ZukanDebugWinPrismImpl* s_inst;

  static s32 s_otherDevMode;  // その他開発向けmode  // OTHER_DEV_MODE_
  static s32 s_qrOutOfRangeVal;
  static s32 s_qrAllDispFlag;

private:
  gfl2::heap::HeapBase*       m_heap;
  gfl2::debug::DebugWinGroup* m_parentGroup;
  gfl2::str::MsgData*         m_pMsgData;

  gfl2::debug::DebugWinGroup* m_zukanPrismGroup;   // キラキラグループトップ

  gfl2::str::StrBuf* m_strBuf;
};




//==============================================================================
//
//  ZukanDebugWinPrismImpl
//
//==============================================================================
ZukanDebugWinPrismImpl::ZukanDebugWinPrismImpl(gfl2::heap::HeapBase* heap, gfl2::debug::DebugWinGroup* parentGroup, gfl2::str::MsgData* pMsgData)
  : m_heap(heap),
    m_parentGroup(parentGroup),
    m_zukanPrismGroup(NULL),
    m_strBuf(NULL),
    m_pMsgData(pMsgData)
{
  Initialize(this);
  
  // 親
  m_zukanPrismGroup = gfl2::debug::DebugWin_AddGroup(m_heap, m_pMsgData, msg_dmenu_dev_zukan_prism_top, parentGroup);
  
  // キラキラ
  {
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanPrismGroup, m_heap, m_pMsgData, msg_dmenu_dev_zukan_prism_00, reinterpret_cast<void*>( 0), Prism_Val_Update, Prism_Val_Draw);  // targetPlace
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanPrismGroup, m_heap, m_pMsgData, msg_dmenu_dev_zukan_prism_01, reinterpret_cast<void*>( 1), Prism_Val_Update, Prism_Val_Draw);  // targetColor
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanPrismGroup, m_heap, m_pMsgData, msg_dmenu_dev_zukan_prism_02, reinterpret_cast<void*>( 2), Prism_Val_Update, Prism_Val_Draw);  // mode
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanPrismGroup, m_heap, m_pMsgData, msg_dmenu_dev_zukan_prism_03, reinterpret_cast<void*>( 3), Prism_Val_Update, Prism_Val_Draw);  // constColor
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanPrismGroup, m_heap, m_pMsgData, msg_dmenu_dev_zukan_prism_04, reinterpret_cast<void*>( 4), Prism_Val_Update, Prism_Val_Draw);  // gyroAngle
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanPrismGroup, m_heap, m_pMsgData, msg_dmenu_dev_zukan_prism_05, reinterpret_cast<void*>( 5), Prism_Val_Update, Prism_Val_Draw);  // funcType
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanPrismGroup, m_heap, m_pMsgData, msg_dmenu_dev_zukan_prism_06, reinterpret_cast<void*>( 6), Prism_Val_Update, Prism_Val_Draw);  // shinpuku_min
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanPrismGroup, m_heap, m_pMsgData, msg_dmenu_dev_zukan_prism_07, reinterpret_cast<void*>( 7), Prism_Val_Update, Prism_Val_Draw);  // shinpuku_max
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanPrismGroup, m_heap, m_pMsgData, msg_dmenu_dev_zukan_prism_08, reinterpret_cast<void*>( 8), Prism_Val_Update, Prism_Val_Draw);  // syuuki
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanPrismGroup, m_heap, m_pMsgData, msg_dmenu_dev_zukan_prism_09, reinterpret_cast<void*>( 9), Prism_Val_Update, Prism_Val_Draw);  // isou
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanPrismGroup, m_heap, m_pMsgData, msg_dmenu_dev_zukan_prism_10, reinterpret_cast<void*>(10), Prism_Val_Update, Prism_Val_Draw);  // isou_x
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanPrismGroup, m_heap, m_pMsgData, msg_dmenu_dev_zukan_prism_11, reinterpret_cast<void*>(11), Prism_Val_Update, Prism_Val_Draw);  // isou_y      
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanPrismGroup, m_heap, m_pMsgData, msg_dmenu_dev_zukan_prism_23, reinterpret_cast<void*>( 0), Prism_Decide_Update, Prism_Decide_Draw);  // print
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanPrismGroup, m_heap, m_pMsgData, msg_dmenu_dev_zukan_prism_24, reinterpret_cast<void*>( 1), Prism_Decide_Update, Prism_Decide_Draw);  // reset
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanPrismGroup, m_heap, m_pMsgData, msg_dmenu_dev_zukan_prism_25, reinterpret_cast<void*>( 2), Prism_Decide_Update, Prism_Decide_Draw);  // allReset
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanPrismGroup, m_heap, m_pMsgData, msg_dmenu_dev_zukan_prism_26, reinterpret_cast<void*>(12), Prism_Val_Update, Prism_Val_Draw);  // gyroAnglePrintFlag
  }
   
  // 分布チェック、ポケモンモデルSRT調整
  gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanPrismGroup, m_heap, m_pMsgData, msg_dmenu_dev_zukan_prism_27, NULL, OtherDevMode_Val_Update, OtherDevMode_Val_Draw);  // その他開発向けmode
  // QR
  gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanPrismGroup, m_heap, m_pMsgData, msg_dmenu_dev_zukan_prism_37, NULL, QROutOfRangeVal_Update, QROutOfRangeVal_Draw);  // QROutOfRangeVal
  gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanPrismGroup, m_heap, m_pMsgData, msg_dmenu_dev_zukan_prism_38, NULL, QRAllDispFlag_Update  , QRAllDispFlag_Draw  );  // QRAllDispFlag

  // ReduceParam
  {
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanPrismGroup, m_heap, m_pMsgData, msg_dmenu_dev_zukan_prism_31, reinterpret_cast<void*>(0), PrismReduceParam_Val_Update, PrismReduceParam_Val_Draw);  // moveAngleBig       
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanPrismGroup, m_heap, m_pMsgData, msg_dmenu_dev_zukan_prism_31, reinterpret_cast<void*>(1), PrismReduceParam_Val_Update, PrismReduceParam_Val_Draw);  // moveAngleSmall     
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanPrismGroup, m_heap, m_pMsgData, msg_dmenu_dev_zukan_prism_32, reinterpret_cast<void*>(2), PrismReduceParam_Val_Update, PrismReduceParam_Val_Draw);  // reduceRateBig      
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanPrismGroup, m_heap, m_pMsgData, msg_dmenu_dev_zukan_prism_32, reinterpret_cast<void*>(3), PrismReduceParam_Val_Update, PrismReduceParam_Val_Draw);  // reduceRateSmall    
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanPrismGroup, m_heap, m_pMsgData, msg_dmenu_dev_zukan_prism_34, reinterpret_cast<void*>(4), PrismReduceParam_Val_Update, PrismReduceParam_Val_Draw);  // reduceSpeedToDark  
    gfl2::debug::DebugWin_AddItemUpdateFunc(m_zukanPrismGroup, m_heap, m_pMsgData, msg_dmenu_dev_zukan_prism_34, reinterpret_cast<void*>(5), PrismReduceParam_Val_Update, PrismReduceParam_Val_Draw);  // reduceSpeedFromDark
  } 

  m_strBuf = GFL_NEW(m_heap) gfl2::str::StrBuf(16, m_heap);
}
ZukanDebugWinPrismImpl::~ZukanDebugWinPrismImpl()
{
  GFL_SAFE_DELETE(m_strBuf);

  gfl2::debug::DebugWin_RemoveGroup(m_zukanPrismGroup);
  
  Terminate();
}

ZukanDebugWinPrismImpl* ZukanDebugWinPrismImpl::s_inst = NULL;

s32 ZukanDebugWinPrismImpl::s_otherDevMode = 0;
s32 ZukanDebugWinPrismImpl::s_qrOutOfRangeVal = 0;
s32 ZukanDebugWinPrismImpl::s_qrAllDispFlag = 0;

s32 ZukanDebugWinPrismImpl::GetMapIconCheckMode(void)
{
  if(s_otherDevMode == OTHER_DEV_MODE_MAP_ICON_CHECK)
  {
    return 1;
  }
  return 0;
}
s32 ZukanDebugWinPrismImpl::GetPokeModelSRTModifyMode(void)
{
  if(s_otherDevMode == OTHER_DEV_MODE_POKE_MODEL_SRT_MODIFY)
  {
    return 1;
  }
  return 0;
}
s32 ZukanDebugWinPrismImpl::GetQROutOfRangeVal(void)
{
  return s_qrOutOfRangeVal;
}
s32 ZukanDebugWinPrismImpl::GetQRAllDispFlag(void)
{
  return s_qrAllDispFlag;
}

void     ZukanDebugWinPrismImpl::Initialize(ZukanDebugWinPrismImpl* inst)
{
  s_inst = inst;
}
void     ZukanDebugWinPrismImpl::Terminate(void)
{}

void     ZukanDebugWinPrismImpl::Prism_Val_Update(void* userWork, gfl2::debug::DebugWinItem* /*item*/)
{
  s32 itemIndex = reinterpret_cast<s32>(userWork);

  s32* var    = NULL;
  s32  valMin = 0;
  s32  valMax = 0;
  GetPrismItem(itemIndex, &var, &valMin, &valMax); 

  bool isJump = false;
  if(    itemIndex == ITEM_INDEX_CONST_COLOR
      || itemIndex == ITEM_INDEX_SHINPUKU_MIN
      || itemIndex == ITEM_INDEX_SHINPUKU_MAX
      || itemIndex == ITEM_INDEX_SYUUKI      
      || itemIndex == ITEM_INDEX_ISOU        
      || itemIndex == ITEM_INDEX_ISOU_X      
      || itemIndex == ITEM_INDEX_ISOU_Y       )
  {
    isJump = true;
  }

  *var = UpdateS32Val(*var, valMin, valMax, false, isJump);
}
wchar_t* ZukanDebugWinPrismImpl::Prism_Val_Draw  (void* userWork, gfl2::debug::DebugWinItem* item)
{
  s32 itemIndex = reinterpret_cast<s32>(userWork);
  
  s32* var    = NULL;
  s32  valMin = 0;
  s32  valMax = 0;
  GetPrismItem(itemIndex, &var, &valMin, &valMax); 

  bool printStr = false;
  switch(itemIndex)
  {
  case ITEM_INDEX_TARGET_PLACE:
    {
      s_inst->m_pMsgData->GetString(msg_dmenu_dev_zukan_prism_12+(*var), *(s_inst->m_strBuf));
      printStr = true;
      break;
    }
  case ITEM_INDEX_MODE        : 
    {
      s_inst->m_pMsgData->GetString(msg_dmenu_dev_zukan_prism_18+(*var), *(s_inst->m_strBuf));
      printStr = true;
      break;
    }
  case ITEM_INDEX_GYRO_ANGLE  :
    {
      if(*var == ZukanPrismDevelopParam::GYRO_ANGLE_AVE )
      {
        s_inst->m_pMsgData->GetString(msg_dmenu_dev_zukan_prism_20, *(s_inst->m_strBuf));
        printStr = true;
      }
      break;
    }
  case ITEM_INDEX_FUNC_TYPE   :
    {
      s_inst->m_pMsgData->GetString(msg_dmenu_dev_zukan_prism_21+(*var), *(s_inst->m_strBuf));
      printStr = true;
      break;
    }
  }

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  if(printStr)
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%09d:%ls]", item->GetName(), *var, s_inst->m_strBuf->GetPtr() );
  }
  else
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%09d]", item->GetName(), *var );
  }
  return workStr;
}

void     ZukanDebugWinPrismImpl::Prism_Decide_Update(void* userWork, gfl2::debug::DebugWinItem* /*item*/)
{
  s32 itemIndex = reinterpret_cast<s32>(userWork);
  
  gfl2::ui::Button* uiButton = gfl2::debug::DebugWin_GetUiButton();
  
  switch(itemIndex)
  {
  case 0:
    {
      if( uiButton->IsTrigger( gfl2::ui::BUTTON_A ) )
      {
        ZukanPrismDevelopParam::Print();
      }
      break;
    }
  case 1:
    {
      if( uiButton->IsTrigger( gfl2::ui::BUTTON_A ) && uiButton->IsHold( gfl2::ui::BUTTON_Y ) )
      {
        ZukanPrismDevelopParam::Reset(false);
      }
      break;
    }
  case 2:
    {
      if( uiButton->IsTrigger( gfl2::ui::BUTTON_A ) && uiButton->IsHold( gfl2::ui::BUTTON_Y ) )
      {
        ZukanPrismDevelopParam::Reset(true);
      }
      break;
    }
  }
}
wchar_t* ZukanDebugWinPrismImpl::Prism_Decide_Draw  (void* /*userWork*/, gfl2::debug::DebugWinItem* item)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls", item->GetName() );
  return workStr;
}

void     ZukanDebugWinPrismImpl::GetPrismItem(s32 itemIndex, s32** itemVar, s32* itemValMin, s32* itemValMax)
{
  const s32 BAIRITSU = 100;

  ZukanPrismDevelopParam::TargetParam* currTargetParam = &(ZukanPrismDevelopParam::s_targetParam[ZukanPrismDevelopParam::s_currTargetPlace][ZukanPrismDevelopParam::s_currTargetColor[ZukanPrismDevelopParam::s_currTargetPlace]]);

  switch(itemIndex)
  {
  case ITEM_INDEX_TARGET_PLACE         : *itemVar = &(ZukanPrismDevelopParam::s_currTargetPlace);                                            *itemValMin = 0;              *itemValMax = ZukanPrismDevelopParam::TARGET_PLACE_NUM-1; break;
  case ITEM_INDEX_TARGET_COLOR         : *itemVar = &(ZukanPrismDevelopParam::s_currTargetColor[ZukanPrismDevelopParam::s_currTargetPlace]); *itemValMin = 0;              *itemValMax = ZukanPrismDevelopParam::TARGET_COLOR_NUM-1; break;
  case ITEM_INDEX_MODE                 : *itemVar = &(currTargetParam->mode);                                                                *itemValMin = 0;              *itemValMax = ZukanPrismDevelopParam::MODE_NUM-1;         break;
  case ITEM_INDEX_CONST_COLOR          : *itemVar = &(currTargetParam->constColor);                                                          *itemValMin = 0;              *itemValMax = 255;                                        break;
  case ITEM_INDEX_GYRO_ANGLE           : *itemVar = &(currTargetParam->gyroAngle);                                                           *itemValMin = 0;              *itemValMax = ZukanPrismDevelopParam::GYRO_ANGLE_NUM-1;   break;
  case ITEM_INDEX_FUNC_TYPE            : *itemVar = &(currTargetParam->funcType);                                                            *itemValMin = 0;              *itemValMax = ZukanPrismDevelopParam::FUNC_TYPE_NUM-1;    break;
  case ITEM_INDEX_SHINPUKU_MIN         : *itemVar = &(currTargetParam->funcParam[ZukanPrismDevelopParam::FUNC_PARAM_SHINPUKU_MIN]);          *itemValMin = -10*BAIRITSU;   *itemValMax = 10*BAIRITSU;                                break;
  case ITEM_INDEX_SHINPUKU_MAX         : *itemVar = &(currTargetParam->funcParam[ZukanPrismDevelopParam::FUNC_PARAM_SHINPUKU_MAX]);          *itemValMin = -10*BAIRITSU;   *itemValMax = 10*BAIRITSU;                                break;
  case ITEM_INDEX_SYUUKI               : *itemVar = &(currTargetParam->funcParam[ZukanPrismDevelopParam::FUNC_PARAM_SYUUKI      ]);          *itemValMin = -1000*BAIRITSU; *itemValMax = 1000*BAIRITSU;                              break;
  case ITEM_INDEX_ISOU                 : *itemVar = &(currTargetParam->funcParam[ZukanPrismDevelopParam::FUNC_PARAM_ISOU        ]);          *itemValMin = -1000*BAIRITSU; *itemValMax = 1000*BAIRITSU;                              break;
  case ITEM_INDEX_ISOU_X               : *itemVar = &(currTargetParam->funcParam[ZukanPrismDevelopParam::FUNC_PARAM_ISOU_X      ]);          *itemValMin = -1000*BAIRITSU; *itemValMax = 1000*BAIRITSU;                              break;
  case ITEM_INDEX_ISOU_Y               : *itemVar = &(currTargetParam->funcParam[ZukanPrismDevelopParam::FUNC_PARAM_ISOU_Y      ]);          *itemValMin = -1000*BAIRITSU; *itemValMax = 1000*BAIRITSU;                              break;
  case ITEM_INDEX_GYRO_ANGLE_PRINT_FLAG: *itemVar = &(ZukanPrismDevelopParam::s_gyroAnglePrintFlag);                                         *itemValMin = 0;              *itemValMax = 1;                                          break;
  }
}

// ReduceParam
void     ZukanDebugWinPrismImpl::PrismReduceParam_Val_Update(void* userWork, gfl2::debug::DebugWinItem* /*item*/)
{
  const s32 BAIRITSU = 100;
  
  s32 itemIndex = reinterpret_cast<s32>(userWork);

  s32* var = NULL;
  s32 valMin = 0;
  s32 valMax = 0;

  switch(itemIndex)
  {
  case REDUCE_PARAM_MOVE_ANGLE_BIG        : var = &(ZukanPrismDevelopParam::s_reduceParam.moveAngleBig);         valMin = 0;  valMax =  360;  break;
  case REDUCE_PARAM_MOVE_ANGLE_SMALL      : var = &(ZukanPrismDevelopParam::s_reduceParam.moveAngleSmall);       valMin = 0;  valMax =  360;  break;    
  case REDUCE_PARAM_REDUCE_RATE_BIG       : var = &(ZukanPrismDevelopParam::s_reduceParam.reduceRateBig);        valMin = 0;  valMax =  100;  break;
  case REDUCE_PARAM_REDUCE_RATE_SMALL     : var = &(ZukanPrismDevelopParam::s_reduceParam.reduceRateSmall);      valMin = 0;  valMax =  100;  break;
  case REDUCE_PARAM_REDUCE_SPEED_TO_DARK  : var = &(ZukanPrismDevelopParam::s_reduceParam.reduceSpeedToDark);    valMin = 0;  valMax = 3000;  break;
  case REDUCE_PARAM_REDUCE_SPEED_FROM_DARK: var = &(ZukanPrismDevelopParam::s_reduceParam.reduceSpeedFromDark);  valMin = 0;  valMax = 3000;  break;
  }

  *var = UpdateS32Val(*var, valMin*BAIRITSU, valMax*BAIRITSU, false, true);
}
wchar_t* ZukanDebugWinPrismImpl::PrismReduceParam_Val_Draw  (void* userWork, gfl2::debug::DebugWinItem* item)
{
  s32 itemIndex = reinterpret_cast<s32>(userWork);

  s32* var = NULL;
  u32 msgId = 0;
  
  switch(itemIndex)
  {
  case REDUCE_PARAM_MOVE_ANGLE_BIG        : var = &(ZukanPrismDevelopParam::s_reduceParam.moveAngleBig);         msgId = msg_dmenu_dev_zukan_prism_30;  break;
  case REDUCE_PARAM_MOVE_ANGLE_SMALL      : var = &(ZukanPrismDevelopParam::s_reduceParam.moveAngleSmall);       msgId = msg_dmenu_dev_zukan_prism_29;  break;    
  case REDUCE_PARAM_REDUCE_RATE_BIG       : var = &(ZukanPrismDevelopParam::s_reduceParam.reduceRateBig);        msgId = msg_dmenu_dev_zukan_prism_30;  break;
  case REDUCE_PARAM_REDUCE_RATE_SMALL     : var = &(ZukanPrismDevelopParam::s_reduceParam.reduceRateSmall);      msgId = msg_dmenu_dev_zukan_prism_29;  break;
  case REDUCE_PARAM_REDUCE_SPEED_TO_DARK  : var = &(ZukanPrismDevelopParam::s_reduceParam.reduceSpeedToDark);    msgId = msg_dmenu_dev_zukan_prism_28;  break;
  case REDUCE_PARAM_REDUCE_SPEED_FROM_DARK: var = &(ZukanPrismDevelopParam::s_reduceParam.reduceSpeedFromDark);  msgId = msg_dmenu_dev_zukan_prism_28;  break;
  }

  s_inst->m_pMsgData->GetString(msgId, *(s_inst->m_strBuf));

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  
  if(itemIndex == REDUCE_PARAM_REDUCE_SPEED_TO_DARK)
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls %ls->[%09d]", item->GetName(), s_inst->m_strBuf->GetPtr(), *var );
  }
  else if(itemIndex == REDUCE_PARAM_REDUCE_SPEED_FROM_DARK)
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls %ls<-[%09d]", item->GetName(), s_inst->m_strBuf->GetPtr(), *var );
  }
  else
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls %ls[%09d]", item->GetName(), s_inst->m_strBuf->GetPtr(), *var );
  }

  return workStr;
}

s32      ZukanDebugWinPrismImpl::UpdateS32Val(s32 currVal, s32 minVal, s32 maxVal, bool isLoop, bool isJump)
{
  gfl2::ui::Button* uiButton = gfl2::debug::DebugWin_GetUiButton();

  s32 retVal = currVal;

  s32 delta = 1;
  if(isJump)
  {
    if( uiButton->IsHold( gfl2::ui::BUTTON_R ) && uiButton->IsHold( gfl2::ui::BUTTON_L ) )
    {
      delta = 1000;
    }
    else if( uiButton->IsHold( gfl2::ui::BUTTON_R ) )
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

void     ZukanDebugWinPrismImpl::OtherDevMode_Val_Update(void* /*userWork*/, gfl2::debug::DebugWinItem* /*item*/)
{
#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_OTHER_DEV_MODE_ENABLE
  s_otherDevMode = UpdateS32Val(s_otherDevMode, 0, OTHER_DEV_MODE_MAX-1, true, false);
#endif
}
wchar_t* ZukanDebugWinPrismImpl::OtherDevMode_Val_Draw  (void* /*userWork*/, gfl2::debug::DebugWinItem* item)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), s_otherDevMode );
  return workStr;
}
void     ZukanDebugWinPrismImpl::QROutOfRangeVal_Update(void* /*userWork*/, gfl2::debug::DebugWinItem* /*item*/)
{
#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_OTHER_DEV_MODE_ENABLE
  s_qrOutOfRangeVal = UpdateS32Val(s_qrOutOfRangeVal, 0, 8, true, false);
#endif
}
wchar_t* ZukanDebugWinPrismImpl::QROutOfRangeVal_Draw  (void* /*userWork*/, gfl2::debug::DebugWinItem* item)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), s_qrOutOfRangeVal );
  return workStr;
}
void     ZukanDebugWinPrismImpl::QRAllDispFlag_Update(void* /*userWork*/, gfl2::debug::DebugWinItem* /*item*/)
{
#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_OTHER_DEV_MODE_ENABLE
  s_qrAllDispFlag = UpdateS32Val(s_qrAllDispFlag, 0, 1, true, false);
#endif
}
wchar_t* ZukanDebugWinPrismImpl::QRAllDispFlag_Draw  (void* /*userWork*/, gfl2::debug::DebugWinItem* item)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), s_qrAllDispFlag );
  return workStr;
}

#endif  // NIJI_APP_ZUKAN_PRISM_DEVELOP












//==============================================================================
//
//  ZukanDebugWin
//
//==============================================================================
ZukanDebugWin::ZukanDebugWin(gfl2::heap::HeapBase* heap, gfl2::debug::DebugWinGroup* parentGroup)
  : m_heap(heap),
    m_parentGroup(parentGroup),
    m_pMsgData(NULL),
    m_impl(NULL),
    m_prismImpl(NULL)
{
  ::Debug::DebugWin::DebugWinWork* devWork = GFL_SINGLETON_INSTANCE( ::Debug::DebugWin::DebugWinWork );
  if( devWork ) m_pMsgData = devWork->GetMessageData();
  m_impl      = GFL_NEW(heap) ZukanDebugWinImpl(m_heap, m_parentGroup, m_pMsgData);
#if NIJI_APP_ZUKAN_PRISM_DEVELOP
  m_prismImpl = GFL_NEW(heap) ZukanDebugWinPrismImpl(m_heap, m_parentGroup, m_pMsgData);
#endif
}
ZukanDebugWin::~ZukanDebugWin()
{
#if NIJI_APP_ZUKAN_PRISM_DEVELOP
  GFL_SAFE_DELETE(m_prismImpl);
#endif
  GFL_SAFE_DELETE(m_impl);
}

s32 ZukanDebugWin::GetMapIconCheckMode(void)
{
#if NIJI_APP_ZUKAN_PRISM_DEVELOP
  return ZukanDebugWinPrismImpl::GetMapIconCheckMode();
#else
  return 0;
#endif
}
s32 ZukanDebugWin::GetPokeModelSRTModifyMode(void)
{
#if NIJI_APP_ZUKAN_PRISM_DEVELOP
  return ZukanDebugWinPrismImpl::GetPokeModelSRTModifyMode();
#else
  return 0;
#endif
}
s32 ZukanDebugWin::GetQROutOfRangeVal(void)
{
#if NIJI_APP_ZUKAN_PRISM_DEVELOP
  return ZukanDebugWinPrismImpl::GetQROutOfRangeVal();
#else
  return 0;
#endif
}
s32 ZukanDebugWin::GetQRAllDispFlag(void)
{
#if NIJI_APP_ZUKAN_PRISM_DEVELOP
  return ZukanDebugWinPrismImpl::GetQRAllDispFlag();
#else
  return 0;
#endif
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // PM_DEBUG

