// ============================================================================
/*
 * @file UpperBGView.cpp
 * @brief バトルツリーアプリの上画面を表示するクラスです。
 * @date 2015.11.30
 * @author yuto_uchida
 */
// ============================================================================
#include "App/BattleTree/source/View/BattleTreeUpperBGView.h"

#include "App/BattleTree/source/BattleTreeResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

//  nijiのインクルード
#include "AppLib/include/util/app_util_GetPaneHelper.h"

//  リソースのインクルード
#include <arc_index/message.gaix>
#include <niji_conv_header/app/battletree/res2d/battletreeLyt.h>
#include <niji_conv_header/app/battletree/res2d/battletreeLyt_anim_list.h>
#include <niji_conv_header/app/battletree/res2d/battletreeLyt_pane.h>

//  メッセージのインクルード
#include <niji_conv_header/message/msg_battletrainer_select.h>

#include <niji_conv_header/sound/SoundMiddleID.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleTree)
GFL_NAMESPACE_BEGIN(View)

  UpperBGView::UpperBGView( App::BattleTree::System::BattleTreeWork* pWork ) :
    NetAppLib::System::NetApplicationViewBase(
      pWork,
      App::BattleTree::BATTLE_TREE_RESOURCE_ID_LAYOUT,
      LYTRES_BATTLETREELYT_BATTLETREE_MIN_UPP_000_BFLYT,
      LA_BATTLETREELYT_BATTLETREE_MIN_UPP_000___NUM,
      app::util::G2DUtil::SETUP_UPPER,
      pWork->GetMessageUtility(),
      GARC_message_battletrainer_select_DAT )
  {
    _Clear();

    GFL_ASSERT( pWork );
    m_pWork = pWork;

    gfl2::heap::HeapBase* pDevHeap = m_pWork->GetAppHeap()->GetDeviceHeap();

    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

    gfl2::lyt::LytMultiResID& rResourceID = pG2DUtil->GetLayoutResourceID( 0 );
    gfl2::lyt::LytWk* pLayoutWork = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_UPPER );
    GFL_ASSERT( pLayoutWork );

    app::util::GetPaneHelper  helper( pLayoutWork, &rResourceID );

    //  BGのループアニメ
    {
      pG2DUtil->StartAnime( LAYOUT_WORK_ID_UPPER, LA_BATTLETREELYT_BATTLETREE_MIN_UPP_000__BG_WAVE_GREEN_UPP_00_BG_LOOP_00 );
    }

    //  pokeicon管理作成
    {
      m_pPokeIconTexPool  = GFL_NEW( pDevHeap ) App::Tool::PokeIconTexPool( pDevHeap, GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() );

#ifdef DEBUG_ONLY_FOR_uchida_yuto
      pDevHeap->PrintHeapInfo( "UpperBGView::PokeIconTexPool \n" );
#endif
    }

    //  ポケモンインフォオブジェクト作成
    {
      u32 loopNum = GFL_NELEMS( m_pPokeInfoObject );
      for( u32 i = 0; i < loopNum; ++i )
      {
        m_pPokeInfoObject[ i ]  = GFL_NEW( pDevHeap ) PokeInfoObject( pDevHeap, pLayoutWork, m_pPokeIconTexPool, i, (PokeInfoPos)i );
      }
    }

    //  ペイン取得
    {
      m_pInfoTextBox  = pLayoutWork->GetTextBoxPane( PANENAME_BATTLETREE_MIN_UPP_000_PANE_TEXTBOX_02 );
    }
  }

  UpperBGView::~UpperBGView()
  {
    u32 loopNum = GFL_NELEMS( m_pPokeInfoObject );
    for( u32 i = 0; i < loopNum; ++i )
    {
      GFL_SAFE_DELETE( m_pPokeInfoObject[ i ] );
    }

   // GFL_SAFE_DELETE( m_pPokeIconManaager );
    GFL_SAFE_DELETE( m_pPokeIconTexPool );
  }

  bool  UpperBGView::InitFunc()
  {
    bool  bRet  = true;
    if( m_pPokeIconTexPool->InitFunc() == false )
    {
      bRet  = false;
    }

    return bRet;
  }

  bool  UpperBGView::EndFunc()
  {
    bool  bRet  = true;
    if( m_pPokeIconTexPool->EndFunc() == false )
    {
      bRet  = false;
    }

    return bRet;
  }

  void UpperBGView::Update(void)
  {
    {
      u32 loopNum = GFL_NELEMS( m_pPokeInfoObject );
      for( u32 i = 0; i < loopNum; ++i )
      {
        m_pPokeInfoObject[ i ]->Update();
      }
    }

    UIView::Update();
  }

  void UpperBGView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    app::util::AppRenderingManager* pAppRenderingManager = m_pWork->GetAppRenderingManager();
    pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_UPPER );
  }

  //  インフォテキストを指定
  void UpperBGView::SetInfoText( const gfl2::str::STRCODE* pText )
  {
    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    {
      gfl2::str::StrBuf*  pStrBuf = pG2DUtil->GetTempStrBuf( 0 );
      pStrBuf->SetStr( pText );
      pG2DUtil->SetRegisterWord( 0, *pStrBuf );
    }
    pG2DUtil->SetTextBoxPaneStringExpand( m_pInfoTextBox, battle_trainer_select_01_01 );
  }

  //  ポケモン表示設定
  void UpperBGView::SetPokeInfo( const PokeTool::SimpleParam& rParam, const PokeInfoPos infoPos )
  {
    m_pPokeInfoObject[ infoPos ]->SetInfo( rParam, UIView::GetG2DUtil() );
  }

//  ここから先は UpperBGView::PokeInfoObject の定義
  //UpperBGView::PokeInfoObject::PokeInfoObject( gfl2::lyt::LytWk* pLayoutWork, app::tool::PokeIconToolEx* pIconToolEx, const u32 iconId, const PokeInfoPos posInfo )
  UpperBGView::PokeInfoObject::PokeInfoObject( gfl2::heap::HeapBase* pHeap, gfl2::lyt::LytWk* pLayoutWork, App::Tool::PokeIconTexPool* pPokeIconPool, const u32 iconId, const PokeInfoPos posInfo )
  {
    _Clear();

 //   GFL_ASSERT( pIconToolEx );

  //  m_pPokeIconManaager = pIconToolEx;
    m_iconId  = iconId;

    const gfl2::lyt::LytPaneIndex aPaneIndex[ POKE_INFO_POS_MAX ][ 2 ]  =
    {
      { PANENAME_BATTLETREE_MIN_UPP_000_PANE_POKE_ICON_00, PANENAME_BATTLETREE_MIN_UPP_000_PANE_TEXTBOX_00 },
      { PANENAME_BATTLETREE_MIN_UPP_000_PANE_POKE_ICON_01, PANENAME_BATTLETREE_MIN_UPP_000_PANE_TEXTBOX_01 },
    };

    m_pIconPic      = pLayoutWork->GetPicturePane( aPaneIndex[ posInfo ][ 0 ] );
    GFL_ASSERT( m_pIconPic );

    m_pNameTextBox  = pLayoutWork->GetTextBoxPane( aPaneIndex[ posInfo ][ 1 ] );
    GFL_ASSERT( m_pNameTextBox );

    m_pPokeIconTexData  = GFL_NEW( pHeap ) App::Tool::PokeIconTexData( pHeap );
    m_pPokeIconTexPool  = pPokeIconPool;

    m_pTempHeap = pHeap;
#ifdef DEBUG_ONLY_FOR_uchida_yuto
    m_pTempHeap->PrintHeapInfo( "UpperBGView::PokeInfoObject \n" );
#endif
  }

  UpperBGView::PokeInfoObject::~PokeInfoObject()
  {
    GFL_SAFE_DELETE( m_pPokeIconTexData );
  }

  void UpperBGView::PokeInfoObject::SetInfo( const PokeTool::SimpleParam& rParam, app::util::G2DUtil* pG2DUtil )
  {
    //  ポケモンがいない場合非表示
    if( rParam.monsNo == MONSNO_NULL )
    {
      m_pIconPic->SetVisible( false );
      pG2DUtil->SetTextBoxPaneString( m_pNameTextBox, L"" );

      m_pIconPic->SetVisible( false );

      m_bReplace = false;
      return;
    }

    m_replaceCount  = 1;
    m_bReplace      = true;

    m_simplePokeParam = rParam;

    //  関数がconstでないのでconstはずしをする(非同期読み)
    //m_pPokeIconManaager->LoadPokeIcon( m_iconId, (PokeTool::SimpleParam&)m_simplePokeParam );
    //  貼り付ける前にアイコンは非表示
//    m_pIconPic->SetVisible( false );
    
    //  アイコンテクスチャー作成して貼り付け
    m_pPokeIconTexPool->CreatePokeData( m_pPokeIconTexData, m_pTempHeap->GetLowerHandle(), (PokeTool::SimpleParam&)m_simplePokeParam, false );
    m_pPokeIconTexData->ReplectPaneTexture( m_pIconPic, 0, 0 );
    m_pIconPic->SetVisible( true );

#ifdef DEBUG_ONLY_FOR_uchida_yuto
    m_pTempHeap->PrintHeapInfo( "UpperBGView::PokeInfoObject::SetInfo ReplaceTex \n" );
#endif

    //  名前設定
    {
      pG2DUtil->SetRegisterMonsNameNo( 0, rParam.monsNo );
      pG2DUtil->SetTextBoxPaneStringExpand( m_pNameTextBox, battle_trainer_select_01_00 );
    }
  }

  void UpperBGView::PokeInfoObject::Update()
  {
    if( m_bReplace == true )
    {
     // if( m_pPokeIconManaager->IsLoadPokeIcon( m_iconId ) == true )
      {
     //   m_pPokeIconManaager->ReplaceIcon( m_iconId, m_pIconPic );
        m_bReplace    = false;
      }
/*
      if( 0 < m_replaceCount )
      {
        --m_replaceCount;
      }
      else
      {
        //  関数がconstでないのでconstはずしをする(非同期読み)
        //m_pPokeIconManaager->SetPokeIcon( m_iconId, (PokeTool::SimpleParam&)m_simplePokeParam, m_pIconPic );
//        m_pPokeIconManaager->LoadPokeIcon( m_iconId, (PokeTool::SimpleParam&)m_simplePokeParam );

        m_pIconPic->SetVisible( true );

        m_bReplace    = false;
      }
*/
    }
  }

GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(BattleTree)
GFL_NAMESPACE_END(App)
