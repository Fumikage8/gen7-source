#if PM_DEBUG

#if !defined __FINDER_STUDIO_CAPTURE_DEBUG_AUTO_CAPTURE_FRAME_H_INCLUDED__
#define __FINDER_STUDIO_CAPTURE_DEBUG_AUTO_CAPTURE_FRAME_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCaptureDebugAutoCaptureFrame.h
 * @date    2017/04/06 12:15:40
 * @author  fukushima_yuya
 * @brief   デバッグ機能フレーム：自動撮影
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <App/FinderStudioCapture/source/Frame/FinderStudioCaptureFrameBase.h>
#include <App/FinderStudioCapture/source/FinderStudioCaptureViewerManager.h>
#include <App/FinderStudioStatic/include/FinderStudioUtil.h>


//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Static )
class ImageDBUtil;
GFL_NAMESPACE_END( Static )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
class ViewerManager;
class UpperView;
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
///
/// @brief    写真リストフレーム
///
//////////////////////////////////////////////////////////////////////////
class DebugAutoCaptureFrame
  : public FrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( DebugAutoCaptureFrame );

private:
  enum {
    SEQ_INIT,
    SEQ_VIEWER_SETUP,
    SEQ_VIEWER_UPDATE,
    SEQ_CHECK_FULL,
    SEQ_CAPTURE,
    SEQ_SAVE_IMAGEDB,
    SEQ_SAVE_EXTSAVE,
    SEQ_FINISH_CHECK,
    SEQ_END,
  };

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  DebugAutoCaptureFrame( void );
  virtual ~DebugAutoCaptureFrame( void ) { ; }

public:
  //-------------------------------------------------------
  // @brief   継承関数群
  //-------------------------------------------------------
  virtual applib::frame::Result InitFunc( void );
  virtual applib::frame::Result EndFunc( void );
  virtual applib::frame::Result UpdateFunc( void );

private:
  void _UpdateFunc_Init( void );
  void _UpdateFunc_ViewerSetup( void );
  void _UpdateFunc_ViewerUpdate( void );
  void _UpdateFunc_CheckFull( void );
  void _UpdateFunc_Capture( void );
  void _UpdateFunc_SaveImageDB( void );
  void _UpdateFunc_SaveExtSave( void );
  void _UpdateFunc_FinishCheck( void );

private:
  PokeTool::SimpleParam _SetupSimpleParam( MonsNo monsNo );
  u32 _SetupBackGroundID( void );
  poke_3d::model::DressUpParam _SetupDressupParam( void );
  Viewer::Composition _SetupComposition( void );

  void _SetupIntegratedData( MonsNo monsNo );
  void _SetupManagementData( MonsNo monsNo );
  void _SetupDecorationData( void );

private:
  MonsNo _GetMonsNo( void );
  pml::Sex _GetSex( MonsNo monsNo );
  pml::FormNo _GetFormNo( MonsNo monsNo );
  pml::FormNo _GetFormMax( MonsNo monsNo );
  u8 _GetDecorationFlg( void );

  Static::Date _GetDate( void );
  Static::Date _GetDate_Now( void );
  Static::Date _GetDate_Random( void );
  bool _IsUruuDoshi( u16 year );
  u8 _GetDateNum( u8 month, bool bUruuDoshi );

  u32 _GetRandomNum( void );
  u32 _GetRandomNum( u32 max );

private:
  static const u32 POKEMON_NUM = MONSNO_END - 1;

private:
  ViewerManager*    m_pViewerManager;

  ViewerManager::RequestViewerParam   m_DefaultRequestParam;

  UpperView*    m_pUpperView;

  Static::ImageDBUtil*      m_pImageDBUtil;

  Static::FinderStudioExtSaveDataAccessor*    m_pExtSaveData;

  Static::IntegratedData    m_IntegratedData;

  Static::ManagementData    m_ManagementData;

private:
  u32       m_CaptureCount;

  u32       m_CaptureCountMax;

  u32       m_MonsNo;

  bool      m_bRandomMonsNo;

  u32       m_BackGroundID;

  u32       m_BackGroundType;

  bool      m_bRandomDressup;

  bool      m_bRandomDeco;

  bool      m_bRandomDate;

  bool      m_bSaveFlg;
};

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif  // __FINDER_STUDIO_CAPTURE_DEBUG_AUTO_CAPTURE_FRAME_H_INCLUDED__

#endif  // PM_DEBUG
