#if !defined __FINDER_STUDIO_CAPTURE_SDCARD_LOAD_FRAME_H_INCLUDED__
#define __FINDER_STUDIO_CAPTURE_SDCARD_LOAD_FRAME_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCaptureSDCardLoadFrame.h
 * @date    2016/11/09 20:12:11
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：SDカード読込みフレーム
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureFrameBase.h"

#include <App/FinderStudioStatic/include/FinderStudioDefine.h>
#include <App/FinderStudioStatic/include/FinderStudioErrorConverter.h>
#include <App/FinderStudioStatic/include/FinderStudioMessageMenuView.h>
#include <App/FinderStudioStatic/include/FinderStudioUtil.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
class UpperView;
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///
/// @brief    SDカード読込みフレーム
///
//////////////////////////////////////////////////////////////////////////
class SDCardLoadFrame
  : public FrameBase
  , public Static::MessageMenuViewListener
{
  GFL_FORBID_COPY_AND_ASSIGN( SDCardLoadFrame );

private:
  enum {
    SEQ_WAIT_FRAME,           //!< 待機フレーム
    SEQ_CHECK_IMGDB,          //!< ImageDBチェック
    SEQ_CHECK_EXTSAVEDATA,    //!< 拡張SDチェック
    SEQ_CHECK_REGION,         //!< 残保存枚数チェック
    SEQ_FATAL_ERROR,          //!< フェイタルエラー
    SEQ_FORCED_END,           //!< 強制終了
    SEQ_END,
  };
  enum MsgID
  {
    MSG_ID_LOAD_BEGIN,
    MSG_ID_IMGDB_ERROR,
    MSG_ID_EXTSAVE_ERROR_BROKEN_DATA,
    MSG_ID_EXTSAVE_ERROR_NOT_SPACE,
    MSG_ID_REGION_CAUTION,
    MSG_ID_NUM,

    MSG_ID_NULL = MSG_ID_NUM,
  };

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  SDCardLoadFrame( void );
  virtual ~SDCardLoadFrame( void ) { ; }

public:
  //-------------------------------------------------------
  // @brief   継承関数群
  //-------------------------------------------------------
  virtual applib::frame::Result InitFunc( void );
  virtual applib::frame::Result EndFunc( void );
  virtual applib::frame::Result UpdateFunc( void );

private:
  //-------------------------------------------------------
  // @brief   更新：非公開
  //-------------------------------------------------------
  void _UpdateWaitFrame( void );
  void _UpdateImageDB( void );
  void _UpdateExtSavedata( void );
  void _UpdateCheckRegion( void );
  void _UpdateErrorMessage( void );

  //-------------------------------------------------------
  // @brief   チェック：非公開
  //-------------------------------------------------------
  bool _CheckImageDB( ErrorConverter::ImageDBResultCode* code );
  bool _CheckExtSavedata( ErrorConverter::SDCardResultCode* code );

  //-------------------------------------------------------
  // @brief   メッセージウィンドウ：非公開
  //-------------------------------------------------------
  void _ShowMesasgeWindow( MsgID id );
  void _HideMessageWindow( void );
  void _ResultMessageWindow( void );

  //-------------------------------------------------------
  // @brief   リスナー関数：非公開
  //-------------------------------------------------------
  virtual void OnEvent( const App::Tool::MessageMenuView::IEventHandler::EventCode event );

private:
  Static::FinderStudioExtSaveDataAccessor*    m_pExtSaveData;

  ErrorConverter::Message*            m_pMessage;
  ErrorConverter::ImageDBResultCode   m_ImageDBResultCode;
  ErrorConverter::SDCardResultCode    m_SDCardResultCode;

  u32   m_WaitFrame;
  u32   m_LoadDataIndex;
  u32   m_SavePhotoNum;

  MsgID   m_MsgID;

  bool  m_bDataFileNone;
};

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_CAPTURE_SDCARD_LOAD_FRAME_H_INCLUDED__
