//======================================================================
/**
 * @file	KawaigariPokemon_StateIdle.C++
 * @brief	Kawaigari Pokemon Actor - Idle State Code
 * @author	PETE
 * @data	2015.10.29
 */
//======================================================================
#include <macro/include/gfl2_Macros.h>
#include <gfx/include/gfl2_types.h>
#include <GameSys/include/GameProcManager.h>

#include "PokeTool/include/PokeModel.h"

#include "KawaigariMain.h"
#include "KawaigariPokemon.h"
#include "KawaigariRenderSystem.h"
#include "KawaigariRandomActionSelector.h"

// Fade
#include <Fade/include/gfl2_FadeManager.h>


GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

void KawaigariPokemon::State_Idle(void)
{
  
  switch(mStateRegSub)
  {
    case AI_SUBSTATE_IDLE_INIT: State_Idle_Init(); break;
    case AI_SUBSTATE_IDLE_MAIN: State_Idle_Main(); break;
    case AI_SUBSTATE_IDLE_PLAYMOTION: State_Idle_PlayMotion(); break;
    case AI_SUBSTATE_IDLE_LOOKAROUND: State_Idle_LookAround(); break;
    case AI_SUBSTATE_IDLE_SLEEP: State_Idle_Sleep(); break;
  }
  /*

  // Just idle for 5 seconds and then go back to appear:
  switch(mStateRegSub)
  {
    case 0:    
      ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT);
      mStateRegs[0] = WAIT_SECOND*5;
      mStateRegSub++;
      break;

    case 1:
      mStateRegs[0]--;

      if(mStateRegs[0]<20)
      {
        // Fade to black:
        gfl2::math::Vector4 blackCol(0.0f,0.0f,0.0f,0.0f);					
        gfl2::math::Vector4 blackCol2(0.0f,0.0f,0.0f,255.0f);					
        gfl2::math::Vector4 *col = &blackCol;
        gfl2::math::Vector4 *col2 = &blackCol2;				
        GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA, col, col2,15);			
        GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_ALPHA, col, col2,15);			
        mStateRegSub++;
      }
      break;

    case 2:
      mStateRegs[0]--;

      if(mStateRegs[0]<=0)
      {        
        SetState(AI_STATE_APPEAR);
      }
      break;
    }
    */
}

void KawaigariPokemon::State_Idle_Init(void)
{

  KawaigariRandomActionSelector *selector = coreSys->GetRandomActionSelector();
  selector->Reset();
    
  if(coreSys->GetTroubleManager()->HasObjects())
  {
    SetSlowMotion(UseSlowMotion());
    SetState(AI_STATE_CAREWAIT);
  }
  else 
  {
    SetSlowMotion(false);
    SetSubState(AI_SUBSTATE_IDLE_MAIN);
  }

  UpdateAgain();  

  if(!coreSys->GetTroubleManager()->HasStatusEffect())
  {  
  //  ChangeFace(TEX_MOUTH, FACE_MOUTH_DEFAULT,true);
  //  ChangeFace(TEX_EYE, FACE_EYE_DEFAULT,true);
    mModel->ResetFaceAnime_Eye();
    mModel->ResetFaceAnime_Mouth();
  }

  coreSys->GetCamera()->SetNewTarget(KG_CAMERA_WHOLE_BODY);
}

void KawaigariPokemon::State_Idle_Main(void)
{
  KawaigariRandomActionSelector *selector = coreSys->GetRandomActionSelector();
  int action,motion;

  switch(mStateRegs[0])
  {
    case 0:
      SetTouchOK(true);
      coreSys->GetUIManager()->EnableEatModeButton();
      coreSys->GetUIManager()->EnableCareButton();      
     // coreSys->GetUIManager()->EnablePokemonSelectButton();

      coreSys->GetCamera()->ResetIdleCamera();
      coreSys->GetCamera()->SetRandomIdleCameraPosition();      
      coreSys->GetCamera()->SetUseIdleCamera(false);

      ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT);

      // Turn on Sleep Flag if PLAYFUL & AFFECTION Level is High:
      if(GetPlayfulLevel() >= PLAYFUL_LEVEL1 && GetFullnessLevel()>= FULLNESS_LEVEL1)
      {
        if(CanSleepFirstTime()) mStateRegs[3] = 1; // Sleep Flag ON
      }

      mStateRegs[0]++;
       
      break;

    case 1:      
      
      // Set IdleCount before doing random action based on Affection Level:
      if(GetAffectionLevel()<50) mStateRegs[1] = 3 + coreSys->GetRandom(2);
      else if(GetAffectionLevel()<150) mStateRegs[1] = 2 + coreSys->GetRandom(3);
      else mStateRegs[1] = 2 + coreSys->GetRandom(2);

      if(IsCharacterID(MONSNO_TESSIIDO)) mStateRegs[1]+=20; // Add 20 Frames of IDLE (Idle Motion is 1 Frame)

      mStateRegs[0]++;
      break;

    case 2:            

      // Time to do random action:
      if(mStateRegs[1]<=0)
      {                
        selector->Reset();
        
        // Try to set a motion (OLD::CARE1/CARE2)
        if(GetPlayfulLevel()<=30 && coreSys->GetRandom(100)<80)
        {                   
          selector->AddIdleCom(AI_SUBSTATE_IDLE_PLAYMOTION,PokeTool::MODEL_ANIME_PLAY_A);
          selector->AddIdleCom(AI_SUBSTATE_IDLE_PLAYMOTION,PokeTool::MODEL_ANIME_PLAY_B);
          
          if(selector->SelectRandomAction(action) && selector->GetSelectedMotion(motion))
          {          
            mModel->ResetFaceAnime_Eye();
            mModel->ResetFaceAnime_Mouth();
            ChangeAnimation(motion);
          }
        }        
        else if(!selector->GetSelectedAction(action)) // Try a different set of actions/motions instead:
        {
          selector->Reset();
          selector->AddIdleCom(AI_SUBSTATE_IDLE_LOOKAROUND);
          selector->AddIdleCom(AI_SUBSTATE_IDLE_PLAYMOTION,PokeTool::MODEL_ANIME_MOVE_A); // HABIT1
          selector->AddIdleCom(AI_SUBSTATE_IDLE_PLAYMOTION,PokeTool::MODEL_ANIME_MOVE_B); // HABIT2
          selector->AddIdleCom(AI_SUBSTATE_IDLE_PLAYMOTION,PokeTool::MODEL_ANIME_MOVE_C); // HABIT3
          selector->AddIdleCom(AI_SUBSTATE_IDLE_PLAYMOTION,PokeTool::MODEL_ANIME_QUESTION); // NOIDEA

          // Try and Select Random Action!
          if(selector->SelectRandomAction(action) && selector->GetSelectedMotion(motion))
          {
              if(action==AI_SUBSTATE_IDLE_PLAYMOTION) 
              {
                mModel->ResetFaceAnime_Eye();
                mModel->ResetFaceAnime_Mouth();
                ChangeAnimation(motion);          
              }
          }
        }
      }
      
      mStateRegs[0] = 50; // Mid Loop

      if(selector->GetSelectedAction(action))
      {        
        coreSys->GetCamera()->SetUseIdleCamera(false);  
        SaveState(); // Come back at Prog::(50)        
        SetSubState(action); // Change to LookAround or PlayMotion, Restores State(50) on Return
        selector->Reset();                
        break;
      }    

      // Just Continue in IDLE Animation:
      else
      {
        selector->Reset();
        ///coreSys->GetCamera()->SetRandomIdleCameraPosition();  
        ///coreSys->GetCamera()->SetUseIdleCamera(true);  
        mStateRegs[0] = 50; // Mid Loop
        coreSys->GetRenderSystem()->GetRenderingPipeLine()->SetUseTouchIDTexture(true);
      }
      break;

    case 50: // Mid Loop  while(!EndOfMotion())
            
      mStateRegs[2]++; // No Touch Count

      UpdateBlink();

      if(coreSys->GetUIManager()->IsPressingButton())
      {
        SetTouchOK(false);
        mTouchOKCount = 0;
      }      
      else
      {
        if(mTouchOKCount>5)
        {
          mTouchOKCount = 0;
          mTouchOK = true;
        }
      }

      if(IsTouchStart() && !coreSys->GetUIManager()->IsPressingButton() && mTouchOK) 
      {
        mStateRegs[2] = 0; // Reset No Touch Count
        ///coreSys->GetCamera()->SetRandomIdleCameraPosition();  
        coreSys->GetCamera()->SetUseIdleCamera(false);  
        SaveState();
        SetState(AI_STATE_TOUCH);
        UpdateAgain();
        break;
      }
      else if(mStateRegs[2] >= WAIT_SECOND*30 || mStateRegs[3]) // Goto Sleep if no touch for 30 seconds
      {
        coreSys->GetRenderSystem()->GetRenderingPipeLine()->SetUseTouchIDTexture(false);
        mStateRegs[3] = 0; // Reset Sleep Flag (Only Sleep Once by Flag)

        SetDualScreenMonsterCamera(KawaigariRenderSystem::DUAL_SCREEN_MONSTER_CAMERA_GO_LOWER);
        // Reset Idle Camera:
        ///coreSys->GetCamera()->SetRandomIdleCameraPosition();  
        coreSys->GetCamera()->SetUseIdleCamera(false);
        mStateRegs[2] = 0; // MIGHT Happen during sleep...

        SaveState();

        if(coreSys->Mode()==KAWAIGARI_MODE_FIELD || coreSys->Mode()==KAWAIGARI_MODE_RESORT) SetState(AI_STATE_FADEEND);
        else SetSubState(AI_SUBSTATE_IDLE_SLEEP);

        break;
      }
      
      if(mStateRegs[2] >= WAIT_SECOND*5) // Enable Auto Camera if No Touch for 5 seconds
      {
        // Turn on Auto Camera
        coreSys->GetCamera()->SetUseIdleCamera(true);        
        coreSys->GetCamera()->UpdateIdleCamera();
      }
      else
      {
         coreSys->GetCamera()->SetUseIdleCamera(false);  
      }

      // Camera is set during Facial Recognition, but lets just set it as it FR was not used:
      coreSys->GetCamera()->SetNewTarget(KG_CAMERA_BUST_UP);
      
      // End Of Cycle:
      if(mModel->IsAnimationLastFrame())
      {
        mStateRegs[1]--; // Lower Idle Count

        if(mStateRegs[1]<0) mStateRegs[0] = 1; // Reset Idle Count
        else mStateRegs[0] = 2; // Loop
      }
      
      break;

  }
}

void KawaigariPokemon::State_Idle_PlayMotion(void)
{
  switch(mStateRegs[0])
  {
    case 0:
      coreSys->GetUIManager()->DisableMainButtons();
      mStateRegs[0]++;
      break;

    case 1:
      if(mModel->IsAnimationLastFrame())
      {
        mStateRegs[0]=999;
        ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT);
      }
      break;

    case 999:      
      coreSys->GetUIManager()->EnableMainButtons();
      RestoreState();
      break;  
  }
}

void KawaigariPokemon::State_Idle_LookAround(void)
{
  switch(mStateRegs[0])
  {
    case 0:
      coreSys->GetUIManager()->DisableMainButtons();
      mStateRegs[1] = WAIT_SECOND*2+ coreSys->GetRandom(WAIT_SECOND*1);
      
      lookPos.x = -100.0f + (coreSys->GetRandom(200));
      lookPos.y = -100.0f + (coreSys->GetRandom(200));
      lookPos.z = 100.0f;
      SetInterestPosition(&lookPos);

      mStateRegs[0]++;
      break;

    case 1:
      // Move Pokemon Head Around here!

      mStateRegs[1]--;

      if(mStateRegs[1]<=0)
      {
        // Reset Timer:
        mStateRegs[1] = WAIT_SECOND*2+ coreSys->GetRandom(WAIT_SECOND*1);

        lookPos.x = -100.0f + (coreSys->GetRandom(200));
        lookPos.y = -100.0f + (coreSys->GetRandom(200));
        lookPos.z = 100.0f;
        SetInterestPosition(&lookPos);
      }

      // Exit after 9 Seconds:
      mStateRegs[2]++;
      if(mStateRegs[2]>WAIT_SECOND*9) mStateRegs[0]=999;

      if(IsTouch())
      {
        SetInterestPosition(NULL);
        mStateRegs[0] = 999;
        UpdateAgain();
        break;
      }

      UpdateBlink();

      break;

    case 999: // Exit to APPEAR_FINISH:
      coreSys->GetUIManager()->EnableMainButtons();
      SetInterestPosition(NULL);
      ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT);
      RestoreState();
      break;        
  }
}

void KawaigariPokemon::State_Idle_Sleep(void)
{
  switch(mStateRegs[0])
  {
    case 0:
      coreSys->GetUIManager()->DisableMainButtons();
      ///SetDualMonsterCamera( DUAL_SCREEN_MONSTER_CAMERA_GO_LOWER );
      SetDualScreenMonsterCamera( KawaigariRenderSystem::DUAL_SCREEN_MONSTER_CAMERA_GO_LOWER );

      coreSys->GetCamera()->SetNewTarget(KG_CAMERA_FAR,0.002f);
      // LookEnd();

      if(CanSetMotion(PokeTool::MODEL_ANIME_DROWSE_A))
      {
          ChangeAnimation(PokeTool::MODEL_ANIME_DROWSE_A);
          mStateRegs[1] = 1; // Sleep Motion Available
      }
      else 
      {
          mStateRegs[1] = ChangeAnimation(PokeTool::MODEL_ANIME_DROWSE_B);
      }

      mStateRegs[3] = WAIT_SECOND * 3; // Sleep Timer if Motion isn't available!

      mStateRegs[5] = 30; // Wait 30 frames before sleep effect
      
      mStateRegs[0]++;
      break;

    case 1:
      if(mStateRegs[5]>0) mStateRegs[5]--;

      if(mStateRegs[5]==0)
      {
        mStateRegs[5]=-1;
        gfl2::math::Vector3 nodePos;
        mModel->GetEffectNodePosition(PokeTool::MODEL_NODE_OVERHEAD,0,&nodePos);
        mSleepEffect = coreSys->CreateCareEffect(KAWAIGARI_EFFECT_SLEEPY,&nodePos);
      }
   
      // Sleep Interupt Logic:
      if(IsTouchRelease())
      {
        mStateRegs[0] = 5; // User Touch Wake up program
        UpdateAgain();
        break;
      }

      // Update Sleep effect Position:
      if(mSleepEffect!=-1) coreSys->UpdateSleepEffectPos(mSleepEffect);   

      // End of Loop Logic:

      if(!mStateRegs[1]) // Use Sleep Timer?
      {
          mStateRegs[3]--; // Dec Sleep Timer if Sleep Motion Isn't available

          // Still Sleeping?
          if(mStateRegs[3]<=0) // End Sleep          
          {
            mStateRegs[0]++;
            break;
          }

          mStateRegs[6]++; // Sleep Sound Counter

          // PlaySound Every Second+1/2:
          if(mStateRegs[6]>=(WAIT_SECOND + (WAIT_SECOND/2)))
          {            
            PlaySE(KW_ROW);
            mStateRegs[6]=0;
          }
                    
          break;
      }
      else // Check End of Motion:
      {
        if(!mModel->IsAnimationLastFrame()) break; // Keep going until motion ends
     ///   else if(!mStateRegs[2])
        {
          ChangeAnimationNotSmooth(PokeTool::MODEL_ANIME_DROWSE_B,mStateRegs[2]==2); // Don't play SE on last loop
        }
      }

      

      mStateRegs[2]++;
      if(mStateRegs[2] == 3) mStateRegs[0]++; // End of 3 Loop
      break;


    case 2: // Sleep Ended Due to timeout:
      if(mSleepEffect!=-1) coreSys->GetCareEffect(mSleepEffect)->StopParticle();
      StopSE(KW_ROW);
      SetState(AI_STATE_END); // Completely Exit the mode
      break;


    case 5: // User Touch Wakeup
      if(mSleepEffect!=-1) coreSys->GetCareEffect(mSleepEffect)->StopParticle();

      if(ChangeAnimation(PokeTool::MODEL_ANIME_DROWSE_C))
      {
        mStateRegs[0]++;
      }
      else 
      {        
        mStateRegs[0]=7;
        UpdateAgain();        
      }
      break;

    case 6: // Wait until end of motion
      if(mModel->IsAnimationLastFrame()) mStateRegs[0]++;
      break;

    case 7:      
      coreSys->GetCamera()->SetNewTarget(KG_CAMERA_WHOLE_BODY);
      mStateRegs[0] = 999; // Exit back to Idle_Main
      break;

    case 999:
      coreSys->GetUIManager()->EnableMainButtons();
      if(mSleepEffect!=-1)
      {
        coreSys->KillCareEffect(mSleepEffect); 
        mSleepEffect = -1;
      }
      coreSys->GetCamera()->SetRandomIdleCameraPosition();      
      ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT);
      RestoreState();
      break;
  }
}

  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)