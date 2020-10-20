//======================================================================
/**
 * @file    PokemonInfoWindow.cpp
 * @date    2015/12/03 21:31:59
 * @author  fukushima_yuya
 * @brief   ポケモンステータス：インフォメーションウィンドウ(下)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "StatusInfoWindow.h"

#include <App/Status/source/StatusDrawerBase.h>

#include <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowSheetType.h>

// arc
#include <arc_index/message.gaix>
#include <niji_conv_header/app/field_status/field_status_pane.h>
#include <niji_conv_header/message/msg_pokemon_status.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)

//==============================================================================
// @brief   インフォメーションウィンドウクラス
//==============================================================================

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
InfoWindow::InfoWindow( DrawerBase* base, app::util::AppRenderingManager* renderMan )
  : m_pBase(base)
  , m_pAppRenderingManager(renderMan)
  , m_pInfoWindow(NULL)
  , m_pInfoWindowListener(NULL)
  , m_pPokeParam(NULL)
  , m_DeleteSeq(0)
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
InfoWindow::~InfoWindow( void )
{
  for( u32 i=0; i<STR_NUM; ++i )
  {
    GFL_SAFE_DELETE( m_pStrBuf[i] );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   セットアップ
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void InfoWindow::Setup( const pml::pokepara::PokemonParam* pp )
{
  //if( m_pPokeParam != pp )
  {
    m_pPokeParam = pp;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   初期化
 *
 * @param   tokuseiMsgData  特性メッセージデータ
 * @param   itemMsgData     アイテムメッセージデータ
 * @param   listener        インフォウィンドウリスナー
 */
//------------------------------------------------------------------------------
void InfoWindow::Initialize( gfl2::str::MsgData* tokuseiMsgData, gfl2::str::MsgData* itemMsgData, app::tool::InfoWindowSheetTypeListener* listener )
{
  m_pInfoWindowListener = listener;

  // メッセージデータのセット
  {
    m_pInfoMsg[MSG_TOKUSEI] = tokuseiMsgData;
    m_pInfoMsg[MSG_ITEM]    = itemMsgData;
  }

  // 開始座標の取得
  {
    // 特性インフォメーションボタン
    gfl2::lyt::LytPane* tokuseiInfoPane = m_pBase->GetPane( PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_INFO_00 );
    {
      gfl2::lyt::LytPane* parentPane = m_pBase->GetPane( PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_TEXT_TOKUSEI );
      {
        gfl2::math::Vector3 parentPos = gfl2::math::ConvertNwVec3ToGfVec3( parentPane->GetTranslate() );
        gfl2::math::Vector3 childPos  = gfl2::math::ConvertNwVec3ToGfVec3( tokuseiInfoPane->GetTranslate() );

        m_TokuseiInfoPos = parentPos + childPos;
      }
    }
    // アイテムインフォメーションボタン
    gfl2::lyt::LytPane* itemInfoPane = m_pBase->GetPane( PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_INFO_01 );
    {
      gfl2::lyt::LytPane* parentPane = m_pBase->GetPane( PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_TEXT_ITEM );
      {
        gfl2::math::Vector3 parentPos = gfl2::math::ConvertNwVec3ToGfVec3( parentPane->GetTranslate() );
        gfl2::math::Vector3 childPos  = gfl2::math::ConvertNwVec3ToGfVec3( itemInfoPane->GetTranslate() );

        m_ItemInfoPos = parentPos + childPos;
      }
    }
  }

  // 文字列の格納バッファの生成
  CreateStrBuf();

  // インフォメーションウィンドウの生成
  CreateInfoWindow();
}

//------------------------------------------------------------------------------
/**
 * @brief   文字列の格納バッファの生成
 */
//------------------------------------------------------------------------------
void InfoWindow::CreateStrBuf( void )
{
  gfl2::heap::HeapBase* devHeap = m_pBase->GetDeviceHeap();

  const u32 c_CharMax[] = { 64, 256 };

  for( u32 i=0; i<STR_NUM; ++i )
  {
    m_pStrBuf[i] = GFL_NEW(devHeap) gfl2::str::StrBuf( c_CharMax[i], devHeap );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   インフォメーションウィンドウの生成
 */
//------------------------------------------------------------------------------
void InfoWindow::CreateInfoWindow( void )
{
  app::util::Heap* heap = m_pBase->GetHeap();
  gfl2::heap::HeapBase* devHeap = m_pBase->GetDeviceHeap();

  // インフォウィンドウの生成
  m_pInfoWindow = GFL_NEW(devHeap) app::tool::InfoWindowSheetType( heap );
  {
    GFL_ASSERT( m_pInfoWindow );

    //// インフォウィンドウリスナーの生成
    //m_pInfoWindowListener = GFL_NEW(devHeap) InfoWindowListener( m_pBase );
    //GFL_ASSERT( m_pInfoWindowListener );

    // 同期生成
    m_pInfoWindow->SyncCreate( heap, m_pAppRenderingManager, NULL );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void InfoWindow::Update( void )
{
  if( m_pInfoWindow )
  {
    m_pInfoWindow->Update();
  }
}
  
//------------------------------------------------------------------------------
/**
 * @brief   描画
 */
//------------------------------------------------------------------------------
void InfoWindow::Draw( gfl2::gfx::CtrDisplayNo displayNo, u8 order, u32 layerNo )
{
  if( m_pInfoWindow )
  {
    m_pInfoWindow->Draw( displayNo, order, layerNo );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   準備完了か？
 */
//------------------------------------------------------------------------------
bool InfoWindow::IsReady( void )
{
  if( m_pInfoWindow )
  {
    if( !m_pInfoWindow->IsCreated() ) return false;
  }

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   終了シーケンス
 *
 * @return  "true  = 終了"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool InfoWindow::DeleteSequence( void )
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


//------------------------------------------------------------------------------
/**
 * @brief   特性のインフォメーションウィンドウを開く
 */
//------------------------------------------------------------------------------
void InfoWindow::OpenTokusei( void )
{
  GFL_ASSERT( m_pPokeParam );

  app::util::G2DUtil* g2d     = m_pBase->GetG2D();
  gfl2::str::MsgData* msgData = m_pBase->GetMsgData();
  print::WordSet*     wordSet = m_pBase->GetWordSet();

  gfl2::str::StrBuf* nameTemp = g2d->GetTempStrBuf(0);
  gfl2::str::StrBuf* descTemp = g2d->GetTempStrBuf(0);

  m_OpenInfoPos = m_TokuseiInfoPos;

  // 特性No
  TokuseiNo tokusei = m_pPokeParam->GetTokuseiNo();

  // 特性名
  wordSet->RegisterTokuseiName( 0, tokusei );
  msgData->GetString( PARAM_TOKUSEI_VALUE, *nameTemp );
  wordSet->Expand( m_pStrBuf[STR_NAME], nameTemp );

  // 特性説明文
  m_pInfoMsg[MSG_TOKUSEI]->GetString( tokusei, *descTemp );
  wordSet->Expand( m_pStrBuf[STR_DESC], descTemp );

  // インフォメーションウィンドウを開く
  OpenInfoWindow( m_pStrBuf[STR_NAME], m_pStrBuf[STR_DESC] );
}

//------------------------------------------------------------------------------
/**
 * @brief   道具のインフォメーションウィンドウを開く
 */
//------------------------------------------------------------------------------
void InfoWindow::OpenItem( void )
{
  GFL_ASSERT( m_pPokeParam );

  // アイテムNo
  u16 itemNo = m_pPokeParam->GetItem();

  if( itemNo != ITEM_DUMMY_DATA )
  {
    app::util::G2DUtil* g2d     = m_pBase->GetG2D();
    gfl2::str::MsgData* msgData = m_pBase->GetMsgData();
    print::WordSet*     wordSet = m_pBase->GetWordSet();

    gfl2::str::StrBuf* nameTemp = g2d->GetTempStrBuf(0);
    gfl2::str::StrBuf* descTemp = g2d->GetTempStrBuf(0);

    m_OpenInfoPos = m_ItemInfoPos;

    // 特性名
    wordSet->RegisterItemName( 0, itemNo, 1 );
    msgData->GetString( PARAM_ITEM_VALUE, *nameTemp );
    wordSet->Expand( m_pStrBuf[STR_NAME], nameTemp );

    // 特性説明文
    m_pInfoMsg[MSG_ITEM]->GetString( itemNo, *descTemp );
    wordSet->Expand( m_pStrBuf[STR_DESC], descTemp );

    // インフォメーションウィンドウを開く
    OpenInfoWindow( m_pStrBuf[STR_NAME], m_pStrBuf[STR_DESC] );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   インフォメーションウィンドウを開く
 *
 * @param   name  項目名
 * @param   desc  説明文
 */
//------------------------------------------------------------------------------
void InfoWindow::OpenInfoWindow( gfl2::str::StrBuf* name, gfl2::str::StrBuf* desc )
{
  if( m_pInfoWindow == NULL ) return;

  // インフォメーションウィンドウを開く
  m_pInfoWindow->StartOpen( m_pInfoWindowListener, m_OpenInfoPos, name, desc );

  // 入力を無効に
  app::ui::UIView* view = m_pBase->GetUIView();
  view->SetInputEnabled( false );
}

GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)
