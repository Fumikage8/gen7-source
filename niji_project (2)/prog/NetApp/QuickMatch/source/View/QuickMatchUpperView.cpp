//=============================================================================
/**
 * @file    QuickMatchUpperView.cpp
 * @brief   クイックマッチ画面
 * @author  kousaka_shin
 * @date    
 */
//=============================================================================

// module

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include "Savedata/include/MyStatus.h"

#include "NetApp/QuickMatch/source/System/QuickMatchWork.h"
#include "NetApp/QuickMatch/source/QuickMatchResourceID.h"
#include "QuickMatchUpperView.h"
#include "heap/include/gfl2_heap_manager.h"
#include "System/include/HeapDefine.h"
#include "System/include/HeapSize.h"
// niji
#include "Sound/include/Sound.h"
// resource
#include "arc_def_index/arc_def.h"
#include "arc_index/QuickMatch.gaix"
#include "arc_index/message.gaix"
#include "niji_conv_header/app/quickmatch/QuickMatch.h"
#include "niji_conv_header/app/quickmatch/QuickMatch_pane.h"
#include "niji_conv_header/app/quickmatch/QuickMatch_anim_list.h"
#include <niji_conv_header/message/msg_quickmatch.h>


#if defined(GF_PLATFORM_CTR)
#include "NetStatic/NetLib/include/NijiNetworkSystem.h"
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"
#endif


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(QuickMatch)
GFL_NAMESPACE_BEGIN(View)



//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//-----------------------------------------------------------------------------
QuickMatchUpperView::QuickMatchUpperView( NetApp::QuickMatch::System::QuickMatchWork* pQuickMatchWork , app::util::Heap * pHeap )
  : app::ui::UIView( pHeap , pHeap->GetDeviceAllocator() , pHeap->GetDeviceAllocator() )
  , m_heap( pHeap )
  , m_pMsgData( NULL )
  , m_pWordSet( NULL )
  , m_isDrawEnable( true )
  , m_pQuickMatchWork( pQuickMatchWork )
{
  CreateLayout();

  Savedata::MyStatus* pStatus = NULL;

#if defined(GF_PLATFORM_CTR)
  NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);
  pStatus = m_pQuickMatchWork->GetMyStatus( 1 - pP2pConnectionManager->GetNetID() );
  if( pStatus )
  {
    m_g2dUtil->SetRegisterPlayerName( 0 , pStatus );
  }
#endif

  gfl2::lyt::LytWk * lytwk = m_g2dUtil->GetLayoutWork( LYTID_MAIN );
  gfl2::lyt::LytMultiResID res_id = m_g2dUtil->GetLayoutResourceID( 0 );

  //プレイヤー名
  m_g2dUtil->SetTextBoxPaneStringExpand(
  lytwk->GetTextBoxPane(lytwk->GetPartsPane(PANENAME_QUICKMATCH_MIN_UPP_000_PANE_PHOTOPLATE),PANENAME_COMMON_PLT_001_PANE_TEXTBOX_00,&res_id),
  msg_quickmatch_cap_01 );

  //プレイヤーアイコン
  gfl2::lyt::LytParts*  pIconPartsPane  =  lytwk->GetPartsPane( PANENAME_QUICKMATCH_MIN_UPP_000_PANE_PHOTOPLATE );
  gfl2::lyt::LytPicture* pPicturePane  = lytwk->GetPicturePane( pIconPartsPane , PANENAME_COMMON_PLT_001_PANE_PLAYERPHOTO , &res_id );
  if( pPicturePane && pStatus )
  {
    NetAppLib::Util::NetAppTrainerIconUtility* pTrainerIconUtility = m_pQuickMatchWork->GetTrainerIconUtility();

    app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA data;


    data.pReplacePane             = pPicturePane;
    data.pTimerIconRootPane       = pIconPartsPane;
    data.pInLoadVisibleChangePane = lytwk->GetPane( pIconPartsPane , PANENAME_COMMON_PLT_001_PANE_WINDOW_PLAYER_FRAME , &res_id );
    data.ConvFromMyStatus( *pStatus );

    pTrainerIconUtility->SetIconObjectData( 0, data ); 
  }

  //背景アニメ
  UIView::GetG2DUtil()->StartAnime( 0, LA_QUICKMATCH_QUICKMATCH_MIN_UPP_000__BG_00_BG_LOOP_00 );

  if( m_pQuickMatchWork->m_pAppParam->m_isBattleTreeMode )
  {//バトルツリーモード
    HideAllLayout();
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
QuickMatchUpperView::~QuickMatchUpperView()
{
  DeleteLayout();
}



void QuickMatchUpperView::Update(void)
{
  app::ui::UIView::Update(); 

  UpdateMain();
}


void QuickMatchUpperView::SetCursorIndex( u32 index )
{
  gfl2::lyt::LytWk * lytwk = m_g2dUtil->GetLayoutWork( LYTID_MAIN );
  gfl2::lyt::LytMultiResID res_id = m_g2dUtil->GetLayoutResourceID( 0 );

  const s32 RULE_MSG[]=
  {
    msg_quickmatch_exp_01,
    msg_quickmatch_exp_02,
    msg_quickmatch_exp_03,
    msg_quickmatch_exp_05
  };

  const s32 RULENAME_MSG[]=
  {
    msg_quickmatch_cap_02,
    msg_quickmatch_cap_03,
    msg_quickmatch_cap_05,
    msg_quickmatch_cap_06
  };

  m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_QUICKMATCH_MIN_UPP_000_PANE_MESSAGE_L4 , true );


  m_g2dUtil->SetTextBoxPaneString(
  lytwk->GetTextBoxPane(lytwk->GetPartsPane(PANENAME_QUICKMATCH_MIN_UPP_000_PANE_MESSAGE_L4),PANENAME_COMMON_MSG_UPP_002_PANE_TEXTBOX_01,&res_id),
  RULENAME_MSG[index] );


  m_g2dUtil->SetTextBoxPaneString(
  lytwk->GetTextBoxPane(lytwk->GetPartsPane(PANENAME_QUICKMATCH_MIN_UPP_000_PANE_MESSAGE_L4),PANENAME_COMMON_MSG_UPP_002_PANE_TEXTBOX_00,&res_id),
  RULE_MSG[index] );
}

void QuickMatchUpperView::ShowClientMessage( void )
{
  gfl2::lyt::LytWk * lytwk = m_g2dUtil->GetLayoutWork( LYTID_MAIN );
  gfl2::lyt::LytMultiResID res_id = m_g2dUtil->GetLayoutResourceID( 0 );

  m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_QUICKMATCH_MIN_UPP_000_PANE_MESSAGE_L4 , true );

#if defined(GF_PLATFORM_CTR)
  NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);
  Savedata::MyStatus* pStatus = m_pQuickMatchWork->GetMyStatus( 1 - pP2pConnectionManager->GetNetID() );
  m_g2dUtil->SetRegisterPlayerName( 0 , pStatus );
#endif

  m_g2dUtil->SetTextBoxPaneStringExpand(
  lytwk->GetTextBoxPane(lytwk->GetPartsPane(PANENAME_QUICKMATCH_MIN_UPP_000_PANE_MESSAGE_L4),PANENAME_COMMON_MSG_UPP_002_PANE_TEXTBOX_01,&res_id),
  msg_quickmatch_cap_07 );

  m_g2dUtil->SetTextBoxPaneString(
  lytwk->GetTextBoxPane(lytwk->GetPartsPane(PANENAME_QUICKMATCH_MIN_UPP_000_PANE_MESSAGE_L4),PANENAME_COMMON_MSG_UPP_002_PANE_TEXTBOX_00,&res_id),
  msg_quickmatch_exp_06 );

}

void QuickMatchUpperView::HideTextBox( void )
{
  m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_QUICKMATCH_MIN_UPP_000_PANE_MESSAGE_L4 , false );
}


void QuickMatchUpperView::HideAllLayout( void )
{
  m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_QUICKMATCH_MIN_UPP_000_PANE_NULL_PLAYERSET , false );
}

void QuickMatchUpperView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pQuickMatchWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LYTID_MAIN );
}



//-----------------------------------------------------------------------------
/**
 * @func    OnLayoutPaneEvent
 * @brief   ペインイベントの検知
 * @date    2015.03.05
 *
 * @param   button_id   通知されたボタンID
 *
 * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
 * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
 * @retval  ENABLE_ACCESS         = それ以外
 *
 * @note  override UIInputListener
 */
//-----------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult QuickMatchUpperView::OnLayoutPaneEvent( const u32 button_id )
{
  return ENABLE_ACCESS;
}

//-----------------------------------------------------------------------------
/**
 * @func    OnLayoutPaneTouchTriggerEvent
 * @brief   ボタンタッチを検知
 * @date    2015.03.05
 *
 * @param   button_id   通知されたボタンID
 *
 * @note  override UIInputListener
 */
//-----------------------------------------------------------------------------
void QuickMatchUpperView::OnLayoutPaneTouchTriggerEvent( const u32 button_id )
{
}

//-----------------------------------------------------------------------------
/**
 * @func    OnKeyAction
 * @brief   キー入力の検知
 * @date    2015.03.05
 *
 * @param   button  ボタン
 * @param   key     十字キー
 * @param   stick   アナログスティック
 *
 * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
 * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
 * @retval  ENABLE_ACCESS         = それ以外
 *
 * @note  override UIInputListener
 */
//-----------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult QuickMatchUpperView::OnKeyAction( gfl2::ui::Button * button, gfl2::ui::VectorDevice * key, gfl2::ui::VectorDevice * stick )
{
  return ENABLE_ACCESS;
}



//メイン処理
void QuickMatchUpperView::UpdateMain(void)
{
}




void QuickMatchUpperView::CreateLayout(void)
{
  NetAppLib::System::ResourceManager* pResourceManager = m_pQuickMatchWork->GetResourceManager();
  void* pLayoutBuffer = pResourceManager->GetResourceBuffer( NetApp::QuickMatch::QUICKMATCH_RESOURCE_ID_LAYOUT );
  void* pMsgBuffer = pResourceManager->GetResourceBuffer( NetApp::QuickMatch::QUICKMATCH_RESOURCE_ID_MSG );

  // リソースデータ
  const app::util::G2DUtil::LytResData res_tbl[] =
  {
    { pLayoutBuffer, 0, app::util::G2DUtil::ATTACH_ROOT },
  };
  // レイアウトデータ
	const app::util::G2DUtil::LytwkData data_tbl[] =
  {
    {
      0,
      LYTRES_QUICKMATCH_QUICKMATCH_MIN_UPP_000_BFLYT,
      LA_QUICKMATCH_QUICKMATCH_MIN_UPP_000___NUM,
      pLayoutBuffer,
      false,
      app::util::G2DUtil::SETUP_UPPER,
      true
    },
  };

  m_pMsgData     = GFL_NEW(m_heap->GetDeviceHeap()) gfl2::str::MsgData( pMsgBuffer, m_heap->GetDeviceHeap() );
  m_pWordSet     = GFL_NEW(m_heap->GetDeviceHeap()) print::WordSet( m_heap->GetDeviceHeap() );

  Create2D(
    m_heap,
    NULL,
    LYTID_MAX,
    res_tbl,
    GFL_NELEMS(res_tbl),
    data_tbl,
    GFL_NELEMS(data_tbl),
    m_pMsgData,
    m_pWordSet );

}

void QuickMatchUpperView::DeleteLayout(void)
{
  Delete2D();
  GFL_SAFE_DELETE( m_pWordSet );
  GFL_SAFE_DELETE( m_pMsgData );
}





//=============================================================================
//=============================================================================







GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(QuickMatch)
GFL_NAMESPACE_END(NetApp)