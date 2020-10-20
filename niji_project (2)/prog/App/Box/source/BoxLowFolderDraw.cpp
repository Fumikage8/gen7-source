//==============================================================================
/**
 * @file	  BoxLowFolderDraw.cpp
 * @brief	  フォルダー下画面描画
 * @author	ohno
 * @data	  2015.08.15
 */
// =============================================================================

#include "BoxLowFolderDraw.h"

//素材
#include <arc_def_index/arc_def.h>
#include <arc_index/boxLyt.gaix>

#include <niji_conv_header/app/box/res2d/fldlow.h>
#include <niji_conv_header/app/box/res2d/fldlow_anim_list.h>
#include <niji_conv_header/app/box/res2d/fldlow_pane.h>
#include <niji_conv_header/app/box/res2d/bgrlow.h>
#include <niji_conv_header/app/box/res2d/bgrlow_anim_list.h>
#include <niji_conv_header/app/box/res2d/bgrlow_pane.h>

#include "arc_index/message.gaix"
#include <niji_conv_header/message/msg_box.h>

#include <Layout/include/gfl2_LayoutRenderPath.h>

#include "SaveData/include/BoxSave.h"
#include "SaveData/include/BoxPokemonSave.h"
#include "BoxSimpleAccesser.h"
#include "App/Box/include/BoxAppSetup.h"
#include "AppLib/include/Tool/PokeIcon.h"
#include "Sound/include/Sound.h"

#include "PokeTool/include/PokeTool.h"


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )

enum{
  CUR_FOLDER_BUTTON,
  CUR_FOLDER_00,
  CUR_FOLDER_01,
  CUR_FOLDER_02,
  CUR_FOLDER_03,
  CUR_FOLDER_04,
  CUR_FOLDER_05,
  CUR_FOLDER_06,
  CUR_FOLDER_07,
  CUR_FOLDER_08,
  CUR_FOLDER_09,
  CUR_FOLDER_10,
  CUR_FOLDER_11,
  CUR_FOLDER_12,
  CUR_FOLDER_13,
  CUR_FOLDER_14,
  CUR_FOLDER_15,
  CUR_FOLDER_16,
  CUR_FOLDER_17,
  CUR_FOLDER_18,
  CUR_FOLDER_19,
  CUR_FOLDER_20,
  CUR_FOLDER_21,
  CUR_FOLDER_22,
  CUR_FOLDER_23,
  CUR_FOLDER_24,
  CUR_FOLDER_25,
  CUR_FOLDER_26,
  CUR_FOLDER_27,
  CUR_FOLDER_28,
  CUR_FOLDER_29,
  CUR_FOLDER_30,
  CUR_FOLDER_31,

  CUR_FOLDER_0123,
  CUR_FOLDER_012345,
  CUR_FOLDER_0_7,
  CUR_FOLDER_4567,
  CUR_FOLDER_12_13_14_15,
  CUR_FOLDER_18_23,
  CUR_FOLDER_24_31,
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
BoxLowFolderDraw::BoxLowFolderDraw( app::util::Heap * heap, app::util::AppRenderingManager* pAppRenderingManager ) :
app::ui::UIView( heap, heap->GetSystemAllocator(), heap->GetDeviceAllocator() )
,mpAppHeap( heap )
,m_uiListener( NULL )
{
  mpAppRenderingManager = pAppRenderingManager;

}

/// デストラクタ
BoxLowFolderDraw::~BoxLowFolderDraw( void )
{
}

//----------------------------------------------------------------------
/**
 *  @brief  初期化
 */
//----------------------------------------------------------------------

void BoxLowFolderDraw::Init(APP_PARAM* pAppParam, void* pAppLytBuff, BoxAppSetup* pBoxAppSetup)
{
  mbCarryNum=0;
  mpAppParam = pAppParam;
  mpBoxAppSetup = pBoxAppSetup;
  mpMsgData = mpAppParam->pMsgData;

  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( mpAppHeap, false );

  Initialize2D( mpAppHeap,pAppLytBuff );

  app::util::G2DUtil * pG2DUtil = UIView::GetG2DUtil();
  pG2DUtil->StartAnime( LYT_WORK_NORMAL, LA_FLDLOW_BOX_FLD_LOW_000_PATERN_00 + mpAppParam->pBoxData->GetTrayLevel() );
  pG2DUtil->StartAnime( LYT_WORK_BG, LA_BGRLOW_BOX_BGR_LOW_000_BG_KEEP );
  pG2DUtil->StartAnime( LYT_WORK_NORMAL,  LA_FLDLOW_BOX_FLD_LOW_000__CURSOR_KEEP );

  if(mpBoxAppSetup->GetBoxSimpleAccesser()->IsBoxSearchMode() ){
    pG2DUtil->StartAnime(  LYT_WORK_NORMAL,  LA_FLDLOW_BOX_FLD_LOW_000__BTN_SEARCH_SEARCH_ON );//GFNMCat[1970]
  }


  InitializeText(mpAppHeap);

//  SetInputListener( this );

  mbDrawOn=false;

}

//----------------------------------------------------------------------
/**
 *  @brief  終了処理
 */
//----------------------------------------------------------------------

bool BoxLowFolderDraw::End(void)
{
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
void BoxLowFolderDraw::SetUIListener( BoxLowFolderDrawListener *listener )
{
  m_uiListener = listener;
  if(listener==NULL){
    SetInputListener( NULL );
  }
  else{
    SetInputEnabled(listener->GetInputEnableFlg());
    SetInputListener( this );
  }

}

//--------------------------------------------------------------------------
/**
 * @brief   更新
 */
//--------------------------------------------------------------------------
void BoxLowFolderDraw::Update(void)
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();



  if(FOLDER_MOUSE_MODE_POKEMON_DRAG==mMousePokemonMode){
    gfl2::math::VEC3 vec3;
    vec3.x = mMoveX;
    vec3.y = mMoveY;
    vec3.x = vec3.x - 160 - 10;
    vec3.y = -vec3.y +120 + 10;

    LiftPokeIconMove( &vec3 );  //x+10 y-10
    DispRangeNum( &vec3 );
  }
  else if(mMousePokemonMode == FOLDER_MOUSE_MODE_MOVE || mMousePokemonMode == FOLDER_MOUSE_MODE_TUKAMI){
    FolderIconMove();
  }
  else{
    FolderIconDelete();
  }



  pG2DUtil->PrintMessage();
  app::ui::UIView::Update();
}

//--------------------------------------------------------------------------
/**
 * @brief   描画
 */
//--------------------------------------------------------------------------
void BoxLowFolderDraw::Draw(gfl2::gfx::CtrDisplayNo no)
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  pG2DUtil->AddDrawableLytWkForOneFrame( mpAppRenderingManager, no, LYT_WORK_BG );
  pG2DUtil->AddDrawableLytWkForOneFrame( mpAppRenderingManager, no, LYT_WORK_NORMAL );

mbDrawOn=true;
}


//--------------------------------------------------------------------------
/**
 * @brief   破棄チェック
 * @return  trueで破棄完了
 */
//--------------------------------------------------------------------------
bool BoxLowFolderDraw::IsExit( void )
{
  return true;
}


// カーソルモード
int BoxLowFolderDraw::TouchFolderHit(int mode,u16 x, u16 y)
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  gfl2::lyt::LytMultiResID res_id = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );
  gfl2::lyt::LytWk * lytwk = pG2DUtil->GetLayoutWork( LYT_WORK_NORMAL );

  for( u32 i=0; i< Savedata::Box::TRAY_MAX+1; i++ )
  {
    gfl2::lyt::LytParts* pParts = GetFolderPane( i );
    if(pParts){
      gfl2::lyt::LytPane * pane;
      if(i==0){
        pane = lytwk->GetPane( pParts, PANENAME_BOX_BTN_LOW_004_PANE_BOUND_00, &res_id );
      }
      else{
        pane = lytwk->GetPane( pParts, PANENAME_BOX_BTN_LOW_002_PANE_BOUND_00, &res_id );
      }
      if( pG2DUtil->GetHitPane( LYT_WORK_NORMAL, pane, x, y, false ) != false )
      {
        return i;
      }
    }
  }
  return -1;
}






void BoxLowFolderDraw::FolderIconMove(void)
{

  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytPane * pPane2 = pLytWk->GetPane(PANENAME_BOX_FLD_LOW_000_PANE_ICON_FOLDER);
  //  pPane2->SetVisible(true);

  gfl2::math::VEC3 vec = pPane2->GetTranslate();
  vec.x =  mMoveX - 160;
  vec.y =  - mMoveY + 120;
  pPane2->SetTranslate(vec);


}


void BoxLowFolderDraw::FolderIconDelete(void)
{

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
::app::ui::UIInputListener::ListenerResult BoxLowFolderDraw::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  u16 x=0,y=0;
  if( GetButtonManager()->GetUpdateState() != app::tool::ButtonManager::UPDATE_STATE_IDLE ){
    OHNO_PRINT("ボタン制御中\n");
    return ENABLE_ACCESS;
  }

  pTouchPanel->GetXY(&x,&y);

  if(isTouch && (x!=0 && y!=0)){
    mMoveX = x;  //アイコン表示位置に使用
    mMoveY = y;
  }
  if(m_uiListener==NULL){
    return DISABLE_ACCESS;
  }
  if( m_uiListener->OnTouchFirstHook(isTouch) ){
    return DISABLE_ACCESS;
  }

  int index = TouchFolderHit(mpAppParam->pBoxData->GetTrayLevel(), x,y);
  if(!mbDrawOn){
    return ENABLE_ACCESS;
  }

  
  if(index != -1 && index != 0){
    if(  mMousePokemonMode == FOLDER_MOUSE_MODE_NONE ){
      m_uiListener->TouchFolder( index );  //タッチ開始
      mMousePokemonMode = FOLDER_MOUSE_MODE_MOVE;
      mMouseFirstX=x;
      mMouseFirstY=y;
      mMouseIndex=index;
    }
  }

  if(mMousePokemonMode == FOLDER_MOUSE_MODE_MOVE && isTouch && (x!=0 && y!=0)){
    if((mMouseFirstX + DIFFER_NUM < x) || (mMouseFirstX - DIFFER_NUM > x) || (mMouseFirstY + DIFFER_NUM < y) || (mMouseFirstY - DIFFER_NUM > y)){
      mMousePokemonMode = FOLDER_MOUSE_MODE_TUKAMI;
      m_uiListener->OnFolderHand(x, y, mMouseIndex);
    }
  }

  if(mMousePokemonMode == FOLDER_MOUSE_MODE_POKEMON_DRAG && isTouch && (x!=0 && y!=0)){
    //ポケモンを持った状態でドラッグドロップ中
    if(m_uiListener->DragDrop( index )){
      if(index==0){
        GetButtonManager()->StartSelectedAct(BTN_ID_BOXLIST);
      }
    }
  }

  if(!isTouch){
    OHNO_PRINT("はなした%d\n",mMousePokemonMode);
    index = TouchFolderHit(mpAppParam->pBoxData->GetTrayLevel(), mMoveX, mMoveY);
    switch(mMousePokemonMode){
    case FOLDER_MOUSE_MODE_TUKAMI:
      if(mbCarryPokemon){//キーでポケモンを運んできた
        m_uiListener->DropFolder( index );
      }
      else{
        m_uiListener->MoveFolder( index,mMouseIndex );
      }
      break;
    case FOLDER_MOUSE_MODE_POKEMON_DRAG:
      m_uiListener->DropFolder( index );
      break;
    case FOLDER_MOUSE_MODE_MOVE:
      if(mbCarryPokemon){//キーでポケモンを運んできた
        m_uiListener->DropFolder( index );
      }
      else{
        m_uiListener->ChangeFolder( index,mMouseIndex );
      }
      break;
    }
    OHNO_PRINT("はなした\n");
    m_uiDrawListener->OnMouseModeEnd();
    MouseModeEnd();
  }

  if( m_uiListener == NULL ){
    return DISABLE_ACCESS;
  }

  // 決定
  if( pTouchPanel->IsTouchTrigger() ){
    //    m_uiListener->OnButtonAction(0);
    return DISABLE_ACCESS;
  }
  return DISABLE_ACCESS;
}


void BoxLowFolderDraw::MouseModeEnd(void)
{
  mMousePokemonMode = FOLDER_MOUSE_MODE_NONE;
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
::app::ui::UIInputListener::ListenerResult BoxLowFolderDraw::OnLayoutPaneEvent( const u32 painId )
{
  switch(painId){
  case BTN_ID_CANCEL:
    {
      m_uiListener->OnButtonAction(BoxLowFolderDrawListener::LOWFLD_ACTION_CANCEL);
      return DISABLE_ACCESS;
    }
  case BTN_ID_Y:
    {
      m_uiListener->OnButtonAction(BoxLowFolderDrawListener::LOWFLD_ACTION_FIND);
      return DISABLE_ACCESS;
    }
  case BTN_ID_BOXLIST:
    {
      m_uiListener->OnButtonAction(BoxLowFolderDrawListener::LOWFLD_ACTION_MODE_CHANGE);
      return DISABLE_ACCESS;
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
::app::ui::UIInputListener::ListenerResult BoxLowFolderDraw::OnKeyAction(  gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  if( m_uiListener == NULL ){
    return ENABLE_ACCESS;
  }

  if(pButton->IsRepeat(gfl2::ui::BUTTON_UP)){
    m_uiListener->OnButtonAction(BoxLowFolderDrawListener::LOWFLD_ACTION_UP);
    return ENABLE_ACCESS;
  }
  else if(pButton->IsRepeat(gfl2::ui::BUTTON_DOWN)){
    m_uiListener->OnButtonAction(BoxLowFolderDrawListener::LOWFLD_ACTION_DOWN);
    return ENABLE_ACCESS;
  }
  else if(pButton->IsRepeat(gfl2::ui::BUTTON_RIGHT)){
    m_uiListener->OnButtonAction(BoxLowFolderDrawListener::LOWFLD_ACTION_RIGHT);
    return ENABLE_ACCESS;
  }
  else if(pButton->IsRepeat(gfl2::ui::BUTTON_LEFT)){
    m_uiListener->OnButtonAction(BoxLowFolderDrawListener::LOWFLD_ACTION_LEFT);
    return ENABLE_ACCESS;
  }
  if( pButton->IsTrigger(gfl2::ui::BUTTON_B)  ){
    //    m_uiListener->OnButtonAction(BoxLowFolderDrawListener::LOWFLD_ACTION_CANCEL);  //終了
    //   return DISABLE_ACCESS;
  }
  if( pButton->IsTrigger(gfl2::ui::BUTTON_A) ){
    if(mButtonPos==CUR_FOLDER_BUTTON){
      GetButtonManager()->StartSelectedAct(BTN_ID_BOXLIST);
    }
    else{
      m_uiListener->OnButtonAction(BoxLowFolderDrawListener::LOWFLD_ACTION_DECIDE);  //決定
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
//----------------------------------------------------------------------
/**
 * @brief   2D関連作成
 *
 * @param   heap    ヒープ
 */
//----------------------------------------------------------------------
void BoxLowFolderDraw::Initialize2D( app::util::Heap * heap, void* pAppLytBuff )
{
  // アーカイブIDテーブル
//  void* pData = mpBoxAppSetup->GetArcReadBuff(GARC_boxLyt_bgrlow_applyt_COMP);
  void* pData = mpBoxAppSetup->GetBinLinkReadBuff(BL_IDX_BOX_PACK_BGRLOW2_APPLYT);

  app::util::G2DUtil::LytResData resTbl[LYT_WORK_MAX] = {
    {  pAppLytBuff, 1,  app::util::G2DUtil::ATTACH_ROOT, },
    {  pData, 1,  app::util::G2DUtil::ATTACH_ROOT, },
  };

  app::util::G2DUtil::LytwkData setupData[LYT_WORK_MAX] = {
    {//
      0,
      LYTRES_FLDLOW_BOX_FLD_LOW_000_BFLYT,
      LA_FLDLOW_BOX_FLD_LOW_000___NUM,
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


  Create2D( heap, NULL, LYT_WORK_MAX,
            resTbl, GFL_NELEMS(resTbl),
            setupData, GFL_NELEMS(setupData),
            GARC_message_box_DAT );


  MyCreateButtonManager();
  MessageLine1Close();
}


void BoxLowFolderDraw::SetDispActive(bool bActive)
{
  //  GetG2DUtil()->StartAnime( 0, LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_COMMANDSELECT_IN );
}


//-----------------------------------------------------------------------------
/**
 * @func    SelectMenu
 * @brief   項目選択時のリスナー
 * @date    2015.03.10
 *
 * @param   id  メニュー項目のID
 */
//-----------------------------------------------------------------------------
void BoxLowFolderDraw::SelectMenu( u32 id )
{

  OHNO_PRINT("選んだ項目 %d\n",id);

  m_uiListener->OnButtonAction(100+id);

}




static int curmode1[][5]={
  { CUR_FOLDER_4567,CUR_FOLDER_0123, -1, -1, -1}, // CUR_FOLDER_BUTTON

  { CUR_FOLDER_BUTTON,CUR_FOLDER_04,CUR_FOLDER_03,CUR_FOLDER_01, 0}, // CUR_FOLDER_00
  { CUR_FOLDER_BUTTON,CUR_FOLDER_05,CUR_FOLDER_00,CUR_FOLDER_02, 1}, // CUR_FOLDER_01
  { CUR_FOLDER_BUTTON,CUR_FOLDER_06,CUR_FOLDER_01,CUR_FOLDER_03, 2}, // CUR_FOLDER_02
  { CUR_FOLDER_BUTTON,CUR_FOLDER_07,CUR_FOLDER_02,CUR_FOLDER_00, 3}, // CUR_FOLDER_03

  { CUR_FOLDER_00,CUR_FOLDER_BUTTON,CUR_FOLDER_07,CUR_FOLDER_05, 0}, // CUR_FOLDER_04
  { CUR_FOLDER_01,CUR_FOLDER_BUTTON,CUR_FOLDER_04,CUR_FOLDER_06, 1}, // CUR_FOLDER_05
  { CUR_FOLDER_02,CUR_FOLDER_BUTTON,CUR_FOLDER_05,CUR_FOLDER_07, 2}, // CUR_FOLDER_06
  { CUR_FOLDER_03,CUR_FOLDER_BUTTON,CUR_FOLDER_06,CUR_FOLDER_04, 3}, // CUR_FOLDER_07

};


static int curmode2[][5]={
  { CUR_FOLDER_12_13_14_15,CUR_FOLDER_0123, -1, -1,-1}, // CUR_FOLDER_BUTTON
  { CUR_FOLDER_BUTTON,CUR_FOLDER_04,CUR_FOLDER_03,CUR_FOLDER_01,0}, // CUR_FOLDER_00
  { CUR_FOLDER_BUTTON,CUR_FOLDER_05,CUR_FOLDER_00,CUR_FOLDER_02,1}, // CUR_FOLDER_01
  { CUR_FOLDER_BUTTON,CUR_FOLDER_06,CUR_FOLDER_01,CUR_FOLDER_03,2}, // CUR_FOLDER_02
  { CUR_FOLDER_BUTTON,CUR_FOLDER_07,CUR_FOLDER_02,CUR_FOLDER_00,3}, // CUR_FOLDER_03

  { CUR_FOLDER_00,CUR_FOLDER_08,CUR_FOLDER_07,CUR_FOLDER_05,0}, // CUR_FOLDER_04
  { CUR_FOLDER_01,CUR_FOLDER_09,CUR_FOLDER_04,CUR_FOLDER_06,1}, // CUR_FOLDER_05
  { CUR_FOLDER_02,CUR_FOLDER_10,CUR_FOLDER_05,CUR_FOLDER_07,2}, // CUR_FOLDER_06
  { CUR_FOLDER_03,CUR_FOLDER_11,CUR_FOLDER_06,CUR_FOLDER_04,3}, // CUR_FOLDER_07

  { CUR_FOLDER_04,CUR_FOLDER_12,CUR_FOLDER_11,CUR_FOLDER_09,0}, // CUR_FOLDER_08
  { CUR_FOLDER_05,CUR_FOLDER_13,CUR_FOLDER_08,CUR_FOLDER_10,1}, // CUR_FOLDER_09
  { CUR_FOLDER_06,CUR_FOLDER_14,CUR_FOLDER_09,CUR_FOLDER_11,2}, // CUR_FOLDER_10
  { CUR_FOLDER_07,CUR_FOLDER_15,CUR_FOLDER_10,CUR_FOLDER_08,3}, // CUR_FOLDER_11

  { CUR_FOLDER_08,CUR_FOLDER_BUTTON,CUR_FOLDER_15,CUR_FOLDER_13,0}, // CUR_FOLDER_12
  { CUR_FOLDER_09,CUR_FOLDER_BUTTON,CUR_FOLDER_12,CUR_FOLDER_14,1}, // CUR_FOLDER_13
  { CUR_FOLDER_10,CUR_FOLDER_BUTTON,CUR_FOLDER_13,CUR_FOLDER_15,2}, // CUR_FOLDER_14
  { CUR_FOLDER_11,CUR_FOLDER_BUTTON,CUR_FOLDER_14,CUR_FOLDER_12,3}, // CUR_FOLDER_15

};



static int curmode3[][5]={
  { CUR_FOLDER_18_23,CUR_FOLDER_012345, -1, -1,-1}, // CUR_FOLDER_BUTTON

  { CUR_FOLDER_BUTTON,CUR_FOLDER_06,CUR_FOLDER_05,CUR_FOLDER_01,0}, // CUR_FOLDER_00
  { CUR_FOLDER_BUTTON,CUR_FOLDER_07,CUR_FOLDER_00,CUR_FOLDER_02,1}, // CUR_FOLDER_01
  { CUR_FOLDER_BUTTON,CUR_FOLDER_08,CUR_FOLDER_01,CUR_FOLDER_03,2}, // CUR_FOLDER_02
  { CUR_FOLDER_BUTTON,CUR_FOLDER_09,CUR_FOLDER_02,CUR_FOLDER_04,3}, // CUR_FOLDER_03
  { CUR_FOLDER_BUTTON,CUR_FOLDER_10,CUR_FOLDER_03,CUR_FOLDER_05,4}, // CUR_FOLDER_04
  { CUR_FOLDER_BUTTON,CUR_FOLDER_11,CUR_FOLDER_04,CUR_FOLDER_00,5}, // CUR_FOLDER_05

  { CUR_FOLDER_00,CUR_FOLDER_12,CUR_FOLDER_11,CUR_FOLDER_07,0}, // CUR_FOLDER_06
  { CUR_FOLDER_01,CUR_FOLDER_13,CUR_FOLDER_06,CUR_FOLDER_08,1}, // CUR_FOLDER_07
  { CUR_FOLDER_02,CUR_FOLDER_14,CUR_FOLDER_07,CUR_FOLDER_09,2}, // CUR_FOLDER_08
  { CUR_FOLDER_03,CUR_FOLDER_15,CUR_FOLDER_08,CUR_FOLDER_10,3}, // CUR_FOLDER_09
  { CUR_FOLDER_04,CUR_FOLDER_16,CUR_FOLDER_09,CUR_FOLDER_11,4}, // CUR_FOLDER_10
  { CUR_FOLDER_05,CUR_FOLDER_17,CUR_FOLDER_10,CUR_FOLDER_06,5}, // CUR_FOLDER_11

  { CUR_FOLDER_06,CUR_FOLDER_18,CUR_FOLDER_17,CUR_FOLDER_13,0}, // CUR_FOLDER_12
  { CUR_FOLDER_07,CUR_FOLDER_19,CUR_FOLDER_12,CUR_FOLDER_14,1}, // CUR_FOLDER_13
  { CUR_FOLDER_08,CUR_FOLDER_20,CUR_FOLDER_13,CUR_FOLDER_15,2}, // CUR_FOLDER_14
  { CUR_FOLDER_09,CUR_FOLDER_21,CUR_FOLDER_14,CUR_FOLDER_16,3}, // CUR_FOLDER_15
  { CUR_FOLDER_10,CUR_FOLDER_22,CUR_FOLDER_15,CUR_FOLDER_17,4}, // CUR_FOLDER_16
  { CUR_FOLDER_11,CUR_FOLDER_23,CUR_FOLDER_16,CUR_FOLDER_12,5}, // CUR_FOLDER_17

  { CUR_FOLDER_12,CUR_FOLDER_BUTTON,CUR_FOLDER_23,CUR_FOLDER_19,0}, // CUR_FOLDER_18
  { CUR_FOLDER_13,CUR_FOLDER_BUTTON,CUR_FOLDER_18,CUR_FOLDER_20,1}, // CUR_FOLDER_19
  { CUR_FOLDER_14,CUR_FOLDER_BUTTON,CUR_FOLDER_19,CUR_FOLDER_21,2}, // CUR_FOLDER_20
  { CUR_FOLDER_15,CUR_FOLDER_BUTTON,CUR_FOLDER_20,CUR_FOLDER_22,3}, // CUR_FOLDER_21
  { CUR_FOLDER_16,CUR_FOLDER_BUTTON,CUR_FOLDER_21,CUR_FOLDER_23,4}, // CUR_FOLDER_22
  { CUR_FOLDER_17,CUR_FOLDER_BUTTON,CUR_FOLDER_22,CUR_FOLDER_18,5}, // CUR_FOLDER_23

};




static int curmode4[][5]={
  { CUR_FOLDER_24_31,CUR_FOLDER_0_7, -1, -1,-1}, // CUR_FOLDER_BUTTON

  { CUR_FOLDER_BUTTON,CUR_FOLDER_08,CUR_FOLDER_07,CUR_FOLDER_01,0}, // CUR_FOLDER_00
  { CUR_FOLDER_BUTTON,CUR_FOLDER_09,CUR_FOLDER_00,CUR_FOLDER_02,1}, // CUR_FOLDER_01
  { CUR_FOLDER_BUTTON,CUR_FOLDER_10,CUR_FOLDER_01,CUR_FOLDER_03,2}, // CUR_FOLDER_02
  { CUR_FOLDER_BUTTON,CUR_FOLDER_11,CUR_FOLDER_02,CUR_FOLDER_04,3}, // CUR_FOLDER_03
  { CUR_FOLDER_BUTTON,CUR_FOLDER_12,CUR_FOLDER_03,CUR_FOLDER_05,4}, // CUR_FOLDER_04
  { CUR_FOLDER_BUTTON,CUR_FOLDER_13,CUR_FOLDER_04,CUR_FOLDER_06,5}, // CUR_FOLDER_05
  { CUR_FOLDER_BUTTON,CUR_FOLDER_14,CUR_FOLDER_05,CUR_FOLDER_07,6}, // CUR_FOLDER_06
  { CUR_FOLDER_BUTTON,CUR_FOLDER_15,CUR_FOLDER_06,CUR_FOLDER_00,7}, // CUR_FOLDER_07

  { CUR_FOLDER_00,CUR_FOLDER_16,CUR_FOLDER_15,CUR_FOLDER_09,0}, // CUR_FOLDER_08
  { CUR_FOLDER_01,CUR_FOLDER_17,CUR_FOLDER_08,CUR_FOLDER_10,1}, // CUR_FOLDER_09
  { CUR_FOLDER_02,CUR_FOLDER_18,CUR_FOLDER_09,CUR_FOLDER_11,2}, // CUR_FOLDER_10
  { CUR_FOLDER_03,CUR_FOLDER_19,CUR_FOLDER_10,CUR_FOLDER_12,3}, // CUR_FOLDER_11
  { CUR_FOLDER_04,CUR_FOLDER_20,CUR_FOLDER_11,CUR_FOLDER_13,4}, // CUR_FOLDER_12
  { CUR_FOLDER_05,CUR_FOLDER_21,CUR_FOLDER_12,CUR_FOLDER_14,5}, // CUR_FOLDER_13
  { CUR_FOLDER_06,CUR_FOLDER_22,CUR_FOLDER_13,CUR_FOLDER_15,6}, // CUR_FOLDER_14
  { CUR_FOLDER_07,CUR_FOLDER_23,CUR_FOLDER_14,CUR_FOLDER_08,7}, // CUR_FOLDER_15

  { CUR_FOLDER_08,CUR_FOLDER_24,CUR_FOLDER_23,CUR_FOLDER_17,0}, // CUR_FOLDER_16
  { CUR_FOLDER_09,CUR_FOLDER_25,CUR_FOLDER_16,CUR_FOLDER_18,1}, // CUR_FOLDER_17
  { CUR_FOLDER_10,CUR_FOLDER_26,CUR_FOLDER_17,CUR_FOLDER_19,2}, // CUR_FOLDER_18
  { CUR_FOLDER_11,CUR_FOLDER_27,CUR_FOLDER_18,CUR_FOLDER_20,3}, // CUR_FOLDER_19
  { CUR_FOLDER_12,CUR_FOLDER_28,CUR_FOLDER_19,CUR_FOLDER_21,4}, // CUR_FOLDER_20
  { CUR_FOLDER_13,CUR_FOLDER_29,CUR_FOLDER_20,CUR_FOLDER_22,5}, // CUR_FOLDER_21
  { CUR_FOLDER_14,CUR_FOLDER_30,CUR_FOLDER_21,CUR_FOLDER_23,6}, // CUR_FOLDER_22
  { CUR_FOLDER_15,CUR_FOLDER_31,CUR_FOLDER_22,CUR_FOLDER_16,7}, // CUR_FOLDER_23

  { CUR_FOLDER_16,CUR_FOLDER_BUTTON,CUR_FOLDER_31,CUR_FOLDER_25,0}, // CUR_FOLDER_24
  { CUR_FOLDER_17,CUR_FOLDER_BUTTON,CUR_FOLDER_24,CUR_FOLDER_26,1}, // CUR_FOLDER_25
  { CUR_FOLDER_18,CUR_FOLDER_BUTTON,CUR_FOLDER_25,CUR_FOLDER_27,2}, // CUR_FOLDER_26
  { CUR_FOLDER_19,CUR_FOLDER_BUTTON,CUR_FOLDER_26,CUR_FOLDER_28,3}, // CUR_FOLDER_27
  { CUR_FOLDER_20,CUR_FOLDER_BUTTON,CUR_FOLDER_27,CUR_FOLDER_29,4}, // CUR_FOLDER_28
  { CUR_FOLDER_21,CUR_FOLDER_BUTTON,CUR_FOLDER_28,CUR_FOLDER_30,5}, // CUR_FOLDER_29
  { CUR_FOLDER_22,CUR_FOLDER_BUTTON,CUR_FOLDER_29,CUR_FOLDER_31,6}, // CUR_FOLDER_30
  { CUR_FOLDER_23,CUR_FOLDER_BUTTON,CUR_FOLDER_30,CUR_FOLDER_24,7}, // CUR_FOLDER_31

};


static int mode1pane[]={
  PANENAME_BOX_FLD_LOW_000_PANE_BTN_MODE_CURPOS,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_L_00,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_L_01,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_L_02,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_L_03,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_L_04,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_L_05,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_L_06,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_L_07,
};


static int mode2pane[]={
  PANENAME_BOX_FLD_LOW_000_PANE_BTN_MODE_CURPOS,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M_00,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M_01,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M_02,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M_03,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M_04,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M_05,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M_06,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M_07,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M_08,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M_09,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M_10,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M_11,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M_12,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M_13,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M_14,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M_15,
};

static int mode3pane[]={
  PANENAME_BOX_FLD_LOW_000_PANE_BTN_MODE_CURPOS,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M2_00,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M2_01,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M2_02,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M2_03,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M2_04,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M2_05,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M2_06,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M2_07,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M2_08,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M2_09,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M2_10,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M2_11,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M2_12,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M2_13,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M2_14,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M2_15,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M2_16,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M2_17,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M2_18,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M2_19,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M2_20,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M2_21,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M2_22,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_M2_23,
};


static int mode4pane[]={
  PANENAME_BOX_FLD_LOW_000_PANE_BTN_MODE_CURPOS,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_S_00,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_S_01,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_S_02,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_S_03,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_S_04,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_S_05,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_S_06,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_S_07,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_S_08,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_S_09,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_S_10,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_S_11,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_S_12,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_S_13,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_S_14,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_S_15,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_S_16,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_S_17,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_S_18,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_S_19,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_S_20,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_S_21,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_S_22,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_S_23,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_S_24,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_S_25,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_S_26,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_S_27,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_S_28,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_S_29,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_S_30,
  PANENAME_BOX_FLD_LOW_000_PANE_FOLDER_S_31,
};


//-----------------------------------------------------------------------------
/**
 * @func    現在の位置からカーソルの位置を決める
 * @brief   項目選択時のリスナー
 * @date    2015.03.10
 *
 * @param   id  メニュー項目のID
 */
//-----------------------------------------------------------------------------
int BoxLowFolderDraw::SetCursorPos(int no, int udlr, int mode )
{
  gfl2::math::VEC3 vec3;
  int nextbutton=no;
  int nb=0;
  //  int pane;

  if(no==-1){ ///位置不定の場合には０始まりにおきなおす NMCat[3851]
    nextbutton=0;
  }


  if(udlr==-1){
		if(no==-1){
			nextbutton=0;
		}
		else{
	    nextbutton=no;
		}
  }
  else{
    switch(mode){
    case 0:
      nb = curmode1[nextbutton][udlr];
      if( curmode1[nextbutton][4] != -1){
        mKeepCursorPosHorizontal = curmode1[nextbutton][4];
      }
      break;
    case 1:
      nb = curmode2[nextbutton][udlr];
      if( curmode2[nextbutton][4] != -1){
        mKeepCursorPosHorizontal = curmode2[nextbutton][4];
      }
      break;
    case 2:
      nb = curmode3[nextbutton][udlr];
      if( curmode3[nextbutton][4] != -1){
        mKeepCursorPosHorizontal = curmode3[nextbutton][4];
      }
      break;
    case 3:
      nb = curmode4[nextbutton][udlr];
      if( curmode4[nextbutton][4] != -1){
        mKeepCursorPosHorizontal = curmode4[nextbutton][4];
      }
      break;
    };
    if(nb!=-1){
      nextbutton = nb;
    }
  }

  switch(nextbutton){
  case CUR_FOLDER_0123:
  case CUR_FOLDER_012345:
  case CUR_FOLDER_0_7:
    nextbutton = mKeepCursorPosHorizontal  + CUR_FOLDER_00;
    break;
  case CUR_FOLDER_4567:
    nextbutton = mKeepCursorPosHorizontal + CUR_FOLDER_04;
    break;
  case CUR_FOLDER_12_13_14_15:
    nextbutton = mKeepCursorPosHorizontal + CUR_FOLDER_12;
    break;
  case CUR_FOLDER_18_23:
    nextbutton = mKeepCursorPosHorizontal + CUR_FOLDER_18;
    break;
  case CUR_FOLDER_24_31:
    nextbutton = mKeepCursorPosHorizontal + CUR_FOLDER_24;
    break;
  default:
    break;
  }

  switch(mode){
  case 0:
    GFL_ASSERT( nextbutton < elementof(mode1pane) );
    nb = mode1pane[nextbutton];
    break;
  case 1:
    GFL_ASSERT( nextbutton < elementof(mode2pane) );
    nb = mode2pane[nextbutton];
    break;
  case 2:
    GFL_ASSERT( nextbutton < elementof(mode3pane) );
    nb = mode3pane[nextbutton];
    break;
  case 3:
    GFL_ASSERT( nextbutton < elementof(mode4pane) );
    nb = mode4pane[nextbutton];
    break;
  };

  vec3 = GetG2DUtil()->GetPanePos(LYT_WORK_NORMAL,  nb  );
  LiftPokeIconMove( &vec3 );
  DispRangeNum( &vec3 );

  if(CUR_FOLDER_BUTTON==nextbutton){
    CursorChange(false);
  }
  else{
    CursorChange(true);
  }

  GetG2DUtil()->SetPanePos(LYT_WORK_NORMAL, PANENAME_BOX_FLD_LOW_000_PANE_CURSOR, &vec3);

  if(mButtonPos != nextbutton){
    Sound::PlaySE(SEQ_SE_SELECT1);
  }
  mButtonPos = nextbutton;

  return nextbutton;
}

void BoxLowFolderDraw::SetButtonPos(int buttonpos)
{
  mButtonPos = buttonpos;
}



gfl2::lyt::LytParts* BoxLowFolderDraw::GetFolderPane(int nextbutton)
{
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  int nb;
  int mode = mpAppParam->pBoxData->GetTrayLevel();

  switch(mode){
  case 0:
    if(nextbutton>=elementof(mode1pane)){
      return NULL;
    }
    nb = mode1pane[nextbutton];
    break;
  case 1:
    if(nextbutton>=elementof(mode2pane)){
      return NULL;
    }
    nb = mode2pane[nextbutton];
    break;
  case 2:
    if(nextbutton>=elementof(mode3pane)){
      return NULL;
    }
    nb = mode3pane[nextbutton];
    break;
  case 3:
    if(nextbutton>=elementof(mode4pane)){
      return NULL;
    }
    nb = mode4pane[nextbutton];
    break;
  }
  if(nb==PANENAME_BOX_FLD_LOW_000_PANE_BTN_MODE_CURPOS){
    nb = PANENAME_BOX_FLD_LOW_000_PANE_BTN_MODE;
  }
  return pLytWk->GetPartsPane( nb  );
}


//----------------------------------------------------------------------
/**
 *	@brief  初期テキスト表示
 */
//----------------------------------------------------------------------


void BoxLowFolderDraw::InitializeText( app::util::Heap * pheap )
{


  mpTempStrBuf = GFL_NEW( pheap->GetDeviceHeap() ) gfl2::str::StrBuf( TEMP_STRBUF_LENGTH, pheap->GetDeviceHeap() );
  mpTempStrBuf2 = GFL_NEW( pheap->GetDeviceHeap() ) gfl2::str::StrBuf( TEMP_STRBUF_LENGTH, pheap->GetDeviceHeap() );

  mpWordSet = GFL_NEW( pheap->GetDeviceHeap() ) print::WordSet( pheap->GetDeviceHeap() );




}


void BoxLowFolderDraw::FinalizeText(void)
{
  GFL_DELETE mpWordSet;
  GFL_DELETE mpTempStrBuf2;
  GFL_DELETE mpTempStrBuf;
}

void BoxLowFolderDraw::SetMessage( int msgid )
{
}




//------------------------------------------------------------------------------
/**
 * @brief   ボタンマネージャの生成
 */
//------------------------------------------------------------------------------
void BoxLowFolderDraw::MyCreateButtonManager( void )
{
  static const int partsnum = 2;
  static const int layoutno = LYT_WORK_NORMAL;

  app::ui::ButtonInfoEx c_DefInfo[partsnum] = {
    {
      BTN_ID_Y , NULL, NULL,
      LA_FLDLOW_BOX_FLD_LOW_000__BTN_SEARCH_TOUCH,
      LA_FLDLOW_BOX_FLD_LOW_000__BTN_SEARCH_RELEASE,
      LA_FLDLOW_BOX_FLD_LOW_000__BTN_SEARCH_CANSEL,
      LA_FLDLOW_BOX_FLD_LOW_000__BTN_SEARCH_TOUCH_RELEASE,
      LA_FLDLOW_BOX_FLD_LOW_000__BTN_SEARCH_ACTIVE,
      LA_FLDLOW_BOX_FLD_LOW_000__BTN_SEARCH_PASSIVE,
    },
    {
      BTN_ID_BOXLIST, NULL, NULL,
      LA_FLDLOW_BOX_FLD_LOW_000__BTN_MODE_TOUCH,
      LA_FLDLOW_BOX_FLD_LOW_000__BTN_MODE_RELEASE,
      LA_FLDLOW_BOX_FLD_LOW_000__BTN_MODE_CANSEL,
      LA_FLDLOW_BOX_FLD_LOW_000__BTN_MODE_TOUCH_RELEASE,
      LA_FLDLOW_BOX_FLD_LOW_000__BTN_MODE_ACTIVE,
      LA_FLDLOW_BOX_FLD_LOW_000__BTN_MODE_PASSIVE,
    },
  };

  int painNo[]={
    PANENAME_BOX_FLD_LOW_000_PANE_BTN_SEARCH,
    PANENAME_BOX_FLD_LOW_000_PANE_BTN_MODE,
  };

  int boundNo[]={
    PANENAME_BOX_BTN_LOW_003_PANE_BOUND_00,
    PANENAME_BOX_BTN_LOW_004_PANE_BOUND_00,
	};
  

  app::ui::ButtonInfoEx * info = GFL_NEW_LOW_ARRAY(mpAppHeap->GetSystemHeap()) app::ui::ButtonInfoEx[partsnum];

  int i=0;
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( layoutno );
  GFL_ASSERT(pLytWk!=NULL);
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( layoutno );

  gfl2::lyt::LytParts* pParts[partsnum];

  for( i = 0 ; i < partsnum; i++){
    pParts[i] = pLytWk->GetPartsPane( painNo[i] );
    info[i] = c_DefInfo[i];
    info[i].picture_pane = pParts[i];
    info[i].bound_pane = pLytWk->GetPane( pParts[i], boundNo[i], &resID );

  }
  this->CreateButtonManager( mpAppHeap, pLytWk, info, partsnum );

  GFL_DELETE_ARRAY info;

  app::tool::ButtonManager* pBtnMan = this->GetButtonManager();
  pBtnMan->SetButtonBindKey( BTN_ID_BOXLIST, gfl2::ui::BUTTON_B, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );
  pBtnMan->SetButtonSelectSE( BTN_ID_BOXLIST, SEQ_SE_DECIDE1 );

  pBtnMan->SetButtonBindKey( BTN_ID_Y, gfl2::ui::BUTTON_Y, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );
  pBtnMan->SetButtonSelectSE( BTN_ID_Y, SEQ_SE_DECIDE1 );


}


void BoxLowFolderDraw::DrawMessageLine1(int messageno)
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytParts* pParts = pLytWk->GetPartsPane( PANENAME_BOX_FLD_LOW_000_PANE_MESSAGE );
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );
  pG2DUtil->GetString( mpTempStrBuf, (u32)messageno );
  pG2DUtil->SetMessage(mpTempStrBuf);
  pParts->SetVisible( true );
  gfl2::lyt::LytPane * pPane = pLytWk->GetPane( pParts, PANENAME_COMMON_MSG_LOW_001_PANE_TEXTBOX_00, &resID );
  pPane->SetVisible( true );
}

void BoxLowFolderDraw::MessageLine1Close(void)
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytParts* pParts = pLytWk->GetPartsPane( PANENAME_BOX_FLD_LOW_000_PANE_MESSAGE );
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );
  pParts->SetVisible( false );
  gfl2::lyt::LytPane * pPane = pLytWk->GetPane( pParts, PANENAME_COMMON_MSG_LOW_001_PANE_TEXTBOX_00, &resID );
  pPane->SetVisible( false );
}





/*
void BoxLowFolderDraw::BoxAnimLarge(int boxno, int num ,int passive)
{
  //アニメは８種類の繰り返し
  int tbl[]={
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_L_00_PATERN_00,  //から
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_L_00_PATERN_01,  //いる
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_L_00_PATERN_02,  //まんたん
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_L_00_PATERN_03,  //からのぱ っしぶ
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_L_00_PATERN_04,  //１いじょうのぱっしぶ
  };

  mpAppParam->pBoxPokemon->

  pG2DUtil->StartAnime( LYT_WORK_NORMAL, LA_BGRLOW_BOX_BGR_LOW_000_BG_KEEP );



}



void BoxLowFolderDraw::InitBoxAnim(void)
{

  pG2DUtil->StartAnime( LYT_WORK_NORMAL, LA_BGRLOW_BOX_BGR_LOW_000_BG_KEEP );


 */

int BoxLowFolderDraw::getFolderAnimNo(int num, int active)
{
  int no;
  no=1;//1-29
  if(num==0){
    no = 0;///フォルダ空っぽ状態
  }
  if(num==30){
    no = 2;//まんたん
  }
  if(!active){
    if(num==0){
      no = 3;///フォルダ空っぽpassive
    }
    else if(num==30){
      no = 8;//まんたん
    }
    else{
      no = 4; //passive
    }
  }
  return no;
}



void BoxLowFolderDraw::AnimFolder( int boxno , int num , int active, int lv)
{
  int no;

  //アニメは８種類の繰り返し
  int tbll[]={
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_L_00_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_L_01_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_L_02_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_L_03_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_L_04_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_L_05_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_L_06_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_L_07_PATERN_00,
  };

  int tblm[]={
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M_00_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M_01_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M_02_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M_03_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M_04_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M_05_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M_06_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M_07_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M_08_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M_09_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M_10_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M_11_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M_12_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M_13_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M_14_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M_15_PATERN_00,
  };


  int tblm2[]={
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M2_00_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M2_01_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M2_02_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M2_03_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M2_04_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M2_05_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M2_06_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M2_07_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M2_08_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M2_09_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M2_10_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M2_11_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M2_12_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M2_13_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M2_14_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M2_15_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M2_16_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M2_17_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M2_18_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M2_19_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M2_20_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M2_21_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M2_22_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_M2_23_PATERN_00,
  };

  int tbls[]={
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_S_00_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_S_01_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_S_02_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_S_03_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_S_04_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_S_05_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_S_06_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_S_07_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_S_08_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_S_09_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_S_10_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_S_11_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_S_12_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_S_13_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_S_14_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_S_15_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_S_16_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_S_17_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_S_18_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_S_19_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_S_20_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_S_21_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_S_22_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_S_23_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_S_24_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_S_25_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_S_26_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_S_27_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_S_28_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_S_29_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_S_30_PATERN_00,
    LA_FLDLOW_BOX_FLD_LOW_000__FOLDER_S_31_PATERN_00,
  };




  if(lv==0 && boxno >= 8){
    return;
  }
  if(lv==1 && boxno >= 16){
    return;
  }
  if(lv==2 && boxno >= 24){
    return;
  }

  no = getFolderAnimNo( num, active);

  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

  if(lv==0){
    pG2DUtil->StartAnime( LYT_WORK_NORMAL, tbll[boxno] + no );
  }
  else if(lv==1){
    pG2DUtil->StartAnime( LYT_WORK_NORMAL, tblm[boxno] + no );
  }
  else if(lv==2){
    pG2DUtil->StartAnime( LYT_WORK_NORMAL, tblm2[boxno] + no );
  }
  else{
    pG2DUtil->StartAnime( LYT_WORK_NORMAL, tbls[boxno] + no );
  }
  SetFolderActive(boxno+1,true);


}



void BoxLowFolderDraw::InitBoxAnim(int cursortype)
{
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
//  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  int curlabel[]={
    LA_FLDLOW_BOX_FLD_LOW_000__CURSOR_COLOR_RED,
    LA_FLDLOW_BOX_FLD_LOW_000__CURSOR_COLOR_BLUE,
    LA_FLDLOW_BOX_FLD_LOW_000__CURSOR_COLOR_GREEN,
  };
//  pG2DUtil->StartAnime( LYT_WORK_NORMAL, curlabel[cursortype] );

  int anm = curlabel[cursortype];

  float x = pLytWk->GetAnimeMaxFrame(anm );
  
  pLytWk->BindAnime( anm, true );
  pLytWk->SetAutoAnime( anm, true );
  pLytWk->SetAnimeFrame( anm, x );

  app::ui::UIView::Update();


}






void BoxLowFolderDraw::LiftPokeIconStart0(void)  // kawa_box_modify  使用中のペインのテクスチャを貼り替えるので処理を2フレームに分ける
{
  gfl2::math::VEC3 vec3;
  gfl2::lyt::LytPicture * pPane;
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );

  gfl2::lyt::LytParts* pParts = pLytWk->GetPartsPane( PANENAME_BOX_FLD_LOW_000_PANE_ICON_POKE );
  pParts->SetVisible(false);

  pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ICONPARTS_POKE, &resID );
  pPane->SetVisible(false);

  pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ICONPARTS_POKESHADOW, &resID );
  pPane->SetVisible(false);

  mbCarryPokemon = true;

  InitTouchPos();
}

void BoxLowFolderDraw::LiftPokeIconStart(PokeTool::SimpleParam* pPara)
{
  gfl2::math::VEC3 vec3;
  gfl2::lyt::LytPicture * pPane;
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );

  gfl2::lyt::LytParts* pParts = pLytWk->GetPartsPane( PANENAME_BOX_FLD_LOW_000_PANE_ICON_POKE );
  pParts->SetVisible(true);

  pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ICONPARTS_POKE, &resID );
  pPane->SetVisible(true);

  mpAppParam->pPokeIcon->ReplacePaneTexture(30, pPara, pPane );
  vec3=pPane->GetTranslate();

  pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ICONPARTS_POKESHADOW, &resID );
  mpAppParam->pPokeIcon->ReplacePaneTexture(30, pPane );
  pPane->SetVisible(true);
  vec3.x += 3;
  vec3.y -= 3;
  pPane->SetTranslate(vec3);
}


void BoxLowFolderDraw::LiftPokeIconMove( gfl2::math::VEC3* pvec)
{

  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytPane * pPane2 = pLytWk->GetPane(PANENAME_BOX_FLD_LOW_000_PANE_ICON_POKE);

  gfl2::math::VEC3 vec = pPane2->GetTranslate();
  vec.x =  pvec->x + 10;
  vec.y =  pvec->y - 10;

  pPane2->SetTranslate(vec);


}


void BoxLowFolderDraw::LiftPokeIconEnd(void)
{
  gfl2::math::VEC3 vec3;
  gfl2::lyt::LytPicture * pPane;
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );

  gfl2::lyt::LytParts* pParts = pLytWk->GetPartsPane( PANENAME_BOX_FLD_LOW_000_PANE_ICON_POKE );
  pParts->SetVisible(false);

  mbCarryPokemon = false;

}





//複数選択のときのカーソルがフォルダーやボックス名と重なった時の数字表示
void BoxLowFolderDraw::DispRangeNum(gfl2::math::VEC3* pvec)
{
  if(mbCarryNum){
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
    gfl2::lyt::LytParts* pParts;
    gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );
    gfl2::lyt::LytTextBox * pPaneText;

    pG2DUtil->SetRegisterNumber( 0, mbCarryNum, 2 );
    pG2DUtil->GetStringExpand( mpTempStrBuf, PARAM__VALUE_2 );
    pPaneText = pLytWk->GetTextBoxPane( PANENAME_BOX_FLD_LOW_000_PANE_TEXTBOX_02);
    pPaneText->SetVisible( true );
    pG2DUtil->SetTextBoxPaneString( pPaneText, mpTempStrBuf );
    pG2DUtil->SetPaneVisible( LYT_WORK_NORMAL, PANENAME_BOX_FLD_LOW_000_PANE_PLT_NUMBER, true );

    gfl2::lyt::LytPane * pPane2 = pLytWk->GetPane(PANENAME_BOX_FLD_LOW_000_PANE_PLT_NUMBER);

    gfl2::math::VEC3 vec = pPane2->GetTranslate();
    vec.x =  pvec->x;
    vec.y =  pvec->y + 10;//GFNMCat5486
    pPane2->SetTranslate(vec);
  }
}


void BoxLowFolderDraw::ResetRangeNum(void )
{

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  pG2DUtil->SetPaneVisible( LYT_WORK_NORMAL, PANENAME_BOX_FLD_LOW_000_PANE_PLT_NUMBER, false );
}


void BoxLowFolderDraw::CarryStart(int page, int startx, int starty, int sizex, int sizey, int topcur, int num)
{
  mbCarryPokemon = true;
  mbCarryNum = num;

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  pG2DUtil->SetPaneVisible( LYT_WORK_NORMAL, PANENAME_BOX_FLD_LOW_000_PANE_PLT_NUMBER, true );

  InitTouchPos();
  
  //  SetCursorPos(int no, int udlr, int mode )

}

void BoxLowFolderDraw::CarryEnd(void)
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  pG2DUtil->SetPaneVisible( LYT_WORK_NORMAL, PANENAME_BOX_FLD_LOW_000_PANE_PLT_NUMBER, false );

  
  mbCarryNum=0;
  mbCarryPokemon = false;
}


void BoxLowFolderDraw::CursorChange(bool byoko )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytParts* pParts;
  gfl2::lyt::LytPane * pPane;
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );

  pParts = pLytWk->GetPartsPane( PANENAME_BOX_FLD_LOW_000_PANE_CURSOR );  //
  pPane = pLytWk->GetPane( pParts, PANENAME_BOX_CUR_LOW_000_PANE_BOX_CURSOR_00, &resID );
  pPane->SetVisible(byoko);
  pPane = pLytWk->GetPane( pParts, PANENAME_BOX_CUR_LOW_000_PANE_BOX_CURSOR_01, &resID );
  pPane->SetVisible(!byoko);
}


void BoxLowFolderDraw::SetCursorVisible(bool bvisible )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytParts* pParts;
  gfl2::lyt::LytPane * pPane;
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );

  pParts = pLytWk->GetPartsPane( PANENAME_BOX_FLD_LOW_000_PANE_CURSOR );  //
  pParts->SetVisible(bvisible);
}


void BoxLowFolderDraw::FolderIconMoveActive(int pos,bool bActive)
{
  app::util::G2DUtil * pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytPane * pPane2 = pLytWk->GetPane(PANENAME_BOX_FLD_LOW_000_PANE_ICON_FOLDER);
  pPane2->SetVisible(bActive);

  if(bActive){
    bool flg=false;
    if(mpBoxAppSetup->GetBoxSimpleAccesser()->IsSearchMode() || mpAppParam->trade.bOn){
      if(mpBoxAppSetup->GetBoxSimpleAccesser()->GetSearchPokemonNum(pos-1) != 0){
        flg=true;
      }
    }
    else{
      flg=true;
    }
    int num = mpBoxAppSetup->GetBoxSimpleAccesser()->GetPokemonCount(pos-1);

    int no;
    no = getFolderAnimNo( num, flg);

    pG2DUtil->StartAnime( LYT_WORK_NORMAL, LA_FLDLOW_BOX_FLD_LOW_000__ICON_FOLDER_PATERN_00+no);
    int mode = mpAppParam->pBoxData->GetTrayLevel();
    int buff[]={0,1,1,2};
    pG2DUtil->StartAnime( LYT_WORK_NORMAL, LA_FLDLOW_BOX_FLD_LOW_000__ICON_FOLDER_FOLDER_SIZE_L+buff[mode]);
  }

}


void BoxLowFolderDraw::SetFolderActive(int pos,bool bActive)
{
  gfl2::lyt::LytParts* pParts = GetFolderPane( pos );
  if(pParts){
    pParts->SetVisible(bActive);
  }
}


void BoxLowFolderDraw::SetSearchButtonPassive(void)
{
  app::tool::ButtonManager* pBtnMan = this->GetButtonManager();
//  if(mpAppParam->trade.bOn == true)
  {
    pBtnMan->SetButtonPassive( BTN_ID_Y );
  }
}

void BoxLowFolderDraw::SetSearchButtonActive(void)
{
  app::tool::ButtonManager* pBtnMan = this->GetButtonManager();
  //if(mpAppParam->trade.bOn != true)
  {
    pBtnMan->SetButtonActive( BTN_ID_Y );
  }
}


void BoxLowFolderDraw::SetBoxListButtonPassive(void)
{
  app::tool::ButtonManager* pBtnMan = this->GetButtonManager();
  pBtnMan->SetButtonPassive( BTN_ID_BOXLIST );
}

void BoxLowFolderDraw::SetBoxListButtonActive(void)
{
  app::tool::ButtonManager* pBtnMan = this->GetButtonManager();
  pBtnMan->SetButtonActive( BTN_ID_BOXLIST );
}


//-----------------------------------------------------------------------------
/**
 * @brief つかみモード遷移時に初期座標をセットする
 */
//-----------------------------------------------------------------------------
void BoxLowFolderDraw::InitTouchPos(void)
{
  gfl2::ui::TouchPanel * tp = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetTouchPanel(gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD);
  if( tp->IsTouch() != false )
  {
    u16 x, y;
    tp->GetXY( &x, &y );
    mMoveX = static_cast<int>( x );
    mMoveY = static_cast<int>( y );
  }
}


//-----------------------------------------------------------------------------
/**
 * @brief カーソルアニメ停止
 */
//-----------------------------------------------------------------------------
void BoxLowFolderDraw::StopMainCursor(void)
{
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  pLytWk->BindAnime( LA_FLDLOW_BOX_FLD_LOW_000__CURSOR_KEEP, true );
  pLytWk->SetAutoAnime( LA_FLDLOW_BOX_FLD_LOW_000__CURSOR_KEEP, false );
  pLytWk->SetAnimeFrame( LA_FLDLOW_BOX_FLD_LOW_000__CURSOR_KEEP, 0.0f );
//  mbStopCursor = true;
}

//-----------------------------------------------------------------------------
/**
 * @brief カーソルアニメ開始
 */
//-----------------------------------------------------------------------------
void BoxLowFolderDraw::StartMainCursor(void)
{
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  pLytWk->BindAnime( LA_FLDLOW_BOX_FLD_LOW_000__CURSOR_KEEP, true );
  pLytWk->SetAutoAnime( LA_FLDLOW_BOX_FLD_LOW_000__CURSOR_KEEP, true );
//  mbStopCursor = false;
}


void BoxLowFolderDraw::ButtonReset(void)
{
  app::tool::ButtonManager* pBtnMan = this->GetButtonManager();
  pBtnMan->SetButtonVisible( BTN_ID_BOXLIST,true );// NMCat[3851]a  リセットをかけている
}



GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )

