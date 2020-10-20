//======================================================================
/**
 * @file FingerCursor.cpp
 * @date 2015/10/22 15:36:28
 * @author uchida_yuto
 * @brief ゆびカーソルクラス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include "AppLib/include/Tool/FingerCursor.h"

//  nijiのインクルード
#include "AppLib/include/Util/app_util_FileAccessor.h"

//  レイアウトのインクルード
#include "arc_def_index/arc_def.h"
#include "arc_index/MenuCursor.gaix"

#include "niji_conv_header/app/common_cursor/MenuCursor.h"
#include "niji_conv_header/app/common_cursor/MenuCursor_pane.h"
#include "niji_conv_header/app/common_cursor/MenuCursor_anim_list.h"

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

  const FingerCursor::ANIME_DATA FingerCursor::s_aAnimeData[]  =
  {
    { LA_MENUCURSOR_COMMONCURSOR_FGR_LOW_000_FINGER_ANIME1, LA_MENUCURSOR_COMMONCURSOR_FGR_LOW_000_FINGER_ANIME2 },
    { LA_MENUCURSOR_COMMONCURSOR_FGR_LOW_000_FINGER_ANIME3, LA_MENUCURSOR_COMMONCURSOR_FGR_LOW_000_FINGER_ANIME4 }
  };

  //  レイアウトデータ作成
  void*  FingerCursor::CreateLytBinData( app::util::Heap * heap )
  {
    GFL_ASSERT( heap );

    void *pBinData  = NULL;
    //  レイアウトデータは内部で同期読み
    {
      app::util::FileAccessor::FileOpenSync( ARCID_MENU_CURSOR, heap->GetDeviceHeap() );

      app::util::FileAccessor::FileLoadSync(
        ARCID_MENU_CURSOR,
        GARC_MenuCursor_MenuCursor_applyt_COMP,
        &pBinData,
        heap->GetDeviceHeap(),
        true,
        nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );

      app::util::FileAccessor::FileCloseSync( ARCID_MENU_CURSOR );
    }

    GFL_ASSERT( pBinData );
    return  pBinData;
  }

  //  レイアウトデータ破棄
  void   FingerCursor::DeleteLytBinData( void* pBinData )
  {
    if( pBinData == NULL )
    {
      GFL_ASSERT( 0 );
      return;
    }

    GflHeapSafeFreeMemory( pBinData );
  }

  //-----------------------------------------------------------------------------
  /**
    * @brief   コンストラクタ
    *
    * @param   heap  ヒープ
    * @param   buf   リソースの確保場所
    */
  //-----------------------------------------------------------------------------
  FingerCursor::FingerCursor( app::util::Heap * heap, void * pLytBuf, app::util::AppRenderingManager* pRenderingManager, app::util::G2DUtil::SETUP_DISPLAY_TYPE displayType, const u8 drawOrder )
  : UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  {
    m_pRenderingManager = pRenderingManager;
    m_drawOrder = drawOrder;

    //  2D画面作成
    {
      // アーカイブIDテーブル
      const ::app::util::G2DUtil::LytResData resTbl[] =
      {
        {  pLytBuf, 8, ::app::util::G2DUtil::ATTACH_NONE, },
      };

      //  作成するレイアウトワーク
      const ::app::util::G2DUtil::LytwkData setupData[ 1 ] =
      {
          //  選択画面
          {
            0,
            LYTRES_MENUCURSOR_COMMONCURSOR_FGR_LOW_000_BFLYT,
            LA_MENUCURSOR_COMMONCURSOR_FGR_LOW_000___NUM,
            pLytBuf,
            true,
            displayType,
            true
          },
      };

      UIView::Create2D( heap, NULL, 8, resTbl, GFL_NELEMS( resTbl ), setupData, GFL_NELEMS( setupData ), UIView::UNUSE_MESSAGE_DATA );
      UIView::SetVisible( false );
    }
  }

  FingerCursor::~FingerCursor()
  {
    UIView::ReleaseButtonManager();
    UIView::Delete2D();
  }

  //-----------------------------------------------------------------------------
  /**
    * @brief   座標設定
    */
  //-----------------------------------------------------------------------------
  void FingerCursor::SetPos( gfl2::math::VEC3& rPos )
  {
    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    pG2DUtil->SetPanePos( 0, PANENAME_COMMONCURSOR_FGR_LOW_000_PANE_FINGER_ICON_POS, &rPos );
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  描画処理
  *         子ウインドウについても自動的にコールする(親→子の順）
  */
  //--------------------------------------------------------------------------------------------
  void FingerCursor::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    pG2DUtil->AddDrawableLytWkForOneFrame( m_pRenderingManager, displayNo, 0, m_drawOrder );
  }

  //-----------------------------------------------------------------------------
  /**
    * @brief   アニメ開始
    */
  //-----------------------------------------------------------------------------
  void FingerCursor::StartAnime( const FormType formType )
  {
    u32 animeId = 0;
    animeId = s_aAnimeData[ formType ].idleAnimeId;

    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    for( u32 i = 0; i < LA_MENUCURSOR_COMMONCURSOR_FGR_LOW_000___NUM; ++i )
    {
      pG2DUtil->StopAnime( 0, i );
    }

    pG2DUtil->StartAnime( 0, animeId );
  }

  //-----------------------------------------------------------------------------
  /**
    * @brief   押すアニメ開始
    */
  //-----------------------------------------------------------------------------
  void FingerCursor::PushAnime( const FormType formType )
  {
    u32 animeId = 0;
    animeId = s_aAnimeData[ formType ].pushAnimeId;

    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    for( u32 i = 0; i < LA_MENUCURSOR_COMMONCURSOR_FGR_LOW_000___NUM; ++i )
    {
      pG2DUtil->StopAnime( 0, i );
    }

    pG2DUtil->StartAnime( 0, animeId );
  }

  //-----------------------------------------------------------------------------
  /**
    * @brief   押すアニメ終了しているか
    */
  //-----------------------------------------------------------------------------
  bool FingerCursor::IsEndPushAnime( const FormType formType )
  {
    u32 animeId = 0;
    animeId = s_aAnimeData[ formType ].pushAnimeId;

    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    return pG2DUtil->IsAnimeEnd( 0, animeId );
  }

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)
