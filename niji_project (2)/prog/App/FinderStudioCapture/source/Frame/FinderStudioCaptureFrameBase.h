#if !defined __FINDER_STUDIO_CAPTURE_FRAME_BASE_H_INCLUDED__
#define __FINDER_STUDIO_CAPTURE_FRAME_BASE_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCaptureFrameBase.h
 * @date    2016/11/09 20:13:47
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：フレームベース
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureFrameManager.h"

#include <AppLib/include/Frame/AppFrameCellSuper.h>
#include <Sound/include/Sound.h>

#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

#if PM_DEBUG
#include <GameSys/include/GameManager.h>
#include <ui/include/gfl2_UI.h>
#endif


//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Static )
class MessageMenuView;
GFL_NAMESPACE_END( Static )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    フレームベース
///
//////////////////////////////////////////////////////////////////////////
class FrameBase
  : public applib::frame::CellSuper
{
  GFL_FORBID_COPY_AND_ASSIGN( FrameBase );

protected:
  /// リザルト
  enum Result
  {
    RESULT_CONTINUE,    //!< 継続
    RESULT_FINISH,      //!< 終了
    RESULT_CHANGE_SEQ,  //!< シーケンス変更
    RESULT_NUM,
  };

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  FrameBase( void )
    : m_pManager( NULL )
    , m_pListener( NULL )
    , m_pMainWork( NULL )
    , m_pMsgMenuView( NULL )
    , m_MainSeq( 0 )
    , m_SubSeq( 0 )
  {
    ;
  }
  virtual ~FrameBase( void ) { ; }

public:
  //-------------------------------------------------------
  // @brief   継承関数群
  //-------------------------------------------------------
  virtual applib::frame::Result InitFunc( void ) { return applib::frame::RES_FINISH; }
  virtual applib::frame::Result EndFunc( void ) { return applib::frame::RES_FINISH; }
  virtual applib::frame::Result UpdateFunc( void ) { return applib::frame::RES_FINISH; }
  virtual void                  DrawFunc( gfl2::gfx::CtrDisplayNo no ) { ; }

public:
  //-------------------------------------------------------
  // @brief   設定関数群
  //-------------------------------------------------------
  void SetFrameManager( FrameManager* manager ) { m_pManager = manager; }
  void SetFrameListener( FrameListener* listener ) { m_pListener = listener; }
  void SetMainWork( FrameManager::MainWork* work ) { m_pMainWork = work; }

  //-------------------------------------------------------
  // @brief   取得関数群
  //-------------------------------------------------------
  FrameManager* GetFrameManager( void ) const { return m_pManager; }

protected:
  //-------------------------------------------------------
  // @brief   シーケンス関数
  //-------------------------------------------------------
  /// メインシーケンスの設定
  void SetMainSeq( u8 seq )
  {
    m_MainSeq = seq;
    SetSubSeq( 0 );
  }
  /// サブシーケンスの設定
  void SetSubSeq( u8 seq )
  {
    m_SubSeq = seq;
  }

#if PM_DEBUG
public:
  applib::frame::Result InputButton( void )
  {
    gfl2::ui::Button* button = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );

    if( button->IsTrigger( gfl2::ui::BUTTON_START ) ) { return applib::frame::RES_FINISH; }
    if( button->IsTrigger( gfl2::ui::BUTTON_A ) ) { return applib::frame::RES_FINISH; }
    if( button->IsTrigger( gfl2::ui::BUTTON_B ) ) { return applib::frame::RES_FINISH; }

    return applib::frame::RES_CONTINUE;
  }
#endif

private:
  FrameManager*   m_pManager;

protected:
  FrameListener*              m_pListener;

  FrameManager::MainWork*     m_pMainWork;

  Static::MessageMenuView*    m_pMsgMenuView;

protected:
  u8    m_MainSeq;
  u8    m_SubSeq;
};

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_CAPTURE_FRAME_BASE_H_INCLUDED__