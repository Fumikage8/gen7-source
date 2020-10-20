//======================================================================
/**
 * @file    FinderInfoWindow.cpp
 * @date    2015/10/23 15:14:15
 * @author  fukushima_yuya
 * @brief   インフォメーションウィンドウ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "FinderInfoWindow.h"
#include "FinderDrawerBase.h"

#include <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowSheetType.h>
#include <AppLib/include/Ui/UIView.h>

// arc
#include <niji_conv_header/app/finder/finder_pane.h>
#include <niji_conv_header/message/msg_pokefinder_camera.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Finder)

//==============================================================================
// @brief   インフォメーションウィンドウクラス
//==============================================================================
  //----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //----------------------------------------------------------------------------
  FinderInfoWindow::FinderInfoWindow( DrawerBase* base, app::util::AppRenderingManager* renderMan, app::tool::InfoWindowSheetTypeListener* listener )
    : m_pBase( base )
    , m_pInfoWindow( NULL )
    , m_pInfoWindowListener( listener )
    , m_EndSeq( 0 )
  {
    app::ui::UIView* view = m_pBase->GetUIView();
    app::util::Heap* heap = m_pBase->GetHeap();
    gfl2::heap::HeapBase* devHeap = m_pBase->GetDeviceHeap();

    const u32 c_CharMax[] = { 64, 512 };

    for( u32 i=0; i<STR_NUM; ++i )
    {
      m_pStrBuf[i] = GFL_NEW(devHeap) gfl2::str::StrBuf( c_CharMax[i], devHeap );
    }

    // InfoWindowSheetType の生成
    m_pInfoWindow = GFL_NEW(devHeap) app::tool::InfoWindowSheetType( heap );
    {
      m_pInfoWindow->SyncCreate( heap, renderMan, NULL );

      // VIEWの追加
      view->AddSubView( m_pInfoWindow );
    }

    {
      gfl2::lyt::LytPane* infoBtn = m_pBase->GetPane( PANENAME_FINDER_MIN_LOW_000_PANE_INFOBUTTON );
      {
        m_InfoBtnPos = gfl2::math::ConvertNwVec3ToGfVec3( infoBtn->GetTranslate() );
      }
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //----------------------------------------------------------------------------
  FinderInfoWindow::~FinderInfoWindow( void )
  {
    for( u32 i=0; i<STR_NUM; ++i )
    {
      GFL_SAFE_DELETE( m_pStrBuf[i] );
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   準備完了か？
   */
  //----------------------------------------------------------------------------
  bool FinderInfoWindow::IsReady( void ) const
  {
    if( m_pInfoWindow )
    {
      if( !m_pInfoWindow->IsCreated() ) return false;
    }

    return true;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   終了シーケンス
   * @return  "true" == 終了
   * @return  "false" == 終了処理中
   */
  //----------------------------------------------------------------------------
  bool FinderInfoWindow::EndFunc( void )
  {
    enum {
      SEQ_DELETE,
      SEQ_WAIT,
      SEQ_END,
    };

    switch( m_EndSeq )
    {
    case SEQ_DELETE:
      {
        m_pInfoWindow->RemoveFromSuperView();

        m_EndSeq = SEQ_WAIT;
      } break;

    case SEQ_WAIT:
      {
        if( !m_pInfoWindow->EndFunc() ) break;

        GFL_SAFE_DELETE( m_pInfoWindow );

        m_EndSeq = SEQ_END;
      } break;

    case SEQ_END:
      {
        return true;
      }
    }

    return false;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   インフォメーションウィンドウを開く
   * @param   version   ヴァージョン
   */
  //----------------------------------------------------------------------------
  void FinderInfoWindow::Open( u8 version )
  {
    if( !m_pInfoWindow ) return;

    gfl2::str::MsgData* msgData = m_pBase->GetMsgData();

    // タイトル
    {
      msgData->GetString( msg_pokefinder_info_01, *m_pStrBuf[STR_TITLE] );
    }

    // 本文
    {
      static const u32 c_InfoDetail[] = {
        msg_pokefinder_info_01_01,
        msg_pokefinder_info_02_01,
        msg_pokefinder_info_02_01,
        msg_pokefinder_info_02_01,
        msg_pokefinder_info_03_01,
      };

      msgData->GetString( c_InfoDetail[version], *m_pStrBuf[STR_DETAIL] );
    }

    // インフォウィンドウのオープン
    m_pInfoWindow->StartOpen(
      m_pInfoWindowListener,
      m_InfoBtnPos,
      m_pStrBuf[STR_TITLE],
      m_pStrBuf[STR_DETAIL] );
  }

GFL_NAMESPACE_END(Finder)
GFL_NAMESPACE_END(App)
