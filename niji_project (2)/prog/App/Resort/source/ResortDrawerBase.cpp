//======================================================================
/**
 * @file ResortDrawerBase.cpp
 * @date 2015/09/08 20:48:03
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "ResortDrawerBase.h"

#include <AppLib/include/Ui/UIView.h>
#include <AppLib/include/Util/app_util_heap.h>

//gflib2
#include <ui/include/gfl2_UI.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)

//==============================================================================
// @brief   共通描画クラス
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
DrawerBase::DrawerBase( app::ui::UIView* pUIView, app::util::Heap* pHeap, u32 layout_id, print::WordSet* pWordSet, gfl2::str::MsgData* pMsgData )
  : m_pUIView( pUIView )
  , m_pHeap( pHeap )
  , m_LayoutID( layout_id )
  , m_pLytWk( m_pUIView->GetLayoutWork( m_LayoutID ) )
  , m_ResID( GetG2D()->GetLayoutResourceID( 0 ) )
  , m_pWordSet( pWordSet )
  , m_pMsgData( pMsgData )
  , m_pPokeIcon( NULL )
  , m_pItemIcon( NULL )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
DrawerBase::~DrawerBase( void )
{
  ;
}




//------------------------------------------------------------------------------
// @brief   ペイン関連
//------------------------------------------------------------------------------

  //----------------------------------------------------------------------------
  /**
   * @brief   ペインの取得
   * @param   index   ペインインデックス
   * @param   pParts  部品ペイン(デフォルト = NULL)
   * @return  gfl2::lyt::LytPaneクラスポインタ
   */
  //----------------------------------------------------------------------------
  gfl2::lyt::LytPane* DrawerBase::GetPane( gfl2::lyt::LytPaneIndex index, gfl2::lyt::LytParts* pParts )
  {
    if( pParts )
    {
      return m_pLytWk->GetPane( pParts, index, &m_ResID );
    }

    return m_pLytWk->GetPane( index );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   テキストボックスペインの取得
   * @param   index   ペインインデックス
   * @param   pParts  部品ペイン(デフォルト = NULL)
   * @return  gfl2::lyt::LytTextBoxクラスポインタ
   */
  //----------------------------------------------------------------------------
  gfl2::lyt::LytTextBox* DrawerBase::GetTextBox( gfl2::lyt::LytPaneIndex index, gfl2::lyt::LytParts* pParts )
  {
    if( pParts )
    {
      return m_pLytWk->GetTextBoxPane( pParts, index, &m_ResID );
    }

    return m_pLytWk->GetTextBoxPane( index );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   境界ペインの取得
   * @param   index   ペインインデックス
   * @param   pParts  部品ペイン(デフォルト = NULL)
   * @return  gfl2::lyt::LytBoundingクラスポインタ
   */
  //----------------------------------------------------------------------------
  gfl2::lyt::LytBounding* DrawerBase::GetBounding( gfl2::lyt::LytPaneIndex index, gfl2::lyt::LytParts* pParts )
  {
    if( pParts )
    {
      return m_pLytWk->GetBoundingPane( pParts, index, &m_ResID );
    }

    return m_pLytWk->GetBoundingPane( index );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   ピクチャーペインの取得
   * @param   index   ペインインデックス
   * @param   pParts  部品ペイン(デフォルト = NULL)
   * @return  gfl2::lyt::LytPictureクラスポインタ
   */
  //----------------------------------------------------------------------------
  gfl2::lyt::LytPicture* DrawerBase::GetPicture( gfl2::lyt::LytPaneIndex index, gfl2::lyt::LytParts* pParts )
  {
    if( pParts )
    {
      return m_pLytWk->GetPicturePane( pParts, index, &m_ResID );
    }

    return m_pLytWk->GetPicturePane( index );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   部品ペインの取得
   * @param   index   ペインインデックス
   * @param   pParts  部品ペイン(デフォルト = NULL)
   * @return  gfl2::lyt::LytPartsクラスポインタ
   */
  //----------------------------------------------------------------------------
  gfl2::lyt::LytParts* DrawerBase::GetParts( gfl2::lyt::LytPaneIndex index, gfl2::lyt::LytParts* pParts )
  {
    if( pParts )
    {
      return m_pLytWk->GetPartsPane( pParts, index, &m_ResID );
    }

    return m_pLytWk->GetPartsPane( index );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   ペインの表示切替
   * @param   pPane     切り替え対象ペイン
   * @param   viisible  表示フラグ
   */
  //----------------------------------------------------------------------------
  void DrawerBase::SetVisiblePane( gfl2::lyt::LytPane* pPane, bool visible )
  {
    if( pPane == NULL )
    {
      GFL_ASSERT(0);
      return;
    }

    GetG2D()->SetPaneVisible( pPane, visible );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   ペインのタッチ判定
   */
  //----------------------------------------------------------------------------
  bool DrawerBase::IsHoldPane( gfl2::ui::TouchPanel* pTouchPanel, gfl2::lyt::LytPane* pPane ) const
  {
    if( pTouchPanel == NULL )
    {
      GFL_ASSERT(0);
      return false;
    }
    if( pPane == NULL )
    {
      GFL_ASSERT(0);
      return false;
    }

    return m_pUIView->GetToucHoldPane( pTouchPanel, m_LayoutID, pPane );
  }



//------------------------------------------------------------------------------
// @brief   アニメーション関連
//------------------------------------------------------------------------------

  //----------------------------------------------------------------------------
  /**
   * @brief   アニメーションの再生
   * @param   anmID         アニメーションID
   * @param   auto_anim     オートアニメーションするか(デフォルト = true)
   * @param   auto_unbind   終了時に自動でアンバインドするか(デフォルト = true)
   */
  //----------------------------------------------------------------------------
  void DrawerBase::StartAnim( u32 anmID, bool auto_anim, bool auto_unbind )
  {
    GetG2D()->StartAnime( m_LayoutID, anmID, auto_anim, auto_unbind );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   アニメーションの停止
   * @param   anmID   アニメーションID
   * @param   clear   アニメの初期化フラグ(デフォルト = false)
   */
  //----------------------------------------------------------------------------
  void DrawerBase::StopAnim( u32 anmID, bool clear )
  {
    GetG2D()->StopAnime( m_LayoutID, anmID, clear );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   アニメーションの終了判定
   * @param   anmID   アニメーションID
   */
  //----------------------------------------------------------------------------
  bool DrawerBase::IsAnimEnd( u32 anmID )
  {
    return GetG2D()->IsAnimeEnd( m_LayoutID, anmID );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   アニメーションのフレームを設定
   * @param   anmID   アニメーションID
   * @param   frame   アニメフレーム
   */
  //----------------------------------------------------------------------------
  void DrawerBase::SetAnimFrame( u32 anmID, f32 frame )
  {
    GetG2D()->SetAnimeFrame( m_LayoutID, anmID, frame );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   アニメーションのフレームを設定
   * @param   anmID   アニメーションID
   * @return  現在のアニメフレーム
   */
  //----------------------------------------------------------------------------
  f32 DrawerBase::GetAnimFrame( u32 anmID ) const
  {
    return GetG2D()->GetAnimeFrame( m_LayoutID, anmID );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   アニメーションの切り替え
   * @param   stop    停止するアニメID
   * @param   start   再生するアニメID
   */
  //----------------------------------------------------------------------------
  void DrawerBase::ChangeAnim( u32 stop, u32 start )
  {
    StopAnim( stop );
    StartAnim( start );
  }


//------------------------------------------------------------------------------
// @brief   取得関連
//------------------------------------------------------------------------------

  //----------------------------------------------------------------------------
  /**
   * @brief   UIViewの取得
   * return   app::ui::UIViewクラスポインタ
   */
  //----------------------------------------------------------------------------
  app::ui::UIView* DrawerBase::GetUIView( void ) const
  {
    return m_pUIView;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   レイアウトリソースの取得
   * return   gfl2::lyt::LytResクラスポインタ
   */
  //----------------------------------------------------------------------------
  gfl2::lyt::LytRes* DrawerBase::GetLytRes( void ) const
  {
    return m_pUIView->GetLayoutSystem()->GetResource( &m_ResID );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   レイアウトワークの取得
   * return   gfl2::lyt::LytWkクラスポインタ
   */
  //----------------------------------------------------------------------------
  gfl2::lyt::LytWk* DrawerBase::GetLytWk( void ) const
  {
    return m_pLytWk;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   ヒープの取得
   * return   app::util::Heapクラスポインタ
   */
  //----------------------------------------------------------------------------
  app::util::Heap* DrawerBase::GetHeap( void ) const
  {
    return m_pHeap;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   システムヒープの取得
   * return   gfl2::heap::HeapBaseクラスポインタ
   */
  //----------------------------------------------------------------------------
  gfl2::heap::HeapBase* DrawerBase::GetSystemHeap( void ) const
  {
    return m_pHeap->GetSystemHeap();
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   デバイスヒープの取得
   * return   gfl2::heap::HeapBaseクラスポインタ
   */
  //----------------------------------------------------------------------------
  gfl2::heap::HeapBase* DrawerBase::GetDeviceHeap( void ) const
  {
    return m_pHeap->GetDeviceHeap();
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   G2DUtilの取得
   * @return  app::util::G2DUtilクラスポインタ
   */
  //----------------------------------------------------------------------------
  app::util::G2DUtil* DrawerBase::GetG2D( void ) const
  {
    return m_pUIView->GetG2DUtil();
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   ボタンマネージャの取得
   * @return  app::tool::ButtonManagerクラスポインタ
   */
  //----------------------------------------------------------------------------
  app::tool::ButtonManager* DrawerBase::GetButtonManager( void ) const
  {
    return m_pUIView->GetButtonManager();
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   ワードセットの取得
   * @return  print::WordSetクラスポインタ
   */
  //----------------------------------------------------------------------------
  print::WordSet* DrawerBase::GetWordSet( void ) const
  {
    return m_pWordSet;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   メッセージデータの取得
   * @return  gfl2::str::StrBufクラスポインタ
   */
  //----------------------------------------------------------------------------
  gfl2::str::MsgData* DrawerBase::GetMsgData( void ) const
  {
    return m_pMsgData;
  }



//------------------------------------------------------------------------------
// @brief   ポケモンアイコン
//------------------------------------------------------------------------------

  //----------------------------------------------------------------------------
  /**
   * @brief   ポケモンアイコンの生成
   * @param   param   生成パラメータ
   * @param   max     最大アイコン数
   * @param   isDummy ダミー
   */
  //----------------------------------------------------------------------------
  void DrawerBase::CreatePokeIcon( app::tool::PokeIconToolEx::PokeIconToolParam param )
  {
    m_pPokeIcon = GFL_NEW(param.pHeap->GetDeviceHeap()) app::tool::PokeIconToolEx( param );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   ポケモンアイコンの破棄
   */
  //----------------------------------------------------------------------------
  void DrawerBase::DeletePokeIcon( void )
  {
    if( m_pPokeIcon )
    {
      GFL_SAFE_DELETE( m_pPokeIcon );
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   ポケモンアイコンの取得
   * @return  app::tool::PokeIconToolのポインタ
   */
  //----------------------------------------------------------------------------
  app::tool::PokeIconToolEx* DrawerBase::GetPokeIcon( void )
  {
    return m_pPokeIcon;
  }



//------------------------------------------------------------------------------
// @brief   アイテムアイコン
//------------------------------------------------------------------------------

  //----------------------------------------------------------------------------
  /**
   * @brief   アイテムアイコンの生成
   * @param   param   生成パラメータ
   */
  //----------------------------------------------------------------------------
  void DrawerBase::CreateItemIcon( app::tool::ItemIconToolEx::ItemIconToolParam param )
  {
    m_pItemIcon = GFL_NEW(param.pHeap->GetDeviceHeap()) app::tool::ItemIconToolEx( param );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   アイテムアイコンの破棄
   */
  //----------------------------------------------------------------------------
  void DrawerBase::DeleteItemIcon( void )
  {
    if( m_pItemIcon )
    {
      GFL_SAFE_DELETE( m_pItemIcon );
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   アイテムアイコンの取得
   * @return  app::tool::ItemIconToolのポインタ
   */
  //----------------------------------------------------------------------------
  app::tool::ItemIconToolEx* DrawerBase::GetItemIcon( void )
  {
    return m_pItemIcon;
  }

GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)
