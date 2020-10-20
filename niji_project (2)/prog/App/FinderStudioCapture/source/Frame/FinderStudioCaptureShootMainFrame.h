#if !defined __FINDER_STUDIO_CAPTURE_SHOOT_MAIN_FRAME_H_INCLUDED__
#define __FINDER_STUDIO_CAPTURE_SHOOT_MAIN_FRAME_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCaptureShootMainFrame.h
 * @date    2016/11/09 20:36:12
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：撮影メインフレーム
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureFrameBase.h"


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
class UpperView;
class BGView;
class ShootView;
class ZMotionView;
class CameraView;
class SelectButtonView;
class PrepareCommonView;
class ShootCommonView;
class LayoutView;
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
///
/// @brief    撮影メインフレーム
///
//////////////////////////////////////////////////////////////////////////
class ShootMainFrame
  : public FrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( ShootMainFrame );

private:
  enum {
    SEQ_SHOOT_MAIN,         //!< 撮影トップ
    SEQ_SELECT_ZMOTION,     //!< Zモーション選択
    SEQ_CAMERA_CONTROL,     //!< カメラ操作
    SEQ_POKEMON_MOVE,       //!< ポケモン移動
    SEQ_POKEMON_ROTATE,     //!< ポケモン回転
    SEQ_TRAINER_ROTATE,     //!< トレーナー回転
    SEQ_FINISH_CONFIRM,     //!< 終了確認
    SEQ_CAPTURE,            //!< 撮影
    SEQ_NUM,
  };

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  ShootMainFrame( void );
  virtual ~ShootMainFrame( void ) { ; }

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
  Result _UpdateShootMain( void );
  Result _UpdateSelectZMotion( void );
  Result _UpdateCameraControl( void );
  Result _UpdateShootLayout( void );
  Result _UpdateFinishConfirm( void );
  Result _UpdateCapture( void );
  Result _UpdateShootCommonView( void );

  //-------------------------------------------------------
  // @brief   撮影シーケンス：非公開
  //-------------------------------------------------------
  bool _UpdateCapture_ShutterOpen( void );
  bool _UpdateCapture_ShutterClsoe( void );
  bool _UpdateCapture_ShutterSE( void );

  //-------------------------------------------------------
  // @brief   位置調整View関連：非公開
  //-------------------------------------------------------
  void _SetLayoutViewType( void );
  
  //-------------------------------------------------------
  // @brief   共通View関連：非公開
  //-------------------------------------------------------
  void _SetCommonViewInputEnabled( void );
  void _SetCommonViewResult( u32 result );
  void _SetCommonViewResult_ShootMain( u32 result );
  void _SetCommonViewResult_ShootOther( u32 result );

  //-------------------------------------------------------
  // @brief   画面 表示・非表示：非公開
  //-------------------------------------------------------
  void _ShowShootView( void );
  void _HideShootView( void );
  void _ShowCameraView( void );
  void _HideCameraView( void );
  void _ShowZMotionView( void );
  void _HideZMotionView( void );
  void _ShowFinishConfirmView( void );
  void _HideFinishConfirmView( void );
  void _ShowShootCommonView( void );
  void _HideShootCommonView( void );
  void _ShowLayoutView( void );
  void _HideLayoutView( void );
  void _ShowBGView( void );
  void _HideBGView( void );

  //-------------------------------------------------------
  // @brief   設定・取得：非公開
  //-------------------------------------------------------
  void _SetPokemonMotionAutoPlayEnable( bool enable );
  
  void* _GetPhotoTexture( u32 index );
  
  Work* _GetWork( void );
  ViewerManager* _GetViewerManager( void );

  //-------------------------------------------------------
  // @brief   チェック：非公開
  //-------------------------------------------------------
  bool _IsFinishCapture( void );
  bool _IsDifficultyEasy( void );
  bool _IsCaptureModePokeOnly( void );

private:
  UpperView*          m_pUpperView;
  BGView*             m_pBGView;
  ShootView*          m_pShootView;
  ShootCommonView*    m_pShootCommonView;
  ZMotionView*        m_pZMotionView;
  CameraView*         m_pCameraView;
  SelectButtonView*   m_pSelectButtonView;
  PrepareCommonView*  m_pPrepareCommonView;
  LayoutView*         m_pLayoutView;

  u8    m_ShutterCloseSeq;
  u8    m_ShutterOpenSeq;
  u8    m_SeSeq;
  u8    m_SeCount;
};

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_CAPTURE_SHOOT_MAIN_FRAME_H_INCLUDED__
