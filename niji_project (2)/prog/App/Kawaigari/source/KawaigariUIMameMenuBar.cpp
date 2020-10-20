//======================================================================
/**
 * @file	KawaigariUIMameMenuBar.C++
 * @brief	Kawaigari Mame Menu Bar UI Interface 
 * @author	PETE
 * @data	2015.12.03
 */
//======================================================================
#include <macro/include/gfl2_Macros.h>
#include <gfx/include/gfl2_types.h>
#include <GameSys/include/GameProcManager.h>
// UI Base:
#include "AppLib/include/Ui/UIView.h"
#include "AppLib/include/Ui/UIInputListener.h"

#include "KawaigariUIManager.h"

#include "niji_conv_header/app/kawaigari/res2d/kw_layout.h"
#include "niji_conv_header/app/kawaigari/res2d/kw_layout_pane.h"
#include "niji_conv_header/app/kawaigari/res2d/kw_layout_anim_list.h"

#include "KawaigariMain.h"


GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

KawaigariUIMameMenuBar::KawaigariUIMameMenuBar(void)
{    
  SetState(GUI_STATE_INIT);
  
  mTouching = false;
}

KawaigariUIMameMenuBar::~KawaigariUIMameMenuBar(void)
{  
}

void KawaigariUIMameMenuBar::SetState(int s) 
{ 
  mStateReg=s; 
  mStateRegSub=0; 
  for(int r=0;r<MAX_UI_STATE_REGS;r++) mStateRegs[r] = 0;

}
void KawaigariUIMameMenuBar::SetSubState(int s) 
{ 
  mStateRegSub=s; 
  for(int r=0;r<MAX_UI_STATE_REGS;r++) mStateRegs[r] = 0;
}

void KawaigariUIMameMenuBar::PrintState(void)
{
  GFL_PRINT("MameMenuState: %i  %i -> ",mStateReg,mStateRegSub);

  for(int r=0;r<MAX_UI_STATE_REGS;r++)
  {
    if(r && (r&3)==0) GFL_PRINT("  ");
    GFL_PRINT("%03i ",mStateRegs[r]);    
  }

  GFL_PRINT("\n");
}

void KawaigariUIMameMenuBar::State_Init(void)
{     
  SetState(GUI_STATE_MAIN);
  SaveOriginalPositions(); // Backup Original Positions
  UpdateAgain();
}

void KawaigariUIMameMenuBar::State_Main(void)
{    
}

void KawaigariUIMameMenuBar::State_End(void)
{  
}

void KawaigariUIMameMenuBar::State_Cleanup(void)
{  
}

void KawaigariUIMameMenuBar::State_Finish(void)
{  
}

bool KawaigariUIMameMenuBar::IsTouchSlide(int speed)
{
  if(touchX == lastTouchX ) return(false);

  if( abs( touchX - lastTouchX ) < abs( touchY - lastTouchY ) ) return(false);

  return (speed < abs( touchX - lastTouchX ) );  
}

bool KawaigariUIMameMenuBar::IsTouchSlideY(int speed)
{
  if(touchY == lastTouchY ) return(false);  
  return (speed < abs( touchY - lastTouchY ) );  
}

void KawaigariUIMameMenuBar::State_Menu_Idle(void)
{
  const f32 SCL_SIZE = 1.25f;
  const f32 SCL_INC = 0.1f;
  const f32 SCL_DEC = 0.3f;

  // Update Touch Pos: (MANUALLY)
  {  
    gfl2::ui::TouchPanel * touchPanel;
    touchPanel = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetTouchPanel( gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD );
    gfl2::math::VEC3 pos;

    if(touchPanel->IsTouch())
    {    
      touchPanel->GetXY(&pos);  
      coreUISys->SetTouchPos(pos.x,pos.y);
    }
    else coreUISys->SetTouchPos(1000.0f,1000.0f); // Set Offscreen
  }

  u32 stx,sty;
  coreUISys->GetTouchRawPos(stx,sty);
  touchX = stx; touchY = sty;

  u32 touchRawX,touchRawY;
  coreUISys->GetTouchRawPos(touchRawX,touchRawY);

  if(coreUISys->GetCoreSys()->GetPokemonAI()->IsTouching())
  {
    if(!mTouching)
    {
      lastTouchX = touchX;
      lastTouchY = touchY;
    }
    mTouching = true;
  }
  else
  {
    mTouching = false;
  }

  int tx=touchRawX,ty=touchRawY;
  coreUISys->ConvertRawToTouchPos(tx,ty);

  const int mameButtons[6] = { PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_01,
    PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_02,
    PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_03,
    PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_04,
    PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_05,
    PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_06 };

  // Mame Icon Scaling
  if(mStateRegSub==0)
  {
    for(int z=0;z<6;z++)
    {    
      gfl2::math::VEC2 scl;    
      coreUISys->GetG2DUtil()->GetScale(0,mameButtons[z] ,&scl);
      if(scl.x > 1.0f) scl.x -= SCL_DEC;
      if(scl.y > 1.0f) scl.y -= SCL_DEC;
      if(scl.x < 1.0f) scl.x = scl.y = 1.0f;      
      coreUISys->GetG2DUtil()->SetScale(0,mameButtons[z],scl);
    }
  }
  else if(mStateRegSub==1) 
  {    
    gfl2::math::VEC2 scl;    
    coreUISys->GetG2DUtil()->GetScale(0,mStateRegs[1] ,&scl);
    if(scl.x < SCL_SIZE) scl.x += SCL_INC;
    if(scl.y < SCL_SIZE) scl.y += SCL_INC;
    coreUISys->GetG2DUtil()->SetScale(0,mStateRegs[1] ,scl);
  }
  else if(mStateRegSub>1)
  {
    gfl2::math::VEC2 scl;    
    scl.x = scl.y = 1.0f;
    coreUISys->GetG2DUtil()->SetScale(0,mStateRegs[1] ,scl);
  }

  switch(mStateRegSub)
  {
   
    // Wait 20 frames to enter:
    case 0:      
      mStateRegs[3] = 20;

      if(coreUISys->GuiMode()==GUI_MODE_MAME) mStateRegSub++;      
      break;

    case 1:
      // Reset if not in MAME_MODE:
      if(coreUISys->GuiMode()!=GUI_MODE_MAME) { mStateRegSub = 0; break; }
      mStateRegs[3]--;

      if(mStateRegs[3]<=0) 
      {
        coreUISys->EnableEatModeButton();
        mStateRegSub++;
      }
      break;

       // Wait for Mame to be touched:
    case 2:
      // Reset if not in MAME_MODE:
      if(coreUISys->GuiMode()!=GUI_MODE_MAME) { mStateRegSub = 0; break; }

      

      coreUISys->GetG2DUtil()->SetPaneVisible(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_00,false);
     
      if(coreUISys->GuiMode()==GUI_MODE_MAME) 
        coreUISys->GetG2DUtil()->StartAnime( 0, LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__MAMEBUTTON_MODE_ON);

      if(!coreUISys->IsTouching()) mStateRegs[4] = 1;

      // Press Care Button?
      if(coreUISys->GuiMode()==GUI_MODE_MAME && coreUISys->GetG2DUtil()->GetHitPane(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAMEBUTTON,touchRawX,touchRawY))
      {
     //   if(mStateRegs[4])
        {
          mStateRegSub=120; // Wait End Touch & Delay
          break;
        }
      }

      if(coreUISys->IsTouching()) mStateRegs[4] = 0;

      if(!coreUISys->IsTouching() || coreUISys->GuiMode()!=GUI_MODE_MAME) 
      {
        coreUISys->SetMameButtonLarge(false);
        mStateRegs[2] = 0;
        if(coreUISys->GuiMode()!=GUI_MODE_MAME ) mStateRegSub = 0;
    //  RestoreOriginalPositions();
        break;
      }

    

      
      if(coreUISys->GetCoreSys()->GetMameManager()->CanScrollRight() || coreUISys->GetCoreSys()->GetMameManager()->CanScrollLeft())
      {
        if(IsTouchSlideY(4))
        {

        }
        else if(mStateRegs[2]<3 && !IsTouchSlide(3))
        {
          mStateRegs[2]++;
          break;
        }
        else if(mStateRegs[2]>=3)
        {

        }
        else if(touchRawY<64)
        {
          posBaseRoot = coreUISys->GetG2DUtil()->GetPanePos(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_NULL_SCROLL);
          u32 touchX,touchY;
          coreUISys->GetTouchRawPos(touchX,touchY);
          mStateRegs[2]=0; // Slide Offset
          mStateRegs[3]=touchX;        
          mStateRegSub = 25; // Slide Program
          UpdateAgain();
          break;
        }
      }

      {
        GFL_PRINT("Xoff:%i ScrollPos: %i\n",mStateRegs[2],coreUISys->GetCoreSys()->GetMameManager()->GetScrollPos());
      }
      
      // Mame 0:      
      if(coreUISys->GetG2DUtil()->GetHitPane(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_01,touchRawX,touchRawY) && coreUISys->GetCoreSys()->GetMameManager()->GetMameType(0)!=-1)
      {
      //  SaveOriginalPositions(); // Backup Original Positions
        mStateRegs[0] = 0; // Slot 0        
        mStateRegs[1] = PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_08;       

        int type = coreUISys->GetCoreSys()->GetMameManager()->GetMameType(mStateRegs[0]);
        coreUISys->GetCoreSys()->GetMameManager()->SetupSelectedMameIcon(type,mStateRegs[0]);

        mStateRegSub++;         
        coreUISys->GetCoreSys()->SetMameType(coreUISys->GetCoreSys()->GetMameManager()->GetMameType(0));

        coreUISys->GetG2DUtil()->AppendChild(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_NULL_MAMEPARENT,mStateRegs[1] );
        gfl2::math::VEC3 pos( (float)tx , (float)ty ,0.0f);      
        coreUISys->GetG2DUtil()->SetPanePos(0,mStateRegs[1],&pos);
        break;
      }
      else if(coreUISys->GetG2DUtil()->GetHitPane(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_02,touchRawX,touchRawY) && coreUISys->GetCoreSys()->GetMameManager()->GetMameType(1)!=-1)
      {
      //  SaveOriginalPositions(); // Backup Original Positions
        mStateRegs[0] = 1; // Slot 1        
        mStateRegs[1] = PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_08;      

        int type = coreUISys->GetCoreSys()->GetMameManager()->GetMameType(mStateRegs[0]);
        coreUISys->GetCoreSys()->GetMameManager()->SetupSelectedMameIcon(type,mStateRegs[0]);

        mStateRegSub++;         
        coreUISys->GetCoreSys()->SetMameType(coreUISys->GetCoreSys()->GetMameManager()->GetMameType(1));

        coreUISys->GetG2DUtil()->AppendChild(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_NULL_MAMEPARENT,mStateRegs[1] );
        gfl2::math::VEC3 pos( (float)tx , (float)ty ,0.0f);      
        coreUISys->GetG2DUtil()->SetPanePos(0,mStateRegs[1],&pos);
        break;
      }
      else if(coreUISys->GetG2DUtil()->GetHitPane(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_03,touchRawX,touchRawY) && coreUISys->GetCoreSys()->GetMameManager()->GetMameType(2)!=-1)
      {
      //  SaveOriginalPositions(); // Backup Original Positions
        mStateRegs[0] = 2; // Slot 2       
        mStateRegs[1] = PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_08;       

        int type = coreUISys->GetCoreSys()->GetMameManager()->GetMameType(mStateRegs[0]);
        coreUISys->GetCoreSys()->GetMameManager()->SetupSelectedMameIcon(type,mStateRegs[0]);

        mStateRegSub++;         
        coreUISys->GetCoreSys()->SetMameType(coreUISys->GetCoreSys()->GetMameManager()->GetMameType(2));

        coreUISys->GetG2DUtil()->AppendChild(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_NULL_MAMEPARENT,mStateRegs[1] );
        gfl2::math::VEC3 pos( (float)tx , (float)ty ,0.0f);      
        coreUISys->GetG2DUtil()->SetPanePos(0,mStateRegs[1],&pos);
        break;
      }
      else if(coreUISys->GetG2DUtil()->GetHitPane(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_04,touchRawX,touchRawY) && coreUISys->GetCoreSys()->GetMameManager()->GetMameType(3)!=-1)
      {
      //  SaveOriginalPositions(); // Backup Original Positions
        mStateRegs[0] = 3; // Slot 3       
        mStateRegs[1] = PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_08;       

        int type = coreUISys->GetCoreSys()->GetMameManager()->GetMameType(mStateRegs[0]);
        coreUISys->GetCoreSys()->GetMameManager()->SetupSelectedMameIcon(type,mStateRegs[0]);

        mStateRegSub++;         
        coreUISys->GetCoreSys()->SetMameType(coreUISys->GetCoreSys()->GetMameManager()->GetMameType(3));

        coreUISys->GetG2DUtil()->AppendChild(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_NULL_MAMEPARENT,mStateRegs[1] );
        gfl2::math::VEC3 pos( (float)tx , (float)ty ,0.0f);      
        coreUISys->GetG2DUtil()->SetPanePos(0,mStateRegs[1],&pos);
        break;
      }
      else if(coreUISys->GetG2DUtil()->GetHitPane(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_05,touchRawX,touchRawY) && coreUISys->GetCoreSys()->GetMameManager()->GetMameType(4)!=-1)
      {
      //  SaveOriginalPositions(); // Backup Original Positions
        mStateRegs[0] = 4; // Slot 4
        mStateRegs[1] = PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_08;       

        int type = coreUISys->GetCoreSys()->GetMameManager()->GetMameType(mStateRegs[0]);
        coreUISys->GetCoreSys()->GetMameManager()->SetupSelectedMameIcon(type,mStateRegs[0]);

        mStateRegSub++;         
        coreUISys->GetCoreSys()->SetMameType(coreUISys->GetCoreSys()->GetMameManager()->GetMameType(4));

        coreUISys->GetG2DUtil()->AppendChild(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_NULL_MAMEPARENT,mStateRegs[1] );
        gfl2::math::VEC3 pos( (float)tx , (float)ty ,0.0f);      
        coreUISys->GetG2DUtil()->SetPanePos(0,mStateRegs[1],&pos);
        break;
      }
      else if(coreUISys->GetG2DUtil()->GetHitPane(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_06,touchRawX,touchRawY) && coreUISys->GetCoreSys()->GetMameManager()->GetMameType(5)!=-1)
      {
      //  SaveOriginalPositions(); // Backup Original Positions
        mStateRegs[0] = 5; // Slot 5
        mStateRegs[1] = PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_08;       

        int type = coreUISys->GetCoreSys()->GetMameManager()->GetMameType(mStateRegs[0]);
        coreUISys->GetCoreSys()->GetMameManager()->SetupSelectedMameIcon(type,mStateRegs[0]);

        mStateRegSub++;         
        coreUISys->GetCoreSys()->SetMameType(coreUISys->GetCoreSys()->GetMameManager()->GetMameType(5));

        coreUISys->GetG2DUtil()->AppendChild(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_NULL_MAMEPARENT,mStateRegs[1] );
        gfl2::math::VEC3 pos( (float)tx , (float)ty ,0.0f);      
        coreUISys->GetG2DUtil()->SetPanePos(0,mStateRegs[1],&pos);
        break;
      }
      break;

    case 3:      
    
      // No longer touching the screen:
      if(!coreUISys->IsTouching())
      {        
        gfl2::math::VEC2 scl(SCL_SIZE,SCL_SIZE);
        coreUISys->GetG2DUtil()->SetScale(0,mameButtons[mStateRegs[0]] ,scl);
        coreUISys->GetCoreSys()->GetMameManager()->RestoreSelectedMameIcon(mStateRegs[0]);
        coreUISys->GetG2DUtil()->StartAnime( 0, LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__MAMEBUTTON_MODE_ON);

        coreUISys->SetMameButtonLarge(false);

        coreUISys->GetG2DUtil()->AppendChild(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_NULL_VISIBLE,mStateRegs[1]);

        RestoreOriginalPositions();
        mStateRegSub=0; // Restart
        break;
      }
      // Reset if not in MAME_MODE:
      else if(coreUISys->GuiMode()!=GUI_MODE_MAME) 
      {
        gfl2::math::VEC2 scl(SCL_SIZE,SCL_SIZE);
        coreUISys->GetG2DUtil()->SetScale(0,mameButtons[mStateRegs[0]] ,scl);
        coreUISys->GetCoreSys()->GetMameManager()->RestoreSelectedMameIcon(mStateRegs[0]);
        coreUISys->GetG2DUtil()->AppendChild(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_NULL_VISIBLE,mStateRegs[1]);
        RestoreOriginalPositions();
        mStateRegSub=0; // Restart
        break;
      }

      if(touchRawY>64)
      {
        coreUISys->DisableEatModeButton();

        coreUISys->GetG2DUtil()->StartAnime( 0, LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__MAMEBUTTON_MODE_DRAG);
        int type = coreUISys->GetCoreSys()->GetMameManager()->GetMameType(mStateRegs[0]);
        coreUISys->GetCoreSys()->GetMameManager()->RemoveBean(type);
        
        coreUISys->GetG2DUtil()->SetPaneVisible(0,mStateRegs[1] ,false);
        coreUISys->GetG2DUtil()->AppendChild(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_NULL_VISIBLE,mStateRegs[1]);
        RestoreOriginalPositions();
        coreUISys->GetG2DUtil()->StartAnime( 0, LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000_MAME_SELECT);  

        gfl2::math::Vector3 pos( 0.0f,0.0f ,0.0f);      

        u32 touchX,touchY;
        coreUISys->GetTouchRawPos(touchX,touchY);
        //coreUISys->GetCoreSys()->Get3DTouchPositionByXY( &pos, touchX, touchY, TPT_FOOD );

      //  pos.z += 10.0f;

        gfl2::ui::TouchPanel * touchPanel;
        touchPanel = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetTouchPanel( gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD );
        touchX = touchPanel->GetX();
        touchY = touchPanel->GetY();

        ///coreUISys->GetCoreSys()->Get3DTouchPositionByXY(&pos,touchX,touchY,TPT_PARTICLE);  
        coreUISys->GetCoreSys()->Get3DEffectTouchPositionByXY(&pos,touchX,touchY);  

        coreUISys->GetCoreSys()->CreateEffect(KAWAIGARI_EFFECT_SMOKE,&pos);
        mStateRegSub=40; // Create Mame Objects & Wait Touch End
        
        break;
      }
      
      // Update Mame Icon Position by Touch Pos:
      {

      //  gfl2::math::VEC3 posNewBaseRoot = coreUISys->GetG2DUtil()->GetPanePos(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_NULL_SCROLL);

      //  gfl2::math::VEC3 pos( (float)tx-posBaseRoot.x , (float)ty-posBaseRoot.y ,0.0f);      
        gfl2::math::VEC3 pos( (float)tx , (float)ty ,0.0f);      
 
        coreUISys->GetG2DUtil()->SetPanePos(0,mStateRegs[1],&pos);
      }


      break;

  
    case 25:
      

      if(!coreUISys->IsTouching() || touchRawY>64)
      {
        coreUISys->SetMameButtonLarge(false);
        coreUISys->GetG2DUtil()->SetPanePos(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_NULL_SCROLL,&posBaseRoot);

        if(mStateRegs[2] <-22 && coreUISys->GetCoreSys()->GetMameManager()->CanScrollRight())
        {
          coreUISys->GetCoreSys()->GetMameManager()->ScrollRight();
        }
        else if(mStateRegs[2] >22 && coreUISys->GetCoreSys()->GetMameManager()->CanScrollLeft())
        {
          coreUISys->GetCoreSys()->GetMameManager()->ScrollLeft();
        }
        
        mStateRegs[2]=0;

        coreUISys->GetCoreSys()->GetMameManager()->SetupMameIcons();
      
        coreUISys->GetG2DUtil()->SetPaneVisible(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_00,false);
        
        mStateRegSub = 2;       
        break;
      }

      // Update Slide:
      {        
        int off = touchX - mStateRegs[3];

        if(off<0 && coreUISys->GetCoreSys()->GetMameManager()->CanScrollRight())
        {        
          mStateRegs[2]+=off;
          
          // Scrolled
          while(mStateRegs[2]<=-44)
          {
            if( coreUISys->GetCoreSys()->GetMameManager()->CanScrollRight() )coreUISys->GetCoreSys()->GetMameManager()->ScrollRight();
            mStateRegs[2]+=44;            
          }
        }
        else if(off>0 && coreUISys->GetCoreSys()->GetMameManager()->CanScrollLeft())
        {
          mStateRegs[2]+=off;          

          // Scrolled         
          while(mStateRegs[2]>=44)
          {
            if(coreUISys->GetCoreSys()->GetMameManager()->CanScrollLeft() ) coreUISys->GetCoreSys()->GetMameManager()->ScrollLeft();
            mStateRegs[2]-=44;

            if(!coreUISys->GetCoreSys()->GetMameManager()->CanScrollLeft()) mStateRegs[2]=0;
          }
          
        }

        {
          gfl2::math::VEC3 pos;
          pos.x = posBaseRoot.x + mStateRegs[2];
          pos.y = posBaseRoot.y;
          pos.z = 0.0f;
          coreUISys->GetG2DUtil()->SetPanePos(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_NULL_SCROLL,&pos);
        }

        if(mStateRegs[2]<=0)
        {
          coreUISys->GetG2DUtil()->SetPaneVisible(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_00,false);          
        }
        else
        {
          coreUISys->GetG2DUtil()->SetPaneVisible(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_00,true);          
        }

        // Save Last Pos:
        mStateRegs[3] = touchX;

        {
          GFL_PRINT("Xoff:%i ScrollPos: %i\n",mStateRegs[2],coreUISys->GetCoreSys()->GetMameManager()->GetScrollPos());
        }


      }
      break;

    case 40: // Mame Load      
      coreUISys->GetCoreSys()->LoadFoodResource(0);
      mStateRegSub++;
      // BREAK-THROUGH
    case 41:
      if(coreUISys->GetCoreSys()->WaitFoodResource())
      {
        mStateRegSub = 45;       
      }
      else break;
      // BREAK-THROUGH

    case 45: // Load Complete, Create Model and Set State:
      coreUISys->GetCoreSys()->GetPokemonAI()->SetSubState(AI_SUBSTATE_EAT_BEGIN);

      coreUISys->GetCoreSys()->LoadFoodObject(0);
      coreUISys->GetCoreSys()->ShowFoodObject(0,true);
        
      mStateRegSub = 51;
      UpdateAgain();
      break;

    case 50:      
      //RestoreOriginalPositions();
      mStateRegSub++;
      break;

    case 51:
      if(!coreUISys->IsTouching() && coreUISys->GetCoreSys()->GetPokemonAI()->GetState()==AI_STATE_IDLE)
      {
        coreUISys->GetG2DUtil()->StartAnime( 0, LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__MAMEBUTTON_MODE_OFF);
        coreUISys->SetMameButtonLarge(false);
       // RestoreOriginalPositions();
        coreUISys->GetG2DUtil()->SetPaneVisible(0,mStateRegs[1] ,true);
     //   coreUISys->GetG2DUtil()->StartAnime( 0, LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000_BUTTON_IN);
      //  coreUISys->SetGUIMode(GUI_MODE_NORMAL);
        //mStateRegSub = 0; // Loop!
        SetState(GUI_STATE_MAIN);        
      }
      break;
   
    case 99:
      RestoreOriginalPositions();
      SetState(GUI_STATE_MAIN);
      //mStateRegSub = 0; // Loop!
      break;

      // Delay & Wait Program (Prevent Tool Grab on CareMode Close)
    case 120: // Wait End touch & Delay:
      if(!coreUISys->IsTouching())
      {
        mStateRegSub++;
        mStateRegs[3]=10;
      }
      break;
    case 121:
      mStateRegs[3]--;

      // Reset Counter if touching CareButton:
      if(coreUISys->IsTouching() && coreUISys->GetG2DUtil()->GetHitPane(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_CAREBUTTON,touchRawX,touchRawY))
      {
        mStateRegs[3]=10;
        mStateRegs[4] = 1;
      }

      if(mStateRegs[3]<=0)
      {
        if(coreUISys->GuiMode()!=GUI_MODE_MAME)
          mStateRegSub = 0; // LOOP FROM START
        else
          mStateRegSub = 2;

        mStateRegs[4] = 1;
      }
      break;
  }

  lastTouchX = touchX;
  lastTouchY = touchY;

}


void KawaigariUIMameMenuBar::Update(void)
{    

  do 
  {
    mRequireUpdate = false;

    switch(mStateReg)
    {
    case GUI_STATE_INIT: State_Init(); break;
    case GUI_STATE_MAIN: State_Main(); break;    
    case GUI_STATE_MENU_IDLE: State_Menu_Idle(); break;    
    case GUI_STATE_END: State_End(); break;
    case GUI_STATE_CLEANUP: State_Cleanup(); break;
    case GUI_STATE_FINISH: State_Finish(); break;
    }

  } while (mRequireUpdate);


  // Print State Debug:
  // PrintState();
}

void KawaigariUIMameMenuBar::SaveOriginalPositions(void)
{
  
  // Get the Root Base Pos:
  posBaseRoot = coreUISys->GetG2DUtil()->GetPanePos(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_NULL_SCROLL);

  // Get each Mame Position:
/*  mameBasePos[0] = coreUISys->GetG2DUtil()->GetPanePos(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_01);
  mameBasePos[1] = coreUISys->GetG2DUtil()->GetPanePos(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_02);
  mameBasePos[2] = coreUISys->GetG2DUtil()->GetPanePos(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_03);
  mameBasePos[3] = coreUISys->GetG2DUtil()->GetPanePos(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_04);
  mameBasePos[4] = coreUISys->GetG2DUtil()->GetPanePos(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_05);
  mameBasePos[5] = coreUISys->GetG2DUtil()->GetPanePos(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_06);
  */
  mameBasePos[6] = coreUISys->GetG2DUtil()->GetPanePos(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_08);
  
  
  for(int n=0;n<MAX_MAME_ICON;n++)
  {
    mameBasePos[n].x-=posBaseRoot.x;
    mameBasePos[n].y-=posBaseRoot.y;
  }
}

void KawaigariUIMameMenuBar::RestoreOriginalPositions(void)
{ 
  
  // Get the Root Base Pos:
  posBaseRoot = coreUISys->GetG2DUtil()->GetPanePos(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_NULL_SCROLL);
/*  
  coreUISys->GetG2DUtil()->SetPanePos(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_01,&mameBasePos[0]);
  coreUISys->GetG2DUtil()->SetPanePos(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_02,&mameBasePos[1]);
  coreUISys->GetG2DUtil()->SetPanePos(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_03,&mameBasePos[2]);
  coreUISys->GetG2DUtil()->SetPanePos(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_04,&mameBasePos[3]);
  coreUISys->GetG2DUtil()->SetPanePos(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_05,&mameBasePos[4]);
  coreUISys->GetG2DUtil()->SetPanePos(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_06,&mameBasePos[5]);
  */
  coreUISys->GetG2DUtil()->SetPanePos(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_MAME_08,&mameBasePos[6]);
  
  
}

  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)