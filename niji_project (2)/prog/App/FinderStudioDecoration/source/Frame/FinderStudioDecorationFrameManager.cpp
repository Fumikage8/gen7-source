//======================================================================
/**
 * @file    FinderStudioDecorationFrameManager.cpp
 * @date    2016/11/22 17:05:51
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「デコレーション」：フレームマネージャ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioDecorationFrameManager.h"

#include "FinderStudioDecorationFrameSequence.h"
#include "FinderStudioDecorationFrameBase.h"
#include "FinderStudioDecorationSDCardLoadFrame.h"
#include "FinderStudioDecorationSDCardSaveFrame.h"
#include "FinderStudioDecorationMainFrame.h"


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Decoration )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    コンストラクタ・デストラクタ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   コンストラクタ
 */
FrameManager::FrameManager( gfl2::heap::HeapBase* heap, Static::ViewList* list, Static::AlbumWork* work, DecoWork* decoWork )
  : applib::frame::Manager( heap )
  , m_pViewList( list )
  , m_pWork( work )
  , m_pDecoWork( decoWork )
  , m_FrameID( Frame::FRAME_ID_NULL )
  , m_FrameResult( Frame::RES_NUM )
{
  ;
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
  else if( id == Frame::FRAME_ID_DECORATION )
  {
    _CallFrame<DecorationMainFrame>();
  }
  else if( id == Frame::FRAME_ID_SDCARD_SAVE )
  {
    _CallFrame<SDCardSaveFrame>();
  }

  m_FrameID = id;
}

/**
* @brief   フレームの呼び出し
*/
template<class CallClass> void FrameManager::_CallFrame( void )
{
  CallClass* cc = MENU_FRAME_CALL_MODAL<applib::frame::Manager, CallClass>( this );
  cc->SetFrameManager( this );
  cc->SetViewList( m_pViewList );
  cc->SetAlbumWork( m_pWork );
  cc->SetDecoWork( m_pDecoWork );
}

GFL_NAMESPACE_END( Decoration )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
