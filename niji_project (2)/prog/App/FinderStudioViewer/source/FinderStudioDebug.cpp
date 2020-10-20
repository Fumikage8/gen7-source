//======================================================================
/**
 * @file FinderStudioMain.cpp
 * @date 2016/11/30
 * @author harada_takumi
 * @brief ファインダースタジオデバッグ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioDebug.h"

// Debug
#if PM_DEBUG
#include <debug/include/gfl2_DebugWinSystem.h>
#include <debug/include/gfl2_DebugWinItemTypes.h>
#endif

// DemoLib
#include "DemoLib/Object/include/Object_Poke.h"

// FinderStudioStatic
#include "App/FinderStudioStatic/include/FinderStudioViewerDefine.h"
#include "App/FinderStudioStatic/include/FinderStudioBGDataAccessor.h"

// FinderStudioViewer
#include "../include/FinderStudioViewer.h"
#include "FinderStudioPokemon.h"
#include "FinderStudioDressUpChara.h"
#include "FinderStudioBackGround.h"
#include "FinderStudioRenderSystem.h"
#include "FinderStudioCameraSystem.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderStudio)
GFL_NAMESPACE_BEGIN(Viewer)

#if PM_DEBUG

static gfl2::heap::HeapBase* pDebugHeap = NULL;
static gfl2::debug::DebugWinGroup* pRootDebugWinGroup = NULL;

static FinderStudioViewer* pMain = NULL;

// バトルorかわいがり
static wchar_t* POKE_MOTION_TYPE[] =
{
  L"バトル",
  L"かわいがり",
  L"フィールド"
};
static PokeMotionType pokeMotionType;

// バトルモーション一覧
static wchar_t* POKE_MOTION_BATTLE[] =
{
  L"MODEL_ANIME_BT_WAIT_A",      ///待機A
  L"MODEL_ANIME_BT_WAIT_B",      ///待機B
  L"MODEL_ANIME_BT_WAIT_C",      ///待機C
  L"MODEL_ANIME_LAND_A",         ///登場1(飛び出し)
  L"MODEL_ANIME_LAND_B",         ///登場1(落下ループ)
  L"MODEL_ANIME_LAND_C",         ///登場1(着地)
  L"MODEL_ANIME_LAND",           ///登場2
  L"MODEL_ANIME_APPEAL",         ///メガシンカアピール
  L"MODEL_ANIME_BUTURI1",        ///物理攻撃１
  L"MODEL_ANIME_BUTURI2",        ///物理攻撃２
  L"MODEL_ANIME_BUTURI3",        ///物理攻撃３
  L"MODEL_ANIME_BUTURI4",        ///物理攻撃４
  L"MODEL_ANIME_TOKUSHU1",       ///特殊攻撃１
  L"MODEL_ANIME_TOKUSHU2",       ///特殊攻撃２
  L"MODEL_ANIME_TOKUSHU3",       ///特殊攻撃３
  L"MODEL_ANIME_TOKUSHU4",       ///特殊攻撃４
  L"MODEL_ANIME_DAMAGE_S",       ///ダメージ
  L"MODEL_ANIME_DOWN",           ///ダウン
};

// 使用するバトルモーション一覧
static PokeTool::MODEL_ANIME POKE_MOTION_BATTLE_USED[] =
{
  PokeTool::MODEL_ANIME_BT_WAIT_A,
  PokeTool::MODEL_ANIME_BT_WAIT_B,
  PokeTool::MODEL_ANIME_BT_WAIT_C,
  PokeTool::MODEL_ANIME_BUTURI1,
  PokeTool::MODEL_ANIME_BUTURI2,
  PokeTool::MODEL_ANIME_BUTURI3,
  PokeTool::MODEL_ANIME_BUTURI4,
  PokeTool::MODEL_ANIME_TOKUSHU1,
  PokeTool::MODEL_ANIME_TOKUSHU2,
  PokeTool::MODEL_ANIME_TOKUSHU3,
  PokeTool::MODEL_ANIME_TOKUSHU4
};

// かわいがりモーション一覧
static wchar_t* POKE_MOTION_KAWAIGARI[] =
{
  L"MODEL_ANIME_KW_WAIT",        ///待機
  L"MODEL_ANIME_RESPOND",        ///反応する
  L"MODEL_ANIME_TURN_A",         ///振り向き１
  L"MODEL_ANIME_TURN_B",         ///振り向き２
  L"MODEL_ANIME_DROWSE_A",       ///うたたね(開始)
  L"MODEL_ANIME_DROWSE_B",       ///うたたね(ループ)
  L"MODEL_ANIME_DROWSE_C",       ///うたたね(起きる)
  L"MODEL_ANIME_SLEEP_A",        ///ねむる(ループ)
  L"MODEL_ANIME_SLEEP_B",        ///ねむる(目を覚ます)
  L"MODEL_ANIME_HATE",           ///いやがる
  L"MODEL_ANIME_QUESTION",       ///わからない
  L"MODEL_ANIME_HAPPY_A",        ///よろこぶ１
  L"MODEL_ANIME_HAPPY_B",        ///よろこぶ２
  L"MODEL_ANIME_HAPPY_C",        ///よろこぶ３
  L"MODEL_ANIME_MOVE_A",         ///くせ１
  L"MODEL_ANIME_MOVE_B",         ///くせ２
  L"MODEL_ANIME_MOVE_C",         ///くせ３
  L"MODEL_ANIME_MOVE_D",         ///くせ４
  L"MODEL_ANIME_LONELY",         ///さみしがる
  L"MODEL_ANIME_PLAY_A",         ///かまってほしい１
  L"MODEL_ANIME_PLAY_B",         ///かまってほしい２
  L"MODEL_ANIME_MAD",            ///怒る
  L"MODEL_ANIME_EAT_A",          ///かぶりつく(開始)
  L"MODEL_ANIME_EAT_B",          ///かぶりつく(ループ)
  L"MODEL_ANIME_EAT_C",          ///かぶりつく(終了)
  L"MODEL_ANIME_TOUCH",          ///タッチ
};

//  使用するかわいがりモーション一覧
static PokeTool::MODEL_ANIME POKE_MOTION_KAWAIGARI_USED[] =
{
  PokeTool::MODEL_ANIME_KW_WAIT,
  PokeTool::MODEL_ANIME_DROWSE_A,
  PokeTool::MODEL_ANIME_DROWSE_B,
  PokeTool::MODEL_ANIME_DROWSE_C,
  PokeTool::MODEL_ANIME_HATE,
  PokeTool::MODEL_ANIME_QUESTION,
  PokeTool::MODEL_ANIME_HAPPY_A,
  PokeTool::MODEL_ANIME_HAPPY_B,
  PokeTool::MODEL_ANIME_HAPPY_C,
  PokeTool::MODEL_ANIME_MOVE_A,
  PokeTool::MODEL_ANIME_MOVE_B,
  PokeTool::MODEL_ANIME_MOVE_C,
  PokeTool::MODEL_ANIME_MOVE_D,
  PokeTool::MODEL_ANIME_PLAY_A,
  PokeTool::MODEL_ANIME_PLAY_B,
  PokeTool::MODEL_ANIME_MAD
};

// フィールドモーション一覧
static wchar_t* POKE_MOTION_FIELD[] =
{
  L"MODEL_ANIME_FI_WAIT_A",       //移動専用待機01
  L"MODEL_ANIME_FI_WAIT_B",       //移動専用待機02（ランダム再生）
  L"MODEL_ANIME_WALK01",          //歩き
  L"MODEL_ANIME_RUN01",           //走り
  L"MODEL_ANIME_MERGE_WALK01",    //(待機＞歩き＞待機)
  L"MODEL_ANIME_WAIT_WALK01",     //待機＞歩きへ移行
  L"MODEL_ANIME_WALK_WAIT01",     //歩き＞待機へ移行
  L"MODEL_ANIME_MERGE_RUN01",     //(待機＞走り＞待機)
  L"MODEL_ANIME_WAIT_RUN01",      //待機＞走りへ移行
  L"MODEL_ANIME_RUN_WAIT01",      //走り＞待機へ移行
  L"MODEL_ANIME_MERGE_MOVE01",    //(歩き＞走り＞歩き)
  L"MODEL_ANIME_WALK_RUN01",      //歩き＞走りへ移行
  L"MODEL_ANIME_RUN_WALK01",      //走り＞歩きへ移行
};
static u32 pokeMotionNo = 0;
static u32 pokeMotionNoUsedIdx = 0;

// デバッグで制御するか
static bool isEnableDebugWaitMotion = false;
// 自動モーションが有効か
static bool isEnableWaitMotion = true;

// モーションブレンドが有効か
static bool isEnableMotionBlend = false;

// バトルorかわいがりorフィールド
static void UpdatePokeMotionType( void* userWork, gfl2::debug::DebugWinItem* item )
{
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  int typeNo = pokeMotionType;

  if(pButton->IsTrigger(gfl2::ui::BUTTON_RIGHT))
  {
    typeNo++;
    if(typeNo > 1) typeNo = 0;
    pokeMotionNo = 0;
    pokeMotionNoUsedIdx = 0;
  }
  if(pButton->IsTrigger(gfl2::ui::BUTTON_LEFT))
  {
    typeNo--;
    if(typeNo < 0) typeNo = 1;
    pokeMotionNo = 0;
    pokeMotionNoUsedIdx = 0;
  }

  pokeMotionType = (PokeMotionType)typeNo;
}

static wchar_t* DrawPokeMotionType( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls[%ls]",item->GetName(),POKE_MOTION_TYPE[pokeMotionType]);
  return workStr;
}

// モーション選択
static void UpdatePokeMotionNo( void* userWork, gfl2::debug::DebugWinItem* item )
{
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  // 選択
  if(pButton->IsRepeat(gfl2::ui::BUTTON_RIGHT))
  {
    switch(pokeMotionType)
    {
    case 0: // バトル
      if(pokeMotionNoUsedIdx < GFL_NELEMS(POKE_MOTION_BATTLE_USED)-1) pokeMotionNoUsedIdx++;
      pokeMotionNo = POKE_MOTION_BATTLE_USED[pokeMotionNoUsedIdx];
      break;
    case 1: // かわいがり
      if(pokeMotionNoUsedIdx < GFL_NELEMS(POKE_MOTION_KAWAIGARI_USED)-1) pokeMotionNoUsedIdx++;
      pokeMotionNo = POKE_MOTION_KAWAIGARI_USED[pokeMotionNoUsedIdx];
      break;
    case 2: // フィールド
      if(pokeMotionNo < PokeTool::MODEL_ANIME_FIELD_NUM-1) pokeMotionNo++;
      break;
    }
    
  }
  if(pButton->IsRepeat(gfl2::ui::BUTTON_LEFT))
  {
    switch(pokeMotionType)
    {
    case 0: // バトル
      if(pokeMotionNoUsedIdx > 0) pokeMotionNoUsedIdx--;
      pokeMotionNo = POKE_MOTION_BATTLE_USED[pokeMotionNoUsedIdx];
      break;
    case 1: // かわいがり
      if(pokeMotionNoUsedIdx > 0) pokeMotionNoUsedIdx--;
      pokeMotionNo = POKE_MOTION_KAWAIGARI_USED[pokeMotionNoUsedIdx];
      break;
    case 2: // フィールド
      if(pokeMotionNo > 0) pokeMotionNo--;
      break;
    }
  }

  // モーション再生
  if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    pMain->GetPokemon()->ChangeAnimation(pokeMotionType,(PokeTool::MODEL_ANIME)pokeMotionNo);
    pMain->GetPokemon()->DebugTransitionState(FinderStudioPokemon::POKE_STATE_PAUSE_MOTION);
  }
}

static wchar_t* DrawPokeMotionNo( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  switch(pokeMotionType)
  {
  case 0: // バトル
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls[%ls]",item->GetName(),POKE_MOTION_BATTLE[pokeMotionNo]);
    break;
  case 1: // かわいがり
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls[%ls]",item->GetName(),POKE_MOTION_KAWAIGARI[pokeMotionNo]);
    break;
  case 2: // フィールド
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls[%ls]",item->GetName(),POKE_MOTION_FIELD[pokeMotionNo]);
    break;
  }

  return workStr;
}

static wchar_t* DrawPokeNowMotionNo( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  switch(pMain->GetPokemon()->GetPokeMotionType())
  {
  case POKE_MOTION_TYPE_BATTLE: // バトル
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls[%ls]",item->GetName(),POKE_MOTION_BATTLE[pMain->GetPokemon()->GetNowAnmNo()]);
    break;

  case POKE_MOTION_TYPE_KAWAIGARI: // かわいがり
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls[%ls]",item->GetName(),POKE_MOTION_KAWAIGARI[pMain->GetPokemon()->GetNowAnmNo()]);
    break;
  }

  return workStr;
}

// ポケモンNo
static int monsNo100;
static int monsNo10;
static int monsNo1;
// フォルムNo
static int formNo;
// 性別
static int sex;

// リロード処理
static void PokeModelReload()
{
  int monsNo = monsNo100 * 100 + monsNo10 * 10 + monsNo1;
  if(monsNo <= 0) return;
  if(monsNo > MONSNO_END) return;

  PokeTool::SimpleParam param(static_cast<MonsNo>(monsNo),static_cast<pml::FormNo>(formNo),static_cast<pml::Sex>(sex),0,0,0);
  pMain->StartChangePokeModel(param);
}

// ポケモンモデル再ロード
static void UpdatePokeModelReload100( void* userWork, gfl2::debug::DebugWinItem* item )
{
  // 再ロード
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  
  // 選択
  if(pButton->IsRepeat(gfl2::ui::BUTTON_LEFT))
  {
    monsNo100--;
    if(monsNo100 < 0) monsNo100 = 9;
  }
  if(pButton->IsRepeat(gfl2::ui::BUTTON_RIGHT))
  {
    monsNo100++;
    if(monsNo100 > 9) monsNo100 = 0;
  }

  if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    PokeModelReload();
  }
}
static void UpdatePokeModelReload10( void* userWork, gfl2::debug::DebugWinItem* item )
{
  // 再ロード
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  
  // 選択
  if(pButton->IsRepeat(gfl2::ui::BUTTON_LEFT))
  {
    monsNo10--;
    if(monsNo10 < 0) monsNo10 = 9;
  }
  if(pButton->IsRepeat(gfl2::ui::BUTTON_RIGHT))
  {
    monsNo10++;
    if(monsNo10 > 9) monsNo10 = 0;
  }

  if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    PokeModelReload();
  }
}
static void UpdatePokeModelReload1( void* userWork, gfl2::debug::DebugWinItem* item )
{
  // 再ロード
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  
  // 選択
  if(pButton->IsRepeat(gfl2::ui::BUTTON_LEFT))
  {
    monsNo1--;
    if(monsNo1 < 0) monsNo1 = 9;
  }
  if(pButton->IsRepeat(gfl2::ui::BUTTON_RIGHT))
  {
    monsNo1++;
    if(monsNo1 > 9) monsNo1 = 0;
  }

  if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    PokeModelReload();
  }
}
static void UpdatePokeModelReloadForm( void* userWork, gfl2::debug::DebugWinItem* item )
{
  // 再ロード
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  
  // 選択
  if(pButton->IsTrigger(gfl2::ui::BUTTON_LEFT))
  {
    formNo--;
    if(formNo < 0) formNo = 99;
  }
  if(pButton->IsTrigger(gfl2::ui::BUTTON_RIGHT))
  {
    formNo++;
    if(formNo > 99) formNo = 0;
  }

  if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    PokeModelReload();
  }
}
static void UpdatePokeModelReloadSex(void* userWork,gfl2::debug::DebugWinItem* item)
{
  // 再ロード
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  // 選択
  if(pButton->IsTrigger(gfl2::ui::BUTTON_LEFT))
  {
    if(sex > 0) sex--;
  }
  if(pButton->IsTrigger(gfl2::ui::BUTTON_RIGHT))
  {
    if(sex < 1) sex++;
  }

  if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    PokeModelReload();
  }
}
static wchar_t* DrawPokeModelReload100( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf(workStr, gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls[%d]",item->GetName(),monsNo100);
  return workStr;
}
static wchar_t* DrawPokeModelReload10( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf(workStr, gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls[%d]",item->GetName(),monsNo10);
  return workStr;
}
static wchar_t* DrawPokeModelReload1( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf(workStr, gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls[%d]",item->GetName(),monsNo1);
  return workStr;
}
static wchar_t* DrawPokeModelReloadForm( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf(workStr, gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls[%d]",item->GetName(),formNo);
  return workStr;
}
static wchar_t* DrawPokeModelReloadSex(void* userWork,gfl2::debug::DebugWinItem* item)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf(workStr,gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls[%d]",item->GetName(),sex);
  return workStr;
}

// ポケモンモデル位置・角度初期化
static void UpdatePokeResetPositionRotate( void* userWork, gfl2::debug::DebugWinItem* item )
{
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    pMain->GetPokemon()->ResetPositionRotate();
  }
}

// 図鑑No何番から再生するか
u32 zukanNo = 1;

// ポケモンモーション自動再生
static void UpdatePokeAutoMotion( void* userWork, gfl2::debug::DebugWinItem* item )
{
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    pMain->GetPokemon()->StartDebugAutoMotion(zukanNo);
    isEnableWaitMotion = false;
  }
  else
  {
    if(pButton->IsRepeat(gfl2::ui::BUTTON_LEFT))
    {
      zukanNo--;
      if(zukanNo < 1) zukanNo = 1;
    }
    if(pButton->IsRepeat(gfl2::ui::BUTTON_RIGHT))
    {
      zukanNo++;
      if(zukanNo > MONSNO_END) zukanNo = MONSNO_END;
    }
  }
}
static wchar_t* DrawPokeAutoMotion( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf(workStr, gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls[%d]",item->GetName(),zukanNo);
  return workStr;
}
static void UpdatePokeAutoMotionEnd( void* userWork, gfl2::debug::DebugWinItem* item )
{
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    pMain->GetPokemon()->EndDebugAutoMotion();
    isEnableWaitMotion = true;
  }
}

// ポケモン表示・非表示切り替え
bool isPokeVisible = true;
// 実物大フラグ
bool isFullSize = false;
// バウンディングボックス表示・非表示切り替え
bool isVisibleBoundingBox = false;
bool isVisibleBoundingBoxUI = false;
// めり込み許容量
int collisionPermit = 0;
// 移動範囲
f32 moveRange = 250.0f;

// Waist無視
bool isIgnoreWaistXZ = true;

static void UpdatePMAABBMinX(void* userWork,gfl2::debug::DebugWinItem* item){}
static wchar_t* DrawPMAABBMinX(void* userWork,gfl2::debug::DebugWinItem* item)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf(workStr,gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls[%.3f]",item->GetName(),pMain->GetPokemon()->GetPokemonAABB()->GetData(pokeMotionType,pokeMotionNo).GetMin().x);
  return workStr;
}
static void UpdatePMAABBMaxX(void* userWork,gfl2::debug::DebugWinItem* item){}
static wchar_t* DrawPMAABBMaxX(void* userWork,gfl2::debug::DebugWinItem* item)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf(workStr,gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls[%.3f]",item->GetName(),pMain->GetPokemon()->GetPokemonAABB()->GetData(pokeMotionType,pokeMotionNo).GetMax().x);
  return workStr;
}
static void UpdatePMAABBMinY(void* userWork,gfl2::debug::DebugWinItem* item){}
static wchar_t* DrawPMAABBMinY(void* userWork,gfl2::debug::DebugWinItem* item)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf(workStr,gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls[%.3f]",item->GetName(),pMain->GetPokemon()->GetPokemonAABB()->GetData(pokeMotionType,pokeMotionNo).GetMin().y);
  return workStr;
}
static void UpdatePMAABBMaxY(void* userWork,gfl2::debug::DebugWinItem* item){}
static wchar_t* DrawPMAABBMaxY(void* userWork,gfl2::debug::DebugWinItem* item)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf(workStr,gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls[%.3f]",item->GetName(),pMain->GetPokemon()->GetPokemonAABB()->GetData(pokeMotionType,pokeMotionNo).GetMax().y);
  return workStr;
}
static void UpdatePMAABBMinZ(void* userWork,gfl2::debug::DebugWinItem* item){}
static wchar_t* DrawPMAABBMinZ(void* userWork,gfl2::debug::DebugWinItem* item)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf(workStr,gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls[%.3f]",item->GetName(),pMain->GetPokemon()->GetPokemonAABB()->GetData(pokeMotionType,pokeMotionNo).GetMin().z);
  return workStr;
}
static void UpdatePMAABBMaxZ(void* userWork,gfl2::debug::DebugWinItem* item){}
static wchar_t* DrawPMAABBMaxZ(void* userWork,gfl2::debug::DebugWinItem* item)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf(workStr,gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls[%.3f]",item->GetName(),pMain->GetPokemon()->GetPokemonAABB()->GetData(pokeMotionType,pokeMotionNo).GetMax().z);
  return workStr;
}

static wchar_t* DrawAdjustScale(void* userWork,gfl2::debug::DebugWinItem* item)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf(workStr,gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls[%.3f]",item->GetName(),pMain->GetPokemon()->DebugGetAdjustScale());
  return workStr;
}

static void UpdateAdjustHeight(void* userWork,gfl2::debug::DebugWinItem* item)
{
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  f32 adjustHeight = pMain->GetPokemon()->DebugGetAdjustHeight();
  bool isChange = false;

  if(pButton->IsRepeat(gfl2::ui::BUTTON_LEFT))
  {
    if(pButton->IsHold(gfl2::ui::BUTTON_L)) adjustHeight -= 0.1f;
    else adjustHeight -= 0.01f;
    isChange = true;
  }
  if(pButton->IsRepeat(gfl2::ui::BUTTON_RIGHT))
  {
    if(pButton->IsHold(gfl2::ui::BUTTON_L)) adjustHeight += 0.1f;
    else adjustHeight += 0.01f;
    isChange = true;
  }

  if(isChange) pMain->GetPokemon()->DebugSetAdjustHeight(adjustHeight);
}
static wchar_t* DrawAdjustHeight(void* userWork,gfl2::debug::DebugWinItem* item)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf(workStr,gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls[%.3f]",item->GetName(),pMain->GetPokemon()->DebugGetAdjustHeight());
  return workStr;
}

// ポケモンメニュー初期化
static void SetupDebugMenuPoke()
{
  gfl2::debug::DebugWinGroup* pPokeDebugWinGroup = gfl2::debug::DebugWin_AddGroup(pDebugHeap,L"ポケモン",pRootDebugWinGroup);

  // 以下、モデル
  {
    gfl2::debug::DebugWinGroup* pPokeModelDebugWinGroup = gfl2::debug::DebugWin_AddGroup(pDebugHeap,L"モデル",pPokeDebugWinGroup);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pPokeModelDebugWinGroup,pDebugHeap,L"ポケモンNo:百の位",NULL,UpdatePokeModelReload100,DrawPokeModelReload100);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pPokeModelDebugWinGroup,pDebugHeap,L"ポケモンNo:十の位",NULL,UpdatePokeModelReload10,DrawPokeModelReload10);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pPokeModelDebugWinGroup,pDebugHeap,L"ポケモンNo:一の位",NULL,UpdatePokeModelReload1,DrawPokeModelReload1);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pPokeModelDebugWinGroup,pDebugHeap,L"フォルムNo",NULL,UpdatePokeModelReloadForm,DrawPokeModelReloadForm);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pPokeModelDebugWinGroup,pDebugHeap,L"性別",NULL,UpdatePokeModelReloadSex,DrawPokeModelReloadSex);
    gfl2::debug::DebugWin_AddItemValueBool(pPokeModelDebugWinGroup,pDebugHeap,L"実物大モード",&isFullSize);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pPokeModelDebugWinGroup,pDebugHeap,L"AdjustHeight",NULL,UpdateAdjustHeight,DrawAdjustHeight);
    gfl2::debug::DebugWinFunc* pFunc = gfl2::debug::DebugWin_AddItemUpdateFunc(pPokeModelDebugWinGroup,pDebugHeap,L"AdjustScale",NULL,NULL,DrawAdjustScale);
    if(pFunc) pFunc->SetIsSelectSkip(true);
  }

  // 以下、モーション
  {
    gfl2::debug::DebugWinGroup* pPokeMotionDebugWinGroup = gfl2::debug::DebugWin_AddGroup(pDebugHeap,L"モーション",pPokeDebugWinGroup);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pPokeMotionDebugWinGroup,pDebugHeap,L"PokeMotionType",NULL,UpdatePokeMotionType,DrawPokeMotionType);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pPokeMotionDebugWinGroup,pDebugHeap,L"PokeMotion",NULL,UpdatePokeMotionNo,DrawPokeMotionNo);
    gfl2::debug::DebugWinFunc* pFunc = gfl2::debug::DebugWin_AddItemUpdateFunc(pPokeMotionDebugWinGroup,pDebugHeap,L"NowPokeMotion",NULL,NULL,DrawPokeNowMotionNo);
    if(pFunc) pFunc->SetIsSelectSkip(true);

    gfl2::debug::DebugWin_AddItemValueBool(pPokeMotionDebugWinGroup,pDebugHeap,L"自動モーション",&isEnableWaitMotion);
    gfl2::debug::DebugWin_AddItemValueBool(pPokeMotionDebugWinGroup,pDebugHeap,L"自動モーションデバッグ上書き",&isEnableDebugWaitMotion);

    // バウンディングボックス表示・非表示切り替え
    gfl2::debug::DebugWin_AddItemValueBool(pPokeMotionDebugWinGroup,pDebugHeap,L"バウンディングボックス表示",&isVisibleBoundingBox);
    gfl2::debug::DebugWin_AddItemValueBool(pPokeMotionDebugWinGroup,pDebugHeap,L"UI上バウンディングボックス表示",&isVisibleBoundingBoxUI);
    // AABB関連
    gfl2::debug::DebugWin_AddItemUpdateFunc(pPokeMotionDebugWinGroup,pDebugHeap,L"min.x",NULL,UpdatePMAABBMinX,DrawPMAABBMinX);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pPokeMotionDebugWinGroup,pDebugHeap,L"min.y",NULL,UpdatePMAABBMinY,DrawPMAABBMinY);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pPokeMotionDebugWinGroup,pDebugHeap,L"min.z",NULL,UpdatePMAABBMinZ,DrawPMAABBMinZ);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pPokeMotionDebugWinGroup,pDebugHeap,L"max.x",NULL,UpdatePMAABBMaxX,DrawPMAABBMaxX);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pPokeMotionDebugWinGroup,pDebugHeap,L"max.y",NULL,UpdatePMAABBMaxY,DrawPMAABBMaxY);
    gfl2::debug::DebugWin_AddItemUpdateFunc(pPokeMotionDebugWinGroup,pDebugHeap,L"max.z",NULL,UpdatePMAABBMaxZ,DrawPMAABBMaxZ);

    gfl2::debug::DebugWin_AddItemValueBool(pPokeMotionDebugWinGroup,pDebugHeap,L"WaistXZ無視",&isIgnoreWaistXZ);
    gfl2::debug::DebugWin_AddItemValueBool(pPokeMotionDebugWinGroup,pDebugHeap,L"モーションブレンド",&isEnableMotionBlend);

    //gfl2::debug::DebugWin_AddItemUpdateFunc(pPokeMotionDebugWinGroup,pDebugHeap,L"モーション自動再生スタート",NULL,UpdatePokeAutoMotion,DrawPokeAutoMotion);
    //gfl2::debug::DebugWin_AddItemUpdateFunc(pPokeMotionDebugWinGroup,pDebugHeap,L"モーション自動再生終了",NULL,UpdatePokeAutoMotionEnd,NULL);
  }

  // ポケモン位置・角度リセット
  gfl2::debug::DebugWin_AddItemUpdateFunc(pPokeDebugWinGroup,pDebugHeap,L"ポケモン位置・角度初期化",NULL,UpdatePokeResetPositionRotate,NULL);
  // 表示・非表示切り替え
  gfl2::debug::DebugWin_AddItemValueBool(pPokeDebugWinGroup,pDebugHeap,L"ポケモン表示",&isPokeVisible);
  // めりこみ許容量
  gfl2::debug::DebugWin_AddItemValueInt(pPokeDebugWinGroup,pDebugHeap,L"めり込み許容量",&collisionPermit,-999,999,2);
  // 移動範囲係数（大きいほど広い範囲を移動可能）
  gfl2::debug::DebugWin_AddItemValueFloat(pPokeDebugWinGroup,pDebugHeap,L"移動範囲係数",&moveRange,10.0f,500.0f,10.0f);
}



static const wchar_t* CHARA_MOTION[] =
{
  //L"ez_2_ashi_action01_lz_PCBIN",  // 1005
  //L"ez_2_ashi_action02_lz_PCBIN",
  //L"ez_2_ashi_action03_lz_PCBIN",
  //L"ez_2_eie_trainer01_lz_PCBIN",
  //L"ez_2_eie_trainer02_lz_PCBIN",
  //L"ez_2_eie_trainer03_lz_PCBIN",
  //L"ez_2_kabi_trainer01_lz_PCBIN",
  //L"ez_2_kabi_trainer02_lz_PCBIN",
  //L"ez_2_kabi_trainer03_lz_PCBIN",
  //L"ez_2_mew_trainer_lz_PCBIN", // 1014
  //L"ez_2_mv17_action01_lz_PCBIN",
  //L"ez_2_mv17_action02_lz_PCBIN",
  //L"ez_2_mv17_action03_lz_PCBIN",
  //L"ez_2_mv17_mirror_action01_lz_PCBIN",
  //L"ez_2_mv17_mirror_action02_lz_PCBIN",
  //L"ez_2_mv17_mirror_action03_lz_PCBIN",
  //L"ez_2_pika_action01_lz_PCBIN",
  //L"ez_2_pika_action02_lz_PCBIN",
  //L"ez_2_pika_action03_lz_PCBIN",
  //L"ez_2_pika_action04_lz_PCBIN",
  //L"ez_2_pika_mirror_action01_lz_PCBIN",
  //L"ez_2_pika_mirror_action02_lz_PCBIN",
  //L"ez_2_pika_mirror_action03_lz_PCBIN",
  //L"ez_2_pika_mirror_action04_lz_PCBIN",
  //L"ez_2_rai_action01_lz_PCBIN",
  //L"ez_2_rai_action02_lz_PCBIN",
  //L"ez_2_rai_action03_lz_PCBIN",
  //L"ez_2_rai_action04_lz_PCBIN",
  //L"ez_2_spika_action01_lz_PCBIN",
  //L"ez_2_spika_action02_lz_PCBIN",
  //L"ez_2_spika_action03_lz_PCBIN",
  //L"ez_2_spika_action04_lz_PCBIN",
  //L"ez_2_spika_mirror_action01_lz_PCBIN",
  //L"ez_2_spika_mirror_action02_lz_PCBIN",
  //L"ez_2_spika_mirror_action03_lz_PCBIN",
  //L"ez_2_spika_mirror_action04_lz_PCBIN",
  //L"ez_2_tiki_action01_lz_PCBIN",
  //L"ez_2_tiki_action02_lz_PCBIN",
  //L"ez_2_tiki_action03_lz_PCBIN",
  //L"ez_2_tiki_mirror_action03_lz_PCBIN",
  //L"ez_2_tora_action01_lz_PCBIN",
  //L"ez_2_tora_action02_lz_PCBIN",
  //L"ez_2_tora_action03_lz_PCBIN",
  //L"ez_2_tora_action04_lz_PCBIN",
  //L"ez_2_tora_mirror_action01_lz_PCBIN",
  //L"ez_2_tora_mirror_action02_lz_PCBIN",
  //L"ez_2_tora_mirror_action03_lz_PCBIN",
  //L"ez_2_tora_mirror_action04_lz_PCBIN",
  L"ノーマル",
  L"ほのお",
  L"みず",
  L"くさ",
  L"でんき",
  L"こおり",
  L"かくとう",
  L"どく",
  L"じめん",
  L"ひこう",
  L"エスパー",
  L"むし",
  L"いわ",
  L"ゴースト",
  L"ドラゴン",
  L"あく",
  L"はがね",
  L"フェアリー",
  L"ピカチュウ",
  L"あいさつ",
//  L"ez_cmn_ghost_wait_lz_PCBIN",
};
static u32 charaMotionNo = 4;

// モーション選択
static void UpdateCharaMotionNo( void* userWork, gfl2::debug::DebugWinItem* item )
{
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  // 選択
  if(pButton->IsRepeat(gfl2::ui::BUTTON_RIGHT))
  {
    if(charaMotionNo < GFL_NELEMS(CHARA_MOTION)-1) charaMotionNo++;
  }
  if(pButton->IsRepeat(gfl2::ui::BUTTON_LEFT))
  {
    if(charaMotionNo > 0) charaMotionNo--;
  }

  // 選択中のアニメーションを通達
  pMain->GetChara()->SetNowSelectAnmNo(charaMotionNo);

  // モーション再生
  if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    //pMain->GetChara()->changeAnimationZ(charaMotionNo);
  }
}

static wchar_t* DrawCharaMotionNo( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf(workStr, gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls[%ls]",item->GetName(),CHARA_MOTION[charaMotionNo]);

  return workStr;
}

//static bool isCharaVisible = true;

static int charaCollisionPermit = 0;

// キャラモーションメニュー初期化
static void SetupDebugMenuChara()
{
  gfl2::debug::DebugWinGroup* pCharaDebugWinGroup = gfl2::debug::DebugWin_AddGroup(pDebugHeap,L"キャラ",pRootDebugWinGroup);
  gfl2::debug::DebugWin_AddItemUpdateFunc(pCharaDebugWinGroup,pDebugHeap,L"CharaMotion",NULL,UpdateCharaMotionNo,DrawCharaMotionNo);
  //gfl2::debug::DebugWin_AddItemValueBool(pCharaDebugWinGroup,pDebugHeap,L"キャラ表示",&isCharaVisible);
  // めりこみ許容量
  gfl2::debug::DebugWin_AddItemValueInt(pCharaDebugWinGroup,pDebugHeap,L"めり込み許容量",&charaCollisionPermit,-999,999,2);
}

// 再ロードするBG
static int groundType = 1;
static int nearType = 1;
static int farType = 1;

static void UpdateBGReload(void* userWork, gfl2::debug::DebugWinItem* item)
{
  // 再ロード
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    pMain->GetBackGround()->StartReload(groundType,nearType,farType);
  }
}

static void UpdateBGOutPutStart(void* userWork,gfl2::debug::DebugWinItem* item)
{
  // 再ロード
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    pMain->GetBackGround()->StartOutputBgData();
  }
}

static void UpdateBGOutPutStop(void* userWork,gfl2::debug::DebugWinItem* item)
{
  // 再ロード
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    pMain->GetBackGround()->StopOutputBgData();
  }
}

// 再ロードするBGセット
static int BGSetIndex = 0;

static void UpdateBGReloadSet(void* userWork,gfl2::debug::DebugWinItem* item)
{
  // 再ロード
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    pMain->GetBackGround()->StartReload(BGSetIndex);
  }
}

// 各要素を表示するかどうか
bool isVisibleSkyBox = true;
bool isVisibleGround = true;
bool isVisibleBackNear = true;
bool isVisibleBackFar = true;

// 時間
int hour = 12;
int minute = 0;
int second = 0;
// 時間有効フラグ
bool isEnableTime = false;

// 背景メニュー初期化
static void SetupDebugBackGround()
{
  gfl2::debug::DebugWinGroup* pBGDebugWinGroup = gfl2::debug::DebugWin_AddGroup(pDebugHeap,L"背景",pRootDebugWinGroup);
  gfl2::debug::DebugWin_AddItemValueInt(pBGDebugWinGroup,pDebugHeap,L"背景セット",&BGSetIndex,0,BG_ID_NUM-1);
  gfl2::debug::DebugWin_AddItemUpdateFunc(pBGDebugWinGroup,pDebugHeap,L"再ロード",NULL,UpdateBGReloadSet,NULL);
  gfl2::debug::DebugWin_AddItemValueInt(pBGDebugWinGroup,pDebugHeap,L"地面",&groundType,1,btl::bg::BGSYS_NORMAL_GROUND_MAX-1);
  gfl2::debug::DebugWin_AddItemValueInt(pBGDebugWinGroup,pDebugHeap,L"近景",&nearType,1,btl::bg::BGSYS_NEAR_GROUND_MAX);
  gfl2::debug::DebugWin_AddItemValueInt(pBGDebugWinGroup,pDebugHeap,L"遠景",&farType,1,btl::bg::BGSYS_FAR_GROUND_MAX);
  gfl2::debug::DebugWin_AddItemUpdateFunc(pBGDebugWinGroup,pDebugHeap,L"再ロード",NULL,UpdateBGReload,NULL);
  gfl2::debug::DebugWin_AddItemValueBool(pBGDebugWinGroup,pDebugHeap,L"天球表示",&isVisibleSkyBox);
  gfl2::debug::DebugWin_AddItemValueBool(pBGDebugWinGroup,pDebugHeap,L"地面表示",&isVisibleGround);
  gfl2::debug::DebugWin_AddItemValueBool(pBGDebugWinGroup,pDebugHeap,L"近景表示",&isVisibleBackNear);
  gfl2::debug::DebugWin_AddItemValueBool(pBGDebugWinGroup,pDebugHeap,L"遠景表示",&isVisibleBackFar);
  gfl2::debug::DebugWin_AddItemValueInt(pBGDebugWinGroup,pDebugHeap,L"時間",&hour,0,23);
  gfl2::debug::DebugWin_AddItemValueInt(pBGDebugWinGroup,pDebugHeap,L"分",&minute,0,59);
  gfl2::debug::DebugWin_AddItemValueInt(pBGDebugWinGroup,pDebugHeap,L"秒",&second,0,59);
  gfl2::debug::DebugWin_AddItemValueBool(pBGDebugWinGroup,pDebugHeap,L"時間有効",&isEnableTime);
  gfl2::debug::DebugWin_AddItemUpdateFunc(pBGDebugWinGroup,pDebugHeap,L"背景データ出力開始",NULL,UpdateBGOutPutStart,NULL);
  gfl2::debug::DebugWin_AddItemUpdateFunc(pBGDebugWinGroup,pDebugHeap,L"背景データ出力停止",NULL,UpdateBGOutPutStop,NULL);
}

static bool isOnlyPoke = false;

// モード切替
static void UpdateOnlyPoke(void* userWork,gfl2::debug::DebugWinItem* item)
{
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if(pButton->IsTrigger(gfl2::ui::BUTTON_LEFT) || pButton->IsTrigger(gfl2::ui::BUTTON_RIGHT))
  {
    isOnlyPoke = !isOnlyPoke;
  }

  if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    if(!isOnlyPoke)
    {
      pMain->ChangeCaptureMode(CAPTURE_MODE_BOTH);
    }
    else
    {
      pMain->ChangeCaptureMode(CAPTURE_MODE_POKE);
    }
  }
}
static wchar_t* DrawOnlyPoke(void* userWork,gfl2::debug::DebugWinItem* item)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if(!isOnlyPoke)
  {
    swprintf(workStr,gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls[%ls]",item->GetName(),L"ツーショット");
  }
  else
  {
    swprintf(workStr,gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls[%ls]",item->GetName(),L"ポケモンのみ");
  }

  return workStr;
}

// カメラリセット
static void UpdateResetCamera(void* userWork, gfl2::debug::DebugWinItem* item)
{
  // 再ロード
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    pMain->GetCameraSystem()->ResetCamera();
  }
}

// ビューワリセット
static void UpdateResetViewer(void* userWork,gfl2::debug::DebugWinItem* item)
{
  // 再ロード
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    pMain->ResetViewer();
  }
}

// カメラ係数
f32 cameraFocusFactor = 0;
// カメラ係数有効フラグ
bool isEnableCameraFocusFactor = false;

// 現在の画角
float nowZoom;

// デバッグカメラパラメータ有効フラグ
bool isEnableDebugCamera = true;
// カメラ移動スピード
f32 cameraMoveSpeed = 3.0f;
// カメラ距離
f32 cameraLength = 880;
// カメラ高さ上限
f32 cameraHeightMax = 150;
// カメラ高さ下限
f32 cameraHeightMin = 11.0f;
// 画角上限・下限
f32 cameraFovMax = 30;
f32 cameraFovMin = 3;
// Near/Far値
int cameraNear = 92;
int cameraFar = 6000;

// ライト角度
int lightDegY = 0;
// デバッグライト使用
bool isUseDebugLight = false;

// デバッグライト使用フラグ取得
bool IsUseDebugLight()
{
  return isUseDebugLight;
}


// 構図距離変更
static void UpdateCompositionDistance(void* userWork,gfl2::debug::DebugWinItem* item)
{
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  int compositionDistance = pMain->GetPokemon()->DebugGetCompositionDistance();

  bool isChange = false;

  if(!pButton->IsHold(gfl2::ui::BUTTON_L))
  {
    if(pButton->IsRepeat(gfl2::ui::BUTTON_LEFT))
    {
      if(compositionDistance > 0) compositionDistance -= 1;
      isChange = true;
    }
    if(pButton->IsRepeat(gfl2::ui::BUTTON_RIGHT))
    {
      if(pMain->GetPokemon()->IsCanComposit(compositionDistance + 1))
      {
        compositionDistance += 1;
        isChange = true;
      }
    }
  }
  else
  {
    if(pButton->IsRepeat(gfl2::ui::BUTTON_LEFT))
    {
      compositionDistance -= 10;
      if(compositionDistance < 0) compositionDistance = 0;
      isChange = true;
    }
    if(pButton->IsRepeat(gfl2::ui::BUTTON_RIGHT))
    {
      u32 i;
      for(i = 1;i <= 10;i++)
      {
        if(!pMain->GetPokemon()->IsCanComposit(compositionDistance + i))
        {
          break;
        }
      }
      if(i-1 > 0)
      {
        compositionDistance += i-1;
        isChange = true;
      }
    }
  }

  if(isChange)
  {
    pMain->GetPokemon()->DebugSetCompositionDistance(compositionDistance);
    pMain->ResetViewer();
  }
}

static wchar_t* DrawCompositionDistance(void* userWork,gfl2::debug::DebugWinItem* item)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf(workStr,gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls[%d]",item->GetName(),pMain->GetPokemon()->DebugGetCompositionDistance());

  return workStr;
}

// 構図ズーム変更
static void UpdateCompositionFoV(void* userWork,gfl2::debug::DebugWinItem* item)
{
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  int compositionFoV = pMain->GetCameraSystem()->DebugGetCompositionFoV();

  bool isChange = false;

  if(!pButton->IsHold(gfl2::ui::BUTTON_L))
  {
    if(pButton->IsRepeat(gfl2::ui::BUTTON_LEFT))
    {
      if(compositionFoV > 3) compositionFoV -= 1;
      isChange = true;
    }
    if(pButton->IsRepeat(gfl2::ui::BUTTON_RIGHT))
    {
      if(compositionFoV < 30) compositionFoV += 1;
      isChange = true;
    }
  }
  else
  {
    if(pButton->IsRepeat(gfl2::ui::BUTTON_LEFT))
    {
      compositionFoV -= 10;
      if(compositionFoV < 3) compositionFoV = 3;
      isChange = true;
    }
    if(pButton->IsRepeat(gfl2::ui::BUTTON_RIGHT))
    {
      compositionFoV += 10;
      if(compositionFoV > 30) compositionFoV = 30;
      isChange = true;
    }
  }

  if(isChange)
  {
    pMain->GetCameraSystem()->DebugSetCompositionFoV(compositionFoV);
    pMain->ResetViewer();
  }
}

static wchar_t* DrawCompositionFoV(void* userWork,gfl2::debug::DebugWinItem* item)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf(workStr,gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls[%d]",item->GetName(),pMain->GetCameraSystem()->DebugGetCompositionFoV());

  return workStr;
}

// 構図１に変更
static void UpdateComposition1(void* userWork,gfl2::debug::DebugWinItem* item)
{
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    if(pMain->IsCanUseComposition(COMPOSITION_1))
    {
      pMain->ChangeComposition(COMPOSITION_1);
    }
  }
}
// 構図２に変更
static void UpdateComposition2(void* userWork,gfl2::debug::DebugWinItem* item)
{
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    if(pMain->IsCanUseComposition(COMPOSITION_2))
    {
      pMain->ChangeComposition(COMPOSITION_2);
    }
  }
}
// 即眠り
static void UpdateDebugSleep(void* userWork,gfl2::debug::DebugWinItem* item)
{
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    pMain->GetPokemon()->DebugForceSleep();
  }
}

// 再生可能か表示
static wchar_t* DrawPokeMotionEnable(void* userWork,gfl2::debug::DebugWinItem* item)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if(!pMain->GetPokemon()->DebugIsCanPlayMotionExist(pokeMotionType,static_cast<PokeTool::MODEL_ANIME>(pokeMotionNo)))
  {
    swprintf(workStr,gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls[× モーションデータ無し]",item->GetName());
  }
  else if(!pMain->GetPokemon()->DebugIsCanPlayMotionHit(pokeMotionType,static_cast<PokeTool::MODEL_ANIME>(pokeMotionNo)))
  {
    swprintf(workStr,gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls[× トレーナーにめり込んでいる]",item->GetName());
  }
  else if(!pMain->GetPokemon()->DebugIsCanPlayMotionDataTable(pokeMotionType,static_cast<PokeTool::MODEL_ANIME>(pokeMotionNo)))
  {
    swprintf(workStr,gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls[× モーションテーブル指定×]",item->GetName());
  }
  else
  {
    // 再生可能
    swprintf(workStr,gfl2::debug::DEBUGWIN_NAME_LEN,L"%ls[○]",item->GetName());
  }

  return workStr;
}

// 自動モーションオフ
static void UpdateAutoMotionOff(void* userWork,gfl2::debug::DebugWinItem* item)
{
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    isEnableDebugWaitMotion = true;
    isEnableWaitMotion = false;
  }
}

// 初期化
void InitializeDebug(gfl2::heap::HeapBase* pHeap,FinderStudioViewer* pFinderStudioMain)
{
  pDebugHeap = pHeap;
  pMain = pFinderStudioMain;
  pRootDebugWinGroup = gfl2::debug::DebugWin_AddGroup(pDebugHeap,L"FinderStudio",NULL);

  SetupDebugMenuPoke();
  SetupDebugMenuChara();

  gfl2::debug::DebugWin_AddItemUpdateFunc(pRootDebugWinGroup,pDebugHeap,L"ポケモンのみ",NULL,UpdateOnlyPoke,DrawOnlyPoke);

  SetupDebugBackGround();

  // カメラ項目
  gfl2::debug::DebugWinGroup* pCamaraDebugWinGroup = gfl2::debug::DebugWin_AddGroup(pDebugHeap,L"カメラ",pRootDebugWinGroup);
  gfl2::debug::DebugWin_AddItemValueFloat(pCamaraDebugWinGroup,pDebugHeap,L"現在の画角",&nowZoom,0,0,0);
  gfl2::debug::DebugWin_AddItemValueFloat(pCamaraDebugWinGroup,pDebugHeap,L"カメラ係数",&cameraFocusFactor,0,1.0f,0.1f);
  gfl2::debug::DebugWin_AddItemValueBool(pCamaraDebugWinGroup,pDebugHeap,L"カメラ係数有効",&isEnableCameraFocusFactor);
  gfl2::debug::DebugWin_AddItemUpdateFunc(pCamaraDebugWinGroup,pDebugHeap,L"カメラリセット",NULL,UpdateResetCamera,NULL);
  gfl2::debug::DebugWin_AddItemValueBool(pCamaraDebugWinGroup,pDebugHeap,L"デバッグカメラパラメータ有効",&isEnableDebugCamera);
  gfl2::debug::DebugWin_AddItemValueFloat(pCamaraDebugWinGroup,pDebugHeap,L"カメラ移動スピード",&cameraMoveSpeed,1.0f,10.0f,0.1f);
  gfl2::debug::DebugWin_AddItemValueFloat(pCamaraDebugWinGroup,pDebugHeap,L"カメラ距離",&cameraLength,1,5000,10);
  gfl2::debug::DebugWin_AddItemValueFloat(pCamaraDebugWinGroup,pDebugHeap,L"カメラ高さ上限",&cameraHeightMax,10,1000,10);
  gfl2::debug::DebugWin_AddItemValueFloat(pCamaraDebugWinGroup,pDebugHeap,L"カメラ高さ下限",&cameraHeightMin,0,100,1);
  gfl2::debug::DebugWin_AddItemValueFloat(pCamaraDebugWinGroup,pDebugHeap,L"カメラ画角上限",&cameraFovMax,10,100,1);
  gfl2::debug::DebugWin_AddItemValueFloat(pCamaraDebugWinGroup,pDebugHeap,L"カメラ画角下限",&cameraFovMin,1,30,1);
  gfl2::debug::DebugWin_AddItemValueInt(pCamaraDebugWinGroup,pDebugHeap,L"カメラNear値",&cameraNear,1,1000,1);
  gfl2::debug::DebugWin_AddItemValueInt(pCamaraDebugWinGroup,pDebugHeap,L"カメラFar値",&cameraFar,1000,100000,10);

  // ライト項目
  gfl2::debug::DebugWinGroup* pLightDebugWinGroup = gfl2::debug::DebugWin_AddGroup(pDebugHeap,L"ライト",pRootDebugWinGroup);
  gfl2::debug::DebugWin_AddItemValueBool(pLightDebugWinGroup,pDebugHeap,L"デバッグライト使用",&isUseDebugLight);
  gfl2::debug::DebugWin_AddItemValueInt(pLightDebugWinGroup,pDebugHeap,L"ライト角度",&lightDegY,0,350,10);

  gfl2::debug::DebugWin_AddItemUpdateFunc(pRootDebugWinGroup,pDebugHeap,L"ビューワ側リセット",NULL,UpdateResetViewer,NULL);

  // ポケモンデータテーブル作成用デバッグ
  gfl2::debug::DebugWinGroup* pPokemonDataTableDebug = gfl2::debug::DebugWin_AddGroup(pDebugHeap,L"ポケモンデータテーブル用",pRootDebugWinGroup);
  gfl2::debug::DebugWin_AddItemUpdateFunc(pPokemonDataTableDebug,pDebugHeap,L"ポケモンNo:百の位",NULL,UpdatePokeModelReload100,DrawPokeModelReload100);
  gfl2::debug::DebugWin_AddItemUpdateFunc(pPokemonDataTableDebug,pDebugHeap,L"ポケモンNo:十の位",NULL,UpdatePokeModelReload10,DrawPokeModelReload10);
  gfl2::debug::DebugWin_AddItemUpdateFunc(pPokemonDataTableDebug,pDebugHeap,L"ポケモンNo:一の位",NULL,UpdatePokeModelReload1,DrawPokeModelReload1);
  gfl2::debug::DebugWin_AddItemUpdateFunc(pPokemonDataTableDebug,pDebugHeap,L"フォルムNo",NULL,UpdatePokeModelReloadForm,DrawPokeModelReloadForm);
  gfl2::debug::DebugWin_AddItemUpdateFunc(pPokemonDataTableDebug,pDebugHeap,L"構図1セット",NULL,UpdateComposition1,NULL);
  gfl2::debug::DebugWin_AddItemUpdateFunc(pPokemonDataTableDebug,pDebugHeap,L"構図2セット",NULL,UpdateComposition2,NULL);
  gfl2::debug::DebugWin_AddItemUpdateFunc(pPokemonDataTableDebug,pDebugHeap,L"構図カメラ",NULL,UpdateCompositionFoV,DrawCompositionFoV);
  gfl2::debug::DebugWin_AddItemUpdateFunc(pPokemonDataTableDebug,pDebugHeap,L"構図距離",NULL,UpdateCompositionDistance,DrawCompositionDistance);
  gfl2::debug::DebugWin_AddItemUpdateFunc(pPokemonDataTableDebug,pDebugHeap,L"PokeMotionType",NULL,UpdatePokeMotionType,DrawPokeMotionType);
  gfl2::debug::DebugWin_AddItemUpdateFunc(pPokemonDataTableDebug,pDebugHeap,L"PokeMotion",NULL,UpdatePokeMotionNo,DrawPokeMotionNo);
  gfl2::debug::DebugWinFunc* pFunc = gfl2::debug::DebugWin_AddItemUpdateFunc(pPokemonDataTableDebug,pDebugHeap,L"再生",NULL,NULL,DrawPokeMotionEnable);
  if(pFunc) pFunc->SetIsSelectSkip(true);
  gfl2::debug::DebugWin_AddItemUpdateFunc(pPokemonDataTableDebug,pDebugHeap,L"即眠り",NULL,UpdateDebugSleep,NULL);
  gfl2::debug::DebugWin_AddItemValueBool(pPokemonDataTableDebug,pDebugHeap,L"バウンディングボックス表示",&isVisibleBoundingBox);
  gfl2::debug::DebugWin_AddItemUpdateFunc(pPokemonDataTableDebug,pDebugHeap,L"自動モーションオフ",NULL,UpdateAutoMotionOff,NULL);
}

// 更新
void UpdateDebug()
{
  if(isEnableDebugWaitMotion) pMain->GetPokemon()->SetEnableAutoMotion(isEnableWaitMotion);
  pMain->GetPokemon()->SetDebugIsEnableMotionBlend(isEnableMotionBlend);
  pMain->GetPokemon()->SetDebugIsVisibleBoundingBox(isVisibleBoundingBox);
  pMain->GetPokemon()->SetDebugIsVisibleBoundingBoxUI(isVisibleBoundingBoxUI);
  pMain->GetPokemon()->SetDebugIsFullSize(isFullSize);
  pMain->GetPokemon()->SetCollisionParmit(collisionPermit);
  pMain->GetPokemon()->SetDebugIsIgnoreWaistXZ(isIgnoreWaistXZ);
  pMain->GetPokemon()->SetMoveRange(moveRange);
  pMain->GetPokemon()->SetVisible(isPokeVisible);

  //pMain->GetChara()->SetVisible(isCharaVisible);
  pMain->GetChara()->SetCollisionPermit(charaCollisionPermit);

  if(isEnableCameraFocusFactor) pMain->GetCameraSystem()->SetFocusRate(cameraFocusFactor);
  pMain->GetCameraSystem()->EnableDebugParam(isEnableDebugCamera);
  pMain->GetCameraSystem()->DebugSetCameraMoveSpeed(cameraMoveSpeed);
  pMain->GetCameraSystem()->DebugSetCameraLength(cameraLength);
  pMain->GetCameraSystem()->DebugSetCameraHeightMax(cameraHeightMax);
  pMain->GetCameraSystem()->DebugSetCameraHeightMin(cameraHeightMin);
  pMain->GetCameraSystem()->DebugSetCameraFoVMax(cameraFovMax);
  pMain->GetCameraSystem()->DebugSetCameraFoVMin(cameraFovMin);
  pMain->GetCameraSystem()->DebugSetCameraNear(cameraNear);
  pMain->GetCameraSystem()->DebugSetCameraFar(cameraFar);

  nowZoom = pMain->GetCameraSystem()->DebugGetNowZoom();

  pMain->GetBackGround()->SetIsVisibleSkyBox(isVisibleSkyBox);
  pMain->GetBackGround()->SetIsVisibleGround(isVisibleGround);
  pMain->GetBackGround()->SetIsVisibleBackNear(isVisibleBackNear);
  pMain->GetBackGround()->SetIsVisibleBackFar(isVisibleBackFar);
  if(isEnableTime)
  {
    pMain->GetBackGround()->SetTime(hour,minute,second);
    TimeZone timeZone;
    if(hour > 6 && hour < 17) timeZone = TIME_ZONE_NOON;
    else if(hour >= 17 && hour < 18) timeZone = TIME_ZONE_EVENING;
    else timeZone = TIME_ZONE_NIGHT;
    // 時間の正確な確認のため削除
    //pMain->ChangeTimeZone(timeZone);
  }

  if(isUseDebugLight)
  {
    gfl2::math::Vector3 vec(0,-1,1);
    vec = vec.Normalize();
    FinderStudioCameraSystem::RotateYVec(&vec,lightDegY);
    pMain->GetRenderSystem()->SetLightDirection(vec);
  }
}

// 解放処理
void TerminateDebug()
{
   gfl2::debug::DebugWin_RemoveGroupByName(L"FinderStudio");
}


#endif

GFL_NAMESPACE_END(Viewer)
GFL_NAMESPACE_END(FinderStudio)
GFL_NAMESPACE_END(App)