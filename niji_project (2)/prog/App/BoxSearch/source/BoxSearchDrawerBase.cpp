//======================================================================
/**
 * @file    BoxSearchDrawerBase.cpp
 * @date    2015/07/27 12:59:01
 * @author  fukushima_yuya
 * @brief   ボックス検索：描画共通処理
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "BoxSearchDrawerBase.h"

#include <AppLib/include/Tool/app_tool_MenuCursor.h>
#include <AppLib/include/Ui/UIView.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/app_util_2d.h>

//gflib2
#include <ui/include/gfl2_UI.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BoxSearch)

//==============================================================================
// @brief   共通描画クラス
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
DrawerBase::DrawerBase( app::ui::UIView* pUIView, app::util::Heap* pHeap, u32 layout_id, gfl2::str::MsgData* pMsgData, print::WordSet* pWordSet )
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
   * @brief   位置の設定
   * @param   pPane   対象ペイン
   * @param   pos     更新座標値
   */
  //----------------------------------------------------------------------------
  void DrawerBase::SetPanePos( gfl2::lyt::LytPane* pPane, gfl2::math::VEC3 pos )
  {
    if( pPane == NULL )
    {
      GFL_ASSERT(0);
      return;
    }

    pPane->SetTranslate( pos );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   位置の取得
   * @param   pPane   対象ペイン
   * @return  現在座標値
   */
  //----------------------------------------------------------------------------
  gfl2::math::VEC3 DrawerBase::GetPanePos( gfl2::lyt::LytPane* pPane )
  {
    if( pPane == NULL )
    {
      GFL_ASSERT(0);
      return gfl2::math::VEC3();
    }

    return pPane->GetTranslate();
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   ペインのアルファを変更
   * @param   pPane     変更対象ペイン
   * @param   alpha     アルファ値
   */
  //----------------------------------------------------------------------------
  void DrawerBase::SetAlpha( gfl2::lyt::LytPane* pPane, u8 alpha )
  {
    if( pPane == NULL )
    {
      GFL_ASSERT(0);
      return;
    }

    pPane->SetAlpha( alpha );
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
   * @brief   ペインの表示切替フラグの取得
   * @param   pPane     取得対象ペイン
   * @return  true = 表示中、false = 非表示
   */
  //----------------------------------------------------------------------------
  bool DrawerBase::IsVisiblePane( gfl2::lyt::LytPane* pPane ) const
  {
    if( pPane == NULL )
    {
      GFL_ASSERT(0);
      return false;
    }

    return GetG2D()->GetPaneVisible( pPane );
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
   * @brief   アニメーションのフレームを取得
   * @param   anmID   アニメーションID
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




//------------------------------------------------------------------------------
// @brief   メニューカーソル
//------------------------------------------------------------------------------

  //----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //----------------------------------------------------------------------------
  MenuCursor::MenuCursor( app::util::Heap* heap, app::util::AppRenderingManager* renderMan )
    : m_pHeap( heap )
    , m_pAppRenderMan( renderMan )
    , m_pMenuCursor( NULL )
    , m_pCursorPosPane( NULL )
    , m_DisplayType( gfl2::lyt::DISPLAY_LOWER )
    , m_pCursorBuffer( NULL )
    , m_CreateSeq( 0 )
    , m_DeleteSeq( 0 )
    , m_CursorPosMax( 0 )
    , m_CursorPos( 0 )
    , m_bUnsetUp( true )
  {
    ;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   メニューカーソルの生成
   */
  //----------------------------------------------------------------------------
  bool MenuCursor::CreateMenuCursor( void )
  {
    enum {
      CREATE_SEQ_BEGIN,
      CREATE_SEQ_LOAD,
      CREATE_SEQ_END,
    };

    switch( m_CreateSeq )
    {
    case CREATE_SEQ_BEGIN:
      {
        m_pMenuCursor = GFL_NEW(m_pHeap->GetDeviceHeap())
          app::tool::MenuCursor( m_pHeap, &m_pCursorBuffer );
        GFL_ASSERT( m_pMenuCursor );

        m_CreateSeq++;
      } break;

    case CREATE_SEQ_LOAD:
      {
        m_pMenuCursor->Update();

        if( m_pMenuCursor->LoadResource() )
        {
          m_CreateSeq++;
        }
      } break;

    case CREATE_SEQ_END:
      {
        return true;
      } // no break;
    }

    return false;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   メニューカーソルの破棄
   */
  //----------------------------------------------------------------------------
  bool MenuCursor::DeleteMenuCursor( void )
  {
    enum {
      DELETE_SEQ_BEGIN,
      DELETE_SEQ_MAIN,
      DELETE_SEQ_WAIT,
      DELETE_SEQ_TERM,
      DELETE_SEQ_END,
    };

    switch( m_DeleteSeq )
    {
    case DELETE_SEQ_BEGIN:
      {
        m_DeleteSeq++;
      } break;

    case DELETE_SEQ_MAIN:
      {
        m_pMenuCursor->DeleteResource();

        m_DeleteSeq++;
      } break;

    case DELETE_SEQ_WAIT:
      {
        if( m_pMenuCursor->IsModuleFree() )
        {
          m_DeleteSeq++;
        }
      } break;

    case DELETE_SEQ_TERM:
      {
        GFL_SAFE_DELETE( m_pMenuCursor );

        m_DeleteSeq++;
      } break;

    case DELETE_SEQ_END:
      {
        return true;
      } // no break;
    }

    return false;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //----------------------------------------------------------------------------
  void MenuCursor::Update( void )
  {
    if( m_pMenuCursor )
    {
      m_pMenuCursor->Update();
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   描画
   */
  //----------------------------------------------------------------------------
  void MenuCursor::Draw( gfl2::gfx::CtrDisplayNo no, u8 order, u32 layerNo )
  {
    if( !m_bUnsetUp )
    {
      if( m_pMenuCursor )
      {
        m_pMenuCursor->Draw( m_pAppRenderMan, no, m_DisplayType, order, layerNo );
      }
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   メニューカーソルのセットアップ
   * @param   g2d       G2DUtil
   * @param   paneTbl   カーソル配置ペイン配列
   * @param   paneCount カーソル配置ペイン数
   * @param   dispType  描画画面
   */
  //----------------------------------------------------------------------------
  void MenuCursor::SetupCursor( app::util::G2DUtil* g2d, gfl2::lyt::LytPane* paneTbl[], const u32 paneCount, gfl2::lyt::DisplayType dispType )
  {
    if( !m_pMenuCursor )
    {
      GFL_ASSERT(0);
      return;
    }

    // 一応入れておく
    if( !m_bUnsetUp )
    {
      // カーソルのセットアップ解除
      UnsetupCursor();
    }

    gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();

    m_DisplayType  = dispType;
    m_bUnsetUp     = false;

    if( paneCount > 0 )
    {
      m_CursorPosMax = paneCount;

      // 配置ペインのセット
      {
        m_pCursorPosPane = GFL_NEW_ARRAY(devHeap) gfl2::lyt::LytPane*[m_CursorPosMax];
        GFL_ASSERT( m_pCursorPosPane );

        for( u32 i=0; i<m_CursorPosMax; ++i )
        {
          m_pCursorPosPane[i] = paneTbl[i];
        }
      }
    }

    // メニューカーソルの作成
    {
      m_pMenuCursor->CreateLayoutWork(
        g2d->GetLayoutSystem(),
        g2d->GetLayoutWorkSetup( dispType ),
        g2d->GetAppLytAccessor(),
        app::tool::MenuCursor::SIZE_SMALL,
        false );

      // アニメの開始
      m_pMenuCursor->StartAnime();
    }
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   メニューカーソルのアンセットアップ
   *
   * @note    レイアウトを破棄する前に呼ぶこと
   */
  //----------------------------------------------------------------------------
  void MenuCursor::UnsetupCursor( void )
  {
    if( !m_bUnsetUp )
    {
      if( m_pMenuCursor )
      {
        m_pMenuCursor->DeleteLayoutWork();
      }

      m_bUnsetUp = true;
    }

    GFL_SAFE_DELETE_ARRAY( m_pCursorPosPane );
  }

  //------------------------------------------------------------------
  /**
   * @brief   カーソル位置ペインの設定
   *
   * @param   paneTbl   カーソル配置ペイン配列
   */
  //------------------------------------------------------------------
  void MenuCursor::SetNullCursor( gfl2::lyt::LytPane* paneTbl[] )
  {
    for( u32 i=0; i<m_CursorPosMax; ++i )
    {
      m_pCursorPosPane[i] = paneTbl[i];
    }
  }


  //----------------------------------------------------------------------------
  /**
   * @brief   カーソル位置の設定
   * @param   lytwk   レイアウトワーク
   * @param   pos     位置
   */
  //----------------------------------------------------------------------------
  void MenuCursor::SetCursorPos( gfl2::lyt::LytWk* lytwk, u32 pos )
  {
    if( !m_pMenuCursor )
    {
      GFL_ASSERT(0);
      return;
    }

    if( pos >= m_CursorPosMax )
    {
      GFL_ASSERT(0);
      return;
    }

    m_pMenuCursor->PutNonVisible( lytwk, m_pCursorPosPane[pos] );

    m_CursorPos = pos;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   カーソル位置の取得
   * @return  位置
   */
  //----------------------------------------------------------------------------
  u32 MenuCursor::GetCursorPos( void ) const
  {
    return m_CursorPos;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   カーソルの表示切替
   * @param   visible   表示フラグ
   */
  //----------------------------------------------------------------------------
  void MenuCursor::SetVisible( bool visible )
  {
    if( !m_pMenuCursor )
    {
      GFL_ASSERT(0);
      return;
    }

    m_pMenuCursor->SetVisible( visible );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   メニューカーソルの取得
   * @return  メニューカーソル
   */
  //----------------------------------------------------------------------------
  app::tool::MenuCursor* MenuCursor::GetMenuCursor( void ) const
  {
    return m_pMenuCursor;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   カーソルバッファの取得
   * @return  カーソルバッファ
   */
  //----------------------------------------------------------------------------
  void* MenuCursor::GetCursorBuffer( void ) const
  {
    return m_pCursorBuffer;
  }


GFL_NAMESPACE_END(BoxSearch)
GFL_NAMESPACE_END(App)
