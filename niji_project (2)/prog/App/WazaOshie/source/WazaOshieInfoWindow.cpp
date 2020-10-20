//======================================================================
/**
 * @file WazaOshieInfoWindow.cpp
 * @date 2015/07/16 12:32:50
 * @author fukushima_yuya
 * @brief 技教え・技忘れ画面のインフォメーションウィンドウ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "App/WazaOshie/source/WazaOshieInfoWindow.h"
#include "App/WazaOshie/source/WazaOshieDrawerBase.h"

#include <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowSheetType.h>

// arc
#include <arc_index/message.gaix>
#include <niji_conv_header/app/common_status/common_status_pane.h>
#include <niji_conv_header/message/msg_pokemon_status.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( WazaOshie )


//==============================================================================
// @brief   インフォメーションウィンドウクラス
//==============================================================================
  //----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //----------------------------------------------------------------------------
  WazaOshieInfoWindow::WazaOshieInfoWindow( DrawerBase* pBase, app::util::AppRenderingManager* pAppRenderingManager, WazaOshieInfoWindowListener* listener )
    : m_pBase( pBase )
    , m_pInfoWindow( NULL )
    , m_pInfoWindowListener( NULL )
    , m_DeleteSeq( 0 )
  {
    app::util::Heap* heap = m_pBase->GetHeap();
    gfl2::heap::HeapBase* devHeap = m_pBase->GetDeviceHeap();

    const u32 c_DataID[] = {
      GARC_message_tokuseiinfo_DAT,
      GARC_message_iteminfo_DAT,
    };

    for( u32 i=0; i<MSG_NUM; ++i )
    {
      m_pInfoMsg[i] = GFL_NEW(devHeap) gfl2::str::MsgData(
        print::GetMessageArcId(), c_DataID[i], devHeap, gfl2::str::MsgData::LOAD_FULL );
    }

    const u32 c_CharMax[] = { 64, 256 };

    for( u32 i=0; i<STR_NUM; ++i )
    {
      m_pStrBuf[i] = GFL_NEW(devHeap) gfl2::str::StrBuf( c_CharMax[i], devHeap );
    }

    // InfoWindowSheetType の生成
    m_pInfoWindow = GFL_NEW(devHeap) app::tool::InfoWindowSheetType( heap );

    // WazaOshieInfoWindowListener の生成
    //m_pInfoWindowListener = GFL_NEW(devHeap) WazaOshieInfoWindowListener( m_pBase );
    m_pInfoWindowListener = listener;

    m_pInfoWindow->SyncCreate( heap, pAppRenderingManager, NULL );

    // VIEWの追加
    app::ui::UIView* pUIView = m_pBase->GetUIView();
    pUIView->AddSubView( m_pInfoWindow );

    {
      // 特性インフォメーションボタン
      gfl2::lyt::LytPane* tokuseiParentPane =
        m_pBase->GetPane( PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_TEXT_TOKUSEI );
      {
        gfl2::lyt::LytPane* tokuseiChildPane =
          m_pBase->GetPane( PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_INFO_00 );
        {
          m_TokuseiInfoPos = gfl2::math::ConvertNwVec3ToGfVec3(
            tokuseiParentPane->GetTranslate() + tokuseiChildPane->GetTranslate() );
        }
      }
      // アイテムインフォメーションボタン
      gfl2::lyt::LytPane* itemParentPane =
        m_pBase->GetPane( PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_TEXT_ITEM );
      {
        gfl2::lyt::LytPane* itemChildPane =
          m_pBase->GetPane( PANENAME_COMMONSTATUS_SKL_LOW_000_PANE_INFO_01 );
        {
          m_ItemInfoPos = gfl2::math::ConvertNwVec3ToGfVec3(
            itemParentPane->GetTranslate() + itemChildPane->GetTranslate() );
        }
      }
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //----------------------------------------------------------------------------
  WazaOshieInfoWindow::~WazaOshieInfoWindow( void )
  {
    for( u32 i=0; i<STR_NUM; ++i )
    {
      GFL_SAFE_DELETE( m_pStrBuf[i] );
    }
    for( u32 i=0; i<MSG_NUM; ++i )
    {
      GFL_SAFE_DELETE( m_pInfoMsg[i] );
    }
  }


  //----------------------------------------------------------------------------
  /**
   * @brief   準備完了か？
   */
  //----------------------------------------------------------------------------
  bool WazaOshieInfoWindow::IsReady( void )
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
  bool WazaOshieInfoWindow::DeleteSequence( void )
  {
    switch( m_DeleteSeq )
    {
    case 0:
      {
        m_pInfoWindow->RemoveFromSuperView();
        m_DeleteSeq++;
      }

    case 1:
      {
        if( !m_pInfoWindow->EndFunc() ) break;

        GFL_SAFE_DELETE( m_pInfoWindow );
        //GFL_SAFE_DELETE( m_pInfoWindowListener );

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
   * @brief   特性のインフォメーションウィンドウを開く
   * @param   tokusei   特性ID
   */
  //----------------------------------------------------------------------------
  void WazaOshieInfoWindow::OpenTokusei( TokuseiNo tokusei )
  {
    app::util::G2DUtil* pG2D = m_pBase->GetG2D();
    gfl2::str::MsgData* pMsgData = m_pBase->GetMsgData();
    print::WordSet* pWordSet = m_pBase->GetWordSet();

    gfl2::str::StrBuf* nameTemp = pG2D->GetTempStrBuf(0);
    gfl2::str::StrBuf* descTemp = pG2D->GetTempStrBuf(0);

    m_OpenInfoPos = m_TokuseiInfoPos;

    // 特性名
    pWordSet->RegisterTokuseiName( 0, tokusei );
    pMsgData->GetString( PARAM_TOKUSEI_VALUE, *nameTemp );
    pWordSet->Expand( m_pStrBuf[STR_NAME], nameTemp );

    // 特性説明文
    m_pInfoMsg[MSG_TOKUSEI]->GetString( tokusei, *descTemp );
    pWordSet->Expand( m_pStrBuf[STR_DESC], descTemp );

    // インフォメーションウィンドウを開く
    OpenInfoWindow( m_pStrBuf[STR_NAME], m_pStrBuf[STR_DESC] );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   道具のインフォメーションウィンドウを開く
   * @param   itemNo    道具ID
   */
  //----------------------------------------------------------------------------
  void WazaOshieInfoWindow::OpenItem( u16 itemNo )
  {
    app::util::G2DUtil* pG2D = m_pBase->GetG2D();
    gfl2::str::MsgData* pMsgData = m_pBase->GetMsgData();
    print::WordSet* pWordSet = m_pBase->GetWordSet();

    gfl2::str::StrBuf* nameTemp = pG2D->GetTempStrBuf(0);
    gfl2::str::StrBuf* descTemp = pG2D->GetTempStrBuf(0);

    m_OpenInfoPos = m_ItemInfoPos;

    // 特性名
    pWordSet->RegisterItemName( 0, itemNo, 1 );
    pMsgData->GetString( PARAM_ITEM_VALUE, *nameTemp );
    pWordSet->Expand( m_pStrBuf[STR_NAME], nameTemp );

    // 特性説明文
    m_pInfoMsg[MSG_ITEM]->GetString( itemNo, *descTemp );
    pWordSet->Expand( m_pStrBuf[STR_DESC], descTemp );

    // インフォメーションウィンドウを開く
    OpenInfoWindow( m_pStrBuf[STR_NAME], m_pStrBuf[STR_DESC] );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   インフォメーションウィンドウを開く
   * @param   name  項目名
   * @param   desc  説明文
   */
  //----------------------------------------------------------------------------
  void WazaOshieInfoWindow::OpenInfoWindow( gfl2::str::StrBuf* name, gfl2::str::StrBuf* desc )
  {
    if( m_pInfoWindow == NULL ) return;

    // インフォメーションウィンドウを開く
    m_pInfoWindow->StartOpen( m_pInfoWindowListener, m_OpenInfoPos, name, desc );

    // 入力を無効に
    app::ui::UIView* pUIView = m_pBase->GetUIView();
    pUIView->SetInputEnabled( false );
  }


GFL_NAMESPACE_END( WazaOshie )
GFL_NAMESPACE_END( App )
