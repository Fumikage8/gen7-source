//======================================================================
/**
 * @file	KawaigariUIManager.C++
 * @brief	Kawaigari All UI Interface / Manager Code (Buttons, Interface, SlideUI, etc)
 * @author	PETE
 * @data	2015.11.02
 */
//======================================================================

#include "KawaigariUIManager.h"

#include "AppLib/include/Util/app_util_FileAccessor.h"
#include "AppLib/include/Util/app_util_Heap.h"

#include "arc_def_index/arc_def.h"

#include "niji_conv_header/app/kawaigari/res2d/kw_layout.h"
#include "niji_conv_header/app/kawaigari/res2d/kw_layout_pane.h"
#include "niji_conv_header/app/kawaigari/res2d/kw_layout_anim_list.h"

#include "arc_index/message.gaix"
#include "niji_conv_header/message/debug/msg_debug_item_make.h"

// Fade
#include <Fade/include/gfl2_FadeManager.h>

#include "KawaigariMain.h"

// Sound
#include "Sound/include/Sound.h"

#include "Print/include/SystemFont.h"

#include "KawaigariRenderSystem.h"

const int NUM_MAME_TEX = 15; 
const int NUM_MAME_TEX_TUTORIAL = 4; 

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

KawaigariUIManager::KawaigariUIManager(app::util::Heap *heap, app::util::AppRenderingManager *renderManager)
: app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
, mHeap( heap )
, mRenderingManager( renderManager )
{  
  mPressedCancelButton = false;
  mChangePokemon = false;

  iconPane = -1;
  guiMode = GUI_MODE_NONE;  

  // Attach Menus:
  mameMenu.SetCoreUISys(this);
  careMenu.SetCoreUISys(this);
  tutorialInfo.SetCoreUISys(this);

  mTexSys = NULL; 
  mTexSys = GFL_NEW( mHeap->GetDeviceHeap() ) app::tool::LytTexReplaceSystem( mHeap->GetDeviceHeap(), mHeap->GetDeviceHeap(), (64*64*2)+512, NUM_MAME_TEX );
    
 
  mCreateNumFont = false;

  mInitSeq = 0;
  mEndSeq = 0;

  mDryerActive = false;
  
  lastUITouchX = lastUITouchY = -1000.0f; // Set to offscreen
  touchReleaseX = touchReleaseY = -1000.0f; // Set to offscreen
}

KawaigariUIManager::~KawaigariUIManager(void)
{  
}

void KawaigariUIManager::CreateNumFont(void)
{
  
  if( print::SystemFont_GetNumFont() == NULL )
  {
    print::SystemFont_LoadNumFont( mHeap->GetSystemHeap(), mHeap->GetDeviceHeap() );
    mCreateNumFont = true;
  }
  
}

bool KawaigariUIManager::SetNumFont(void)
{
  
  if( mCreateNumFont == false )
  {
    return true;
  }
  if( print::SystemFont_IsLoadNumFont() == false )
  {
    return false;
  }
  print::SystemFont_SetLoadNumFont( mHeap->GetSystemHeap() );
  print::SystemFont_SetLytSystemNumFont();
  
  return true;
}

void KawaigariUIManager::DeleteNumFont(void)
{
  if( mCreateNumFont == true )
  {
    print::SystemFont_ResetNumFontInfo();
    print::SystemFont_DeleteNumFont();
  }
  
}

void KawaigariUIManager::LoadLayoutResource(void)
{
  
  app::util::FileAccessor::FileOpenSync( ARCID_KW_LAYOUT, mHeap->GetDeviceHeap() );
  app::util::FileAccessor::FileLoadSync(
    ARCID_KW_LAYOUT, 0, &resourceBuf, mHeap->GetDeviceHeap(), false, 128 );
  app::util::FileAccessor::FileCloseSync( ARCID_KW_LAYOUT );
    
}

void KawaigariUIManager::Initialize2D( void )
{
  
  // Resource Data:
  const app::util::G2DUtil::LytResData res_tbl[] =
  {
    { resourceBuf, 0, app::util::G2DUtil::ATTACH_ROOT },
  };
  
  // Layout Data:
  const app::util::G2DUtil::LytwkData data_tbl[] =
  {
    {
      0,
        LYTRES_KW_LAYOUT_KAWAIGARI_MIN_LOW_000_BFLYT,
        LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000___NUM,
        resourceBuf,
        false,
        app::util::G2DUtil::SETUP_LOWER,
        true
    },
    {
      0,
        LYTRES_KW_LAYOUT_KAWAIGARI_ICN_LOW_000_BFLYT,
        LA_KW_LAYOUT_KAWAIGARI_ICN_LOW_000___NUM,
        resourceBuf,
        false,
        app::util::G2DUtil::SETUP_LOWER,
        true
      },
    {
      0,
        LYTRES_KW_LAYOUT_KAWAIGARI_MESSAGE_UPP_000_BFLYT,
        LA_KW_LAYOUT_KAWAIGARI_MESSAGE_UPP_000___NUM,
        resourceBuf,
        false,
        app::util::G2DUtil::SETUP_UPPER,
        true
    },
      {
        0,
          LYTRES_KW_LAYOUT_KAWAIGARI_INFO_BFLYT,
          LA_KW_LAYOUT_KAWAIGARI_INFO___NUM,
          resourceBuf,
          false,
          app::util::G2DUtil::SETUP_LOWER,
          true
      },
  };
  

  
  m_pMsgData = GFL_NEW(mHeap->GetDeviceHeap()) gfl2::str::MsgData( print::GetMessageArcId(), GARC_message_pokeparure_DAT, mHeap->GetDeviceHeap() );
  m_pWordSet = GFL_NEW(mHeap->GetDeviceHeap()) print::WordSet( mHeap->GetDeviceHeap() );
 
  Create2D(
    mHeap,
    NULL,
    4,
    res_tbl,
    GFL_NELEMS(res_tbl),
    data_tbl,
    GFL_NELEMS(data_tbl),
    m_pMsgData,
    m_pWordSet );
  
   int arcID = ARCID_KAWAIGARI_MAME_TEX;
   gfl2::fs::AsyncFileManager * man = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();

   for(int z=0;z<15;z++)
   {   
      if( man->GetArcFile(arcID) == NULL )
      {
        mTexSys->LoadTexture( mHeap->GetDeviceHeap(), z, arcID, z, true );
      }
   }
      
   // Set Tutorial Res:
   gfl2::lyt::LytMultiResID resID = GetG2DUtil()->GetLayoutResourceID( 0 );   
   gfl2::lyt::LytSys* lytSys = GetG2DUtil()->GetLayoutSystem();   
   mTexInfoRes = lytSys->GetResource( &resID );
   
   coreSys->GetMameManager()->SetupMameIcons();
   GetG2DUtil()->StartAnime( 2, LA_KW_LAYOUT_KAWAIGARI_MESSAGE_UPP_000__MESSAGE_L2_OUT);
   GetG2DUtil()->SetPaneVisible(2,PANENAME_KAWAIGARI_MESSAGE_UPP_000_PANE_MESSAGE_L2,false);
        
}

void KawaigariUIManager::OpenTutorialWindow(void)
{
  tutorialInfo.SetState(GUI_STATE_MAIN);
}

bool KawaigariUIManager::IsTutorialOver(void)
{
  return(tutorialInfo.IsTutorialOver());
}

void KawaigariUIManager::SetCareTextMessage(int index)
{  
  GetG2DUtil()->SetPaneVisible(2,PANENAME_KAWAIGARI_MESSAGE_UPP_000_PANE_MESSAGE_L2,true);

  app::util::G2DUtil* pG2D = this->GetG2DUtil();
  gfl2::lyt::LytWk* lytWk = pG2D->GetLayoutWork( 2 );
  gfl2::lyt::LytParts *parts = lytWk->GetPartsPane( PANENAME_KAWAIGARI_MESSAGE_UPP_000_PANE_MESSAGE_L2 );
  
  gfl2::lyt::LytMultiResID resID = pG2D->GetLayoutResourceID( 0 );              
//  lytWk->GetPane(parts,PANENAME_COMMON_MSG_UPP_000_PANE_TEXTBOX_00,&resID);

  GetG2DUtil()->SetTextBoxPaneString(lytWk->GetTextBoxPane(parts,PANENAME_COMMON_MSG_UPP_000_PANE_TEXTBOX_00,&resID),index);  
  lytWk->GetTextBoxPane(parts,PANENAME_COMMON_MSG_UPP_000_PANE_TEXTBOX_00,&resID)->SetVisible(true);
}

void KawaigariUIManager::Terminate2D(void)
{
  Delete2D();

  GFL_SAFE_DELETE( m_pWordSet );
  GFL_SAFE_DELETE( m_pMsgData );
  GFL_SAFE_DELETE( mTexSys );  
  
}

bool KawaigariUIManager::TouchingPane(int layoutIndex,int paneIndex)
{
  gfl2::math::VEC3 pos = GetG2DUtil()->GetPaneLocalPos(layoutIndex,paneIndex);
  nw::ut::Rect r = GetG2DUtil()->GetPaneRect(layoutIndex,paneIndex);

  if(lastUITouchX < r.left + pos.x) return(false);
  else if(lastUITouchX > pos.x + r.right) return(false);
  else if(lastUITouchY > pos.y - r.bottom) return(false);
  else if(lastUITouchY < pos.y - r.top) return(false);

  return(true);
}

void KawaigariUIManager::SetMameButtonLarge(bool b)
{
  gfl2::math::VEC2 scl;

  if(b)
  {  
    scl.x = scl.y = 1.2f;
  }
  else
  {
    scl.x = scl.y = 1.0f;
  }

  GetG2DUtil()->SetScale(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAMEBUTTON ,scl);
  
}

void KawaigariUIManager::SetCareButtonNotifyMode(bool b)
{
  gfl2::math::VEC2 scl;

  if(b)
  {  
    static float sinPos = 0.0f;
    float sinV = gfl2::math::SinDeg(sinPos) * 0.1f;
    if(sinV < 0.0f) sinV = -sinV;
    scl.x = scl.y = 1.0f + sinV;
    sinPos += 10.0f;
  }
  else
  {
    scl.x = scl.y = 1.0f;
  }

  GetG2DUtil()->SetScale(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_CAREBUTTON ,scl);

}

void KawaigariUIManager::SetCursorOffscreen(void)
{
  SetTouchPos(-1000.0f,-1000.0f);
}

void KawaigariUIManager::SetHitTouch(int x,int y)
{
  hitTouchX = x - 160;
  hitTouchY = 120 - y;
}

void KawaigariUIManager::UpdateUITouchPos(void)
{
  gfl2::ui::TouchPanel * touchPanel;
  touchPanel = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetTouchPanel( gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD );

  static float lastX,lastY;

  if(touchPanel->IsTouch())
  {
    // Update Touch Pos:    
    gfl2::math::VEC3 pos;
    touchPanel->GetXY(&pos);

    lastUITouchX = pos.x - 160;
    lastUITouchY = 120 - pos.y;

    lastX = lastUITouchX;
    lastY = lastUITouchY;
  }
  else
  {
    lastUITouchX = lastUITouchY = -1000.0f;    
  }

  if(touchPanel->IsTouchRelease())
  {
    touchReleaseX = lastX;
    touchReleaseY = lastY;
  }
  
}

void KawaigariUIManager::Update(void)
{
  if(mButtonDisableCount>0 && GuiMode()==GUI_MODE_NORMAL)
  {
    mButtonDisableCount--;    
  }

  if(mButtonDisableCount==0 && GuiMode()==GUI_MODE_NORMAL)
  {
    mButtonDisableCount=-1;
    EnablePokemonSelectButton();
    EnableBackButton();
  }

  app::ui::UIView::Update();

  UpdateUITouchPos();
  mameMenu.Update();
  careMenu.Update();
  tutorialInfo.Update();

  if(GuiMode()==GUI_MODE_NORMAL)
  {
    GetG2DUtil()->StartAnime( 0, LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__MAMEBUTTON_MODE_OFF);
    GetG2DUtil()->StartAnime( 0, LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__CAREBUTTON_MODE_OFF);
  }

  if(!coreSys->GetTroubleManager()->HasObjects())
  {
    GetG2DUtil()->SetPaneVisible(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_CAREBUTTON ,false);
  }
  else if(GuiMode()==GUI_MODE_NORMAL)
  {
    if(!GetG2DUtil()->GetPaneVisible(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_CAREBUTTON))
      GetG2DUtil()->SetPaneVisible(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_CAREBUTTON ,true);
  }
    

  /*
  // Turn off UI elements depending on mode:
  switch(coreSys->Mode())
  {
    case KAWAIGARI_MODE_NORMAL: break;

    case KAWAIGARI_MODE_FIELD:
      GetG2DUtil()->SetPaneVisible(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_POKESELECTBUTTON ,false);
      GetG2DUtil()->SetPaneVisible(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_CAREBUTTON ,false);
      break;

    case KAWAIGARI_MODE_FESTIVAL_PLAYER:
      GetG2DUtil()->SetPaneVisible(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_POKESELECTBUTTON ,false);
      GetG2DUtil()->SetPaneVisible(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_CAREBUTTON ,false);
      break;

    case KAWAIGARI_MODE_FESTIVAL_EVENT:
      GetG2DUtil()->SetPaneVisible(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_POKESELECTBUTTON ,false);
      break;

    case KAWAIGARI_MODE_FESTIVAL_NPC:
      GetG2DUtil()->SetPaneVisible(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_POKESELECTBUTTON ,false);
      GetG2DUtil()->SetPaneVisible(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_CAREBUTTON ,false);
      break;

    case KAWAIGARI_MODE_RESORT:
      GetG2DUtil()->SetPaneVisible(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_POKESELECTBUTTON ,false);
      break;

  }
  */


  if(coreSys->GetTroubleManager()->HasObjects()) 
  {
    GetG2DUtil()->SetPaneVisible(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAMEBUTTON ,false);

    if(GuiMode()==GUI_MODE_NORMAL)
    {
      SetCareButtonNotifyMode(true);
    }
    else SetCareButtonNotifyMode(false);
  }
  else SetCareButtonNotifyMode(false);
  
  
  //GetG2DUtil()->SetPaneVisible(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_FACEGAMEICON ,false);
 // GetG2DUtil()->SetPaneVisible(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MESSAGE_TOP ,false);
  //GetG2DUtil()->SetPaneVisible(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_FACEICON ,false);

  

  // Pressed B:Cancel ?
  if(GuiMode()==GUI_MODE_NORMAL)
  {

    if(GetG2DUtil()->IsAnimeEnd(0,LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000_BUTTON_IN))
    {
      SetInputEnabled(true);
    }
    
    
  }

  
}

bool KawaigariUIManager::IsTouching(void)
{
  // Touch Pen:
  gfl2::ui::TouchPanel * touchPanel;

  touchPanel = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetTouchPanel( gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD );

  if(touchPanel->IsTouchRelease()) return(false);

  return(touchPanel->IsTouch());
}


void KawaigariUIManager::SetIconPaneVisible(int pane,bool b)
{  
  GetG2DUtil()->SetPaneVisible(1,pane,b);
  if(b) iconPane = pane;

  gfl2::math::VEC3 pos(touchX,touchY,0.0f);
  if(iconPane == PANENAME_KAWAIGARI_ICN_LOW_000_PANE_ICON_TAP0_00)
  {
    gfl2::math::VEC3 posV(touchReleaseX,touchReleaseY,0.0f);
    GetG2DUtil()->SetPanePos(1,iconPane,&posV);
  }
  else if(iconPane == PANENAME_KAWAIGARI_ICN_LOW_000_PANE_ICON_HIT0_00)
  {
    gfl2::math::VEC3 posV(hitTouchX,hitTouchY,0.0f);
    GetG2DUtil()->SetPanePos(1,iconPane,&posV);
  }
  else if(iconPane == PANENAME_KAWAIGARI_ICN_LOW_000_PANE_FINGERICON)
  {
    gfl2::math::VEC3 lastTouchPos(lastUITouchX,lastUITouchY,0.0f); 
    if(coreSys->GetPokemonAI()->IsTouching()) GetG2DUtil()->SetPanePos(1,iconPane,&lastTouchPos);
  }
  else if(iconPane==PANENAME_KAWAIGARI_ICN_LOW_000_PANE_ICON_BURN0_00)
  {
    pos.y += 16;
    GetG2DUtil()->SetPanePos(1,iconPane,&pos);
  }
  else if(iconPane==PANENAME_KAWAIGARI_ICN_LOW_000_PANE_ICON_WATER0_00)
  {
    pos.y += 16;
    GetG2DUtil()->SetPanePos(1,iconPane,&pos);
  }
  else if(iconPane!=-1) GetG2DUtil()->SetPanePos(1,iconPane,&pos);
}

void KawaigariUIManager::ResetTouchIcon(void)
{
  iconPane = -1;  
}

void KawaigariUIManager::StopIconAnimation(int ani)
{
  GetG2DUtil()->StopAnime(1,ani);
}

void KawaigariUIManager::SetIconAnimation(int ani)
{    
  
  GetG2DUtil()->StartAnime( 1, ani );  

  gfl2::math::VEC3 pos(touchX,touchY,0.0f);
  if(iconPane == PANENAME_KAWAIGARI_ICN_LOW_000_PANE_ICON_TAP0_00)
  {
    gfl2::math::VEC3 posV(touchReleaseX,touchReleaseY,0.0f);
    GetG2DUtil()->SetPanePos(1,iconPane,&posV);
  }
  else if(iconPane == PANENAME_KAWAIGARI_ICN_LOW_000_PANE_ICON_HIT0_00)
  {
    gfl2::math::VEC3 posV(hitTouchX,hitTouchY,0.0f);
    GetG2DUtil()->SetPanePos(1,iconPane,&posV);
  }
  else if(iconPane == PANENAME_KAWAIGARI_ICN_LOW_000_PANE_FINGERICON)
  {
    gfl2::math::VEC3 lastTouchPos(lastUITouchX,lastUITouchY,0.0f); 
    if(coreSys->GetPokemonAI()->IsTouching()) GetG2DUtil()->SetPanePos(1,iconPane,&lastTouchPos);
  }
  else if(iconPane==PANENAME_KAWAIGARI_ICN_LOW_000_PANE_ICON_BURN0_00)
  {
    pos.y += 16;
    GetG2DUtil()->SetPanePos(1,iconPane,&pos);
  }
  else if(iconPane==PANENAME_KAWAIGARI_ICN_LOW_000_PANE_ICON_WATER0_00)
  {    
    pos.y += 16;
    GetG2DUtil()->SetPanePos(1,iconPane,&pos);
  }
  else if(iconPane!=-1) GetG2DUtil()->SetPanePos(1,iconPane,&pos);
}

void KawaigariUIManager::ConvertRawToTouchPos(int &x,int &y)
{
  x = x - 160;
  y = 120 - y;
}

void KawaigariUIManager::SetTouchPos(float x,float y)
{
  touchRawX = (u32)x;
  touchRawY = (u32)y;
  ///GFL_PRINT("*Touch Pos: %f,%f \n",x,y);
  touchX = x - 160;
  touchY = 120 - y;

  gfl2::math::VEC3 pos(touchX,touchY,0.0f);

  gfl2::math::VEC3 posRelease(touchReleaseX,touchReleaseY,0.0f);  
  gfl2::math::VEC3 lastTouchPos(lastUITouchX,lastUITouchY,0.0f);  

  if(iconPane!=-1 && coreSys->GetPokemonAI()->IsTouching()) 
  {
    switch(iconPane)
    {
      case PANENAME_KAWAIGARI_ICN_LOW_000_PANE_ICON_BURN0_00: 
        {
          gfl2::math::VEC3 hitPos(hitTouchX,hitTouchY+16,0.0f);
          GetG2DUtil()->SetPanePos(1,iconPane,&hitPos);
        } 
        break;

    case PANENAME_KAWAIGARI_ICN_LOW_000_PANE_ICON_BIRIBIRI_00: 
//    case PANENAME_KAWAIGARI_ICN_LOW_000_PANE_ICON_WATER0_00: break;
    case PANENAME_KAWAIGARI_ICN_LOW_000_PANE_ICON_HURT0_00: 
    case PANENAME_KAWAIGARI_ICN_LOW_000_PANE_ICON_FREEZE0_00:
  //  case PANENAME_KAWAIGARI_ICN_LOW_000_PANE_ICON_JUNK0_00: break;
      {
        gfl2::math::VEC3 hitPos(hitTouchX,hitTouchY,0.0f);
        GetG2DUtil()->SetPanePos(1,iconPane,&hitPos);
      }      
      break;

    case PANENAME_KAWAIGARI_ICN_LOW_000_PANE_FINGERICON: 
      GetG2DUtil()->SetPanePos(1,iconPane,&lastTouchPos);
      break;

    case PANENAME_KAWAIGARI_ICN_LOW_000_PANE_ICON_HIT0_00:
      break;

    case PANENAME_KAWAIGARI_ICN_LOW_000_PANE_ICON_TAP0_00:      
      GetG2DUtil()->SetPanePos(1,iconPane,&posRelease);
      break;

    case PANENAME_KAWAIGARI_ICN_LOW_000_PANE_ICON_WATER0_00:
      pos.y+=16;
      GetG2DUtil()->SetPanePos(1,iconPane,&pos); break;      

      default:
        GetG2DUtil()->SetPanePos(1,iconPane,&pos); break;
    }
    
  }
}

void KawaigariUIManager::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{  
  GetG2DUtil()->AddDrawableLytWkForOneFrame( mRenderingManager, displayNo, 0 );
  GetG2DUtil()->AddDrawableLytWkForOneFrame( mRenderingManager, displayNo, 1 );
  GetG2DUtil()->AddDrawableLytWkForOneFrame( mRenderingManager, displayNo, 2 );
}

bool KawaigariUIManager::Init( void )
{
  switch(mInitSeq)
  {
    case 0:
      CreateNumFont();
      mInitSeq++;
      break;

    case 1:
      if(SetNumFont()) mInitSeq++;
      break;
  
    case 2:
      LoadLayoutResource();
      Initialize2D();
      CreateButtons();

      DisablePokemonSelectButton();
      DisableBackButton();
  
      SetInputEnabled(false);

      SetInputListener( this );

    ///  GetG2DUtil()->StartAnime( 0, LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000_BUTTON_OUT);
      mInitSeq++;
      break;

    case 3:
      //GetG2DUtil()->SetPaneVisible(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_FACEGAMEICON ,false);
      GetG2DUtil()->SetPaneVisible(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MESSAGE_TOP ,false);
      GetG2DUtil()->SetPaneVisible(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_FACEICON ,false);
      tutorialInfo.Setup();
      return(true);      
  }

  return(false);
  
}


const int BUTTON_ID_MAMEMODE = 0;
const int BUTTON_ID_CAREMODE = 1;
const int BUTTON_ID_CHANGEPOKEMON = 2;
const int BUTTON_ID_EXIT = 3;
const int BUTTON_ID_MAX = 4;

void KawaigariUIManager::CreateButtons(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  app::ui::ButtonInfoEx * info = GFL_NEW_LOW_ARRAY(mHeap->GetDeviceHeap()) app::ui::ButtonInfoEx[BUTTON_ID_MAX];

  CreateNormalButton( &info[BUTTON_ID_MAMEMODE] );
  CreateExitButton( &info[BUTTON_ID_EXIT] );

  app::ui::UIResponder::CreateButtonManager( mHeap, lytwk, info, BUTTON_ID_MAX );

  GFL_DELETE_ARRAY info;

  app::tool::ButtonManager * man  = app::ui::UIResponder::GetButtonManager();
  ///man->SetButtonBindKey( BUTTON_ID_EXIT, gfl2::ui::BUTTON_B, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );
  
  man->SetButtonSelectSE( BUTTON_ID_MAMEMODE, SEQ_SE_DECIDE1 );
  man->SetButtonSelectSE( BUTTON_ID_CAREMODE, SEQ_SE_DECIDE1 );
  man->SetButtonSelectSE( BUTTON_ID_CHANGEPOKEMON, SEQ_SE_DECIDE1 ); 
  man->SetButtonSelectSE( BUTTON_ID_EXIT, SEQ_SE_CANCEL1 ); 

  
}

void KawaigariUIManager::DisableEatModeButton(void)
{
  app::tool::ButtonManager * man  = app::ui::UIResponder::GetButtonManager();
  man->SetButtonPassive(BUTTON_ID_MAMEMODE,false);
}

void KawaigariUIManager::EnableEatModeButton(void)
{
  app::tool::ButtonManager * man  = app::ui::UIResponder::GetButtonManager();
  man->SetButtonActive(BUTTON_ID_MAMEMODE,false);
}

void KawaigariUIManager::DisablePokemonSelectButton(void)
{
  app::tool::ButtonManager * man  = app::ui::UIResponder::GetButtonManager();
  man->SetButtonPassive(BUTTON_ID_CHANGEPOKEMON,false);
  man->SetButtonInputEnable(BUTTON_ID_CHANGEPOKEMON,false);
}

void KawaigariUIManager::EnablePokemonSelectButton(void)
{
  app::tool::ButtonManager * man  = app::ui::UIResponder::GetButtonManager();
  man->SetButtonActive(BUTTON_ID_CHANGEPOKEMON,false);
  man->SetButtonInputEnable(BUTTON_ID_CHANGEPOKEMON,true);

}

void KawaigariUIManager::DisableBackButton(void)
{
  app::tool::ButtonManager * man  = app::ui::UIResponder::GetButtonManager();
  man->SetButtonPassive(BUTTON_ID_EXIT,false);
  man->SetButtonInputEnable(BUTTON_ID_EXIT,false);
}

void KawaigariUIManager::EnableBackButton(void)
{
  app::tool::ButtonManager * man  = app::ui::UIResponder::GetButtonManager();
  man->SetButtonActive(BUTTON_ID_EXIT,false);
  man->SetButtonInputEnable(BUTTON_ID_EXIT,true);
}

void KawaigariUIManager::DisableCareButton(void)
{
  app::tool::ButtonManager * man  = app::ui::UIResponder::GetButtonManager();
  man->SetButtonPassive(BUTTON_ID_CAREMODE,false);
  man->SetButtonInputEnable(BUTTON_ID_CAREMODE,false);
}

void KawaigariUIManager::EnableCareButton(void)
{
  app::tool::ButtonManager * man  = app::ui::UIResponder::GetButtonManager();
  man->SetButtonActive(BUTTON_ID_CAREMODE,false);
  man->SetButtonInputEnable(BUTTON_ID_CAREMODE,true);
}


void KawaigariUIManager::SetButtonInfoEx( app::ui::ButtonInfoEx * info, const gfl2::lyt::LytPaneIndex * parts_tbl, const app::ui::ButtonInfoEx * info_tbl, gfl2::lyt::LytPaneIndex bound, u32 max )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  for( u32 i=0; i<max; i++ )
  {
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( parts_tbl[i] );
    info[i] = info_tbl[i];
    info[i].picture_pane = parts;
    info[i].bound_pane   = lytwk->GetBoundingPane( parts, bound, &res_id );
  }
}

void KawaigariUIManager::CreateExitButton( app::ui::ButtonInfoEx * info )
{
  
  
  static const gfl2::lyt::LytPaneIndex parts_tbl[] =
  {
    PANENAME_KAWAIGARI_MIN_LOW_000_PANE_BACKBUTTON,
  };

  static const app::ui::ButtonInfoEx info_tbl[] =
  {
    {
      BUTTON_ID_EXIT, NULL, NULL,
        LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__BACKBUTTON_TOUCH,
        LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__BACKBUTTON_RELEASE,
        LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__BACKBUTTON_CANSEL,
        LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__BACKBUTTON_TOUCH_RELEASE,
        LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__BACKBUTTON_ACTIVE,
        LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__BACKBUTTON_PASSIVE,
    },
  };

  SetButtonInfoEx( info, parts_tbl, info_tbl, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, GFL_NELEMS(parts_tbl) );
  
}

void KawaigariUIManager::CreateNormalButton( app::ui::ButtonInfoEx * info )
{
  static const gfl2::lyt::LytPaneIndex parts_tbl[] =
  {
    PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAMEBUTTON,
    PANENAME_KAWAIGARI_MIN_LOW_000_PANE_POKESELECTBUTTON,    
    PANENAME_KAWAIGARI_MIN_LOW_000_PANE_CAREBUTTON,        
  };

  static const app::ui::ButtonInfoEx info_tbl[] =
  {
    {
      BUTTON_ID_MAMEMODE, NULL, NULL,
        LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__MAMEBUTTON_TOUCH,
        LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__MAMEBUTTON_RELEASE,
        LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__MAMEBUTTON_CANSEL,
        LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__MAMEBUTTON_TOUCH_RELEASE,
        LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__MAMEBUTTON_ACTIVE,
        LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__MAMEBUTTON_PASSIVE,
    },
    {
      BUTTON_ID_CHANGEPOKEMON, NULL, NULL,
        LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__POKESELECTBUTTON_TOUCH,
        LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__POKESELECTBUTTON_RELEASE,
        LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__POKESELECTBUTTON_CANSEL,
        LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__POKESELECTBUTTON_TOUCH_RELEASE,
        LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__POKESELECTBUTTON_ACTIVE,
        LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__POKESELECTBUTTON_PASSIVE,
      }, 
    {
      BUTTON_ID_CAREMODE, NULL, NULL,
        LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__CAREBUTTON_TOUCH,
        LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__CAREBUTTON_RELEASE,
        LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__CAREBUTTON_CANSEL,
        LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__CAREBUTTON_TOUCH_RELEASE,
        LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__CAREBUTTON_ACTIVE,
        LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__CAREBUTTON_PASSIVE,
      },     
      
  };

  SetButtonInfoEx( info, parts_tbl, info_tbl, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, GFL_NELEMS(parts_tbl) );
}

bool KawaigariUIManager::End( void )
{
  switch(mEndSeq)
  {
    case 0:
      tutorialInfo.Finish();
      mEndSeq++;
      break;

    case 1:
      tutorialInfo.Update();
      if(tutorialInfo.IsFinish()) mEndSeq++;
      break;

    case 2:
      Terminate2D();
      DeleteNumFont();
      GflHeapFreeMemory( resourceBuf );
      return(true);
  }
  
  return false;
}

void KawaigariUIManager::ChangeToMainMenu(void)
{
  GetG2DUtil()->StartAnime( 0, LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000_BUTTON_IN);
  SetGUIMode(GUI_MODE_NORMAL);

  EnablePokemonSelectButton();
  EnableBackButton();

  SetInputEnabled(false);
}

void KawaigariUIManager::DisableMainButtons(void)
{
  GetG2DUtil()->StartAnime( 0, LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000_BUTTON_OUT);
  SetInputEnabled(false);

  DisableBackButton();
  DisableCareButton();
  DisableEatModeButton();
  DisablePokemonSelectButton();

  guiMode = GUI_MODE_NONE;
}

int KawaigariUIManager :: GuiMode(void)
{ 
  return(guiMode); 
}

void KawaigariUIManager::EnableMainButtons(void)
{
  GetG2DUtil()->StartAnime( 0, LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000_BUTTON_IN);
  SetInputEnabled(true);

  EnableBackButton();
  EnableCareButton();
  EnableEatModeButton();
  EnablePokemonSelectButton();

  guiMode = GUI_MODE_NORMAL;
}

bool KawaigariUIManager::IsPressingButton(void)
{
  UpdateUITouchPos();

  if(GuiMode()==GUI_MODE_NORMAL && TouchingPane(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_POKESELECTBUTTON)) return(true);
  if(GuiMode()==GUI_MODE_NORMAL && TouchingPane(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAMEBUTTON)) return(true);


  // Only check care button if it is visible:
  if(coreSys->GetTroubleManager()->HasObjects())
  {  
    if(GuiMode()==GUI_MODE_NORMAL && TouchingPane(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_CAREBUTTON)) return(true);
  }
    
  if(GuiMode()==GUI_MODE_NORMAL && TouchingPane(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_BACKBUTTON)) return(true);
  return(false);
}

app::ui::UIInputListener::ListenerResult KawaigariUIManager::OnLayoutPaneEvent( const u32 button_id )
{
  switch(button_id)
  {
    case BUTTON_ID_EXIT:
      if(GuiMode()==GUI_MODE_NORMAL) 
      {
        SetCursorOffscreen();
     //   mPressedCancelButton = true; 
        if(!coreSys->IsDebugMode())
        {
          coreSys->GetPokemonAI()->SetState(AI_STATE_END);          
        }
        else mPressedCancelButton = true; 

        SetInputEnabled(false);
        SetGUIMode(GUI_STATE_END);
      }
      break;

    case BUTTON_ID_CHANGEPOKEMON:
      if(GuiMode()==GUI_MODE_NORMAL)// && TouchingPane(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_POKESELECTBUTTON)) 
      {
        SetCursorOffscreen();
        mPressedCancelButton = true;        
        mChangePokemon = true;        

        if(!coreSys->IsDebugMode())
        {
          coreSys->GetPokemonAI()->SetState(AI_STATE_END);          
        }
        else mPressedCancelButton = true; 

        // Fade to black:
        {          
          gfl2::math::Vector4 blackCol(0.0f,0.0f,0.0f,0.0f);					
          gfl2::math::Vector4 blackCol2(0.0f,0.0f,0.0f,255.0f);					
          gfl2::math::Vector4 *col = &blackCol;
          gfl2::math::Vector4 *col2 = &blackCol2;				
          GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA, col, col2);			
          GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_ALPHA, col, col2);			          
        }

        SetInputEnabled(false);
        SetGUIMode(GUI_STATE_END);
      }
      break;

    case BUTTON_ID_MAMEMODE:
      if(GuiMode()==GUI_MODE_NORMAL ) 
      {                
        if(coreSys->GetPokemonAI()->CanChangeMode())
        {                  
          SetCursorOffscreen();
          coreSys->GetRenderSystem()->GetRenderingPipeLine()->SetUseTouchIDTexture(false);

          coreSys->GetMameManager()->SetupMameIcons();
          mameMenu.SetState(GUI_STATE_MENU_IDLE);
          GetG2DUtil()->StartAnime( 0, LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000_MAME_IN);
          GetG2DUtil()->StartAnime( 0, LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__MAMEBUTTON_MODE_ON);
          SetGUIMode(GUI_MODE_MAME);
          coreSys->GetPokemonAI()->SetState(AI_STATE_EAT);          

          DisableBackButton();
          DisablePokemonSelectButton();
          DisableCareButton();
          DisableEatModeButton();
          
        }
      }
      else 
      {               
        SetCursorOffscreen();
        GetG2DUtil()->StartAnime( 0, LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000_MAME_CANCEL);
        SetGUIMode(GUI_MODE_NORMAL);
        coreSys->GetPokemonAI()->SetState(AI_STATE_IDLE);
        coreSys->GetPokemonAI()->SetTouchOK(true);
        GetG2DUtil()->StartAnime( 0, LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__MAMEBUTTON_MODE_OFF);
      }
      break;

    case BUTTON_ID_CAREMODE:
      if(GuiMode()==GUI_MODE_NORMAL) 
      {
        if(coreSys->GetPokemonAI()->CanChangeMode())
        {          
          SetCursorOffscreen();
          coreSys->GetPokemonAI()->PlaySE( SEQ_SE_KW_HIKIDASHI_OP );
          GetG2DUtil()->StartAnime( 0, LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__CAREBUTTON_MODE_ON);
          careMenu.SetState(GUI_STATE_MENU_IDLE);
          mameMenu.SetState(GUI_STATE_MAIN);
          GetG2DUtil()->StartAnime( 0, LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000_CARE_IN);
          SetGUIMode(GUI_MODE_CARE);       
          coreSys->GetPokemonAI()->SetState(AI_STATE_CARE);

          DisableBackButton();
          DisablePokemonSelectButton();
          DisableEatModeButton();
          DisableCareButton();

        }
      }
      else 
      {       
        SetCursorOffscreen();
        coreSys->GetRenderSystem()->GetRenderingPipeLine()->SetUseTouchIDTexture(false);
        coreSys->GetPokemonAI()->PlaySE( SEQ_SE_KW_HIKIDASHI_CL );
        GetG2DUtil()->StartAnime( 0, LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__CAREBUTTON_MODE_OFF);
        GetG2DUtil()->StartAnime( 0, LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000_CARE_CANCEL);
        SetGUIMode(GUI_MODE_NORMAL);                
        coreSys->GetPokemonAI()->SetTouchOK(true);
        coreSys->GetPokemonAI()->SetState(AI_STATE_IDLE);
      }
      break;
  }
  return DISABLE_ACCESS;
}

void KawaigariUIManager::SetGUIMode(int n) 
{ 
  if(n==GUI_MODE_NORMAL)
  {
    mButtonDisableCount = 20;
    GetG2DUtil()->StartAnime( 0, LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__MAMEBUTTON_MODE_OFF);
    GetG2DUtil()->StartAnime( 0, LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__CAREBUTTON_MODE_OFF);
  }
  guiMode = n;
  GFL_PRINT("Set GUIMODE = %i\n",guiMode);

  EnableCareButton();
}


  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)