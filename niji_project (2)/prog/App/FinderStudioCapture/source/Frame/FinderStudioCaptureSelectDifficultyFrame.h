#if !defined __FINDER_STUDIO_CAPTURE_SELECT_DIFFICULTY_FRAME_H_INCLUDED__
#define __FINDER_STUDIO_CAPTURE_SELECT_DIFFICULTY_FRAME_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCaptureSelectDifficultyFrame.h
 * @date    2017/01/20 21:12:29
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：難易度選択フレーム
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureFrameBase.h"

#include <model/include/gfl2_DressUpParam.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
class UpperView;
class SelectButtonView;
class PrepareSelectView;
class PrepareCommonView;
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///
/// @brief    難易度選択フレーム
///
//////////////////////////////////////////////////////////////////////////
class SelectDifficultyFrame
  : public FrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( SelectDifficultyFrame );

public:
  //シーケンス
  enum {
    SEQ_SELECT_DIFFICULTY,
    SEQ_SETUP_VIEWER,
    SEQ_CURTAIN_OPEN,
    SEQ_FINISH_CONFIRM,
  };

private:
  /// カーテン用シーケンス
  enum {
    CURTAIN_SEQ_IDLE,
    CURTAIN_SEQ_OPEN,
    CURTAIN_SEQ_OPEN_WAIT,
    CURTAIN_SEQ_NUM,
  };

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  SelectDifficultyFrame( void );
  virtual ~SelectDifficultyFrame( void ) { ; }

public:
  //-------------------------------------------------------
  // @brief   継承関数群
  //-------------------------------------------------------
  virtual applib::frame::Result InitFunc( void );
  virtual applib::frame::Result EndFunc( void );
  virtual applib::frame::Result UpdateFunc( void );

private:
  //-------------------------------------------------------
  // @brief   更新関数：非公開
  //-------------------------------------------------------
  Result _UpdateSelectDifficulty( void );
  Result _UpdateViewer( void );
  Result _UpdateCurtainOpen( void );
  Result _UpdateFinishConfirm( void );

  //-------------------------------------------------------
  // @brief   画面 表示・非表示：非公開
  //-------------------------------------------------------
  void _ShowSelectDifficulty( void );
  void _HideSelectDifficulty( void );

  void _ShowFinishConfirm( void );
  void _HideFinishConfirm( void );

  //-------------------------------------------------------
  // @brief   設定：非公開
  //-------------------------------------------------------
  bool _SetupViewer( void );
  void _SetupViewer_EasyShoot( void );
  void _SetupViewer_EvolutionMode( void );
  void _SetupViewer_KisekaeReversion( void );

  void _SetRandomBackGround( u32* dstBGIndex );
  void _SetDefaultFieldDressupParam( poke_3d::model::DressUpParam* dstDressupParam );

  void _StartViewerUpdate_Easy( void );
  void _StartViewerUpdate_Authentic( void );
  void _StartViewerUpdate_KisekaeReversion( void );

  //-------------------------------------------------------
  // @brief   チェック：非公開
  //-------------------------------------------------------
  bool _IsEventFlag( u32 flg );
  bool _IsKisekaeReversion( void );

  bool _IsGenerationBackGround( u32 bgID );
  bool _IsCompareGenerationBackGround( u32 bgID );

#if PM_DEBUG
private:
  void _CheckBackGroundUnlock( void );
#endif

private:
  UpperView*          m_pUpperView;
  SelectButtonView*   m_pSelectButtonView;
  PrepareCommonView*  m_pPrepareCommonView;
};

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_CAPTURE_SELECT_DIFFICULTY_FRAME_H_INCLUDED__