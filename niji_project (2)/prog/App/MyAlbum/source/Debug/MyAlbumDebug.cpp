#if PM_DEBUG

//======================================================================
/**
 * @file MyAlbumDebug.cpp
 * @date 2017/04/25 16:56:27
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "MyAlbumDebug.h"

#include <GameSys/include/GameData.h>
#include <NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketManager.h>
#include <util/include/gfl2_std_string.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( MyAlbum )


//////////////////////////////////////////////////////////////////////////
/// ---[ グローバル変数宣言 begin ]---

App::FinderStudio::Viewer::FinderStudioViewer* DebugClass::m_pViewer = NULL;
App::FinderStudio::Static::IntegratedData DebugClass::m_ReadPhotoParam;
App::FinderStudio::Static::IntegratedData DebugClass::m_IllegalPhotoParam;
App::FinderStudio::Static::IntegratedData DebugClass::m_ViewerPhotoParam;
bool DebugClass::m_bPhotoReproduct = false;

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
  m_pHeap = GFL_CREATE_LOCAL_HEAP_NAME( heap, 0x10000, gfl2::heap::HEAP_TYPE_EXP, false, "MyAlbumDebugHeap" );

  /// Rootグループの追加
  m_pRootGroup = DEBUG_WIN_ADD_GROUP( m_pHeap, L"MyAlbum", NULL );

  /// デバッグメニューの初期化
  {
    /// 不正データ設定
    InitDebugMenu_IllegalParameter();
    /// データ出力
    InitDebugMenu_OutputParameter();
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
 * @brief   不正データ
 */
void DebugClass::InitDebugMenu_IllegalParameter( void )
{
  gfl2::debug::DebugWinGroup* mainGroup = DEBUG_WIN_ADD_GROUP( m_pHeap, L"不正データ設定", m_pRootGroup );
  {
    gfl2::debug::DebugWinGroup* pokeGroup = DEBUG_WIN_ADD_GROUP( m_pHeap, L"ポケモンデータ", mainGroup );
    {
      DEBUG_WIN_ADD_ITEM( pokeGroup, m_pHeap, L"モンスターNo", NULL, UpdateIllegalParameter_PokeMonsNo, DrawIllegalParameter_PokeMonsNo );
      DEBUG_WIN_ADD_ITEM( pokeGroup, m_pHeap, L"フォルムNo", NULL, UpdateIllegalParameter_PokeFormNo, DrawIllegalParameter_PokeFormNo );
      DEBUG_WIN_ADD_ITEM( pokeGroup, m_pHeap, L"性別", NULL, UpdateIllegalParameter_PokeSex, DrawIllegalParameter_PokeSex );
      DEBUG_WIN_ADD_ITEM( pokeGroup, m_pHeap, L"色違いフラグ", NULL, UpdateIllegalParameter_PokeRare, DrawIllegalParameter_PokeRare );
    }

    gfl2::debug::DebugWinGroup* dressupGroup = DEBUG_WIN_ADD_GROUP( m_pHeap, L"着せ替えデータ", mainGroup );
    {
      DEBUG_WIN_ADD_ITEM( dressupGroup, m_pHeap, L"性別", NULL, UpdateIllegalParameter_DressupSex, DrawIllegalParameter_DressupSex );
      DEBUG_WIN_ADD_ITEM( dressupGroup, m_pHeap, L"体の色", NULL, UpdateIllegalParameter_DressupBodyColor, DrawIllegalParameter_DressupBodyColor );
      DEBUG_WIN_ADD_ITEM( dressupGroup, m_pHeap, L"目の色", NULL, UpdateIllegalParameter_DressupEyeColor, DrawIllegalParameter_DressupEyeColor );
      DEBUG_WIN_ADD_ITEM( dressupGroup, m_pHeap, L"髪の色", NULL, UpdateIllegalParameter_DressupHairColor, DrawIllegalParameter_DressupHairColor );
      DEBUG_WIN_ADD_ITEM( dressupGroup, m_pHeap, L"リップの色", NULL, UpdateIllegalParameter_DressupLipColor, DrawIllegalParameter_DressupLipColor );
      DEBUG_WIN_ADD_ITEM( dressupGroup, m_pHeap, L"髪型", NULL, UpdateIllegalParameter_DressupHairStyle, DrawIllegalParameter_DressupHairStyle );
      DEBUG_WIN_ADD_ITEM( dressupGroup, m_pHeap, L"アイアクセサリ", NULL, UpdateIllegalParameter_DressupEyeAcce, DrawIllegalParameter_DressupEyeAcce );
      DEBUG_WIN_ADD_ITEM( dressupGroup, m_pHeap, L"ヘアアクセサリ", NULL, UpdateIllegalParameter_DressupHairAcce, DrawIllegalParameter_DressupHairAcce );
      DEBUG_WIN_ADD_ITEM( dressupGroup, m_pHeap, L"バッグ", NULL, UpdateIllegalParameter_DressupBag, DrawIllegalParameter_DressupBag );
      DEBUG_WIN_ADD_ITEM( dressupGroup, m_pHeap, L"ボトムス", NULL, UpdateIllegalParameter_DressupBottoms, DrawIllegalParameter_DressupBottoms );
      DEBUG_WIN_ADD_ITEM( dressupGroup, m_pHeap, L"帽子", NULL, UpdateIllegalParameter_DressupHat, DrawIllegalParameter_DressupHat );
      DEBUG_WIN_ADD_ITEM( dressupGroup, m_pHeap, L"レッグ", NULL, UpdateIllegalParameter_DressupLegs, DrawIllegalParameter_DressupLegs );
      DEBUG_WIN_ADD_ITEM( dressupGroup, m_pHeap, L"シューズ", NULL, UpdateIllegalParameter_DressupShoes, DrawIllegalParameter_DressupShoes );
      DEBUG_WIN_ADD_ITEM( dressupGroup, m_pHeap, L"トップス", NULL, UpdateIllegalParameter_DressupTops, DrawIllegalParameter_DressupTops );
      DEBUG_WIN_ADD_ITEM( dressupGroup, m_pHeap, L"シチュエーション着替え", NULL, UpdateIllegalParameter_DressupSituation, DrawIllegalParameter_DressupSituation );
    }

    gfl2::debug::DebugWinGroup* bgGroup = DEBUG_WIN_ADD_GROUP( m_pHeap, L"背景", mainGroup );
    {
      DEBUG_WIN_ADD_ITEM( bgGroup, m_pHeap, L"地面ループアニメーションフレーム", NULL, UpdateIllegalParameter_BgGroundAnimFrame, DrawIllegalParameter_BgGroundAnimFrame );
      DEBUG_WIN_ADD_ITEM( bgGroup, m_pHeap, L"近景ループアニメーションフレーム", NULL, UpdateIllegalParameter_BgNearAnimFrame, DrawIllegalParameter_BgNearAnimFrame );
      DEBUG_WIN_ADD_ITEM( bgGroup, m_pHeap, L"遠景ループアニメーションフレーム", NULL, UpdateIllegalParameter_BgFarAnimFrame, DrawIllegalParameter_BgFarAnimFrame );
      DEBUG_WIN_ADD_ITEM( bgGroup, m_pHeap, L"頂点モデルループアニメーションフレーム", NULL, UpdateIllegalParameter_BgVertexModelAnimFrame, DrawIllegalParameter_BgVertexModelAnimFrame );
      DEBUG_WIN_ADD_ITEM( bgGroup, m_pHeap, L"戦闘背景の時間アニメーションフレーム", NULL, UpdateIllegalParameter_BgTimeAnimFrame, DrawIllegalParameter_BgTimeAnimFrame );
      DEBUG_WIN_ADD_ITEM( bgGroup, m_pHeap, L"天球(雲)のアニメフレーム", NULL, UpdateIllegalParameter_BgCloudAnimFrame, DrawIllegalParameter_BgCloudAnimFrame );
      DEBUG_WIN_ADD_ITEM( bgGroup, m_pHeap, L"天球(太陽)のギラギラのアニメフレーム", NULL, UpdateIllegalParameter_BgSunAnimFrame, DrawIllegalParameter_BgSunAnimFrame );
      DEBUG_WIN_ADD_ITEM( bgGroup, m_pHeap, L"背景セットインデックス", NULL, UpdateIllegalParameter_BgID, DrawIllegalParameter_BgID );
      DEBUG_WIN_ADD_ITEM( bgGroup, m_pHeap, L"戦闘背景の時間アニメーション種", NULL, UpdateIllegalParameter_BgAnimID, DrawIllegalParameter_BgAnimID );
      DEBUG_WIN_ADD_ITEM( bgGroup, m_pHeap, L"天球(月)の満ち欠けパターン", NULL, UpdateIllegalParameter_BgMoonPattern, DrawIllegalParameter_BgMoonPattern );
      DEBUG_WIN_ADD_ITEM( bgGroup, m_pHeap, L"時間帯", NULL, UpdateIllegalParameter_BgTimeZone, DrawIllegalParameter_BgTimeZone );
    }

    gfl2::debug::DebugWinGroup* pokeMotionGroup = DEBUG_WIN_ADD_GROUP( m_pHeap, L"ポケモンモーション", mainGroup );
    {
      DEBUG_WIN_ADD_ITEM( pokeMotionGroup, m_pHeap, L"モーション種", NULL, UpdateIllegalParameter_PokeMotionType, DrawIllegalParameter_PokeMotionType );
      DEBUG_WIN_ADD_ITEM( pokeMotionGroup, m_pHeap, L"モーションNo", NULL, UpdateIllegalParameter_PokeMotionNo, DrawIllegalParameter_PokeMotionNo );
      DEBUG_WIN_ADD_ITEM( pokeMotionGroup, m_pHeap, L"通常モーションフレーム", NULL, UpdateIllegalParameter_PokeMotionFrame, DrawIllegalParameter_PokeMotionFrame );
      DEBUG_WIN_ADD_ITEM( pokeMotionGroup, m_pHeap, L"目パチアニメーションパターン１", NULL, UpdateIllegalParameter_PokeMotionBlink1, DrawIllegalParameter_PokeMotionBlink1 );
      DEBUG_WIN_ADD_ITEM( pokeMotionGroup, m_pHeap, L"目パチアニメーションパターン２", NULL, UpdateIllegalParameter_PokeMotionBlink2, DrawIllegalParameter_PokeMotionBlink2 );
      DEBUG_WIN_ADD_ITEM( pokeMotionGroup, m_pHeap, L"目パチアニメーションパターン３", NULL, UpdateIllegalParameter_PokeMotionBlink3, DrawIllegalParameter_PokeMotionBlink3 );
      DEBUG_WIN_ADD_ITEM( pokeMotionGroup, m_pHeap, L"口アニメーションパターン１", NULL, UpdateIllegalParameter_PokeMotionLipSync1, DrawIllegalParameter_PokeMotionLipSync1 );
      DEBUG_WIN_ADD_ITEM( pokeMotionGroup, m_pHeap, L"口アニメーションパターン２", NULL, UpdateIllegalParameter_PokeMotionLipSync2, DrawIllegalParameter_PokeMotionLipSync2 );
      DEBUG_WIN_ADD_ITEM( pokeMotionGroup, m_pHeap, L"口アニメーションパターン３", NULL, UpdateIllegalParameter_PokeMotionLipSync3, DrawIllegalParameter_PokeMotionLipSync3 );
      DEBUG_WIN_ADD_ITEM( pokeMotionGroup, m_pHeap, L"ループアニメーションパターンフレーム１", NULL, UpdateIllegalParameter_PokeMotionLoopAnimFrame1, DrawIllegalParameter_PokeMotionLoopAnimFrame1 );
      DEBUG_WIN_ADD_ITEM( pokeMotionGroup, m_pHeap, L"ループアニメーションパターンフレーム２", NULL, UpdateIllegalParameter_PokeMotionLoopAnimFrame2, DrawIllegalParameter_PokeMotionLoopAnimFrame2 );
      DEBUG_WIN_ADD_ITEM( pokeMotionGroup, m_pHeap, L"ループアニメーションパターンフレーム３", NULL, UpdateIllegalParameter_PokeMotionLoopAnimFrame3, DrawIllegalParameter_PokeMotionLoopAnimFrame3 );
      DEBUG_WIN_ADD_ITEM( pokeMotionGroup, m_pHeap, L"ループアニメーションパターンフレーム４", NULL, UpdateIllegalParameter_PokeMotionLoopAnimFrame4, DrawIllegalParameter_PokeMotionLoopAnimFrame4 );
    }

    gfl2::debug::DebugWinGroup* trainerMotionGroup = DEBUG_WIN_ADD_GROUP( m_pHeap, L"トレーナーモーション", mainGroup );
    {
      DEBUG_WIN_ADD_ITEM( trainerMotionGroup, m_pHeap, L"モーション種", NULL, UpdateIllegalParameter_TrainerMotionID, DrawIllegalParameter_TrainerMotionID );
      DEBUG_WIN_ADD_ITEM( trainerMotionGroup, m_pHeap, L"モーションフレーム", NULL, UpdateIllegalParameter_TrainerMotionFrame, DrawIllegalParameter_TrainerMotionFrame );
      DEBUG_WIN_ADD_ITEM( trainerMotionGroup, m_pHeap, L"目パチ状態", NULL, UpdateIllegalParameter_TrainerMotionBlink, DrawIllegalParameter_TrainerMotionBlink );
    }

    gfl2::debug::DebugWinGroup* posRotGroup = DEBUG_WIN_ADD_GROUP( m_pHeap, L"座標・角度", mainGroup );
    {
      DEBUG_WIN_ADD_ITEM( posRotGroup, m_pHeap, L"ポケモンX座標", NULL, UpdateIllegalParameter_PosDegPokePosX, DrawIllegalParameter_PosDegPokePosX );
      DEBUG_WIN_ADD_ITEM( posRotGroup, m_pHeap, L"ポケモンZ座標", NULL, UpdateIllegalParameter_PosDegPokePosZ, DrawIllegalParameter_PosDegPokePosZ );
      DEBUG_WIN_ADD_ITEM( posRotGroup, m_pHeap, L"ポケモン角度", NULL, UpdateIllegalParameter_PosDegPokeDeg, DrawIllegalParameter_PosDegPokeDeg );
      DEBUG_WIN_ADD_ITEM( posRotGroup, m_pHeap, L"トレーナー角度", NULL, UpdateIllegalParameter_PosDegTrainerDeg, DrawIllegalParameter_PosDegTrainerDeg );
    }

    gfl2::debug::DebugWinGroup* cameraGroup = DEBUG_WIN_ADD_GROUP( m_pHeap, L"カメラ", mainGroup );
    {
      DEBUG_WIN_ADD_ITEM( cameraGroup, m_pHeap, L"カメラ高さ", NULL, UpdateIllegalParameter_CameraHeight, DrawIllegalParameter_CameraHeight );
      DEBUG_WIN_ADD_ITEM( cameraGroup, m_pHeap, L"カメラY軸角度", NULL, UpdateIllegalParameter_CameraDegAxisY, DrawIllegalParameter_CameraDegAxisY );
      DEBUG_WIN_ADD_ITEM( cameraGroup, m_pHeap, L"カメラX軸角度", NULL, UpdateIllegalParameter_CameraDegAxisX, DrawIllegalParameter_CameraDegAxisX );
      DEBUG_WIN_ADD_ITEM( cameraGroup, m_pHeap, L"画角", NULL, UpdateIllegalParameter_CameraAngle, DrawIllegalParameter_CameraAngle );
      DEBUG_WIN_ADD_ITEM( cameraGroup, m_pHeap, L"カメラ係数", NULL, UpdateIllegalParameter_CameraCoefficient, DrawIllegalParameter_CameraCoefficient );
    }

    gfl2::debug::DebugWinGroup* otherGroup = DEBUG_WIN_ADD_GROUP( m_pHeap, L"その他", mainGroup );
    {
      DEBUG_WIN_ADD_ITEM( otherGroup, m_pHeap, L"撮影モード", NULL, UpdateIllegalParameter_OtherCaptureMode, DrawIllegalParameter_OtherCaptureMode );
      DEBUG_WIN_ADD_ITEM( otherGroup, m_pHeap, L"構図", NULL, UpdateIllegalParameter_OtherComposition, DrawIllegalParameter_OtherComposition );
    }

    gfl2::debug::DebugWinGroup* stamp1Group = DEBUG_WIN_ADD_GROUP( m_pHeap, L"スタンプデータ[1]", mainGroup );
    {
      DEBUG_WIN_ADD_ITEM( stamp1Group, m_pHeap, L"種類", NULL, UpdateIllegalParameter_Stamp1ID, DrawIllegalParameter_Stamp1ID );
      DEBUG_WIN_ADD_ITEM( stamp1Group, m_pHeap, L"X座標", NULL, UpdateIllegalParameter_Stamp1X, DrawIllegalParameter_Stamp1X );
      DEBUG_WIN_ADD_ITEM( stamp1Group, m_pHeap, L"Y座標", NULL, UpdateIllegalParameter_Stamp1Y, DrawIllegalParameter_Stamp1Y );
      DEBUG_WIN_ADD_ITEM( stamp1Group, m_pHeap, L"フレームの種類", NULL, UpdateIllegalParameter_Stamp1Frame, DrawIllegalParameter_Stamp1Frame );
    }

    gfl2::debug::DebugWinGroup* stamp2Group = DEBUG_WIN_ADD_GROUP( m_pHeap, L"スタンプデータ[2]", mainGroup );
    {
      DEBUG_WIN_ADD_ITEM( stamp2Group, m_pHeap, L"種類", NULL, UpdateIllegalParameter_Stamp2ID, DrawIllegalParameter_Stamp2ID );
      DEBUG_WIN_ADD_ITEM( stamp2Group, m_pHeap, L"X座標", NULL, UpdateIllegalParameter_Stamp2X, DrawIllegalParameter_Stamp2X );
      DEBUG_WIN_ADD_ITEM( stamp2Group, m_pHeap, L"Y座標", NULL, UpdateIllegalParameter_Stamp2Y, DrawIllegalParameter_Stamp2Y );
      DEBUG_WIN_ADD_ITEM( stamp2Group, m_pHeap, L"フレームの種類", NULL, UpdateIllegalParameter_Stamp2Frame, DrawIllegalParameter_Stamp2Frame );
    }

    gfl2::debug::DebugWinGroup* stamp3Group = DEBUG_WIN_ADD_GROUP( m_pHeap, L"スタンプデータ[3]", mainGroup );
    {
      DEBUG_WIN_ADD_ITEM( stamp3Group, m_pHeap, L"種類", NULL, UpdateIllegalParameter_Stamp3ID, DrawIllegalParameter_Stamp3ID );
      DEBUG_WIN_ADD_ITEM( stamp3Group, m_pHeap, L"X座標", NULL, UpdateIllegalParameter_Stamp3X, DrawIllegalParameter_Stamp3X );
      DEBUG_WIN_ADD_ITEM( stamp3Group, m_pHeap, L"Y座標", NULL, UpdateIllegalParameter_Stamp3Y, DrawIllegalParameter_Stamp3Y );
      DEBUG_WIN_ADD_ITEM( stamp3Group, m_pHeap, L"フレームの種類", NULL, UpdateIllegalParameter_Stamp3Frame, DrawIllegalParameter_Stamp3Frame );
    }

    gfl2::debug::DebugWinGroup* stamp4Group = DEBUG_WIN_ADD_GROUP( m_pHeap, L"スタンプデータ[4]", mainGroup );
    {
      DEBUG_WIN_ADD_ITEM( stamp4Group, m_pHeap, L"種類", NULL, UpdateIllegalParameter_Stamp4ID, DrawIllegalParameter_Stamp4ID );
      DEBUG_WIN_ADD_ITEM( stamp4Group, m_pHeap, L"X座標", NULL, UpdateIllegalParameter_Stamp4X, DrawIllegalParameter_Stamp4X );
      DEBUG_WIN_ADD_ITEM( stamp4Group, m_pHeap, L"Y座標", NULL, UpdateIllegalParameter_Stamp4Y, DrawIllegalParameter_Stamp4Y );
      DEBUG_WIN_ADD_ITEM( stamp4Group, m_pHeap, L"フレームの種類", NULL, UpdateIllegalParameter_Stamp4Frame, DrawIllegalParameter_Stamp4Frame );
    }

    gfl2::debug::DebugWinGroup* stamp5Group = DEBUG_WIN_ADD_GROUP( m_pHeap, L"スタンプデータ[5]", mainGroup );
    {
      DEBUG_WIN_ADD_ITEM( stamp5Group, m_pHeap, L"種類", NULL, UpdateIllegalParameter_Stamp5ID, DrawIllegalParameter_Stamp5ID );
      DEBUG_WIN_ADD_ITEM( stamp5Group, m_pHeap, L"X座標", NULL, UpdateIllegalParameter_Stamp5X, DrawIllegalParameter_Stamp5X );
      DEBUG_WIN_ADD_ITEM( stamp5Group, m_pHeap, L"Y座標", NULL, UpdateIllegalParameter_Stamp5Y, DrawIllegalParameter_Stamp5Y );
      DEBUG_WIN_ADD_ITEM( stamp5Group, m_pHeap, L"フレームの種類", NULL, UpdateIllegalParameter_Stamp5Frame, DrawIllegalParameter_Stamp5Frame );
    }

    gfl2::debug::DebugWinGroup* stamp6Group = DEBUG_WIN_ADD_GROUP( m_pHeap, L"スタンプデータ[6]", mainGroup );
    {
      DEBUG_WIN_ADD_ITEM( stamp6Group, m_pHeap, L"種類", NULL, UpdateIllegalParameter_Stamp6ID, DrawIllegalParameter_Stamp6ID );
      DEBUG_WIN_ADD_ITEM( stamp6Group, m_pHeap, L"X座標", NULL, UpdateIllegalParameter_Stamp6X, DrawIllegalParameter_Stamp6X );
      DEBUG_WIN_ADD_ITEM( stamp6Group, m_pHeap, L"Y座標", NULL, UpdateIllegalParameter_Stamp6Y, DrawIllegalParameter_Stamp6Y );
      DEBUG_WIN_ADD_ITEM( stamp6Group, m_pHeap, L"フレームの種類", NULL, UpdateIllegalParameter_Stamp6Frame, DrawIllegalParameter_Stamp6Frame );
    }

    gfl2::debug::DebugWinGroup* stamp7Group = DEBUG_WIN_ADD_GROUP( m_pHeap, L"スタンプデータ[7]", mainGroup );
    {
      DEBUG_WIN_ADD_ITEM( stamp7Group, m_pHeap, L"種類", NULL, UpdateIllegalParameter_Stamp7ID, DrawIllegalParameter_Stamp7ID );
      DEBUG_WIN_ADD_ITEM( stamp7Group, m_pHeap, L"X座標", NULL, UpdateIllegalParameter_Stamp7X, DrawIllegalParameter_Stamp7X );
      DEBUG_WIN_ADD_ITEM( stamp7Group, m_pHeap, L"Y座標", NULL, UpdateIllegalParameter_Stamp7Y, DrawIllegalParameter_Stamp7Y );
      DEBUG_WIN_ADD_ITEM( stamp7Group, m_pHeap, L"フレームの種類", NULL, UpdateIllegalParameter_Stamp7Frame, DrawIllegalParameter_Stamp7Frame );
    }

    gfl2::debug::DebugWinGroup* stamp8Group = DEBUG_WIN_ADD_GROUP( m_pHeap, L"スタンプデータ[8]", mainGroup );
    {
      DEBUG_WIN_ADD_ITEM( stamp8Group, m_pHeap, L"種類", NULL, UpdateIllegalParameter_Stamp8ID, DrawIllegalParameter_Stamp8ID );
      DEBUG_WIN_ADD_ITEM( stamp8Group, m_pHeap, L"X座標", NULL, UpdateIllegalParameter_Stamp8X, DrawIllegalParameter_Stamp8X );
      DEBUG_WIN_ADD_ITEM( stamp8Group, m_pHeap, L"Y座標", NULL, UpdateIllegalParameter_Stamp8Y, DrawIllegalParameter_Stamp8Y );
      DEBUG_WIN_ADD_ITEM( stamp8Group, m_pHeap, L"フレームの種類", NULL, UpdateIllegalParameter_Stamp8Frame, DrawIllegalParameter_Stamp8Frame );
    }

    gfl2::debug::DebugWinGroup* stamp9Group = DEBUG_WIN_ADD_GROUP( m_pHeap, L"スタンプデータ[9]", mainGroup );
    {
      DEBUG_WIN_ADD_ITEM( stamp9Group, m_pHeap, L"種類", NULL, UpdateIllegalParameter_Stamp9ID, DrawIllegalParameter_Stamp9ID );
      DEBUG_WIN_ADD_ITEM( stamp9Group, m_pHeap, L"X座標", NULL, UpdateIllegalParameter_Stamp9X, DrawIllegalParameter_Stamp9X );
      DEBUG_WIN_ADD_ITEM( stamp9Group, m_pHeap, L"Y座標", NULL, UpdateIllegalParameter_Stamp9Y, DrawIllegalParameter_Stamp9Y );
      DEBUG_WIN_ADD_ITEM( stamp9Group, m_pHeap, L"フレームの種類", NULL, UpdateIllegalParameter_Stamp9Frame, DrawIllegalParameter_Stamp9Frame );
    }

    gfl2::debug::DebugWinGroup* stamp10Group = DEBUG_WIN_ADD_GROUP( m_pHeap, L"スタンプデータ[10]", mainGroup );
    {
      DEBUG_WIN_ADD_ITEM( stamp10Group, m_pHeap, L"種類", NULL, UpdateIllegalParameter_Stamp10ID, DrawIllegalParameter_Stamp10ID );
      DEBUG_WIN_ADD_ITEM( stamp10Group, m_pHeap, L"X座標", NULL, UpdateIllegalParameter_Stamp10X, DrawIllegalParameter_Stamp10X );
      DEBUG_WIN_ADD_ITEM( stamp10Group, m_pHeap, L"Y座標", NULL, UpdateIllegalParameter_Stamp10Y, DrawIllegalParameter_Stamp10Y );
      DEBUG_WIN_ADD_ITEM( stamp10Group, m_pHeap, L"フレームの種類", NULL, UpdateIllegalParameter_Stamp10Frame, DrawIllegalParameter_Stamp10Frame );
    }

    DEBUG_WIN_ADD_ITEM( mainGroup, m_pHeap, L"データ反映", NULL, UpdateIllegalParameter_ReflectParam, DrawIllegalParameter_ReflectParam );
    DEBUG_WIN_ADD_ITEM( mainGroup, m_pHeap, L"写真共有", NULL, UpdateIllegalParameter_PhotoShare, DrawIllegalParameter_PhotoShare );
  }
}

/**
 * @brief   データ出力
 */
void DebugClass::InitDebugMenu_OutputParameter( void )
{
  gfl2::debug::DebugWinGroup* mainGroup = DEBUG_WIN_ADD_GROUP( m_pHeap, L"データ出力", m_pRootGroup );
  {
    DEBUG_WIN_ADD_ITEM( mainGroup, m_pHeap, L"出力：正規データ", NULL, UpdateOutputParameter_ReadParameter, DrawOutputParameter_ReadParameter );
    DEBUG_WIN_ADD_ITEM( mainGroup, m_pHeap, L"出力：不正データ", NULL, UpdateOutputParameter_IllegalParameter, DrawOutputParameter_IllegalParameter );
    DEBUG_WIN_ADD_ITEM( mainGroup, m_pHeap, L"出力：表示データ", NULL, UpdateOutputParameter_ViewerParameter, DrawOutputParameter_ViewerParameter );
  }
}




//////////////////////////////////////////////////////////////////////////
///
///   @brief    不正データ設定：ポケモン
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   更新：不正データ設定「種族No」
 */
void DebugClass::UpdateIllegalParameter_PokeMonsNo( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->monsNo;
  u32 min = 1;
  u32 max = _GetBitMaxNum( 10 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  FUKUSHIMA_PRINT( "PokeMonsNo : now=[%d] min=[%d] max=[%d] next=[%d]\n", now, min, max, next );

  /// パラメータ更新
  photoParam->monsNo = next;
}

/**
 * @brief   描画：不正データ設定「種族No」
 */
wchar_t* DebugClass::DrawIllegalParameter_PokeMonsNo( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->monsNo );

  return workStr;
}


/**
 * @brief   更新：不正データ設定「フォルム」
 */
void DebugClass::UpdateIllegalParameter_PokeFormNo( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->formNo;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 5 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->formNo = next;
}

/**
 * @brief   描画：不正データ設定「フォルム」
 */
wchar_t* DebugClass::DrawIllegalParameter_PokeFormNo( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->formNo );

  return workStr;
}


/**
 * @brief   更新：不正データ設定「性別」
 */
void DebugClass::UpdateIllegalParameter_PokeSex( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->sexPoke;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 2 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->sexPoke = next;
}

/**
 * @brief   描画：不正データ設定「性別」
 */
wchar_t* DebugClass::DrawIllegalParameter_PokeSex( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->sexPoke );

  return workStr;
}


/**
 * @brief   更新：不正データ設定「色違い」
 */
void DebugClass::UpdateIllegalParameter_PokeRare( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->isRare;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 1 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->isRare = next;
}

/**
 * @brief   描画：不正データ設定「色違い」
 */
wchar_t* DebugClass::DrawIllegalParameter_PokeRare( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->isRare );

  return workStr;
}




//////////////////////////////////////////////////////////////////////////
///
///   @brief    不正データ設定：着せ替え
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   更新：不正データ設定「性別」
 */
void DebugClass::UpdateIllegalParameter_DressupSex( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->sexTrainer;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 1 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->sexTrainer = next;
}

/**
 * @brief   描画：不正データ設定「性別」
 */
wchar_t* DebugClass::DrawIllegalParameter_DressupSex( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->sexTrainer );

  return workStr;
}


/**
 * @brief   更新：不正データ設定「体色」
 */
void DebugClass::UpdateIllegalParameter_DressupBodyColor( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->skinColor;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 2 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->skinColor = next;
}

/**
 * @brief   描画：不正データ設定「体色」
 */
wchar_t* DebugClass::DrawIllegalParameter_DressupBodyColor( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->skinColor );

  return workStr;
}


/**
 * @brief   更新：不正データ設定「目色」
 */
void DebugClass::UpdateIllegalParameter_DressupEyeColor( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->eyeColor;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 4 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->eyeColor = next;
}

/**
 * @brief   描画：不正データ設定「目色」
 */
wchar_t* DebugClass::DrawIllegalParameter_DressupEyeColor( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->eyeColor );

  return workStr;
}


/**
 * @brief   更新：不正データ設定「髪色」
 */
void DebugClass::UpdateIllegalParameter_DressupHairColor( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->hairColor;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 4 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->hairColor = next;
}

/**
 * @brief   描画：不正データ設定「髪色」
 */
wchar_t* DebugClass::DrawIllegalParameter_DressupHairColor( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->hairColor );

  return workStr;
}


/**
 * @brief   更新：不正データ設定「リップ色」
 */
void DebugClass::UpdateIllegalParameter_DressupLipColor( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->lipColor;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 3 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->lipColor = next;
}

/**
 * @brief   描画：不正データ設定「リップ色」
 */
wchar_t* DebugClass::DrawIllegalParameter_DressupLipColor( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->lipColor );

  return workStr;
}


/**
 * @brief   更新：不正データ設定「髪型」
 */
void DebugClass::UpdateIllegalParameter_DressupHairStyle( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->hairStyle;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 5 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->hairStyle = next;
}

/**
 * @brief   描画：不正データ設定「髪型」
 */
wchar_t* DebugClass::DrawIllegalParameter_DressupHairStyle( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->hairStyle );

  return workStr;
}


/**
 * @brief   更新：不正データ設定「アイアクセサリ」
 */
void DebugClass::UpdateIllegalParameter_DressupEyeAcce( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->eyeAccessory;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 5 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->eyeAccessory = next;
}

/**
 * @brief   描画：不正データ設定「アイアクセサリ」
 */
wchar_t* DebugClass::DrawIllegalParameter_DressupEyeAcce( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->eyeAccessory );

  return workStr;
}


/**
 * @brief   更新：不正データ設定「ヘアアクセサリ」
 */
void DebugClass::UpdateIllegalParameter_DressupHairAcce( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->hairAccessory;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 6 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->hairAccessory = next;
}

/**
 * @brief   描画：不正データ設定「ヘアアクセサリ」
 */
wchar_t* DebugClass::DrawIllegalParameter_DressupHairAcce( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->hairAccessory );

  return workStr;
}


/**
 * @brief   更新：不正データ設定「バッグ」
 */
void DebugClass::UpdateIllegalParameter_DressupBag( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->bag;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 8 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->bag = next;
}

/**
 * @brief   描画：不正データ設定「バッグ」
 */
wchar_t* DebugClass::DrawIllegalParameter_DressupBag( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->bag );

  return workStr;
}


/**
 * @brief   更新：不正データ設定「ボトムス」
 */
void DebugClass::UpdateIllegalParameter_DressupBottoms( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->bottoms;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 8 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->bottoms = next;
}

/**
 * @brief   描画：不正データ設定「ボトムス」
 */
wchar_t* DebugClass::DrawIllegalParameter_DressupBottoms( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->bottoms );

  return workStr;
}


/**
 * @brief   更新：不正データ設定「ハット」
 */
void DebugClass::UpdateIllegalParameter_DressupHat( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->hat;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 7 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->hat = next;
}

/**
 * @brief   描画：不正データ設定「ハット」
 */
wchar_t* DebugClass::DrawIllegalParameter_DressupHat( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->hat );

  return workStr;
}


/**
 * @brief   更新：不正データ設定「レッグス」
 */
void DebugClass::UpdateIllegalParameter_DressupLegs( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->leg;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 7 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->leg = next;
}

/**
 * @brief   描画：不正データ設定「レッグス」
 */
wchar_t* DebugClass::DrawIllegalParameter_DressupLegs( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->leg );

  return workStr;
}


/**
 * @brief   更新：不正データ設定「シューズ」
 */
void DebugClass::UpdateIllegalParameter_DressupShoes( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->shoes;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 8 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->shoes = next;
}

/**
 * @brief   描画：不正データ設定「シューズ」
 */
wchar_t* DebugClass::DrawIllegalParameter_DressupShoes( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->shoes );

  return workStr;
}


/**
 * @brief   更新：不正データ設定「トップス」
 */
void DebugClass::UpdateIllegalParameter_DressupTops( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->tops;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 9 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->tops = next;
}

/**
 * @brief   描画：不正データ設定「トップス」
 */
wchar_t* DebugClass::DrawIllegalParameter_DressupTops( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->tops );

  return workStr;
}


/**
 * @brief   更新：不正データ設定「シチュエーション」
 */
void DebugClass::UpdateIllegalParameter_DressupSituation( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->situ;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 2 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->situ = next;
}

/**
 * @brief   描画：不正データ設定「シチュエーション」
 */
wchar_t* DebugClass::DrawIllegalParameter_DressupSituation( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->situ );

  return workStr;
}




//////////////////////////////////////////////////////////////////////////
///
///   @brief    不正データ設定：背景
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   更新：不正データ設定「地面アニメフレーム」
 */
void DebugClass::UpdateIllegalParameter_BgGroundAnimFrame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->bgGroundLoopAnmFrame;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 10 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->bgGroundLoopAnmFrame = next;
}

/**
 * @brief   描画：不正データ設定「地面アニメフレーム」
 */
wchar_t* DebugClass::DrawIllegalParameter_BgGroundAnimFrame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->bgGroundLoopAnmFrame );

  return workStr;
}


/**
 * @brief   更新：不正データ設定「近景アニメフレーム」
 */
void DebugClass::UpdateIllegalParameter_BgNearAnimFrame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->bgNearLoopAnmFrame;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 10 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->bgNearLoopAnmFrame = next;
}

/**
 * @brief   描画：不正データ設定「近景アニメフレーム」
 */
wchar_t* DebugClass::DrawIllegalParameter_BgNearAnimFrame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->bgNearLoopAnmFrame );

  return workStr;
}


/**
 * @brief   更新：不正データ設定「遠景アニメフレーム」
 */
void DebugClass::UpdateIllegalParameter_BgFarAnimFrame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->bgFarLoopAnmFrame;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 10 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->bgFarLoopAnmFrame = next;
}

/**
 * @brief   描画：不正データ設定「遠景アニメフレーム」
 */
wchar_t* DebugClass::DrawIllegalParameter_BgFarAnimFrame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->bgFarLoopAnmFrame );

  return workStr;
}


/**
 * @brief   更新：不正データ設定「頂点モデルアニメフレーム」
 */
void DebugClass::UpdateIllegalParameter_BgVertexModelAnimFrame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->bgBuildLoopAnmFrame;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 10 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->bgBuildLoopAnmFrame = next;
}

/**
 * @brief   描画：不正データ設定「頂点モデルアニメフレーム」
 */
wchar_t* DebugClass::DrawIllegalParameter_BgVertexModelAnimFrame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->bgBuildLoopAnmFrame );

  return workStr;
}


/**
 * @brief   更新：不正データ設定「戦闘背景アニメフレーム」
 */
void DebugClass::UpdateIllegalParameter_BgTimeAnimFrame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->bgTimeAnmFrame;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 5 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->bgTimeAnmFrame = next;
}

/**
 * @brief   描画：不正データ設定「戦闘背景アニメフレーム」
 */
wchar_t* DebugClass::DrawIllegalParameter_BgTimeAnimFrame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->bgTimeAnmFrame );

  return workStr;
}


/**
 * @brief   更新：不正データ設定「天球(雲)アニメフレーム」
 */
void DebugClass::UpdateIllegalParameter_BgCloudAnimFrame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->skyCloudAnmFrame;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 16 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->skyCloudAnmFrame = next;
}

/**
 * @brief   描画：不正データ設定「天球(雲)アニメフレーム」
 */
wchar_t* DebugClass::DrawIllegalParameter_BgCloudAnimFrame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->skyCloudAnmFrame );

  return workStr;
}


/**
 * @brief   更新：不正データ設定「天球(太陽)アニメフレーム」
 */
void DebugClass::UpdateIllegalParameter_BgSunAnimFrame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->skySunAnmFrame;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 16 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->skySunAnmFrame = next;
}

/**
 * @brief   描画：不正データ設定「天球(太陽)アニメフレーム」
 */
wchar_t* DebugClass::DrawIllegalParameter_BgSunAnimFrame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->skySunAnmFrame );

  return workStr;
}


/**
 * @brief   更新：不正データ設定「背景ID」
 */
void DebugClass::UpdateIllegalParameter_BgID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->bgSetIndex;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 7 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->bgSetIndex = next;
}

/**
 * @brief   描画：不正データ設定「背景ID」
 */
wchar_t* DebugClass::DrawIllegalParameter_BgID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->bgSetIndex );

  return workStr;
}


/**
 * @brief   更新：不正データ設定「戦闘背景アニメの種類」
 */
void DebugClass::UpdateIllegalParameter_BgAnimID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->bgTimeAnmType;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 4 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->bgTimeAnmType = next;
}

/**
 * @brief   描画：不正データ設定「戦闘背景アニメの種類」
 */
wchar_t* DebugClass::DrawIllegalParameter_BgAnimID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->bgTimeAnmType );

  return workStr;
}


/**
 * @brief   更新：不正データ設定「天球(月)の満ち欠けパターン」
 */
void DebugClass::UpdateIllegalParameter_BgMoonPattern( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->skyMoonPattern;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 3 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->skyMoonPattern = next;
}

/**
 * @brief   描画：不正データ設定「天球(月)の満ち欠けパターン」
 */
wchar_t* DebugClass::DrawIllegalParameter_BgMoonPattern( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->skyMoonPattern );

  return workStr;
}


/**
 * @brief   更新：不正データ設定「時間帯」
 */
void DebugClass::UpdateIllegalParameter_BgTimeZone( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->timeZone;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 2 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->timeZone = next;
}

/**
 * @brief   描画：不正データ設定「時間帯」
 */
wchar_t* DebugClass::DrawIllegalParameter_BgTimeZone( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->timeZone );

  return workStr;
}




//////////////////////////////////////////////////////////////////////////
///
///   @brief    不正データ設定：ポケモンモーション
///
//////////////////////////////////////////////////////////////////////////
/**
* @brief   更新：不正データ設定「モーションタイプ」
*/
void DebugClass::UpdateIllegalParameter_PokeMotionType( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->pokeMotionType;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 1 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->pokeMotionType = next;
}

/**
* @brief   描画：不正データ設定「モーションタイプ」
*/
wchar_t* DebugClass::DrawIllegalParameter_PokeMotionType( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->pokeMotionType );

  return workStr;
}


/**
* @brief   更新：不正データ設定「モーションNo」
*/
void DebugClass::UpdateIllegalParameter_PokeMotionNo( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->pokeMotionNo;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 5 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->pokeMotionNo = next;
}

/**
* @brief   描画：不正データ設定「モーションNo」
*/
wchar_t* DebugClass::DrawIllegalParameter_PokeMotionNo( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->pokeMotionNo );

  return workStr;
}


/**
* @brief   更新：不正データ設定「モーションフレーム」
*/
void DebugClass::UpdateIllegalParameter_PokeMotionFrame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->pokeMainMotionFrame;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 8 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->pokeMainMotionFrame = next;
}

/**
* @brief   描画：不正データ設定「モーションフレーム」
*/
wchar_t* DebugClass::DrawIllegalParameter_PokeMotionFrame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->pokeMainMotionFrame );

  return workStr;
}


/**
* @brief   更新：不正データ設定「目パチ１」
*/
void DebugClass::UpdateIllegalParameter_PokeMotionBlink1( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->pokeEyeAnmPattern1;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 3 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->pokeEyeAnmPattern1 = next;
}

/**
* @brief   描画：不正データ設定「目パチ１」
*/
wchar_t* DebugClass::DrawIllegalParameter_PokeMotionBlink1( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->pokeEyeAnmPattern1 );

  return workStr;
}


/**
* @brief   更新：不正データ設定「目パチ２」
*/
void DebugClass::UpdateIllegalParameter_PokeMotionBlink2( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->pokeEyeAnmPattern2;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 3 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->pokeEyeAnmPattern2 = next;
}

/**
* @brief   描画：不正データ設定「目パチ２」
*/
wchar_t* DebugClass::DrawIllegalParameter_PokeMotionBlink2( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->pokeEyeAnmPattern2 );

  return workStr;
}


/**
* @brief   更新：不正データ設定「目パチ３」
*/
void DebugClass::UpdateIllegalParameter_PokeMotionBlink3( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->pokeEyeAnmPattern3;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 3 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->pokeEyeAnmPattern3 = next;
}

/**
* @brief   描画：不正データ設定「目パチ３」
*/
wchar_t* DebugClass::DrawIllegalParameter_PokeMotionBlink3( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->pokeEyeAnmPattern3 );

  return workStr;
}


/**
* @brief   更新：不正データ設定「口パク１」
*/
void DebugClass::UpdateIllegalParameter_PokeMotionLipSync1( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->pokeMouthAnmpattern1;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 3 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->pokeMouthAnmpattern1 = next;
}

/**
* @brief   描画：不正データ設定「口パク１」
*/
wchar_t* DebugClass::DrawIllegalParameter_PokeMotionLipSync1( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->pokeMouthAnmpattern1 );

  return workStr;
}


/**
* @brief   更新：不正データ設定「口パク２」
*/
void DebugClass::UpdateIllegalParameter_PokeMotionLipSync2( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->pokeMouthAnmpattern2;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 3 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->pokeMouthAnmpattern2 = next;
}

/**
* @brief   描画：不正データ設定「口パク２」
*/
wchar_t* DebugClass::DrawIllegalParameter_PokeMotionLipSync2( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->pokeMouthAnmpattern2 );

  return workStr;
}


/**
* @brief   更新：不正データ設定「口パク３」
*/
void DebugClass::UpdateIllegalParameter_PokeMotionLipSync3( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->pokeMouthAnmpattern3;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 3 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->pokeMouthAnmpattern3 = next;
}

/**
* @brief   描画：不正データ設定「口パク３」
*/
wchar_t* DebugClass::DrawIllegalParameter_PokeMotionLipSync3( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->pokeMouthAnmpattern3 );

  return workStr;
}


/**
* @brief   更新：不正データ設定「ループアニメーションフレーム１」
*/
void DebugClass::UpdateIllegalParameter_PokeMotionLoopAnimFrame1( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->pokeLoopAnmFrame1;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 10 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->pokeLoopAnmFrame1 = next;
}

/**
* @brief   描画：不正データ設定「ループアニメーションフレーム１」
*/
wchar_t* DebugClass::DrawIllegalParameter_PokeMotionLoopAnimFrame1( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->pokeLoopAnmFrame1 );

  return workStr;
}


/**
* @brief   更新：不正データ設定「ループアニメーションフレーム２」
*/
void DebugClass::UpdateIllegalParameter_PokeMotionLoopAnimFrame2( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->pokeLoopAnmFrame2;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 10 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->pokeLoopAnmFrame2 = next;
}

/**
* @brief   描画：不正データ設定「ループアニメーションフレーム２」
*/
wchar_t* DebugClass::DrawIllegalParameter_PokeMotionLoopAnimFrame2( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->pokeLoopAnmFrame2 );

  return workStr;
}


/**
* @brief   更新：不正データ設定「ループアニメーションフレーム３」
*/
void DebugClass::UpdateIllegalParameter_PokeMotionLoopAnimFrame3( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->pokeLoopAnmFrame3;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 10 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->pokeLoopAnmFrame3 = next;
}

/**
* @brief   描画：不正データ設定「ループアニメーションフレーム３」
*/
wchar_t* DebugClass::DrawIllegalParameter_PokeMotionLoopAnimFrame3( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->pokeLoopAnmFrame3 );

  return workStr;
}


/**
* @brief   更新：不正データ設定「ループアニメーションフレーム４」
*/
void DebugClass::UpdateIllegalParameter_PokeMotionLoopAnimFrame4( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->pokeLoopAnmFrame4;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 10 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->pokeLoopAnmFrame4 = next;
}

/**
* @brief   描画：不正データ設定「ループアニメーションフレーム４」
*/
wchar_t* DebugClass::DrawIllegalParameter_PokeMotionLoopAnimFrame4( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->pokeLoopAnmFrame4 );

  return workStr;
}




//////////////////////////////////////////////////////////////////////////
///
///   @brief    不正データ設定：トレーナーモーション
///
//////////////////////////////////////////////////////////////////////////
/**
* @brief   更新：不正データ設定「」
*/
void DebugClass::UpdateIllegalParameter_TrainerMotionID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->trainerMotionType;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 6 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->trainerMotionType = next;
}

/**
* @brief   描画：不正データ設定「」
*/
wchar_t* DebugClass::DrawIllegalParameter_TrainerMotionID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->trainerMotionType );

  return workStr;
}


/**
* @brief   更新：不正データ設定「」
*/
void DebugClass::UpdateIllegalParameter_TrainerMotionFrame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->trainerMotionFrame;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 16 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->trainerMotionFrame = next;
}

/**
* @brief   描画：不正データ設定「」
*/
wchar_t* DebugClass::DrawIllegalParameter_TrainerMotionFrame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->trainerMotionFrame );

  return workStr;
}


/**
* @brief   更新：不正データ設定「」
*/
void DebugClass::UpdateIllegalParameter_TrainerMotionBlink( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->trainerEyeAnmPattern;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 2 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->trainerEyeAnmPattern = next;
}

/**
* @brief   描画：不正データ設定「」
*/
wchar_t* DebugClass::DrawIllegalParameter_TrainerMotionBlink( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->trainerEyeAnmPattern );

  return workStr;
}




//////////////////////////////////////////////////////////////////////////
///
///   @brief    不正データ設定：位置・回転
///
//////////////////////////////////////////////////////////////////////////
/**
* @brief   更新：不正データ設定「」
*/
void DebugClass::UpdateIllegalParameter_PosDegPokePosX( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  s32 now = photoParam->pokePosX;
  s32 min = 0xFFFF & 0x8000;
  s32 max = 0xFFFF & ~0x8000;
  s32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->pokePosX = next;
}

/**
* @brief   描画：不正データ設定「」
*/
wchar_t* DebugClass::DrawIllegalParameter_PosDegPokePosX( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->pokePosX );

  return workStr;
}


/**
* @brief   更新：不正データ設定「」
*/
void DebugClass::UpdateIllegalParameter_PosDegPokePosZ( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  s32 now = photoParam->pokePosZ;
  s32 min = -25000;
  s32 max = _GetBitMaxNum( 16 );
  s32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->pokePosZ = next;
}

/**
* @brief   描画：不正データ設定「」
*/
wchar_t* DebugClass::DrawIllegalParameter_PosDegPokePosZ( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->pokePosZ );

  return workStr;
}


/**
* @brief   更新：不正データ設定「」
*/
void DebugClass::UpdateIllegalParameter_PosDegPokeDeg( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->pokeDegY;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 9 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->pokeDegY = next;
}

/**
* @brief   描画：不正データ設定「」
*/
wchar_t* DebugClass::DrawIllegalParameter_PosDegPokeDeg( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->pokeDegY );

  return workStr;
}


/**
* @brief   更新：不正データ設定「」
*/
void DebugClass::UpdateIllegalParameter_PosDegTrainerDeg( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->trainerDegY;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 9 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->trainerDegY = next;
}

/**
* @brief   描画：不正データ設定「」
*/
wchar_t* DebugClass::DrawIllegalParameter_PosDegTrainerDeg( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->trainerDegY );

  return workStr;
}




//////////////////////////////////////////////////////////////////////////
///
///   @brief    不正データ設定：カメラ
///
//////////////////////////////////////////////////////////////////////////
/**
* @brief   更新：不正データ設定「」
*/
void DebugClass::UpdateIllegalParameter_CameraHeight( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->cameraHeight;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 7 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->cameraHeight = next;
}

/**
* @brief   描画：不正データ設定「」
*/
wchar_t* DebugClass::DrawIllegalParameter_CameraHeight( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->cameraHeight );

  return workStr;
}


/**
* @brief   更新：不正データ設定「」
*/
void DebugClass::UpdateIllegalParameter_CameraDegAxisY( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->cameraDegY;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 9 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->cameraDegY = next;
}

/**
* @brief   描画：不正データ設定「」
*/
wchar_t* DebugClass::DrawIllegalParameter_CameraDegAxisY( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->cameraDegY );

  return workStr;
}


/**
* @brief   更新：不正データ設定「」
*/
void DebugClass::UpdateIllegalParameter_CameraDegAxisX( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->cameraDegX;
  u32 min = 1;
  u32 max = _GetBitMaxNum( 7 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->cameraDegX = next;
}

/**
* @brief   描画：不正データ設定「」
*/
wchar_t* DebugClass::DrawIllegalParameter_CameraDegAxisX( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->cameraDegX );

  return workStr;
}


/**
* @brief   更新：不正データ設定「」
*/
void DebugClass::UpdateIllegalParameter_CameraAngle( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->cameraFov;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 7 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->cameraFov = next;
}

/**
* @brief   描画：不正データ設定「」
*/
wchar_t* DebugClass::DrawIllegalParameter_CameraAngle( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->cameraFov );

  return workStr;
}


/**
* @brief   更新：不正データ設定「」
*/
void DebugClass::UpdateIllegalParameter_CameraCoefficient( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->cameraFocusFactor;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 7 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->cameraFocusFactor = next;
}

/**
* @brief   描画：不正データ設定「」
*/
wchar_t* DebugClass::DrawIllegalParameter_CameraCoefficient( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->cameraFocusFactor );

  return workStr;
}




//////////////////////////////////////////////////////////////////////////
///
///   @brief    不正データ設定：その他
///
//////////////////////////////////////////////////////////////////////////
/**
* @brief   更新：不正データ設定「」
*/
void DebugClass::UpdateIllegalParameter_OtherCaptureMode( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->captureMode;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 1 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->captureMode = next;
}

/**
* @brief   描画：不正データ設定「」
*/
wchar_t* DebugClass::DrawIllegalParameter_OtherCaptureMode( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->captureMode );

  return workStr;
}


/**
* @brief   更新：不正データ設定「」
*/
void DebugClass::UpdateIllegalParameter_OtherComposition( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  u32 now = photoParam->composition;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 1 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  photoParam->composition = next;
}

/**
* @brief   描画：不正データ設定「」
*/
wchar_t* DebugClass::DrawIllegalParameter_OtherComposition( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Viewer::PhotoParam* photoParam = _GetIllegalParameter_PhotoParam();

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), photoParam->composition );

  return workStr;
}




//////////////////////////////////////////////////////////////////////////
///
///   @brief    不正データ設定：スタンプ
///
//////////////////////////////////////////////////////////////////////////
/**
* @brief   更新：不正データ設定「スタンプ１：種類」
*/
void DebugClass::UpdateIllegalParameter_Stamp1ID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 0 );

  u32 now = stampData->id;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 8 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->id = next;
}

/**
* @brief   描画：不正データ設定「スタンプ１：種類」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp1ID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 0 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->id );

  return workStr;
}


/**
* @brief   更新：不正データ設定「スタンプ１：Ｘ座標」
*/
void DebugClass::UpdateIllegalParameter_Stamp1X( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 0 );

  u32 now = stampData->x;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 9 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->x = next;
}

/**
* @brief   描画：不正データ設定「スタンプ１：Ｘ座標」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp1X( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 0 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->x );

  return workStr;
}


/**
* @brief   更新：不正データ設定「スタンプ１：Ｙ座標」
*/
void DebugClass::UpdateIllegalParameter_Stamp1Y( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 0 );

  u32 now = stampData->y;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 9 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->y = next;
}

/**
* @brief   描画：不正データ設定「スタンプ１：Ｙ座標」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp1Y( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 0 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->y );

  return workStr;
}


/**
* @brief   更新：不正データ設定「スタンプ１：フレーム」
*/
void DebugClass::UpdateIllegalParameter_Stamp1Frame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 0 );

  u32 now = stampData->frame_id;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 9 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->frame_id = next;
}

/**
* @brief   描画：不正データ設定「スタンプ１：フレーム」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp1Frame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 0 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->frame_id );

  return workStr;
}


/**
* @brief   更新：不正データ設定「スタンプ２：種類」
*/
void DebugClass::UpdateIllegalParameter_Stamp2ID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 1 );

  u32 now = stampData->id;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 8 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->id = next;
}

/**
* @brief   描画：不正データ設定「スタンプ２：種類」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp2ID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 1 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->id );

  return workStr;
}


/**
* @brief   更新：不正データ設定「スタンプ２：Ｘ座標」
*/
void DebugClass::UpdateIllegalParameter_Stamp2X( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 1 );

  u32 now = stampData->x;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 9 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->x = next;
}

/**
* @brief   描画：不正データ設定「スタンプ２：Ｘ座標」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp2X( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 1 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->x );

  return workStr;
}


/**
* @brief   更新：不正データ設定「スタンプ２：Ｙ座標」
*/
void DebugClass::UpdateIllegalParameter_Stamp2Y( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 1 );

  u32 now = stampData->y;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 9 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->y = next;
}

/**
* @brief   描画：不正データ設定「スタンプ２：Ｙ座標」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp2Y( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 1 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->y );

  return workStr;
}


/**
* @brief   更新：不正データ設定「スタンプ２：フレーム」
*/
void DebugClass::UpdateIllegalParameter_Stamp2Frame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 1 );

  u32 now = stampData->frame_id;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 9 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->frame_id = next;
}

/**
* @brief   描画：不正データ設定「スタンプ２：フレーム」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp2Frame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 1 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->frame_id );

  return workStr;
}


/**
* @brief   更新：不正データ設定「スタンプ３：種類」
*/
void DebugClass::UpdateIllegalParameter_Stamp3ID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 2 );

  u32 now = stampData->id;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 8 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->id = next;
}

/**
* @brief   描画：不正データ設定「スタンプ３：種類」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp3ID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 2 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->id );

  return workStr;
}


/**
* @brief   更新：不正データ設定「スタンプ３：Ｘ座標」
*/
void DebugClass::UpdateIllegalParameter_Stamp3X( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 2 );

  u32 now = stampData->x;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 9 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->x = next;
}

/**
* @brief   描画：不正データ設定「スタンプ３：Ｘ座標」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp3X( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 2 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->x );

  return workStr;
}


/**
* @brief   更新：不正データ設定「スタンプ３：Ｙ座標」
*/
void DebugClass::UpdateIllegalParameter_Stamp3Y( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 2 );

  u32 now = stampData->y;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 9 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->y = next;
}

/**
* @brief   描画：不正データ設定「スタンプ３：Ｙ座標」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp3Y( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 2 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->y );

  return workStr;
}


/**
* @brief   更新：不正データ設定「スタンプ３：フレーム」
*/
void DebugClass::UpdateIllegalParameter_Stamp3Frame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 2 );

  u32 now = stampData->frame_id;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 9 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->frame_id = next;
}

/**
* @brief   描画：不正データ設定「スタンプ３：フレーム」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp3Frame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 2 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->frame_id );

  return workStr;
}


/**
* @brief   更新：不正データ設定「スタンプ４：種類」
*/
void DebugClass::UpdateIllegalParameter_Stamp4ID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 3 );

  u32 now = stampData->id;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 8 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->id = next;
}

/**
* @brief   描画：不正データ設定「スタンプ４：種類」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp4ID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 3 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->id );

  return workStr;
}


/**
* @brief   更新：不正データ設定「スタンプ４：Ｘ座標」
*/
void DebugClass::UpdateIllegalParameter_Stamp4X( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 3 );

  u32 now = stampData->x;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 9 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->x = next;
}

/**
* @brief   描画：不正データ設定「スタンプ４：Ｘ座標」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp4X( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 3 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->x );

  return workStr;
}


/**
* @brief   更新：不正データ設定「スタンプ４：Ｙ座標」
*/
void DebugClass::UpdateIllegalParameter_Stamp4Y( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 3 );

  u32 now = stampData->y;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 9 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->y = next;
}

/**
* @brief   描画：不正データ設定「スタンプ４：Ｙ座標」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp4Y( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 3 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->y );

  return workStr;
}


/**
* @brief   更新：不正データ設定「スタンプ４：フレーム」
*/
void DebugClass::UpdateIllegalParameter_Stamp4Frame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 3 );

  u32 now = stampData->frame_id;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 9 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->frame_id = next;
}

/**
* @brief   描画：不正データ設定「スタンプ４：フレーム」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp4Frame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 3 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->frame_id );

  return workStr;
}


/**
* @brief   更新：不正データ設定「スタンプ５：種類」
*/
void DebugClass::UpdateIllegalParameter_Stamp5ID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 4 );

  u32 now = stampData->id;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 8 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->id = next;
}

/**
* @brief   描画：不正データ設定「スタンプ５：種類」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp5ID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 4 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->id );

  return workStr;
}


/**
* @brief   更新：不正データ設定「スタンプ５：Ｘ座標」
*/
void DebugClass::UpdateIllegalParameter_Stamp5X( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 4 );

  u32 now = stampData->x;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 9 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->x = next;
}

/**
* @brief   描画：不正データ設定「スタンプ５：Ｘ座標」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp5X( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 4 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->x );

  return workStr;
}


/**
* @brief   更新：不正データ設定「スタンプ５：Ｙ座標」
*/
void DebugClass::UpdateIllegalParameter_Stamp5Y( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 4 );

  u32 now = stampData->y;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 9 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->y = next;
}

/**
* @brief   描画：不正データ設定「スタンプ５：Ｙ座標」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp5Y( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 4 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->y );

  return workStr;
}


/**
* @brief   更新：不正データ設定「スタンプ５：フレーム」
*/
void DebugClass::UpdateIllegalParameter_Stamp5Frame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 4 );

  u32 now = stampData->frame_id;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 9 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->frame_id = next;
}

/**
* @brief   描画：不正データ設定「スタンプ５：フレーム」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp5Frame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 4 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->frame_id );

  return workStr;
}


/**
* @brief   更新：不正データ設定「スタンプ６：種類」
*/
void DebugClass::UpdateIllegalParameter_Stamp6ID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 5 );

  u32 now = stampData->id;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 8 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->id = next;
}

/**
* @brief   描画：不正データ設定「スタンプ６：種類」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp6ID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 5 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->id );

  return workStr;
}


/**
* @brief   更新：不正データ設定「スタンプ６：Ｘ座標」
*/
void DebugClass::UpdateIllegalParameter_Stamp6X( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 5 );

  u32 now = stampData->x;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 9 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->x = next;
}

/**
* @brief   描画：不正データ設定「スタンプ６：Ｘ座標」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp6X( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 5 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->x );

  return workStr;
}


/**
* @brief   更新：不正データ設定「スタンプ６：Ｙ座標」
*/
void DebugClass::UpdateIllegalParameter_Stamp6Y( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 5 );

  u32 now = stampData->y;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 9 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->y = next;
}

/**
* @brief   描画：不正データ設定「スタンプ６：Ｙ座標」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp6Y( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 5 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->y );

  return workStr;
}


/**
* @brief   更新：不正データ設定「スタンプ６：フレーム」
*/
void DebugClass::UpdateIllegalParameter_Stamp6Frame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 5 );

  u32 now = stampData->frame_id;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 9 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->frame_id = next;
}

/**
* @brief   描画：不正データ設定「スタンプ６：フレーム」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp6Frame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 5 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->frame_id );

  return workStr;
}


/**
* @brief   更新：不正データ設定「スタンプ７：種類」
*/
void DebugClass::UpdateIllegalParameter_Stamp7ID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 6 );

  u32 now = stampData->id;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 8 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->id = next;
}

/**
* @brief   描画：不正データ設定「スタンプ７：種類」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp7ID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 6 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->id );

  return workStr;
}


/**
* @brief   更新：不正データ設定「スタンプ７：Ｘ座標」
*/
void DebugClass::UpdateIllegalParameter_Stamp7X( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 6 );

  u32 now = stampData->x;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 9 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->x = next;
}

/**
* @brief   描画：不正データ設定「スタンプ７：Ｘ座標」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp7X( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 6 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->x );

  return workStr;
}


/**
* @brief   更新：不正データ設定「スタンプ７：Ｙ座標」
*/
void DebugClass::UpdateIllegalParameter_Stamp7Y( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 6 );

  u32 now = stampData->y;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 9 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->y = next;
}

/**
* @brief   描画：不正データ設定「スタンプ７：Ｙ座標」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp7Y( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 6 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->y );

  return workStr;
}


/**
* @brief   更新：不正データ設定「スタンプ７：フレーム」
*/
void DebugClass::UpdateIllegalParameter_Stamp7Frame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 6 );

  u32 now = stampData->frame_id;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 9 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->frame_id = next;
}

/**
* @brief   描画：不正データ設定「スタンプ７：フレーム」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp7Frame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 6 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->frame_id );

  return workStr;
}


/**
* @brief   更新：不正データ設定「スタンプ８：種類」
*/
void DebugClass::UpdateIllegalParameter_Stamp8ID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 7 );

  u32 now = stampData->id;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 8 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->id = next;
}

/**
* @brief   描画：不正データ設定「スタンプ８：種類」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp8ID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 7 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->id );

  return workStr;
}


/**
* @brief   更新：不正データ設定「スタンプ８：Ｘ座標」
*/
void DebugClass::UpdateIllegalParameter_Stamp8X( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 7 );

  u32 now = stampData->x;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 9 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->x = next;
}

/**
* @brief   描画：不正データ設定「スタンプ８：Ｘ座標」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp8X( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 7 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->x );

  return workStr;
}


/**
* @brief   更新：不正データ設定「スタンプ８：Ｙ座標」
*/
void DebugClass::UpdateIllegalParameter_Stamp8Y( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 7 );

  u32 now = stampData->y;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 9 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->y = next;
}

/**
* @brief   描画：不正データ設定「スタンプ８：Ｙ座標」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp8Y( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 7 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->y );

  return workStr;
}


/**
* @brief   更新：不正データ設定「スタンプ８：フレーム」
*/
void DebugClass::UpdateIllegalParameter_Stamp8Frame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 7 );

  u32 now = stampData->frame_id;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 9 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->frame_id = next;
}

/**
* @brief   描画：不正データ設定「スタンプ８：フレーム」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp8Frame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 7 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->frame_id );

  return workStr;
}


/**
* @brief   更新：不正データ設定「スタンプ９：種類」
*/
void DebugClass::UpdateIllegalParameter_Stamp9ID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 8 );

  u32 now = stampData->id;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 8 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->id = next;
}

/**
* @brief   描画：不正データ設定「スタンプ９：種類」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp9ID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 8 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->id );

  return workStr;
}


/**
* @brief   更新：不正データ設定「スタンプ９：Ｘ座標」
*/
void DebugClass::UpdateIllegalParameter_Stamp9X( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 8 );

  u32 now = stampData->x;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 9 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->x = next;
}

/**
* @brief   描画：不正データ設定「スタンプ９：Ｘ座標」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp9X( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 8 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->x );

  return workStr;
}


/**
* @brief   更新：不正データ設定「スタンプ９：Ｙ座標」
*/
void DebugClass::UpdateIllegalParameter_Stamp9Y( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 8 );

  u32 now = stampData->y;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 9 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->y = next;
}

/**
* @brief   描画：不正データ設定「スタンプ９：Ｙ座標」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp9Y( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 8 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->y );

  return workStr;
}


/**
* @brief   更新：不正データ設定「スタンプ９：フレーム」
*/
void DebugClass::UpdateIllegalParameter_Stamp9Frame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 8 );

  u32 now = stampData->frame_id;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 9 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->frame_id = next;
}

/**
* @brief   描画：不正データ設定「スタンプ９：フレーム」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp9Frame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 8 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->frame_id );

  return workStr;
}


/**
* @brief   更新：不正データ設定「スタンプ１０：種類」
*/
void DebugClass::UpdateIllegalParameter_Stamp10ID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 9 );

  u32 now = stampData->id;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 8 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->id = next;
}

/**
* @brief   描画：不正データ設定「スタンプ１０：種類」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp10ID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 9 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->id );

  return workStr;
}


/**
* @brief   更新：不正データ設定「スタンプ１０：Ｘ座標」
*/
void DebugClass::UpdateIllegalParameter_Stamp10X( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 9 );

  u32 now = stampData->x;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 9 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->x = next;
}

/**
* @brief   描画：不正データ設定「スタンプ１０：Ｘ座標」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp10X( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 9 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->x );

  return workStr;
}


/**
* @brief   更新：不正データ設定「スタンプ１０：Ｙ座標」
*/
void DebugClass::UpdateIllegalParameter_Stamp10Y( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 9 );

  u32 now = stampData->y;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 9 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->y = next;
}

/**
* @brief   描画：不正データ設定「スタンプ１０：Ｙ座標」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp10Y( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 9 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->y );

  return workStr;
}


/**
* @brief   更新：不正データ設定「スタンプ１０：フレーム」
*/
void DebugClass::UpdateIllegalParameter_Stamp10Frame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 9 );

  u32 now = stampData->frame_id;
  u32 min = 0;
  u32 max = _GetBitMaxNum( 9 );
  u32 next = 0;

  /// パラメータ操作
  next = _SelectNextParameter( now, min, max );

  /// パラメータ更新
  stampData->frame_id = next;
}

/**
* @brief   描画：不正データ設定「スタンプ１０：フレーム」
*/
wchar_t* DebugClass::DrawIllegalParameter_Stamp10Frame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  App::FinderStudio::Static::StampData* stampData = _GetIllegalParameter_StampData( 9 );

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), stampData->frame_id );

  return workStr;
}




//////////////////////////////////////////////////////////////////////////
///
///   @brief    不正データ設定：データ反映
///
//////////////////////////////////////////////////////////////////////////
/**
* @brief   更新：不正データ設定「データ反映」
*/
void DebugClass::UpdateIllegalParameter_ReflectParam( void* userWork, gfl2::debug::DebugWinItem* item )
{
  gfl2::ui::Button* button = gfl2::debug::DebugWin_GetUiButton();

  if( button->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    m_bPhotoReproduct = true;
  }
}

/**
* @brief   描画：不正データ設定「データ反映」
*/
wchar_t* DebugClass::DrawIllegalParameter_ReflectParam( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls", item->GetName() );

  return workStr;
}




//////////////////////////////////////////////////////////////////////////
///
///   @brief    不正データ設定：写真共有
///
//////////////////////////////////////////////////////////////////////////
/**
* @brief   更新：不正データ設定「写真共有」
*/
void DebugClass::UpdateIllegalParameter_PhotoShare( void* userWork, gfl2::debug::DebugWinItem* item )
{
  gfl2::ui::Button* button = gfl2::debug::DebugWin_GetUiButton();

  if( button->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    /// 共有する写真データを設定
    GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFinderStudioSave()->SetSendPhotoData( m_IllegalPhotoParam );
    /// 通信を行う
#if defined(GF_PLATFORM_CTR)
    GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPacketManager )->SetPacketGameStatusWithMe();
    GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPacketManager )->SendMyData();
#endif
  }
}

/**
* @brief   描画：不正データ設定「写真共有」
*/
wchar_t* DebugClass::DrawIllegalParameter_PhotoShare( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls", item->GetName() );

  return workStr;
}




//////////////////////////////////////////////////////////////////////////
///
///   @brief    データ出力
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   更新：データ出力「正規データ」
 */
void DebugClass::UpdateOutputParameter_ReadParameter( void* userWork, gfl2::debug::DebugWinItem* item )
{
  gfl2::ui::Button* button = gfl2::debug::DebugWin_GetUiButton();

  if( button->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    _OutputPhotoParam( m_ReadPhotoParam );
  }
}

/**
 * @brief   描画：データ出力「正規データ」
 */
wchar_t* DebugClass::DrawOutputParameter_ReadParameter( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls", item->GetName() );

  return workStr;
}


/**
 * @brief   更新：データ出力「不正データ」
 */
void DebugClass::UpdateOutputParameter_IllegalParameter( void* userWork, gfl2::debug::DebugWinItem* item )
{
  gfl2::ui::Button* button = gfl2::debug::DebugWin_GetUiButton();

  if( button->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    _OutputPhotoParam( m_IllegalPhotoParam );
  }
}

/**
 * @brief   描画：データ出力「不正データ」
 */
wchar_t* DebugClass::DrawOutputParameter_IllegalParameter( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls", item->GetName() );

  return workStr;
}


/**
 * @brief   更新：データ出力「表示データ」
 */
void DebugClass::UpdateOutputParameter_ViewerParameter( void* userWork, gfl2::debug::DebugWinItem* item )
{
  gfl2::ui::Button* button = gfl2::debug::DebugWin_GetUiButton();

  if( button->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    if( m_pViewer )
    {
      m_pViewer->SavePhotoParam( &m_ViewerPhotoParam.photo_data.photo_param );
      _OutputPhotoParam( m_ViewerPhotoParam );
    }
  }
}

/**
 * @brief   描画：データ出力「表示データ」
 */
wchar_t* DebugClass::DrawOutputParameter_ViewerParameter( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls", item->GetName() );

  return workStr;
}




//////////////////////////////////////////////////////////////////////////
///
///   @brief    その他
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   SDカードから読み込んだ写真データの設定
 */
void DebugClass::SetReadPhotoParam( const App::FinderStudio::Static::IntegratedData* param )
{
  if( param )
  {
    gfl2::std::MemCopy( param, &m_ReadPhotoParam, sizeof( App::FinderStudio::Static::IntegratedData ) );
    gfl2::std::MemCopy( param, &m_IllegalPhotoParam, sizeof( App::FinderStudio::Static::IntegratedData ) );
    gfl2::std::MemCopy( param, &m_ViewerPhotoParam, sizeof( App::FinderStudio::Static::IntegratedData ) );
  }
}




//////////////////////////////////////////////////////////////////////////
///
///   @brief    パラメータ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   パラメータ加算値の取得
 */
u32 DebugClass::_GetAddParameter( void )
{
  gfl2::ui::Button* button = gfl2::debug::DebugWin_GetUiButton();

  u32 addNum = 1;

  if( button->IsHold( gfl2::ui::BUTTON_L ) && button->IsHold( gfl2::ui::BUTTON_R ) )
  {
    addNum = 100;
  }
  else if( button->IsHold( gfl2::ui::BUTTON_L ) || button->IsHold( gfl2::ui::BUTTON_R ) )
  {
    addNum = 10;
  }

  return addNum;
}


/**
 * @brief   パラメータ操作：u32版
 */
u32 DebugClass::_SelectNextParameter( u32 now, u32 min, u32 max )
{
  gfl2::ui::Button* button = gfl2::debug::DebugWin_GetUiButton();

  /// 返却値
  u32 retNum = 0;
  /// 加算値
  u32 addNum = _GetAddParameter();

  if( button->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    if( now <= addNum )
    {
      retNum = min;
    }
    else {
      retNum = now - addNum;
    }
  }
  else if( button->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    if( ( now + addNum ) >= max )
    {
      retNum = max;
    }
    else {
      retNum = now + addNum;
    }
  }
  else {
    retNum = now;
  }

  return retNum;
}

/**
 * @brief   パラメータ操作：s32版
 */
s32 DebugClass::_SelectNextParameter( s32 now, s32 min, s32 max )
{
  gfl2::ui::Button* button = gfl2::debug::DebugWin_GetUiButton();

  /// 返却値
  s32 retNum = 0;
  /// 加算値
  s32 addNum = static_cast<s32>( _GetAddParameter() );

  if( button->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    if( now <= addNum )
    {
      retNum = min;
    }
    else {
      retNum = now - addNum;
    }
  }
  else if( button->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    if( ( now + addNum ) >= max )
    {
      retNum = max;
    }
    else {
      retNum = now + addNum;
    }
  }
  else {
    retNum = now;
  }

  return retNum;
}

/**
 * @brief   パラメータ最大数の取得
 *
 * @param[in]   bitNum    ビット数
 */
u32 DebugClass::_GetBitMaxNum( u32 bitNum )
{
  return ( 1 << bitNum ) - 1;
}




//////////////////////////////////////////////////////////////////////////
///
///   @brief    出力
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   写真データの出力
 */
void DebugClass::_OutputPhotoParam( const App::FinderStudio::Static::IntegratedData param )
{
  App::FinderStudio::Viewer::PhotoParam photoParam = param.photo_data.photo_param;
  {
    GFL_RELEASE_PRINT( "ポケモンデータ\n" );
    GFL_RELEASE_PRINT( "モンスターNo：[%d]\n", photoParam.monsNo );
    GFL_RELEASE_PRINT( "フォルムNo：[%d]\n", photoParam.formNo );
    GFL_RELEASE_PRINT( "性別：[%d]\n", photoParam.sexPoke );
    GFL_RELEASE_PRINT( "色違い：[%d]\n", photoParam.isRare );
    GFL_RELEASE_PRINT( "\n" );

    GFL_RELEASE_PRINT( "着せ替えデータ\n" );
    GFL_RELEASE_PRINT( "性別：[%d]\n", photoParam.sexTrainer );
    GFL_RELEASE_PRINT( "体の色：[%d]\n", photoParam.skinColor );
    GFL_RELEASE_PRINT( "目の色：[%d]\n", photoParam.eyeColor );
    GFL_RELEASE_PRINT( "髪の色：[%d]\n", photoParam.hairColor );
    GFL_RELEASE_PRINT( "リップの色：[%d]\n", photoParam.lipColor );
    GFL_RELEASE_PRINT( "髪型：[%d]\n", photoParam.hairStyle );
    GFL_RELEASE_PRINT( "アイアクセサリ：[%d]\n", photoParam.eyeAccessory );
    GFL_RELEASE_PRINT( "ヘアアクセサリ：[%d]\n", photoParam.hairAccessory );
    GFL_RELEASE_PRINT( "バッグ：[%d]\n", photoParam.bag );
    GFL_RELEASE_PRINT( "ボトムス：[%d]\n", photoParam.bottoms );
    GFL_RELEASE_PRINT( "帽子：[%d]\n", photoParam.hat );
    GFL_RELEASE_PRINT( "レッグ：[%d]\n", photoParam.leg );
    GFL_RELEASE_PRINT( "シューズ：[%d]\n", photoParam.shoes );
    GFL_RELEASE_PRINT( "トップス：[%d]\n", photoParam.tops );
    GFL_RELEASE_PRINT( "シチュエーション着替え：[%d]\n", photoParam.situ );
    GFL_RELEASE_PRINT( "\n" );

    GFL_RELEASE_PRINT( "背景\n" );
    GFL_RELEASE_PRINT( "地面ループアニメーションフレーム：[%d]\n", photoParam.bgGroundLoopAnmFrame );
    GFL_RELEASE_PRINT( "近景ループアニメーションフレーム：[%d]\n", photoParam.bgNearLoopAnmFrame );
    GFL_RELEASE_PRINT( "遠景ループアニメーションフレーム：[%d]\n", photoParam.bgFarLoopAnmFrame );
    GFL_RELEASE_PRINT( "頂点モデルループアニメーションフレーム：[%d]\n", photoParam.bgBuildLoopAnmFrame );
    GFL_RELEASE_PRINT( "戦闘背景の時間アニメーションフレーム：[%d]\n", photoParam.bgTimeAnmFrame );
    GFL_RELEASE_PRINT( "天球の雲のアニメーションフレーム：[%d]\n", photoParam.skyCloudAnmFrame );
    GFL_RELEASE_PRINT( "天球の太陽のギラギラアニメーションフレーム：[%d]\n", photoParam.skySunAnmFrame );
    GFL_RELEASE_PRINT( "表示していた背景セットインデックス：[%d]\n", photoParam.bgSetIndex );
    GFL_RELEASE_PRINT( "戦闘背景の時間アニメーション種：[%d]\n", photoParam.bgTimeAnmType );
    GFL_RELEASE_PRINT( "天球の月の満ち欠けのパターン：[%d]\n", photoParam.skyMoonPattern );
    GFL_RELEASE_PRINT( "時間帯：[%d]\n", photoParam.timeZone );
    GFL_RELEASE_PRINT( "\n" );

    GFL_RELEASE_PRINT( "ポケモンモーション\n" );
    GFL_RELEASE_PRINT( "モーション種：[%d]\n", photoParam.pokeMotionType );
    GFL_RELEASE_PRINT( "モーションNo：[%d]\n", photoParam.pokeMotionNo );
    GFL_RELEASE_PRINT( "通常モーションフレーム：[%d]\n", photoParam.pokeMainMotionFrame );
    GFL_RELEASE_PRINT( "目パチアニメーションパターン１：[%d]\n", photoParam.pokeEyeAnmPattern1 );
    GFL_RELEASE_PRINT( "目パチアニメーションパターン２：[%d]\n", photoParam.pokeEyeAnmPattern2 );
    GFL_RELEASE_PRINT( "目パチアニメーションパターン３：[%d]\n", photoParam.pokeEyeAnmPattern3 );
    GFL_RELEASE_PRINT( "口アニメーションパターン１：[%d]\n", photoParam.pokeMouthAnmpattern1 );
    GFL_RELEASE_PRINT( "口アニメーションパターン２：[%d]\n", photoParam.pokeMouthAnmpattern2 );
    GFL_RELEASE_PRINT( "口アニメーションパターン３：[%d]\n", photoParam.pokeMouthAnmpattern3 );
    GFL_RELEASE_PRINT( "ループアニメーションフレーム１：[%d]\n", photoParam.pokeLoopAnmFrame1 );
    GFL_RELEASE_PRINT( "ループアニメーションフレーム２：[%d]\n", photoParam.pokeLoopAnmFrame2 );
    GFL_RELEASE_PRINT( "ループアニメーションフレーム３：[%d]\n", photoParam.pokeLoopAnmFrame3 );
    GFL_RELEASE_PRINT( "ループアニメーションフレーム４：[%d]\n", photoParam.pokeLoopAnmFrame4 );
    GFL_RELEASE_PRINT( "\n" );

    GFL_RELEASE_PRINT( "トレーナーモーション\n" );
    GFL_RELEASE_PRINT( "モーション種：[%d]\n", photoParam.trainerMotionType );
    GFL_RELEASE_PRINT( "モーションフレーム：[%d]\n", photoParam.trainerMotionFrame );
    GFL_RELEASE_PRINT( "目パチ状態：[%d]\n", photoParam.trainerEyeAnmPattern );
    GFL_RELEASE_PRINT( "\n" );

    GFL_RELEASE_PRINT( "座標・角度\n" );
    GFL_RELEASE_PRINT( "ポケモンX座標：[%d]\n", photoParam.pokePosX );
    GFL_RELEASE_PRINT( "ポケモンZ座標：[%d]\n", photoParam.pokePosZ );
    GFL_RELEASE_PRINT( "ポケモン角度：[%d]\n", photoParam.pokeDegY );
    GFL_RELEASE_PRINT( "トレーナー角度：[%d]\n", photoParam.trainerDegY );
    GFL_RELEASE_PRINT( "\n" );

    GFL_RELEASE_PRINT( "カメラ\n" );
    GFL_RELEASE_PRINT( "カメラ高さ：[%d]\n", photoParam.cameraHeight );
    GFL_RELEASE_PRINT( "カメラY軸回転：[%d]\n", photoParam.cameraDegY );
    GFL_RELEASE_PRINT( "カメラX軸回転：[%d]\n", photoParam.cameraDegX );
    GFL_RELEASE_PRINT( "画角：[%d]\n", photoParam.cameraFov );
    GFL_RELEASE_PRINT( "カメラ係数：[%d]\n", photoParam.cameraFocusFactor );
    GFL_RELEASE_PRINT( "\n" );

    GFL_RELEASE_PRINT( "その他\n" );
    GFL_RELEASE_PRINT( "撮影モード：[%d]\n", photoParam.captureMode );
    GFL_RELEASE_PRINT( "構図：[%d]\n", photoParam.composition );
    GFL_RELEASE_PRINT( "\n" );
  }

  App::FinderStudio::Static::DecorationData decoData = param.photo_data.deco_param;
  {
    for( u8 i = 0; i < App::FinderStudio::Static::DECO_ITEM_MAX; ++i )
    {
      App::FinderStudio::Static::StampData stampData = decoData.deco_param[i];
      {
        GFL_RELEASE_PRINT( "スタンプデータ[%d]\n", i+1 );
        GFL_RELEASE_PRINT( "スタンプの種類：[%d]\n", stampData.id );
        GFL_RELEASE_PRINT( "スタンプのX座標：[%d]\n", stampData.x );
        GFL_RELEASE_PRINT( "スタンプのY座標：[%d]\n", stampData.y );
        GFL_RELEASE_PRINT( "フレームの種類：[%d]\n", stampData.frame_id );
        GFL_RELEASE_PRINT( "\n" );
      }
    }
  }
}




//////////////////////////////////////////////////////////////////////////
///
///   @brief    取得
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   不正データ：写真用データの取得
 *
 * @retval  写真用データ
 */
App::FinderStudio::Static::PhotoData* DebugClass::_GetIllegalParameter_PhotoData( void )
{
  return &m_IllegalPhotoParam.photo_data;
}

/**
 * @brief   不正データ：写真データの取得
 *
 * @retval  写真データ
 */
App::FinderStudio::Viewer::PhotoParam* DebugClass::_GetIllegalParameter_PhotoParam( void )
{
  /// 写真用データの取得
  App::FinderStudio::Static::PhotoData* photoData = _GetIllegalParameter_PhotoData();
  /// 写真データを返す
  return &photoData->photo_param;
}

/**
 * @brief   不正データ：スタンプデータの取得
 *
 * @retval  スタンプデータ
 */
App::FinderStudio::Static::StampData* DebugClass::_GetIllegalParameter_StampData( u8 id )
{
  if( id >= App::FinderStudio::Static::DECO_ITEM_MAX )
  {
    GFL_ASSERT( 0 );
    id = 0;
  }

  /// 写真用データの取得
  App::FinderStudio::Static::PhotoData* photoData = _GetIllegalParameter_PhotoData();
  /// スタンプデータを返す
  return &photoData->deco_param.deco_param[id];
}

/// ---[ DebugClass end ]---
//////////////////////////////////////////////////////////////////////////

GFL_NAMESPACE_END( MyAlbum )
GFL_NAMESPACE_END( App )

#endif // PM_DEBUG
