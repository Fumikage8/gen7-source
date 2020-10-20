#if PM_DEBUG

//======================================================================
/**
 * @file    FinderStudioCaptureDebug.cpp
 * @date    2017/04/05 19:11:00
 * @author  fukushima_yuya
 * @brief   撮影アプリ用デバッグ機能
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureDebug.h"

#include "App/FinderStudioStatic/include/FinderStudioImageDBUtil.h"
#include "App/FinderStudioStatic/include/FinderStudioUtil.h"


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
/// ---[ グローバル変数宣言 begin ]---

/// AutoCapture
u32   DebugClass::m_AutoCapCaptureCount = 1;
u32   DebugClass::m_AutoCapStartMonsNo = MONSNO_NULL + 1;
u8    DebugClass::m_bAutoCapRandomMonsNoFlg = 0;
bool  DebugClass::m_bAutoCaptureFlg = false;
bool  DebugClass::m_bAutoCaptureStopFlg = false;
u32   DebugClass::m_AutoCapBackGroundID = 0;
u32   DebugClass::m_AutoCapBackGroundType = 0;
s32   DebugClass::m_AutoCapDressupHat = 0;
s32   DebugClass::m_AutoCapDressupHairAcce = -1;
s32   DebugClass::m_AutoCapDressupEyeAcce = 0;
s32   DebugClass::m_AutoCapDressupTops = 0;
s32   DebugClass::m_AutoCapDressupBottoms = 0;
s32   DebugClass::m_AutoCapDressupLeg = 0;
s32   DebugClass::m_AutoCapDressupShoes = 0;
s32   DebugClass::m_AutoCapDressupBag = 0;
bool  DebugClass::m_AutoCapDressupRandomFlg = false;
u8    DebugClass::m_AutoCapComposition = 2;
bool  DebugClass::m_AutoCapDecoRandomFlg = false;
bool  DebugClass::m_AutoCapDateRandomFlg = false;
bool  DebugClass::m_bAutoCaptureSaveFlg = true;
/// PhotoCompare
bool  DebugClass::m_bPhotoCompareFlg = false;
const Viewer::PhotoParam* DebugClass::m_pPhotoComparePhotoParam = NULL;
/// Aging
u16   DebugClass::m_AgingPokemonStartMonsNo = 1;
u16   DebugClass::m_AgingPokemonFinishMonsNo = 1;
u8    DebugClass::m_AgingPokemonMode = 0;
bool  DebugClass::m_bAgingPokemonFlg = false;
u8    DebugClass::m_AgingTrainerMode = 0;
bool  DebugClass::m_bAgingTrainerFlg = false;
u8    DebugClass::m_AgingBackGroundMode = 1;
u8    DebugClass::m_AgingBackGroundChange = 0;
u8    DebugClass::m_AgingBackGroundCamera = 0;
bool  DebugClass::m_bAgingBackGroundFlg = false;

/// ---[ グローバル変数宣言 end ]---
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
/// ---[ マクロ begin ]---

#if !defined( DEBUG_WIN_ADD_GROUP )
#define DEBUG_WIN_ADD_GROUP gfl2::debug::DebugWin_AddGroup
#endif

#if !defined( DEBUG_WIN_ADD_ITEM )
#define DEBUG_WIN_ADD_ITEM gfl2::debug::DebugWin_AddItemUpdateFunc
#endif

/// ---[ マクロ end ]---
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
/// ---[ DebugClass begin ]---

/**
 * @brief   コンストラクタ
 */
DebugClass::DebugClass( void )
  : m_pHeap( NULL )
  , m_pRootGroup( NULL )
{
  ;
}


/**
 * @brief   初期化
 */
void DebugClass::Initialize( gfl2::heap::HeapBase* heap )
{
  /// デバッグ用のヒープを確保
  m_pHeap = GFL_CREATE_LOCAL_HEAP_NAME( heap, 0x10000, gfl2::heap::HEAP_TYPE_EXP, false, "FSCaptureDebugHeap" );

  /// Rootグループの追加
  m_pRootGroup = DEBUG_WIN_ADD_GROUP( m_pHeap, L"UI用デバッグ機能", NULL );

  /// デバッグメニューの初期化
  {
    /// 自動撮影
    InitDebugMenu_AutoCapture();
    /// データの削除
    InitDebugMenu_DeleteData();
    /// 写真見比べ
    InitDebugMenu_PhotoCompare();
    /// エージング
    InitDebugMenu_Aging();
  }
}

/**
 * @brief   開放
 */
void DebugClass::Terminate( void )
{
  /// グループの解除
  gfl2::debug::DebugWin_RemoveGroup( m_pRootGroup );
  /// ヒープ削除
  GFL_DELETE_HEAP( m_pHeap );
}


/**
 * @brief   デバッグメニューの初期化：自動撮影
 */
void DebugClass::InitDebugMenu_AutoCapture( void )
{
  gfl2::debug::DebugWinGroup* autoCaptureGroup = DEBUG_WIN_ADD_GROUP( m_pHeap, L"自動撮影機能", m_pRootGroup );
  {
    DEBUG_WIN_ADD_ITEM( autoCaptureGroup, m_pHeap, L"撮影枚数", NULL, UpdateAutoCap_CaptureCount, DrawAutoCap_CaptureCount );

    DEBUG_WIN_ADD_ITEM( autoCaptureGroup, m_pHeap, L"開始：ポケモンID", NULL, UpdateAutoCap_StartMonsNo, DrawAutoCap_StartMonsNo );
    DEBUG_WIN_ADD_ITEM( autoCaptureGroup, m_pHeap, L"ポケモンランダム", NULL, UpdateAutoCap_RandomMonsNo, DrawAutoCap_RandomMonsNo );

    DEBUG_WIN_ADD_ITEM( autoCaptureGroup, m_pHeap, L"背景ID", NULL, UpdateAutoCap_BackGroundID, DrawAutoCap_BackGroundID );
    DEBUG_WIN_ADD_ITEM( autoCaptureGroup, m_pHeap, L"背景表示タイプ", NULL, UpdateAutoCap_BackGroundType, DrawAutoCap_BackGroundType );

    gfl2::debug::DebugWinGroup* dressupGroup = DEBUG_WIN_ADD_GROUP( m_pHeap, L"着せ替え", autoCaptureGroup );
    {
      DEBUG_WIN_ADD_ITEM( dressupGroup, m_pHeap, L"ハット", NULL, UpdateAutoCap_DressupHat, DrawAutoCap_DressupHat );
      DEBUG_WIN_ADD_ITEM( dressupGroup, m_pHeap, L"ヘアアクセ", NULL, UpdateAutoCap_DressupHairAcce, DrawAutoCap_DressupHairAcce );
      DEBUG_WIN_ADD_ITEM( dressupGroup, m_pHeap, L"アイアクセ", NULL, UpdateAutoCap_DressupEyeAcce, DrawAutoCap_DressupEyeAcce );
      DEBUG_WIN_ADD_ITEM( dressupGroup, m_pHeap, L"トップス", NULL, UpdateAutoCap_DressupTops, DrawAutoCap_DressupTops );
      DEBUG_WIN_ADD_ITEM( dressupGroup, m_pHeap, L"ボトムス", NULL, UpdateAutoCap_DressupBottoms, DrawAutoCap_DressupBottoms );
      DEBUG_WIN_ADD_ITEM( dressupGroup, m_pHeap, L"レッグ", NULL, UpdateAutoCap_DressupLeg, DrawAutoCap_DressupLeg );
      DEBUG_WIN_ADD_ITEM( dressupGroup, m_pHeap, L"シューズ", NULL, UpdateAutoCap_DressupShoes, DrawAutoCap_DressupShoes );
      DEBUG_WIN_ADD_ITEM( dressupGroup, m_pHeap, L"バッグ", NULL, UpdateAutoCap_DressupBag, DrawAutoCap_DressupBag );
      DEBUG_WIN_ADD_ITEM( dressupGroup, m_pHeap, L"リセット：フィールド", NULL, UpdateAutoCap_DressupReset, DrawAutoCap_DressupReset );
    }
    DEBUG_WIN_ADD_ITEM( autoCaptureGroup, m_pHeap, L"衣装ランダム", NULL, UpdateAutoCap_RandomDressup, DrawAutoCap_RandomDressup );

    DEBUG_WIN_ADD_ITEM( autoCaptureGroup, m_pHeap, L"構図", NULL, UpdateAutoCap_Composition, DrawAutoCap_Composition );

    DEBUG_WIN_ADD_ITEM( autoCaptureGroup, m_pHeap, L"デコランダム", NULL, UpdateAutoCap_RandomDeco, DrawAutoCap_RandomDeco );

    DEBUG_WIN_ADD_ITEM( autoCaptureGroup, m_pHeap, L"撮影日時ランダム", NULL, UpdateAutoCap_RandomDate, DrawAutoCap_RandomDate );

    DEBUG_WIN_ADD_ITEM( autoCaptureGroup, m_pHeap, L"写真保存", NULL, UpdateAutoCap_Save, DrawAutoCap_Save );
    DEBUG_WIN_ADD_ITEM( autoCaptureGroup, m_pHeap, L"開始", NULL, UpdateAutoCap_Start, DrawAutoCap_Start );
    DEBUG_WIN_ADD_ITEM( autoCaptureGroup, m_pHeap, L"停止", NULL, UpdateAutoCap_Stop, DrawAutoCap_Stop );
  }
}

/**
 * @brief   データの削除
 */
void DebugClass::InitDebugMenu_DeleteData( void )
{
  gfl2::debug::DebugWinGroup* deleteDataGroup = DEBUG_WIN_ADD_GROUP( m_pHeap, L"データ削除", m_pRootGroup );
  {
    DEBUG_WIN_ADD_ITEM( deleteDataGroup, m_pHeap, L"拡張セーブデータ全削除", NULL, UpdateDeleteData_AllExtSave, DrawDeleteData_AllExtSave );
    DEBUG_WIN_ADD_ITEM( deleteDataGroup, m_pHeap, L"ImageDB管理写真全削除", NULL, UpdateDeleteData_AllImageDBPhoto, DrawDeleteData_AllImageDBPhoto );
  }
}

/**
 * @brief   写真見比べ
 */
void DebugClass::InitDebugMenu_PhotoCompare( void )
{
  gfl2::debug::DebugWinGroup* photoCompGroup = DEBUG_WIN_ADD_GROUP( m_pHeap, L"写真見比べ機能", m_pRootGroup );
  {
    gfl2::debug::DebugWinGroup* guideGroup = DEBUG_WIN_ADD_GROUP( m_pHeap, L"操作説明", photoCompGroup );
    {
      DEBUG_WIN_ADD_ITEM( guideGroup, m_pHeap, L"Ｌ：写真表示切替(ON/OFF)", NULL, NULL, NULL );
      DEBUG_WIN_ADD_ITEM( guideGroup, m_pHeap, L"Ｒ：写真表示切替(ON/OFF)", NULL, NULL, NULL );
      DEBUG_WIN_ADD_ITEM( guideGroup, m_pHeap, L"Ｘ：写真見比べ機能を終了", NULL, NULL, NULL );
      DEBUG_WIN_ADD_ITEM( guideGroup, m_pHeap, L"Ｙ：写真見比べ機能を終了", NULL, NULL, NULL );
    }

    DEBUG_WIN_ADD_ITEM( photoCompGroup, m_pHeap, L"機能", NULL, UpdatePhotoCompare_StartFlg, DrawPhotoCompare_StartFlg );

    gfl2::debug::DebugWinGroup* photoParamGroup = DEBUG_WIN_ADD_GROUP( m_pHeap, L"写真データ", photoCompGroup, 64 );
    {
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"ポケモン：種族No", NULL, NULL, DrawPhotoCompare_PokeMonsNo );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"ポケモン：フォルム", NULL, NULL, DrawPhotoCompare_PokeFormNo );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"ポケモン：性別", NULL, NULL, DrawPhotoCompare_PokeSex );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"ポケモン：色違い", NULL, NULL, DrawPhotoCompare_PokeRare );

      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"着せ替え：性別", NULL, NULL, DrawPhotoCompare_DressupSex );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"着せ替え：体色", NULL, NULL, DrawPhotoCompare_DressupBodyColor );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"着せ替え：目色", NULL, NULL, DrawPhotoCompare_DressupEyeColor );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"着せ替え：髪色", NULL, NULL, DrawPhotoCompare_DressupHairColor );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"着せ替え：リップ色", NULL, NULL, DrawPhotoCompare_DressupLipColor );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"着せ替え：髪型", NULL, NULL, DrawPhotoCompare_DressupHairStyle );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"着せ替え：アイアクセ", NULL, NULL, DrawPhotoCompare_DressupEyeAcce );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"着せ替え：ヘアアクセ", NULL, NULL, DrawPhotoCompare_DressupHairAcce );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"着せ替え：バッグ", NULL, NULL, DrawPhotoCompare_DressupBag );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"着せ替え：ボトムス", NULL, NULL, DrawPhotoCompare_DressupBottoms );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"着せ替え：帽子", NULL, NULL, DrawPhotoCompare_DressupHat );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"着せ替え：レッグ", NULL, NULL, DrawPhotoCompare_DressupLegs );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"着せ替え：シューズ", NULL, NULL, DrawPhotoCompare_DressupShoes );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"着せ替え：トップス", NULL, NULL, DrawPhotoCompare_DressupTops );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"着せ替え：シチュ", NULL, NULL, DrawPhotoCompare_DressupSituation );

      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"背景：地面アニメフレーム", NULL, NULL, DrawPhotoCompare_BgGroundAnimFrame );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"背景：近景アニメフレーム", NULL, NULL, DrawPhotoCompare_BgNearAnimFrame );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"背景：遠景アニメフレーム", NULL, NULL, DrawPhotoCompare_BgFarAnimFrame );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"背景：頂点モデルアニメフレーム", NULL, NULL, DrawPhotoCompare_BgVertexModelAnimFrame );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"背景：戦闘背景時間アニメフレーム", NULL, NULL, DrawPhotoCompare_BgTimeAnimFrame );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"背景：天球(雲)アニメフレーム", NULL, NULL, DrawPhotoCompare_BgCloudAnimFrame );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"背景：天球(太陽)アニメフレーム", NULL, NULL, DrawPhotoCompare_BgSunAnimFrame );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"背景：ＩＤ", NULL, NULL, DrawPhotoCompare_BgID );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"背景：戦闘背景アニメ種", NULL, NULL, DrawPhotoCompare_BgAnimID );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"背景：天球(月)満ち欠け", NULL, NULL, DrawPhotoCompare_BgMoonPattern );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"背景：時間帯", NULL, NULL, DrawPhotoCompare_BgTimeZone );

      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"ポケモーション：種類", NULL, NULL, DrawPhotoCompare_PokeMotionType );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"ポケモーション：Ｎｏ", NULL, NULL, DrawPhotoCompare_PokeMotionNo );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"ポケモーション：フレーム", NULL, NULL, DrawPhotoCompare_PokeMotionFrame );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"ポケモーション：目パチ１", NULL, NULL, DrawPhotoCompare_PokeMotionBlink1 );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"ポケモーション：目パチ２", NULL, NULL, DrawPhotoCompare_PokeMotionBlink2 );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"ポケモーション：目パチ３", NULL, NULL, DrawPhotoCompare_PokeMotionBlink3 );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"ポケモーション：口アニメ１", NULL, NULL, DrawPhotoCompare_PokeMotionLipSync1 );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"ポケモーション：口アニメ２", NULL, NULL, DrawPhotoCompare_PokeMotionLipSync2 );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"ポケモーション：口アニメ３", NULL, NULL, DrawPhotoCompare_PokeMotionLipSync3 );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"ポケモーション：ループ１フレーム", NULL, NULL, DrawPhotoCompare_PokeMotionLoopAnimFrame1 );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"ポケモーション：ループ２フレーム", NULL, NULL, DrawPhotoCompare_PokeMotionLoopAnimFrame2 );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"ポケモーション：ループ３フレーム", NULL, NULL, DrawPhotoCompare_PokeMotionLoopAnimFrame3 );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"ポケモーション：ループ４フレーム", NULL, NULL, DrawPhotoCompare_PokeMotionLoopAnimFrame4 );

      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"人物モーション：種類", NULL, NULL, DrawPhotoCompare_TrainerMotionID );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"人物モーション：フレーム", NULL, NULL, DrawPhotoCompare_TrainerMotionFrame );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"人物モーション：目パチ", NULL, NULL, DrawPhotoCompare_TrainerMotionBlink );

      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"座標角度：ポケＸ", NULL, NULL, DrawPhotoCompare_PosDegPokePosX );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"座標角度：ポケＺ", NULL, NULL, DrawPhotoCompare_PosDegPokePosZ );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"座標角度：ポケ角度", NULL, NULL, DrawPhotoCompare_PosDegPokeDeg );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"座標角度：人物角度", NULL, NULL, DrawPhotoCompare_PosDegTrainerDeg );

      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"カメラ：高さ", NULL, NULL, DrawPhotoCompare_CameraHeight );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"カメラ：Ｙ軸角度", NULL, NULL, DrawPhotoCompare_CameraDegAxisY );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"カメラ：Ｘ軸角度", NULL, NULL, DrawPhotoCompare_CameraDegAxisX );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"カメラ：画角", NULL, NULL, DrawPhotoCompare_CameraAngle );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"カメラ：係数", NULL, NULL, DrawPhotoCompare_CameraCoefficient );

      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"その他：撮影モード", NULL, NULL, DrawPhotoCompare_OtherCaptureMode );
      DEBUG_WIN_ADD_ITEM( photoParamGroup, m_pHeap, L"その他：構図", NULL, NULL, DrawPhotoCompare_OtherComposition );
    }
  }
}

/**
 * @brief   エージング
 */
void DebugClass::InitDebugMenu_Aging( void )
{
  gfl2::debug::DebugWinGroup* agingGroup = DEBUG_WIN_ADD_GROUP( m_pHeap, L"エージング", m_pRootGroup );
  {
    /// エージング：ポケモン
    gfl2::debug::DebugWinGroup* pokemonAgingGroup = DEBUG_WIN_ADD_GROUP( m_pHeap, L"ポケモン", agingGroup );
    {
      DEBUG_WIN_ADD_ITEM( pokemonAgingGroup, m_pHeap, L"開始ポケモン", NULL, UpdateAgingPokemon_StartMonsNo, DrawAgingPokemon_StartMonsNo );
      DEBUG_WIN_ADD_ITEM( pokemonAgingGroup, m_pHeap, L"終了ポケモン", NULL, UpdateAgingPokemon_FinishMonsNo, DrawAgingPokemon_FinishMonsNo );
      DEBUG_WIN_ADD_ITEM( pokemonAgingGroup, m_pHeap, L"表示順", NULL, UpdateAgingPokemon_Mode, DrawAgingPokemon_Mode );
      DEBUG_WIN_ADD_ITEM( pokemonAgingGroup, m_pHeap, L"開始/停止", NULL, UpdateAgingPokemon_StartStop, DrawAgingPokemon_StartStop );
    }
    /// エージング：トレーナー
    gfl2::debug::DebugWinGroup* trainerAgingGroup = DEBUG_WIN_ADD_GROUP( m_pHeap, L"トレーナー", agingGroup );
    {
      DEBUG_WIN_ADD_ITEM( trainerAgingGroup, m_pHeap, L"チェック内容", NULL, UpdateAgingTrainer_Mode, DrawAgingTrainer_Mode );
      DEBUG_WIN_ADD_ITEM( trainerAgingGroup, m_pHeap, L"開始/停止", NULL, UpdateAgingTrainer_StartStop, DrawAgingTrainer_StartStop );
    }
    /// エージング：背景
    gfl2::debug::DebugWinGroup* backgroundAgingGroup = DEBUG_WIN_ADD_GROUP( m_pHeap, L"背景", agingGroup );
    {
      DEBUG_WIN_ADD_ITEM( backgroundAgingGroup, m_pHeap, L"モード", NULL, UpdateAgingBackGround_Mode, DrawAgingBackGround_Mode );
      DEBUG_WIN_ADD_ITEM( backgroundAgingGroup, m_pHeap, L"切替方法", NULL, UpdateAgingBackGround_Change, DrawAgingBackGround_Change );
      DEBUG_WIN_ADD_ITEM( backgroundAgingGroup, m_pHeap, L"カメラ操作", NULL, UpdateAgingBackGround_Camera, DrawAgingBackGround_Camera );
      DEBUG_WIN_ADD_ITEM( backgroundAgingGroup, m_pHeap, L"開始/停止", NULL, UpdateAgingBackGround_StartStop, DrawAgingBackGround_StartStop );
    }
  }
}






//////////////////////////////////////////////////////////////////////////
///
///   @brief    共通処理
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   デバッグウィンドウが開いているか
 *
 * @retval  "true"なら開いている
 * @retval  "false"ならそれ以外
 */
bool DebugClass::IsDebugWinOpen( void )
{
  return gfl2::debug::DebugWin_IsOpen();
}




//////////////////////////////////////////////////////////////////////////
///
///   @brief    自動撮影：撮影枚数
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   更新：撮影枚数
 */
void DebugClass::UpdateAutoCap_CaptureCount( void* userWork, gfl2::debug::DebugWinItem* item )
{
  /// キー入力の取得
  gfl2::ui::Button* button = _GetButton();

  u32 addCount = 1;

  if( button->IsHold( gfl2::ui::BUTTON_L ) && button->IsHold( gfl2::ui::BUTTON_R ) )
  {
    addCount = 100;
  }
  else if( button->IsHold( gfl2::ui::BUTTON_L ) || button->IsHold( gfl2::ui::BUTTON_R ) )
  {
    addCount = 10;
  }

  /// 増減
  if( button->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    if( m_AutoCapCaptureCount <= addCount )
    {
      m_AutoCapCaptureCount = 1;
    }
    else {
      m_AutoCapCaptureCount -= addCount;
    }
  }
  else if( button->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    if( m_AutoCapCaptureCount >= (Static::SAVE_PHOTO_MAX - addCount) )
    {
      m_AutoCapCaptureCount = 420;
    }
    else {
      m_AutoCapCaptureCount += addCount;
    }
  }
}

/**
 * @brief   描画：撮影枚数
 */
wchar_t* DebugClass::DrawAutoCap_CaptureCount( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_AutoCapCaptureCount );

  return workStr;
}



//////////////////////////////////////////////////////////////////////////
///
///   @brief    自動撮影：ポケモン
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   更新：開始ポケモンID
 */
void DebugClass::UpdateAutoCap_StartMonsNo( void* userWork, gfl2::debug::DebugWinItem* item )
{
  /// キー入力の取得
  gfl2::ui::Button* button = _GetButton();

  u32 addCount = 1;

  if( button->IsHold( gfl2::ui::BUTTON_L ) && button->IsHold( gfl2::ui::BUTTON_R ) )
  {
    addCount = 100;
  }
  else if( button->IsHold( gfl2::ui::BUTTON_L ) || button->IsHold( gfl2::ui::BUTTON_R ) )
  {
    addCount = 10;
  }

  /// 増減
  if( button->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    if( m_AutoCapStartMonsNo <= addCount )
    {
      m_AutoCapStartMonsNo = MONSNO_NULL + 1;
    }
    else {
      m_AutoCapStartMonsNo -= addCount;
    }
  }
  else if( button->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    if( m_AutoCapStartMonsNo >= ( MONSNO_END - addCount ) )
    {
      m_AutoCapStartMonsNo = MONSNO_END;
    }
    else {
      m_AutoCapStartMonsNo += addCount;
    }
  }
}

/**
 * @brief   描画：開始ポケモンID
 */
wchar_t* DebugClass::DrawAutoCap_StartMonsNo( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%03d]", item->GetName(), m_AutoCapStartMonsNo );

  return workStr;
}


/**
 * @brief   更新：ポケモンランダム
 */
void DebugClass::UpdateAutoCap_RandomMonsNo( void* userWork, gfl2::debug::DebugWinItem* item )
{
  /// キー入力の取得
  gfl2::ui::Button* button = _GetButton();

  /// ON
  if( button->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
  {
    m_bAutoCapRandomMonsNoFlg = 0;
  }
  /// OFF
  else if( button->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    m_bAutoCapRandomMonsNoFlg = 1;
  }
}

/**
 * @brief   描画：ポケモンランダム
 */
wchar_t* DebugClass::DrawAutoCap_RandomMonsNo( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_bAutoCapRandomMonsNoFlg )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[IDランダム]", item->GetName() );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ID順番]", item->GetName() );
  }

  return workStr;
}



//////////////////////////////////////////////////////////////////////////
///
///   @brief    自動撮影：背景
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   更新：背景ID
 */
void DebugClass::UpdateAutoCap_BackGroundID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  /// キー入力の取得
  gfl2::ui::Button* button = _GetButton();

  u32 addCount = 1;

  if( button->IsHold( gfl2::ui::BUTTON_L ) || button->IsHold( gfl2::ui::BUTTON_R ) )
  {
    addCount = 10;
  }

  /// 増減
  if( button->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    if( m_AutoCapBackGroundID <= addCount )
    {
      m_AutoCapBackGroundID = BG_ID_00;
    }
    else {
      m_AutoCapBackGroundID -= addCount;
    }
  }
  else if( button->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    if( m_AutoCapBackGroundID >= ( BG_ID_NUM - addCount ) )
    {
      m_AutoCapBackGroundID = BG_ID_NUM;
    }
    else {
      m_AutoCapBackGroundID += addCount;
    }
  }
}

/**
 * @brief   描画：背景ID
 */
wchar_t* DebugClass::DrawAutoCap_BackGroundID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_AutoCapBackGroundID );

  return workStr;
}


/**
 * @brief   更新：背景タイプ
 */
void DebugClass::UpdateAutoCap_BackGroundType( void* userWork, gfl2::debug::DebugWinItem* item )
{
  /// キー入力の取得
  gfl2::ui::Button* button = _GetButton();

  if( button->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
  {
    if( m_AutoCapBackGroundType > 0 )
    {
      m_AutoCapBackGroundType--;
    }
  }
  else if( button->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    if( m_AutoCapBackGroundType < 2 )
    {
      m_AutoCapBackGroundType++;
    }
  }
}

/**
 * @brief   描画：背景タイプ
 */
wchar_t* DebugClass::DrawAutoCap_BackGroundType( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_AutoCapBackGroundType == 0 )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[固定]", item->GetName() );
  }
  else if( m_AutoCapBackGroundType == 1 )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[順番]", item->GetName() );
  }
  else if( m_AutoCapBackGroundType == 2 )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ランダム]", item->GetName() );
  }

  return workStr;
}



//////////////////////////////////////////////////////////////////////////
///
///   @brief    自動撮影：着せ替え
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   更新：着せ替え[帽子]
 */
void DebugClass::UpdateAutoCap_DressupHat( void* userWork, gfl2::debug::DebugWinItem* item )
{
  if( _IsPlayerSexMale() )
  {
    _UpdateAutoCapDressup( &m_AutoCapDressupHat, DRESSUP_ITEM_ID_HERO_HAT_COUNT );
  }
  else {
    _UpdateAutoCapDressup( &m_AutoCapDressupHat, DRESSUP_ITEM_ID_HEROINE_HAT_COUNT );
  }
}

/**
 * @brief   描画：着せ替え[帽子]
 */
wchar_t* DebugClass::DrawAutoCap_DressupHat( void* userWork, gfl2::debug::DebugWinItem* item )
{
  return _DrawAutoCapDressup( item, m_AutoCapDressupHat );
}

/**
 * @brief   更新：着せ替え[ヘアアクセサリ]
 */
void DebugClass::UpdateAutoCap_DressupHairAcce( void* userWork, gfl2::debug::DebugWinItem* item )
{
  if( _IsPlayerSexMale() )
  {
    m_AutoCapDressupHairAcce = -1;
  }
  else {
    _UpdateAutoCapDressup( &m_AutoCapDressupHairAcce, DRESSUP_ITEM_ID_HEROINE_ACCHAIR_COUNT );
  }
}

/**
 * @brief   描画：着せ替え[ヘアアクセサリ]
 */
wchar_t* DebugClass::DrawAutoCap_DressupHairAcce( void* userWork, gfl2::debug::DebugWinItem* item )
{
  return _DrawAutoCapDressup( item, m_AutoCapDressupHairAcce );
}

/**
 * @brief   更新：着せ替え[アイアクセサリ]
 */
void DebugClass::UpdateAutoCap_DressupEyeAcce( void* userWork, gfl2::debug::DebugWinItem* item )
{
  if( _IsPlayerSexMale() )
  {
    _UpdateAutoCapDressup( &m_AutoCapDressupEyeAcce, DRESSUP_ITEM_ID_HERO_ACCEYE_COUNT );
  }
  else {
    _UpdateAutoCapDressup( &m_AutoCapDressupEyeAcce, DRESSUP_ITEM_ID_HEROINE_ACCEYE_COUNT );
  }
}

/**
 * @brief   描画：着せ替え[アイアクセサリ]
 */
wchar_t* DebugClass::DrawAutoCap_DressupEyeAcce( void* userWork, gfl2::debug::DebugWinItem* item )
{
  return _DrawAutoCapDressup( item, m_AutoCapDressupEyeAcce );
}

/**
* @brief   更新：着せ替え[トップス]
*/
void DebugClass::UpdateAutoCap_DressupTops( void* userWork, gfl2::debug::DebugWinItem* item )
{
  if( _IsPlayerSexMale() )
  {
    _UpdateAutoCapDressup( &m_AutoCapDressupTops, DRESSUP_ITEM_ID_HERO_TOPS_COUNT );
  }
  else {
    _UpdateAutoCapDressup( &m_AutoCapDressupTops, DRESSUP_ITEM_ID_HEROINE_TOPS_COUNT );
  }
}

/**
 * @brief   描画：着せ替え[トップス]
 */
wchar_t* DebugClass::DrawAutoCap_DressupTops( void* userWork, gfl2::debug::DebugWinItem* item )
{
  return _DrawAutoCapDressup( item, m_AutoCapDressupTops );
}

/**
 * @brief   更新：着せ替え[ボトムス]
 */
void DebugClass::UpdateAutoCap_DressupBottoms( void* userWork, gfl2::debug::DebugWinItem* item )
{
  if( _IsPlayerSexMale() )
  {
    _UpdateAutoCapDressup( &m_AutoCapDressupBottoms, DRESSUP_ITEM_ID_HERO_BOTTOMS_COUNT );
  }
  else {
    _UpdateAutoCapDressup( &m_AutoCapDressupBottoms, DRESSUP_ITEM_ID_HEROINE_BOTTOMS_COUNT );
  }
}

/**
 * @brief   描画：着せ替え[ボトムス]
 */
wchar_t* DebugClass::DrawAutoCap_DressupBottoms( void* userWork, gfl2::debug::DebugWinItem* item )
{
  return _DrawAutoCapDressup( item, m_AutoCapDressupBottoms );
}

/**
 * @brief   更新：着せ替え[レッグ]
 */
void DebugClass::UpdateAutoCap_DressupLeg( void* userWork, gfl2::debug::DebugWinItem* item )
{
  if( _IsPlayerSexMale() )
  {
    _UpdateAutoCapDressup( &m_AutoCapDressupLeg, DRESSUP_ITEM_ID_HERO_LEGS_COUNT );
  }
  else {
    _UpdateAutoCapDressup( &m_AutoCapDressupLeg, DRESSUP_ITEM_ID_HEROINE_LEGS_COUNT );
  }
}

/**
 * @brief   描画：着せ替え[レッグ]
 */
wchar_t* DebugClass::DrawAutoCap_DressupLeg( void* userWork, gfl2::debug::DebugWinItem* item )
{
  return _DrawAutoCapDressup( item, m_AutoCapDressupLeg );
}

/**
 * @brief   更新：着せ替え[シューズ]
 */
void DebugClass::UpdateAutoCap_DressupShoes( void* userWork, gfl2::debug::DebugWinItem* item )
{
  if( _IsPlayerSexMale() )
  {
    _UpdateAutoCapDressup( &m_AutoCapDressupShoes, DRESSUP_ITEM_ID_HERO_SHOES_COUNT );
  }
  else {
    _UpdateAutoCapDressup( &m_AutoCapDressupShoes, DRESSUP_ITEM_ID_HEROINE_SHOES_COUNT );
  }
}

/**
 * @brief   描画：着せ替え[シューズ]
 */
wchar_t* DebugClass::DrawAutoCap_DressupShoes( void* userWork, gfl2::debug::DebugWinItem* item )
{
  return _DrawAutoCapDressup( item, m_AutoCapDressupShoes );
}

/**
 * @brief   更新：着せ替え[バッグ]
 */
void DebugClass::UpdateAutoCap_DressupBag( void* userWork, gfl2::debug::DebugWinItem* item )
{
  if( _IsPlayerSexMale() )
  {
    _UpdateAutoCapDressup( &m_AutoCapDressupBag, DRESSUP_ITEM_ID_HERO_BAG_COUNT );
  }
  else {
    _UpdateAutoCapDressup( &m_AutoCapDressupBag, DRESSUP_ITEM_ID_HEROINE_BAG_COUNT );
  }
}

/**
 * @brief   描画：着せ替え[バッグ]
 */
wchar_t* DebugClass::DrawAutoCap_DressupBag( void* userWork, gfl2::debug::DebugWinItem* item )
{
  return _DrawAutoCapDressup( item, m_AutoCapDressupBag );
}

/**
 * @brief   更新：着せ替え[リセット：フィールド]
 */
void DebugClass::UpdateAutoCap_DressupReset( void* userWork, gfl2::debug::DebugWinItem* item )
{
  /// キー入力の取得
  gfl2::ui::Button* button = _GetButton();

  if( button->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    /// フィールド衣装を取得
    poke_3d::model::DressUpParam param = _GetFieldDressup();
    {
      m_AutoCapDressupHat       = param.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAT];
      m_AutoCapDressupHairAcce  = param.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCHAIR];
      m_AutoCapDressupEyeAcce   = param.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCEYE];
      m_AutoCapDressupTops      = param.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_TOPS];
      m_AutoCapDressupBottoms   = param.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BOTTOMS];
      m_AutoCapDressupLeg       = param.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_LEGS];
      m_AutoCapDressupShoes     = param.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_SHOES];
      m_AutoCapDressupBag       = param.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BAG];
    }
  }
}

/**
 * @brief   描画：着せ替え[リセット：フィールド]
 */
wchar_t* DebugClass::DrawAutoCap_DressupReset( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls", item->GetName() );

  return workStr;
}

/**
 * @brief   更新：衣装ランダム
 */
void DebugClass::UpdateAutoCap_RandomDressup( void* userWork, gfl2::debug::DebugWinItem* item )
{
  /// キー入力の取得
  gfl2::ui::Button* button = _GetButton();

  if( button->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
  {
    m_AutoCapDressupRandomFlg = false;
  }
  else if( button->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    m_AutoCapDressupRandomFlg = true;
  }
}

/**
 * @brief   描画：衣装ランダム
 */
wchar_t* DebugClass::DrawAutoCap_RandomDressup( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_AutoCapDressupRandomFlg )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ランダム]", item->GetName() );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[固定]", item->GetName() );
  }

  return workStr;
}



//////////////////////////////////////////////////////////////////////////
///
///   @brief    自動撮影：構図
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   更新：構図
 */
void DebugClass::UpdateAutoCap_Composition( void* userWork, gfl2::debug::DebugWinItem* item )
{
  /// キー入力の取得
  gfl2::ui::Button* button = _GetButton();

  if( button->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    if( m_AutoCapComposition > 0 )
    {
      m_AutoCapComposition--;
    }
  }
  else if( button->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    if( m_AutoCapComposition < 2 )
    {
      m_AutoCapComposition++;
    }
  }
}

/**
 * @brief   描画：構図
 */
wchar_t* DebugClass::DrawAutoCap_Composition( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_AutoCapComposition == 0 )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[構図１]", item->GetName() );
  }
  else if( m_AutoCapComposition == 1 )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[構図２]", item->GetName() );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ランダム]", item->GetName() );
  }

  return workStr;
}



//////////////////////////////////////////////////////////////////////////
///
///   @brief    自動撮影：デコ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   更新：デコのランダム
 */
void DebugClass::UpdateAutoCap_RandomDeco( void* userWork, gfl2::debug::DebugWinItem* item )
{
  /// キー入力の取得
  gfl2::ui::Button* button = _GetButton();

  if( button->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
  {
    m_AutoCapDecoRandomFlg = false;
  }
  else if( button->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    m_AutoCapDecoRandomFlg = true;
  }
}

/**
 * @brief   描画：デコのランダム
 */
wchar_t* DebugClass::DrawAutoCap_RandomDeco( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_AutoCapDecoRandomFlg )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ランダム]", item->GetName() );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[未編集]", item->GetName() );
  }

  return workStr;
}



//////////////////////////////////////////////////////////////////////////
///
///   @brief    自動撮影：撮影日時
///
//////////////////////////////////////////////////////////////////////////
/**
* @brief   更新：撮影日時のランダム
*/
void DebugClass::UpdateAutoCap_RandomDate( void* userWork, gfl2::debug::DebugWinItem* item )
{
  /// キー入力の取得
  gfl2::ui::Button* button = _GetButton();

  if( button->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
  {
    m_AutoCapDateRandomFlg = false;
  }
  else if( button->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    m_AutoCapDateRandomFlg = true;
  }
}

/**
* @brief   描画：撮影日時のランダム
*/
wchar_t* DebugClass::DrawAutoCap_RandomDate( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_AutoCapDateRandomFlg )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ランダム]", item->GetName() );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[現在]", item->GetName() );
  }

  return workStr;
}



//////////////////////////////////////////////////////////////////////////
///
///   @brief    自動撮影：保存
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   更新：保存
 */
void DebugClass::UpdateAutoCap_Save( void* userWork, gfl2::debug::DebugWinItem* item )
{
  /// キー入力の取得
  gfl2::ui::Button* button = _GetButton();

  /// ON
  if( button->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
  {
    m_bAutoCaptureSaveFlg = false;
  }
  if( button->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    m_bAutoCaptureSaveFlg = true;
  }
}

/**
 * @brief   描画：保存
 */
wchar_t* DebugClass::DrawAutoCap_Save( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_bAutoCaptureSaveFlg )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[する]", item->GetName() );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[しない]", item->GetName() );
  }

  return workStr;
}



//////////////////////////////////////////////////////////////////////////
///
///   @brief    自動撮影：開始・停止
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   更新：開始
 */
void DebugClass::UpdateAutoCap_Start( void* userWork, gfl2::debug::DebugWinItem* item )
{
  /// キー入力の取得
  gfl2::ui::Button* button = _GetButton();

  /// ON
  if( button->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    m_bAutoCaptureFlg = true;
  }
}

/**
 * @brief   描画：開始
 */
wchar_t* DebugClass::DrawAutoCap_Start( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls", item->GetName() );

  return workStr;
}


/**
 * @brief   更新：停止
 */
void DebugClass::UpdateAutoCap_Stop( void* userWork, gfl2::debug::DebugWinItem* item )
{
  /// キー入力の取得
  gfl2::ui::Button* button = _GetButton();

  /// ON
  if( button->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    m_bAutoCaptureStopFlg = true;
  }
}

/**
 * @brief   描画：停止
 */
wchar_t* DebugClass::DrawAutoCap_Stop( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls", item->GetName() );

  return workStr;
}





//////////////////////////////////////////////////////////////////////////
///
///   @brief    データ削除：拡張セーブデータ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   更新：拡張セーブデータ全削除
 */
void DebugClass::UpdateDeleteData_AllExtSave( void* userWork, gfl2::debug::DebugWinItem* item )
{
  /// キー入力の取得
  gfl2::ui::Button* button = _GetButton();

  if( button->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    _DeleteData_AllExtSave();
  }
}

/**
 * @brief   描画：拡張セーブデータ全削除
 */
wchar_t* DebugClass::DrawDeleteData_AllExtSave( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls", item->GetName() );

  return workStr;
}

/**
 * @brief   更新：ImageDB管理写真全削除
 */
void DebugClass::UpdateDeleteData_AllImageDBPhoto( void* userWork, gfl2::debug::DebugWinItem* item )
{
  /// キー入力の取得
  gfl2::ui::Button* button = _GetButton();

  if( button->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    _DeleteData_AllImageDBPhoto();
  }
}

/**
 * @brief   描画：ImageDB管理写真全削除
 */
wchar_t* DebugClass::DrawDeleteData_AllImageDBPhoto( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls", item->GetName() );

  return workStr;
}





//////////////////////////////////////////////////////////////////////////
///
///   @brief    写真見比べ：機能(ON/OFF)
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   更新：機能(ON/OFF)
 */
void DebugClass::UpdatePhotoCompare_StartFlg( void* userWork, gfl2::debug::DebugWinItem* item )
{
  /// キー入力の取得
  gfl2::ui::Button* button = _GetButton();

  if( button->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
  {
    m_bPhotoCompareFlg = false;
  }
  if( button->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    m_bPhotoCompareFlg = true;
  }
}

/**
 * @brief   描画：機能(ON/OFF)
 */
wchar_t* DebugClass::DrawPhotoCompare_StartFlg( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_bPhotoCompareFlg )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ON]", item->GetName() );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[OFF]", item->GetName() );
  }

  return workStr;
}





//////////////////////////////////////////////////////////////////////////
///
///   @brief    写真見比べ：写真データ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   描画：ポケモン「種族No」
 */
wchar_t* DebugClass::DrawPhotoCompare_PokeMonsNo( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->monsNo );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
 * @brief   描画：ポケモン「フォルムNo」
 */
wchar_t* DebugClass::DrawPhotoCompare_PokeFormNo( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->formNo );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
 * @brief   描画：ポケモン「性別」
 */
wchar_t* DebugClass::DrawPhotoCompare_PokeSex( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->sexPoke );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：ポケモン「色違い」
*/
wchar_t* DebugClass::DrawPhotoCompare_PokeRare( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->isRare );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：着せ替え「性別」
*/
wchar_t* DebugClass::DrawPhotoCompare_DressupSex( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->sexTrainer );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：着せ替え「体色」
*/
wchar_t* DebugClass::DrawPhotoCompare_DressupBodyColor( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->skinColor );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：着せ替え「目色」
*/
wchar_t* DebugClass::DrawPhotoCompare_DressupEyeColor( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->eyeColor );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：着せ替え「髪色」
*/
wchar_t* DebugClass::DrawPhotoCompare_DressupHairColor( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->hairColor );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：着せ替え「リップ色」
*/
wchar_t* DebugClass::DrawPhotoCompare_DressupLipColor( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->lipColor );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：着せ替え「髪型」
*/
wchar_t* DebugClass::DrawPhotoCompare_DressupHairStyle( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->hairStyle );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：着せ替え「アイアクセサリ」
*/
wchar_t* DebugClass::DrawPhotoCompare_DressupEyeAcce( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->eyeAccessory );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：着せ替え「ヘアアクセサリ」
*/
wchar_t* DebugClass::DrawPhotoCompare_DressupHairAcce( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->hairAccessory );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：着せ替え「バッグ」
*/
wchar_t* DebugClass::DrawPhotoCompare_DressupBag( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->bag );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：着せ替え「ボトムス」
*/
wchar_t* DebugClass::DrawPhotoCompare_DressupBottoms( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->bottoms );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：着せ替え「帽子」
*/
wchar_t* DebugClass::DrawPhotoCompare_DressupHat( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->hat );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：着せ替え「レッグ」
*/
wchar_t* DebugClass::DrawPhotoCompare_DressupLegs( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->leg );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：着せ替え「シューズ」
*/
wchar_t* DebugClass::DrawPhotoCompare_DressupShoes( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->shoes );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：着せ替え「トップス」
*/
wchar_t* DebugClass::DrawPhotoCompare_DressupTops( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->tops );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：着せ替え「シチュエーション」
*/
wchar_t* DebugClass::DrawPhotoCompare_DressupSituation( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->situ );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：背景「地面アニメフレーム」
*/
wchar_t* DebugClass::DrawPhotoCompare_BgGroundAnimFrame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->bgGroundLoopAnmFrame );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
 * @brief   描画：背景「近景アニメフレーム」
 */
wchar_t* DebugClass::DrawPhotoCompare_BgNearAnimFrame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->bgNearLoopAnmFrame );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：背景「遠景アニメフレーム」
*/
wchar_t* DebugClass::DrawPhotoCompare_BgFarAnimFrame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->bgFarLoopAnmFrame );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：背景「頂点モデルアニメフレーム」
*/
wchar_t* DebugClass::DrawPhotoCompare_BgVertexModelAnimFrame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->bgBuildLoopAnmFrame );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：背景「戦闘背景時間アニメフレーム」
*/
wchar_t* DebugClass::DrawPhotoCompare_BgTimeAnimFrame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->bgTimeAnmFrame );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：背景「天球(雲)アニメフレーム」
*/
wchar_t* DebugClass::DrawPhotoCompare_BgCloudAnimFrame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->skyCloudAnmFrame );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：背景「天球(太陽)アニメフレーム」
*/
wchar_t* DebugClass::DrawPhotoCompare_BgSunAnimFrame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->skySunAnmFrame );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：背景「ＩＤ」
*/
wchar_t* DebugClass::DrawPhotoCompare_BgID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->bgSetIndex );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：背景「戦闘背景アニメ種」
*/
wchar_t* DebugClass::DrawPhotoCompare_BgAnimID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->bgTimeAnmType );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：背景「天球(月)満ち欠け」
*/
wchar_t* DebugClass::DrawPhotoCompare_BgMoonPattern( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->skyMoonPattern );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：背景「時間帯」
*/
wchar_t* DebugClass::DrawPhotoCompare_BgTimeZone( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->timeZone );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：ポケモンモーション「種類」
*/
wchar_t* DebugClass::DrawPhotoCompare_PokeMotionType( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->pokeMotionType );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：ポケモンモーション「Ｎｏ」
*/
wchar_t* DebugClass::DrawPhotoCompare_PokeMotionNo( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->pokeMotionNo );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：ポケモンモーション「フレーム」
*/
wchar_t* DebugClass::DrawPhotoCompare_PokeMotionFrame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->pokeMainMotionFrame );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：ポケモンモーション「目パチ１」
*/
wchar_t* DebugClass::DrawPhotoCompare_PokeMotionBlink1( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->pokeEyeAnmPattern1 );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：ポケモンモーション「目パチ２」
*/
wchar_t* DebugClass::DrawPhotoCompare_PokeMotionBlink2( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->pokeEyeAnmPattern2 );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：ポケモンモーション「目パチ３」
*/
wchar_t* DebugClass::DrawPhotoCompare_PokeMotionBlink3( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->pokeEyeAnmPattern3 );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：ポケモンモーション「口アニメ１」
*/
wchar_t* DebugClass::DrawPhotoCompare_PokeMotionLipSync1( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->pokeMouthAnmpattern1 );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：ポケモンモーション「口アニメ２」
*/
wchar_t* DebugClass::DrawPhotoCompare_PokeMotionLipSync2( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->pokeMouthAnmpattern2 );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：ポケモンモーション「口アニメ３」
*/
wchar_t* DebugClass::DrawPhotoCompare_PokeMotionLipSync3( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->pokeMouthAnmpattern3 );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：ポケモンモーション「ループ１フレーム」
*/
wchar_t* DebugClass::DrawPhotoCompare_PokeMotionLoopAnimFrame1( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->pokeLoopAnmFrame1 );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：ポケモンモーション「ループ２フレーム」
*/
wchar_t* DebugClass::DrawPhotoCompare_PokeMotionLoopAnimFrame2( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->pokeLoopAnmFrame2 );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：ポケモンモーション「ループ３フレーム」
*/
wchar_t* DebugClass::DrawPhotoCompare_PokeMotionLoopAnimFrame3( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->pokeLoopAnmFrame3 );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：ポケモンモーション「ループ４フレーム」
*/
wchar_t* DebugClass::DrawPhotoCompare_PokeMotionLoopAnimFrame4( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->pokeLoopAnmFrame4 );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：トレーナーモーション「種類」
*/
wchar_t* DebugClass::DrawPhotoCompare_TrainerMotionID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->trainerMotionType );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：トレーナーモーション「フレーム」
*/
wchar_t* DebugClass::DrawPhotoCompare_TrainerMotionFrame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->trainerMotionFrame );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：トレーナーモーション「目パチ」
*/
wchar_t* DebugClass::DrawPhotoCompare_TrainerMotionBlink( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->trainerEyeAnmPattern );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：座標角度「ポケモンＸ座標」
*/
wchar_t* DebugClass::DrawPhotoCompare_PosDegPokePosX( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->pokePosX );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：座標角度「ポケモンＺ座標」
*/
wchar_t* DebugClass::DrawPhotoCompare_PosDegPokePosZ( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->pokePosZ );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：座標角度「ポケモン回転角度」
*/
wchar_t* DebugClass::DrawPhotoCompare_PosDegPokeDeg( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->pokeDegY );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   描画：座標角度「トレーナー回転角度」
*/
wchar_t* DebugClass::DrawPhotoCompare_PosDegTrainerDeg( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->trainerDegY );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
 * @brief   描画：カメラ「高さ」
 */
wchar_t* DebugClass::DrawPhotoCompare_CameraHeight( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->cameraHeight );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
 * @brief   描画：カメラ「Ｙ軸回転」
 */
wchar_t* DebugClass::DrawPhotoCompare_CameraDegAxisY( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->cameraDegY );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
 * @brief   描画：カメラ「Ｘ軸回転」
 */
wchar_t* DebugClass::DrawPhotoCompare_CameraDegAxisX( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->cameraDegX );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
 * @brief   描画：カメラ「画角」
 */
wchar_t* DebugClass::DrawPhotoCompare_CameraAngle( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->cameraFov );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
 * @brief   描画：カメラ「係数」
 */
wchar_t* DebugClass::DrawPhotoCompare_CameraCoefficient( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->cameraFocusFactor );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
 * @brief   描画：その他「撮影モード」
 */
wchar_t* DebugClass::DrawPhotoCompare_OtherCaptureMode( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->captureMode );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}

/**
 * @brief   描画：その他「構図」
 */
wchar_t* DebugClass::DrawPhotoCompare_OtherComposition( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_pPhotoComparePhotoParam )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_pPhotoComparePhotoParam->composition );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[NULL]", item->GetName() );
  }

  return workStr;
}




//////////////////////////////////////////////////////////////////////////
///
///   @brief    エージング：ポケモン
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   更新：開始ポケモン
 */
void DebugClass::UpdateAgingPokemon_StartMonsNo( void* userWork, gfl2::debug::DebugWinItem* item )
{
  /// キー入力の取得
  gfl2::ui::Button* button = _GetButton();

  u32 addCount = 1;

  if( button->IsHold( gfl2::ui::BUTTON_L ) && button->IsHold( gfl2::ui::BUTTON_R ) )
  {
    addCount = 100;
  }
  else if( button->IsHold( gfl2::ui::BUTTON_L ) || button->IsHold( gfl2::ui::BUTTON_R ) )
  {
    addCount = 10;
  }

  /// 増減
  if( button->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    if( m_AgingPokemonStartMonsNo <= addCount )
    {
      m_AgingPokemonStartMonsNo = MONSNO_NULL + 1;
    }
    else {
      m_AgingPokemonStartMonsNo -= addCount;
    }
  }
  else if( button->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    if( m_AgingPokemonStartMonsNo >= ( m_AgingPokemonFinishMonsNo - addCount ) )
    {
      m_AgingPokemonStartMonsNo = m_AgingPokemonFinishMonsNo - 1;
    }
    else {
      m_AgingPokemonStartMonsNo += addCount;
    }
  }
}

/**
 * @brief   描画：開始ポケモン
 */
wchar_t* DebugClass::DrawAgingPokemon_StartMonsNo( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_AgingPokemonStartMonsNo );

  return workStr;
}

/**
 * @brief   更新：終了ポケモン
 */
void DebugClass::UpdateAgingPokemon_FinishMonsNo( void* userWork, gfl2::debug::DebugWinItem* item )
{
  /// キー入力の取得
  gfl2::ui::Button* button = _GetButton();

  u32 addCount = 1;

  if( button->IsHold( gfl2::ui::BUTTON_L ) && button->IsHold( gfl2::ui::BUTTON_R ) )
  {
    addCount = 100;
  }
  else if( button->IsHold( gfl2::ui::BUTTON_L ) || button->IsHold( gfl2::ui::BUTTON_R ) )
  {
    addCount = 10;
  }

  /// 増減
  if( button->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    if( m_AgingPokemonFinishMonsNo <= m_AgingPokemonStartMonsNo || m_AgingPokemonFinishMonsNo <= addCount )
    {
      m_AgingPokemonFinishMonsNo = m_AgingPokemonStartMonsNo;
    }
    else {
      m_AgingPokemonFinishMonsNo -= addCount;
    }
  }
  else if( button->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    if( m_AgingPokemonFinishMonsNo >= ( MONSNO_END - addCount ) )
    {
      m_AgingPokemonFinishMonsNo = MONSNO_END - 1;
    }
    else {
      m_AgingPokemonFinishMonsNo += addCount;
    }
  }
}

/**
 * @brief   描画：終了ポケモン
 */
wchar_t* DebugClass::DrawAgingPokemon_FinishMonsNo( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), m_AgingPokemonFinishMonsNo );

  return workStr;
}

/**
 * @brief   更新：モード
 */
void DebugClass::UpdateAgingPokemon_Mode( void* userWork, gfl2::debug::DebugWinItem* item )
{
  /// キー入力の取得
  gfl2::ui::Button* button = _GetButton();

  if( button->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
  {
    if( m_AgingPokemonMode > 0 )
    {
      m_AgingPokemonMode--;
    }
  }
  if( button->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    if( m_AgingPokemonMode < 1 )
    {
      m_AgingPokemonMode++;
    }
  }
}

/**
* @brief   描画：モード
*/
wchar_t* DebugClass::DrawAgingPokemon_Mode( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_AgingPokemonMode == 0 )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[順番]", item->GetName() );
  }
  else if( m_AgingPokemonMode == 1 )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ランダム]", item->GetName() );
  }

  return workStr;
}

/**
* @brief   更新：開始/停止
*/
void DebugClass::UpdateAgingPokemon_StartStop( void* userWork, gfl2::debug::DebugWinItem* item )
{
  /// キー入力の取得
  gfl2::ui::Button* button = _GetButton();

  if( button->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    m_bAgingPokemonFlg = !m_bAgingPokemonFlg;
    m_bAgingTrainerFlg = false;
    m_bAgingBackGroundFlg = false;
  }
}

/**
* @brief   描画：開始/停止
*/
wchar_t* DebugClass::DrawAgingPokemon_StartStop( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( !m_bAgingPokemonFlg )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[開始]", item->GetName() );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[停止]", item->GetName() );
  }

  return workStr;
}




//////////////////////////////////////////////////////////////////////////
///
///   @brief    エージング：トレーナー
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   更新：モード
 */
void DebugClass::UpdateAgingTrainer_Mode( void* userWork, gfl2::debug::DebugWinItem* item )
{
  /// キー入力の取得
  gfl2::ui::Button* button = _GetButton();

  if( button->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
  {
    if( m_AgingTrainerMode > 0 )
    {
      m_AgingTrainerMode--;
    }
  }
  if( button->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    if( m_AgingTrainerMode < 2 )
    {
      m_AgingTrainerMode++;
    }
  }
}

/**
 * @brief   描画：モード
 */
wchar_t* DebugClass::DrawAgingTrainer_Mode( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_AgingTrainerMode == 0 )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[男女両方]", item->GetName() );
  }
  else if( m_AgingTrainerMode == 1 )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[男の子のみ]", item->GetName() );
  }
  else if( m_AgingTrainerMode == 2 )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[女の子のみ]", item->GetName() );
  }

  return workStr;
}

/**
 * @brief   更新：開始/停止
 */
void DebugClass::UpdateAgingTrainer_StartStop( void* userWork, gfl2::debug::DebugWinItem* item )
{
  /// キー入力の取得
  gfl2::ui::Button* button = _GetButton();

  if( button->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    m_bAgingTrainerFlg = !m_bAgingTrainerFlg;
    m_bAgingPokemonFlg = false;
    m_bAgingBackGroundFlg = false;
  }
}

/**
 * @brief   描画：開始/停止
 */
wchar_t* DebugClass::DrawAgingTrainer_StartStop( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( !m_bAgingTrainerFlg )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[開始]", item->GetName() );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[停止]", item->GetName() );
  }

  return workStr;
}




//////////////////////////////////////////////////////////////////////////
///
///   @brief    エージング：背景
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   更新：モード
 */
void DebugClass::UpdateAgingBackGround_Mode( void* userWork, gfl2::debug::DebugWinItem* item )
{
  /// キー入力の取得
  gfl2::ui::Button* button = _GetButton();

  if( button->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
  {
    if( m_AgingBackGroundMode > 0 )
    {
      m_AgingBackGroundMode--;
    }
  }
  if( button->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    if( m_AgingBackGroundMode < 1 )
    {
      m_AgingBackGroundMode++;
    }
  }
}

/**
 * @brief   描画：モード
 */
wchar_t* DebugClass::DrawAgingBackGround_Mode( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_AgingBackGroundMode == 0 )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[順番]", item->GetName() );
  }
  else if( m_AgingBackGroundMode == 1 )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ランダム]", item->GetName() );
  }

  return workStr;
}

/**
 * @brief   更新：切替方法
 */
void DebugClass::UpdateAgingBackGround_Change( void* userWork, gfl2::debug::DebugWinItem* item )
{
  /// キー入力の取得
  gfl2::ui::Button* button = _GetButton();

  if( button->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
  {
    if( m_AgingBackGroundChange > 0 )
    {
      m_AgingBackGroundChange--;
    }
  }
  if( button->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    if( m_AgingBackGroundChange < 1 )
    {
      m_AgingBackGroundChange++;
    }
  }
}

/**
 * @brief   描画：切替方法
 */
wchar_t* DebugClass::DrawAgingBackGround_Change( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_AgingBackGroundChange == 0 )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[自動]", item->GetName() );
  }
  else if( m_AgingBackGroundChange == 1 )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[手動]", item->GetName() );
  }

  return workStr;
}

/**
 * @brief   更新：カメラ
 */
void DebugClass::UpdateAgingBackGround_Camera( void* userWork, gfl2::debug::DebugWinItem* item )
{
  /// キー入力の取得
  gfl2::ui::Button* button = _GetButton();

  if( button->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
  {
    if( m_AgingBackGroundCamera > 0 )
    {
      m_AgingBackGroundCamera--;
    }
  }
  if( button->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    if( m_AgingBackGroundCamera < 2 )
    {
      m_AgingBackGroundCamera++;
    }
  }
}

/**
 * @brief   描画：カメラ
 */
wchar_t* DebugClass::DrawAgingBackGround_Camera( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( m_AgingBackGroundCamera == 0 )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[固定]", item->GetName() );
  }
  else if( m_AgingBackGroundCamera == 1 )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[手動]", item->GetName() );
  }
  else if( m_AgingBackGroundCamera == 2 )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[自動回転(Y)]", item->GetName() );
  }

  return workStr;
}
/**
 * @brief   更新：開始/停止
 */
void DebugClass::UpdateAgingBackGround_StartStop( void* userWork, gfl2::debug::DebugWinItem* item )
{
  /// キー入力の取得
  gfl2::ui::Button* button = _GetButton();

  if( button->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    m_bAgingBackGroundFlg = !m_bAgingBackGroundFlg;
    m_bAgingPokemonFlg = false;
    m_bAgingTrainerFlg = false;
  }
}

/**
 * @brief   描画：開始/停止
 */
wchar_t* DebugClass::DrawAgingBackGround_StartStop( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( !m_bAgingBackGroundFlg )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[開始]", item->GetName() );
  }
  else {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[停止]", item->GetName() );
  }

  return workStr;
}





//////////////////////////////////////////////////////////////////////////
///
///   @brief    拡張セーブデータの全削除
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   拡張セーブデータの全削除
 */
void DebugClass::_DeleteData_AllExtSave( void )
{
  gfl2::heap::HeapBase* heap = Static::ImageDBUtil::GetImageDBHeap();

  Static::FinderStudioExtSaveDataAccessor* extSave = GFL_NEW( heap ) Static::FinderStudioExtSaveDataAccessor( heap );

  extSave->DeleteDataFile();

  GFL_SAFE_DELETE( extSave );
}




//////////////////////////////////////////////////////////////////////////
///
///   @brief    ImageDB管理写真の全削除
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ImageDB管理写真の全削除
 */
void DebugClass::_DeleteData_AllImageDBPhoto( void )
{
#if defined(GF_PLATFORM_CTR)
  gfl2::heap::HeapBase* heap = Static::ImageDBUtil::GetImageDBHeap();

  /// ImageDB内の検索
  Static::ImageDBUtil::SearchPhotoData( heap->GetLowerHandle(), heap->GetLowerHandle() );
  {
    // 検索した写真の数
    u32 photoCount = Static::ImageDBUtil::GetPictureInfoCount();

    /// ImageDBUtilの生成
    Static::ImageDBUtil* imgdbUtil = GFL_NEW( heap ) Static::ImageDBUtil();
    {
      for( u32 i = 0; i < photoCount; ++i )
      {
        wchar_t* filepath = Static::ImageDBUtil::GetImageDBPictureInfo( i ).m_path;

        imgdbUtil->DeleteImage( heap, filepath );
        GFL_PRINT( "DeleteImage : index=[%d] filepath=[%ls]\n", i, filepath );
      }
    }
    GFL_SAFE_DELETE( imgdbUtil );
  }
  /// ImageDBの検索データを削除
  Static::ImageDBUtil::DeletePictureInfo();
#endif
}




//////////////////////////////////////////////////////////////////////////
///
///   @brief    便利関数
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   キー入力の取得
 *
 * @retval  gfl2::ui::Buttonクラス
 */
gfl2::ui::Button* DebugClass::_GetButton( void )
{
  return GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
}

/**
 * @brief   Savedata::MyStatusの取得
 *
 * @retval  Savedata::MyStatus
 */
Savedata::MyStatus* DebugClass::_GetMyStatus( void )
{
  /// Savedata::MyStatusの取得
  return GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetPlayerStatus();
}


/**
 * @brief   フィールド衣装の取得
 *
 * @retval  フィールド衣装
 */
poke_3d::model::DressUpParam DebugClass::_GetFieldDressup( void )
{
  /// Savedata::MyStatusの取得
  Savedata::MyStatus* myStatus = _GetMyStatus();

  return myStatus->GetDressup();
}

/**
 * @brief   プレイヤーの性別
 *
 * @retval  "true"なら男
 * @retval  "false"なら女
 */
bool DebugClass::_IsPlayerSexMale( void )
{
  /// Savedata::MyStatusの取得
  Savedata::MyStatus* myStatus = _GetMyStatus();

  return ( myStatus->GetSex() == PM_MALE );
}



//////////////////////////////////////////////////////////////////////////
///
///   @brief    便利関数：自動撮影
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   着せ替え共通更新処理
 *
 * @param[in]   value
 * @param[in]   itemMax
 */
void DebugClass::_UpdateAutoCapDressup( s32* value, u32 itemMax )
{
  s32 nextVal = *value;

  /// キー入力の取得
  gfl2::ui::Button* button = _GetButton();

  if( button->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    if( nextVal > -1 )
    {
      nextVal -= 1;
    }
  }
  if( button->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    if( nextVal < static_cast<s32>( itemMax - 1 ) )
    {
      nextVal += 1;
    }
  }

  *value = nextVal;
}

/**
 * @brief   着せ替え共通描画処理
 *
 * @param[in]   item
 * @param[in]   value
 *
 * @retval  文字列
 */
wchar_t* DebugClass::_DrawAutoCapDressup( gfl2::debug::DebugWinItem* item, s32 value )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), value );

  return workStr;
}

/// ---[ DebugClass end ]---
//////////////////////////////////////////////////////////////////////////

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif  // PM_DEBUG
