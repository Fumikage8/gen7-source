//==============================================================================
/**
 * @file	  BoxLowBgDraw.cpp
 * @brief	  メイン下画面描画
 * @author	ohno
 * @data	  February 17, 2015
 */
// =============================================================================

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


#include "arc_index/message.gaix"
#include <niji_conv_header/message/msg_box.h>
#include <niji_conv_header/message/msg_boxname.h>

#include <Layout/include/gfl2_LayoutRenderPath.h>

#include "BoxSimpleAccesser.h"

#include "App/Box/include/BoxAppSetup.h"
#include "SaveData/include/BoxSave.h"
#include "SaveData/include/BoxPokemonSave.h"
#include "AppLib/include/Tool/PokeIcon.h"
#include "AppLib/include/Tool/ItemIcon.h"
#include "AppLib/include/Tool/app_tool_TimeIcon.h"


#include "PokeTool/include/PokeTool.h"
#include <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookType.h>
#include <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookTypeListener.h>
#include <AppLib/include/Util/app_util_KeyRepeatController.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )


#define  PANE_CURSOR  PANENAME_BOX_MIN_LOW_000_PANE_CURSOR

#define _CARRY_UP_Y 6


//バトルカーソルの時のenum
enum{
  BTLIN_POKETEMO_POS01 = 0,//ここから
  BTLIN_POKETEMO_POS02,
  BTLIN_POKETEMO_POS03,
  BTLIN_POKETEMO_POS04,
  BTLIN_POKETEMO_POS05,
  BTLIN_POKETEMO_POS06,

  BTLIN_POKEBUTTON_BOX,

  BTLIN_PANEL_MAX,

  BTLIN_POKETEMO_POS01_02,
  BTLIN_POKETEMO_POS05_06,
  BTLIN_POKETEMO_PAGE_R,
  BTLIN_POKETEMO_PAGE_L,

  BTLIN_MOVE_NULL,
};


//通常カーソル時のenum
enum{
  POKEBOX_POS01=0,//ここから
  POKEBOX_POS02,
  POKEBOX_POS03,
  POKEBOX_POS04,
  POKEBOX_POS05,
  POKEBOX_POS06,
  POKEBOX_POS07,
  POKEBOX_POS08,
  POKEBOX_POS09,
  POKEBOX_POS10,
  POKEBOX_POS11,
  POKEBOX_POS12,
  POKEBOX_POS13,
  POKEBOX_POS14,
  POKEBOX_POS15,
  POKEBOX_POS16,
  POKEBOX_POS17,
  POKEBOX_POS18,
  POKEBOX_POS19,
  POKEBOX_POS20,
  POKEBOX_POS21,
  POKEBOX_POS22,
  POKEBOX_POS23,
  POKEBOX_POS24,
  POKEBOX_POS25,
  POKEBOX_POS26,
  POKEBOX_POS27,
  POKEBOX_POS28,
  POKEBOX_POS29,
  POKEBOX_POS30,

  POKETEMO_POS01,
  POKETEMO_POS02,
  POKETEMO_POS03,
  POKETEMO_POS04,
  POKETEMO_POS05,
  POKETEMO_POS06,


  POKEBUTTON_BOX,
  POKEBUTTON_TEMO,
  POKEBUTTON_FOLDER,//ここまでは並び依存

  POKE_PANEL_MAX,

  POKEBOX_0_5,  //0から５のどれか
  POKETEMO_1_2,
  POKETEMO_5_6,
  POKEBOX_25_30,
  POKEBOX_1_7,
  POKEBOX_19_25,
  POKEBOX_6_12,
  POKEBOX_24_30,

  POKE_PAGE_R,
  POKE_PAGE_L,

  POKE_PAGE_R_TEAM,
  POKE_PAGE_L_TEAM,

};


enum{
  VERTICAL01=0,
  VERTICAL02,
  VERTICAL03,
  VERTICAL04,
  VERTICAL05,

  HORIZONTAL01,
  HORIZONTAL02,
  HORIZONTAL03,
  HORIZONTAL04,
  HORIZONTAL05,
  HORIZONTAL06,
  HORIZONTAL07,
  HORIZONTAL08,

};


int label_to_pane[]=
{
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_00,
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_01,
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_02,
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_03,
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_04,
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_05,
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_06,
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_07,
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_08,
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_09,
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_10,
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_11,
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_12,
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_13,
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_14,
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_15,
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_16,
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_17,
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_18,
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_19,
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_20,
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_21,
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_22,
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_23,
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_24,
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_25,
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_26,
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_27,
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_28,
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_29,

  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_90,
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_91,
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_92,
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_93,
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_94,
  PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_95,
  PANENAME_BOX_MIN_LOW_000_PANE_BTN_BOXCATEGORY_00,
  PANENAME_BOX_MIN_LOW_000_PANE_TEAMTEXTPOS,//PANENAME_BOX_MIN_LOW_000_PANE_TEAM_0,0
  PANENAME_BOX_MIN_LOW_000_PANE_BTN_MODE_CURPOS,

};



int buttonmove[][6]=
{
  //上下左右を記す
  {  POKEBUTTON_BOX, POKEBOX_POS07, POKETEMO_POS02, POKEBOX_POS02,VERTICAL01,HORIZONTAL01},  //  POKEBOX_POS01
  {  POKEBUTTON_BOX, POKEBOX_POS08, POKEBOX_POS01, POKEBOX_POS03, VERTICAL01,HORIZONTAL02},  //  POKEBOX_POS02
  {  POKEBUTTON_BOX, POKEBOX_POS09, POKEBOX_POS02, POKEBOX_POS04, VERTICAL01,HORIZONTAL03},  //  POKEBOX_POS03
  {  POKEBUTTON_BOX, POKEBOX_POS10, POKEBOX_POS03, POKEBOX_POS05, VERTICAL01,HORIZONTAL04},  //  POKEBOX_POS04
  {  POKEBUTTON_BOX, POKEBOX_POS11, POKEBOX_POS04, POKEBOX_POS06, VERTICAL01,HORIZONTAL05},  //  POKEBOX_POS05
  {  POKEBUTTON_BOX, POKEBOX_POS12, POKEBOX_POS05, POKETEMO_POS01, VERTICAL01,HORIZONTAL06},  //  POKEBOX_POS06

  {  POKEBOX_POS01, POKEBOX_POS13, POKETEMO_POS02, POKEBOX_POS08, VERTICAL02,HORIZONTAL01},  //  POKEBOX_POS07
  {  POKEBOX_POS02, POKEBOX_POS14, POKEBOX_POS07,  POKEBOX_POS09, VERTICAL02,HORIZONTAL02},  //  POKEBOX_POS08
  {  POKEBOX_POS03, POKEBOX_POS15, POKEBOX_POS08,  POKEBOX_POS10, VERTICAL02,HORIZONTAL03},  //  POKEBOX_POS09
  {  POKEBOX_POS04, POKEBOX_POS16, POKEBOX_POS09,  POKEBOX_POS11, VERTICAL02,HORIZONTAL04},  //  POKEBOX_POS10
  {  POKEBOX_POS05, POKEBOX_POS17, POKEBOX_POS10,  POKEBOX_POS12, VERTICAL02,HORIZONTAL05},  //  POKEBOX_POS11
  {  POKEBOX_POS06, POKEBOX_POS18, POKEBOX_POS11,  POKETEMO_POS01, VERTICAL02,HORIZONTAL06},  //  POKEBOX_POS12

  {  POKEBOX_POS07, POKEBOX_POS19, POKETEMO_POS04, POKEBOX_POS14, VERTICAL03,HORIZONTAL01},  //  POKEBOX_POS13
  {  POKEBOX_POS08, POKEBOX_POS20, POKEBOX_POS13,  POKEBOX_POS15, VERTICAL03,HORIZONTAL02},  //  POKEBOX_POS14
  {  POKEBOX_POS09, POKEBOX_POS21, POKEBOX_POS14,  POKEBOX_POS16, VERTICAL03,HORIZONTAL03},  //  POKEBOX_POS15
  {  POKEBOX_POS10, POKEBOX_POS22, POKEBOX_POS15,  POKEBOX_POS17, VERTICAL03,HORIZONTAL04},  //  POKEBOX_POS16
  {  POKEBOX_POS11, POKEBOX_POS23, POKEBOX_POS16,  POKEBOX_POS18, VERTICAL03,HORIZONTAL05},  //  POKEBOX_POS17
  {  POKEBOX_POS12, POKEBOX_POS24, POKEBOX_POS17,  POKETEMO_POS03, VERTICAL03,HORIZONTAL06},  //  POKEBOX_POS18

  {  POKEBOX_POS13, POKEBOX_POS25, POKETEMO_POS06, POKEBOX_POS20, VERTICAL04,HORIZONTAL01},  //  POKEBOX_POS19
  {  POKEBOX_POS14, POKEBOX_POS26, POKEBOX_POS19,  POKEBOX_POS21, VERTICAL04,HORIZONTAL02},  //  POKEBOX_POS20
  {  POKEBOX_POS15, POKEBOX_POS27, POKEBOX_POS20,  POKEBOX_POS22, VERTICAL04,HORIZONTAL03},  //  POKEBOX_POS21
  {  POKEBOX_POS16, POKEBOX_POS28, POKEBOX_POS21,  POKEBOX_POS23, VERTICAL04,HORIZONTAL04},  //  POKEBOX_POS22
  {  POKEBOX_POS17, POKEBOX_POS29, POKEBOX_POS22,  POKEBOX_POS24, VERTICAL04,HORIZONTAL05},  //  POKEBOX_POS23
  {  POKEBOX_POS18, POKEBOX_POS30, POKEBOX_POS23,  POKETEMO_POS05, VERTICAL04,HORIZONTAL06}, //  POKEBOX_POS24


  {  POKEBOX_POS19, POKEBUTTON_FOLDER, POKETEMO_POS06, POKEBOX_POS26, VERTICAL05,HORIZONTAL01},  //  POKEBOX_POS25
  {  POKEBOX_POS20, POKEBUTTON_FOLDER, POKEBOX_POS25,  POKEBOX_POS27, VERTICAL05,HORIZONTAL02},  //  POKEBOX_POS26
  {  POKEBOX_POS21, POKEBUTTON_FOLDER, POKEBOX_POS26,  POKEBOX_POS28, VERTICAL05,HORIZONTAL03},  //  POKEBOX_POS27
  {  POKEBOX_POS22, POKEBUTTON_FOLDER, POKEBOX_POS27,  POKEBOX_POS29, VERTICAL05,HORIZONTAL04},  //  POKEBOX_POS28
  {  POKEBOX_POS23, POKEBUTTON_FOLDER, POKEBOX_POS28,  POKEBOX_POS30, VERTICAL05,HORIZONTAL05},  //  POKEBOX_POS29
  {  POKEBOX_POS24, POKEBUTTON_FOLDER, POKEBOX_POS29,  POKETEMO_POS05, VERTICAL05,HORIZONTAL06},  // POKEBOX_POS30

  {  POKEBUTTON_TEMO, POKETEMO_POS03, POKEBOX_6_12, POKETEMO_POS02,-1,HORIZONTAL07},  //  POKETEMO_POS01
  {  POKEBUTTON_TEMO, POKETEMO_POS04, POKETEMO_POS01, POKEBOX_1_7,-1,HORIZONTAL08},  //  POKETEMO_POS02
  {  POKETEMO_POS01, POKETEMO_POS05, POKEBOX_POS18, POKETEMO_POS04,-1,HORIZONTAL07},  //  POKETEMO_POS03
  {  POKETEMO_POS02, POKETEMO_POS06, POKETEMO_POS03, POKEBOX_POS13,-1,HORIZONTAL08},  //  POKETEMO_POS04
  {  POKETEMO_POS03, POKEBUTTON_TEMO, POKEBOX_24_30, POKETEMO_POS06,-1,HORIZONTAL07},  //  POKETEMO_POS05
  {  POKETEMO_POS04, POKEBUTTON_TEMO, POKETEMO_POS05, POKEBOX_19_25,-1,HORIZONTAL08},  //  POKETEMO_POS06

  {  POKEBUTTON_FOLDER, POKEBOX_0_5, POKE_PAGE_L, POKE_PAGE_R,-1,-1},  //POKEBUTTON_BOX
  {  POKETEMO_5_6, POKETEMO_1_2, POKE_PAGE_L_TEAM, POKE_PAGE_R_TEAM,-1,-1},  //POKEBUTTON_TEMO
  {  POKEBOX_25_30, POKEBUTTON_BOX, POKEBUTTON_FOLDER, POKEBUTTON_FOLDER,-1,-1},  //POKEBUTTON_FOLDER
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
BoxLowBgDraw::BoxLowBgDraw( app::util::Heap * heap, app::util::AppRenderingManager* pAppRenderingManager ) :
app::ui::UIView( heap, heap->GetSystemAllocator(), heap->GetDeviceAllocator() )
,mpAppHeap( heap )
,m_uiListener( NULL )
{
  mpAppRenderingManager = pAppRenderingManager;
  mKeepCursorPosVertical = VERTICAL01;
  mKeepCursorPosHorizontal = HORIZONTAL01;
  // mDownCount=0;
  mbCarryPokemon=false;
  mbMarkWin = false;
  mBkButtonPos=-1;
  mAnimSeq=0;
  mMousePokemonNo=-1;
  mbActiveRangeButton=true;
  _delayTimer=0;
  mbPokeIconInit=false;
  mVecMoveCursorBackup.x = 0.0f;//GFNMCat[2831]
  mVecMoveCursorBackup.y = 0.0f;
  mVecMoveCursorBackup.z = 0.0f;
 // mbCancelButtonPassive = true;
}

/// デストラクタ
BoxLowBgDraw::~BoxLowBgDraw( void )
{
}

//----------------------------------------------------------------------
/**
 *  @brief  初期化
 */
//----------------------------------------------------------------------

void BoxLowBgDraw::Init(APP_PARAM* pAppParam, void* pAppLytBuff,BoxAppSetup* pBoxAppSetup, bool TouchPassive)
{
  gfl2::lyt::LytParts* pParts;
  mpAppParam = pAppParam;
  mpBoxAppSetup = pBoxAppSetup;
  mpMsgData = mpAppParam->pMsgData;

  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( mpAppHeap, false );

  Initialize2D( mpAppHeap,pAppLytBuff );

  mbTemotiTrayCenter = false;

  InitializeText(mpAppHeap);

  mbTouchPassive = TouchPassive;
  mbTouchPassiveF = TouchPassive;

  SetInputListener( this );

  IconClear();
  LiftPokeIconClear();
	StartMainCursor();

//  LiftPokeIconEnd(0,0);

  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = pG2DUtil->GetLayoutWork( LYT_WORK_NORMAL );

  pG2DUtil->StartAnime(  LYT_WORK_NORMAL,  LA_MINLOW_BOX_MIN_LOW_000_BG_KEEP );
  pG2DUtil->StartAnime(  LYT_WORK_NORMAL,  LA_MINLOW_BOX_MIN_LOW_000__CURSOR_KEEP );

  if(mpBoxAppSetup->GetBoxSimpleAccesser()->IsBoxSearchMode() ){
    pG2DUtil->StartAnime(  LYT_WORK_NORMAL,  LA_MINLOW_BOX_MIN_LOW_000__BTN_SEARCH_SEARCH_ON );//GFNMCat[1970]
  }
  if(mpAppParam->type == BOX_CALL_TRADE){
    mbTouchPassive=false;
    pG2DUtil->StartAnime(  LYT_WORK_NORMAL,  LA_MINLOW_BOX_MIN_LOW_000_PATERN_TRADE );
  }
  else{
    pG2DUtil->StartAnime(  LYT_WORK_NORMAL,  LA_MINLOW_BOX_MIN_LOW_000_PATERN_NORMAL );
  }
  {
    pParts = lytwk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_OBJECTTYPEBUTTON );
    if( mpAppParam->type == BOX_CALL_MIRACLE || mpAppParam->type == BOX_CALL_GTS || mpAppParam->type == BOX_CALL_TRADE || mpAppParam->type == BOX_CALL_FIELD_TRADE )
    {
      pParts->SetVisible(false);
    }
    if(BOX_CALL_BATTLE_TEAM == mpAppParam->type){  //バトルチームモードの時はバトルチーム
      pParts->SetVisible(false);
    }
    if(mpAppParam->type == BOX_CALL_FIELD_TRADE){
      pParts = lytwk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_BTN_MOVETYPE_00 );
      pParts->SetVisible(false);
      pParts = lytwk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_BTN_MOVETYPE_01 );
      pParts->SetVisible(false);
      pParts = lytwk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_BTN_MOVETYPE_02 );
      pParts->SetVisible(false);
    }
  }



  for(int pos=0;pos<Savedata::BoxPokemon::TRAY_POKE_MAX;pos++){
    pParts = lytwk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_108+pos );
    pParts->SetVisible(false);
  }

  mbPokemonTouchOn = true;
  DrawBoxName();

  BgChange(mpAppParam->pBoxData->GetWallPaper( mpAppParam->nowDispTray ));
  
  _iconPoke30Backup();


  if( mpAppParam->type == BOX_CALL_PC || mpAppParam->type == BOX_CALL_BATTLE_TEAM || mpAppParam->type == BOX_CALL_RESORT_GIM || mpAppParam->type == BOX_CALL_RESORT_HOTSPA )
  {
    if((mpBoxAppSetup->GetSelectMode() == BOX_CHANGE_EDITTYPE_ITEM))
    {//アイテムのとき
      ActiveRangeButton(false); //NMCat[1273]
    }
  }


  
}



void BoxLowBgDraw::_iconPoke30Backup(void)
{
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::math::VEC3 vec3;
  gfl2::lyt::LytParts* pParts;

  pParts = pLytWk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_30 );
  vec3 = pParts->GetTranslate();
  mVecMoveCursorBackup = vec3;
}




//----------------------------------------------------------------------
/**
 *  @brief  終了処理
 */
//----------------------------------------------------------------------

bool BoxLowBgDraw::End(void)
{
  GFL_SAFE_DELETE(mpInfoWindowBookType);

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
void BoxLowBgDraw::SetUIListener( BoxLowBgDrawListener *listener )
{
#if PM_DEBUG
  if(listener){
    listener->DebugDisp();
  }
  else{
    OHNO_PRINT("SetUIListener null\n");
  }
#endif
  if(listener){
    SetInputEnabled(listener->GetInputEnableFlg());
  }
  m_uiListener = listener;
}

//--------------------------------------------------------------------------
/**
 * @brief   更新
 */
//--------------------------------------------------------------------------
void BoxLowBgDraw::Update(void)
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

  AnimRevCheck();

  pG2DUtil->PrintMessage();
  app::ui::UIView::Update();

  TemotiSelectCheckFunc();

  switch(mInfoRemove)
  {
  case 1:
    mpInfoWindowBookType->RemoveFromSuperView();
    mInfoRemove++;
    break;
  case 2:
    if(mpInfoWindowBookType->EndFunc())
    {
      GFL_SAFE_DELETE(mpInfoWindowBookType);
      mInfoRemove=0;
    }
  }

  if(BOX_MOUSE_MODE_TUKAMI==mMousePokemonMode){
    SetCatchPokeIconTranslate();
  }
  else{
    if(!mbPageChangeAnim){
      ReloadCursorPos();
    }
  }


  DrawPokeFunc(mpAppParam->nowDispTray , mpAppParam->nowBattleTray);

}

//--------------------------------------------------------------------------
/**
 * @brief   描画
 */
//--------------------------------------------------------------------------
void BoxLowBgDraw::Draw(gfl2::gfx::CtrDisplayNo no)
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();


  pG2DUtil->AddDrawableLytWkForOneFrame( mpAppRenderingManager, no, LYT_WORK_BG );
  pG2DUtil->AddDrawableLytWkForOneFrame( mpAppRenderingManager, no, LYT_WORK_NORMAL );

}


//--------------------------------------------------------------------------
/**
 * @brief   破棄チェック
 * @return  trueで破棄完了
 */
//--------------------------------------------------------------------------
bool BoxLowBgDraw::IsExit( void )
{
  return true;
}














// カーソルモード
int BoxLowBgDraw::TouchPokemonHit(u16 x, u16 y)
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  gfl2::lyt::LytMultiResID res_id = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );
  gfl2::lyt::LytWk * lytwk = pG2DUtil->GetLayoutWork( LYT_WORK_NORMAL );

  for( u32 i=0; i< 36; i++ )
  {
    gfl2::lyt::LytParts* pParts = GetIconParts(i);
    gfl2::lyt::LytPane * pane = lytwk->GetPane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_BOUND_00, &res_id );
    if( pG2DUtil->GetHitPane( LYT_WORK_NORMAL, pane, x, y, false ) != false )
    {
      return i;
    }
  }
  return -1;
}

// カーソルモード
int BoxLowBgDraw::TouchLRButtonHit(u16 x, u16 y)
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  gfl2::lyt::LytMultiResID res_id = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );
  gfl2::lyt::LytWk * pLytWk = pG2DUtil->GetLayoutWork( LYT_WORK_NORMAL );

  int paneNo[][2]={
    { PANENAME_BOX_MIN_LOW_000_PANE_CURSOR_R1,TOUCH_PAIN_R1},
    { PANENAME_BOX_MIN_LOW_000_PANE_CURSOR_L1,TOUCH_PAIN_L1},
    { PANENAME_BOX_MIN_LOW_000_PANE_CURSOR_R2,TOUCH_PAIN_R2},
    { PANENAME_BOX_MIN_LOW_000_PANE_CURSOR_L2,TOUCH_PAIN_L2},
    { PANENAME_BOX_MIN_LOW_000_PANE_BTN_MODE, TOUCH_PAIN_FOLDER},
  };
  for(int i=0;i<elementof(paneNo);i++){
    if( pG2DUtil->GetHitPane( LYT_WORK_NORMAL, pLytWk->GetPartsPane( paneNo[i][0] ), x, y, false ) != false )
    {
      return paneNo[i][1];
    }
  }
  return TOUCH_PAIN_NONE;
}


void BoxLowBgDraw::TouchLRButtonHit2(u16 x, u16 y)
{
  int LRHit = TouchLRButtonHit(x,y);
  if(LRHit){
    int action[]={
      BTN_ID_L1,
      BTN_ID_L2,
      BTN_ID_R1,
      BTN_ID_R2,
      BTN_ID_BOXLIST,
    };
    if(LRHit != TOUCH_PAIN_FOLDER){
      if(_delayTimer==0){
        if(GetButtonManager()->IsButtonActive(action[LRHit-1])){
          GetButtonManager()->StartSelectedAct(action[LRHit-1]);
          _delayTimer = 16;
        }
      }
    }
    else if(_bFookFolderButton==false){
      if(GetButtonManager()->IsButtonActive(action[LRHit-1])){
        GetButtonManager()->StartSelectedAct(action[LRHit-1]);
        OHNO_PRINT("変更フォルダーモード\n");
        mMousePokemonMode = BOX_MOUSE_MODE_FOLDER;
        _bFookFolderButton = true;
      }
    }
  }
  else{
    _bFookFolderButton = false;
  }
}


void BoxLowBgDraw::SetTukamiMode(void)
{
  OHNO_PRINT("つかみに変更\n");
  _bFookFolderButton = true;
  mMousePokemonMode = BOX_MOUSE_MODE_TUKAMI;

}




void BoxLowBgDraw::PokemonTouchEventFunc(gfl2::ui::TouchPanel* pTouchPanel, bool isTouch,u16 x,u16 y)
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
//  gfl2::lyt::LytWk * lytwk = pG2DUtil->GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytMultiResID res_id = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );


  int no = TouchPokemonHit(mMouseX, mMouseY);
  if(_delayTimer!=0){
    _delayTimer--;
  }

  if(m_uiListener==NULL){
    return;
  }

  if(no != -1)
  {
    _bFookFolderButton=false;
    switch(mMousePokemonMode)
    {
    case BOX_MOUSE_MODE_NONE:
      if( pTouchPanel->IsTouchTrigger() )  //タッチとリガ
      {
        App::Box::PokemonIconDraw* pPkd = mpBoxAppSetup->GetBoxSimpleAccesser()->GetPokemonIconDrawData( mpAppParam->nowDispTray ,mpAppParam->nowBattleTray , no);
        if(pPkd->simp.monsNo == MONSNO_NULL ){
          if(!m_uiListener->OnMousePokemonCurpos(no)){//単数選択
            mMousePokemonMode = BOX_MOUSE_MODE_NO_TOUCH;
            OHNO_PRINT("いないところをタッチ開始\n");
          }
          else{//複数選択
            OHNO_PRINT("掴みに変更2\n");

            mMousePokemonX = x;  //初期位置キープ開始
            mMousePokemonY = y;
            mMousePokemonNo = no;
            mMousePokemonMode = BOX_MOUSE_MODE_TOUCH;
            PassiveMainCursor();
            m_uiListener->OnMousePokemonFirstTouch(no);
            m_uiListener->OnMousePokemonHand(x, y, mMousePokemonNo);
            mMousePokemonMode = BOX_MOUSE_MODE_TUKAMI;
          }
        }
        else{
          mMousePokemonX = x;  //初期位置キープ開始
          mMousePokemonY = y;
          mMousePokemonNo = no;
          OHNO_PRINT("初期位置キープ開始,%d,%d no = %d\n", x, y, no);
          mMousePokemonMode = BOX_MOUSE_MODE_TOUCH;
          PassiveMainCursor();
          m_uiListener->OnMousePokemonCurpos(no);
          m_uiListener->OnMousePokemonFirstTouch(no);
        }
      }
   //   else{
   //     mMousePokemonMode = BOX_MOUSE_MODE_NO_TOUCH;
    //  }
      break;
    case BOX_MOUSE_MODE_TOUCH:
      if( mpAppParam->type == BOX_CALL_TRADE || mpAppParam->type == BOX_CALL_FIELD_TRADE || mpAppParam->type == BOX_CALL_GTS || mpAppParam->type == BOX_CALL_MIRACLE){
        //このモードにはつかみが無い
      }
      else if(x==0 && y==0){
        //０，０値は比較しない
      }
      else if((mMousePokemonX + DIFFER_NUM < x) || (mMousePokemonX - DIFFER_NUM > x) || (mMousePokemonY + DIFFER_NUM < y) || (mMousePokemonY - DIFFER_NUM > y)){
        OHNO_PRINT("掴みに変更 %d %d == %d %d\n",mMousePokemonX,mMousePokemonY, x,y);
        m_uiListener->OnMousePokemonHand(x, y, mMousePokemonNo);
        mMousePokemonMode = BOX_MOUSE_MODE_TUKAMI;
      }
      break;
    case BOX_MOUSE_MODE_TUKAMI:
      m_uiListener->OnMouseDragCurpos(no);
      break;
    }
  }
  else{
    if(mMousePokemonMode == BOX_MOUSE_MODE_TUKAMI){
      TouchLRButtonHit2(x,y);  //掴んでる時のややこしい遷移処理
    }
  }
  if(!isTouch){
    switch(mMousePokemonMode){
    case BOX_MOUSE_MODE_TUKAMI: //ポケモンを握っていた
      if(!m_uiListener->OnMousePokemonPut(mMousePokemonNo, no)){
        ResetMainCursor();
      }
      OHNO_PRINT("適切に配置,%d,%d  %d %d\n", x, y, no,mMousePokemonNo);
      break;
    case BOX_MOUSE_MODE_TOUCH:
      m_uiListener->OnMouseMenuOpen(mMousePokemonNo);
      OHNO_PRINT("メニュー\n");
      break;
    case BOX_MOUSE_MODE_NO_TOUCH://いないところタッチリリース
      m_uiListener->OnNoTouchRelease(no);
      OHNO_PRINT("いないところタッチリリース\n");
      break;
    default:
      // @fix niji GFNMCat[644]: チームトレイが動作中はカーソル位置を動かさないようにした
      if( IsTeamFrameMove() == false )
      {
        if(m_uiListener->IsReleaseControl()){
          ResetMainCursor();
          OHNO_PRINT("カーソルを出す\n");
        }
      }
      break;
    }
    m_uiDrawListener->OnMouseModeEnd();
  }
}


void BoxLowBgDraw::MouseModeEnd(void)
{
  OHNO_PRINT("つかみ終了\n");
  mMousePokemonMode=BOX_MOUSE_MODE_NONE;
  mMousePokemonNo=-1;
  _bFookFolderButton=false;
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
::app::ui::UIInputListener::ListenerResult BoxLowBgDraw::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  u16 x,y;
  if( GetButtonManager()->GetUpdateState() != app::tool::ButtonManager::UPDATE_STATE_IDLE ){
    return ENABLE_ACCESS;
  }

  pTouchPanel->GetXY(&x,&y);


  if(m_uiDrawListener==NULL){
    return DISABLE_ACCESS;   //GFNMCat:1962
  }
  if(m_uiListener==NULL){
    return DISABLE_ACCESS;
  }
  if( m_uiListener->OnTouchFirstHook(isTouch) ){
    //    OHNO_PRINT("OnTouchFirstHook\n");
    return DISABLE_ACCESS;
  }
  //  OHNO_PRINT("OnTouchFirstHook  none\n");

  if(isTouch && (x!=0 && y!=0)){
    mMouseX = x;  //アイコン表示位置に使用
    mMouseY = y;
  }

  if( mbPokemonTouchOn ){
    PokemonTouchEventFunc(pTouchPanel,isTouch,x,y);
  }

  if(!mbTouchPassive && !mbTouchPassiveF)  //上のボタンタッチが有効かどうか
  {
    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    gfl2::lyt::LytWk * lytwk = pG2DUtil->GetLayoutWork( LYT_WORK_NORMAL );
    gfl2::lyt::LytMultiResID res_id = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );
    if( !pTouchPanel->IsTouchTrigger() ){  //タッチとリガで無ければ終了
      return DISABLE_ACCESS;
    }


    // カーソルモード
    {
      if(TeamTextBoxFunc(x,y)){
        return DISABLE_ACCESS;
      }
    }
    //移動モード切替
    {
      int PaneTable2[]={
        PANENAME_BOX_BTN_LOW_014_PANE_BTN_OBJECTTYPE_00,
        PANENAME_BOX_BTN_LOW_014_PANE_BTN_OBJECTTYPE_01,
        PANENAME_BOX_BTN_LOW_014_PANE_BTN_OBJECTTYPE_02,
      };
      gfl2::lyt::LytParts* pParts = lytwk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_OBJECTTYPEBUTTON );
      if(pParts->IsVisible()){  //表示されてる時のみ

        for( u32 i=0; i<GFL_NELEMS(PaneTable2); i++ )
        {
          gfl2::lyt::LytParts* pParts2 = lytwk->GetPartsPane(pParts,PaneTable2[i],&res_id);

          gfl2::lyt::LytPane * pane = lytwk->GetPane( pParts2, PANENAME_BOX_BTN_LOW_008_PANE_BOUND_00, &res_id );
          if( m_g2dUtil->GetHitPane( LYT_WORK_NORMAL, pane, x, y, false ) != false )
          {
            m_uiListener->OnButtonAction(BoxLowBgDrawListener::LOWBG_ACTION_MODE_CHANGE0+i);  //終了
            return DISABLE_ACCESS;
          }
        }
      }
    }
  }


  if( m_uiListener == NULL ){
    return DISABLE_ACCESS;
  }

  // 決定
  if( pTouchPanel->IsTouchTrigger() ){
    return DISABLE_ACCESS;
  }
  return DISABLE_ACCESS;
}


bool BoxLowBgDraw::TeamTextBoxFunc( u16 x, u16 y )
{
	app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytParts* pParts = pLytWk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_TEAM_00 );
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );
  gfl2::lyt::LytTextBox * pPane = pLytWk->GetTextBoxPane( pParts, PANENAME_BOX_PLT_LOW_000_PANE_TEXTBOX_00, &resID );
 
  if( m_g2dUtil->GetHitPane( LYT_WORK_NORMAL, pPane, x, y, false ) != false )
  {
    m_uiListener->OnButtonAction(BoxLowBgDrawListener::LOWBG_ACTION_TEAM_NAME);
    return true;
  }
  return false;
}


//トレイアニメの終了検査

void BoxLowBgDraw::AnimRevCheck( void )
{
 // app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );

  if(mbPageChangeAnim){
    if(pLytWk->IsBindAnime(LA_MINLOW_BOX_MIN_LOW_000_POKEICON_SET_01)){
      if(pLytWk->IsAnimeEnd(LA_MINLOW_BOX_MIN_LOW_000_POKEICON_SET_01)){
        pLytWk->SetAnimeFrame(LA_MINLOW_BOX_MIN_LOW_000_POKEICON_SET_01, 0);
        if(m_uiDrawListener)
        {
          m_uiDrawListener->OnPageScrollEnd(BoxLowBgDrawListener::SCROLL_PAGE_LEFT);
        }
        mbPageChangeAnim=false;
        mPageChangeSpeed=0;
      }
    }
    if(pLytWk->IsBindAnime(LA_MINLOW_BOX_MIN_LOW_000_POKEICON_SET_00)){
      if(pLytWk->IsAnimeEnd(LA_MINLOW_BOX_MIN_LOW_000_POKEICON_SET_00)){
        pLytWk->SetAnimeFrame(LA_MINLOW_BOX_MIN_LOW_000_POKEICON_SET_00, 0);
        if(m_uiDrawListener)
        {
          m_uiDrawListener->OnPageScrollEnd(BoxLowBgDrawListener::SCROLL_PAGE_RIGHT);
        }
        mbPageChangeAnim=false;
        mPageChangeSpeed=0;
      }
    }
  }


  if(mbTeamChangeAnim){
    if(pLytWk->IsBindAnime(LA_MINLOW_BOX_MIN_LOW_000_TEAM_SET_01)){
      if(pLytWk->IsAnimeEnd(LA_MINLOW_BOX_MIN_LOW_000_TEAM_SET_01)){
        pLytWk->SetAnimeFrame(LA_MINLOW_BOX_MIN_LOW_000_TEAM_SET_01, 0);
        app::ui::UIView::Update();

        pLytWk->UnBindAnime( LA_MINLOW_BOX_MIN_LOW_000_TEAM_SET_01 );

        if(m_uiDrawListener)
        {
          m_uiDrawListener->OnPageScrollEnd(BoxLowBgDrawListener::SCROLL_TEAM_RIGHT);
        }
        mbTeamChangeAnim=false;
        mTeamChangeSpeed=0;
      }
    }
    if(pLytWk->IsBindAnime(LA_MINLOW_BOX_MIN_LOW_000_TEAM_SET_00)){
      if(pLytWk->IsAnimeEnd(LA_MINLOW_BOX_MIN_LOW_000_TEAM_SET_00)){
        pLytWk->SetAnimeFrame(LA_MINLOW_BOX_MIN_LOW_000_TEAM_SET_00, 0);
        app::ui::UIView::Update();

        pLytWk->UnBindAnime( LA_MINLOW_BOX_MIN_LOW_000_TEAM_SET_00 );

        if(m_uiDrawListener)
        {
          m_uiDrawListener->OnPageScrollEnd(BoxLowBgDrawListener::SCROLL_TEAM_LEFT);
        }
        mbTeamChangeAnim=false;
        mTeamChangeSpeed=0;
      }
    }
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
::app::ui::UIInputListener::ListenerResult BoxLowBgDraw::OnLayoutPaneEvent( const u32 painId )
{
  OHNO_PRINT( "OnLayoutPaneEvent %d \n",painId);
//  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();


  //  if(mDownCount!=0){
  // /   return ENABLE_ACCESS;
  //  }
  switch(painId){
  case BTN_ID_CANCEL:
    if(m_uiListener)
    {
      m_uiListener->OnButtonAction(BoxLowBgDrawListener::LOWBG_ACTION_CANCEL);  //終了
    }
    return DISABLE_ACCESS;
  case BTN_ID_X:
    if(m_uiListener)
    {
      if(!mbTouchPassive){
        m_uiListener->OnButtonAction(BoxLowBgDrawListener::LOWBG_ACTION_MODE_CHANGE);
      }
    }
    return DISABLE_ACCESS;
  case BTN_ID_Y:
    if(m_uiListener)
    {
      m_uiListener->OnButtonAction(BoxLowBgDrawListener::LOWBG_ACTION_FIND);
    }
    return DISABLE_ACCESS;
  case BTN_ID_INFO:
    if(m_uiListener)
    {
      m_uiListener->OnButtonAction(BoxLowBgDrawListener::LOWBG_ACTION_INFO);
    }
    return DISABLE_ACCESS;
  case BTN_ID_BOXLIST:
    if(m_uiListener)
    {
      m_uiListener->OnButtonAction(BoxLowBgDrawListener::LOWBG_ACTION_FOLDER);
    }
    return DISABLE_ACCESS;
  case BTN_ID_CURTYPE_NORMAL:
    {
      Sound::PlaySE(SEQ_SE_DECIDE1);//GFNMCat1484
      PushButtonType(0);
      return DISABLE_ACCESS;
    }
  case BTN_ID_CURTYPE_SHORTCUT:
    {
      Sound::PlaySE(SEQ_SE_DECIDE1);//GFNMCat1484
      PushButtonType(1);
      return DISABLE_ACCESS;
    }
  case BTN_ID_CURTYPE_RANGE:
    {
      Sound::PlaySE(SEQ_SE_DECIDE1);//GFNMCat1484
      PushButtonType(2);
      return DISABLE_ACCESS;
    }
  case BTN_ID_R1:
    if(m_uiListener)
    {
      m_uiListener->OnButtonAction(BoxLowBgDrawListener::LOWBG_ACTION_R_CHANGE);
    }
    break;
  case BTN_ID_L1:
    if(m_uiListener)
    {
      m_uiListener->OnButtonAction(BoxLowBgDrawListener::LOWBG_ACTION_L_CHANGE);
    }
    break;
  case BTN_ID_R2:
    if(m_uiListener)
    {
      m_uiListener->OnButtonAction(BoxLowBgDrawListener::LOWBG_ACTION_R_TEAM);
    }
    return ENABLE_ACCESS;
  case BTN_ID_L2:
    if(m_uiListener)
    {
      m_uiListener->OnButtonAction(BoxLowBgDrawListener::LOWBG_ACTION_L_TEAM);
    }
    return ENABLE_ACCESS;
  case BTN_ID_JUDGE:
    if(m_uiListener)
    {
      m_uiListener->OnButtonAction(BoxLowBgDrawListener::LOWBG_ACTION_JUDGE);
    }
    return ENABLE_ACCESS;
  case BTN_ID_CHAT_CHANGE:
    if(m_uiListener)
    {
      m_uiListener->OnButtonAction(BoxLowBgDrawListener::LOWBG_ACTION_CHAT_CHANGE);
    }
    return ENABLE_ACCESS;
  case BTN_ID_CHAT:
    if(m_uiListener)
    {
      m_uiListener->OnButtonAction(BoxLowBgDrawListener::LOWBG_ACTION_CHAT);
    }
    return ENABLE_ACCESS;
  case BTN_ID_BOXBAR:
    if(m_uiListener)
    {
      m_uiListener->OnButtonAction(BoxLowBgDrawListener::LOWBG_ACTION_BOXBAR);
    }
    return ENABLE_ACCESS;

  case BoxLowBgDrawListener::LOWBG_FOLDER_ACTION_MARK0:
  case BoxLowBgDrawListener::LOWBG_FOLDER_ACTION_MARK1:
  case BoxLowBgDrawListener::LOWBG_FOLDER_ACTION_MARK2:
  case BoxLowBgDrawListener::LOWBG_FOLDER_ACTION_MARK3:
  case BoxLowBgDrawListener::LOWBG_FOLDER_ACTION_MARK4:
  case BoxLowBgDrawListener::LOWBG_FOLDER_ACTION_MARK5:
  case BoxLowBgDrawListener::LOWBG_FOLDER_ACTION_OK:
  case BoxLowBgDrawListener::LOWBG_FOLDER_ACTION_CANCEL:
    if(m_uiListener)
    {
      m_uiListener->OnButtonAction(painId);
    }
    return ENABLE_ACCESS;


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
::app::ui::UIInputListener::ListenerResult BoxLowBgDraw::OnKeyAction(  gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  if( m_uiListener == NULL ){
    return ENABLE_ACCESS;
  }


  
  if(pButton->IsRepeat(gfl2::ui::BUTTON_UP)){
    m_uiListener->OnButtonAction(BoxLowBgDrawListener::LOWBG_ACTION_UP);
    return ENABLE_ACCESS;
  }
  else if(pButton->IsRepeat(gfl2::ui::BUTTON_DOWN)){
    m_uiListener->OnButtonAction(BoxLowBgDrawListener::LOWBG_ACTION_DOWN);
    return ENABLE_ACCESS;
  }
  else if(pButton->IsRepeat(gfl2::ui::BUTTON_RIGHT)){
    m_uiListener->OnButtonAction(BoxLowBgDrawListener::LOWBG_ACTION_RIGHT);
    return ENABLE_ACCESS;
  }
  else if(pButton->IsRepeat(gfl2::ui::BUTTON_LEFT)){
    m_uiListener->OnButtonAction(BoxLowBgDrawListener::LOWBG_ACTION_LEFT);
    return ENABLE_ACCESS;
  }
  else if(pButton->IsRepeat(gfl2::ui::BUTTON_L)){
    //    m_uiListener->OnButtonAction(BoxLowBgDrawListener::LOWBG_ACTION_L_CHANGE);
    //   return ENABLE_ACCESS;
  }
  else if(pButton->IsRepeat(gfl2::ui::BUTTON_R)){
    //    m_uiListener->OnButtonAction(BoxLowBgDrawListener::LOWBG_ACTION_R_CHANGE);
    //    return ENABLE_ACCESS;
  }
  if( pButton->IsTrigger(gfl2::ui::BUTTON_B)  ){
    if(m_uiListener->PreButtonAction(BoxLowBgDrawListener::LOWBG_ACTION_CANCEL))
    {
      return DISABLE_ACCESS;
    }
    else
    {
      GetButtonManager()->StartSelectedAct(BTN_ID_CANCEL);
    }
    return DISABLE_ACCESS;
  }
  if( pButton->IsTrigger(gfl2::ui::BUTTON_X) ){
    if(!mbTouchPassive){

      if(mpAppParam->type == BOX_CALL_TRADE){
        GetButtonManager()->StartSelectedAct(BTN_ID_CHAT_CHANGE);
      }
      else if(mpAppParam->type == BOX_CALL_FIELD_TRADE){
      }
      else{
        m_uiListener->OnButtonAction(BoxLowBgDrawListener::LOWBG_ACTION_MODE_CHANGE);
      }
    }
    return ENABLE_ACCESS;
  }
  if( pButton->IsTrigger(gfl2::ui::BUTTON_START) ){
    if(!mbTouchPassive){
      if( mpAppParam->type == BOX_CALL_GTS || mpAppParam->type == BOX_CALL_MIRACLE || mpAppParam->type == BOX_CALL_TRADE || mpAppParam->type == BOX_CALL_FIELD_TRADE ){
      }
      else{
        m_uiListener->OnButtonAction(BoxLowBgDrawListener::LOWBG_ACTION_SELECT_CHANGE);
      }
    }
    return ENABLE_ACCESS;
  }
  // 決定
  if( pButton->IsTrigger(gfl2::ui::BUTTON_A) ){
    
    if(m_uiListener->PreButtonAction(BoxLowBgDrawListener::LOWBG_ACTION_DECIDE))
    {
      return DISABLE_ACCESS;
    }
    else
    {

      if(mbMarkWin){
        GetButtonManager()->StartSelectedAct(BoxLowBgDrawListener::LOWBG_FOLDER_ACTION_MARK0+mBkMarkButtonPos);
        return DISABLE_ACCESS;
      }
      
      OHNO_PRINT("ボタン番号 %d\n",mButtonPos);
      switch(mButtonPos){
      case POKEBUTTON_BOX:
        GetButtonManager()->StartSelectedAct(BTN_ID_BOXBAR);
        return ENABLE_ACCESS;
      case POKEBUTTON_TEMO:
        m_uiListener->OnButtonAction(BoxLowBgDrawListener::LOWBG_ACTION_TEAM_NAME);
        break;
      case POKEBUTTON_FOLDER:
        GetButtonManager()->StartSelectedAct(BTN_ID_BOXLIST);
        return ENABLE_ACCESS;
      case POKETEMO_POS01:
      case POKETEMO_POS02:
      case POKETEMO_POS03:
      case POKETEMO_POS04:
      case POKETEMO_POS05:
      case POKETEMO_POS06:
        m_uiListener->OnButtonAction(BoxLowBgDrawListener::LOWBG_ACTION_DECIDE);
        break;
      default:
        m_uiListener->OnButtonAction(BoxLowBgDrawListener::LOWBG_ACTION_DECIDE);
        break;
      }
      return DISABLE_ACCESS;
    }
  }

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
void BoxLowBgDraw::Initialize2D( app::util::Heap * heap, void* pAppLytBuff )
{
  // アーカイブIDテーブル
  //  void* pData = mpBoxAppSetup->GetArcReadBuff(GARC_boxLyt_bgrlow_applyt_COMP);
  void* pData = mpBoxAppSetup->GetBinLinkReadBuff(BL_IDX_BOX_PACK_BGRLOW2_APPLYT);

  app::util::G2DUtil::LytResData resTbl[] = {
    {  pAppLytBuff, 1,  app::util::G2DUtil::ATTACH_ROOT, },
    {  pData, 1,  app::util::G2DUtil::ATTACH_ROOT, },
  };

  app::util::G2DUtil::LytwkData setupData[] = {
    {//
      0,
      LYTRES_MINLOW_BOX_MIN_LOW_000_BFLYT,
      LA_MINLOW_BOX_MIN_LOW_000___NUM,
      pAppLytBuff,
      true,
      app::util::G2DUtil::SETUP_LOWER,
      true
      },
    {//
      1,
      LYTRES_BGRLOW_BOX_BGR_LOW_000_BFLYT,
      LA_BGRLOW_BOX_BGR_LOW_000___NUM,
      pData,
      true,
      app::util::G2DUtil::SETUP_LOWER,
      true
      },
  };


  Create2D( heap, NULL, 32,
            resTbl, GFL_NELEMS(resTbl),
            setupData, GFL_NELEMS(setupData),
            GARC_message_box_DAT );

  MyCreateButtonManager(false);


}


void BoxLowBgDraw::SetDispActive(bool bActive)
{
  //  GetG2DUtil()->StartAnime( 0, LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_COMMANDSELECT_IN );
}



int BoxLowBgDraw::SetCursorPos(int no,int udlr )
{
  gfl2::math::VEC3 vec3;
  int nextbutton = 0;
  int ver = 0;
  int hor = 0;

  GFL_ASSERT(no < POKE_PANEL_MAX);
  GFL_ASSERT(no >= 0); 
  if((no<0) || (no >= POKE_PANEL_MAX)){ ///NMCat[3851]
    return no;
  }

  if(udlr==-1){
    nextbutton=no;
  }
  else{
    nextbutton = buttonmove[no][udlr];  //次の位置
    ver = buttonmove[no][4];
    hor = buttonmove[no][5];

    if(ver!=-1){
      mKeepCursorPosVertical = ver;
    }
    if(hor!=-1){
      mKeepCursorPosHorizontal = hor;
    }

    if((udlr == 0) || (udlr == 1)){ //上下に動いた
      mKeepCursorPosVertical=-1;
    }

    
    switch(nextbutton){
    case POKE_PAGE_R:
      GetButtonManager()->StartSelectedAct(BTN_ID_R1);
      //      m_uiListener->OnButtonAction(BoxLowBgDrawListener::LOWBG_ACTION_R_CHANGE);
      return no;
    case POKE_PAGE_L:
      GetButtonManager()->StartSelectedAct(BTN_ID_L1);
      //      m_uiListener->OnButtonAction(BoxLowBgDrawListener::LOWBG_ACTION_L_CHANGE);
      return no;
    case POKE_PAGE_R_TEAM:
      GetButtonManager()->StartSelectedAct(BTN_ID_R2);
      //     m_uiListener->OnButtonAction(BoxLowBgDrawListener::LOWBG_ACTION_R_TEAM);
      return no;
    case POKE_PAGE_L_TEAM:
      GetButtonManager()->StartSelectedAct(BTN_ID_L2);
      //      m_uiListener->OnButtonAction(BoxLowBgDrawListener::LOWBG_ACTION_L_TEAM);
      return no;
    case POKEBOX_1_7:  //1-7のどれか
      switch(mKeepCursorPosVertical){
			default:
      case VERTICAL01:
        nextbutton=POKEBOX_POS01;
        break;
      case VERTICAL02:
        nextbutton=POKEBOX_POS07;
        break;
      }
      break;
    case POKEBOX_19_25:  //19-25のどれか
      switch(mKeepCursorPosVertical){
			default:
      case VERTICAL04:
        nextbutton=POKEBOX_POS19;
        break;
      case VERTICAL05:
        nextbutton=POKEBOX_POS25;
        break;
      }
      break;
    case POKEBOX_6_12:
      switch(mKeepCursorPosVertical){
			default:
      case VERTICAL01:
        nextbutton=POKEBOX_POS06;
        break;
      case VERTICAL02:
        nextbutton=POKEBOX_POS12;
        break;
      }
      break;
    case POKEBOX_24_30:
      switch(mKeepCursorPosVertical){
			default:
      case VERTICAL04:
        nextbutton=POKEBOX_POS24;
        break;
      case VERTICAL05:
        nextbutton=POKEBOX_POS30;
        break;
      }
      break;

    case POKEBOX_0_5:  //0から５のどれか
      switch(mKeepCursorPosHorizontal){
			default:
      case HORIZONTAL01:
        nextbutton=POKEBOX_POS01;
        break;
      case HORIZONTAL02:
        nextbutton=POKEBOX_POS02;
        break;
      case HORIZONTAL03:
        nextbutton=POKEBOX_POS03;
        break;
      case HORIZONTAL04:
        nextbutton=POKEBOX_POS04;
        break;
      case HORIZONTAL05:
        nextbutton=POKEBOX_POS05;
        break;
      case HORIZONTAL06:
        nextbutton=POKEBOX_POS06;
        break;
      }
      break;
    case POKETEMO_1_2:
      switch(mKeepCursorPosHorizontal){
			default:
      case HORIZONTAL07:
        nextbutton = POKETEMO_POS01;
        break;
      case HORIZONTAL08:
        nextbutton = POKETEMO_POS02;
        break;
      }
      break;
    case POKETEMO_5_6:
      switch(mKeepCursorPosHorizontal){
			default:
      case HORIZONTAL07:
        nextbutton = POKETEMO_POS05;
        break;
      case HORIZONTAL08:
        nextbutton = POKETEMO_POS06;
        break;
      }
      break;
    case POKEBOX_25_30:
      switch(mKeepCursorPosHorizontal){
			default:
      case HORIZONTAL01:
        nextbutton=POKEBOX_POS25;
        break;
      case HORIZONTAL02:
        nextbutton=POKEBOX_POS26;
        break;
      case HORIZONTAL03:
        nextbutton=POKEBOX_POS27;
        break;
      case HORIZONTAL04:
        nextbutton=POKEBOX_POS28;
        break;
      case HORIZONTAL05:
        nextbutton=POKEBOX_POS29;
        break;
      case HORIZONTAL06:
        nextbutton=POKEBOX_POS30;
        break;
      }
      break;
    }
  }

  vec3 = GetG2DUtil()->GetPanePos(0,  label_to_pane[nextbutton]);

  if(nextbutton<36 && mbStopCursor){
    vec3.y += 5.0f;
  }

  GetG2DUtil()->SetPanePos(0,  PANE_CURSOR, &vec3);

  if(udlr!=-1){
    if(mButtonPos != nextbutton){
      Sound::PlaySE(SEQ_SE_SELECT1);
    }
  }

  mButtonPos = nextbutton;

  LiftPokeIconMove( &vec3 );

  CarryRangeSelectMove(nextbutton ,&vec3);
  GFL_ASSERT(nextbutton>=0);
  return nextbutton;
}


void BoxLowBgDraw::ResetCursorPos(void)
{
  gfl2::math::VEC3 vec3;

  vec3 = GetG2DUtil()->GetPanePos(0,  label_to_pane[mButtonPos]);

  if(mButtonPos<36 && mbStopCursor){
    vec3.y += 5.0f;
  }

  GetG2DUtil()->SetPanePos(0,  PANE_CURSOR, &vec3);
}



//----------------------------------------------------------------------
/**
 *	@brief  初期テキスト表示
 */
//----------------------------------------------------------------------


void BoxLowBgDraw::InitializeText( app::util::Heap * pheap )
{


  mpTempStrBuf = GFL_NEW( pheap->GetDeviceHeap() ) gfl2::str::StrBuf( TEMP_STRBUF_LENGTH, pheap->GetDeviceHeap() );
  mpTempStrBuf1 = GFL_NEW( pheap->GetDeviceHeap() ) gfl2::str::StrBuf( TEMP_STRBUF_LENGTH, pheap->GetDeviceHeap() );
  mpTempStrBuf2 = GFL_NEW( pheap->GetDeviceHeap() ) gfl2::str::StrBuf( TEMP_STRBUF_LENGTH, pheap->GetDeviceHeap() );
  mpTempStrBuf3 = GFL_NEW( pheap->GetDeviceHeap() ) gfl2::str::StrBuf( TEMP_STRBUF_LENGTH, pheap->GetDeviceHeap() );
  mpTempStrBuf4 = GFL_NEW( pheap->GetDeviceHeap() ) gfl2::str::StrBuf( TEMP_STRBUF_LENGTH, pheap->GetDeviceHeap() );
  mpTempStrBuf5 = GFL_NEW( pheap->GetDeviceHeap() ) gfl2::str::StrBuf( TEMP_STRBUF_LENGTH, pheap->GetDeviceHeap() );
  mpTempStrBuf6 = GFL_NEW( pheap->GetDeviceHeap() ) gfl2::str::StrBuf( TEMP_STRBUF_LENGTH, pheap->GetDeviceHeap() );

  mpWordSet = GFL_NEW( pheap->GetDeviceHeap() ) print::WordSet( pheap->GetDeviceHeap() );

  {
    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
    gfl2::lyt::LytParts* pParts = pLytWk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_PLT_MESSAGE_L2 );
    gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );

    gfl2::ui::DeviceManager * man =  GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
    gfl2::lyt::LytTextBox * pPane1 = pLytWk->GetTextBoxPane( pParts, PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_1A, &resID );
    gfl2::lyt::LytTextBox * pPane2 = pLytWk->GetTextBoxPane( pParts, PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_1B, &resID );

    pG2DUtil->CreateMessageWindow( man, 0, pPane1,pPane2, true, pheap  );
  }

  DrawJudgeName();



}


void BoxLowBgDraw::FinalizeText(void)
{
  GFL_DELETE mpWordSet;
  GFL_DELETE mpTempStrBuf6;
  GFL_DELETE mpTempStrBuf5;
  GFL_DELETE mpTempStrBuf4;
  GFL_DELETE mpTempStrBuf3;
  GFL_DELETE mpTempStrBuf2;
  GFL_DELETE mpTempStrBuf1;
  GFL_DELETE mpTempStrBuf;
}

void BoxLowBgDraw::SetMessage( int msgid )
{
}




//------------------------------------------------------------------------------
/**
 * @brief   ボタンマネージャの生成
 */
//------------------------------------------------------------------------------
void BoxLowBgDraw::MyCreateButtonManager( bool breload )
{
  static const int partsnum = 15;
  static const int layoutno = LYT_WORK_NORMAL;

  app::ui::ButtonInfoEx c_DefInfo[partsnum] = {
    {
      BTN_ID_CANCEL , NULL, NULL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_BACK_TOUCH,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_BACK_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_BACK_CANSEL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_BACK_TOUCH_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_BACK_ACTIVE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_BACK_PASSIVE,
    },
    {
      BTN_ID_Y , NULL, NULL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_SEARCH_TOUCH,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_SEARCH_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_SEARCH_CANSEL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_SEARCH_TOUCH_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_SEARCH_ACTIVE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_SEARCH_PASSIVE,
    },
    {
      BTN_ID_BOXLIST, NULL, NULL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MODE_TOUCH,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MODE_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MODE_CANSEL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MODE_TOUCH_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MODE_ACTIVE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MODE_PASSIVE,
    },
    {
      BTN_ID_INFO, NULL, NULL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_INFO_TOUCH,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_INFO_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_INFO_CANSEL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_INFO_TOUCH_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_INFO_ACTIVE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_INFO_PASSIVE,
    },
    {
      BTN_ID_CURTYPE_NORMAL, NULL, NULL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MOVETYPE_00_TOUCH,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MOVETYPE_00_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MOVETYPE_00_CANSEL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MOVETYPE_00_TOUCH_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MOVETYPE_00_ACTIVE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MOVETYPE_00_PASSIVE,
    },
    {
      BTN_ID_CURTYPE_SHORTCUT, NULL, NULL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MOVETYPE_01_TOUCH,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MOVETYPE_01_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MOVETYPE_01_CANSEL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MOVETYPE_01_TOUCH_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MOVETYPE_01_ACTIVE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MOVETYPE_01_PASSIVE,
    },
    {
      BTN_ID_CURTYPE_RANGE, NULL, NULL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MOVETYPE_02_TOUCH,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MOVETYPE_02_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MOVETYPE_02_CANSEL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MOVETYPE_02_TOUCH_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MOVETYPE_02_ACTIVE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MOVETYPE_02_PASSIVE,
    },
    {
      BTN_ID_R1, NULL, NULL,
      LA_MINLOW_BOX_MIN_LOW_000__CURSOR_R1_TOUCH,
      LA_MINLOW_BOX_MIN_LOW_000__CURSOR_R1_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__CURSOR_R1_CANSEL,
      LA_MINLOW_BOX_MIN_LOW_000__CURSOR_R1_TOUCH_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__CURSOR_R1_ACTIVE,
      LA_MINLOW_BOX_MIN_LOW_000__CURSOR_R1_PASSIVE,
    },
    {
      BTN_ID_L1, NULL, NULL,
      LA_MINLOW_BOX_MIN_LOW_000__CURSOR_L1_TOUCH,
      LA_MINLOW_BOX_MIN_LOW_000__CURSOR_L1_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__CURSOR_L1_CANSEL,
      LA_MINLOW_BOX_MIN_LOW_000__CURSOR_L1_TOUCH_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__CURSOR_L1_ACTIVE,
      LA_MINLOW_BOX_MIN_LOW_000__CURSOR_L1_PASSIVE,
    },
    {
      BTN_ID_R2, NULL, NULL,
      LA_MINLOW_BOX_MIN_LOW_000__CURSOR_R2_TOUCH,
      LA_MINLOW_BOX_MIN_LOW_000__CURSOR_R2_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__CURSOR_R2_CANSEL,
      LA_MINLOW_BOX_MIN_LOW_000__CURSOR_R2_TOUCH_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__CURSOR_R2_ACTIVE,
      LA_MINLOW_BOX_MIN_LOW_000__CURSOR_R2_PASSIVE,
    },
    {
      BTN_ID_L2, NULL, NULL,
      LA_MINLOW_BOX_MIN_LOW_000__CURSOR_L2_TOUCH,
      LA_MINLOW_BOX_MIN_LOW_000__CURSOR_L2_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__CURSOR_L2_CANSEL,
      LA_MINLOW_BOX_MIN_LOW_000__CURSOR_L2_TOUCH_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__CURSOR_L2_ACTIVE,
      LA_MINLOW_BOX_MIN_LOW_000__CURSOR_L2_PASSIVE,
    },
    {
      BTN_ID_JUDGE, NULL, NULL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_JUDGE_TOUCH,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_JUDGE_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_JUDGE_CANSEL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_JUDGE_TOUCH_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_JUDGE_ACTIVE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_JUDGE_PASSIVE,
    },
    {
      BTN_ID_CHAT_CHANGE, NULL, NULL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_CHANGE_TOUCH,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_CHANGE_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_CHANGE_CANSEL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_CHANGE_TOUCH_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_CHANGE_ACTIVE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_CHANGE_PASSIVE,
    },
    {
      BTN_ID_CHAT, NULL, NULL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_CHAT_TOUCH,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_CHAT_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_CHAT_CANSEL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_CHAT_TOUCH_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_CHAT_ACTIVE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_CHAT_PASSIVE,
    },
    {
      BTN_ID_BOXBAR, NULL, NULL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_BOXCATEGORY_00_TOUCH,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_BOXCATEGORY_00_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_BOXCATEGORY_00_CANSEL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_BOXCATEGORY_00_TOUCH_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_BOXCATEGORY_00_ACTIVE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_BOXCATEGORY_00_PASSIVE,
    },
  };

  int painNo[]={
    PANENAME_BOX_MIN_LOW_000_PANE_BTN_BACK,
    PANENAME_BOX_MIN_LOW_000_PANE_BTN_SEARCH,
    PANENAME_BOX_MIN_LOW_000_PANE_BTN_MODE,
    PANENAME_BOX_MIN_LOW_000_PANE_BTN_INFO,
    PANENAME_BOX_MIN_LOW_000_PANE_BTN_MOVETYPE_00,
    PANENAME_BOX_MIN_LOW_000_PANE_BTN_MOVETYPE_01,
    PANENAME_BOX_MIN_LOW_000_PANE_BTN_MOVETYPE_02,
    PANENAME_BOX_MIN_LOW_000_PANE_CURSOR_R1,
    PANENAME_BOX_MIN_LOW_000_PANE_CURSOR_L1,
    PANENAME_BOX_MIN_LOW_000_PANE_CURSOR_R2,
    PANENAME_BOX_MIN_LOW_000_PANE_CURSOR_L2,
    PANENAME_BOX_MIN_LOW_000_PANE_BTN_JUDGE,
    PANENAME_BOX_MIN_LOW_000_PANE_BTN_CHANGE,
    PANENAME_BOX_MIN_LOW_000_PANE_BTN_CHAT,
    PANENAME_BOX_MIN_LOW_000_PANE_BTN_BOXCATEGORY_00,
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
  if(breload){
    this->ReloadButtonManager( mpAppHeap, pLytWk, info, partsnum );
  }
  else{
    this->CreateButtonManager( mpAppHeap, pLytWk, info, partsnum );
  }
  GFL_DELETE_ARRAY info;
  app::tool::ButtonManager* pBtnMan = this->GetButtonManager();

  pBtnMan->SetButtonBindKey( BTN_ID_CANCEL, gfl2::ui::BUTTON_B, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );
  pBtnMan->SetButtonSelectSE( BTN_ID_CANCEL, SEQ_SE_CANCEL1 );
  pBtnMan->SetButtonInputEnable(BTN_ID_CANCEL,true);

  pBtnMan->SetButtonBindKey( BTN_ID_Y, gfl2::ui::BUTTON_Y, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );
  pBtnMan->SetButtonBindKey( BTN_ID_L1, gfl2::ui::BUTTON_L, app::tool::ButtonManager::BIND_KEY_MODE_REPEAT );
  pBtnMan->SetButtonBindKey( BTN_ID_R1, gfl2::ui::BUTTON_R, app::tool::ButtonManager::BIND_KEY_MODE_REPEAT );

  pBtnMan->SetButtonSelectSE( BTN_ID_L1, SEQ_SE_PAGE2 );
  pBtnMan->SetButtonSelectSE( BTN_ID_R1, SEQ_SE_PAGE2 );
  pBtnMan->SetButtonSelectSE( BTN_ID_Y, SEQ_SE_DECIDE1 );

  pBtnMan->SetButtonSelectSE( BTN_ID_BOXBAR, SEQ_SE_DECIDE1 );
  pBtnMan->SetButtonSelectSE( BTN_ID_BOXLIST, SEQ_SE_DECIDE1 );


  if(mpAppParam->trade.bOn != 0){
    pBtnMan->SetButtonPassive( BTN_ID_Y );
  }

  if(mpAppParam->bBoxJudge==false){
    GetG2DUtil()->SetPaneVisible(0,  PANENAME_BOX_MIN_LOW_000_PANE_BTN_JUDGE,false);  //ジャッジがＯＮの時のみ表示
  }

  GetG2DUtil()->SetPaneVisible(0,  PANENAME_BOX_MIN_LOW_000_PANE_CURSOR_L2,true);
  GetG2DUtil()->SetPaneVisible(0,  PANENAME_BOX_MIN_LOW_000_PANE_CURSOR_R2,true);

  pBtnMan->SetButtonSelectSE( BTN_ID_L2, SEQ_SE_PAGE2 );
  pBtnMan->SetButtonSelectSE( BTN_ID_R2, SEQ_SE_PAGE2 );

  if((mpBoxAppSetup->GetSelectMode() == BOX_CHANGE_EDITTYPE_BATTLE_TEAM)){  //バトルチーム選択以外では、名前左右切り替えは機能しない
    pBtnMan->SetButtonVisible(BTN_ID_L2);
    pBtnMan->SetButtonVisible(BTN_ID_R2);
  }
  else{
    pBtnMan->SetButtonInvisible(BTN_ID_L2);
    pBtnMan->SetButtonInvisible(BTN_ID_R2);
  }

  if( mpAppParam->type == BOX_CALL_GTS || mpAppParam->type == BOX_CALL_MIRACLE || mpAppParam->type == BOX_CALL_TRADE || mpAppParam->type == BOX_CALL_FIELD_TRADE ){
    GetG2DUtil()->SetPaneVisible(0,  PANENAME_BOX_MIN_LOW_000_PANE_BTN_MOVETYPE_00,false);
    GetG2DUtil()->SetPaneVisible(0,  PANENAME_BOX_MIN_LOW_000_PANE_BTN_MOVETYPE_01,false);
    GetG2DUtil()->SetPaneVisible(0,  PANENAME_BOX_MIN_LOW_000_PANE_BTN_MOVETYPE_02,false);
    GetG2DUtil()->SetPaneVisible(0,  PANENAME_BOX_MIN_LOW_000_PANE_NULL_TYPESELECT,false);
  }



  if(mpAppParam->type == BOX_CALL_TRADE){
    pBtnMan->SetButtonBindKey( BTN_ID_CHAT_CHANGE, gfl2::ui::BUTTON_X, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );
    GetG2DUtil()->SetPaneVisible(0,  PANENAME_BOX_MIN_LOW_000_PANE_NULL_TYPESELECT,false);
    GetG2DUtil()->SetPaneVisible(0,  PANENAME_BOX_MIN_LOW_000_PANE_BTN_CHANGE,true);
    GetG2DUtil()->SetPaneVisible(0,  PANENAME_BOX_MIN_LOW_000_PANE_BTN_CHAT,true);
  }
  else if(mpAppParam->type == BOX_CALL_RESORT_HOTSPA){
    GetG2DUtil()->SetPaneVisible(0,  PANENAME_BOX_MIN_LOW_000_PANE_OBJECTTYPEBUTTON,false);
  }
  else if(mpAppParam->type == BOX_CALL_RESORT_GIM){
    GetG2DUtil()->SetPaneVisible(0,  PANENAME_BOX_MIN_LOW_000_PANE_OBJECTTYPEBUTTON,false);
  }
  else if(mpAppParam->type == BOX_CALL_FIELD_TRADE){
    GetG2DUtil()->SetPaneVisible(0,  PANENAME_BOX_MIN_LOW_000_PANE_NULL_TYPESELECT,false);
    GetG2DUtil()->SetPaneVisible(0,  PANENAME_BOX_MIN_LOW_000_PANE_BTN_INFO,false);
  }
  else if(BOX_CALL_BATTLE_TEAM != mpAppParam->type){  //バトルチームモードの時はバトルチーム
    pBtnMan->SetButtonBindKey( BTN_ID_X, gfl2::ui::BUTTON_X, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );

  }
  
  if(mpAppParam->type != BOX_CALL_PC){  //PCでなければinfo出さない NMCat[354]
    GetG2DUtil()->SetPaneVisible(0,  PANENAME_BOX_MIN_LOW_000_PANE_BTN_INFO,false);
  }
  
  if(mbTouchPassiveF){
    pBtnMan->SetButtonInputEnable(BTN_ID_CURTYPE_NORMAL,false);
    pBtnMan->SetButtonInputEnable(BTN_ID_CURTYPE_SHORTCUT,false);
    pBtnMan->SetButtonInputEnable(BTN_ID_CURTYPE_RANGE,false);
  }


  
}


bool BoxLowBgDraw::IsEditTypeVisible(void)
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = pG2DUtil->GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytParts* pParts = lytwk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_OBJECTTYPEBUTTON );
  return pParts->IsVisible();
}




//アイコンのテクスチャー以外の状態反映
void BoxLowBgDraw::DrawPokeIconState(int pos, App::Box::PokemonIconDraw* pPkd)
{
  gfl2::lyt::LytParts* pParts = GetIconParts(pos);

  if(!pPkd->bHide)
  {
    PokeIconVisible(pos,true);
  }
  else{
    PokeIconVisible(pos,false);
  }

  SetPokeIconColorAndAlpha( pParts, pPkd );   // マテリアルカラーとアルファの変更

  PokeIconTeam(pos,false);
  PokeIconLock(pos,false);

  //  if(BOX_CHANGE_EDITTYPE_BATTLE_TEAM == mpBoxAppSetup->GetSelectMode()){  //バトルチームモードの時はバトルチーム
  if(!pPkd->bHide)  //表示ＯＫのとき
  {
    if(pPkd->bLock){
      PokeIconLock(pos,true);
    }
    else if( mpBoxAppSetup->GetBoxSimpleAccesser()->IsTeamIcon( pPkd )){
      PokeIconTeam(pos,true);
    }
  }
}

void BoxLowBgDraw::SetPokeIconColorAndAlpha( gfl2::lyt::LytParts* pParts, App::Box::PokemonIconDraw* pPkd )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );
  gfl2::lyt::LytPicture * pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ICONPARTS_POKE, &resID );
  gfl2::lyt::LytPicture * pPaneM = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_MATERIAL_ICONPARTS_POKE_OF_ICONPARTS_POKE, &resID );

  {
    nw::ut::Color8 b = pG2DUtil->GetMaterialColor( pPaneM, app::util::G2DUtil::COLOR_INDEX_BLACK );
    if(pPkd->bwhite)
    {
      b.r=128;
      b.g=128;
      b.b=128;
      pG2DUtil->SetMaterialColor( pPaneM, app::util::G2DUtil::COLOR_INDEX_BLACK,b );
    }
    else
    {
      b.r=0;
      b.g=0;
      b.b=0;
      pG2DUtil->SetMaterialColor( pPaneM, app::util::G2DUtil::COLOR_INDEX_BLACK,b );
    }
  }

  if(!pPkd->bHide)
  {
    nw::ut::Color8 b = pG2DUtil->GetMaterialColor( pPaneM, app::util::G2DUtil::COLOR_INDEX_WHITE );
    if(pPkd->bblack && !pPkd->bwhite)
    {
      b.r=64;
      b.g=64;
      b.b=64;
      pG2DUtil->SetMaterialColor( pPaneM, app::util::G2DUtil::COLOR_INDEX_WHITE,b );
    }
    else
    {
      b.r=255;
      b.g=255;
      b.b=255;
      pG2DUtil->SetMaterialColor( pPaneM, app::util::G2DUtil::COLOR_INDEX_WHITE,b );
    }
  }

  if(BOX_CHANGE_EDITTYPE_ITEM == mpBoxAppSetup->GetSelectMode()){  //アイテム部分

    if(pPkd->bItem){
      pPane->SetAlpha(255);
    }
    else{
      pPane->SetAlpha(App::Box::BoxSimpleAccesser::POKEICON_PASSIVE_NUM);
    }
  }
  else{
    if(pPkd->bPassive){
      pPane->SetAlpha(App::Box::BoxSimpleAccesser::POKEICON_PASSIVE_NUM);
    }
    else{
      pPane->SetAlpha(255);
    }
  }
}



void BoxLowBgDraw::DrawPokeIcon3(int pos, App::Box::PokemonIconDraw* pPkd)
{
  switch(pPkd->nowRedraw){
  case 0:
    break;
  case 1:
    DrawPokeIconState(pos,pPkd);
    pPkd->nowRedraw=0;
    break;
  case 2:
    DrawPokeIconState(pos,pPkd);
    PokeIconVisible(pos,false);
    pPkd->nowRedraw=3;
//    break;
  case 3:
    {
      if(!pPkd->bHide)
      {
        gfl2::lyt::LytParts* pParts;
        app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
        gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
        gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );
        pParts = GetIconParts(pos);
        gfl2::lyt::LytPicture * pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ICONPARTS_POKE, &resID );
        PokeIconVisible(pos,true);
        u32 tmp_pos = ( Savedata::BoxPokemon::TRAY_POKE_MAX + pml::PokeParty::MAX_MEMBERS ) * 2 + pos;
        mpAppParam->pPokeIcon->ReplacePaneTexture( tmp_pos, &pPkd->simp, NULL );  //最初は位置
        mpAppParam->pPokeIcon->CopyTextureBuffer( tmp_pos, pos );
        mpAppParam->pPokeIcon->ReplaceReadTexture( pos, pPane );
      }
      pPkd->nowRedraw=0;
      mbPokeIconInit=true;  //アイコン初期化完了
    }
    break;

  case 4:
    DrawPokeIconState( pos, pPkd );
    if( !pPkd->bHide )
    {
      app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
      gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
      gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );
      gfl2::lyt::LytParts* pParts = GetIconParts(pos);
      mpAppParam->pPokeIcon->CopyTextureBuffer( (Savedata::BoxPokemon::TRAY_POKE_MAX+pml::PokeParty::MAX_MEMBERS)*2+pos, pos );
      mpAppParam->pPokeIcon->ReplaceReadTexture(
        pos, pLytWk->GetPicturePane(pParts,PANENAME_BOX_ICN_LOW_000_PANE_ICONPARTS_POKE,&resID) );
    }
    pPkd->nowRedraw = 0;
    mbPokeIconInit=true;  //アイコン初期化完了
    PokeIconVisible(pos,!pPkd->bHide);
    break;
  }
}



void BoxLowBgDraw::ResetLiftPokeIconClear(void)
{

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );
  gfl2::lyt::LytParts* pParts;
  gfl2::lyt::LytPicture * pPane;
  for(int pos=0;pos<Savedata::BoxPokemon::TRAY_POKE_MAX;pos++){
    pParts = pLytWk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_30+pos );
    pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ICONPARTS_POKE, &resID );
    pPane->SetVisible(false);
    pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ICONPARTS_POKESHADOW, &resID );
    pPane->SetVisible(false);
  }


}



void BoxLowBgDraw::LiftPokeIconClear(void)
{
  ResetLiftPokeIconClear();
 // ResetMainCursor();

}


void BoxLowBgDraw::LiftPokeIconReady(int page, int pos)
{
  mpBoxAppSetup->GetBoxSimpleAccesser()->PushPokemonIconDrawData(page,pos,0,0);  //現状保存

  PokeIconWhite(page, pos, true);

  PassiveMainCursor();

  if(pos >= Savedata::BoxPokemon::TRAY_POKE_MAX)
  {
    if(mpBoxAppSetup->GetMoveMode() != BOX_CHANGE_SELECTMODE_SHORTCUT){
      StartTemotiSelect();
    }
  }
  PassiveUpperButton();
  PassiveUnderButton();
  PassiveBoxChangeCursor();
  PassiveFolderButton();
  PassiveCancelButton();
}


void BoxLowBgDraw::LiftPokeIconShortcut(int page, int pos)
{
  mpBoxAppSetup->GetBoxSimpleAccesser()->PushPokemonIconDrawData(page,pos,0,0);  //現状保存

  PassiveUpperButton();
}


void BoxLowBgDraw::LiftPokeIconReadyReturn(int page, int pos)
{
  PokeIconActive(page, pos);
 // gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  ResetMainCursor();
  mpBoxAppSetup->GetBoxSimpleAccesser()->RefreshPokemonIconDrawData();  //現状復帰

  EndTemotiSelect();
}

void BoxLowBgDraw::LiftPokeIconReadyReturn2(int page, int pos)
{
  PokeIconActive(page, pos);
//  ResetMainCursor();
  mpBoxAppSetup->GetBoxSimpleAccesser()->RefreshPokemonIconDrawData();  //現状復帰
}



gfl2::lyt::LytParts* BoxLowBgDraw::MovePokeIconReady(int page, int pos)
{
  PokeTool::SimpleParam* pPara = mpBoxAppSetup->GetBoxSimpleAccesser()->GetSimpleData(page, pos);

  GetIconParts(pos)->SetVisible(false);//今のアイコンを消す

  gfl2::math::VEC3 vec3;
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytParts* pParts = GetIconParts(pos);
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );
  gfl2::lyt::LytPicture * pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ICONPARTS_POKE, &resID );


  pParts = pLytWk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_108 );
 // pParts->SetTranslate(vec3);
  pParts->SetVisible(true);

  
  pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ITEMICON, &resID );
  pPane->SetVisible(false);
  pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ITEMICONSHADOW, &resID );
  pPane->SetVisible(false);
  pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ICONPARTS_POKESHADOW, &resID );
  pPane->SetVisible(false);
  pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ICONPARTS_POKE, &resID );

  if(pPara->monsNo != MONSNO_NULL){
    pPane->SetVisible(true);
    mpAppParam->pPokeIcon->CopyTextureBuffer( pos, Savedata::BoxPokemon::TRAY_POKE_MAX+pml::PokeParty::MAX_MEMBERS+pos );
    mpAppParam->pPokeIcon->ReplaceReadTexture( Savedata::BoxPokemon::TRAY_POKE_MAX+pml::PokeParty::MAX_MEMBERS+pos, pPane );

    App::Box::PokemonIconDraw* pPkd = mpBoxAppSetup->GetBoxSimpleAccesser()->GetPokemonIconDrawData(page, pos);
    SetPokeIconColorAndAlpha( pParts, pPkd );   // マテリアルカラーとアルファの変更
  }
  else{
    pPane->SetVisible(false);
  }
  
  return pParts;
}






void BoxLowBgDraw::LiftPokeIconStart(int page, int pos, bool bteam)
{
  PokeTool::SimpleParam* pPara = mpBoxAppSetup->GetBoxSimpleAccesser()->GetSimpleData(page, pos);

  gfl2::math::VEC3 vec3;
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
//  gfl2::lyt::LytParts* pParts = GetIconParts(pos);
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );
//  gfl2::lyt::LytPicture * pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ICONPARTS_POKE, &resID );

  mpBoxAppSetup->GetBoxSimpleAccesser()->PushPokemonIconDrawData(page,pos,0,0);  //現状保存

  // bwhiteをOFFにする
  mpBoxAppSetup->GetBoxSimpleAccesser()->SetPokeIconWhite( page, pos, false );

  // bHideがONになるので先に変更する
  gfl2::lyt::LytParts* pParts = pLytWk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_30 );  //０からはじめる
  App::Box::PokemonIconDraw* pPkd = mpBoxAppSetup->GetBoxSimpleAccesser()->GetPokemonIconDrawData(page, pos);
  SetPokeIconColorAndAlpha( pParts, pPkd );   // マテリアルカラーとアルファの変更

  if(bteam){
    mpBoxAppSetup->GetBoxSimpleAccesser()->SetPokeIconWhite(page,pos,false);
    mpBoxAppSetup->GetBoxSimpleAccesser()->SetPokeIconActive(page,pos,false);
  }
  else{
    mpBoxAppSetup->GetBoxSimpleAccesser()->SetPokeIconWhite(page,pos,false);
    mpBoxAppSetup->GetBoxSimpleAccesser()->SetVisible(page,pos,false);
    mpBoxAppSetup->GetBoxSimpleAccesser()->SetCarryFlg(page,pos,true);
  }

  gfl2::lyt::LytPane * pPane2 = pLytWk->GetPane(PANENAME_BOX_MIN_LOW_000_PANE_POKESET_00);
  pPane2->SetVisible(true);

//  pParts = pLytWk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_30 );  //０からはじめる
  pParts->SetTranslate(mVecMoveCursorBackup);

  gfl2::lyt::LytPicture * pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ITEMICON, &resID );
  pPane->SetVisible(false);
  pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ITEMICONSHADOW, &resID );
  pPane->SetVisible(false);

  pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ICONPARTS_POKE, &resID );
  pPane->SetVisible(true);
  mpAppParam->pPokeIcon->CopyTextureBuffer( pos, Savedata::BoxPokemon::TRAY_POKE_MAX+pml::PokeParty::MAX_MEMBERS+pos );
  mpAppParam->pPokeIcon->ReplaceReadTexture( Savedata::BoxPokemon::TRAY_POKE_MAX+pml::PokeParty::MAX_MEMBERS+pos, pPane );
  vec3=pPane->GetTranslate();

  pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ICONPARTS_POKESHADOW, &resID );
  mpAppParam->pPokeIcon->ReplacePaneTexture(Savedata::BoxPokemon::TRAY_POKE_MAX+ pml::PokeParty::MAX_MEMBERS + pos, pPane );
  pPane->SetVisible(true);
  vec3.x += 3;
  vec3.y -= 3;
  pPane->SetTranslate(vec3);
  //  OHNO_PRINT("PokeIcon表示 %f %f %f\n",vec3.x,vec3.y,vec3.z);

  // PokeIconActive(page);
  ResetTemotiTray();
  ResetMainCursor();

  if( mMousePokemonMode != BOX_MOUSE_MODE_TUKAMI){
    ResetCancelButton();
  }

  StopMainCursor();
  ResetBoxChangeCursor();




}


void BoxLowBgDraw::LiftPokeIconMove( gfl2::math::VEC3* pvec)
{
  //app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytPane * pPane2 = pLytWk->GetPane(PANENAME_BOX_MIN_LOW_000_PANE_POKESET_00);
  //  pPane2->SetVisible(true);

  //OHNO_PRINT("PokeIcon表示 %f %f %f\n",pvec->x,pvec->y,pvec->z);

  gfl2::math::VEC3 vec = pPane2->GetTranslate();
  vec.x =  pvec->x + 78;
  vec.y =  pvec->y - 51;

  pPane2->SetTranslate(vec);
  //  OHNO_PRINT("PokeIcon表示 %f %f %f\n",vec.x,vec.y,vec.z);
  pPane2 = pLytWk->GetPane(PANENAME_BOX_MIN_LOW_000_PANE_POKESET_03);

  vec = pPane2->GetTranslate();
  vec.x =  pvec->x + 78;
  vec.y =  pvec->y - 51 - 15;

  pPane2->SetTranslate(vec);


  
}



void BoxLowBgDraw::LiftPokeIconEnd(int page,int tray)
{
//  int i;
  //app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

 // gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
 // gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );

  mpBoxAppSetup->GetBoxSimpleAccesser()->RefreshPokemonIconDrawData();  //現状復帰

//  Sound::PlaySE(SEQ_SE_RELEASE);
  StartMainCursor();


}


void BoxLowBgDraw::DrawPokeIconBefore(int no)
{
  {
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
    gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );

    u32 start = ( Savedata::BoxPokemon::TRAY_POKE_MAX + pml::PokeParty::MAX_MEMBERS ) * 2;

    for(int pos=0;pos<Savedata::BoxPokemon::TRAY_POKE_MAX;pos++){
      gfl2::lyt::LytParts* pParts = pLytWk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_60+pos );
      App::Box::PokemonIconDraw* pPkd = mpBoxAppSetup->GetBoxSimpleAccesser()->GetPokemonIconDrawData(no,pos);
      if( !pPkd->bHide )
      {
        mpAppParam->pPokeIcon->ReplacePaneTexture( start+pos, &pPkd->simp, pLytWk->GetPicturePane(pParts,PANENAME_BOX_ICN_LOW_000_PANE_ICONPARTS_POKE,&resID) );
        pLytWk->GetPane(pParts,PANENAME_BOX_ICN_LOW_000_PANE_ICONPARTS_POKESHADOW,&resID)->SetVisible(false);
        SetPokeIconColorAndAlpha( pParts, pPkd );
      }
      pParts->SetVisible(!pPkd->bHide);
    }
  }
}



void BoxLowBgDraw::DrawBoxNameBefore(int no)
{

  mpTempStrBuf->SetStr(mpAppParam->pBoxData->GetTrayName(no));

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytParts* pParts = pLytWk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_BTN_BOXCATEGORY_01 );
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );
  gfl2::lyt::LytTextBox * pPane = pLytWk->GetTextBoxPane( pParts, PANENAME_BOX_BTN_LOW_006_PANE_TEXTBOX_00, &resID );

  pG2DUtil->SetTextBoxPaneString( pPane, mpTempStrBuf );


}




void BoxLowBgDraw::DrawBoxName(void)
{

  mpTempStrBuf->SetStr(mpAppParam->pBoxData->GetTrayName(mpAppParam->nowDispTray));

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytParts* pParts = pLytWk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_BTN_BOXCATEGORY_00 );
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );
  gfl2::lyt::LytTextBox * pPane = pLytWk->GetTextBoxPane( pParts, PANENAME_BOX_BTN_LOW_006_PANE_TEXTBOX_00, &resID );

  pG2DUtil->SetTextBoxPaneString( pPane, mpTempStrBuf );



}


void BoxLowBgDraw::DrawPageChangeAnim(int changenum)
{
  if(!mbPageChangeAnim){

    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );

    if(changenum==0){
      if(m_uiDrawListener)
      {
        m_uiDrawListener->OnPageScrollEnd(BoxLowBgDrawListener::SCROLL_PAGE_RESET);
      }
    }
    else if(changenum>0){
      pG2DUtil->StopAnime(LYT_WORK_NORMAL,LA_MINLOW_BOX_MIN_LOW_000_POKEICON_SET_01);
      pLytWk->BindAnime(LA_MINLOW_BOX_MIN_LOW_000_POKEICON_SET_00,false);
      pG2DUtil->StartAnime(  LYT_WORK_NORMAL,  LA_MINLOW_BOX_MIN_LOW_000_POKEICON_SET_00 );
      mbPageChangeAnim = true;
      mPageChangeSpeed = changenum;
    }
    else{
      pG2DUtil->StopAnime(LYT_WORK_NORMAL,LA_MINLOW_BOX_MIN_LOW_000_POKEICON_SET_00);
      pLytWk->BindAnime(LA_MINLOW_BOX_MIN_LOW_000_POKEICON_SET_01,false);
      pG2DUtil->StartAnime(  LYT_WORK_NORMAL,  LA_MINLOW_BOX_MIN_LOW_000_POKEICON_SET_01 );
      mbPageChangeAnim = true;
      mPageChangeSpeed = changenum;
    }
  }

}



void BoxLowBgDraw::DrawTeamAnim(int changenum)
{
  if(!mbTeamChangeAnim){

    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );

    if(changenum==0){
      if(m_uiDrawListener)
      {
        m_uiDrawListener->OnPageScrollEnd(BoxLowBgDrawListener::SCROLL_TEAM_RESET);
      }
    }
    else if(changenum>0){
      //      pG2DUtil->StopAnime(LYT_WORK_NORMAL,LA_MINLOW_BOX_MIN_LOW_000_TEAM_SET_00);
      //      pLytWk->BindAnime(LA_MINLOW_BOX_MIN_LOW_000_TEAM_SET_01,false);
      //      pG2DUtil->StartAnime(  LYT_WORK_NORMAL,  LA_MINLOW_BOX_MIN_LOW_000_TEAM_SET_01 );

      pLytWk->BindAnime( LA_MINLOW_BOX_MIN_LOW_000_TEAM_SET_01, false );
      pLytWk->SetAutoAnime( LA_MINLOW_BOX_MIN_LOW_000_TEAM_SET_01, true );
      pLytWk->SetAnimeFrame( LA_MINLOW_BOX_MIN_LOW_000_TEAM_SET_01, 0.0f );

      mbTeamChangeAnim = true;
      mTeamChangeSpeed = changenum;
    }
    else{
      //      pG2DUtil->StopAnime(LYT_WORK_NORMAL,LA_MINLOW_BOX_MIN_LOW_000_TEAM_SET_01);
      //      pLytWk->BindAnime(LA_MINLOW_BOX_MIN_LOW_000_TEAM_SET_00,false);
      //     pG2DUtil->StartAnime(  LYT_WORK_NORMAL,  LA_MINLOW_BOX_MIN_LOW_000_TEAM_SET_00 );

      pLytWk->BindAnime( LA_MINLOW_BOX_MIN_LOW_000_TEAM_SET_00, false );
      pLytWk->SetAutoAnime( LA_MINLOW_BOX_MIN_LOW_000_TEAM_SET_00, true );
      pLytWk->SetAnimeFrame( LA_MINLOW_BOX_MIN_LOW_000_TEAM_SET_00, 0.0f );

      mbTeamChangeAnim = true;
      mTeamChangeSpeed = changenum;
    }
  }


}


//チームトレイの表示
void BoxLowBgDraw::localDrawTeamName(int i,int partsno, int color)
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();


  if((mpBoxAppSetup->GetSelectMode() == BOX_CHANGE_EDITTYPE_BATTLE_TEAM)){
    pG2DUtil->GetString( mpTempStrBuf, msg_team_name_00 );
    mpTempStrBuf->SetStr(mpAppParam->pBoxData->GetTeamName( i));
  }
  else if((mpAppParam->type==BOX_CALL_RESORT_HOTSPA) || (mpAppParam->type==BOX_CALL_RESORT_GIM))
  {
    pG2DUtil->GetString( mpTempStrBuf, msg_group_name_02 + i );
  }
  else
  {
    pG2DUtil->GetString( mpTempStrBuf, msg_team_name_00 );
  }


  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytParts* pParts = pLytWk->GetPartsPane( partsno );
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );
  gfl2::lyt::LytTextBox * pPane = pLytWk->GetTextBoxPane( pParts, PANENAME_BOX_PLT_LOW_000_PANE_TEXTBOX_00, &resID );

  pG2DUtil->SetTextBoxPaneString( pPane, mpTempStrBuf );

  gfl2::lyt::LytPicture * pPane2 = pLytWk->GetPicturePane( pParts, PANENAME_BOX_PLT_LOW_000_PANE_ICON_LOCK, &resID );
  pPane2->SetVisible(false);

  if(BOX_CHANGE_EDITTYPE_BATTLE_TEAM == mpBoxAppSetup->GetSelectMode()){
    if(mpAppParam->pBoxData->IsTeamLock( i )){
      pPane2->SetVisible(true);
    }
    pG2DUtil->StartAnime(  LYT_WORK_NORMAL,  color + i + 1);
  }
  else if((mpAppParam->type==BOX_CALL_RESORT_HOTSPA) || (mpAppParam->type==BOX_CALL_RESORT_GIM))
  {
    pG2DUtil->StartAnime(  LYT_WORK_NORMAL,  color + 7 );
  }
  else{
    pG2DUtil->StartAnime(  LYT_WORK_NORMAL,  color );
  }


}



void BoxLowBgDraw::DrawTeamName(int i)
{
  localDrawTeamName(i,PANENAME_BOX_MIN_LOW_000_PANE_TEAM_00,LA_MINLOW_BOX_MIN_LOW_000__TEAM_00_COLOR_00 );
}

//未来のチームトレイの表示
void BoxLowBgDraw::DrawTeamNameBefore(int i)
{
  localDrawTeamName(i,PANENAME_BOX_MIN_LOW_000_PANE_TEAM_01,LA_MINLOW_BOX_MIN_LOW_000__TEAM_01_COLOR_00 );
}



void BoxLowBgDraw::DrawTeamIconBefore(int no)
{
  {
    gfl2::math::VEC3 vec3;
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );

    gfl2::lyt::LytPicture * pPane;
    gfl2::lyt::LytParts* pParts;
    gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );

    for(int pos = 0 ; pos < pml::PokeParty::MAX_MEMBERS ; pos++){
      pParts = pLytWk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_96 + pos );
      App::Box::PokemonIconDraw* pPkd = mpBoxAppSetup->GetBoxSimpleAccesser()->GetPokemonIconDrawData(no,Savedata::BoxPokemon::TRAY_POKE_MAX+pos);
      pParts->SetVisible(!pPkd->bHide);
    }
  }
}






void BoxLowBgDraw::DrawJudgeName(void)
{

  //  mpMsgData
  //    g2d->GetString( str, msg_bag_08_01 );

  // mpTempStrBuf->SetStr(mpAppParam->pBoxData->GetTrayName(mpAppParam->nowDispTray));

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  pG2DUtil->GetString( mpTempStrBuf, msg_box_judge );

  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytParts* pParts = pLytWk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_BTN_JUDGE );
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );
  gfl2::lyt::LytTextBox * pPane = pLytWk->GetTextBoxPane( pParts, PANENAME_BOX_BTN_LOW_005_PANE_TEXTBOX_00, &resID );

  pG2DUtil->SetTextBoxPaneString( pPane, mpTempStrBuf );
}


void BoxLowBgDraw::ExpandPokemon(int no, const pml::pokepara::CoreParam * pp )
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  pG2DUtil->SetRegisterPokeNickName(no, pp);
}


void BoxLowBgDraw::ExpandItem(int no, int itemno)
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  pG2DUtil->SetRegisterItemName(no, itemno);
}


void BoxLowBgDraw::DrawMessageLine1(int messageno )
{

  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytParts* pParts = pLytWk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_PLT_MESSAGE_L1 );
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );

  pG2DUtil->GetStringExpand( mpTempStrBuf, (u32)messageno );
  pParts->SetVisible( true );

  gfl2::lyt::LytTextBox * pPane = pLytWk->GetTextBoxPane( pParts, PANENAME_COMMON_MSG_LOW_003_PANE_TEXTBOX_00, &resID );
  pPane->SetVisible( true );

  pG2DUtil->SetTextBoxPaneString( pPane, mpTempStrBuf );

  pG2DUtil->SetPaneVisible( pPane, true );
}



void BoxLowBgDraw::DrawMessageLine2(int messageno)
{

  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytParts* pParts = pLytWk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_PLT_MESSAGE_L2 );
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );

  pG2DUtil->GetStringExpand( mpTempStrBuf, (u32)messageno );
  // pG2DUtil->SetMessage(mpTempStrBuf);
  pParts->SetVisible( true );

  gfl2::lyt::LytTextBox * pPane = pLytWk->GetTextBoxPane( pParts, PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_2, &resID );
  pPane->SetVisible( true );

  pG2DUtil->SetTextBoxPaneString( pPane, mpTempStrBuf );

  pG2DUtil->SetPaneVisible( pPane, true );
}


void BoxLowBgDraw::DrawMessageLine2Upper(int messageno)
{

  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytParts* pParts = pLytWk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_PLT_MESSAGE_L2 );
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );

  pG2DUtil->GetString( mpTempStrBuf, (u32)messageno );
  // pG2DUtil->SetMessage(mpTempStrBuf);
  pParts->SetVisible( true );
  gfl2::math::VEC3 vec = pParts->GetTranslate();
  vec.y =  96.0f;
  pParts->SetTranslate(vec);

  gfl2::lyt::LytTextBox * pPane = pLytWk->GetTextBoxPane( pParts, PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_2, &resID );
  pPane->SetVisible( true );
  pG2DUtil->SetTextBoxPaneString( pPane, mpTempStrBuf );

  pG2DUtil->SetPaneVisible( pPane, true );

}








void BoxLowBgDraw::MessageLine2Close(void)
{
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytParts* pParts = pLytWk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_PLT_MESSAGE_L2 );
  
  pParts->SetVisible( false );
  gfl2::math::VEC3 vec = pParts->GetTranslate();
  vec.y =  -96.0f;
  pParts->SetTranslate(vec);


  SetTimerIcon(false);

  pParts = pLytWk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_PLT_MESSAGE_L1 );
  pParts->SetVisible( false );


}

void BoxLowBgDraw::SetTimerIcon( bool flg )
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  if( flg != false )
  {
    g2d->StartAnime( 0, LA_MINLOW_BOX_MIN_LOW_000__PLT_MESSAGE_L2__TIMERICON_IN );
    App::Tool::TimeIcon::StartAnime( g2d->GetLayoutWork( LYT_WORK_NORMAL ), LA_MINLOW_BOX_MIN_LOW_000__PLT_MESSAGE_L2__TIMERICON_KEEP );
    // 表示
    gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYT_WORK_NORMAL );
    gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( LYT_WORK_NORMAL );
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_PLT_MESSAGE_L2 );
    gfl2::lyt::LytPane * pane = lytwk->GetPane( parts, PANENAME_COMMON_MSG_LOW_000_PANE_TIMERICON, &res_id );
    pane->SetVisible( true );
  }
  else
  {
    g2d->StartAnime( 0, LA_MINLOW_BOX_MIN_LOW_000__PLT_MESSAGE_L2__TIMERICON_OUT );
    gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYT_WORK_NORMAL );
    gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( LYT_WORK_NORMAL );
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_PLT_MESSAGE_L2 );
    gfl2::lyt::LytPane * pane = lytwk->GetPane( parts, PANENAME_COMMON_MSG_LOW_000_PANE_TIMERICON, &res_id );
    pane->SetVisible( false );

  }
}



//-------------------------------------------------------------------------------------マーク選択


enum{
  MARK_POS00 = 0,//ここから
  MARK_POS01,
  MARK_POS02,
  MARK_POS03,
  MARK_POS04,
  MARK_POS05,

  MARKCOMMAND_YES,
  MARKCOMMAND_NO,

  MARK_PANEL_MAX,
  MARK_POS04_05,
  MARK_MOVE_NULL,
};


int label_to_panemk[]=
{
  PANENAME_BOX_MIN_LOW_000_PANE_BTN_MARK_00,
  PANENAME_BOX_MIN_LOW_000_PANE_BTN_MARK_01,
  PANENAME_BOX_MIN_LOW_000_PANE_BTN_MARK_02,
  PANENAME_BOX_MIN_LOW_000_PANE_BTN_MARK_03,
  PANENAME_BOX_MIN_LOW_000_PANE_BTN_MARK_04,
  PANENAME_BOX_MIN_LOW_000_PANE_BTN_MARK_05,

  PANENAME_BOX_MIN_LOW_000_PANE_BTN_COMMAND_00_CURPOS,
  PANENAME_BOX_MIN_LOW_000_PANE_BTN_COMMAND_01_CURPOS,

};



int buttonmovemk[][6]=
{
  //上下左右を記す
  {  MARK_MOVE_NULL, MARK_POS02, MARK_MOVE_NULL, MARK_POS01,0 },  //  MARK_POS00
  {  MARK_MOVE_NULL, MARK_POS03, MARK_POS00, MARK_MOVE_NULL,0 },  //  MARK_POS01
  {  MARK_POS00, MARK_POS04, MARK_MOVE_NULL, MARK_POS03,0 },  //  MARK_POS02
  {  MARK_POS01, MARK_POS05, MARK_POS02, MARK_MOVE_NULL,0 },  //  MARK_POS03
  {  MARK_POS02, MARKCOMMAND_YES, MARK_MOVE_NULL, MARK_POS05,MARK_POS04 },  //  MARK_POS04
  {  MARK_POS03, MARKCOMMAND_YES, MARK_POS04, MARK_MOVE_NULL,MARK_POS05 },  //  MARK_POS05

  {  MARK_POS04_05, MARKCOMMAND_NO, MARK_MOVE_NULL, MARK_MOVE_NULL,0 },  //  MARKCOMMAND_YES
  {  MARKCOMMAND_YES, MARK_MOVE_NULL, MARK_MOVE_NULL, MARK_MOVE_NULL,0 },  //  MARKCOMMAND_NO

};



//------------------------------------------------------------------------------
/**
 * @brief   ボタンマネージャの生成
 */
//------------------------------------------------------------------------------
void BoxLowBgDraw::CreateMarkButtonManager( void )
{
  static const int partsnum = 8;
  static const int layoutno = LYT_WORK_NORMAL;

  app::ui::ButtonInfoEx c_DefInfomk[partsnum] = {
    {
      BoxLowBgDrawListener::LOWBG_FOLDER_ACTION_MARK0 , NULL, NULL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_00_TOUCH,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_00_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_00_CANSEL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_00_TOUCH_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_00_ACTIVE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_00_PASSIVE,
    },
    {
      BoxLowBgDrawListener::LOWBG_FOLDER_ACTION_MARK1 , NULL, NULL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_01_TOUCH,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_01_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_01_CANSEL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_01_TOUCH_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_01_ACTIVE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_01_PASSIVE,
    },
    {
      BoxLowBgDrawListener::LOWBG_FOLDER_ACTION_MARK2 , NULL, NULL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_02_TOUCH,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_02_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_02_CANSEL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_02_TOUCH_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_02_ACTIVE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_02_PASSIVE,
    },
    {
      BoxLowBgDrawListener::LOWBG_FOLDER_ACTION_MARK3 , NULL, NULL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_03_TOUCH,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_03_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_03_CANSEL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_03_TOUCH_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_03_ACTIVE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_03_PASSIVE,
    },
    {
      BoxLowBgDrawListener::LOWBG_FOLDER_ACTION_MARK4 , NULL, NULL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_04_TOUCH,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_04_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_04_CANSEL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_04_TOUCH_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_04_ACTIVE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_04_PASSIVE,
    },
    {
      BoxLowBgDrawListener::LOWBG_FOLDER_ACTION_MARK5 , NULL, NULL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_05_TOUCH,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_05_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_05_CANSEL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_05_TOUCH_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_05_ACTIVE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_05_PASSIVE,
    },

    {
      BoxLowBgDrawListener::LOWBG_FOLDER_ACTION_OK , NULL, NULL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_COMMAND_00_TOUCH,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_COMMAND_00_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_COMMAND_00_CANSEL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_COMMAND_00_TOUCH_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_COMMAND_00_ACTIVE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_COMMAND_00_PASSIVE,
    },
    {
      BoxLowBgDrawListener::LOWBG_FOLDER_ACTION_CANCEL , NULL, NULL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_COMMAND_01_TOUCH,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_COMMAND_01_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_COMMAND_01_CANSEL,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_COMMAND_01_TOUCH_RELEASE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_COMMAND_01_ACTIVE,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_COMMAND_01_PASSIVE,
    },


  };

  int painNomk[]={
    PANENAME_BOX_MIN_LOW_000_PANE_BTN_MARK_00,
    PANENAME_BOX_MIN_LOW_000_PANE_BTN_MARK_01,
    PANENAME_BOX_MIN_LOW_000_PANE_BTN_MARK_02,
    PANENAME_BOX_MIN_LOW_000_PANE_BTN_MARK_03,
    PANENAME_BOX_MIN_LOW_000_PANE_BTN_MARK_04,
    PANENAME_BOX_MIN_LOW_000_PANE_BTN_MARK_05,
    PANENAME_BOX_MIN_LOW_000_PANE_BTN_COMMAND_00,
    PANENAME_BOX_MIN_LOW_000_PANE_BTN_COMMAND_01,
  };


  //  app::ui::ButtonInfoEx* info;

  app::ui::ButtonInfoEx * info = GFL_NEW_LOW_ARRAY(mpAppHeap->GetSystemHeap()) app::ui::ButtonInfoEx[partsnum];

  int i=0;
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( layoutno );
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( layoutno );

  gfl2::lyt::LytParts* pParts[partsnum];

  for(i=0;i<partsnum;i++){
    pParts[i] = pLytWk->GetPartsPane( painNomk[i] );
  }
  for( i = 0 ; i < partsnum; i++){
    info[i] = c_DefInfomk[i];
    info[i].picture_pane = pParts[i];
    info[i].bound_pane = pLytWk->GetPane( pParts[i], PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &resID );
  }
  this->ReloadButtonManager( mpAppHeap, pLytWk, info, partsnum );

  GFL_DELETE_ARRAY info;

  app::tool::ButtonManager* pBtnMan = this->GetButtonManager();

  pBtnMan->SetButtonBindKey( BoxLowBgDrawListener::LOWBG_FOLDER_ACTION_CANCEL, gfl2::ui::BUTTON_B, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );
  pBtnMan->SetButtonSelectSE( BoxLowBgDrawListener::LOWBG_FOLDER_ACTION_CANCEL, SEQ_SE_CANCEL1 );


}




int BoxLowBgDraw::SetCursorPosMk(int no,int udlr )
{
  gfl2::math::VEC3 vec3;

  GFL_ASSERT(no < MARK_PANEL_MAX);

  int nextbutton = buttonmovemk[no][udlr];  //次の位置
  int ver = buttonmovemk[no][4];

  if(ver != 0){
    mKeepCursorPosVerticalMk = ver;
  }
  if(nextbutton == MARK_MOVE_NULL){
    nextbutton = no;
  }
  if(nextbutton == MARK_POS04_05){  //4か5-のどれか
    switch(mKeepCursorPosVerticalMk){
    case MARK_POS04:
      nextbutton=MARK_POS04;
      break;
    case MARK_POS05:
      nextbutton=MARK_POS05;
      break;
    default:
      GFL_ASSERT(0);  //
    }
  }
  OHNO_PRINT("ma-ku cursor %d  %d\n",nextbutton,udlr);

  if(mMarkButtonPos!=nextbutton){
    Sound::PlaySE(SEQ_SE_SELECT1);
  }

  mMarkButtonPos = nextbutton;
  return nextbutton;
}


void BoxLowBgDraw::DirectCursorPosMk(int no )
{
  mMarkButtonPos = no;
}



void BoxLowBgDraw::MarkWinStart(int btemochi)
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );
  pLytWk->GetPane(PANENAME_BOX_MIN_LOW_000_PANE_NULL_COMMAND)->SetVisible(true);

  mMarkAnimType = btemochi;
  if(!btemochi){
    pG2DUtil->StartAnime(  LYT_WORK_NORMAL,  LA_MINLOW_BOX_MIN_LOW_000_COMMAND_IN );
  }
  else{
    pLytWk->GetPane(PANENAME_BOX_MIN_LOW_000_PANE_NULL_COMMAND)->SetVisible(true);
  }
  mbMarkWin = true;

  {
    pG2DUtil->GetString( mpTempStrBuf, msg_box_01_50 );

    gfl2::lyt::LytParts* pParts = pLytWk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_BTN_COMMAND_00 );
    gfl2::lyt::LytTextBox * pPane = pLytWk->GetTextBoxPane( pParts, PANENAME_BOX_BTN_LOW_010_PANE_TEXTBOX_00, &resID );
    pG2DUtil->SetTextBoxPaneString( pPane, mpTempStrBuf );

  }
  {
    pG2DUtil->GetString( mpTempStrBuf, msg_box_end );

    gfl2::lyt::LytParts* pParts = pLytWk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_BTN_COMMAND_01 );
    gfl2::lyt::LytTextBox * pPane = pLytWk->GetTextBoxPane( pParts, PANENAME_BOX_BTN_LOW_010_PANE_TEXTBOX_00, &resID );

    pG2DUtil->SetTextBoxPaneString( pPane, mpTempStrBuf );

    gfl2::lyt::LytPane* pPaneb = pLytWk->GetPane( pParts, PANENAME_BOX_BTN_LOW_010_PANE_ICON_BACK, &resID );
    pPaneb->SetVisible(true);
    
  }

  //ResetUnderButton();  kawa_box_modify  マークウィンドウに行くときはまだ下段ボタン郡を復活させなくていいのでコメントアウト。
  CreateMarkButtonManager();

}


void BoxLowBgDraw::MarkWinEnd(void)
{
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  mpBoxAppSetup->GetBoxSimpleAccesser()->RefreshPokemonIconDrawData();  //現状復帰
  int anm=0;

  if( !mMarkAnimType ){
    anm = LA_MINLOW_BOX_MIN_LOW_000_COMMAND_OUT;
    pLytWk->BindAnime( anm, true );
    pLytWk->SetAutoAnime( anm, true );
    pLytWk->SetAnimeFrame( anm, 0.0f );
  }
  else{
    ResetTemotiTray();
  }

  mbMarkWin = false;

  MyCreateButtonManager(true);
 // ResetFunc();


}




//----------------------------------------------------------------------
/**
 * @brief アイコンの部品を得る
 * @param heap  ヒープ
 */
//----------------------------------------------------------------------

gfl2::lyt::LytParts* BoxLowBgDraw::GetIconParts(int pos)
{
  gfl2::lyt::LytParts* pParts;
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );

  if(pos < Savedata::BoxPokemon::TRAY_POKE_MAX){
    pParts = pLytWk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_00 + pos );
  }
  else{
    pParts = pLytWk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_90 + pos - Savedata::BoxPokemon::TRAY_POKE_MAX );
  }
  return pParts;
}


//----------------------------------------------------------------------
/**
 * @brief 移動しているアイコンの部品を得る
 * @param heap  ヒープ
 */
//----------------------------------------------------------------------

gfl2::lyt::LytParts* BoxLowBgDraw::GetCarryIconParts(int pos)
{
  gfl2::lyt::LytParts* pParts;
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  pParts = pLytWk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_30 + pos );
  return pParts;
}



void BoxLowBgDraw::IconClear(void)
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );
  gfl2::lyt::LytParts* pParts;
  gfl2::lyt::LytPicture * pPane;
  for(int pos=0;pos<Savedata::BoxPokemon::TRAY_POKE_MAX;pos++){
    pParts = pLytWk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_00+pos );
    pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ICONPARTS_POKE, &resID );
    pPane->SetVisible(false);
    pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ICONPARTS_POKESHADOW, &resID );
    pPane->SetVisible(false);
  }

  for(int pos=0;pos<pml::PokeParty::MAX_MEMBERS;pos++){
    pParts = pLytWk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_90 + pos);
    pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ICONPARTS_POKE, &resID );
    pPane->SetVisible(false);
    pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ICONPARTS_POKESHADOW, &resID );
    pPane->SetVisible(false);
  }


}



/*
void BoxLowBgDraw::BtlCursorPosEnd(int cursor)
{
  SetCursorPos(cursor,-1);
}
*/

void BoxLowBgDraw::InfoDispStart(app::tool::InfoWindowBookTypeListener* pListener)
{
  gfl2::math::Vector3 pos( 145.0f, -100.0f, 0.0f );

  mpInfoWindowBookType = GFL_NEW(mpAppHeap->GetDeviceHeap()) app::tool::InfoWindowBookType( mpAppHeap );

  mpInfoWindowBookType->SyncCreate(mpAppHeap,  mpAppRenderingManager ,6 ,NULL);

  mpInfoWindowBookType->ResetContents(6, true);  // @fix GFNMCat[964] 「ボックス」のインフォメーションでページを切り替えるとアサート
  //                   インフォウィンドウの使い方を正しく直した。

  AddSubView(mpInfoWindowBookType);

  GetG2DUtil()->GetString( mpTempStrBuf1 , msg_box_info_01_01 );
  GetG2DUtil()->GetString( mpTempStrBuf2, msg_box_info_01_02 );
  GetG2DUtil()->GetString( mpTempStrBuf3, msg_box_info_01_03 );
  GetG2DUtil()->GetString( mpTempStrBuf4, msg_box_info_01_04 );
  GetG2DUtil()->GetString( mpTempStrBuf5, msg_box_info_01_05 );
  GetG2DUtil()->GetString( mpTempStrBuf6, msg_box_info_01_06 );

  mpInfoWindowBookType->SetTextContent(0,mpTempStrBuf1 ,NULL);
  mpInfoWindowBookType->SetTextContent(1,mpTempStrBuf2,NULL);
  mpInfoWindowBookType->SetTextContent(2,mpTempStrBuf3,NULL);
  mpInfoWindowBookType->SetTextContent(3,mpTempStrBuf4,NULL);
  mpInfoWindowBookType->SetTextContent(4,mpTempStrBuf5,NULL);
  mpInfoWindowBookType->SetTextContent(5,mpTempStrBuf6,NULL);

  mpInfoWindowBookType->StartOpen(pListener, pos);

  //mbBattleInWin=true;
}


void BoxLowBgDraw::InfoDispClose(void)
{
  mInfoRemove = 1;
  //  mbBattleInWin=false;
}


void BoxLowBgDraw::StartMarkAnim(int no, int mark)
{
  GFL_ASSERT(no<6);
  GFL_ASSERT(mark<3);

  int animno[6][3]=
  {
    { LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_00_COLOR_00,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_00_COLOR_02,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_00_COLOR_01,},
    { LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_01_COLOR_00,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_01_COLOR_02,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_01_COLOR_01,},
    { LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_02_COLOR_00,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_02_COLOR_02,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_02_COLOR_01,},
    { LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_03_COLOR_00,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_03_COLOR_02,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_03_COLOR_01,},
    { LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_04_COLOR_00,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_04_COLOR_02,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_04_COLOR_01,},
    { LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_05_COLOR_00,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_05_COLOR_02,
      LA_MINLOW_BOX_MIN_LOW_000__BTN_MARK_05_COLOR_01,},
  };


  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  pG2DUtil->StartAnime(  LYT_WORK_NORMAL,  animno[no][mark] );

}




bool BoxLowBgDraw::MarkSrideEndFunc(void)
{
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::math::VEC3 vec3;
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

  if(mbMarkWin)
  {
    if(!pLytWk->IsBindAnime(LA_MINLOW_BOX_MIN_LOW_000_COMMAND_IN)){
      //			if(pLytWk->IsAnimeEnd(LA_MINLOW_BOX_MIN_LOW_000_COMMAND_IN)){
      mBkMarkButtonPos=-1;
      OHNO_PRINT("バインド          おわり\n");
      return true;
      //		}
    }
    else{
      OHNO_PRINT("バインド中***********************\n");
    }
  }
  return false;
}


void BoxLowBgDraw::ReloadCursorPos(void)
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::math::VEC3 vec3;

  if(mbMarkWin)
  {
    if(mMarkButtonPos == mBkMarkButtonPos){
      return;
    }
    mBkMarkButtonPos = mMarkButtonPos;
    mBkButtonPos=-1;

    if(mMarkButtonPos==MARKCOMMAND_YES || mMarkButtonPos==MARKCOMMAND_NO){
      CursorChange(false);
      //      pG2DUtil->StartAnime(  LYT_WORK_NORMAL, LA_MINLOW_BOX_MIN_LOW_000__CURSOR_CUR_HORIZ );
    }
    else{
      CursorChange(true);
      //      pG2DUtil->StartAnime(  LYT_WORK_NORMAL, LA_MINLOW_BOX_MIN_LOW_000__CURSOR_CUR_VERT );
    }

    if(mMarkButtonPos < 6){
      vec3 = pG2DUtil->GetPanePos( pLytWk->GetPane( pLytWk->GetPartsPane( label_to_panemk[mMarkButtonPos] ), PANENAME_BOX_BTN_LOW_011_PANE_BOUND_00, &resID));
    }
    else{
      vec3 = pG2DUtil->GetPanePos(0,  label_to_panemk[mMarkButtonPos]);
    }

    pG2DUtil->SetPanePos(0,  PANE_CURSOR, &vec3);
  }
  else
  {
    if(mButtonPos == mBkButtonPos){
      return;
    }
    mBkButtonPos = mButtonPos;
    mBkMarkButtonPos=-1;

    if(mButtonPos==POKEBUTTON_FOLDER){
      CursorChange(false);

      //      pG2DUtil->StartAnime(  LYT_WORK_NORMAL, LA_MINLOW_BOX_MIN_LOW_000__CURSOR_CUR_HORIZ );

    }
    else{
      CursorChange(true);

      //    pG2DUtil->StartAnime(  LYT_WORK_NORMAL, LA_MINLOW_BOX_MIN_LOW_000__CURSOR_CUR_VERT );

      vec3 = pG2DUtil->GetPanePos(0,  label_to_pane[mButtonPos]);
      if(mButtonPos<36 && mbStopCursor){
        vec3.y += 5.0f;
      }
      pG2DUtil->SetPanePos(0,  PANE_CURSOR, &vec3);
    }
  }



  //app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  //gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );


}





void BoxLowBgDraw::PushButtonModePos(int type, int onoff,int quick)
{
  int onlabel[]={
    LA_MINLOW_BOX_MIN_LOW_000__OBJECTTYPEBUTTON__BTN_OBJECTTYPE_00_SELECT_00,
    LA_MINLOW_BOX_MIN_LOW_000__OBJECTTYPEBUTTON__BTN_OBJECTTYPE_01_SELECT_01,
    LA_MINLOW_BOX_MIN_LOW_000__OBJECTTYPEBUTTON__BTN_OBJECTTYPE_02_SELECT_02,
  };
  int offlabel[]={
    LA_MINLOW_BOX_MIN_LOW_000__OBJECTTYPEBUTTON__BTN_OBJECTTYPE_00_UNSELECT_00,
    LA_MINLOW_BOX_MIN_LOW_000__OBJECTTYPEBUTTON__BTN_OBJECTTYPE_01_UNSELECT_01,
    LA_MINLOW_BOX_MIN_LOW_000__OBJECTTYPEBUTTON__BTN_OBJECTTYPE_02_UNSELECT_02,
  };
  int curlabel[]={
    LA_MINLOW_BOX_MIN_LOW_000_BGCOLOR_BOX,
    LA_MINLOW_BOX_MIN_LOW_000_BGCOLOR_ITEM,
    LA_MINLOW_BOX_MIN_LOW_000_BGCOLOR_TEAM,
  };


  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

  if((onoff == 1) && (mpBoxAppSetup->GetSelectButton(type)==0)){  //OFF->ON

    _animStart(onlabel[type],quick);
    _animStart(curlabel[type],quick);

    mpBoxAppSetup->SetSelectButton( type, onoff);//ON
  }
  else if((onoff == 0) && (mpBoxAppSetup->GetSelectButton(type)==1)){  //ON->OFF
    _animStart(offlabel[type],quick);

    mpBoxAppSetup->SetSelectButton( type, onoff);//OFF
  }
  else if(onoff == -1){  //-1
    if(mpBoxAppSetup->GetSelectButton( type)){ //ON
      _animStart(onlabel[type],quick);
      _animStart(curlabel[type],quick);

    }
    else{
      _animStart(offlabel[type],quick);
    }
  }
}


void BoxLowBgDraw::PushButtonMode(int type, int quick)
{
  switch(type){
  case -1:
    PushButtonModePos(0, -1,quick);
    PushButtonModePos(1, -1,quick);
    PushButtonModePos(2, -1,quick);
    break;
  case 0:
    PushButtonModePos(0,1,quick);
    PushButtonModePos(1,0,quick);
    PushButtonModePos(2,0,quick);
    break;
  case 1:
    PushButtonModePos(0,0,quick);
    PushButtonModePos(1,1,quick);
    PushButtonModePos(2,0,quick);
    break;
  case 2:
    PushButtonModePos(0,0,quick);
    PushButtonModePos(1,0,quick);
    PushButtonModePos(2,1,quick);
    break;
  }
}







void BoxLowBgDraw::_animStart(int no,int quick)
{
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  float x = pLytWk->GetAnimeMaxFrame(no );
  pLytWk->BindAnime(no ,true);
  pLytWk->SetAutoAnime( no, true );
  if(quick){
    pLytWk->SetAnimeFrame(no, x);
  }
}



void BoxLowBgDraw::PushButtonTypePos(int type, int onoff, int quick)
{


#if 1
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );

  int onlabel[]={
    LA_MINLOW_BOX_MIN_LOW_000__BTN_MOVETYPE_00_PATERN_00,
    LA_MINLOW_BOX_MIN_LOW_000__BTN_MOVETYPE_01_PATERN_00,
    LA_MINLOW_BOX_MIN_LOW_000__BTN_MOVETYPE_02_PATERN_00,
  };
  int offlabel[]={
    LA_MINLOW_BOX_MIN_LOW_000__BTN_MOVETYPE_00_PATERN_01,
    LA_MINLOW_BOX_MIN_LOW_000__BTN_MOVETYPE_01_PATERN_01,
    LA_MINLOW_BOX_MIN_LOW_000__BTN_MOVETYPE_02_PATERN_01,
  };
  int curlabel[]={
    LA_MINLOW_BOX_MIN_LOW_000__CURSOR_COLOR_RED,
    LA_MINLOW_BOX_MIN_LOW_000__CURSOR_COLOR_BLUE,
    LA_MINLOW_BOX_MIN_LOW_000__CURSOR_COLOR_GREEN,
  };

  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();



  if((onoff == 1) && (mpBoxAppSetup->GetMoveButton(type)==0)){  //OFF->ON
    // pG2DUtil->StartAnime(  LYT_WORK_NORMAL,  onlabel[type] );  //ON
    //  pG2DUtil->StartAnime(  LYT_WORK_NORMAL, curlabel[type] );  //ONのところでカーソル切り替え


    _animStart(onlabel[type],quick);
    _animStart(curlabel[type],quick);
    mpBoxAppSetup->SetMoveButton( type, onoff);//ON
  }
  else if((onoff == 0) && (mpBoxAppSetup->GetMoveButton(type)==1)){  //ON->OFF
    //    pG2DUtil->StartAnime(  LYT_WORK_NORMAL,  offlabel[type] );  //OFF

    {
      _animStart(offlabel[type],quick);
    }

    mpBoxAppSetup->SetMoveButton( type, onoff);//OFF
  }
  else if(onoff == -1){  //-1
    if(mpBoxAppSetup->GetMoveButton( type)){ //ON
      //      pG2DUtil->StartAnime(  LYT_WORK_NORMAL,  onlabel[type] );  //ON
      //      pG2DUtil->StartAnime(  LYT_WORK_NORMAL, curlabel[type] );  //ONのところでカーソル切り替え

      {
        _animStart(onlabel[type],quick);
      }
      {
        _animStart(curlabel[type],quick);
      }

    }
    else{

      {
        _animStart(offlabel[type],quick);
      }

      //      pG2DUtil->StartAnime(  LYT_WORK_NORMAL,  offlabel[type] );  //OFF
    }
  }
#endif
}


void BoxLowBgDraw::PushButtonType(int type, int quick)
{
  if(mbActiveRangeButton==false && type==2){
    return;
  }
  switch(type){
  case -1:
    PushButtonTypePos(0, -1,quick);
    PushButtonTypePos(1, -1,quick);
    PushButtonTypePos(2, -1,quick);
    break;
  case 0:
    PushButtonTypePos(0,1,quick);
    PushButtonTypePos(1,0,quick);
    PushButtonTypePos(2,0,quick);
    break;
  case 1:
    PushButtonTypePos(0,0,quick);
    PushButtonTypePos(1,1,quick);
    PushButtonTypePos(2,0,quick);
    break;
  case 2:
    PushButtonTypePos(0,0,quick);
    PushButtonTypePos(1,0,quick);
    PushButtonTypePos(2,1,quick);
    break;
  }
}


void BoxLowBgDraw::PokeIconWhite(int page,int x, bool bWhite)
{
  mpBoxAppSetup->GetBoxSimpleAccesser()->SetPokeIconWhite(page, x, bWhite);
}





void BoxLowBgDraw::PokeIconPassive(int page,int x)
{
  if(x != -1){
    SetPokeIconActive(page, x, false);
  }
  else{
    for(int i=0;i<30;i++){
      SetPokeIconActive(page, i, false);
    }
  }
}


void BoxLowBgDraw::PokeIconActive(int page,int x)
{

  if(x != -1){
    SetPokeIconActive(page,x, true);

  }
  else{
    for(int i=0;i<30;i++){
      SetPokeIconActive(page,i, true);
    }
  }
}







void BoxLowBgDraw::SetPokeIconActive(int page, int pos, bool bActive)
{

  if(bActive){
    mpBoxAppSetup->GetBoxSimpleAccesser()->SetPokeIconActive(page, pos,1);
  }
  else{
    mpBoxAppSetup->GetBoxSimpleAccesser()->SetPokeIconActive(page, pos, 0);
  }

}




void BoxLowBgDraw::StartTemotiSelect(void)
{
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  // app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  //  pG2DUtil->StartAnime(  LYT_WORK_NORMAL,  LA_MINLOW_BOX_MIN_LOW_000_TEAM_SET_02 );

  pLytWk->BindAnime( LA_MINLOW_BOX_MIN_LOW_000_TEAM_SET_02, false );
  pLytWk->SetAutoAnime( LA_MINLOW_BOX_MIN_LOW_000_TEAM_SET_02, true );
  pLytWk->SetAnimeFrame( LA_MINLOW_BOX_MIN_LOW_000_TEAM_SET_02, 0.0f );
  pLytWk->GetPane(PANENAME_BOX_MIN_LOW_000_PANE_NULL_COMMAND)->SetVisible(false);

  mbTemotiTrayCenter = true;
  mbTemotiCursorDisp = true;
  PassiveBoxChangeCursor();
  PassiveUpperButton();

  SetVisibleBoxTrayChangeCursor( false );
}

void BoxLowBgDraw::TemotiSelectCheckFunc(void)
{
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  if(  mbTemotiCursorDisp == true)
  {
    if(pLytWk->IsBindAnime(LA_MINLOW_BOX_MIN_LOW_000_TEAM_SET_02)){
      gfl2::math::VEC3 vec3;
      vec3 = GetG2DUtil()->GetPanePos(0,  label_to_pane[mButtonPos]);
      LiftPokeIconMove( &vec3 );
    }
    if(pLytWk->IsAnimeEnd(LA_MINLOW_BOX_MIN_LOW_000_TEAM_SET_02))
    {
      mbTemotiCursorDisp=false;
      pLytWk->UnBindAnime(LA_MINLOW_BOX_MIN_LOW_000_TEAM_SET_02);
    }
  }
  if( mbTemotiCursorDisp2 == true)
  {
    if(pLytWk->IsBindAnime(LA_MINLOW_BOX_MIN_LOW_000_TEAM_SET_03))
    {
      gfl2::math::VEC3 vec3;
      vec3 = GetG2DUtil()->GetPanePos(0,  label_to_pane[mButtonPos]);
      LiftPokeIconMove( &vec3 );

      ReloadCursorPos();


    }
    if(pLytWk->IsAnimeEnd(LA_MINLOW_BOX_MIN_LOW_000_TEAM_SET_03))
    {
      mbTemotiCursorDisp2 = false;
      pLytWk->UnBindAnime(LA_MINLOW_BOX_MIN_LOW_000_TEAM_SET_03);
      ResetMainCursor();
    }
  }
}




void BoxLowBgDraw::EndTemotiSelect(void)
{
  ResetFunc();
}







void BoxLowBgDraw::ResetTemotiTray(void)
{
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  if(mbTemotiTrayCenter){
    int anm = LA_MINLOW_BOX_MIN_LOW_000_TEAM_SET_03;
    pLytWk->BindAnime( anm, false );
    pLytWk->SetAutoAnime( anm, true );
    pLytWk->SetAnimeFrame( anm, 0.0f );
    mbTemotiTrayCenter=false;
    mbTemotiCursorDisp2=true;
  }
}



void BoxLowBgDraw::ResetMainCursorVisible(void)
{
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );

  if(mbCursorPassive){
    gfl2::lyt::LytPane * pPane2 = pLytWk->GetPane(PANE_CURSOR);
    pPane2->SetVisible(true);
    mbCursorPassive=false;
  }
}


void BoxLowBgDraw::ResetMainCursor(void)
{
  if(mMousePokemonMode == BOX_MOUSE_MODE_TUKAMI){ //ポケモンを握っていた
    return;
  }
  ResetMainCursorVisible();
  ResetCursorPos();
}


void BoxLowBgDraw::StopMainCursor(void)
{
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

  pLytWk->BindAnime(LA_MINLOW_BOX_MIN_LOW_000__CURSOR_KEEP,true);
  pLytWk->SetAutoAnime( LA_MINLOW_BOX_MIN_LOW_000__CURSOR_KEEP, false );
  pLytWk->SetAnimeFrame(LA_MINLOW_BOX_MIN_LOW_000__CURSOR_KEEP, 0.0f);

  mbStopCursor = true;
}


void BoxLowBgDraw::StartMainCursor(void)
{
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  pLytWk->BindAnime(LA_MINLOW_BOX_MIN_LOW_000__CURSOR_KEEP,true);
  pLytWk->SetAutoAnime( LA_MINLOW_BOX_MIN_LOW_000__CURSOR_KEEP, true );
  mbStopCursor = false;

}




void BoxLowBgDraw::PassiveMainCursor(void)
{
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytPane * pPane2 = pLytWk->GetPane(PANE_CURSOR);
  pPane2->SetVisible(false);
  mbCursorPassive=true;
}


void BoxLowBgDraw::PassiveUpperButton(void)
{
  if(!mbTouchPassive){
    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    pG2DUtil->StartAnime( LYT_WORK_NORMAL,LA_MINLOW_BOX_MIN_LOW_000_TOUCH_PASSIVE);
    app::tool::ButtonManager* pBtnMan = this->GetButtonManager();
    pBtnMan->SetButtonInputEnable(BTN_ID_CURTYPE_NORMAL,false);
    pBtnMan->SetButtonInputEnable(BTN_ID_CURTYPE_SHORTCUT,false);
    pBtnMan->SetButtonInputEnable(BTN_ID_CURTYPE_RANGE,false);
  }
  mbTouchPassive=true;

}


void BoxLowBgDraw::ResetUpperButton(void)
{
  if(mbTouchPassive){
    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    pG2DUtil->StartAnime( LYT_WORK_NORMAL,LA_MINLOW_BOX_MIN_LOW_000_TOUCH_ACTIVE);
    mbTouchPassive=false;

    app::tool::ButtonManager* pBtnMan = this->GetButtonManager();
    pBtnMan->SetButtonInputEnable(BTN_ID_CURTYPE_NORMAL,true);
    pBtnMan->SetButtonInputEnable(BTN_ID_CURTYPE_SHORTCUT,true);
    pBtnMan->SetButtonInputEnable(BTN_ID_CURTYPE_RANGE,true);

  }
}


void BoxLowBgDraw::PassiveCancelButton(void)
{
  if(!mbCancelButtonPassive)
  {
    app::tool::ButtonManager* pBtnMan = this->GetButtonManager();
    pBtnMan->SetButtonPassive( BTN_ID_CANCEL);
    mbCancelButtonPassive=true;
  }
}


void BoxLowBgDraw::ResetCancelButton(void)
{
  if(mbCancelButtonPassive)
  {
    app::tool::ButtonManager* pBtnMan = this->GetButtonManager();
    pBtnMan->SetButtonActiveForce( BTN_ID_CANCEL);  // kawa_box_modify  リセットでは必ずボタンの状態変更処理を行う。
    mbCancelButtonPassive=false;
  }
}


void BoxLowBgDraw::PassiveChatChangeButton(void)
{
  if(!mbChatChangeButtonPassive)
  {
    app::tool::ButtonManager* pBtnMan = this->GetButtonManager();
    pBtnMan->SetButtonPassive( BTN_ID_CHAT_CHANGE);
    mbChatChangeButtonPassive=true;
  }
}


void BoxLowBgDraw::ResetChatChangeButton(void)
{
  if(mbChatChangeButtonPassive)
  {
    app::tool::ButtonManager* pBtnMan = this->GetButtonManager();
    pBtnMan->SetButtonActiveForce( BTN_ID_CHAT_CHANGE);
    mbChatChangeButtonPassive=false;
  }
}



void BoxLowBgDraw::PassiveBoxTrayChangeCursor(void)
{
  if(!mbTemotiChangeCursor)
  {
    if( mpBoxAppSetup->GetSelectMode() == BOX_CHANGE_EDITTYPE_BATTLE_TEAM ){
      app::tool::ButtonManager* pBtnMan = this->GetButtonManager();
      pBtnMan->SetButtonPassive( BTN_ID_L2);
      pBtnMan->SetButtonPassive( BTN_ID_R2);
    }
    mbTemotiChangeCursor=true;
  }
}


void BoxLowBgDraw::ResetBoxTrayChangeCursor(void)
{
  if(mbTemotiChangeCursor)
  {
    if( mpBoxAppSetup->GetSelectMode() == BOX_CHANGE_EDITTYPE_BATTLE_TEAM ){
      app::tool::ButtonManager* pBtnMan = this->GetButtonManager();
      pBtnMan->SetButtonActiveForce( BTN_ID_L2 );
      pBtnMan->SetButtonActiveForce( BTN_ID_R2 );
    }
    mbTemotiChangeCursor=false;
  }
}


void BoxLowBgDraw::SetVisibleBoxTrayChangeCursor( bool flg )
{
  if( mpBoxAppSetup->GetSelectMode() == BOX_CHANGE_EDITTYPE_BATTLE_TEAM ){
    app::tool::ButtonManager* pBtnMan = this->GetButtonManager();
    if( flg == false )
    {
      pBtnMan->SetButtonInvisible( BTN_ID_L2 );
      pBtnMan->SetButtonInvisible( BTN_ID_R2 );
    }
    else
    {
      pBtnMan->SetButtonVisible( BTN_ID_L2 );
      pBtnMan->SetButtonVisible( BTN_ID_R2 );
    }
  }
}




void BoxLowBgDraw::PassiveBoxChangeCursor(void)
{
  app::tool::ButtonManager* pBtnMan = this->GetButtonManager();
  pBtnMan->SetButtonPassive( BTN_ID_R1);
  pBtnMan->SetButtonPassive( BTN_ID_L1);
  pBtnMan->SetButtonPassive( BTN_ID_BOXBAR);

  mbLR1Passive= true;



}


void BoxLowBgDraw::SetBoxBarInputEnable(bool benable)
{
  app::tool::ButtonManager* pBtnMan = this->GetButtonManager();
  if(!benable){
    pBtnMan->SetButtonPassive( BTN_ID_BOXBAR, false);
  }
  else{
    pBtnMan->SetButtonActiveForce( BTN_ID_BOXBAR,  false);
  }
}




void BoxLowBgDraw::ResetBoxChangeCursor(void)
{
  if(mbLR1Passive){
    app::tool::ButtonManager* pBtnMan = this->GetButtonManager();
    pBtnMan->SetButtonActiveForce( BTN_ID_BOXBAR);  // kawa_box_modify  リセットでは必ずボタンの状態変更処理を行う。
    pBtnMan->SetButtonActiveForce( BTN_ID_R1 );
    pBtnMan->SetButtonActiveForce( BTN_ID_L1 );
    mbLR1Passive=false;
  }
}


void BoxLowBgDraw::PassiveFolderButton(void)
{
  if(!mbFolderButtonPassive){
    app::tool::ButtonManager* pBtnMan = this->GetButtonManager();
    pBtnMan->SetButtonPassive( BTN_ID_BOXLIST );
    mbFolderButtonPassive= true;
  }
}


void BoxLowBgDraw::ResetFolderButton(void)
{
  if(mbFolderButtonPassive)
  {
    app::tool::ButtonManager* pBtnMan = this->GetButtonManager();
    pBtnMan->SetButtonActiveForce( BTN_ID_BOXLIST );
    mbFolderButtonPassive=false;
  }
}




void BoxLowBgDraw::PassiveRightUnderButton(void)
{
  if(!mbRightUnderButtonPassive)
  {
    app::tool::ButtonManager* pBtnMan = this->GetButtonManager();
    pBtnMan->SetButtonPassive( BTN_ID_Y );
    pBtnMan->SetButtonPassive( BTN_ID_INFO );
    pBtnMan->SetButtonPassive( BTN_ID_JUDGE );
    pBtnMan->SetButtonPassive( BTN_ID_CHAT);  //GFNMCat[1542]
    mbRightUnderButtonPassive=true;
  }
}


void BoxLowBgDraw::ResetRightUnderButton(void)
{
  if(mbRightUnderButtonPassive)
  {
    app::tool::ButtonManager* pBtnMan = this->GetButtonManager();
    if(!mpAppParam->trade.bOn){
      pBtnMan->SetButtonActiveForce( BTN_ID_Y );  // kawa_box_modify  リセットでは必ずボタンの状態変更処理を行う。
    }
    pBtnMan->SetButtonActiveForce( BTN_ID_CHAT);  //GFNMCat[1542]
    pBtnMan->SetButtonActiveForce( BTN_ID_INFO );
    pBtnMan->SetButtonActiveForce( BTN_ID_JUDGE );
    mbRightUnderButtonPassive=false;
  }
}






void BoxLowBgDraw::PassiveUnderButton(void)
{
  app::tool::ButtonManager* pBtnMan = this->GetButtonManager();

  PassiveRightUnderButton();
  PassiveFolderButton();
}


void BoxLowBgDraw::ResetUnderButton(void)
{
  ResetRightUnderButton();
  ResetFolderButton();
}



void BoxLowBgDraw::ResetFunc(void)
{
  ResetLiftPokeIconClear();
  LiftItemIconEnd(0);
  CarryRangeSelectVisible(false);
  mbCarryPokemon = false;
  ResetRangeSelect();
  ResetTemotiTray();
	ResetChatChangeButton();
	ResetBoxChangeCursor();
//  ResetMainCursor();    // @fix niji GFNMCat[254]: 手持ちトレイ動作中のカーソルが表示されるのでコメントアウト
  ResetUpperButton();
  ResetUnderButton();
  ResetFolderButton();
  ResetRangeNum();
  ResetBoxTrayChangeCursor();
  ResetCancelButton();
  StartMainCursor();
  SetBoxBarInputEnable(true);
//  TemotiPokeIconAnimEnd();
  


}



 //GFNMCat[1609]
void BoxLowBgDraw::WarrningMessageBefore(BoxLowBgDrawListener *listener)
{
  PassiveBoxTrayChangeCursor();
  PassiveUnderButton();
  PassiveUpperButton();
  SetUIListener(listener);
  MessageLine2Close();
  PassiveBoxChangeCursor();
  PassiveMainCursor(); //GFNMCat[1941]
  PassiveCancelButton();
  PassiveChatChangeButton();
}

 //GFNMCat[1609]
void BoxLowBgDraw::WarrningMessageAfter(int nowDispTray, int selectTray, int battleTray)
{
  CarryPokeIconRangeEnd( nowDispTray );
  LiftPokeIconEnd(  selectTray, battleTray );
  LiftPokeIconClear();
}





void BoxLowBgDraw::LiftItemIconReady(int page, int pos, int item,bool bTouch )
{
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );

  PassiveMainCursor();
  PassiveUpperButton();
//  PassiveBoxChangeCursor();
  PassiveUnderButton();
  PassiveFolderButton();
  PassiveCancelButton();
  if(pos >= Savedata::BoxPokemon::TRAY_POKE_MAX)
  {
    if(mpBoxAppSetup->GetMoveMode() != BOX_CHANGE_SELECTMODE_SHORTCUT){
      if(!bTouch){
        StartTemotiSelect();
      }
    }
  }

  if(item!=ITEM_DUMMY_DATA){

    gfl2::math::VEC3 vec3;
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

    gfl2::lyt::LytPicture * pPane;
    gfl2::lyt::LytParts* pParts;
    gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );

    gfl2::lyt::LytPane * pPane2 = pLytWk->GetPane(PANENAME_BOX_MIN_LOW_000_PANE_POKESET_00);
    pPane2->SetVisible(true);


    pParts = pLytWk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_30 );  //０からはじめる
    pParts->SetTranslate(mVecMoveCursorBackup);

    pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ITEMICON, &resID );
    pPane->SetVisible(true);

    mpAppParam->pItemIcon->ReplacePaneTexture(pos, item, pPane );


    OHNO_PRINT("位置::ITEMICON %f %f %f\n", vec3.x,	vec3.y, vec3.z);

    pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ITEMICONSHADOW, &resID );

    mpAppParam->pItemIcon->ReplacePaneTexture(pos, item, pPane );
    pPane->SetVisible(false);  //影は最初みせない


  }

}



void BoxLowBgDraw::LiftItemIconStart(int page, int pos, int item, bool bTouch)
{
  gfl2::math::VEC3 vec3;
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );

  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );
  gfl2::lyt::LytPicture * pPane;
  gfl2::lyt::LytParts* pParts;

  pParts = pLytWk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_30 );
  pParts->SetTranslate(mVecMoveCursorBackup);
  pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ITEMICON, &resID );
  pPane->SetVisible(true);

//  vec3=pPane->GetTranslate();
//  vec3.y = -1.0f;
//  pPane->SetTranslate(vec3);

  
 // if(bTouch){
    mpAppParam->pItemIcon->ReplacePaneTexture(pos, item, pPane );
 // }
  //  vec3=pPane->GetTranslate();

  //  pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ICONPARTS_POKESHADOW, &resID );
  pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ITEMICONSHADOW, &resID );

 // if(bTouch){
    mpAppParam->pItemIcon->ReplacePaneTexture(pos, item, pPane );
    //  vec3.x += 3;
    //  vec3.y -= 3;
    //  pPane->SetTranslate(vec3);
  //}
  pPane->SetVisible(true);
	PokeIconActive(page);

}


void BoxLowBgDraw::LiftItemIconAfter(bool bTouch)
{
	gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );

  pLytWk->GetPane(PANENAME_BOX_MIN_LOW_000_PANE_POKESET_00)->SetVisible(true);
//  PokeIconActive(page);
  ResetTemotiTray();
  ResetBoxChangeCursor();
  if(!bTouch){
    ResetCancelButton();
    ResetMainCursor();
    StopMainCursor();
    ResetCursorPos();
  }

}





void BoxLowBgDraw::LiftItemIconEnd(int page)
{

  gfl2::math::VEC3 vec3;
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytParts* pParts;

  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );

  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );
  gfl2::lyt::LytPicture * pPane;

  pParts = pLytWk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_30 );
  pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ITEMICON, &resID );
  pPane->SetVisible(false);
  pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ICONPARTS_POKESHADOW, &resID );
  pPane->SetVisible(false);
  pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ITEMICONSHADOW, &resID );
  pPane->SetVisible(false);

//  ResetFunc();

}



bool BoxLowBgDraw::RangeSelectStart(int inx,int iny)
{
  bool retflg;
  gfl2::lyt::LytPane * pPane2;
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );

  if(iny >= 5){
    pPane2 = pLytWk->GetPane(PANENAME_BOX_MIN_LOW_000_PANE_PLT_SELECTAREA_00);
    pPane2->SetVisible(true);
  }
  else
  {
    pPane2 = pLytWk->GetPane(PANENAME_BOX_MIN_LOW_000_PANE_PLT_SELECTAREA);
    pPane2->SetVisible(true);
  }
  retflg = RangeSelectStretch(inx,iny,inx,iny);
  PassiveUpperButton();
  PassiveUnderButton();
  PassiveFolderButton();
  PassiveBoxChangeCursor();  //追加
  return retflg;
}


void BoxLowBgDraw::ResetRangeSelect(void)
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );

  gfl2::lyt::LytPane * pPane2 = pLytWk->GetPane(PANENAME_BOX_MIN_LOW_000_PANE_PLT_SELECTAREA);
  pPane2->SetVisible(false);
  pPane2 = pLytWk->GetPane(PANENAME_BOX_MIN_LOW_000_PANE_PLT_SELECTAREA_00);
  pPane2->SetVisible(false);


}

void BoxLowBgDraw::RangeSelectEnd(void)
{
  ResetFunc();

}

//枠の範囲を決める関数
//手持ちでスキップ選択している場合trueを帰す
//スキップ選択とは  1 3 5のような場合
bool BoxLowBgDraw::RangeSelectStretch(int inx,int iny,int outx, int outy)
{
  bool retflg = false;
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytParts* pParts;
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );
  gfl2::lyt::LytPicture * pPane;



  if(iny >= 5){  //手持ちトレイの場合
    gfl2::math::VEC3 startvec3;
    gfl2::math::VEC3 endvec3;

    int startx=inx,  endx=outx,  sizex=0,sizey=0;

    if(inx > outx){
      startx=outx;
      endx=inx;
    }

    if( startx==3 && endx==4 ){  //ロジック化できなかった記述
      startx=2;
      endx=5;
    }
    if( startx==1 && endx==2 ){
      startx=0;
      endx=3;
    }
    if( startx==1 && endx==4 ){
      startx=0;
      endx=5;
    }


    gfl2::lyt::LytWindow * pPane2 = pLytWk->GetWindowPane(PANENAME_BOX_MIN_LOW_000_PANE_PLT_SELECTAREA_00);

    gfl2::math::VEC3 vec3 = pPane2->GetTranslate();
    nw::lyt::Size size = pPane2->GetSize();

    vec3.x = -25.0f;

    switch(startx){
    case 0:
      vec3.y = 21.0f - startx * 20.0f;
      break;
    case 1:
      vec3.x += 48.0f;
      vec3.y = 21.0f - startx * 20.0f;
      break;
    case 2:
      vec3.y = 21.0f - startx * 20.0f;
      break;
    case 3:
      vec3.x += 48.0f;
      vec3.y = 21.0f - startx * 20.0f;
      break;
    case 4:
      vec3.y = 21.0f - startx * 20.0f;
      break;
    case 5:
      vec3.x += 48.0f;
      vec3.y = 21.0f - startx * 20.0f;
      break;
    }
    int aida = endx - startx;
    size.width = 50.0f;
    size.height = 40.0f + aida * 20.0f;
    switch(aida){
    case 1:
    case 3:
    case 5:
      size.width += 48.0f;
      break;
    default:
      retflg = true;  //スキップ選択
      break;
    }
    pPane2->SetTranslate(vec3);
    pPane2->SetSize(size);
  }

  else{

    int startx=inx, starty=iny, sizex=0,sizey=0;

    if(inx > outx){
      startx=outx;
    }
    if(iny > outy){
      starty=outy;
    }
    if(inx!=outx){
      sizex = gfl2::math::Abs(inx-outx);
    }
    if(iny!=outy){
      sizey = gfl2::math::Abs(iny-outy);
    }


    gfl2::lyt::LytWindow * pPane2 = pLytWk->GetWindowPane(PANENAME_BOX_MIN_LOW_000_PANE_PLT_SELECTAREA);

    //  幅
    nw::lyt::Size size = pPane2->GetSize();
    gfl2::math::VEC3 vec3 = pPane2->GetTranslate();

    vec3.x = -20.0f + startx * 32.0f; //
    vec3.y = 14.0f + starty * -30.0f;  //@fix 一度調整 2015.12.14

    size.width = 42.0f + sizex * 32.0f;  //
    size.height = 34.0f + sizey * 30.0f;

    pPane2->SetSize(size);

    pPane2->SetTranslate(vec3);
  }
  return retflg;
}





void BoxLowBgDraw::CarryRangeSelectMove(int cursor,gfl2::math::VEC3* pvec)
{
  if(mbCarryPokemon){
    if((cursor == POKEBUTTON_BOX) || (cursor == POKEBUTTON_FOLDER)){
      DispRangeNum(mbCarryNum, pvec);
      CarryRangeSelectVisible(false);
    }
    else{
      DispRangeNum(-1, pvec);
      CarryRangeSelectVisible(true);
    }
  }

}


void BoxLowBgDraw::CarryPokeIconRegist(PokeTool::SimpleParam* pPara,u32 pos,int cursor,App::Box::PokemonIconDraw* pPkd, int painno)
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytParts* pParts;
  gfl2::lyt::LytPicture * pPane;
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );
  gfl2::math::VEC3 vec3;

  OHNO_PRINT("Carry MonstNo %d\n",pPara->monsNo);

  if(pPara->monsNo != MONSNO_NULL){
    pParts = pLytWk->GetPartsPane( painno + cursor );  //
    pParts->SetVisible(true);

    pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ICONPARTS_POKE, &resID );

    pPane->SetVisible(true);

    SetPokeIconColorAndAlpha( pParts, pPkd );   // マテリアルカラーとアルファの変更

//    mpAppParam->pPokeIcon->ReplacePaneTexture(cursor+Savedata::BoxPokemon::TRAY_POKE_MAX+ pml::PokeParty::MAX_MEMBERS , pPara, pPane );
    mpAppParam->pPokeIcon->CopyTextureBuffer( pos, cursor+Savedata::BoxPokemon::TRAY_POKE_MAX+pml::PokeParty::MAX_MEMBERS );
    mpAppParam->pPokeIcon->ReplaceReadTexture( cursor+Savedata::BoxPokemon::TRAY_POKE_MAX+pml::PokeParty::MAX_MEMBERS, pPane );
    vec3=pPane->GetTranslate();

    pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ICONPARTS_POKESHADOW, &resID );
    mpAppParam->pPokeIcon->ReplacePaneTexture(cursor+Savedata::BoxPokemon::TRAY_POKE_MAX+ pml::PokeParty::MAX_MEMBERS , pPane );
    pPane->SetVisible(true);
    vec3.x += 3;
    vec3.y -= 3;
    pPane->SetTranslate(vec3);
  }
}


// ポケモンが持ちあがってる時は、今まで表示していたポケモンを消している
void BoxLowBgDraw::CarryPokeIconPassive(int page, int cursor )
{
  if((mpBoxAppSetup->GetSelectMode() == BOX_CHANGE_EDITTYPE_BATTLE_TEAM) ){
    mpBoxAppSetup->GetBoxSimpleAccesser()->SetPokeIconActive(page,cursor,false);  //GFNMCat[1931]
  }
  else{
    mpBoxAppSetup->GetBoxSimpleAccesser()->SetVisible(page,cursor,false);
    mpBoxAppSetup->GetBoxSimpleAccesser()->SetCarryFlg(page,cursor,true);
  }
}



void BoxLowBgDraw::CarryPokeIconRangeStart(int page, int startx, int starty, int sizex, int sizey, int topcur, bool skipselect)
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytParts* pParts;
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );
  gfl2::lyt::LytPane * pPane2 = pLytWk->GetPane(PANENAME_BOX_MIN_LOW_000_PANE_POKESET_00);
  pPane2->SetVisible(true);

  pParts = pLytWk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_30 );  //
  pParts->SetTranslate(mVecMoveCursorBackup);
  
  mbCarryNum = 0;


  if(starty == 5){  //手持ちの場合

    for(int ypos = 0 ; ypos < sizey; ypos++){
      for(int xpos = 0; xpos < sizex; xpos++){
        int cur = (xpos+startx) + (ypos+starty)*6;
        int regpos = xpos + ypos * 6;

        if(skipselect && (xpos%2==1)){
          continue;
        }
        mpBoxAppSetup->GetBoxSimpleAccesser()->PushPokemonIconDrawData(page, cur, xpos+startx, ypos+starty);//backup

        PokeTool::SimpleParam* pPara = mpBoxAppSetup->GetBoxSimpleAccesser()->GetSimpleData(page, cur);
        App::Box::PokemonIconDraw* pPkd = mpBoxAppSetup->GetBoxSimpleAccesser()->GetPokemonIconDrawData(page, cur);

        int buff[6]={0,1,6,7,12,13};
        if(pPara->monsNo != MONSNO_NULL){
          mbCarryNum++;
        }
        CarryPokeIconRegist(pPara, cur, buff[regpos], pPkd, PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_30);  //表示をする
        CarryPokeIconPassive(page,cur);  //けす
      }
    }


  }
  else{
    for(int ypos = 0 ; ypos < sizey; ypos++){
      for(int xpos = 0; xpos < sizex; xpos++){
        int cur = (xpos+startx) + (ypos+starty)*6;
        int regpos = xpos + ypos * 6;
        mpBoxAppSetup->GetBoxSimpleAccesser()->PushPokemonIconDrawData(page, cur, xpos+startx, ypos+starty);//backup

        PokeTool::SimpleParam* pPara = mpBoxAppSetup->GetBoxSimpleAccesser()->GetSimpleData(page, cur);
        App::Box::PokemonIconDraw* pPkd = mpBoxAppSetup->GetBoxSimpleAccesser()->GetPokemonIconDrawData(page, cur);

        if(pPara->monsNo != MONSNO_NULL){
          mbCarryNum++;
        }
        CarryPokeIconRegist(pPara, cur, regpos, pPkd, PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_30);  //表示をする
        CarryPokeIconPassive(page,cur);  //けす
      }
    }
  }

  ResetRangeSelect();

  StopMainCursor();   // 範囲選択時カーソルが動いたままだったので追加

  mbCarryPokemon = true;

}


void BoxLowBgDraw::CarryPokeIconRangeEnd(int page)
{
  mbCarryPokemon = false;

  mpBoxAppSetup->GetBoxSimpleAccesser()->RefreshPokemonIconDrawData();  //現状復帰

 // ResetFunc();
}



void BoxLowBgDraw::CarryRangeSelectVisible(bool bVisible )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );
  gfl2::lyt::LytPane * pPane2 = pLytWk->GetPane(PANENAME_BOX_MIN_LOW_000_PANE_POKESET_00);

  pPane2->SetVisible(bVisible);
}


//複数選択のときのカーソルがフォルダーやボックス名と重なった時の数字表示
void BoxLowBgDraw::DispRangeNum(int no ,gfl2::math::VEC3* pvec)
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytParts* pParts;
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );
  gfl2::lyt::LytTextBox * pPaneText;

  if(-1==no){
    ResetRangeNum();
  }
  else{
    pG2DUtil->SetRegisterNumber( 0, no, 2 );
    pG2DUtil->GetStringExpand( mpTempStrBuf, PARAM__VALUE_2 );
    pPaneText = pLytWk->GetTextBoxPane( PANENAME_BOX_MIN_LOW_000_PANE_TEXTBOX_02);
    pPaneText->SetVisible( true );
    pG2DUtil->SetTextBoxPaneString( pPaneText, mpTempStrBuf );
    pG2DUtil->SetPaneVisible( LYT_WORK_NORMAL, PANENAME_BOX_MIN_LOW_000_PANE_PLT_NUMBER, true );

    gfl2::lyt::LytPane * pPane2 = pLytWk->GetPane(PANENAME_BOX_MIN_LOW_000_PANE_PLT_NUMBER);

    gfl2::math::VEC3 vec = pPane2->GetTranslate();
    vec.x =  pvec->x;
    vec.y =  pvec->y+10;//GFNMCat5486
    pPane2->SetTranslate(vec);
  }


}


void BoxLowBgDraw::ResetRangeNum(void )
{

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  pG2DUtil->SetPaneVisible( LYT_WORK_NORMAL, PANENAME_BOX_MIN_LOW_000_PANE_PLT_NUMBER, false );
}




void BoxLowBgDraw::ResetL2R2Key(void)
{
  app::tool::ButtonManager* pBtnMan = this->GetButtonManager();

  if((mpBoxAppSetup->GetSelectMode() == BOX_CHANGE_EDITTYPE_BATTLE_TEAM) ||
     mpAppParam->type == BOX_CALL_RESORT_HOTSPA ||
     mpAppParam->type == BOX_CALL_RESORT_GIM ){

    pBtnMan->SetButtonVisible(BTN_ID_L2);
    pBtnMan->SetButtonVisible(BTN_ID_R2);

    //    GetG2DUtil()->SetPaneVisible(LYT_WORK_NORMAL,  PANENAME_BOX_MIN_LOW_000_PANE_CURSOR_L2,true);
    //  GetG2DUtil()->SetPaneVisible(LYT_WORK_NORMAL,  PANENAME_BOX_MIN_LOW_000_PANE_CURSOR_R2,true);
  }
  else{
    pBtnMan->SetButtonInvisible(BTN_ID_L2);
    pBtnMan->SetButtonInvisible(BTN_ID_R2);

    //    GetG2DUtil()->SetPaneVisible(LYT_WORK_NORMAL,  PANENAME_BOX_MIN_LOW_000_PANE_CURSOR_L2,false);
    //  GetG2DUtil()->SetPaneVisible(LYT_WORK_NORMAL,  PANENAME_BOX_MIN_LOW_000_PANE_CURSOR_R2,false);
  }
}


void BoxLowBgDraw::VisibleL2R2Key(bool bvisible)
{
  app::tool::ButtonManager* pBtnMan = this->GetButtonManager();

  //	GetG2DUtil()->SetPaneVisible(0,  PANENAME_BOX_MIN_LOW_000_PANE_CURSOR_L2,bvisible);
  //  GetG2DUtil()->SetPaneVisible(0,  PANENAME_BOX_MIN_LOW_000_PANE_CURSOR_R2,bvisible);

  if(bvisible){
    pBtnMan->SetButtonVisible(BTN_ID_L2);
    pBtnMan->SetButtonVisible(BTN_ID_R2);
  }
  else{
    pBtnMan->SetButtonInvisible(BTN_ID_L2);
    pBtnMan->SetButtonInvisible(BTN_ID_R2);
  }

}




//------------------------------------------------------------------
/**
 * @brief	  バトルと例変更
 */
//------------------------------------------------------------------
void BoxLowBgDraw::DrawPokeIconBattle(int traypage)
{
  int i;
  for(i = Savedata::BoxPokemon::TRAY_POKE_MAX ; i < Savedata::BoxPokemon::TRAY_POKE_MAX + pml::PokeParty::MAX_MEMBERS;i++){
    mpBoxAppSetup->GetBoxSimpleAccesser()->SetPokeIconDrawAll(traypage,i);
  }
}





//------------------------------------------------------------------
/**
 * @brief	  ポケモンアイコンALL
 */
//------------------------------------------------------------------
void BoxLowBgDraw::DrawPokeIconAll(int page,int traypage)
{
  int i;
  for(i=0;i<Savedata::BoxPokemon::TRAY_POKE_MAX+pml::PokeParty::MAX_MEMBERS;i++){
    if(i<Savedata::BoxPokemon::TRAY_POKE_MAX)
    {
      mpBoxAppSetup->GetBoxSimpleAccesser()->SetPokeIconDrawAll(page,i);
      _pokeIconAllOff(i);
    }
    else
    {
      mpBoxAppSetup->GetBoxSimpleAccesser()->SetPokeIconDrawAll(traypage,i);
      _pokeIconAllOff(i);
    }
  }

}



//------------------------------------------------------------------
/**
 * @brief	  ポケモンアイコンstateのみ更新
 */
//------------------------------------------------------------------
void BoxLowBgDraw::DrawPokeIconStateAll(int page,int traypage)
{
  int i;
  for(i=0;i<Savedata::BoxPokemon::TRAY_POKE_MAX+pml::PokeParty::MAX_MEMBERS;i++){
    if(i<Savedata::BoxPokemon::TRAY_POKE_MAX)
    {
      mpBoxAppSetup->GetBoxSimpleAccesser()->SetPokeIconDrawState(page,i);
    }
    else
    {
      mpBoxAppSetup->GetBoxSimpleAccesser()->SetPokeIconDrawState(traypage,i);
    }
  }

}


//------------------------------------------------------------------
/**
 * @brief	  ポケモンアイコンフラグがたってるものだけ描画
 */
//------------------------------------------------------------------
void BoxLowBgDraw::DrawPokeFunc(int page,int traypage)
{


  int i;
  for(i=0;i<Savedata::BoxPokemon::TRAY_POKE_MAX+pml::PokeParty::MAX_MEMBERS;i++){
    if(i<Savedata::BoxPokemon::TRAY_POKE_MAX)
    {
      DrawPokeIcon3(i,	mpBoxAppSetup->GetBoxSimpleAccesser()->GetPokemonIconDrawData(page,i) );
    }
    else
    {
      DrawPokeIcon3(i,	mpBoxAppSetup->GetBoxSimpleAccesser()->GetPokemonIconDrawData(traypage,i) );
    }
  }
}



/*
void BoxLowBgDraw::RedrawTemochi(void)
{
//  gfl2::lyt::LytParts* pParts = GetIconParts(30);

 // mpBoxAppSetup->GetBoxSimpleAccesser()->TumeruTemotiSimpleData();
}
 */

///-----------------------------低レベル関数


void BoxLowBgDraw::_pokeIconAllOff(int pos)
{
  PokeIconVisible(pos,false);
  PokeIconTeam(pos,false);
  PokeIconLock(pos,false);
}


void BoxLowBgDraw::PokeIconVisible(int pos, bool visible)
{
  gfl2::lyt::LytParts* pParts;
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );

  pParts = GetIconParts(pos);
  gfl2::lyt::LytPicture * pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ICONPARTS_POKE, &resID );
  pPane->SetVisible(visible);

  pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ICONPARTS_POKESHADOW, &resID );
  pPane->SetVisible(false);

}


void BoxLowBgDraw::PokeIconTeam(int pos, bool visible)
{
  gfl2::lyt::LytParts* pParts;
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );

  pParts = GetIconParts(pos);
  gfl2::lyt::LytPicture * pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ICONPARTS_TEAM, &resID );
  pPane->SetVisible(visible);
}

void BoxLowBgDraw::PokeIconLock(int pos, bool visible)
{
  gfl2::lyt::LytParts* pParts;
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );

  pParts = GetIconParts(pos);
  gfl2::lyt::LytPicture * pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ICONPARTS_LOCK, &resID );
  pPane->SetVisible(visible);
}


void BoxLowBgDraw::SetMenuCursor(app::tool::MenuCursor* pMenuCursor)
{
  //mpMenuCursor = pMenuCursor;
  //  app::util::G2DUtil * g2d = GetG2DUtil();

  //  mpMenuCursor->CreateLayoutWork(
  //  g2d->GetLayoutSystem(),
  //  g2d->GetLayoutWorkSetup( gfl2::lyt::DISPLAY_LOWER ),
  // g2d->GetAppLytAccessor(),
  // app::tool::MenuCursor::SIZE_SMALL,
  // false );

  //  mpMenuCursor->StartAnime();

}



void BoxLowBgDraw::localBgChange(int wpNo, int partlabel, int bglabel)
{
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );
  gfl2::lyt::LytPane* pane1 = pLytWk->GetPane(bglabel);
  gfl2::lyt::LytParts* pParts = pLytWk->GetPartsPane( partlabel );
  gfl2::lyt::LytPane * pane2 = pLytWk->GetPicturePane( pParts, PANENAME_BOX_BTN_LOW_006_PANE_BG_MAIN, &resID );
  pG2DUtil->SetTexMap( pane2 , LYT_WORK_NORMAL, LYTRES_MINLOW_BOX_NAME01_BFLIM + wpNo );
  pG2DUtil->SetTexMap( pane1 , LYT_WORK_NORMAL, LYTRES_MINLOW_BOX_WP01_BFLIM + wpNo );


}



void BoxLowBgDraw::BgChange(int wpNo)
{
  localBgChange(wpNo,PANENAME_BOX_MIN_LOW_000_PANE_BTN_BOXCATEGORY_00,PANENAME_BOX_MIN_LOW_000_PANE_WALLPAPER_00);
}


void BoxLowBgDraw::BeforeBgChange(int wpNo)
{
  localBgChange(wpNo,PANENAME_BOX_MIN_LOW_000_PANE_BTN_BOXCATEGORY_01,PANENAME_BOX_MIN_LOW_000_PANE_WALLPAPER_01);

}





void BoxLowBgDraw::CursorChange(bool byoko )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytParts* pParts;
  gfl2::lyt::LytPane * pPane;
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );

  pParts = pLytWk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_CURSOR );  //
  pPane = pLytWk->GetPane( pParts, PANENAME_BOX_CUR_LOW_000_PANE_BOX_CURSOR_00, &resID );
  pPane->SetVisible(byoko);
  pPane = pLytWk->GetPane( pParts, PANENAME_BOX_CUR_LOW_000_PANE_BOX_CURSOR_01, &resID );
  pPane->SetVisible(!byoko);

}


//--------------------------------------------------------------------------------------------
/**
 * @brief		壁紙変更設定
 *
 * @param		id		ペインテーブルID
 * @param		wpNo	壁紙番号
 * @param		mv		変更方向（インorアウト）
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BoxLowBgDraw::InitChangeWallPaper( s32 mv )
{
  wpChgCount = 0;
  wpChgMove  = mv;
}

static const s32 WP_CHG_COUNT = 16;										//!< 壁紙変更カウント
static const s32 WP_CHG_SPEED = 256 / WP_CHG_COUNT;		//!< 壁紙変更速度


//--------------------------------------------------------------------------------------------
/**
 * @brief		壁紙ホワイトフェード処理
 *
 * @param		none
 *
 * @retval	"true = 処理中"
 * @retval	"false = それ以外"
 */
//--------------------------------------------------------------------------------------------
bool BoxLowBgDraw::ChangeWallpaperColor(void)
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );
  gfl2::lyt::LytParts* pParts = pLytWk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_BTN_BOXCATEGORY_00 );
  gfl2::lyt::LytPane * pane2 = pLytWk->GetPicturePane( pParts, PANENAME_BOX_BTN_LOW_006_PANE_BG_MAIN, &resID );
  gfl2::lyt::LytPane* pane1 = pLytWk->GetPane(PANENAME_BOX_MIN_LOW_000_PANE_WALLPAPER_00);
  nw::ut::Color8 color;

  if( wpChgCount == WP_CHG_COUNT ){

    if( wpChgMove > 0 ){
      color.Set( 255, 255, 255, 0 );
    }else{
      color.Set( 0, 0, 0, 0 );
    }
    pG2DUtil->SetMaterialColor(pane2, app::util::G2DUtil::COLOR_INDEX_BLACK, color );
    pG2DUtil->SetMaterialColor(pane1, app::util::G2DUtil::COLOR_INDEX_BLACK, color );

    return false;
  }else{
    color = pG2DUtil->GetMaterialColor(pane2, app::util::G2DUtil::COLOR_INDEX_BLACK );
    if( wpChgMove < 0 ){
      if( color.r <= WP_CHG_SPEED ){
        color.r = 0;
      }else{
        color.r -= WP_CHG_SPEED;
      }
    }else{
      if( color.r >= (255-WP_CHG_SPEED) ){
        color.r = 255;
      }else{
        color.r += WP_CHG_SPEED;
      }
    }
    color.g = color.r;
    color.b = color.r;
    pG2DUtil->SetMaterialColor(pane2, app::util::G2DUtil::COLOR_INDEX_BLACK, color );
    pG2DUtil->SetMaterialColor(pane1, app::util::G2DUtil::COLOR_INDEX_BLACK, color );
    wpChgCount++;
  }

  return true;
}



//--------------------------------------------------------------------------------------------
/**
 * @brief		メインシーケンス：壁紙変更
 *
 * @param		none
 *
 * @return	次のシーケンス
 *
 *	定義：MAINSEQ_WALLPAPER_CHANGE
 */
//--------------------------------------------------------------------------------------------
bool BoxLowBgDraw::WallPaperChange(int newno)
{

  switch( mAnimSeq ){
  case 0:
    InitChangeWallPaper( 1 );
    mAnimSeq++;

  case 1:
    // ホワイトアウト
    if( ChangeWallpaperColor() == false ){
      mAnimSeq++;
    }
    break;

  case 2:
    // テクスチャ差し替え
    BgChange(newno);
    InitChangeWallPaper( -1 );
    mAnimSeq++;
    break;

  case 3:
    // ホワイトイン
    if( ChangeWallpaperColor() == false ){
      mAnimSeq = 0;
      return true;
    }
    break;
  }

  return false;
}


void BoxLowBgDraw::ActiveRangeButton(bool bactive)
{
  mbActiveRangeButton = bactive;

  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytParts* pParts = pLytWk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_BTN_MOVETYPE_02 );

  pParts->SetVisible(bactive);

}


void BoxLowBgDraw::ChangeItemAnimStart(int selectpage,int selectpos, int nowpage,int nowpos,IconPutArrow put_arrow)
{
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );

  gfl2::lyt::LytPane * global_pane = NULL;

  switch( put_arrow )
  {
  case ICON_PUT_ARROW_NONE:
    global_pane = GetIconParts( nowpos );
    break;
  case ICON_PUT_ARROW_LEFT:
    global_pane = pLytWk->GetPane( PANENAME_BOX_MIN_LOW_000_PANE_CURSOR_L1 );
    break;
  case ICON_PUT_ARROW_RIGHT:
    global_pane = pLytWk->GetPane( PANENAME_BOX_MIN_LOW_000_PANE_CURSOR_R1 );
    break;
  }
  mpBoxAppSetup->GetBoxSimpleAccesser()->SetLandingTypePain( pLytWk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_30 ),global_pane,selectpage, selectpos,  nowpage, nowpos );
}



void BoxLowBgDraw::ChangeItemIcon(int pos, int itemno)
{

	gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::math::VEC3 vec3;
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytPicture * pPane;
  gfl2::lyt::LytParts* pParts;
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );

  gfl2::lyt::LytPane * pPane2 = pLytWk->GetPane(PANENAME_BOX_MIN_LOW_000_PANE_POKESET_00);
  pPane2->SetVisible(true);

  pParts = pLytWk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_30 );  //０からはじめる

  pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ITEMICON, &resID );
  pPane->SetVisible(true);

  mpAppParam->pItemIcon->ReplacePaneTexture(pos, itemno, pPane );

  pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ITEMICONSHADOW, &resID );

  mpAppParam->pItemIcon->ReplacePaneTexture(pos, itemno, pPane );
  pPane->SetVisible(false);  //影は最初みせない

}







void BoxLowBgDraw::ChangePokemonAnimStart(int selectpage,int selectpos, int nowpage,int nowpos,IconPutArrow put_arrow)
{
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );


  if((nowpage!=selectpage) || (nowpos!=selectpos)){  //座標が異なる時に表示 GFNMCat[2217]
    gfl2::lyt::LytPane * global_pane = NULL;

    switch( put_arrow )
    {
    case ICON_PUT_ARROW_NONE:
      global_pane = GetIconParts( selectpos );
      break;
    case ICON_PUT_ARROW_LEFT:
      global_pane = pLytWk->GetPane( PANENAME_BOX_MIN_LOW_000_PANE_CURSOR_L1 );
      break;
    case ICON_PUT_ARROW_RIGHT:
      global_pane = pLytWk->GetPane( PANENAME_BOX_MIN_LOW_000_PANE_CURSOR_R1 );
      break;
    }

    mpBoxAppSetup->GetBoxSimpleAccesser()->SetMoveTypePain( MovePokeIconReady(nowpage, nowpos), global_pane, nowpage, nowpos,  selectpage, selectpos ,GetIconParts(nowpos));
  }

  mpBoxAppSetup->GetBoxSimpleAccesser()->SetLandingTypePain( pLytWk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_30 ),GetIconParts(nowpos),nowpage, nowpos,  nowpage, nowpos );
}










void BoxLowBgDraw::AllPassiveStart(void)
{
  PassiveChatChangeButton();
  PassiveCancelButton();
  PassiveMainCursor();
  PassiveUpperButton();
  PassiveBoxChangeCursor();
  PassiveUnderButton();
  PassiveFolderButton();
  PassiveBoxTrayChangeCursor();
}

void BoxLowBgDraw::AllPassiveEnd(void)
{
  ResetChatChangeButton();
  ResetCancelButton();
  ResetMainCursor();
  ResetUpperButton();
  ResetBoxChangeCursor();
  ResetUnderButton();
  ResetFolderButton();
  ResetBoxTrayChangeCursor();
}


//-----------------------------------------------------------------------------
/**
 * @brief   アニメ終了チェック
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BoxLowBgDraw::IsAnimeEnd( u32 anm )
{
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  if( pLytWk->IsBindAnime(anm) )
  {
    return pLytWk->IsAnimeEnd( anm );
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @brief   手持ちのトレイが動作中か
 * @retval  true  = 動作中
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BoxLowBgDraw::IsTemochiFrameMove(void)
{
  /*
    @note
      GFNMCat[254]の修正のために追加したので、左=>右への移動しかみていない。
      必要なら反対方向も追加してください。 2016/03/11 by nakahiro
  */
  if( IsAnimeEnd( LA_MINLOW_BOX_MIN_LOW_000_TEAM_SET_03 ) )
  {
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @brief   チームのフレームが動作中か
 * @retval  true  = 動作中
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BoxLowBgDraw::IsTeamFrameMove(void)
{
  if( IsAnimeEnd( LA_MINLOW_BOX_MIN_LOW_000_TEAM_SET_00 ) &&
      IsAnimeEnd( LA_MINLOW_BOX_MIN_LOW_000_TEAM_SET_01 ) )
  {
    return false;
  }
  return true;
}


//-----------------------------------------------------------------------------
/**
 * @brief   ページ切り替えのアニメが動作中か
 * @retval  true  = 動作中
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BoxLowBgDraw::IsPageFrameMove(void)
{
  return mbPageChangeAnim;
}


//-----------------------------------------------------------------------------
/**
 * @brief つかみモード遷移時に初期座標をセットする
 */
//-----------------------------------------------------------------------------
void BoxLowBgDraw::InitTouchPos(void)
{
  gfl2::ui::TouchPanel * tp = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetTouchPanel(gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD);
  if( tp->IsTouch() != false )
  {
    u16 x, y;
    tp->GetXY( &x, &y );
    mMouseX = static_cast<int>( x );
    mMouseY = static_cast<int>( y );

    SetCatchPokeIconTranslate();  // 座標を入れないと反映されない
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief つかみモード時のポケモンアイコン移動
 */
//-----------------------------------------------------------------------------
void BoxLowBgDraw::SetCatchPokeIconTranslate(void)
{
  gfl2::math::VEC3 vec3;
  vec3.x=mMouseX;
  vec3.y=mMouseY;

  vec3.x = vec3.x - 160 + 78 - 78;
  vec3.y = -vec3.y +120 - 61 + 51;

  LiftPokeIconMove( &vec3 );  //x+78 y-51
}


//-----------------------------------------------------------------------------
/**
 * @brief   逃がすアニメ
 * @param   pos   位置
 * @retval  true  = 処理中
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BoxLowBgDraw::PokeFreeAnime( int pos )
{
  static const f32 POKEICON_FREE_OUT_VALUE = 0.05f;

  bool result = true;

  gfl2::lyt::LytParts * pane = GetIconParts( pos );
  gfl2::math::VEC2 scale = pane->GetScale();

  scale.x -= POKEICON_FREE_OUT_VALUE;
  scale.y -= POKEICON_FREE_OUT_VALUE;

  if( scale.x < 0.0f || scale.y < 0.0f )
  {
    scale.x = 1.0f;
    scale.y = 1.0f;
    result = false;
  }
  pane->SetScale( scale );

  return result;
}




//--------------------------------------------------------------------------------------------
/**
 * @brief   現在のトレイと切り替え先トレイとで距離の近い方向を取得
 *
 * @param   now   現在のトレイ
 * @param   chg   切り替え先のトレイ
 *
 * @return  切り替え方向
 */
//--------------------------------------------------------------------------------------------
int BoxLowBgDraw::GetNearScrollInterval( int now, int chg )
{
  int max = static_cast<int>( mpAppParam->pBoxData->GetTrayMax() );
  int l = GetScrollInterval( now, chg, max, -1 );
  int r = GetScrollInterval( now, chg, max, 1 );
  if( l > r )
  {
    return 1;
  }
  return -1;
}

int BoxLowBgDraw::GetScrollInterval( int start, int end, int max, int mv )
{
  int tmp = start;
  int i;
  for( i=0; i<=max; i++ )
  {
    if( tmp == end )
    {
      break;
    }
    tmp += mv;
    if( tmp < 0 )
    {
      tmp = max;
    }
    else if( tmp > max )
    {
      tmp = 0;
    }
  }
  return i;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   配置先のトレイ切り替え矢印を取得
 *
 * @param   now_tray  現在のトレイ
 * @param   get_tray  取得したトレイ
 * @param   get_pos   取得した位置
 *
 * @return  トレイ切り替え矢印
 */
//--------------------------------------------------------------------------------------------
BoxLowBgDraw::IconPutArrow BoxLowBgDraw::GetIconPutArrow( int now_tray, int get_tray , int get_pos )
{
  BoxLowBgDraw::IconPutArrow arraw = BoxLowBgDraw::ICON_PUT_ARROW_NONE;

  if( get_pos < Savedata::BoxPokemon::TRAY_POKE_MAX )
  {
    if( now_tray != get_tray )
    {
      if( GetNearScrollInterval( now_tray, get_tray ) == 1 )
      {
        arraw = BoxLowBgDraw::ICON_PUT_ARROW_RIGHT;
      }
      else
      {
        arraw = BoxLowBgDraw::ICON_PUT_ARROW_LEFT;
      }
    }
  }

  return arraw;
}





void BoxLowBgDraw::TemotiTumeruPokeIconRegist(PokeTool::SimpleParam* pPara,u32 pos,int cursor,App::Box::PokemonIconDraw* pPkd, int painno)
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytParts* pParts;
  gfl2::lyt::LytPicture * pPane;
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );
  gfl2::math::VEC3 vec3;


  if(pPara->monsNo != MONSNO_NULL){
    pParts = pLytWk->GetPartsPane( painno + cursor );  //
    pParts->SetVisible(true);

    pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ICONPARTS_POKE, &resID );

    pPane->SetVisible(true);

    SetPokeIconColorAndAlpha( pParts, pPkd );   // マテリアルカラーとアルファの変更

    mpAppParam->pPokeIcon->CopyTextureBuffer( pos, cursor+Savedata::BoxPokemon::TRAY_POKE_MAX+pml::PokeParty::MAX_MEMBERS );
    mpAppParam->pPokeIcon->ReplaceReadTexture( cursor+Savedata::BoxPokemon::TRAY_POKE_MAX+pml::PokeParty::MAX_MEMBERS, pPane );
    vec3=pPane->GetTranslate();

    pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ICONPARTS_POKESHADOW, &resID );
    pPane->SetVisible(false);
  }
}





// 1908の対応の為に必要  手持ちアニメーション 

void BoxLowBgDraw::TemotiPokeIconAnimStart(void)
{
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytPane * pPane2 = pLytWk->GetPane(PANENAME_BOX_MIN_LOW_000_PANE_POKESET_02);
  pPane2->SetVisible(true);

  {
    gfl2::math::VEC3 vec = pPane2->GetTranslate();
    vec.x =  80;
    vec.y =  40;
    pPane2->SetTranslate(vec);
  }

  for(int cur = Savedata::BoxPokemon::TRAY_POKE_MAX ; cur < (Savedata::BoxPokemon::TRAY_POKE_MAX+ pml::PokeParty::MAX_MEMBERS); cur++){
    gfl2::lyt::LytPane * pPane3 = GetTemotiIconParts(cur);
    pPane3->SetVisible(false);
  }

}


// 1908の対応の為に必要  手持ちアニメーション 

void BoxLowBgDraw::TemotiIconPartsReady(int page,int pos)
{
  int cur = pos;

  {

    PokeTool::SimpleParam* pPara = mpBoxAppSetup->GetBoxSimpleAccesser()->GetSimpleData(page, cur);
    App::Box::PokemonIconDraw* pPkd = mpBoxAppSetup->GetBoxSimpleAccesser()->GetPokemonIconDrawData(page, cur);

    {
      TemotiTumeruPokeIconRegist(pPara, cur, cur-Savedata::BoxPokemon::TRAY_POKE_MAX , pPkd, PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_102);  //表示をする
      CarryPokeIconPassive(page,cur);  //今のものはけす
    }
  }
}




void BoxLowBgDraw::TemotiPokeIconAnimEnd(void)
{
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytPane * pPane2 = pLytWk->GetPane(PANENAME_BOX_MIN_LOW_000_PANE_POKESET_02);
  pPane2->SetVisible(false);
  
}

//----------------------------------------------------------------------
/**
 * @brief 手持ちの部品を得る
 * @param heap  ヒープ
 */
//----------------------------------------------------------------------

gfl2::lyt::LytParts* BoxLowBgDraw::GetTemotiIconParts(int pos)
{
  gfl2::lyt::LytParts* pParts;
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  pParts = pLytWk->GetPartsPane( PANENAME_BOX_MIN_LOW_000_PANE_ICON_POKE_102 + pos  - Savedata::BoxPokemon::TRAY_POKE_MAX);
  return pParts;
}




GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )

