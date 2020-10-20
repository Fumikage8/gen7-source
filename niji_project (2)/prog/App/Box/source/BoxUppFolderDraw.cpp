//==============================================================================
/**
 * @file	  BoxUppFolderDraw.cpp
 * @brief	  フォルダー下画面描画
 * @author	ohno
 * @data	  2015.08.15
 */
// =============================================================================

#include "BoxUppFolderDraw.h"

//素材
#include <arc_def_index/arc_def.h>
#include <arc_index/boxLyt.gaix>

#include <niji_conv_header/app/box/res2d/fldupp.h>
#include <niji_conv_header/app/box/res2d/fldupp_anim_list.h>
#include <niji_conv_header/app/box/res2d/fldupp_pane.h>
#include <niji_conv_header/app/box/res2d/bgrupp.h>
#include <niji_conv_header/app/box/res2d/bgrupp_anim_list.h>
#include <niji_conv_header/app/box/res2d/bgrupp_pane.h>

#include "arc_index/message.gaix"
#include <niji_conv_header/message/msg_box.h>

#include <Layout/include/gfl2_LayoutRenderPath.h>

#include "SaveData/include/BoxSave.h"
#include "SaveData/include/BoxPokemonSave.h"
#include "AppLib/include/Tool/PokeIcon.h"
#include "AppLib/include/Tool/ItemIcon.h"

#include "App/Box/include/BoxAppParam.h"
#include "App/Box/include/BoxAppSetup.h"
#include "BoxSimpleAccesser.h"

#include "PokeTool/include/PokeTool.h"


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )




//--------------------------------------------------------------------------





//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

//----------------------------------------------------------------------
/**
 * @brief コンストラクタ
 * @param heap  ヒープ
 */
//----------------------------------------------------------------------
BoxUppFolderDraw::BoxUppFolderDraw( app::util::Heap * heap, app::util::AppRenderingManager* pAppRenderingManager ) :
app::ui::UIView( heap, heap->GetSystemAllocator(), heap->GetDeviceAllocator() )
,mpAppHeap( heap )
,m_uiListener( NULL )
{
  mpAppRenderingManager = pAppRenderingManager;

}

/// デストラクタ
BoxUppFolderDraw::~BoxUppFolderDraw( void )
{
}

//----------------------------------------------------------------------
/**
 *  @brief  初期化
 */
//----------------------------------------------------------------------

void BoxUppFolderDraw::Init(APP_PARAM* pAppParam, void* pAppLytBuff, BoxAppSetup* pBoxAppSetup)
{

  mpAppParam = pAppParam;
  mpMsgData = mpAppParam->pMsgData;

  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( mpAppHeap, false );

	mpBoxAppSetup=pBoxAppSetup;
  Initialize2D( mpAppHeap,pAppLytBuff );

  InitializeText(mpAppHeap);

  SetPokemonPanelVisible(false);//GFNMcat[3249]

 // SetInputListener( this );

//  mBoxPartsDraw.Setup(pBoxAppSetup,UIView::GetG2DUtil());


}

//----------------------------------------------------------------------
/**
 *  @brief  終了処理
 */
//----------------------------------------------------------------------

bool BoxUppFolderDraw::End(void)
{
 // app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  //  pG2DUtil->DeleteMessageWindow();
  FinalizeText();
  Delete2D();
  DeleteGraphicsSystem();
  return true;
}


//--------------------------------------------------------------------------
/**
 * @brief   更新
 */
//--------------------------------------------------------------------------
void BoxUppFolderDraw::Update(void)
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
void BoxUppFolderDraw::Draw(gfl2::gfx::CtrDisplayNo no)
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  pG2DUtil->AddDrawableLytWkForOneFrame( mpAppRenderingManager, no, LYT_WORK_NORMAL );


}


//--------------------------------------------------------------------------
/**
 * @brief   破棄チェック
 * @return  trueで破棄完了
 */
//--------------------------------------------------------------------------
bool BoxUppFolderDraw::IsExit( void )
{
  return true;
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
void BoxUppFolderDraw::Initialize2D( app::util::Heap * heap, void* pAppLytBuff )
{
  // アーカイブIDテーブル
  app::util::G2DUtil::LytResData resTbl[LYT_WORK_MAX] = {
    {  pAppLytBuff, 1,  app::util::G2DUtil::ATTACH_ROOT, },
  };

  app::util::G2DUtil::LytwkData setupData[LYT_WORK_MAX] = {
    {//
      0,
      LYTRES_FLDUPP_BOX_FLD_UPP_000_BFLYT,
      LA_FLDUPP_BOX_FLD_UPP_000___NUM,
      pAppLytBuff,
      true,
      app::util::G2DUtil::SETUP_UPPER,
      true
      },
  };


  Create2D( heap, NULL, LYT_WORK_MAX,
            resTbl, GFL_NELEMS(resTbl),
            setupData, GFL_NELEMS(setupData),
            GARC_message_box_DAT );


  MessageLine1Close();
}


void BoxUppFolderDraw::SetDispActive(bool bActive)
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
void BoxUppFolderDraw::SelectMenu( u32 id )
{

  OHNO_PRINT("選んだ項目 %d\n",id);
  
  m_uiListener->OnButtonAction(100+id);

}


//----------------------------------------------------------------------
/**
 *	@brief  初期テキスト表示
 */
//----------------------------------------------------------------------


void BoxUppFolderDraw::InitializeText( app::util::Heap * pheap )
{
  mpTempStrBuf = GFL_NEW( pheap->GetDeviceHeap() ) gfl2::str::StrBuf( TEMP_STRBUF_LENGTH, pheap->GetDeviceHeap() );
  mpTempStrBuf2 = GFL_NEW( pheap->GetDeviceHeap() ) gfl2::str::StrBuf( TEMP_STRBUF_LENGTH, pheap->GetDeviceHeap() );
  mpWordSet = GFL_NEW( pheap->GetDeviceHeap() ) print::WordSet( pheap->GetDeviceHeap() );

}


void BoxUppFolderDraw::FinalizeText(void)
{
  GFL_SAFE_DELETE( mpWordSet );
  GFL_SAFE_DELETE( mpTempStrBuf2 );
  GFL_SAFE_DELETE( mpTempStrBuf );
}

void BoxUppFolderDraw::SetMessage( int msgid )
{
}



void BoxUppFolderDraw::DrawMessageLine1(int messageno)
{
}

void BoxUppFolderDraw::MessageLine1Close(void)
{
}

static int icontbl[]={
  PANENAME_BOX_FLD_UPP_000_PANE_PICON_00,
  PANENAME_BOX_FLD_UPP_000_PANE_PICON_01,
  PANENAME_BOX_FLD_UPP_000_PANE_PICON_02,
  PANENAME_BOX_FLD_UPP_000_PANE_PICON_03,
  PANENAME_BOX_FLD_UPP_000_PANE_PICON_04,
  PANENAME_BOX_FLD_UPP_000_PANE_PICON_05,
  PANENAME_BOX_FLD_UPP_000_PANE_PICON_06,
  PANENAME_BOX_FLD_UPP_000_PANE_PICON_07,
  PANENAME_BOX_FLD_UPP_000_PANE_PICON_08,
  PANENAME_BOX_FLD_UPP_000_PANE_PICON_09,
  PANENAME_BOX_FLD_UPP_000_PANE_PICON_10,
  PANENAME_BOX_FLD_UPP_000_PANE_PICON_11,
  PANENAME_BOX_FLD_UPP_000_PANE_PICON_12,
  PANENAME_BOX_FLD_UPP_000_PANE_PICON_13,
  PANENAME_BOX_FLD_UPP_000_PANE_PICON_14,
  PANENAME_BOX_FLD_UPP_000_PANE_PICON_15,
  PANENAME_BOX_FLD_UPP_000_PANE_PICON_16,
  PANENAME_BOX_FLD_UPP_000_PANE_PICON_17,
  PANENAME_BOX_FLD_UPP_000_PANE_PICON_18,
  PANENAME_BOX_FLD_UPP_000_PANE_PICON_19,
  PANENAME_BOX_FLD_UPP_000_PANE_PICON_20,
  PANENAME_BOX_FLD_UPP_000_PANE_PICON_21,
  PANENAME_BOX_FLD_UPP_000_PANE_PICON_22,
  PANENAME_BOX_FLD_UPP_000_PANE_PICON_23,
  PANENAME_BOX_FLD_UPP_000_PANE_PICON_24,
  PANENAME_BOX_FLD_UPP_000_PANE_PICON_25,
  PANENAME_BOX_FLD_UPP_000_PANE_PICON_26,
  PANENAME_BOX_FLD_UPP_000_PANE_PICON_27,
  PANENAME_BOX_FLD_UPP_000_PANE_PICON_28,
  PANENAME_BOX_FLD_UPP_000_PANE_PICON_29,
};



void BoxUppFolderDraw::DrawTitle(int boxpos)
{

  mpTempStrBuf->SetStr(mpAppParam->pBoxData->GetTrayName(boxpos));

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytTextBox * pPane = pLytWk->GetTextBoxPane( PANENAME_BOX_FLD_UPP_000_PANE_TEXTBOX_00 );

  pG2DUtil->SetTextBoxPaneString( pPane, mpTempStrBuf );


  {
    int ppno = mpAppParam->pBoxData->GetWallPaper(boxpos);
    gfl2::lyt::LytPane* pane1 = pLytWk->GetPane(PANENAME_BOX_FLD_UPP_000_PANE_BG_00);
    gfl2::lyt::LytPane* pane2 = pLytWk->GetPane(PANENAME_BOX_FLD_UPP_000_PANE_BUTTON_MAIN );
    pG2DUtil->SetTexMap( pane1 , LYT_WORK_NORMAL, LYTRES_FLDUPP_BOX_WP01_BFLIM + ppno );
    pG2DUtil->SetTexMap( pane2 , LYT_WORK_NORMAL, LYTRES_FLDUPP_BOX_NAME01_BFLIM + ppno );
  }



}

void BoxUppFolderDraw::DrawPokeIcon(int pos, App::Box::PokemonIconDraw* pPara)
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytParts* pParts = pLytWk->GetPartsPane( icontbl[ pos ] );
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );
  gfl2::lyt::LytPicture * pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ICONPARTS_POKE, &resID );
  
  if(pPara->simp.monsNo != MONSNO_NULL){
    mpAppParam->pPokeIcon->ReplacePaneTexture(pos, &pPara->simp, pPane );
    pPane->SetVisible(true);
  }
  else{
    pPane->SetVisible(false);
  }

  pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ICONPARTS_POKESHADOW, &resID );
  pPane->SetVisible(false);

  DrawPokeIconState(pos,pPara);
  
}


void BoxUppFolderDraw::SetPokemonPanelVisible(bool bvisible)
{
//  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytPane * pPane = pLytWk->GetPane( PANENAME_BOX_FLD_UPP_000_PANE_ALL );
  
  pPane->SetVisible(bvisible);
}





//アイコンのテクスチャー以外の状態反映
void BoxUppFolderDraw::DrawPokeIconState(int pos, App::Box::PokemonIconDraw* pPkd)
{
  // gfl2::lyt::LytParts* pParts;
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( LYT_WORK_NORMAL );
  gfl2::lyt::LytMultiResID resID = pG2DUtil->GetLayoutResourceID( LYT_WORK_NORMAL );
  gfl2::lyt::LytParts* pParts = pLytWk->GetPartsPane( icontbl[ pos ] );
  gfl2::lyt::LytPicture * pPane = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ICONPARTS_POKE, &resID );
  gfl2::lyt::LytPicture * pPaneM = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_MATERIAL_ICONPARTS_POKE_OF_ICONPARTS_POKE, &resID );

  gfl2::lyt::LytPicture * pPaneTeam = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ICONPARTS_TEAM, &resID );
  gfl2::lyt::LytPicture * pPaneLock = pLytWk->GetPicturePane( pParts, PANENAME_BOX_ICN_LOW_000_PANE_ICONPARTS_LOCK, &resID );
  

  if(pPkd->carryup){//持ち上がっているときは半透明
  }
  else if(!pPkd->bHide)
  {
    pPane->SetVisible(true);
  }
  else{
    pPane->SetVisible(false);
  }

  if(!pPkd->bHide)
  {
    nw::ut::Color8 b = pG2DUtil->GetMaterialColor( pPaneM, app::util::G2DUtil::COLOR_INDEX_WHITE );
    if(pPkd->bblack)
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

  if(BOX_CHANGE_EDITTYPE_ITEM == mpBoxAppSetup->GetSelectMode()){  //アイテム部分

    if(pPkd->bItem){
      pPane->SetAlpha(255);
    }
    else{
      pPane->SetAlpha(App::Box::BoxSimpleAccesser::POKEICON_PASSIVE_NUM);
    }
  }
  else{
    if(pPkd->bPassive || pPkd->carryup){
      pPane->SetAlpha(App::Box::BoxSimpleAccesser::POKEICON_PASSIVE_NUM);
    }
    else{
      pPane->SetAlpha(255);
    }

  }

  pPaneTeam->SetVisible(false);
  pPaneLock->SetVisible(false);

  if(!pPkd->bHide)  //表示ＯＫのとき
  {
    if(pPkd->bLock){
      pPaneLock->SetVisible(true);
    }
    else if( mpBoxAppSetup->GetBoxSimpleAccesser()->IsTeamIcon( pPkd )){
      pPaneTeam->SetVisible(true);
    }
  }
}




GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )

