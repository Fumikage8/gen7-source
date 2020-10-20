//======================================================================
/**
 * @file	KawaigariUICareMenuBar.C++
 * @brief	Kawaigari Care Menu Bar UI Interface 
 * @author	PETE
 * @data	2015.12.03
 */
//======================================================================

// UI Base:
#include "AppLib/include/Ui/UIView.h"
#include "AppLib/include/Ui/UIInputListener.h"

#include "KawaigariUIManager.h"

#include "niji_conv_header/app/kawaigari/res2d/kw_layout.h"
#include "niji_conv_header/app/kawaigari/res2d/kw_layout_pane.h"
#include "niji_conv_header/app/kawaigari/res2d/kw_layout_anim_list.h"

#include "niji_conv_header/message/msg_pokeparure.h"

#include "KawaigariMain.h"

// Sound
#include "Sound/include/Sound.h"


GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

KawaigariUICareMenuBar::KawaigariUICareMenuBar(void)
{    
  SetState(GUI_STATE_INIT);
}

KawaigariUICareMenuBar::~KawaigariUICareMenuBar(void)
{  
}

void KawaigariUICareMenuBar::SetState(int s) 
{ 
  mStateReg=s; 
  mStateRegSub=0; 
  for(int r=0;r<MAX_UI_STATE_REGS;r++) mStateRegs[r] = 0;

}
void KawaigariUICareMenuBar::SetSubState(int s) 
{ 
  mStateRegSub=s; 
  for(int r=0;r<MAX_UI_STATE_REGS;r++) mStateRegs[r] = 0;
}

void KawaigariUICareMenuBar::TurnOnDryer(void)
{
                    
  coreUISys->GetG2DUtil()->StartAnime( 0, LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__CARETOOL_04_DRYER_KEEP );
  coreUISys->GetG2DUtil()->SetAnimeSpeed( 0, 1.0f );

  app::util::G2DUtil* pG2D = coreUISys->GetG2DUtil();
  gfl2::lyt::LytWk* lytWk = pG2D->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID resID = pG2D->GetLayoutResourceID( 0 );            
  gfl2::lyt::LytParts *parts = lytWk->GetPartsPane( PANENAME_KAWAIGARI_MIN_LOW_000_PANE_CARETOOL_04 );
  lytWk->GetPane(parts,PANENAME_KAWAIGARI_BTN_LOW_005_PANE_NULL_EFFECT,&resID)->SetVisible(true);            
  
}

void KawaigariUICareMenuBar::TurnOffDryer(void)
{
  coreUISys->GetG2DUtil()->StopAnime( 0, LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__CARETOOL_04_DRYER_KEEP );
  coreUISys->GetG2DUtil()->SetAnimeSpeed( 0, 1.0f );
  app::util::G2DUtil* pG2D = coreUISys->GetG2DUtil();
  gfl2::lyt::LytWk* lytWk = pG2D->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID resID = pG2D->GetLayoutResourceID( 0 );            
  gfl2::lyt::LytParts *parts = lytWk->GetPartsPane( PANENAME_KAWAIGARI_MIN_LOW_000_PANE_CARETOOL_04 );
  lytWk->GetPane(parts,PANENAME_KAWAIGARI_BTN_LOW_005_PANE_NULL_EFFECT,&resID)->SetVisible(false);            

  Sound::StopSE(SEQ_SE_KW_OTEIRE_DRYER,0,1);
  
}

void KawaigariUICareMenuBar::PrintState(void)
{
  GFL_PRINT("CareMenuState: %i  %i -> ",mStateReg,mStateRegSub);

  for(int r=0;r<MAX_UI_STATE_REGS;r++)
  {
    if(r && (r&3)==0) GFL_PRINT("  ");
    GFL_PRINT("%03i ",mStateRegs[r]);    
  }

  GFL_PRINT("\n");
}

void KawaigariUICareMenuBar::State_Init(void)
{    
  SetState(GUI_STATE_MAIN);

  const int mameButtonsAnimeOUT[5] = { LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__CARETOOL_00_ITEMOFF_00,
    LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__CARETOOL_01_ITEMOFF_01,
    LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__CARETOOL_02_ITEMOFF_02,
    LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__CARETOOL_03_ITEMOFF_03,
    LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__CARETOOL_04_ITEMOFF_04 };

  for(int z=0;z<5;z++)
  {
    // Default Animation:
    coreUISys->GetG2DUtil()->StartAnime(0,mameButtonsAnimeOUT[mStateRegs[0]]);
  }

  UpdateAgain();
}

void KawaigariUICareMenuBar::State_Main(void)
{  
}

void KawaigariUICareMenuBar::CareToolFadeEffect(void)
{
  gfl2::ui::TouchPanel * touchPanel;
  touchPanel = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetTouchPanel( gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD );
  u32 touchX = 0,touchY = 0;

  touchX = touchPanel->GetPrevX();
  touchY = touchPanel->GetPrevY();

  gfl2::math::Vector3 pos;  
  
  ///coreUISys->GetCoreSys()->Get3DTouchPositionByXY(&pos,touchX,touchY,TPT_PARTICLE);  
  coreUISys->GetCoreSys()->Get3DEffectTouchPositionByXY(&pos,touchX,touchY);  

  coreUISys->GetCoreSys()->CreateEffect(KAWAIGARI_EFFECT_CARETOOLFADE,&pos,2);

  /// NO SOUND NEEDED:
  /// coreUISys->GetCoreSys()->GetPokemonAI()->PlaySE( SEQ_SE_EXTRA_0063 );
}

void KawaigariUICareMenuBar::State_Menu_Idle(void)
{
  u32 touchRawX,touchRawY;
  coreUISys->GetTouchRawPos(touchRawX,touchRawY);

  gfl2::ui::TouchPanel * touchPanel;
  touchPanel = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetTouchPanel( gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD );
  u32 nowTouchRawX=0xFFFF,nowTouchRawY=0xFFFF;

  if(touchPanel->IsTouch())
  {
    nowTouchRawX = touchPanel->GetX();
    nowTouchRawY = touchPanel->GetY();
  }

  int tx=nowTouchRawX,ty=nowTouchRawY;
  coreUISys->ConvertRawToTouchPos(tx,ty);

  if(mStateRegs[0] == CARETOOL_BANDAGE && mStateRegSub!=4 )
  {
    tx +=20;
    ty += 10;
  }

  const int mameButtonsAnimeIN[5] = { LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__CARETOOL_00_ITEMON_00,
    LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__CARETOOL_01_ITEMON_01,
    LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__CARETOOL_02_ITEMON_02,
    LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__CARETOOL_03_ITEMON_03,
    LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__CARETOOL_04_ITEMON_04 };

  const int mameButtonsAnimeOUT[5] = { LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__CARETOOL_00_ITEMOFF_00,
    LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__CARETOOL_01_ITEMOFF_01,
    LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__CARETOOL_02_ITEMOFF_02,
    LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__CARETOOL_03_ITEMOFF_03,
    LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__CARETOOL_04_ITEMOFF_04 };

  switch(mStateRegSub)
  {
    case 0: // Wait 20 frames
      mStateRegs[7] = 20;
      
      if(coreUISys->GuiMode()==GUI_MODE_CARE) mStateRegSub++;
      break;

    case 1:
      mStateRegs[7]--;

      // Restart wait?
      if(coreUISys->GuiMode()!=GUI_MODE_CARE) 
      {
        mStateRegSub = 20;
        break;
      }

      if(mStateRegs[7]<=0) 
      {
        coreUISys->EnableCareButton();
        mStateRegSub=4;
      }

      break;

    // Wait for Mame to be touched:
    case 4:      
      if(!coreUISys->IsTouching()) mStateRegs[3] = 1;

      // Press Care Button?
      if(coreUISys->GuiMode()==GUI_MODE_CARE && coreUISys->GetG2DUtil()->GetHitPane(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_CAREBUTTON,nowTouchRawX,nowTouchRawY))
      {
        if(mStateRegs[3])
        {
          mStateRegSub=120; // Wait End Touch & Delay
          break;
        }
      }

      if(coreUISys->IsTouching()) mStateRegs[3] = 0;
      


      if(coreUISys->GuiMode()==GUI_MODE_CARE)
        coreUISys->GetG2DUtil()->StartAnime( 0, LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__CAREBUTTON_MODE_ON);
    
      if(!coreUISys->IsTouching() || coreUISys->GuiMode()!=GUI_MODE_CARE) break;

      // Bandage Tool:
      if(coreUISys->GuiMode()==GUI_MODE_CARE && coreUISys->GetG2DUtil()->GetHitPane(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_BG_CATETOOL_00,nowTouchRawX,nowTouchRawY))
      {
        coreUISys->GetCoreSys()->GetPokemonAI()->PlaySE( SEQ_SE_CATCH );

        SaveOriginalPositions(); // Backup Original Positions
        mStateRegs[0] = CARETOOL_BANDAGE; 
        mStateRegs[1] = PANENAME_KAWAIGARI_MIN_LOW_000_PANE_CARETOOL_00;       
        mStateRegSub++;         

        coreUISys->SetCareTextMessage(msg_pkparure_09_01);
        coreUISys->GetG2DUtil()->StartAnime( 2, LA_KW_LAYOUT_KAWAIGARI_MESSAGE_UPP_000__MESSAGE_L2_IN);

        coreUISys->GetG2DUtil()->AppendChild(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_NULL_00,mStateRegs[1] );      
        //tx+=20; ty+=10;
        gfl2::math::VEC3 pos( (float)tx , (float)ty ,0.0f);      
        coreUISys->GetG2DUtil()->SetPanePos(0,mStateRegs[1],&pos);

        // Active Animation:
        coreUISys->GetG2DUtil()->StartAnime(0,mameButtonsAnimeIN[mStateRegs[0]]);
  
        break;
      }

      // Hair Tool:
      if(coreUISys->GuiMode()==GUI_MODE_CARE && coreUISys->GetG2DUtil()->GetHitPane(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_BG_CATETOOL_01,nowTouchRawX,nowTouchRawY))
      {
        coreUISys->GetCoreSys()->GetPokemonAI()->PlaySE( SEQ_SE_CATCH );

        SaveOriginalPositions(); // Backup Original Positions
        mStateRegs[0] = CARETOOL_HAIR;
        mStateRegs[1] = PANENAME_KAWAIGARI_MIN_LOW_000_PANE_CARETOOL_01;       
        mStateRegSub++;         

        coreUISys->SetCareTextMessage(msg_pkparure_09_02);
        coreUISys->GetG2DUtil()->StartAnime( 2, LA_KW_LAYOUT_KAWAIGARI_MESSAGE_UPP_000__MESSAGE_L2_IN);

        coreUISys->GetG2DUtil()->AppendChild(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_NULL_00,mStateRegs[1] );
        gfl2::math::VEC3 pos( (float)tx , (float)ty ,0.0f);      
        coreUISys->GetG2DUtil()->SetPanePos(0,mStateRegs[1],&pos);

        // Active Animation:
        coreUISys->GetG2DUtil()->StartAnime(0,mameButtonsAnimeIN[mStateRegs[0]]);
        break;
      }

      // Dirt Tool:      
      if(coreUISys->GuiMode()==GUI_MODE_CARE && coreUISys->GetG2DUtil()->GetHitPane(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_BG_CATETOOL_02,nowTouchRawX,nowTouchRawY))
      {
        coreUISys->GetCoreSys()->GetPokemonAI()->PlaySE( SEQ_SE_CATCH );

        SaveOriginalPositions(); // Backup Original Positions
        mStateRegs[0] = CARETOOL_DIRT; 
        mStateRegs[1] = PANENAME_KAWAIGARI_MIN_LOW_000_PANE_CARETOOL_02;       
        mStateRegSub++;         

        coreUISys->SetCareTextMessage(msg_pkparure_09_04);
        coreUISys->GetG2DUtil()->StartAnime( 2, LA_KW_LAYOUT_KAWAIGARI_MESSAGE_UPP_000__MESSAGE_L2_IN);

        coreUISys->GetG2DUtil()->AppendChild(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_NULL_00,mStateRegs[1] );
        gfl2::math::VEC3 pos( (float)tx , (float)ty ,0.0f);      
        coreUISys->GetG2DUtil()->SetPanePos(0,mStateRegs[1],&pos);

        // Active Animation:
        coreUISys->GetG2DUtil()->StartAnime(0,mameButtonsAnimeIN[mStateRegs[0]]);
        break;
      }

      // Dust Tool:
      if(coreUISys->GuiMode()==GUI_MODE_CARE && coreUISys->GetG2DUtil()->GetHitPane(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_BG_CATETOOL_03,nowTouchRawX,nowTouchRawY))
      {
        coreUISys->GetCoreSys()->GetPokemonAI()->PlaySE( SEQ_SE_CATCH );

        SaveOriginalPositions(); // Backup Original Positions
        mStateRegs[0] = CARETOOL_DUST;
        mStateRegs[1] = PANENAME_KAWAIGARI_MIN_LOW_000_PANE_CARETOOL_03;       
        mStateRegSub++;         

        coreUISys->SetCareTextMessage(msg_pkparure_09_03);
        coreUISys->GetG2DUtil()->StartAnime( 2, LA_KW_LAYOUT_KAWAIGARI_MESSAGE_UPP_000__MESSAGE_L2_IN);

        coreUISys->GetG2DUtil()->AppendChild(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_NULL_00,mStateRegs[1] );
        gfl2::math::VEC3 pos( (float)tx , (float)ty ,0.0f);      
        coreUISys->GetG2DUtil()->SetPanePos(0,mStateRegs[1],&pos);

        // Active Animation:
        coreUISys->GetG2DUtil()->StartAnime(0,mameButtonsAnimeIN[mStateRegs[0]]);
        break;
      }

      // Water Tool:
      if(coreUISys->GuiMode()==GUI_MODE_CARE && coreUISys->GetG2DUtil()->GetHitPane(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_BG_CATETOOL_04,nowTouchRawX,nowTouchRawY))
      {
        coreUISys->GetCoreSys()->GetPokemonAI()->PlaySE( SEQ_SE_CATCH );

        SaveOriginalPositions(); // Backup Original Positions
        mStateRegs[0] = CARETOOL_WATER; 
        mStateRegs[1] = PANENAME_KAWAIGARI_MIN_LOW_000_PANE_CARETOOL_04;       
        mStateRegSub++;         

        coreUISys->SetCareTextMessage(msg_pkparure_09_05);
        coreUISys->GetG2DUtil()->StartAnime( 2, LA_KW_LAYOUT_KAWAIGARI_MESSAGE_UPP_000__MESSAGE_L2_IN);

        coreUISys->GetG2DUtil()->AppendChild(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_NULL_00,mStateRegs[1] );
        gfl2::math::VEC3 pos( (float)tx , (float)ty ,0.0f);      
        coreUISys->GetG2DUtil()->SetPanePos(0,mStateRegs[1],&pos);

        // Active Animation:
        coreUISys->GetG2DUtil()->StartAnime(0,mameButtonsAnimeIN[mStateRegs[0]]);

        break;
      }

      break;

    // No longer touching the screen:
    case 5:                     
      if(!coreUISys->IsTouching())
      {       
        coreUISys->GetG2DUtil()->StartAnime( 2, LA_KW_LAYOUT_KAWAIGARI_MESSAGE_UPP_000__MESSAGE_L2_OUT);
        
        // CareToolFadeEffect();

        coreUISys->GetG2DUtil()->AppendChild(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_CAREMENU,mStateRegs[1]);
        
        RestoreOriginalPositions();

        // Deactive Animation:
        coreUISys->GetG2DUtil()->StartAnime(0,mameButtonsAnimeOUT[mStateRegs[0]]);

        mStateRegSub=4; // Loop
        break;
      }
      
      if(touchRawY>64)
      {
        coreUISys->DisableCareButton();
        coreUISys->GetG2DUtil()->StartAnime( 2, LA_KW_LAYOUT_KAWAIGARI_MESSAGE_UPP_000__MESSAGE_L2_OUT);

        if( mStateRegs[0] == CARETOOL_WATER ) TurnOnDryer();
   //     coreUISys->GetG2DUtil()->SetPaneVisible(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_CARETOOL_02 ,false);
        coreUISys->GetG2DUtil()->StartAnime( 0, LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000_CARE_SELECT);  

        coreUISys->GetG2DUtil()->AppendChild(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_NULL_00,mStateRegs[1]);

      //  gfl2::math::Vector3 pos( 0.0f,0.0f ,30.0f);      

        ///u32 touchX,touchY;
        ///coreUISys->GetTouchRawPos(touchX,touchY);
        ///coreUISys->GetCoreSys()->Get3DTouchPositionByXY( &pos, touchX, touchY, TPT_FOOD );

        ///pos.z += 10.0f;
        ///coreUISys->GetCoreSys()->CreateEffect(KAWAIGARI_EFFECT_SMOKE,&pos);

        if(coreUISys->GetCoreSys()->GetTroubleManager()->HasObjects()) 
          mStateRegSub=50; // Wait Touch End or No more Objects
        else mStateRegSub=60; // Wait Touch End

        coreUISys->GetCoreSys()->GetPokemonAI()->SetSubState(AI_SUBSTATE_CARE_MAIN);

        coreUISys->GetCoreSys()->GetPokemonAI()->SetCareToolType(mStateRegs[0]);

        if( mStateRegs[0] == CARETOOL_WATER ) Sound::PlaySE(SEQ_SE_KW_OTEIRE_DRYER,0,2);
        
        break;
      }

      // Update Mame Icon Position by Touch Pos:
      {

        //  gfl2::math::VEC3 posNewBaseRoot = coreUISys->GetG2DUtil()->GetPanePos(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_NULL_SCROLL);

      //  gfl2::math::VEC3 pos( (float)tx-posBaseRoot.x , (float)ty-posBaseRoot.y ,0.0f);      
        gfl2::math::VEC3 pos( (float)tx, (float)ty,0.0f);      

        coreUISys->GetG2DUtil()->SetPanePos(0,mStateRegs[1],&pos);
      }


      break;

    case 50:      
     // coreUISys->GetG2DUtil()->StartAnime( 0, LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__CAREBUTTON_MODE_OFF);

      if( mStateRegs[0] == CARETOOL_WATER)
      {
     //   if(coreUISys->DryerActive()) coreUISys->GetG2DUtil()->SetAnimeSpeed( 0, 2.0f );        
      //  else coreUISys->GetG2DUtil()->SetAnimeSpeed( 0, 1.0f );       
      }

      if(!coreUISys->GetCoreSys()->GetTroubleManager()->HasObjects()) 
      {
        if( mStateRegs[0] == CARETOOL_WATER ) TurnOffDryer();
        CareToolFadeEffect();        
        coreUISys->GetCoreSys()->GetPokemonAI()->SetCareToolType(CARETOOL_NONE);
        coreUISys->GetG2DUtil()->AppendChild(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_CAREMENU,mStateRegs[1]);
        RestoreOriginalPositions();
        coreUISys->GetG2DUtil()->StartAnime( 0, LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000_BUTTON_IN);
        mStateRegSub=70; // Wait No Touch
         coreUISys->GetG2DUtil()->StartAnime( 0, LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__CAREBUTTON_MODE_OFF);
        coreUISys->GetG2DUtil()->SetAnimeSpeed( 0, 1.0f );       
        break;
      }
    case 60: // FALL THROUGH!
      
      if( mStateRegs[0] == CARETOOL_WATER ) coreUISys->GetCoreSys()->GetPokemonAI()->PlayCareSE(SEQ_SE_KW_OTEIRE_DRYER); 
  //    coreUISys->GetG2DUtil()->StartAnime( 0, LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__CAREBUTTON_MODE_OFF);
      if(!coreUISys->IsTouching())
      {
        if( mStateRegs[0] == CARETOOL_WATER ) TurnOffDryer();

        CareToolFadeEffect();
        
        coreUISys->DisableCareButton();
        mStateRegs[3] = 20;
        mStateRegSub=98; // Wait & Loop
        coreUISys->GetG2DUtil()->SetAnimeSpeed( 0, 1.0f );     

        // Set Offscreen:
        gfl2::math::VEC3 posNewBaseRoot = coreUISys->GetG2DUtil()->GetPanePos(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_NULL_SCROLL);

        // gfl2::math::VEC3 pos( (float)tx-posBaseRoot.x , (float)ty-posBaseRoot.y ,0.0f);      
        gfl2::math::VEC3 pos( (float)-512, (float)-512 ,0.0f);      

        coreUISys->GetG2DUtil()->SetPanePos(0,mStateRegs[1],&pos);

        break;
      }

      // Update Mame Icon Position by Touch Pos:
      {

        gfl2::math::VEC3 posNewBaseRoot = coreUISys->GetG2DUtil()->GetPanePos(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_NULL_SCROLL);

       // gfl2::math::VEC3 pos( (float)tx-posBaseRoot.x , (float)ty-posBaseRoot.y ,0.0f);      
        gfl2::math::VEC3 pos( (float)tx, (float)ty ,0.0f);      

        coreUISys->GetG2DUtil()->SetPanePos(0,mStateRegs[1],&pos);
      }

   
      break;

    case 70:      
      if(!coreUISys->IsTouching())
      {        
        if( mStateRegs[0] == CARETOOL_WATER ) TurnOffDryer();
        coreUISys->SetGUIMode(GUI_MODE_NORMAL);

        // Deactive Animation:
        coreUISys->GetG2DUtil()->StartAnime(0,mameButtonsAnimeOUT[mStateRegs[0]]);

        mStateRegSub=0; // Wait No Touch
      }
      break;

    case 98:
      mStateRegs[3]--;

      if(mStateRegs[3]<=0)
      {
        coreUISys->GetCoreSys()->GetPokemonAI()->SetCareToolType(CARETOOL_NONE);
        coreUISys->GetG2DUtil()->AppendChild(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_CAREMENU,mStateRegs[1]);
        RestoreOriginalPositions();
        coreUISys->GetG2DUtil()->StartAnime( 0, LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000_BUTTON_IN);
        coreUISys->SetGUIMode(GUI_MODE_NORMAL);
        mStateRegSub++;
      }
      break;

   
    case 99:
      coreUISys->GetG2DUtil()->StartAnime( 0, LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000__CAREBUTTON_MODE_OFF);
      mStateRegSub = 0; // Loop!

      // Deactive Animation:
      coreUISys->GetG2DUtil()->StartAnime(0,mameButtonsAnimeOUT[mStateRegs[0]]);
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
      if(coreUISys->IsTouching() && coreUISys->GetG2DUtil()->GetHitPane(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_CAREBUTTON,nowTouchRawX,nowTouchRawY))
      {
        mStateRegs[3]=10;
      }

      if(mStateRegs[3]<=0)
      {
        mStateRegSub = 4; // LOOP
      }
      break;

  }

}

void KawaigariUICareMenuBar::State_End(void)
{  
}

void KawaigariUICareMenuBar::State_Cleanup(void)
{  
}

void KawaigariUICareMenuBar::State_Finish(void)
{  
}


void KawaigariUICareMenuBar::Update(void)
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
  ///PrintState();
}

void KawaigariUICareMenuBar::SaveOriginalPositions(void)
{
  // Get the Root Base Pos:
  posBaseRoot = coreUISys->GetG2DUtil()->GetPanePos(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_CAREMENU);

  // Get each Mame Position:
  careBasePos[0] = coreUISys->GetG2DUtil()->GetPanePos(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_CARETOOL_00);
  careBasePos[1] = coreUISys->GetG2DUtil()->GetPanePos(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_CARETOOL_01);
  careBasePos[2] = coreUISys->GetG2DUtil()->GetPanePos(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_CARETOOL_02);
  careBasePos[3] = coreUISys->GetG2DUtil()->GetPanePos(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_CARETOOL_03);
  careBasePos[4] = coreUISys->GetG2DUtil()->GetPanePos(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_CARETOOL_04);

  for(int n=0;n<MAX_CARE_ICON;n++)
  {
    careBasePos[n].x-=posBaseRoot.x;
    careBasePos[n].y-=posBaseRoot.y;
  }
}

void KawaigariUICareMenuBar::RestoreOriginalPositions(void)
{ 
  // Get the Root Base Pos:
  posBaseRoot = coreUISys->GetG2DUtil()->GetPanePos(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_CAREMENU);

  coreUISys->GetG2DUtil()->SetPanePos(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_CARETOOL_00,&careBasePos[0]);
  coreUISys->GetG2DUtil()->SetPanePos(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_CARETOOL_01,&careBasePos[1]);
  coreUISys->GetG2DUtil()->SetPanePos(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_CARETOOL_02,&careBasePos[2]);
  coreUISys->GetG2DUtil()->SetPanePos(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_CARETOOL_03,&careBasePos[3]);
  coreUISys->GetG2DUtil()->SetPanePos(0,PANENAME_KAWAIGARI_MIN_LOW_000_PANE_CARETOOL_04,&careBasePos[4]);

}


  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)