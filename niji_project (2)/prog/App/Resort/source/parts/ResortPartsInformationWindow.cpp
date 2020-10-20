//======================================================================
/**
 * @file ResortPartsInformationWindow.cpp
 * @date 2015/10/02 15:05:45
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <App/Resort/source/parts/ResortPartsInformationWindow.h>
#include <App/Resort/source/parts/ResortPartsPaneList.h>
#include <App/Resort/source/ResortDrawerBase.h>

#include <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowSheetType.h>

// arc
#include <arc_index/message.gaix>
#include <niji_conv_header/message/msg_pokeresort_system.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Resort )

//==============================================================================
// @brief   インフォメーションウィンドウリスナークラス
//==============================================================================
  //----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //----------------------------------------------------------------------------
  ResortInfoWindowListener::ResortInfoWindowListener( DrawerBase* pBase, ResortPaneList* paneList )
    : m_pBase( pBase )
    , m_pPaneList( paneList )
  {
    ;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //----------------------------------------------------------------------------
  ResortInfoWindowListener::~ResortInfoWindowListener( void )
  {
    ;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   閉じ終わった
   */
  //----------------------------------------------------------------------------
  void ResortInfoWindowListener::OnClose( void )
  {
    app::ui::UIView* pUIView = m_pBase->GetUIView();

    pUIView->SetInputEnabled( true );

    m_pPaneList->SetInputEnable( true );
  }



//==============================================================================
// @brief   インフォメーションウィンドウクラス
//==============================================================================
  //----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //----------------------------------------------------------------------------
  ResortInfoWindow::ResortInfoWindow( DrawerBase* pBase, app::util::AppRenderingManager* pAppRenderingManager, ResortPaneList* paneList, gfl2::str::MsgData* itemInfoMsgData )
    : m_pBase( pBase )
    , m_pInfoWindow( NULL )
    , m_pInfoWindowListener( NULL )
    , m_pPaneList( paneList )
    , m_pItemInfo( itemInfoMsgData )
    , m_DeleteSeq( 0 )
  {
    app::util::Heap* heap = m_pBase->GetHeap();
    gfl2::heap::HeapBase* devHeap = m_pBase->GetDeviceHeap();

    const u32 c_CharMax[] = { 64, 512 };

    for( u32 i=0; i<STR_NUM; ++i )
    {
      m_pStrBuf[i] = GFL_NEW(devHeap) gfl2::str::StrBuf( c_CharMax[i], devHeap );
    }

    // InfoWindowSheetType の生成
    m_pInfoWindow = GFL_NEW(devHeap) app::tool::InfoWindowSheetType( heap );

    // ResortInfoWindowListener の生成
    m_pInfoWindowListener = GFL_NEW(devHeap) ResortInfoWindowListener( m_pBase, m_pPaneList );

    m_pInfoWindow->SyncCreate( heap, pAppRenderingManager, NULL );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //----------------------------------------------------------------------------
  ResortInfoWindow::~ResortInfoWindow( void )
  {
    for( u32 i=0; i<STR_NUM; ++i )
    {
      GFL_SAFE_DELETE( m_pStrBuf[i] );
    }
  }


  //----------------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //----------------------------------------------------------------------------
  void ResortInfoWindow::Update( void )
  {
    if( m_pInfoWindow )
    {
      m_pInfoWindow->Update();
    }
  }
  
  //----------------------------------------------------------------------------
  /**
   * @brief   描画
   */
  //----------------------------------------------------------------------------
  void ResortInfoWindow::Draw( gfl2::gfx::CtrDisplayNo displayNo, u8 order, u32 layerNo )
  {
    if( m_pInfoWindow )
    {
      m_pInfoWindow->Draw( displayNo, order, layerNo );
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   準備完了か？
   */
  //----------------------------------------------------------------------------
  bool ResortInfoWindow::IsReady( void )
  {
    if( m_pInfoWindow && !m_pInfoWindow->IsCreated() ) return false;

    return true;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   終了シーケンス
   * @return  "true" == 終了
   * @return  "false" == 終了処理中
   */
  //----------------------------------------------------------------------------
  bool ResortInfoWindow::DeleteSequence( void )
  {
    switch( m_DeleteSeq )
    {
    case 0:
      {
        m_DeleteSeq++;
      }

    case 1:
      {
        if( !m_pInfoWindow->EndFunc() ) break;

        GFL_SAFE_DELETE( m_pInfoWindow );
        GFL_SAFE_DELETE( m_pInfoWindowListener );

        m_DeleteSeq++;
      }

    case 2:
      {
        return true;
      }
    }

    return false;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   インフォメーションウィンドウを開く
   * @param   itemNo    アイテムNo
   * @param   btnPos    ボタンの位置座標
   */
  //----------------------------------------------------------------------------
  void ResortInfoWindow::Open( u16 itemNo, gfl2::math::Vector3 btnPos )
  {
    if( m_pInfoWindow == NULL ) return;
    if( itemNo == ITEM_DUMMY_DATA ) return;

    print::WordSet* wordSet = m_pBase->GetWordSet();
    gfl2::str::MsgData* msgData = m_pBase->GetMsgData();

    // アイテム名
    gfl2::str::StrBuf* nameBuf = m_pBase->GetG2D()->GetTempStrBuf(0);
    {
      wordSet->RegisterItemName( 0, itemNo, 1 );
      msgData->GetString( msg_prsys_common_05, *nameBuf );
      wordSet->Expand( m_pStrBuf[STR_ITEM_NAME], nameBuf );
    }
    // アイテム説明
    gfl2::str::StrBuf* descBuf = m_pBase->GetG2D()->GetTempStrBuf(0);
    {
      m_pItemInfo->GetString( itemNo, *descBuf );
      wordSet->Expand( m_pStrBuf[STR_ITEM_INFO], descBuf );
    }

    // インフォメーションウィンドウを開く
    m_pInfoWindow->StartOpen(
      m_pInfoWindowListener, btnPos, m_pStrBuf[STR_ITEM_NAME], m_pStrBuf[STR_ITEM_INFO] );

    // 入力を無効に
    app::ui::UIView* pUIView = m_pBase->GetUIView();
    pUIView->SetInputEnabled( false );

    m_pPaneList->SetInputEnable( false );
  }

GFL_NAMESPACE_END( Resort )
GFL_NAMESPACE_END( App )
