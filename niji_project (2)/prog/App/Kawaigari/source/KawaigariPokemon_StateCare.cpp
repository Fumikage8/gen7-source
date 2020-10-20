//======================================================================
/**
 * @file	KawaigariPokemon_StateCare.C++
 * @brief	Kawaigari Pokemon Actor - Care State Code
 * @author	PETE
 * @data	2015.12.14
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

// Sound:
#include <niji_conv_header/sound/VoiceID.h>
#include <niji_conv_header/sound/SoundMiddleID.h>
#include "Sound/include/Sound.h"

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)


void KawaigariPokemon::State_Care(void)
{

   switch(mStateRegSub)
  {
    case AI_SUBSTATE_CARE_INIT: State_Care_Init(); break;
    case AI_SUBSTATE_CARE_MAIN: State_Care_Main(); break;
    case AI_SUBSTATE_CARE_WAIT: State_Care_Wait(); break;
  }

}

void KawaigariPokemon::State_Care_Wait(void)
{  
  if(coreSys->GetTroubleManager()->HasSleepyStatus() )
  {
    if(mModel->IsAnimationLastFrame()) 
    {
      ChangeAnimation(PokeTool::MODEL_ANIME_DROWSE_B);        
      SetSlowMotion(UseSlowMotion());
    }
  }
  else if(!CheckMotion(PokeTool::MODEL_ANIME_KW_WAIT) && mModel->IsAnimationLastFrame())
  {
    ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT);        
    SetSlowMotion(UseSlowMotion());
  }

  if(coreSys->GetTroubleManager()->HasStatusEffect() && !coreSys->GetTroubleManager()->HasSleepyStatus())
  { 
    ChangeFace( TEX_MOUTH, FACE_MOUTH_TOUCH_HATE, true );
    ChangeFace( TEX_EYE, FACE_EYE_TOUCH_HATE, true );
  }
  else if(!coreSys->GetTroubleManager()->HasSleepyStatus())
  {
    UpdateBlink(); 
  }
}

void KawaigariPokemon::State_Care_Main(void)
{  
  bool correctTouch = false;

  if(GetCareToolType()==CARETOOL_WATER && !IsTouchSlide(1)) correctTouch = true;
  else if(IsTouchSlide(1) && !IsTouchSlide(30)) correctTouch = true;

  float checkDist = 35.0f;
  if(GetCareToolType()==CARETOOL_WATER) checkDist = 35.0f;

  
  switch(mStateRegs[0])
  {
    case 0: // Make sure in IDLE animation first:     

      // Turn On Face/Mouth Animation Smoothing:
      mModel->SetFaceAnimeSmoothEnable(true);

      if(coreSys->GetTroubleManager()->HasSleepyStatus()) mStateRegs[7] = 1;
     
      coreSys->GetRenderSystem()->GetRenderingPipeLine()->SetUseTouchIDTexture(true);
      ///SetDualScreenMonsterCamera( KawaigariRenderSystem::DUAL_SCREEN_MONSTER_CAMERA_GO_UPPER );
      
      if(coreSys->GetTroubleManager()->HasSleepyStatus()) coreSys->GetCamera()->SetNewTarget(KG_CAMERA_ALL_MOTION);
      else coreSys->GetCamera()->SetNewTarget(KG_CAMERA_WHOLE_BODY);

      if(!coreSys->GetTroubleManager()->HasStatusEffect()) mModel->ResetFaceAnime_Eye();

      if(!coreSys->GetTroubleManager()->HasStatusEffect())
      {
        mModel->ResetFaceAnime_Mouth();
      }

      if(CheckMotion(PokeTool::MODEL_ANIME_KW_WAIT) && !coreSys->GetTroubleManager()->HasSleepyStatus())
      {
        if(coreSys->GetTroubleManager()->HasObjects()) mStateRegs[0]++;
        else mStateRegs[0]=5; // Wait end touch mode          
      }

      else if(mModel->IsAnimationLastFrame() && !coreSys->GetTroubleManager()->HasSleepyStatus())
      {
        ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT);           
        SetSlowMotion(UseSlowMotion());

        if(coreSys->GetTroubleManager()->HasObjects()) mStateRegs[0]++;
        else mStateRegs[0]=5; // Wait end touch mode
        
        break;
      }
      else
      {
        if(coreSys->GetTroubleManager()->HasObjects()) mStateRegs[0]++;
        else mStateRegs[0]=5; // Wait end touch mode
      }
      break;

    case 1:           
      
      if(coreSys->GetTroubleManager()->HasSleepyStatus())
      {
        if(mModel->IsAnimationLastFrame()) 
        {
          ChangeAnimation(PokeTool::MODEL_ANIME_DROWSE_B);        
          SetSlowMotion(UseSlowMotion());
        }

       // ChangeFace( TEX_EYE, FACE_EYE_EYECLOSE, true );
      }
      else if(coreSys->GetTroubleManager()->HasStatusEffect())
      { 
        ChangeFace( TEX_MOUTH, FACE_MOUTH_TOUCH_HATE, true );
      }

      coreSys->GetUIManager()->SetDryerActive(false);
      
      if(correctTouch  && coreSys->GetTroubleManager()->CheckDistance(checkDist) )
///      if(correctTouch && GetTouchColor() && coreSys->GetTroubleManager()->CheckDistance(89.0f+30.0f) )
      {
        if(GetCareToolType()==CARETOOL_WATER) coreSys->GetUIManager()->SetDryerActive(true);
      
        if(mStateRegs[2]%10==0 && !coreSys->GetTroubleManager()->SuccessSoundPlay())
        {
          switch(GetCareToolType())
          {
          case CARETOOL_WATER: break;            
          case CARETOOL_BANDAGE: coreSys->GetPokemonAI()->PlayCareSE(SEQ_SE_KW_OTEIRE_OMNISHEET); break;                        
          case CARETOOL_HAIR: coreSys->GetPokemonAI()->PlayCareSE(SEQ_SE_KW_OTEIRE_COMB); break;            
          case CARETOOL_DIRT: coreSys->GetPokemonAI()->PlayCareSE(SEQ_SE_KW_OTEIRE_TOWEL); break;                        
          case CARETOOL_DUST: coreSys->GetPokemonAI()->PlayCareSE(SEQ_SE_KW_OTEIRE_BRUSH); break;                        

          default: Sound::PlaySE( SEQ_SE_KW_STROKE,0,1 ); break;
          }
          
        }
        mStateRegs[2]++;
        
        if(mStateRegs[2]>=6)
        {                            
          mStateRegs[3] = 0;         

          if(!coreSys->GetTroubleManager()->HasSleepyStatus() && !coreSys->GetTroubleManager()->HasFreezeStatus())                   
            ChangeFace( TEX_EYE, FACE_EYE_TOUCHD, true );

        }

        else if(mStateRegs[2]>=5)
        {                            
          mStateRegs[3] = 0;                    

          if(!coreSys->GetTroubleManager()->HasSleepyStatus() && !coreSys->GetTroubleManager()->HasFreezeStatus())
            ChangeFace( TEX_EYE, FACE_EYE_TOUCHD, true ); // DON'T USE HALF OPEN EYES (USE CLOSE)
        }
        
        coreSys->GetTroubleManager()->Scrub(checkDist);
        coreSys->GetTroubleManager()->Update();
        coreSys->GetTroubleManager()->SetCanUpdate(false); // Already updated for this frame..
                
      }
      else
      {
        mStateRegs[3]++;
        if(mStateRegs[3]>15)
        {          
          mStateRegs[2] = 0;
          mStateRegs[3] = 0;
          
          if(coreSys->GetTroubleManager()->HasSleepyStatus())
          {
            //ChangeFace( TEX_EYE, FACE_EYE_EYECLOSE, true );
          }
          else if(coreSys->GetTroubleManager()->HasStatusEffect())
          {
            ChangeFace( TEX_EYE, FACE_EYE_TOUCH_HATE, true );
          }
          else mModel->SetFaceAnime_Eye(PokeTool::EYETYPE_OPEN);   
        }
      }

      if(!coreSys->GetTroubleManager()->HasObjects())
      {
        coreSys->GetRenderSystem()->GetRenderingPipeLine()->SetUseTouchIDTexture(false);
        mStateRegs[0]=50; // Success!

        if(mStateRegs[7]) StopSE(KW_ROW);
        break;
      }

      if(!IsTouching())
      {        
        coreSys->GetRenderSystem()->GetRenderingPipeLine()->SetUseTouchIDTexture(false);
        if(coreSys->GetTroubleManager()->HasSleepyStatus())
        {
        //  ChangeFace( TEX_EYE, FACE_EYE_EYECLOSE, true );
        }
        else if(coreSys->GetTroubleManager()->HasStatusEffect())
        {
          ChangeFace( TEX_EYE, FACE_EYE_TOUCH_HATE, true );
        }
        else
        {
          mModel->SetFaceAnime_Eye(PokeTool::EYETYPE_OPEN);   
          SetSlowMotion(false);
        }
        SetState(AI_STATE_IDLE);
      }
      break;

    case 5:     
      coreSys->GetRenderSystem()->GetRenderingPipeLine()->SetUseTouchIDTexture(false);

      if(mModel->IsAnimationLastFrame() && coreSys->GetTroubleManager()->HasSleepyStatus()) 
        ChangeAnimation(PokeTool::MODEL_ANIME_DROWSE_B);        

      if(!IsTouching())
      {        
        if(coreSys->GetTroubleManager()->HasSleepyStatus())
        {
        //  ChangeFace( TEX_EYE, FACE_EYE_EYECLOSE, true );
        }
        else if(coreSys->GetTroubleManager()->HasStatusEffect())
        {          
          ChangeFace( TEX_EYE, FACE_EYE_TOUCH_HATE, true );
        }
        else 
        {
          mModel->SetFaceAnime_Eye(PokeTool::EYETYPE_OPEN);   
          SetSlowMotion(false);
        }
        SetState(AI_STATE_IDLE);
      }
      break;

    case 50: // Clean Success:
      coreSys->GetRenderSystem()->GetRenderingPipeLine()->SetUseTouchIDTexture(false);
      SetSlowMotion(false);

      //ChangeFace( TEX_MOUTH, FACE_MOUTH_DEFAULT, true );
      mModel->ResetFaceAnime_Eye();
      mModel->ResetFaceAnime_Mouth();

      mModel->SetFaceAnime_Eye(PokeTool::EYETYPE_OPEN);

      if(mStateRegs[7]) // Was Sleeping
      {
        if(ChangeAnimation(PokeTool::MODEL_ANIME_DROWSE_C,true))
        {
          mStateRegs[0]=61;
          break;
        }        
      }

    ///  coreSys->CreateEffect(KAWAIGARI_EFFECT_CLEANSUCCESS);
      ChangeAnimation(PokeTool::MODEL_ANIME_HAPPY_A);

      AddAffectionAmount(4); // Need 3 hearts...
      RemovePlayfulnessAmount(51);

      mStateRegs[0]++;
      break;

    case 51:
      if(mModel->IsAnimationLastFrame())
      {
        ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT);           
        mStateRegs[0]++;
      }
      break;

    case 52:
      if(!IsTouching())
      {      
        SetState(AI_STATE_IDLE);
      }
      break;

    case 61: // Need To finish DROWSE_C ANIMATION:
      if(mModel->IsAnimationLastFrame())
      {
        ChangeAnimation(PokeTool::MODEL_ANIME_HAPPY_A);

        AddAffectionAmount(4); // Need 3 hearts...
        RemovePlayfulnessAmount(51);
        mStateRegs[0] = 51;
      }
      break;
  }

   
}


void KawaigariPokemon::State_Care_Init(void)
{
  SetTouchIcon(-1);
  SetSubState(AI_SUBSTATE_CARE_WAIT);
  
  if(coreSys->GetTroubleManager()->HasSleepyStatus()) coreSys->GetCamera()->SetNewTarget(KG_CAMERA_ALL_MOTION);
  else coreSys->GetCamera()->SetNewTarget(KG_CAMERA_WHOLE_BODY);

  coreSys->GetRenderSystem()->GetRenderingPipeLine()->SetUseTouchIDTexture(false);

}

  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)