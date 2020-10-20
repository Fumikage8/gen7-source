#if PM_DEBUG

#if !defined(__FINDER_STUDIO_CAPTURE_DEBUG_AGING_FRAME_H_INCLUDED__)
#define __FINDER_STUDIO_CAPTURE_DEBUG_AGING_FRAME_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCaptureDebugAgingFrame.h
 * @date    2017/04/18 10:54:52
 * @author  fukushima_yuya
 * @brief   デバッグ機能フレーム：エージング
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <App/FinderStudioCapture/source/Frame/FinderStudioCaptureFrameBase.h>

#include <App/FinderStudioStatic/include/FinderStudioDefine.h>
#include <model/include/gfl2_DressUpParam.h>
#include <PokeTool/include/PokeModel.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
class ViewerManager;
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///
/// @brief    デバッグ機能フレーム：エージング
///
//////////////////////////////////////////////////////////////////////////
class DebugAgingFrame
  : public FrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( DebugAgingFrame );

private:
  enum Sequence
  {
    SEQ_INIT,
    SEQ_UPDATE_POKEMON,
    SEQ_UPDATE_TRAINER,
    SEQ_UPDATE_BACKGROUND,
    SEQ_END,
    SEQ_NUM,
  };

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  DebugAgingFrame( void );
  virtual ~DebugAgingFrame( void ) { ; }

public:
  //-------------------------------------------------------
  // @brief   継承関数群
  //-------------------------------------------------------
  virtual applib::frame::Result InitFunc( void );
  virtual applib::frame::Result EndFunc( void );
  virtual applib::frame::Result UpdateFunc( void );

private:
  //-------------------------------------------------------
  // @brief   初期化：非公開
  //-------------------------------------------------------
  void _InitSequence( void );
  void _InitSequence_Pokemon( void );
  void _InitSequence_Trainer( void );
  void _InitSequence_BackGround( void );

  //-------------------------------------------------------
  // @brief   更新：非公開
  //-------------------------------------------------------
  void _UpdateSequence_Pokemon( void );
  void _UpdateSequence_Trainer( void );
  void _UpdateSequence_BackGround( void );

  //-------------------------------------------------------
  // @brief   ポケモン：非公開
  //-------------------------------------------------------
  void _RequestChangePokeModel( void );
  void _RequestChangePokeModel( PokeTool::SimpleParam& param );
  bool _IsEndChangePokeModel( void ) { return _IsFinishUpdateViewer(); }

  void _RequestChanePokeMotionType_Battle( void );
  void _RequestChanePokeMotionType_Kawaigari( void );
  void _RequestChanePokeMotionType( Viewer::PokeMotionType type );
  bool _IsEndChangePokeMotionType( void ) { return _IsFinishUpdateViewer(); }

  bool _CheckAvailablePokemonMotion( void );
  bool _CheckPokemonAllMotion( void );
  bool _CheckPokemonSleepMotion( void );

  void _LoadPersonalData( MonsNo monsNo );
  void _LoadPersonalData( MonsNo monsNo, pml::FormNo formNo );

  u32 _GetMonsNo( void );
  pml::FormNo _GetFormMax( MonsNo monsNo );
  pml::Sex _GetBaseSex( MonsNo monsNo );
  pml::personal::SexType _GetSexType( MonsNo monsNo );

  bool _PokemonPlacement( void );
  bool _PokemonPlacement( gfl2::math::VEC3 pos );
  void _AdjustPokemonPosition( gfl2::math::VEC3* pos );
  bool _PokemonAvailableRotate( void );

  f32  _GetPlayableRatePokemonMotion( void );

  void _SetAvailableMotionList( void );
  PokeTool::MODEL_ANIME _GetPlayMotionID( void );
  void _PlayPokemonMotion( PokeTool::MODEL_ANIME modelAnime );

  bool _IsAvailableKawaigariMotion( void );
  bool _IsAvailableSleepMotion( void );

  bool _IsFinishPokeMotion( void );

  bool _PlayPokemonMotionSleep( void );
  bool _IsAvailableWake( void );
  void _PlayPokemonMotionWake( void );

  //-------------------------------------------------------
  // @brief   トレーナー：非公開
  //-------------------------------------------------------
  poke_3d::model::DressUpParam _GetDressUpParam( void );
  poke_3d::model::DressUpParam _GetDressUpParam_Male( void );
  poke_3d::model::DressUpParam _GetDressUpParam_Female( void );
  poke_3d::model::DressUpParam _FraudCheckDressUpParam( poke_3d::model::DressUpParam param );
  
  void _RequestChangeDressUp( void );
  void _RequestChangeDressUp( poke_3d::model::DressUpParam param );
  void _RequestPlayTrainerMotion( void );
  void _RequestPlayTrainerMotion( Viewer::ZMotionType type );
  bool _IsEndChangeDressUp( void ) { return _IsFinishUpdateViewer(); }
  bool _IsEndTrainerMotion( void );

  u8 _GetAgingTrainerMode( void );

  //-------------------------------------------------------
  // @brief   背景：非公開
  //-------------------------------------------------------
  bool _IsEndBackGround( void );
  bool _IsEndBackGround_WaitTime( void );
  bool _IsEndBackGround_InputKey( void );

  u32  _GetAgingBackGroundID( void );
  Viewer::TimeZone _GetAgingBackGroundTimeZone( u32 bgID );
  Viewer::TimeZone _GetAgingBackGroundNextTimeZone( Viewer::TimeZone now );
  bool _IsAvailableTimeZone( u32 bgID, Viewer::TimeZone timeZone );

  u8   _GetAgingBackGroundMode( void );
  u8   _GetAgingBackGroundChange( void );
  u8   _GetAgingBackGroundCamera( void );

  void _BackGroundKeyAction( void );
  void _BackGroundKeyActionRotate( gfl2::ui::VectorDevice* stick );
  void _BackGroundKeyActionZoom( gfl2::ui::Button* button );
  void _BackGroundKeyActionHeight( gfl2::ui::Button* button );
  void _BackGroundKeyActionFocus( gfl2::ui::Button* button );
  void _BackGroundKeyActionAutoRotate( void );

  //-------------------------------------------------------
  // @brief   エージングチェック関数：非公開
  //-------------------------------------------------------
  bool _IsAgingStart( void );
  bool _IsAgingStartPokemon( void );
  bool _IsAgingStartTrainer( void );
  bool _IsAgingStartBackGround( void );

  //-------------------------------------------------------
  // @brief   便利関数：非公開
  //-------------------------------------------------------
  gfl2::ui::Button* _GetUIDevice_Button( void );
  gfl2::ui::VectorDevice* _GetUIDevice_Stick( void );

  u32 _GetRandomNum( void );
  u32 _GetRandomNum( u32 max );

  bool _IsFinishUpdateViewer( void );

  void _IncrementLoopCount( void );

private:
  ViewerManager*    m_pViewerManager;

  u32   m_AgingLoopCount;

/// ポケモン
private:
  PokeTool::SimpleParam   m_PokemonSimpleParam;

  u16   m_PokemonStartMonsNo;

  u16   m_PokemonFinishMonsNo;

  u16   m_PokemonMonsNo;

  pml::FormNo   m_PokemonFormNo;

  pml::FormNo   m_PokemonFormMax;

  pml::Sex      m_PokemonSex;

  pml::personal::SexType  m_PokemonSexType;

  bool          m_bPokemonRare;

  u8    m_PokemonPositionSeq;

  u8    m_PokemonPositionID;

  u8    m_PokemonRotationID;

  u8    m_PokemonMotionSeq;

  u8    m_PokemonMotionID;

  const PokeTool::MODEL_ANIME*    m_pPokemonMotionList;

  PokeTool::MODEL_ANIME           m_PokemonMotion;

  u32   m_MotionListMax;

  Viewer::PokeMotionType    m_PokemonMotionType;

  u32   m_MotionWaitFrame;

  u8    m_PokemonSleepMotionSeq;

  u8    m_PokemonSleepWaitFrame;

/// トレーナー
private:
  poke_3d::model::DressUpParam::Sex   m_TrainerDressUpParamSex;
  
  u8    m_TrainerZMotionType;

/// 背景
private:
  u32   m_BackGroundID;

  u32   m_BackGroundCount;

  u32   m_BackGroundWaitTime;

  Viewer::TimeZone    m_BackGroundTimeZone;
};

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif  // __FINDER_STUDIO_CAPTURE_DEBUG_AGING_FRAME_H_INCLUDED__
#endif  // PM_DEBUG
