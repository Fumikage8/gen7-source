//======================================================================
/**
 * @file    FinderStudioDecorationSDCardLoadFrame.cpp
 * @date    2017/01/31 15:28:12
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「デコレーション」：SDカード読込みフレーム
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioDecorationSDCardLoadFrame.h"
#include "FinderStudioDecorationFrameDefine.h"

#include <App/FinderStudioStatic/include/FinderStudioViewList.h>
#include <App/FinderStudioStatic/include/FinderStudioViewBase.h>
#include <App/FinderStudioDecoration/source/View/FinderStudioDecorationViewDefine.h>
#include <App/FinderStudioDecoration/source/View/FinderStudioDecorationUpperView.h>


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
SDCardLoadFrame::SDCardLoadFrame( void )
  : m_pExtSaveData( NULL )
  , m_pUpperView( NULL )
  , m_FrameCount(0)
{
  ;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    初期化・破棄
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   初期化
 */
applib::frame::Result SDCardLoadFrame::InitFunc( void )
{
  GFL_PRINT( "SDCardLoadFrame Start\n" );

  /// 上画面View
  m_pUpperView = reinterpret_cast<UpperView*>( m_pViewList->GetView( VIEW_ID_UPP_MAIN ) );
  m_pUpperView->Show();

  /// 下背景画面View

  return applib::frame::RES_FINISH;
}

/**
 * @brief   終了
 */
applib::frame::Result SDCardLoadFrame::EndFunc( void )
{
  GFL_PRINT( "SDCardLoadFrame Finish\n" );

  // 非表示
  m_pUpperView->Hide();

  return applib::frame::RES_FINISH;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    更新
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   更新
 */
applib::frame::Result SDCardLoadFrame::UpdateFunc( void )
{
  applib::frame::Result( SDCardLoadFrame::*UPDATE_FUNC[] )( void ) = {
    &SDCardLoadFrame::_UpdateFrameCount,
    &SDCardLoadFrame::_UpdateCheckImageDB,
    &SDCardLoadFrame::_UpdateCheckExtSaveData,
  };

  return ( this->*UPDATE_FUNC[m_MainSeq] )( );
}

/**
 * @brief   更新：フレームカウント
 */
applib::frame::Result SDCardLoadFrame::_UpdateFrameCount( void )
{
  m_FrameCount++;

  if( m_FrameCount >= 120 )
  {
    this->SetMainSeq( SEQ_CHECK_IMAGEDB );
  }

  return applib::frame::RES_CONTINUE;
}

/**
 * @brief   更新：ImageDBチェック
 */
applib::frame::Result SDCardLoadFrame::_UpdateCheckImageDB( void )
{
  this->SetMainSeq( SEQ_CHECK_EXTSAVEDATA );

  return applib::frame::RES_CONTINUE;
}

/**
 * @brief   更新：ExtSaveDataチェック
 */
applib::frame::Result SDCardLoadFrame::_UpdateCheckExtSaveData( void )
{
  this->SetMainSeq( SEQ_END );
  this->GetFrameManager()->SetResult( Frame::RES_NEXT );

  return applib::frame::RES_FINISH;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    リスナー関数：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   MessageMenuViewListener：イベント取得
 */
void SDCardLoadFrame::OnEvent( const App::Tool::MessageMenuView::IEventHandler::EventCode event )
{

}

GFL_NAMESPACE_END( Decoration )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
