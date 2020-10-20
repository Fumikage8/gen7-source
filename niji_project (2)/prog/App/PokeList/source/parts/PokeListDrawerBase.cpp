//==============================================================================
/**
 * @file    PokeListDrawerBase.cpp
 * @author  fukushima_yuya
 * @date    2015.03.09
 * @brief   共通データのアクセス用の基底クラス
 *          sango_project app_pokelist_DrawerBase
 */
//==============================================================================


// niji
#include "App/PokeList/source/parts/PokeListDrawerBase.h"
#include "App/PokeList/source/work/PokeListLocalWork.h"
#include "App/PokeList/source/work/PokeListStaticWork.h"

#include <AppLib/include/Ui/UIView.h>

// arc
#include <niji_conv_header/app/common_pokeList/common_pokeList_pane.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )


//==============================================================================
// @brief   共通描画クラス
//==============================================================================

//------------------------------------------------------------------------------
// @brief   プロトタイプ宣言
//------------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  /**
   * @brief   プレートの取得
   * @param   pBase   DrawerBaseクラスポインタ
   * @param   cpos    プレートのインデックス(カーソル位置基準)
   */
//----------------------------------------------------------------------------
  gfl2::lyt::LytParts* GetPlateParts( DrawerBase* pBase, CursorPos cpos )
  {
    const u32 c_PlatePartsIndex[] = {
      PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_00,
      PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_03,
      PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_01,
      PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_04,
      PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_02,
      PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_05,
    };

    if( cpos >= GFL_NELEMS(c_PlatePartsIndex) )
    {
      GFL_ASSERT(0);
      cpos = CPOS_PLATE_TL;
    }

    return pBase->GetParts( c_PlatePartsIndex[cpos] );
  }


//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
DrawerBase::DrawerBase( app::ui::UIView* pUIView, app::util::Heap* pHeap, u32 layout_id, StaticWork* pStaticWork, LocalWork* pLocalWork )
  : m_pUIView( pUIView )
  , m_pHeap( pHeap )
  , m_LayoutID( layout_id )
  , m_pStaticWork( pStaticWork )
  , m_pLocalWork( pLocalWork )
  , m_pLytWk( m_pUIView->GetLayoutWork( m_LayoutID ) )
  , m_ResID( GetG2D()->GetLayoutResourceID( m_LayoutID ) )
{
  app::util::G2DUtil* pG2D = this->GetG2D();
  {
    gfl2::str::MsgData* pMsgData = this->GetMsgData();
    print::WordSet* pWordSet = this->GetWordSet();

    if( pMsgData != NULL && pWordSet != NULL )
    {
      // メッセージデータのセット
      pG2D->SetMessageData( pMsgData, pWordSet );
    }
  }

  gfl2::heap::HeapBase* pDevHeap = m_pHeap->GetDeviceHeap();
  {
    // StrBufの初期化
    for( u8 i=0; i<STR_BUF_NUM; ++i )
    {
      m_pStrBuf[i] = GFL_NEW(pDevHeap) gfl2::str::StrBuf( CHAR_COUNT, pDevHeap );
      GFL_ASSERT( m_pStrBuf[i] );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
DrawerBase::~DrawerBase( void )
{
  for( u8 i=0; i<STR_BUF_NUM; ++i )
  {
    GFL_DELETE m_pStrBuf[i];
  }
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
  void DrawerBase::SetPanePos( gfl2::lyt::LytPane* pPane, gfl2::math::Vector3 pos )
  {
    if( pPane == NULL )
    {
      GFL_ASSERT(0);
      return;
    }

    gfl2::math::VEC3 vec = gfl2::math::ConvertGfVec3ToNwVec3( pos );

    pPane->SetTranslate( vec );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   位置の取得
   * @param   pPane   対象ペイン
   * @return  現在座標値
   */
  //----------------------------------------------------------------------------
  gfl2::math::Vector3 DrawerBase::GetPanePos( gfl2::lyt::LytPane* pPane )
  {
    if( pPane == NULL )
    {
      GFL_ASSERT(0);
      return gfl2::math::Vector3( 0, 0, 0 );
    }

    gfl2::math::VEC3 vec = pPane->GetTranslate();

    return gfl2::math::ConvertNwVec3ToGfVec3( vec );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   ペインのアルファ情報の変更
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
   * @brief   レイアウトIDの取得
   * return   レイアウトID
   */
  //----------------------------------------------------------------------------
  u32 DrawerBase::GetLytID( void ) const
  {
    return m_LayoutID;
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
   * @brief   スタティックワークの取得
   * @return  StatickWorkクラスポインタ
   */
  //----------------------------------------------------------------------------
  StaticWork* DrawerBase::GetStaticWork( void ) const
  {
    return m_pStaticWork;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   ローカルワークの取得
   * @return  LocalWorkクラスポインタ
   */
  //----------------------------------------------------------------------------
  LocalWork* DrawerBase::GetLocalWork( void ) const
  {
    return m_pLocalWork;
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   メッセージデータの取得
   * @return  gfl2::str::MsgDataクラスポインタ
   */
  //----------------------------------------------------------------------------
  gfl2::str::MsgData* DrawerBase::GetMsgData( void ) const
  {
    if( m_pLocalWork == NULL ) return NULL;
    return m_pLocalWork->GetMsgData();
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   単語セットの取得
   * @return  print::WordSetクラスポインタ
   */
  //----------------------------------------------------------------------------
  print::WordSet* DrawerBase::GetWordSet( void ) const
  {
    if( m_pLocalWork == NULL ) return NULL;
    return m_pLocalWork->GetWordSet();
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   StrBufの取得
   * @param   id  バッファID
   * @return  gfl2::str::StrBufクラスポインタ
   */
  //----------------------------------------------------------------------------
  gfl2::str::StrBuf* DrawerBase::GetStrBuf( u8 id ) const
  {
    GFL_ASSERT( id < STR_BUF_NUM );

    return m_pStrBuf[id];
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

GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )
