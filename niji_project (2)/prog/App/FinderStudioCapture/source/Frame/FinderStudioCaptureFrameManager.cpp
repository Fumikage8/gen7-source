//======================================================================
/**
 * @file    FinderStudioCaptureFrameManager.cpp
 * @date    2016/11/09 19:11:13
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：Viewリスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureFrameManager.h"

#include "FinderStudioCaptureFrameSequence.h"
#include "FinderStudioCaptureFrameBase.h"
#include "FinderStudioCaptureSDCardLoadFrame.h"
#include "FinderStudioCaptureSDCardSaveFrame.h"
#include "FinderStudioCaptureShootPrepareFrame.h"
#include "FinderStudioCaptureShootMainFrame.h"
#include "FinderStudioCapturePhotoListFrame.h"
#include "FinderStudioCaptureSelectDifficultyFrame.h"
#include <App/FinderStudioCapture/source/Debug/FinderStudioCaptureDebugAutoCaptureFrame.h>
#include <App/FinderStudioCapture/source/Debug/FinderStudioCaptureDebugPhotoCompareFrame.h>
#include <App/FinderStudioCapture/source/Debug/FinderStudioCaptureDebugAgingFrame.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    コンストラクタ・デストラクタ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   コンストラクタ
 */
 FrameManager::FrameManager( gfl2::heap::HeapBase* heap, app::util::Heap* appHeap, Work* work, Static::ViewList* list, ViewerManager* viewer )
  : applib::frame::Manager( heap )
  , m_pFrameListener( NULL )
  , m_MainWork()
  , m_FrameID( Frame::FRAME_ID_NULL )
  , m_FrameResult( Frame::RES_NUM )
{
  m_MainWork.heap = appHeap;
  m_MainWork.work = work;
  m_MainWork.viewList = list;
  m_MainWork.viewer = viewer;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    フレーム
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   フレームの開始
 *
 * @param   id    開始するフレームID
 */
void FrameManager::StartFrame( Frame::FrameID id )
{
  CallFrame( id );
}

/**
 * @brief   次のフレーム
 *
 * @param   status  メイン結果
 *
 * @return  "true  == 次がある"
 * @return  "false == 次がない"
 */
bool FrameManager::NextFrame( applib::frame::MainStatus status )
{
  Frame::FrameID frameID = FrameSequence::GetNextFrameID( m_FrameID, m_FrameResult );
  m_FrameResult = Frame::RES_NUM;

  if( frameID != Frame::FRAME_ID_NULL )
  {
    CallFrame( frameID );

    return true;
  }

  return false;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    コール関数
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   フレームの呼び出し
 *
 * @param   id    呼び出すフレームID
 */
void FrameManager::CallFrame( Frame::FrameID id )
{
  if( id == Frame::FRAME_ID_SDCARD_LOAD )
  {
    _CallFrame<SDCardLoadFrame>();
  }
  else if( id == Frame::FRAME_ID_SHOOT_PREPARE )
  {
    _CallFrame<ShootPrepareFrame>();
  }
  else if( id == Frame::FRAME_ID_SHOOT_MAIN )
  {
    _CallFrame<ShootMainFrame>();
  }
  else if( id == Frame::FRAME_ID_PHOTO_LIST )
  {
    _CallFrame<PhotoListFrame>();
  }
  else if( id == Frame::FRAME_ID_SDCARD_SAVE )
  {
    _CallFrame<SDCardSaveFrame>();
  }
  else if( id == Frame::FRAME_ID_SELECT_DIFFICULTY )
  {
    _CallFrame<SelectDifficultyFrame>();
  }
#if PM_DEBUG
  else if( id == Frame::FRAME_ID_DEBUG_AUTOCAP )
  {
    _CallFrame<DebugAutoCaptureFrame>();
  }
  else if( id == Frame::FRAME_ID_DEBUG_PHOTOCOMPARE )
  {
    _CallFrame<DebugPhotoCompareFrame>();
  }
  else if( id == Frame::FRAME_ID_DEBUG_AGING )
  {
    _CallFrame<DebugAgingFrame>();
  }
#endif

  m_FrameID = id;
}

/**
 * @brief   フレームの呼び出し
 */
template<class CallClass> void FrameManager::_CallFrame( void )
{
  CallClass* cc = MENU_FRAME_CALL_MODAL<applib::frame::Manager, CallClass>( this );
  cc->SetFrameManager( this );
  cc->SetFrameListener( m_pFrameListener );
  cc->SetMainWork( &m_MainWork );
}

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
