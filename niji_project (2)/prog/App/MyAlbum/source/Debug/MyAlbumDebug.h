#if PM_DEBUG

#if !defined(__MYALBUM_DEBUG_H_INCLUDED__)
#define __MYALBUM_DEBUG_H_INCLUDED__
//======================================================================
/**
 * @file    MyAlbumDebug.h
 * @date    2017/04/25 16:56:01
 * @author  fukushima_yuya
 * @brief   デバッグ機能
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <App/FinderStudioStatic/include/FinderStudioDefine.h>
#include <App/FinderStudioViewer/include/FinderStudioViewer.h>

#include <System/include/GflUse.h>

#include <debug/include/gfl2_DebugWinSystem.h>
#include <debug/include/gfl2_DebugWinItemTypes.h>
#include <GameSys/include/GameData.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( MyAlbum )

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
  /// 不正データ
  void InitDebugMenu_IllegalParameter( void );
  /// データ出力
  void InitDebugMenu_OutputParameter( void );

private:
  /// 更新・描画
  DEBUG_FUNCTION_SET( IllegalParameter_PokeMonsNo );
  DEBUG_FUNCTION_SET( IllegalParameter_PokeFormNo );
  DEBUG_FUNCTION_SET( IllegalParameter_PokeSex );
  DEBUG_FUNCTION_SET( IllegalParameter_PokeRare );
  DEBUG_FUNCTION_SET( IllegalParameter_DressupSex );
  DEBUG_FUNCTION_SET( IllegalParameter_DressupBodyColor );
  DEBUG_FUNCTION_SET( IllegalParameter_DressupEyeColor );
  DEBUG_FUNCTION_SET( IllegalParameter_DressupHairColor );
  DEBUG_FUNCTION_SET( IllegalParameter_DressupLipColor );
  DEBUG_FUNCTION_SET( IllegalParameter_DressupHairStyle );
  DEBUG_FUNCTION_SET( IllegalParameter_DressupEyeAcce );
  DEBUG_FUNCTION_SET( IllegalParameter_DressupHairAcce );
  DEBUG_FUNCTION_SET( IllegalParameter_DressupBag );
  DEBUG_FUNCTION_SET( IllegalParameter_DressupBottoms );
  DEBUG_FUNCTION_SET( IllegalParameter_DressupHat );
  DEBUG_FUNCTION_SET( IllegalParameter_DressupLegs );
  DEBUG_FUNCTION_SET( IllegalParameter_DressupShoes );
  DEBUG_FUNCTION_SET( IllegalParameter_DressupTops );
  DEBUG_FUNCTION_SET( IllegalParameter_DressupSituation );
  DEBUG_FUNCTION_SET( IllegalParameter_BgGroundAnimFrame );
  DEBUG_FUNCTION_SET( IllegalParameter_BgNearAnimFrame );
  DEBUG_FUNCTION_SET( IllegalParameter_BgFarAnimFrame );
  DEBUG_FUNCTION_SET( IllegalParameter_BgVertexModelAnimFrame );
  DEBUG_FUNCTION_SET( IllegalParameter_BgTimeAnimFrame );
  DEBUG_FUNCTION_SET( IllegalParameter_BgCloudAnimFrame );
  DEBUG_FUNCTION_SET( IllegalParameter_BgSunAnimFrame );
  DEBUG_FUNCTION_SET( IllegalParameter_BgID );
  DEBUG_FUNCTION_SET( IllegalParameter_BgAnimID );
  DEBUG_FUNCTION_SET( IllegalParameter_BgMoonPattern );
  DEBUG_FUNCTION_SET( IllegalParameter_BgTimeZone );
  DEBUG_FUNCTION_SET( IllegalParameter_PokeMotionType );
  DEBUG_FUNCTION_SET( IllegalParameter_PokeMotionNo );
  DEBUG_FUNCTION_SET( IllegalParameter_PokeMotionFrame );
  DEBUG_FUNCTION_SET( IllegalParameter_PokeMotionBlink1 );
  DEBUG_FUNCTION_SET( IllegalParameter_PokeMotionBlink2 );
  DEBUG_FUNCTION_SET( IllegalParameter_PokeMotionBlink3 );
  DEBUG_FUNCTION_SET( IllegalParameter_PokeMotionLipSync1 );
  DEBUG_FUNCTION_SET( IllegalParameter_PokeMotionLipSync2 );
  DEBUG_FUNCTION_SET( IllegalParameter_PokeMotionLipSync3 );
  DEBUG_FUNCTION_SET( IllegalParameter_PokeMotionLoopAnimFrame1 );
  DEBUG_FUNCTION_SET( IllegalParameter_PokeMotionLoopAnimFrame2 );
  DEBUG_FUNCTION_SET( IllegalParameter_PokeMotionLoopAnimFrame3 );
  DEBUG_FUNCTION_SET( IllegalParameter_PokeMotionLoopAnimFrame4 );
  DEBUG_FUNCTION_SET( IllegalParameter_TrainerMotionID );
  DEBUG_FUNCTION_SET( IllegalParameter_TrainerMotionFrame );
  DEBUG_FUNCTION_SET( IllegalParameter_TrainerMotionBlink );
  DEBUG_FUNCTION_SET( IllegalParameter_PosDegPokePosX );
  DEBUG_FUNCTION_SET( IllegalParameter_PosDegPokePosZ );
  DEBUG_FUNCTION_SET( IllegalParameter_PosDegPokeDeg );
  DEBUG_FUNCTION_SET( IllegalParameter_PosDegTrainerDeg );
  DEBUG_FUNCTION_SET( IllegalParameter_CameraHeight );
  DEBUG_FUNCTION_SET( IllegalParameter_CameraDegAxisY );
  DEBUG_FUNCTION_SET( IllegalParameter_CameraDegAxisX );
  DEBUG_FUNCTION_SET( IllegalParameter_CameraAngle );
  DEBUG_FUNCTION_SET( IllegalParameter_CameraCoefficient );
  DEBUG_FUNCTION_SET( IllegalParameter_OtherCaptureMode );
  DEBUG_FUNCTION_SET( IllegalParameter_OtherComposition );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp1ID );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp1X );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp1Y );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp1Frame );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp2ID );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp2X );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp2Y );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp2Frame );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp3ID );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp3X );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp3Y );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp3Frame );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp4ID );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp4X );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp4Y );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp4Frame );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp5ID );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp5X );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp5Y );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp5Frame );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp6ID );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp6X );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp6Y );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp6Frame );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp7ID );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp7X );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp7Y );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp7Frame );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp8ID );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp8X );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp8Y );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp8Frame );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp9ID );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp9X );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp9Y );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp9Frame );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp10ID );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp10X );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp10Y );
  DEBUG_FUNCTION_SET( IllegalParameter_Stamp10Frame );
  DEBUG_FUNCTION_SET( IllegalParameter_ReflectParam );
  DEBUG_FUNCTION_SET( IllegalParameter_PhotoShare );

  DEBUG_FUNCTION_SET( OutputParameter_ReadParameter );
  DEBUG_FUNCTION_SET( OutputParameter_IllegalParameter );
  DEBUG_FUNCTION_SET( OutputParameter_ViewerParameter );

public:
  /// Viewerの設定
  static void SetFinderStudioViewer( App::FinderStudio::Viewer::FinderStudioViewer* viewer ) { m_pViewer = viewer; }
  /// SDカードから読み込んだ写真データの設定
  static void SetReadPhotoParam( const App::FinderStudio::Static::IntegratedData* param );
  
  /// 写真反映フラグの取得
  static bool IsPhotoReproduct( void ) { return m_bPhotoReproduct; }
  /// 写真反映フラグの初期化
  static void ResetPhotoReproduct( void ) { m_bPhotoReproduct = false; }

  /// 不正データの取得
  static App::FinderStudio::Static::IntegratedData* GetIllegalParameter( void ) { return &m_IllegalPhotoParam; }

private:
  /// 入力
  static u32 _GetAddParameter( void );
  /// パラメータ操作
  static u32 _SelectNextParameter( u32 now, u32 min, u32 max );
  static s32 _SelectNextParameter( s32 now, s32 min, s32 max );
  /// ビット最大数の取得
  static u32 _GetBitMaxNum( u32 bitNum );

  /// 写真データの出力
  static void _OutputPhotoParam( const App::FinderStudio::Static::IntegratedData param );

  /// 不正データ：写真用データの取得
  static App::FinderStudio::Static::PhotoData*  _GetIllegalParameter_PhotoData( void );
  /// 不正データ：写真データの取得
  static App::FinderStudio::Viewer::PhotoParam* _GetIllegalParameter_PhotoParam( void );
  /// 不正データ：スタンプデータの取得
  static App::FinderStudio::Static::StampData*  _GetIllegalParameter_StampData( u8 id );

private:
  gfl2::heap::HeapBase*           m_pHeap;

  gfl2::debug::DebugWinGroup*     m_pRootGroup;

private:
  static App::FinderStudio::Viewer::FinderStudioViewer*   m_pViewer;

  static App::FinderStudio::Static::IntegratedData    m_ReadPhotoParam;

  static App::FinderStudio::Static::IntegratedData    m_IllegalPhotoParam;

  static App::FinderStudio::Static::IntegratedData    m_ViewerPhotoParam;

  static bool   m_bPhotoReproduct;
};

GFL_NAMESPACE_END( MyAlbum )
GFL_NAMESPACE_END( App )

#endif // __MYALBUM_DEBUG_H_INCLUDED__

#endif // PM_DEBUG
