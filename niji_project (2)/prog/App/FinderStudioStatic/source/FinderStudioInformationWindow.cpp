//======================================================================
/**
 * @file    FinderStudioInformationWindow.cpp
 * @date    2017/01/11 16:41:55
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ：インフォメーションウィンドウ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <App/FinderStudioStatic/include/FinderStudioInformationWindow.h>

#include <AppLib/include/UI/UIView.h>
#include <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookType.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Static )

//////////////////////////////////////////////////////////////////////////
/// ---[ 変数の初期化 begin ]---
app::util::Heap* InformationWindow::m_pHeap = NULL;
app::util::AppRenderingManager* InformationWindow::m_pAppRenderingManager = NULL;
app::tool::InfoWindowBookType* InformationWindow::m_pInfoWindow = NULL;
u8 InformationWindow::m_Seq = 0;
/// ---[ 変数の初期化 end ]---
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///
/// @brief    生成・破棄：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   生成
 */
void InformationWindow::_CreateInfoWindow( u8 pageMax )
{
  GFL_ASSERT( m_pHeap );
  GFL_ASSERT( m_pAppRenderingManager );
  GFL_ASSERT( m_pInfoWindow == NULL );

  // InfoWindowBookTypeの生成
  m_pInfoWindow = GFL_NEW_LOW( m_pHeap->GetDeviceHeap() ) app::tool::InfoWindowBookType( m_pHeap );
  m_pInfoWindow->SyncCreate( m_pHeap, m_pAppRenderingManager, pageMax, NULL );
  m_pInfoWindow->ResetContents( pageMax, true );
}

/**
 * @brief   破棄
 */
bool InformationWindow::_DeleteInfoWindow( void )
{
  if( !m_pInfoWindow->EndFunc() )
  {
    return false;
  }

  GFL_SAFE_DELETE( m_pInfoWindow );

  return true;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    SubView：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   SubViewへ追加
 *
 * @param[in]   view    親View
 */
void InformationWindow::_AddSubView( app::ui::UIView* view )
{
  GFL_ASSERT( m_pInfoWindow );

  // SubViewへ追加
  view->AddSubView( m_pInfoWindow );
}

/**
 * @brief   SubViewの解除
 */
void InformationWindow::_RemoveSubView( void )
{
  GFL_ASSERT( m_pInfoWindow );

  // SubViewを解除
  m_pInfoWindow->RemoveFromSuperView();
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    更新・描画
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   更新
 */
void InformationWindow::Update( void )
{
  /// インフォウィンドウの更新
  if( m_pInfoWindow )
  {
    /// 更新
    if( !m_pInfoWindow->GetSuperView() )
    {
      m_pInfoWindow->Update();
    }

    /// インフォウィンドウの解除
    if( m_Seq == SEQ_INFO_REMOVE )
    {
      _RemoveSubView();
      m_Seq = SEQ_INFO_DELETE;
    }
    /// インフォウィンドウの破棄
    else if( m_Seq == SEQ_INFO_DELETE )
    {
      if( _DeleteInfoWindow() )
      {
        m_Seq = SEQ_INFO_IDLE;
      }
    }
  }
}

/**
 * @brief   描画
 */
void InformationWindow::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  /// インフォウィンドウの描画
  if( m_pInfoWindow && !m_pInfoWindow->GetSuperView() )
  {
    m_pInfoWindow->Draw( displayNo, gfl2::lyt::ORDER_LAST, 0 );
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    セットアップ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   セットアップ
 *
 * @param   heap        ヒープ
 * @param   renderMan   レンダリングマネージャ
 */
void InformationWindow::SetupInfoWindow( app::util::Heap* heap, app::util::AppRenderingManager* renderMan )
{
  m_pHeap = heap;
  m_pAppRenderingManager = renderMan;
}

/**
 * @brief   セットアップの解除
 */
void InformationWindow::ResetInfoWindow( void )
{
  GFL_ASSERT( m_pInfoWindow == NULL );

  m_pAppRenderingManager = NULL;
  m_pHeap = NULL;
}

/**
 * @brief   コンテンツセットアップ
 *
 * @param   pageMax   ページ数
 * @param   texArray  テクスチャ
 */
void InformationWindow::SetupContents( u8 pageMax, gfl2::str::StrBuf** strArray )
{
  // 生成
  _CreateInfoWindow( pageMax );

  for( u8 i = 0; i < pageMax; ++i )
  {
    GFL_ASSERT( strArray[i] );
    m_pInfoWindow->SetTextContent( i, strArray[i], NULL );
  }
}

/**
 * @brief   コンテンツセットアップ
 *
 * @param   pageMax   ページ数
 * @param   strArray  文字列
 * @param   texArray  テクスチャ
 */
void InformationWindow::SetupContents( u8 pageMax, gfl2::str::StrBuf** strArray, const gfl2::lyt::LytResTextureInfo** texArray )
{
  // 生成
  _CreateInfoWindow( pageMax );

  for( u8 i = 0; i < pageMax; ++i )
  {
    GFL_ASSERT( strArray[i] );
    GFL_ASSERT( texArray[i] );
    m_pInfoWindow->SetTextureAndTextContent( i, texArray[i], strArray[i], NULL );
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    表示・非表示
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   表示
 */
void InformationWindow::Show( app::tool::InfoWindowBookTypeListener* listener, gfl2::math::VEC3 pos )
{
  GFL_ASSERT( m_pInfoWindow );
  GFL_ASSERT( listener );

  gfl2::math::Vector3 vec = gfl2::math::ConvertNwVec3ToGfVec3( pos );

  m_pInfoWindow->StartOpen( listener, vec );
}

/**
 * @brief   非表示
 *
 * @note    OnCloseのイベント時に呼ぶ事で自動的に破棄することが出来る。
 */
void InformationWindow::Hide( void )
{
  if( m_pInfoWindow )
  {
    m_Seq = SEQ_INFO_REMOVE;

    FUKUSHIMA_PRINT( "InformationWindow::Hide\n" );
  }
}

GFL_NAMESPACE_END( Static )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
