//==============================================================================
/**
 * @file	  ResortDraw.cpp
 * @brief	  描画サンプル
 * @author	ohno
 * @data	  February 17, 2015
 */
// =============================================================================

#include "ResortDraw.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/parts_test.gaix>
#include <niji_conv_header/app/island/prog2d/islandup.h>
#include <niji_conv_header/app/island/prog2d/islandup_pane.h>
#include <niji_conv_header/app/island/prog2d/islandup_anim_list.h>

#include <niji_conv_header/sound/SoundMiddleID.h>


#include "arc_index/message.gaix"
#include <arc_index/debug_message.gaix>

#include <Layout/include/gfl2_LayoutRenderPath.h>



GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Resort )

//--------------------------------------------------------------------------





//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

//----------------------------------------------------------------------
/**
 * @brief コンストラクタ
 * @param heap  ヒープ
 */
//----------------------------------------------------------------------
ResortDraw::ResortDraw( app::util::Heap * heap, app::util::AppRenderingManager* pAppRenderingManager ) :
app::ui::UIView( heap, heap->GetSystemAllocator(), heap->GetDeviceAllocator() )
,m_heap( heap )
,m_uiListener( NULL )
{
  mpAppRenderingManager = pAppRenderingManager;
  mbNearMode=false;
}

/// デストラクタ
ResortDraw::~ResortDraw( void )
{
  UIView::ReleaseButtonManager();
 // UIView::Delete2D();
}

//----------------------------------------------------------------------
/**
 *  @brief  初期化
 */
//----------------------------------------------------------------------

void ResortDraw::Init(APP_PARAM* pAppParam, void* pAppLytBuff)
{


	SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
	CreateGraphicsSystem( m_heap, false );


  Initialize2D( m_heap,pAppLytBuff );

//  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  InitializeText(m_heap);

  SetInputListener( this );
}

//----------------------------------------------------------------------
/**
 *  @brief  終了処理
 */
//----------------------------------------------------------------------

bool ResortDraw::End(void)
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  pG2DUtil->DeleteMessageWindow();
  FinalizeText();
  Delete2D();
  DeleteGraphicsSystem();
  return true;
}


//----------------------------------------------------------------------
/**
 *  @brief  UIイベント用リスナーの登録
 */
//----------------------------------------------------------------------
void ResortDraw::SetTitleViewUIListener( ResortDrawListener *listener )
{
  m_uiListener = listener;
}

//--------------------------------------------------------------------------
/**
 * @brief   更新
 */
//--------------------------------------------------------------------------
void ResortDraw::Update(void)
{

  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  pG2DUtil->PrintMessage();
  app::ui::UIView::Update();



}

//--------------------------------------------------------------------------
/**
 * @brief   描画
 */
//--------------------------------------------------------------------------
void ResortDraw::Draw(gfl2::gfx::CtrDisplayNo no)
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  pG2DUtil->AddDrawableLytWkForOneFrame( mpAppRenderingManager, no, 0 );
  pG2DUtil->AddDrawableLytWkForOneFrame( mpAppRenderingManager, no, 1 );
}


//--------------------------------------------------------------------------
/**
 * @brief   破棄チェック
 * @return  trueで破棄完了
 */
//--------------------------------------------------------------------------
bool ResortDraw::IsExit( void )
{
  return true;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief  タッチパネルイベントの検知
 *
 * @param  pTouchPanel タッチパネル
 * @param  isTouch     タッチ状態であるならtrue、タッチされていないならfalse
 *                     (タッチトリガを調べたい場合はTouchPanel::IsTouchTrigger()にて判断可能）
 *
 * @return 同フレーム内での他入力イベントを許可するならtrue、
 *         他のイベントを排除するならfalseを返却すること。
 */
//--------------------------------------------------------------------------------------------
// overwride::
::app::ui::UIInputListener::ListenerResult ResortDraw::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  if( m_uiListener == NULL ){
    return ENABLE_ACCESS;
  }

  // 決定
  if( pTouchPanel->IsTouchTrigger() ){
    return DISABLE_ACCESS;
  }
  return ENABLE_ACCESS;
}





void ResortDraw::PushButtonR(void)
{
  {
    int anim_tbl_rup[]={
      LA_ISLANDUP_ISLAND_MIN_UPP_000_1TO2,
      LA_ISLANDUP_ISLAND_MIN_UPP_000_2TO3,
      LA_ISLANDUP_ISLAND_MIN_UPP_000_3TO4,
      LA_ISLANDUP_ISLAND_MIN_UPP_000_4TO5,
      LA_ISLANDUP_ISLAND_MIN_UPP_000_5TO1,
    };
    int anim_tbl_rlow[]={
      LA_ISLANDUP_ISLAND_MIN_LOW_000_1TO2,
      LA_ISLANDUP_ISLAND_MIN_LOW_000_2TO3,
      LA_ISLANDUP_ISLAND_MIN_LOW_000_3TO4,
      LA_ISLANDUP_ISLAND_MIN_LOW_000_4TO5,
      LA_ISLANDUP_ISLAND_MIN_LOW_000_5TO1,
    };
    
    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    pG2DUtil->StartAnime( 0, anim_tbl_rup[m_Page] );
    pG2DUtil->StartAnime( 1, anim_tbl_rlow[m_Page] );
    m_Page++;
    if(m_Page >= elementof(anim_tbl_rup) ){
      m_Page=0;
    }
  }
}

void ResortDraw::PushButtonL(void)
{
  {
    int anim_tbl_lup[]={
      LA_ISLANDUP_ISLAND_MIN_UPP_000_1TO5,
      LA_ISLANDUP_ISLAND_MIN_UPP_000_2TO1,
      LA_ISLANDUP_ISLAND_MIN_UPP_000_3TO2,
      LA_ISLANDUP_ISLAND_MIN_UPP_000_4TO3,
      LA_ISLANDUP_ISLAND_MIN_UPP_000_5TO4,
    };

    int anim_tbl_llow[]={
      LA_ISLANDUP_ISLAND_MIN_LOW_000_1TO5,
      LA_ISLANDUP_ISLAND_MIN_LOW_000_2TO1,
      LA_ISLANDUP_ISLAND_MIN_LOW_000_3TO2,
      LA_ISLANDUP_ISLAND_MIN_LOW_000_4TO3,
      LA_ISLANDUP_ISLAND_MIN_LOW_000_5TO4,
    };


    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    pG2DUtil->StartAnime( 0, anim_tbl_lup[m_Page] );
    pG2DUtil->StartAnime( 1, anim_tbl_llow[m_Page] );
    m_Page--;
    if(m_Page < 0 ){
      m_Page=elementof(anim_tbl_lup)-1;
    }
  }
}


void ResortDraw::PushButtonB(bool nearmode)
{
  // 終了
  if(!nearmode){
    m_uiListener->OnButtonAction(1);
  }
  else{
    m_uiListener->OnButtonAction(0);
  }
}




//------------------------------------------------------------------
/**
 * @brief   ペインイベントの検知
 *
 * @param  layoutId 通知を受けるレイアウトＩＤ
 * @param  painId   通知を受けるペインＩＤ
 *
 * @return 同フレーム内での他入力イベントを許可するならtrue、
 *         他のイベントを排除するならfalseを返却すること。
 */
//------------------------------------------------------------------
// overwride
::app::ui::UIInputListener::ListenerResult ResortDraw::OnLayoutPaneEvent( const u32 button_id )
{

  if( button_id == BTN_ID_CANCEL )
  {
    PushButtonB(mbNearMode);
    return DISABLE_ACCESS;
  }
  if(!mbNearMode){
    if( button_id == BTN_ID_L )
    {
      PushButtonL();
    }
    if( button_id == BTN_ID_R )
    {
      PushButtonR();
    }
  }
  return ENABLE_ACCESS;
}


//------------------------------------------------------------------
/**
 * @brief   キー入力の検知
 *
 * @param  pKey    十字キー
 * @param  pStick  アナログスティック
 *
 * @return 同フレーム内での他入力イベントを許可するならtrue、
 *         他のイベントを排除するならfalseを返却すること。
 */
//------------------------------------------------------------------
// overwride
::app::ui::UIInputListener::ListenerResult ResortDraw::OnKeyAction(  gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  return ENABLE_ACCESS;
}

//==========================================================================
/**
 *        private関数
 */
//==========================================================================
//----------------------------------------------------------------------
/**
 * @brief   2D関連作成
 *
 * @param   heap    ヒープ
 */
//----------------------------------------------------------------------
void ResortDraw::Initialize2D( app::util::Heap * heap, void* pAppLytBuff )
{

  // アーカイブIDテーブル
  app::util::G2DUtil::LytResData resTbl[] = {
    {  pAppLytBuff, 16,  app::util::G2DUtil::ATTACH_ROOT, },
    {  pAppLytBuff, 16,  app::util::G2DUtil::ATTACH_ROOT, },
  };

	 app::util::G2DUtil::LytwkData setupData[] = {
      {//上
        0,
        LYTRES_ISLANDUP_ISLAND_MIN_UPP_000_BFLYT,
        LA_ISLANDUP_ISLAND_MIN_UPP_000___NUM,
        pAppLytBuff,
        true,
        app::util::G2DUtil::SETUP_UPPER,
        true
      },
      {//下
        0,
        LYTRES_ISLANDUP_ISLAND_MIN_LOW_000_BFLYT,
        LA_ISLANDUP_ISLAND_MIN_LOW_000___NUM,
        pAppLytBuff,
        true,
        app::util::G2DUtil::SETUP_LOWER,
        true
      },
    };

  Create2D( heap, NULL, 2, 
            resTbl, GFL_NELEMS(resTbl), 
            setupData, GFL_NELEMS(setupData), 
            GARC_message_btl_ui_DAT );

  IslandCreateButtonManager();

}

//----------------------------------------------------------------------
/**
 *	@brief  初期テキスト表示
 */
//----------------------------------------------------------------------
void ResortDraw::InitializeText( app::util::Heap * pheap )
{

#if PM_DEBUG
  mpMsgData = GFL_NEW(pheap->GetDeviceHeap()) gfl2::str::MsgData( print::GetMessageArcId_Debug(),
    GARC_debug_message_d_menuparts_DAT, pheap->GetDeviceHeap(), gfl2::str::MsgData::LOAD_FULL );
#endif // PM_DEBUG

  mpTempStrBuf = GFL_NEW( pheap->GetDeviceHeap() ) gfl2::str::StrBuf( TEMP_STRBUF_LENGTH, pheap->GetDeviceHeap() );
  mpTempStrBuf2 = GFL_NEW( pheap->GetDeviceHeap() ) gfl2::str::StrBuf( TEMP_STRBUF_LENGTH, pheap->GetDeviceHeap() );

  mpWordSet = GFL_NEW( pheap->GetDeviceHeap() ) print::WordSet( pheap->GetDeviceHeap() );

}


void ResortDraw::FinalizeText(void)
{
  GFL_DELETE mpWordSet;

#if PM_DEBUG
  GFL_DELETE mpMsgData;
#endif // PM_DEBUG
  GFL_DELETE mpTempStrBuf2;
  GFL_DELETE mpTempStrBuf;
}




//------------------------------------------------------------------------------
/**
 * @brief   ボタンマネージャの生成
 */
//------------------------------------------------------------------------------
void ResortDraw::IslandCreateButtonManager( void )
{
   app::ui::ButtonInfoEx c_DefInfo[3] = {
    {
      BTN_ID_CANCEL, NULL, NULL,
      LA_ISLANDUP_ISLAND_MIN_LOW_000__BACKBUTTON_TOUCH,
      LA_ISLANDUP_ISLAND_MIN_LOW_000__BACKBUTTON_RELEASE,
      LA_ISLANDUP_ISLAND_MIN_LOW_000__BACKBUTTON_CANSEL,
      LA_ISLANDUP_ISLAND_MIN_LOW_000__BACKBUTTON_TOUCH_RELEASE,
      LA_ISLANDUP_ISLAND_MIN_LOW_000__BACKBUTTON_ACTIVE,
      LA_ISLANDUP_ISLAND_MIN_LOW_000__BACKBUTTON_PASSIVE,
    },
    {
      BTN_ID_R, NULL, NULL,
      LA_ISLANDUP_ISLAND_MIN_LOW_000__BUTTON_R_TOUCH,
      LA_ISLANDUP_ISLAND_MIN_LOW_000__BUTTON_R_RELEASE,
      LA_ISLANDUP_ISLAND_MIN_LOW_000__BUTTON_R_CANSEL,
      LA_ISLANDUP_ISLAND_MIN_LOW_000__BUTTON_R_TOUCH_RELEASE,
      LA_ISLANDUP_ISLAND_MIN_LOW_000__BUTTON_R_ACTIVE,
      LA_ISLANDUP_ISLAND_MIN_LOW_000__BUTTON_R_PASSIVE,
    },
    {
      BTN_ID_L, NULL, NULL,
      LA_ISLANDUP_ISLAND_MIN_LOW_000__BUTTON_L_TOUCH,
      LA_ISLANDUP_ISLAND_MIN_LOW_000__BUTTON_L_RELEASE,
      LA_ISLANDUP_ISLAND_MIN_LOW_000__BUTTON_L_CANSEL,
      LA_ISLANDUP_ISLAND_MIN_LOW_000__BUTTON_L_TOUCH_RELEASE,
      LA_ISLANDUP_ISLAND_MIN_LOW_000__BUTTON_L_ACTIVE,
      LA_ISLANDUP_ISLAND_MIN_LOW_000__BUTTON_L_PASSIVE,
    },
  };


//  app::ui::ButtonInfoEx* info;

	app::ui::ButtonInfoEx * info = GFL_NEW_LOW_ARRAY(m_heap->GetSystemHeap()) app::ui::ButtonInfoEx[3];


  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( 1 );
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( 1 );

   gfl2::lyt::LytParts* pParts[3];
  pParts[0] = pLytWk->GetPartsPane( PANENAME_ISLAND_MIN_LOW_000_PANE_BACKBUTTON );
  pParts[1] = pLytWk->GetPartsPane( PANENAME_ISLAND_MIN_LOW_000_PANE_BUTTON_R );
  pParts[2] = pLytWk->GetPartsPane( PANENAME_ISLAND_MIN_LOW_000_PANE_BUTTON_L );

  for(int i = 0 ; i < 3; i++){
    info[i] = c_DefInfo[i];
    info[i].picture_pane = pParts[i];
    info[i].bound_pane = pLytWk->GetPane( pParts[i], PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &resID );

  }
  this->CreateButtonManager( m_heap, pLytWk, info, 3 );
  
  GFL_DELETE_ARRAY info;

  app::tool::ButtonManager* pBtnMan = this->GetButtonManager();
  pBtnMan->SetButtonBindKey( BTN_ID_CANCEL, gfl2::ui::BUTTON_B, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );
  pBtnMan->SetButtonSelectSE( BTN_ID_CANCEL, SEQ_SE_ISLAND_CNACEL );
   //  pBtnMan->SetButtonInputEnable( BTN_ID_CANCEL, false );  // 初期化時は押せないようにしておく

  pBtnMan->SetButtonBindKey( BTN_ID_R, gfl2::ui::BUTTON_R, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );
  pBtnMan->SetButtonSelectSE( BTN_ID_R, SEQ_SE_ISLAND_PAGESWITCH );
    //  pBtnMan->SetButtonInputEnable( BTN_ID_R, false );  // 初期化時は押せないようにしておく

  pBtnMan->SetButtonBindKey( BTN_ID_L, gfl2::ui::BUTTON_L, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );
  pBtnMan->SetButtonSelectSE( BTN_ID_L, SEQ_SE_ISLAND_PAGESWITCH );
    //  pBtnMan->SetButtonInputEnable( BTN_ID_L, false );  // 初期化時は押せないようにしておく

  
}



void ResortDraw::ChageNearMode(void)
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
	  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( 1 );

  gfl2::lyt::LytParts* pParts[3];
  pParts[1] = pLytWk->GetPartsPane( PANENAME_ISLAND_MIN_LOW_000_PANE_BUTTON_R );
  pParts[2] = pLytWk->GetPartsPane( PANENAME_ISLAND_MIN_LOW_000_PANE_BUTTON_L );

  gfl2::lyt::LytPane* pPane = pLytWk->GetPane(PANENAME_ISLAND_MIN_LOW_000_PANE_TEMP_BG_TEXT);
	pPane->SetVisible(false);
  pG2DUtil->SetPaneVisible(pParts[1],false);
  pG2DUtil->SetPaneVisible(pParts[2],false);
  mbNearMode=true;
}

void ResortDraw::ChageFarMode(void)
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
	  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( 1 );

  gfl2::lyt::LytParts* pParts[3];
  pParts[1] = pLytWk->GetPartsPane( PANENAME_ISLAND_MIN_LOW_000_PANE_BUTTON_R );
  pParts[2] = pLytWk->GetPartsPane( PANENAME_ISLAND_MIN_LOW_000_PANE_BUTTON_L );

  gfl2::lyt::LytPane* pPane = pLytWk->GetPane(PANENAME_ISLAND_MIN_LOW_000_PANE_TEMP_BG_TEXT);
	pPane->SetVisible(true);

  pG2DUtil->SetPaneVisible(pParts[1],true);
  pG2DUtil->SetPaneVisible(pParts[2],true);
  mbNearMode=false;
}







GFL_NAMESPACE_END( Resort )
GFL_NAMESPACE_END( App )

