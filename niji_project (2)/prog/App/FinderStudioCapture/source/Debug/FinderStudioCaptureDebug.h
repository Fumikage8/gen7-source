#if PM_DEBUG

#if !defined( __FINDER_STUDIO_CAPTURE_DEBUG_H_INCLUDED__ )
#define __FINDER_STUDIO_CAPTURE_DEBUG_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCaptureDebug.h
 * @date    2017/04/05 19:10:47
 * @author  fukushima_yuya
 * @brief   撮影アプリ用デバッグ機能
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "App/FinderStudioStatic/include/FinderStudioDefine.h"

#include <System/include/GflUse.h>

#include <debug/include/gfl2_DebugWinSystem.h>
#include <debug/include/gfl2_DebugWinItemTypes.h>
#include <GameSys/include/GameData.h>
#include <model/include/gfl2_DressUpParam.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
/// ---[ マクロ定義 begin ]---

#if !defined(DEBUG_UPDATE_FUNCTION)
#define DEBUG_UPDATE_FUNCTION( name ) static void Update##name( void* userWork, gfl2::debug::DebugWinItem* item );
#endif

#if !defined(DEBUG_DRAW_FUNCTION)
#define DEBUG_DRAW_FUNCTION( name ) static wchar_t* Draw##name( void* userWork, gfl2::debug::DebugWinItem* item );
#endif

#if !defined(DEBUG_FUNCTION_SET)
#define DEBUG_FUNCTION_SET( name ) \
  DEBUG_UPDATE_FUNCTION( name ); \
  DEBUG_DRAW_FUNCTION( name );
#endif

/// ---[ マクロ定義 end ]---
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///
/// @brief    デバッグクラス
///
//////////////////////////////////////////////////////////////////////////
class DebugClass
{
public:
  /// コンストラクタ
  DebugClass( void );
  /// デストラクタ
  ~DebugClass( void ) { ; }

public:
  /// 初期化
  void Initialize( gfl2::heap::HeapBase* heap );
  /// 開放
  void Terminate( void );

private:
  /// 初期化：自動撮影
  void InitDebugMenu_AutoCapture( void );
  /// データの削除
  void InitDebugMenu_DeleteData( void );
  /// 写真見比べ
  void InitDebugMenu_PhotoCompare( void );
  /// エージング
  void InitDebugMenu_Aging( void );

private:
  /// 更新・描画
  DEBUG_FUNCTION_SET( AutoCap_CaptureCount );
  DEBUG_FUNCTION_SET( AutoCap_StartMonsNo );
  DEBUG_FUNCTION_SET( AutoCap_RandomMonsNo );
  DEBUG_FUNCTION_SET( AutoCap_BackGroundID );
  DEBUG_FUNCTION_SET( AutoCap_BackGroundType );
  DEBUG_FUNCTION_SET( AutoCap_DressupHat );
  DEBUG_FUNCTION_SET( AutoCap_DressupHairAcce );
  DEBUG_FUNCTION_SET( AutoCap_DressupEyeAcce );
  DEBUG_FUNCTION_SET( AutoCap_DressupTops );
  DEBUG_FUNCTION_SET( AutoCap_DressupBottoms );
  DEBUG_FUNCTION_SET( AutoCap_DressupLeg );
  DEBUG_FUNCTION_SET( AutoCap_DressupShoes );
  DEBUG_FUNCTION_SET( AutoCap_DressupBag );
  DEBUG_FUNCTION_SET( AutoCap_DressupReset );
  DEBUG_FUNCTION_SET( AutoCap_RandomDressup );
  DEBUG_FUNCTION_SET( AutoCap_Composition );
  DEBUG_FUNCTION_SET( AutoCap_RandomDeco );
  DEBUG_FUNCTION_SET( AutoCap_RandomDate );
  DEBUG_FUNCTION_SET( AutoCap_Save );
  DEBUG_FUNCTION_SET( AutoCap_Start );
  DEBUG_FUNCTION_SET( AutoCap_Stop );

  DEBUG_FUNCTION_SET( DeleteData_AllExtSave );
  DEBUG_FUNCTION_SET( DeleteData_AllImageDBPhoto );

  DEBUG_FUNCTION_SET( PhotoCompare_StartFlg );
  DEBUG_DRAW_FUNCTION( PhotoCompare_PokeMonsNo );
  DEBUG_DRAW_FUNCTION( PhotoCompare_PokeFormNo );
  DEBUG_DRAW_FUNCTION( PhotoCompare_PokeSex );
  DEBUG_DRAW_FUNCTION( PhotoCompare_PokeRare );
  DEBUG_DRAW_FUNCTION( PhotoCompare_DressupSex );
  DEBUG_DRAW_FUNCTION( PhotoCompare_DressupBodyColor );
  DEBUG_DRAW_FUNCTION( PhotoCompare_DressupEyeColor );
  DEBUG_DRAW_FUNCTION( PhotoCompare_DressupHairColor );
  DEBUG_DRAW_FUNCTION( PhotoCompare_DressupLipColor );
  DEBUG_DRAW_FUNCTION( PhotoCompare_DressupHairStyle );
  DEBUG_DRAW_FUNCTION( PhotoCompare_DressupEyeAcce );
  DEBUG_DRAW_FUNCTION( PhotoCompare_DressupHairAcce );
  DEBUG_DRAW_FUNCTION( PhotoCompare_DressupBag );
  DEBUG_DRAW_FUNCTION( PhotoCompare_DressupBottoms );
  DEBUG_DRAW_FUNCTION( PhotoCompare_DressupHat );
  DEBUG_DRAW_FUNCTION( PhotoCompare_DressupLegs );
  DEBUG_DRAW_FUNCTION( PhotoCompare_DressupShoes );
  DEBUG_DRAW_FUNCTION( PhotoCompare_DressupTops );
  DEBUG_DRAW_FUNCTION( PhotoCompare_DressupSituation );
  DEBUG_DRAW_FUNCTION( PhotoCompare_BgGroundAnimFrame );
  DEBUG_DRAW_FUNCTION( PhotoCompare_BgNearAnimFrame );
  DEBUG_DRAW_FUNCTION( PhotoCompare_BgFarAnimFrame );
  DEBUG_DRAW_FUNCTION( PhotoCompare_BgVertexModelAnimFrame );
  DEBUG_DRAW_FUNCTION( PhotoCompare_BgTimeAnimFrame );
  DEBUG_DRAW_FUNCTION( PhotoCompare_BgCloudAnimFrame );
  DEBUG_DRAW_FUNCTION( PhotoCompare_BgSunAnimFrame );
  DEBUG_DRAW_FUNCTION( PhotoCompare_BgID );
  DEBUG_DRAW_FUNCTION( PhotoCompare_BgAnimID );
  DEBUG_DRAW_FUNCTION( PhotoCompare_BgMoonPattern );
  DEBUG_DRAW_FUNCTION( PhotoCompare_BgTimeZone );
  DEBUG_DRAW_FUNCTION( PhotoCompare_PokeMotionType );
  DEBUG_DRAW_FUNCTION( PhotoCompare_PokeMotionNo );
  DEBUG_DRAW_FUNCTION( PhotoCompare_PokeMotionFrame );
  DEBUG_DRAW_FUNCTION( PhotoCompare_PokeMotionBlink1 );
  DEBUG_DRAW_FUNCTION( PhotoCompare_PokeMotionBlink2 );
  DEBUG_DRAW_FUNCTION( PhotoCompare_PokeMotionBlink3 );
  DEBUG_DRAW_FUNCTION( PhotoCompare_PokeMotionLipSync1 );
  DEBUG_DRAW_FUNCTION( PhotoCompare_PokeMotionLipSync2 );
  DEBUG_DRAW_FUNCTION( PhotoCompare_PokeMotionLipSync3 );
  DEBUG_DRAW_FUNCTION( PhotoCompare_PokeMotionLoopAnimFrame1 );
  DEBUG_DRAW_FUNCTION( PhotoCompare_PokeMotionLoopAnimFrame2 );
  DEBUG_DRAW_FUNCTION( PhotoCompare_PokeMotionLoopAnimFrame3 );
  DEBUG_DRAW_FUNCTION( PhotoCompare_PokeMotionLoopAnimFrame4 );
  DEBUG_DRAW_FUNCTION( PhotoCompare_TrainerMotionID );
  DEBUG_DRAW_FUNCTION( PhotoCompare_TrainerMotionFrame );
  DEBUG_DRAW_FUNCTION( PhotoCompare_TrainerMotionBlink );
  DEBUG_DRAW_FUNCTION( PhotoCompare_PosDegPokePosX );
  DEBUG_DRAW_FUNCTION( PhotoCompare_PosDegPokePosZ );
  DEBUG_DRAW_FUNCTION( PhotoCompare_PosDegPokeDeg );
  DEBUG_DRAW_FUNCTION( PhotoCompare_PosDegTrainerDeg );
  DEBUG_DRAW_FUNCTION( PhotoCompare_CameraHeight );
  DEBUG_DRAW_FUNCTION( PhotoCompare_CameraDegAxisY );
  DEBUG_DRAW_FUNCTION( PhotoCompare_CameraDegAxisX );
  DEBUG_DRAW_FUNCTION( PhotoCompare_CameraAngle );
  DEBUG_DRAW_FUNCTION( PhotoCompare_CameraCoefficient );
  DEBUG_DRAW_FUNCTION( PhotoCompare_OtherCaptureMode );
  DEBUG_DRAW_FUNCTION( PhotoCompare_OtherComposition );

  DEBUG_FUNCTION_SET( AgingPokemon_StartMonsNo );
  DEBUG_FUNCTION_SET( AgingPokemon_FinishMonsNo );
  DEBUG_FUNCTION_SET( AgingPokemon_Mode );
  DEBUG_FUNCTION_SET( AgingPokemon_StartStop );
  DEBUG_FUNCTION_SET( AgingTrainer_Mode );
  DEBUG_FUNCTION_SET( AgingTrainer_StartStop );
  DEBUG_FUNCTION_SET( AgingBackGround_Mode );
  DEBUG_FUNCTION_SET( AgingBackGround_Change );
  DEBUG_FUNCTION_SET( AgingBackGround_Camera );
  DEBUG_FUNCTION_SET( AgingBackGround_StartStop );

public:
  /// デバッグウィンドウが開いているかどうか
  static bool IsDebugWinOpen( void );

public:
  /// 何かしらのデバッグモードが開始されたか
  static bool IsAnyDebugMode( void )
  {
    return IsAutoCapture_Start() || IsAging_Start();
  }

  /// 自動撮影の撮影枚数を取得
  static u32  GetAutoCapture_CaptureCount( void ) { return m_AutoCapCaptureCount; }
  /// 自動撮影の開始ポケモンIDを取得
  static u32  GetAutoCapture_StartMonsNo( void ) { return m_AutoCapStartMonsNo; }
  /// 自動撮影のポケモンランダム撮影フラグを取得
  static bool IsAutoCapture_RandomMonsNo( void ) { return m_bAutoCapRandomMonsNoFlg == 1; }
  /// 自動撮影の背景IDを取得
  static u32  GetAutoCapture_BackGroundID( void ) { return m_AutoCapBackGroundID; }
  /// 自動撮影の背景表示タイプを取得
  static u32  GetAutoCapture_BackGroundType( void ) { return m_AutoCapBackGroundType; }
  /// 着せ替え：帽子
  static s32  GetAutoCapture_DressupHat( void ) { return m_AutoCapDressupHat; }
  /// 着せ替え：ヘアアクセ
  static s32  GetAutoCapture_DressupHairAcce( void ) { return m_AutoCapDressupHairAcce; }
  /// 着せ替え：アイアクセ
  static s32  GetAutoCapture_DressupEyeAcce( void ) { return m_AutoCapDressupEyeAcce; }
  /// 着せ替え：トップス
  static s32  GetAutoCapture_DressupTops( void ) { return m_AutoCapDressupTops; }
  /// 着せ替え：ボトムス
  static s32  GetAutoCapture_DressupBottoms( void ) { return m_AutoCapDressupBottoms; }
  /// 着せ替え：レッグ
  static s32  GetAutoCapture_DressupLeg( void ) { return m_AutoCapDressupLeg; }
  /// 着せ替え：シューズ
  static s32  GetAutoCapture_DressupShoes( void ) { return m_AutoCapDressupShoes; }
  /// 着せ替え：バッグ
  static s32  GetAutoCapture_DressupBag( void ) { return m_AutoCapDressupBag; }
  /// 着せ替え：ランダム
  static bool IsAutoCapture_RandomDressup( void ) { return m_AutoCapDressupRandomFlg; }
  /// 構図
  static u8   GetAutoCapture_Composition( void ) { return m_AutoCapComposition; }
  /// デコのランダム
  static bool IsAutoCapture_RandomDecoration( void ) { return m_AutoCapDecoRandomFlg; }
  /// 撮影日時のランダム
  static bool IsAutoCapture_RandomDate( void ) { return m_AutoCapDateRandomFlg; }

  /// 保存フラグのチェック
  static bool IsAutoCapture_Save( void ) { return m_bAutoCaptureSaveFlg; }
  /// 保存フラグのリセット
  static void ResetAutoCapture_Save( void ) { m_bAutoCaptureSaveFlg = false; }

  /// 自動撮影開始チェック
  static bool IsAutoCapture_Start( void ) { return m_bAutoCaptureFlg; }
  /// 自動撮影開始フラグのリセット
  static void ResetAutoCapture_Start( void ) { m_bAutoCaptureFlg = false; }

  /// 自動撮影停止チェック
  static bool IsAutoCapture_Stop( void ) { return m_bAutoCaptureStopFlg; }
  /// 自動撮影停止フラグのリセット
  static void ResetAutoCapture_Stop( void ) { m_bAutoCaptureStopFlg = false; }

  /// フィールド衣装の取得
  static poke_3d::model::DressUpParam GetFieldDressup( void ) { return _GetFieldDressup(); }
  /// プレイヤーの性別判定
  static bool IsPlayerSexMale( void ) { return _IsPlayerSexMale(); }

private:
  /// 拡張セーブデータの全削除
  static void _DeleteData_AllExtSave( void );
  /// ImageDB管理の写真を全削除
  static void _DeleteData_AllImageDBPhoto( void );

public:
  /// 写真差分開始チェック
  static bool IsPhotoCompare_Start( void ) { return m_bPhotoCompareFlg; }
  /// 写真再現パラメータのセット
  static void SetPhotoCompare_PhotoParam( const Viewer::PhotoParam* param ) { m_pPhotoComparePhotoParam = param; }

public:
  /// エージング
  static bool IsAging_Start( void )
  {
    return IsAging_PokemonStart() || IsAging_TrainerStart() || IsAging_BackGroundStart();
  }
  /// エージング：ポケモン
  static u16  GetAging_PokemonStartMonsNo( void ) { return m_AgingPokemonStartMonsNo; }
  static u16  GetAging_PokemonFinishMonsNo( void ) { return m_AgingPokemonFinishMonsNo; }
  static u8   GetAging_PokemonMode( void ) { return m_AgingPokemonMode; }
  static bool IsAging_PokemonStart( void ) { return m_bAgingPokemonFlg; }
  /// エージング：トレーナー
  static u8   GetAging_TrainerMode( void ) { return m_AgingTrainerMode; }
  static bool IsAging_TrainerStart( void ) { return m_bAgingTrainerFlg; }
  /// エージング：背景
  static u8   GetAging_BackGroundMode( void ) { return m_AgingBackGroundMode; }
  static u8   GetAging_BackGroundChange( void ) { return m_AgingBackGroundChange; }
  static u8   GetAging_BackGroundCamera( void ) { return m_AgingBackGroundCamera; }
  static bool IsAging_BackGroundStart( void ) { return m_bAgingBackGroundFlg; }

private:
  /// キー入力の取得
  static gfl2::ui::Button* _GetButton( void );
  /// MyStatusの取得
  static Savedata::MyStatus* _GetMyStatus( void );

  /// フィールド衣装の取得
  static poke_3d::model::DressUpParam _GetFieldDressup( void );
  /// プレイヤーの性別判定
  static bool _IsPlayerSexMale( void );

  /// 着せ替え共通更新処理
  static void _UpdateAutoCapDressup( s32* value, u32 itemMax );
  /// 着せ替え共通描画処理
  static wchar_t* _DrawAutoCapDressup( gfl2::debug::DebugWinItem* item, s32 value );

private:
  gfl2::heap::HeapBase*          m_pHeap;

  gfl2::debug::DebugWinGroup*    m_pRootGroup;

/// AutoCap
private:
  static u32   m_AutoCapCaptureCount;

  static u32   m_AutoCapStartMonsNo;

  static u8    m_bAutoCapRandomMonsNoFlg;

  static bool  m_bAutoCaptureFlg;

  static bool  m_bAutoCaptureStopFlg;

  static u32   m_AutoCapBackGroundID;

  static u32   m_AutoCapBackGroundType;

  static s32   m_AutoCapDressupHat;

  static s32   m_AutoCapDressupHairAcce;

  static s32   m_AutoCapDressupEyeAcce;

  static s32   m_AutoCapDressupTops;

  static s32   m_AutoCapDressupBottoms;

  static s32   m_AutoCapDressupLeg;

  static s32   m_AutoCapDressupShoes;

  static s32   m_AutoCapDressupBag;

  static bool  m_AutoCapDressupRandomFlg;

  static u8    m_AutoCapComposition;

  static bool  m_AutoCapDecoRandomFlg;

  static bool  m_AutoCapDateRandomFlg;

  static bool  m_bAutoCaptureSaveFlg;

/// PhotoCompare
private:
  static bool   m_bPhotoCompareFlg;

  static const Viewer::PhotoParam*  m_pPhotoComparePhotoParam;

/// Aging
private:
  static u16    m_AgingPokemonStartMonsNo;

  static u16    m_AgingPokemonFinishMonsNo;

  static u8     m_AgingPokemonMode;

  static bool   m_bAgingPokemonFlg;

  static u8     m_AgingTrainerMode;

  static bool   m_bAgingTrainerFlg;

  static u8     m_AgingBackGroundMode;

  static u8     m_AgingBackGroundChange;

  static u8     m_AgingBackGroundCamera;

  static bool   m_bAgingBackGroundFlg;
};

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif  // __FINDER_STUDIO_CAPTURE_DEBUG_H_INCLUDED__

#endif  // PM_DEBUG
