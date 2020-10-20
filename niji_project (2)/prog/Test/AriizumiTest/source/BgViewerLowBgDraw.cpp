//==============================================================================
/**
 * @file	  BgViewerLowBgDraw.cpp
 * @brief	  メイン下画面描画
 * @author	ohno
 * @data	  February 17, 2015
 */
// =============================================================================
#if PM_DEBUG

#include "BgViewerLowBgDraw.h"

//素材
#include <arc_def_index/arc_def.h>
#include <arc_index/battlebgview.gaix>
#include <niji_conv_header/app/debug/bgviewer/res2d/battlebgview.h>
#include <niji_conv_header/app/debug/bgviewer/res2d/battlebgview_anim_list.h>
#include <niji_conv_header/app/debug/bgviewer/res2d/battlebgview_pane.h>

#include "arc_index/debug_message.gaix"
#include <niji_conv_header/message/debug/msg_d_btlbgviewer.h>

#include <Layout/include/gfl2_LayoutRenderPath.h>



GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Ariizumi )

//--------------------------------------------------------------------------





//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

//----------------------------------------------------------------------
/**
 * @brief コンストラクタ
 * @param heap  ヒープ
 */
//----------------------------------------------------------------------
BgViewerLowBgDraw::BgViewerLowBgDraw( app::util::Heap * heap, app::util::AppRenderingManager* pAppRenderingManager ) :
app::ui::UIView( heap, heap->GetSystemAllocator(), heap->GetDeviceAllocator() )
,mpAppHeap( heap )
,m_uiListener( NULL )
{
  mpAppRenderingManager = pAppRenderingManager;

}

/// デストラクタ
BgViewerLowBgDraw::~BgViewerLowBgDraw( void )
{
}

//----------------------------------------------------------------------
/**
 *  @brief  初期化
 */
//----------------------------------------------------------------------

void BgViewerLowBgDraw::Init(APP_PARAM* pAppParam, void* pAppLytBuff)
{


  mpAppParam = pAppParam;

  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( mpAppHeap, false );


  Initialize2D( mpAppHeap,pAppLytBuff );

  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

  InitializeText(mpAppHeap);
  //  pG2DUtil->CreateMessageWindow(  app::util::G2DUtil::SETUP_LOWER, messageWindowPane,true, m_heap  );

  SetInputListener( this );
}

//----------------------------------------------------------------------
/**
 *  @brief  終了処理
 */
//----------------------------------------------------------------------

bool BgViewerLowBgDraw::End(void)
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  //  pG2DUtil->DeleteMessageWindow();
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
void BgViewerLowBgDraw::SetUIListener( BgViewerLowBgDrawListener *listener )
{
  m_uiListener = listener;
}

//--------------------------------------------------------------------------
/**
 * @brief   更新
 */
//--------------------------------------------------------------------------
void BgViewerLowBgDraw::Update(void)
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  pG2DUtil->PrintMessage();
  app::ui::UIView::Update();

  const gfl2::ui::Button* button = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if(button->IsHold(gfl2::ui::BUTTON_Y))
  {


    gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( 0 );
    //    gfl2::lyt::LytParts* pParts = pLytWk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_00 );
    //    gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( 0 );

    //    gfl2::lyt::LytPicture * pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ICONPARTS_POKE, &resID );

    //    m_pPokeIcon->ReplacePaneTexture(0, &spara, pPane );

  }
}

//--------------------------------------------------------------------------
/**
 * @brief   描画
 */
//--------------------------------------------------------------------------
void BgViewerLowBgDraw::Draw(gfl2::gfx::CtrDisplayNo no)
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  pG2DUtil->AddDrawableLytWkForOneFrame( mpAppRenderingManager, no, 0 );


}


//--------------------------------------------------------------------------
/**
 * @brief   破棄チェック
 * @return  trueで破棄完了
 */
//--------------------------------------------------------------------------
bool BgViewerLowBgDraw::IsExit( void )
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
::app::ui::UIInputListener::ListenerResult BgViewerLowBgDraw::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  if( m_uiListener == NULL ){
    return ENABLE_ACCESS;
  }

  // 決定
  if( pTouchPanel->IsTouchTrigger() ){
    //    m_uiListener->OnButtonAction(0);
    return DISABLE_ACCESS;
  }
  return ENABLE_ACCESS;
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
::app::ui::UIInputListener::ListenerResult BgViewerLowBgDraw::OnLayoutPaneEvent( const u32 painId )
{
  m_uiListener->OnButtonAction(painId);
  if( painId == BgViewerLowBgDrawListener::BTN_ID_CANCEL )
  {
    return DISABLE_ACCESS;
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
::app::ui::UIInputListener::ListenerResult BgViewerLowBgDraw::OnKeyAction(  gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  if( m_uiListener == NULL ){
    return ENABLE_ACCESS;
  }

  // 決定
  //  if( pButton->IsTrigger(gfl2::ui::BUTTON_B) || pButton->IsTrigger(gfl2::ui::BUTTON_START) ){
  //  m_uiListener->OnButtonAction(0);
  //return DISABLE_ACCESS;
  //  }
  // if( pButton->IsTrigger(gfl2::ui::BUTTON_A) ){
  //  m_uiListener->OnButtonAction(1);
  // return DISABLE_ACCESS;
  //  }

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
void BgViewerLowBgDraw::Initialize2D( app::util::Heap * heap, void* pAppLytBuff )
{
  // アーカイブIDテーブル
  app::util::G2DUtil::LytResData resTbl[] = {
    {  pAppLytBuff, 2,  app::util::G2DUtil::ATTACH_ROOT, },
  };

  app::util::G2DUtil::LytwkData setupData[] = {
    {//上
      0,
      LYTRES_BATTLEBGVIEW_BATTLEBGVIEW_BFLYT,
      LA_BATTLEBGVIEW_BATTLEBGVIEW___NUM,
      pAppLytBuff,
      true,
      app::util::G2DUtil::SETUP_LOWER,
      true
      },
  };


  Create2D( heap, NULL, 1,
            resTbl, GFL_NELEMS(resTbl),
            setupData, GFL_NELEMS(setupData),
            GARC_debug_message_d_btlbgviewer_DAT );


  // GetG2DUtil()->StartAnime( 0, LA_MINLOW_BOX_MIN_LOW_000_POKEICON_SET_00 );
  //  GetG2DUtil()->StartAnime( 0, LA_MINLOW_BOX_MIN_LOW_000_PATERN_NORMAL );
  //  GetG2DUtil()->StartAnime( 0, LA_MINLOW_BOX_MIN_LOW_000_BGCOLOR_BOX );

  //  GetG2DUtil()->SetPaneVisible(0,  PANENAME_BOX_MIN_LOW_000_PANE_TEAM_01,false);
  // GetG2DUtil()->SetPaneVisible(0,  PANENAME_BOX_MIN_LOW_000_PANE_BTN_JUDGE,false);
  //  GetG2DUtil()->SetPaneVisible(0,  PANENAME_BOX_MIN_LOW_000_PANE_TEAM_01,false);
  {
    gfl2::math::VEC3 vec3;

    // vec3 = GetG2DUtil()->GetPanePos(0,  PANENAME_BOX_MIN_LOW_000_PANE_TEAM_01);
    //    vec3.z -= 1.0f;
    // GetG2DUtil()->SetPanePos(0,  PANENAME_BOX_MIN_LOW_000_PANE_TEAM_01, &vec3);
  }

  //  GetG2DUtil()->StartAnime( 0, LA_MINLOW_BOX_MIN_LOW_000_BGCOLOR_BOX );


  MyCreateButtonManager();

}


//----------------------------------------------------------------------
/**
 *	@brief  初期テキスト表示
 */
//----------------------------------------------------------------------
void BgViewerLowBgDraw::InitializeText( app::util::Heap * pheap )
{

  mpMsgData = GFL_NEW(pheap->GetDeviceHeap()) gfl2::str::MsgData( print::GetMessageArcId_Debug(),
                                                                  GARC_debug_message_d_btlbgviewer_DAT, pheap->GetDeviceHeap(), gfl2::str::MsgData::LOAD_FULL );

  mpTempStrBuf = GFL_NEW( pheap->GetDeviceHeap() ) gfl2::str::StrBuf( TEMP_STRBUF_LENGTH, pheap->GetDeviceHeap() );
  mpTempStrBuf2 = GFL_NEW( pheap->GetDeviceHeap() ) gfl2::str::StrBuf( TEMP_STRBUF_LENGTH, pheap->GetDeviceHeap() );

  mpWordSet = GFL_NEW( pheap->GetDeviceHeap() ) print::WordSet( pheap->GetDeviceHeap() );





  {
    static const int layoutno = 0;
    gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( layoutno );
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( layoutno );


    int msgtbl[]={
      D_BTLBGVIEWER_001,
      D_BTLBGVIEWER_002,
      D_BTLBGVIEWER_003,
      D_BTLBGVIEWER_019,
      D_BTLBGVIEWER_004};

    int textpain[]={
      PANENAME_BATTLEBGVIEW_PANE_BUTTON_00,
      PANENAME_BATTLEBGVIEW_PANE_BUTTON_01,
      PANENAME_BATTLEBGVIEW_PANE_BUTTON_02,
      PANENAME_BATTLEBGVIEW_PANE_BUTTON_03,
      PANENAME_BATTLEBGVIEW_PANE_BUTTON_04,
    };


    for(int i=0; i < elementof(msgtbl); i++){
      gfl2::lyt::LytTextBox * text = pLytWk->GetTextBoxPane( pLytWk->GetPartsPane( textpain[i] ), PANENAME_CONFIG_BTN_LOW_000_PANE_TEXTBOX_00, &resID );
      mpMsgData->GetString(msgtbl[i], *mpTempStrBuf );
      pG2DUtil->SetTextBoxPaneString( text, mpTempStrBuf );
    }


    {
      gfl2::lyt::LytTextBox * text = pLytWk->GetTextBoxPane( pLytWk->GetPartsPane( PANENAME_BATTLEBGVIEW_PANE_OKBUTTON), PANENAME_COMMON_WIN_LOW_002_PANE_TEXTBOX_00, &resID );
      mpMsgData->GetString(D_BTLBGVIEWER_037, *mpTempStrBuf );
      pG2DUtil->SetTextBoxPaneString( text, mpTempStrBuf );
    }
    {
      gfl2::lyt::LytTextBox * text = pLytWk->GetTextBoxPane( pLytWk->GetPartsPane( PANENAME_BATTLEBGVIEW_PANE_PARTS_00), PANENAME_COMMON_BTN_LOW_008_PANE_TEXTBOX_00, &resID );
      mpMsgData->GetString(D_BTLBGVIEWER_005, *mpTempStrBuf );
      pG2DUtil->SetTextBoxPaneString( text, mpTempStrBuf );
    }

    {
      gfl2::lyt::LytTextBox * text = pLytWk->GetTextBoxPane( pLytWk->GetPartsPane( PANENAME_BATTLEBGVIEW_PANE_PARTS_01), PANENAME_COMMON_BTN_LOW_008_PANE_TEXTBOX_00, &resID );
      mpMsgData->GetString(D_BTLBGVIEWER_040, *mpTempStrBuf );
      pG2DUtil->SetTextBoxPaneString( text, mpTempStrBuf );
    }

    SkyboxOnOff(true);
    AnimChange(0);
    BackGroundChange(0,0 );
    BackGroundChange(0,1 );
    BackGroundChange(0,2 );
  }

}



void BgViewerLowBgDraw::SkyboxOnOff( bool bOn )
{
  int no = D_BTLBGVIEWER_038;
  if(!bOn){
    no = D_BTLBGVIEWER_039;
  }
  {
    static const int layoutno = 0;
    gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( layoutno );
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( layoutno );
    gfl2::lyt::LytTextBox * text = pLytWk->GetTextBoxPane( pLytWk->GetPartsPane( PANENAME_BATTLEBGVIEW_PANE_BUTTON_03), PANENAME_CONFIG_BTN_LOW_000_PANE_TEXTBOX_01, &resID );
    mpMsgData->GetString(no, *mpTempStrBuf );
    pG2DUtil->SetTextBoxPaneString( text, mpTempStrBuf );
  }
}



void BgViewerLowBgDraw::AnimChange( int no )
{
  int animmsg[]={
    D_BTLBGVIEWER_007,
    D_BTLBGVIEWER_008,
    D_BTLBGVIEWER_018,
    D_BTLBGVIEWER_020,
    D_BTLBGVIEWER_009,
    D_BTLBGVIEWER_021,
    D_BTLBGVIEWER_022,
    D_BTLBGVIEWER_010,
    D_BTLBGVIEWER_024,
    D_BTLBGVIEWER_023,
    D_BTLBGVIEWER_011,
    D_BTLBGVIEWER_026,
    D_BTLBGVIEWER_025,
    D_BTLBGVIEWER_012,
    D_BTLBGVIEWER_028,
    D_BTLBGVIEWER_027,
    D_BTLBGVIEWER_013,
    D_BTLBGVIEWER_014,
    D_BTLBGVIEWER_030,
    D_BTLBGVIEWER_029,
    D_BTLBGVIEWER_015,
    D_BTLBGVIEWER_032,
    D_BTLBGVIEWER_031,
    D_BTLBGVIEWER_016,
    D_BTLBGVIEWER_034,
    D_BTLBGVIEWER_033,
    D_BTLBGVIEWER_017,
    D_BTLBGVIEWER_036,
    D_BTLBGVIEWER_035};


  {
    static const int layoutno = 0;
    gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( layoutno );
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( layoutno );
    gfl2::lyt::LytTextBox * text = pLytWk->GetTextBoxPane( pLytWk->GetPartsPane( PANENAME_BATTLEBGVIEW_PANE_BUTTON_04), PANENAME_CONFIG_BTN_LOW_000_PANE_TEXTBOX_01, &resID );
    mpMsgData->GetString(animmsg[no], *mpTempStrBuf );
    pG2DUtil->SetTextBoxPaneString( text, mpTempStrBuf );
  }

}




void BgViewerLowBgDraw::BackGroundChange( int no , int pos)
{
  int pospain[]={
    PANENAME_BATTLEBGVIEW_PANE_BUTTON_00,
    PANENAME_BATTLEBGVIEW_PANE_BUTTON_01,
    PANENAME_BATTLEBGVIEW_PANE_BUTTON_02};


  {
    static const int layoutno = 0;
    gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( layoutno );
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( layoutno );
    gfl2::lyt::LytTextBox * text = pLytWk->GetTextBoxPane( pLytWk->GetPartsPane( pospain[pos] ), PANENAME_CONFIG_BTN_LOW_000_PANE_TEXTBOX_01, &resID );

    switch(pos){
    case 0:
      mpMsgData->GetString(D_BTLBGVIEWER_G_00+no, *mpTempStrBuf );
      break;
    case 1:
      mpMsgData->GetString(D_BTLBGVIEWER_N_00+no, *mpTempStrBuf );
      break;
    case 2:
      mpMsgData->GetString(D_BTLBGVIEWER_F_00+no, *mpTempStrBuf );
      break;
    }
    pG2DUtil->SetTextBoxPaneString( text, mpTempStrBuf );
  }

}




void BgViewerLowBgDraw::FinalizeText(void)
{
  GFL_DELETE mpWordSet;
  GFL_DELETE mpMsgData;
  GFL_DELETE mpTempStrBuf2;
  GFL_DELETE mpTempStrBuf;
}

void BgViewerLowBgDraw::SetMessage( int msgid )
{
}




//------------------------------------------------------------------------------
/**
 * @brief   ボタンマネージャの生成
 */
//------------------------------------------------------------------------------
void BgViewerLowBgDraw::MyCreateButtonManager( void )
{
  static const int partsnum = 8+6;
  static const int layoutno = 0;

  app::ui::ButtonInfoEx c_DefInfo[partsnum] = {
    {
      BgViewerLowBgDrawListener::BTN_ID_CANCEL , NULL, NULL,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BACKBUTTON_TOUCH,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BACKBUTTON_RELEASE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BACKBUTTON_CANSEL,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BACKBUTTON_TOUCH_RELEASE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BACKBUTTON_ACTIVE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BACKBUTTON_PASSIVE,
    },
    {
      BgViewerLowBgDrawListener::BTN_ID_A , NULL, NULL,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__OKBUTTON_TOUCH,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__OKBUTTON_RELEASE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__OKBUTTON_CANSEL,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__OKBUTTON_TOUCH_RELEASE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__OKBUTTON_ACTIVE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__OKBUTTON_PASSIVE,
    },
    {
      BgViewerLowBgDrawListener::BTN_ID_ANIM , NULL, NULL,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__PARTS_00_TOUCH,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__PARTS_00_RELEASE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__PARTS_00_CANSEL,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__PARTS_00_TOUCH_RELEASE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__PARTS_00_ACTIVE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__PARTS_00_PASSIVE,
    },

    {
      BgViewerLowBgDrawListener::BTN_ID_ANIM_REQ , NULL, NULL,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__PARTS_01_TOUCH,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__PARTS_01_RELEASE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__PARTS_01_CANSEL,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__PARTS_01_TOUCH_RELEASE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__PARTS_01_ACTIVE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__PARTS_01_PASSIVE,
    },

    { //左
      BgViewerLowBgDrawListener::BTN_ID_TEN_L , NULL, NULL,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_03__CURSOR_L_TOUCH,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_03__CURSOR_L_RELEASE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_03__CURSOR_L_CANSEL,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_03__CURSOR_L_TOUCH_RELEASE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_03__CURSOR_L_ACTIVE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_03__CURSOR_L_PASSIVE,
    },
    {  //右
      BgViewerLowBgDrawListener::BTN_ID_TEN_R , NULL, NULL,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_03__CURSOR_R_TOUCH,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_03__CURSOR_R_RELEASE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_03__CURSOR_R_CANSEL,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_03__CURSOR_R_TOUCH_RELEASE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_03__CURSOR_R_ACTIVE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_03__CURSOR_R_PASSIVE,
    },


    { //左
      BgViewerLowBgDrawListener::BTN_ID_ANIM_L , NULL, NULL,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_04__CURSOR_L_TOUCH,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_04__CURSOR_L_RELEASE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_04__CURSOR_L_CANSEL,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_04__CURSOR_L_TOUCH_RELEASE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_04__CURSOR_L_ACTIVE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_04__CURSOR_L_PASSIVE,
    },
    {  //右
      BgViewerLowBgDrawListener::BTN_ID_ANIM_R , NULL, NULL,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_04__CURSOR_R_TOUCH,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_04__CURSOR_R_RELEASE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_04__CURSOR_R_CANSEL,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_04__CURSOR_R_TOUCH_RELEASE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_04__CURSOR_R_ACTIVE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_04__CURSOR_R_PASSIVE,
    },



    { //左
      BgViewerLowBgDrawListener::BTN_ID_GROUND_L , NULL, NULL,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_00__CURSOR_L_TOUCH,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_00__CURSOR_L_RELEASE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_00__CURSOR_L_CANSEL,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_00__CURSOR_L_TOUCH_RELEASE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_00__CURSOR_L_ACTIVE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_00__CURSOR_L_PASSIVE,
    },
    {  //右
      BgViewerLowBgDrawListener::BTN_ID_GROUND_R , NULL, NULL,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_00__CURSOR_R_TOUCH,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_00__CURSOR_R_RELEASE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_00__CURSOR_R_CANSEL,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_00__CURSOR_R_TOUCH_RELEASE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_00__CURSOR_R_ACTIVE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_00__CURSOR_R_PASSIVE,
    },

    { //左
      BgViewerLowBgDrawListener::BTN_ID_NEAR_L , NULL, NULL,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_01__CURSOR_L_TOUCH,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_01__CURSOR_L_RELEASE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_01__CURSOR_L_CANSEL,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_01__CURSOR_L_TOUCH_RELEASE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_01__CURSOR_L_ACTIVE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_01__CURSOR_L_PASSIVE,
    },
    {  //右
      BgViewerLowBgDrawListener::BTN_ID_NEAR_R , NULL, NULL,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_01__CURSOR_R_TOUCH,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_01__CURSOR_R_RELEASE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_01__CURSOR_R_CANSEL,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_01__CURSOR_R_TOUCH_RELEASE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_01__CURSOR_R_ACTIVE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_01__CURSOR_R_PASSIVE,
    },

    { //左
      BgViewerLowBgDrawListener::BTN_ID_FAR_L , NULL, NULL,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_02__CURSOR_L_TOUCH,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_02__CURSOR_L_RELEASE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_02__CURSOR_L_CANSEL,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_02__CURSOR_L_TOUCH_RELEASE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_02__CURSOR_L_ACTIVE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_02__CURSOR_L_PASSIVE,
    },
    {  //右
      BgViewerLowBgDrawListener::BTN_ID_FAR_R , NULL, NULL,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_02__CURSOR_R_TOUCH,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_02__CURSOR_R_RELEASE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_02__CURSOR_R_CANSEL,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_02__CURSOR_R_TOUCH_RELEASE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_02__CURSOR_R_ACTIVE,
      LA_BATTLEBGVIEW_BATTLEBGVIEW__BUTTON_02__CURSOR_R_PASSIVE,
    },

  };

  int painNo[]={
    PANENAME_BATTLEBGVIEW_PANE_BACKBUTTON,
    PANENAME_BATTLEBGVIEW_PANE_OKBUTTON,
    PANENAME_BATTLEBGVIEW_PANE_PARTS_00,
    PANENAME_BATTLEBGVIEW_PANE_PARTS_01,
    PANENAME_BATTLEBGVIEW_PANE_BUTTON_03,
    PANENAME_BATTLEBGVIEW_PANE_BUTTON_03,
    PANENAME_BATTLEBGVIEW_PANE_BUTTON_04,
    PANENAME_BATTLEBGVIEW_PANE_BUTTON_04,
    PANENAME_BATTLEBGVIEW_PANE_BUTTON_00,
    PANENAME_BATTLEBGVIEW_PANE_BUTTON_00,
    PANENAME_BATTLEBGVIEW_PANE_BUTTON_01,
    PANENAME_BATTLEBGVIEW_PANE_BUTTON_01,
    PANENAME_BATTLEBGVIEW_PANE_BUTTON_02,
    PANENAME_BATTLEBGVIEW_PANE_BUTTON_02,
  };

  int painNo2[]={
    0,
    0,
    0,
    0,
    PANENAME_CONFIG_BTN_LOW_000_PANE_CURSOR_L,
    PANENAME_CONFIG_BTN_LOW_000_PANE_CURSOR_R,
    PANENAME_CONFIG_BTN_LOW_000_PANE_CURSOR_L,
    PANENAME_CONFIG_BTN_LOW_000_PANE_CURSOR_R,
    PANENAME_CONFIG_BTN_LOW_000_PANE_CURSOR_L,
    PANENAME_CONFIG_BTN_LOW_000_PANE_CURSOR_R,
    PANENAME_CONFIG_BTN_LOW_000_PANE_CURSOR_L,
    PANENAME_CONFIG_BTN_LOW_000_PANE_CURSOR_R,
    PANENAME_CONFIG_BTN_LOW_000_PANE_CURSOR_L,
    PANENAME_CONFIG_BTN_LOW_000_PANE_CURSOR_R,
  };


  int boundNo[]={
    PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00,
    PANENAME_COMMON_CUR_LOW_004_PANE_BOUND_00,
    PANENAME_COMMON_CUR_LOW_004_PANE_BOUND_00,
    PANENAME_COMMON_CUR_LOW_004_PANE_BOUND_00,
    PANENAME_COMMON_CUR_LOW_004_PANE_BOUND_00,
    PANENAME_COMMON_CUR_LOW_004_PANE_BOUND_00,
    PANENAME_COMMON_CUR_LOW_004_PANE_BOUND_00,
    PANENAME_COMMON_CUR_LOW_004_PANE_BOUND_00,
    PANENAME_COMMON_CUR_LOW_004_PANE_BOUND_00,
    PANENAME_COMMON_CUR_LOW_004_PANE_BOUND_00,
    PANENAME_COMMON_CUR_LOW_004_PANE_BOUND_00
    };



  app::ui::ButtonInfoEx * info = GFL_NEW_LOW_ARRAY(mpAppHeap->GetSystemHeap()) app::ui::ButtonInfoEx[partsnum];

  int i=0;
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( layoutno );
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( layoutno );

  gfl2::lyt::LytParts* pParts[partsnum];

  for(i=0;i<partsnum;i++){
    pParts[i] = pLytWk->GetPartsPane( painNo[i] );

    if(i >= 4){ //部品の部品
      pParts[i] = pLytWk->GetPartsPane(pParts[i], painNo2[i], &resID );
    }
  }
  for( i = 0 ; i < partsnum; i++){
    info[i] = c_DefInfo[i];
    info[i].picture_pane = pParts[i];
    info[i].bound_pane = pLytWk->GetPane( pParts[i], boundNo[i], &resID );

  }
  this->CreateButtonManager( mpAppHeap, pLytWk, info, partsnum );

  GFL_DELETE_ARRAY info;

  app::tool::ButtonManager* pBtnMan = this->GetButtonManager();
  pBtnMan->SetButtonBindKey( BgViewerLowBgDrawListener::BTN_ID_CANCEL, gfl2::ui::BUTTON_B, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );

  pBtnMan->SetButtonBindKey( BgViewerLowBgDrawListener::BTN_ID_A, gfl2::ui::BUTTON_A, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );

  pBtnMan->SetButtonBindKey( BgViewerLowBgDrawListener::BTN_ID_ANIM, gfl2::ui::BUTTON_X, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );


}


void BgViewerLowBgDraw::AnimStartOnOff(bool bAnim)
{
  static const int layoutno = 0;
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( layoutno );
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( layoutno );
  
  {
    gfl2::lyt::LytTextBox * text = pLytWk->GetTextBoxPane( pLytWk->GetPartsPane( PANENAME_BATTLEBGVIEW_PANE_PARTS_00), PANENAME_COMMON_BTN_LOW_008_PANE_TEXTBOX_00, &resID );
    if(bAnim){
      mpMsgData->GetString(D_BTLBGVIEWER_005, *mpTempStrBuf );
    }
    else{
      mpMsgData->GetString(D_BTLBGVIEWER_006, *mpTempStrBuf );
    }
    pG2DUtil->SetTextBoxPaneString( text, mpTempStrBuf );
  }

}


GFL_NAMESPACE_END( Ariizumi )
GFL_NAMESPACE_END( Test )

#endif //PM_DEBUG
