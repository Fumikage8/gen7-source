//==============================================================================
/**
 * @file	  BoxLowBattleInDraw.cpp
 * @brief	  メイン下画面描画
 * @author	ohno
 * @date	  2015.9.24
 */
// =============================================================================

#include "BoxLowBattleInDraw.h"
#include "BoxLowBgDraw.h"

//素材
#include <arc_def_index/arc_def.h>
#include <arc_index/boxLyt.gaix>
#include <niji_conv_header/app/box/res2d/minlow.h>
#include <niji_conv_header/app/box/res2d/minlow_anim_list.h>
#include <niji_conv_header/app/box/res2d/minlow_pane.h>
#include <niji_conv_header/app/box/res2d/bgrlow.h>
#include <niji_conv_header/app/box/res2d/bgrlow_pane.h>
#include <niji_conv_header/app/box/res2d/bgrlow_anim_list.h>
#include "niji_conv_header/app/box/res2d/bgrupp.h"
#include "niji_conv_header/app/box/res2d/bgrupp_pane.h"
#include "niji_conv_header/app/box/res2d/bgrupp_anim_list.h"


#include "arc_index/message.gaix"
#include <niji_conv_header/message/msg_box.h>
#include <niji_conv_header/message/msg_boxname.h>

#include <Layout/include/gfl2_LayoutRenderPath.h>

#include "SaveData/include/BoxSave.h"
#include "SaveData/include/BoxPokemonSave.h"
#include "AppLib/include/Tool/PokeIcon.h"
#include "AppLib/include/Tool/ItemIcon.h"


#include "PokeTool/include/PokeTool.h"


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )


#define  PANE_CURSOR  PANENAME_BOX_MIN_LOW_000_PANE_CURSOR


enum{
  POKETEMO_POS01 = 0,//ここから
  POKETEMO_POS02,
  POKETEMO_POS03,
  POKETEMO_POS04,
  POKETEMO_POS05,
  POKETEMO_POS06,

  POKEBUTTON_BOX,

  BATTLEIN_PANEL_MAX,
  
  POKETEMO_POS01_02,
  POKETEMO_PAGE_R,
  POKETEMO_PAGE_L,
    
  MOVE_NULL,
};


int label_to_pane[]=
{
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_90,
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_91,
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_92,
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_93,
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_94,
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_95,

  PANENAME_BOX_PLT_LOW_000_PANE_TEXTBOX_00,
  
};



int buttonmove[][5]=
{
  //上下左右を記す
  {  POKEBUTTON_BOX, POKETEMO_POS03, MOVE_NULL, POKETEMO_POS02, POKETEMO_POS01 },  //  POKETEMO_POS01
  {  POKEBUTTON_BOX, POKETEMO_POS04, POKETEMO_POS01, MOVE_NULL, POKETEMO_POS02 },  //  POKETEMO_POS02
  {  POKETEMO_POS01, POKETEMO_POS05, MOVE_NULL, POKETEMO_POS04, MOVE_NULL },  //  POKETEMO_POS03
  {  POKETEMO_POS02, POKETEMO_POS06, POKETEMO_POS03, MOVE_NULL, MOVE_NULL },  //  POKETEMO_POS04
  {  POKETEMO_POS03, MOVE_NULL, MOVE_NULL, POKETEMO_POS06,      MOVE_NULL },  //  POKETEMO_POS05
  {  POKETEMO_POS04, MOVE_NULL, POKETEMO_POS05, MOVE_NULL,      MOVE_NULL },  //  POKETEMO_POS06

  {  MOVE_NULL, POKETEMO_POS01_02, POKETEMO_PAGE_L, POKETEMO_PAGE_R,MOVE_NULL },  //  ボタン
  
};




//--------------------------------------------------------------------------





//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

//----------------------------------------------------------------------
/**
 * @brief コンストラクタ
 * @param heap  ヒープ
 */
//----------------------------------------------------------------------
BoxLowBattleInDraw::BoxLowBattleInDraw( BoxLowBgDraw* pBoxLowBgDraw )
{
  mpBoxLowBgDraw = pBoxLowBgDraw;
  mpUiListener = NULL;
}

/// デストラクタ
BoxLowBattleInDraw::~BoxLowBattleInDraw( void )
{
}

//----------------------------------------------------------------------
/**
 *  @brief  初期化
 */
//----------------------------------------------------------------------

void BoxLowBattleInDraw::Init(void)
{

  mpBoxLowBgDraw->SetInputListener( this );
}

//----------------------------------------------------------------------
/**
 *  @brief  終了処理
 */
//----------------------------------------------------------------------

bool BoxLowBattleInDraw::End(void)
{
  return true;
}


//----------------------------------------------------------------------
/**
 *  @brief  UIイベント用リスナーの登録
 */
//----------------------------------------------------------------------
void BoxLowBattleInDraw::SetUIListener( BoxLowBattleInDrawListener *listener )
{
  mpUiListener = listener;
}

//--------------------------------------------------------------------------
/**
 * @brief   更新
 */
//--------------------------------------------------------------------------
void BoxLowBattleInDraw::Update(void)
{

}


//--------------------------------------------------------------------------
/**
 * @brief   破棄チェック
 * @return  trueで破棄完了
 */
//--------------------------------------------------------------------------
bool BoxLowBattleInDraw::IsExit( void )
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
::app::ui::UIInputListener::ListenerResult BoxLowBattleInDraw::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  if( mpUiListener == NULL ){
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
::app::ui::UIInputListener::ListenerResult BoxLowBattleInDraw::OnLayoutPaneEvent( const u32 painId )
{
  switch(painId){
  case BTN_ID_CANCEL:
  {
    mpUiListener->OnButtonAction(BoxLowBattleInDrawListener::LOWBG_ACTION_CANCEL);  //終了
    return DISABLE_ACCESS;
  }
  case BTN_ID_R1:
    {
      mpUiListener->OnButtonAction(BoxLowBattleInDrawListener::LOWBG_ACTION_R_CHANGE);
    }
    break;
  case BTN_ID_L1:
    {
      mpUiListener->OnButtonAction(BoxLowBattleInDrawListener::LOWBG_ACTION_L_CHANGE);
    }
    break;
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
::app::ui::UIInputListener::ListenerResult BoxLowBattleInDraw::OnKeyAction(  gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  if( mpUiListener == NULL ){
    return ENABLE_ACCESS;
  }

  if(pButton->IsTrigger(gfl2::ui::BUTTON_UP)){
    mpUiListener->OnButtonAction(BoxLowBattleInDrawListener::LOWBG_ACTION_UP);
    return ENABLE_ACCESS;
  }
  else if(pButton->IsTrigger(gfl2::ui::BUTTON_DOWN)){
    mpUiListener->OnButtonAction(BoxLowBattleInDrawListener::LOWBG_ACTION_DOWN);
    return ENABLE_ACCESS;
  }
  else if(pButton->IsTrigger(gfl2::ui::BUTTON_RIGHT)){
    mpUiListener->OnButtonAction(BoxLowBattleInDrawListener::LOWBG_ACTION_RIGHT);
    return ENABLE_ACCESS;
  }
  else if(pButton->IsTrigger(gfl2::ui::BUTTON_LEFT)){
    mpUiListener->OnButtonAction(BoxLowBattleInDrawListener::LOWBG_ACTION_LEFT);
    return ENABLE_ACCESS;
  }
  else if(pButton->IsTrigger(gfl2::ui::BUTTON_L)){
    mpUiListener->OnButtonAction(BoxLowBattleInDrawListener::LOWBG_ACTION_L_CHANGE);
    return ENABLE_ACCESS;
  }
  else if(pButton->IsTrigger(gfl2::ui::BUTTON_R)){
    mpUiListener->OnButtonAction(BoxLowBattleInDrawListener::LOWBG_ACTION_R_CHANGE);
    return ENABLE_ACCESS;
  }
  else if( pButton->IsTrigger(gfl2::ui::BUTTON_B)  ){
    mpUiListener->OnButtonAction(BoxLowBattleInDrawListener::LOWBG_ACTION_CANCEL);  //終了
    return DISABLE_ACCESS;
  }
  else if( pButton->IsTrigger(gfl2::ui::BUTTON_A) ){  // 決定
    switch(mButtonPos){
    case POKEBUTTON_BOX:
      //反応なし
      break;
    case POKETEMO_POS01:
    case POKETEMO_POS02:
    case POKETEMO_POS03:
    case POKETEMO_POS04:
    case POKETEMO_POS05:
    case POKETEMO_POS06:
      mpUiListener->OnButtonAction(BoxLowBattleInDrawListener::LOWBG_ACTION_DECIDE);  //決定
      break;
    default:
      break;
    }
    return DISABLE_ACCESS;
  }

  return ENABLE_ACCESS;
}

//==========================================================================
/**
 *        private関数
 */
//==========================================================================





int BoxLowBattleInDraw::SetCursorPos(int no,int udlr )
{
  gfl2::math::VEC3 vec3;

  GFL_ASSERT(no < BATTLEIN_PANEL_MAX);

  int nextbutton = buttonmove[no][udlr];  //次の位置
  int ver = buttonmove[no][4];

  if(ver!=0){
    mKeepButtonPos = ver;
  }

  switch(nextbutton){
	case POKETEMO_POS01_02:
		nextbutton = mKeepButtonPos;
		break;
  }

  vec3 = mpBoxLowBgDraw->GetG2DUtil()->GetPanePos(0,  label_to_pane[nextbutton]);
  mpBoxLowBgDraw->GetG2DUtil()->SetPanePos(0,  PANE_CURSOR, &vec3);

  mButtonPos = nextbutton;
  return nextbutton;
}






//------------------------------------------------------------------------------
/**
 * @brief   ボタンマネージャの生成
 */
//------------------------------------------------------------------------------
/*
void BoxLowBattleInDraw::MyCreateButtonManager( void )
{
  static const int partsnum = 8;
  static const int layoutno = LYT_WORK_NORMAL;

  app::ui::ButtonInfoEx c_DefInfo[partsnum] = {
    {
      BTN_ID_MARK00 , NULL, NULL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_00_TOUCH,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_00_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_00_CANSEL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_00_TOUCH_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_00_ACTIVE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_00_PASSIVE,
    },
    {
      BTN_ID_MARK01 , NULL, NULL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_01_TOUCH,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_01_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_01_CANSEL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_01_TOUCH_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_01_ACTIVE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_01_PASSIVE,
    },
    {
      BTN_ID_MARK02 , NULL, NULL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_02_TOUCH,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_02_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_02_CANSEL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_02_TOUCH_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_02_ACTIVE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_02_PASSIVE,
    },
    {
      BTN_ID_MARK03 , NULL, NULL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_03_TOUCH,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_03_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_03_CANSEL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_03_TOUCH_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_03_ACTIVE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_03_PASSIVE,
    },
    {
      BTN_ID_MARK04 , NULL, NULL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_04_TOUCH,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_04_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_04_CANSEL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_04_TOUCH_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_04_ACTIVE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_04_PASSIVE,
    },
    {
      BTN_ID_MARK05 , NULL, NULL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_05_TOUCH,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_05_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_05_CANSEL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_05_TOUCH_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_05_ACTIVE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_05_PASSIVE,
    },


  };

  int painNo[]={
    PANENAME_BOX_MIN_LOW_000_PANE_BTN_BACK,
    PANENAME_BOX_MIN_LOW_000_PANE_CURSOR_R1,
    PANENAME_BOX_MIN_LOW_000_PANE_CURSOR_L1,
  };


  //  app::ui::ButtonInfoEx* info;

  app::ui::ButtonInfoEx * info = GFL_NEW_LOW_ARRAY(mpAppHeap->GetSystemHeap()) app::ui::ButtonInfoEx[partsnum];

  int i=0;
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( layoutno );
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( layoutno );

  gfl2::lyt::LytParts* pParts[partsnum];

  for(i=0;i<partsnum;i++){
    pParts[i] = pLytWk->GetPartsPane( painNo[i] );
  }
  for( i = 0 ; i < partsnum; i++){
    info[i] = c_DefInfo[i];
    info[i].picture_pane = pParts[i];
    info[i].bound_pane = pLytWk->GetPane( pParts[i], PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &resID );
  }
  this->CreateButtonManager( mpAppHeap, pLytWk, info, partsnum );

  GFL_DELETE_ARRAY info;

  app::tool::ButtonManager* pBtnMan = this->GetButtonManager();
  pBtnMan->SetButtonBindKey( BTN_ID_CANCEL, gfl2::ui::BUTTON_B, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );
  //  pBtnMan->SetButtonSelectSE( BTN_ID_CANCEL, SEQ_SE_ISLAND_CNACEL );


}

*/













GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )

