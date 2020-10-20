//======================================================================
/**
 * @file TeamSelectUppRootView.cpp
 * @date 2015/06/30 19:29:11
 * @author uchida_yuto
 * @brief チーム選択の上画面ルートView
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include "NetApp/TeamSelect/source/View/TeamSelectUppRootView.h"

#include "niji_conv_header/app/teamselect/res2d/teamSelect.h"
#include "niji_conv_header/app/teamselect/res2d/teamSelect_anim_list.h"
#include "niji_conv_header/app/teamselect/res2d/teamSelect_pane.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(TeamSelect)

  TeamSelectUppRootView::TeamSelectUppRootView( app::util::Heap* pHeap, app::util::AppRenderingManager* pRenderManager ) :
  UIView( pHeap, pHeap->GetDeviceAllocator(), pHeap->GetDeviceAllocator() )
  {
    _Clear();

    GFL_ASSERT( pRenderManager );
    m_pHeap           = pHeap;
    m_pRenderManager  = pRenderManager;
  }

  TeamSelectUppRootView::~TeamSelectUppRootView()
  {
    UIView::ReleaseButtonManager();
    UIView::Delete2D();
  }

  //------------------------------------------------------------------
  /**
    * @brief   初期化
    */
  //------------------------------------------------------------------
  void TeamSelectUppRootView::Initialize( void* pLytBinData, gfl2::str::MsgData* pAppMsgData, print::WordSet* pWordSet )
  {
    GFL_ASSERT( pLytBinData );
    GFL_ASSERT( pAppMsgData );
    GFL_ASSERT( pWordSet );

    _Initialize2D( pLytBinData, pAppMsgData, pWordSet );

    gfl2::lyt::LytWk* pLayoutWork         = UIView::GetLayoutWork( LYT_WORK_NORMAL );
    ::app::util::G2DUtil* pG2DUtil        = UIView::GetG2DUtil();
    gfl2::lyt::LytMultiResID& rMultiResId = pG2DUtil->GetLayoutResourceID( 0 );

    //  BGのループアニメ再生
    {
      pG2DUtil->StartAnime( LYT_WORK_NORMAL, LA_TEAMSELECT_TEAMSELECT_MIN_UPP_000__BG_00_BG_LOOP_00 );
    }

    {
      m_pWindowPartsPane  = pLayoutWork->GetPartsPane( PANENAME_TEAMSELECT_MIN_UPP_000_PANE_MESSAGE );
      GFL_ASSERT( m_pWindowPartsPane );

      m_pWindowTextBox  = pLayoutWork->GetTextBoxPane( m_pWindowPartsPane, PANENAME_COMMON_MSG_UPP_001_PANE_TEXTBOX_00, &rMultiResId );
      GFL_ASSERT( m_pWindowTextBox );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   終了処理
    */
  //------------------------------------------------------------------
  void TeamSelectUppRootView::Terminate()
  {
  }

  //------------------------------------------------------------------
  /**
    * @brief   破棄可能か
    */
  //------------------------------------------------------------------
  bool TeamSelectUppRootView::IsDelete()
  {
    return  ( UIView::IsDrawing() == false );
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  描画処理
  *         子ウインドウについても自動的にコールする(親→子の順）
  */
  //--------------------------------------------------------------------------------------------
  void TeamSelectUppRootView::Draw(  gfl2::gfx::CtrDisplayNo displayNo )
  {
    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    for( s32 i = 0; i < LYT_WORK_MAX; ++i )
    {
      pG2DUtil->AddDrawableLytWkForOneFrame( m_pRenderManager, displayNo, i );
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  メッセージ表示
  */
  //--------------------------------------------------------------------------------------------
  void TeamSelectUppRootView::ShowMessageWindow( const u32 msgId )
  {
    //  メッセージ表示
    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    if( pG2DUtil->GetPaneVisible( m_pWindowPartsPane ) == false )
    {
      pG2DUtil->SetPaneVisible( m_pWindowPartsPane, true );

      pG2DUtil->StartAnime( LYT_WORK_NORMAL, LA_TEAMSELECT_TEAMSELECT_MIN_UPP_000__MESSAGE_IN );
    }

    pG2DUtil->SetTextBoxPaneString( m_pWindowTextBox, msgId );
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  メッセージ非表示
  */
  //--------------------------------------------------------------------------------------------
  void TeamSelectUppRootView::HideMesssageWindow()
  {
    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    pG2DUtil->SetPaneVisible( m_pWindowPartsPane, false );
  }

  //------------------------------------------------------------------
  /**
    * @brief   2D画面の作成
    * @param   pAppLytBuff レイアウトデータのバイナリポインタ
    */
  //------------------------------------------------------------------
  void TeamSelectUppRootView::_Initialize2D( void* pAppLytBuff, gfl2::str::MsgData* pAppMsgData, print::WordSet* pWordSet )
  {
    // アーカイブIDテーブル
    const ::app::util::G2DUtil::LytResData aResTbl[] =
    {
      {  pAppLytBuff, 32, ::app::util::G2DUtil::ATTACH_NONE }
    };

    //  作成するレイアウトワーク
    const ::app::util::G2DUtil::LytwkData aSetupData[ LYT_WORK_MAX ] =
    {
      {
        0,
        LYTRES_TEAMSELECT_TEAMSELECT_MIN_UPP_000_BFLYT,
        LA_TEAMSELECT_TEAMSELECT_MIN_UPP_000___NUM,
        pAppLytBuff,
        true,
        ::app::util::G2DUtil::SETUP_UPPER,
        true
      }
    };

    UIView::Create2D( m_pHeap, NULL, 32, aResTbl, GFL_NELEMS(aResTbl), aSetupData, GFL_NELEMS(aSetupData), pAppMsgData, pWordSet );
  }

GFL_NAMESPACE_END(TeamSelect)
GFL_NAMESPACE_END(NetApp)
