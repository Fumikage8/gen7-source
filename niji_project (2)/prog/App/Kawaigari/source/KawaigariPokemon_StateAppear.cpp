//======================================================================
/**
 * @file	KawaigariPokemon_Appear.C++
 * @brief	Kawaigari Pokemon Actor - Appear State Code
 * @author	PETE
 * @data	2015.10.26
 */
//======================================================================
#include <macro/include/gfl2_Macros.h>
#include <gfx/include/gfl2_types.h>
#include <GameSys/include/GameProcManager.h>

#include "PokeTool/include/PokeModel.h"

#include "KawaigariMain.h"
#include "KawaigariPokemon.h"
#include "KawaigariRandomActionSelector.h"
#include "KawaigariRenderSystem.h"

#include "Sound/include/Sound.h"

// Fade
#include <Fade/include/gfl2_FadeManager.h>

#include "niji_conv_header/app/kawaigari/res2d/kw_layout_anim_list.h"

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

void KawaigariPokemon::State_Appear(void)
{
  
  switch( mStateRegSub )
  {
    case AI_SUBSTATE_INIT:            
      
      coreSys->GetUIManager()->GetG2DUtil()->StartAnime( 0, LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000_BUTTON_OUT);

      // Fade In:
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA,15); 
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_ALPHA,15);
      // Use Far Camera by default:
      coreSys->GetCamera()->SetLocationDirect(KG_CAMERA_FAR);
      
      if(mForceAppearState==-1)
        SetSubState( ChooseAppearState() );
      else SetSubState( mForceAppearState + 2);
      
      UpdateAgain(); // Run Again same frame
      break;

    case AI_SUBSTATE_APPEAR_SICK:      
      State_Appear_Sick();
      break;    

    case AI_SUBSTATE_APPEAR_UPPER:
      State_Appear_Upper();
      break;

    case AI_SUBSTATE_APPEAR_LOWER:
      State_Appear_Lower();
      break;

    case AI_SUBSTATE_APPEAR_BEHIND:
      State_Appear_Behind();
      break;

    case AI_SUBSTATE_APPEAR_LEFT:
      State_Appear_Left();
      break;

    case AI_SUBSTATE_APPEAR_RIGHT:
      State_Appear_Right();
      break;

    case AI_SUBSTATE_APPEAR_DO_UP:
      State_Appear_Do_Up();
      break;

    case AI_SUBSTATE_APPEAR_BACK:
      State_Appear_Back();
      break;

    case AI_SUBSTATE_APPEAR_DROWSEY:
      State_Appear_Drowsey();
      break;

    case AI_SUBSTATE_APPEAR_SLEEP:
      State_Appear_Sleep();
      break;

    case AI_SUBSTATE_APPEAR_LOOKAROUND:
      State_Appear_LookAround();
      break;

    case AI_SUBSTATE_APPEAR_PLAYMOTION:
      State_Appear_PlayMotion();
      break;    

    case AI_SUBSTATE_APPEAR_FINISH:
      State_Appear_Finish();
      break;    

      // No State, just exit to main:
    case AI_SUBSTATE_APPEAR_NONE:
    default:          
      SetState(AI_STATE_MAIN);      
      break;

  }
}

int KawaigariPokemon::ChooseAppearState(void)
{
  KawaigariRandomActionSelector *selector = coreSys->GetRandomActionSelector();
  selector->Reset();

  // Change to Idle Motion by default:
  if(!IsSick()) ChangeAnimationNotSmooth(PokeTool::MODEL_ANIME_KW_WAIT);
  
  //return(AI_SUBSTATE_APPEAR_BACK);
  /*
  switch(coreSys->GetRandom(10))
  {
  case 0: return(AI_SUBSTATE_APPEAR_DO_UP);
  case 1: return(AI_SUBSTATE_APPEAR_LOWER);
  case 2: return(AI_SUBSTATE_APPEAR_UPPER);
  case 3: return(AI_SUBSTATE_APPEAR_LEFT);
  case 4: return(AI_SUBSTATE_APPEAR_RIGHT);
  case 5: return(AI_SUBSTATE_APPEAR_SICK);
  case 6: return(AI_SUBSTATE_APPEAR_BACK);
  case 7: return(AI_SUBSTATE_APPEAR_DROWSEY);
  case 8: return(AI_SUBSTATE_APPEAR_LOOKAROUND);
  case 9: return(AI_SUBSTATE_APPEAR_SLEEP);
  }

  */

  if(IsSick()) return(AI_SUBSTATE_APPEAR_SICK);
  
  if(IsCalledByMonster()) 
  {
    selector->AddAppearCom(AI_SUBSTATE_APPEAR_DO_UP);
    selector->AddAppearCom(AI_SUBSTATE_APPEAR_PLAYMOTION,PokeTool::MODEL_ANIME_PLAY_A);
    selector->AddAppearCom(AI_SUBSTATE_APPEAR_PLAYMOTION,PokeTool::MODEL_ANIME_PLAY_B);      
  }

  else if(IsIgnoredTooLong())
  {
    // Use Lonely Motion if available, otherwise, Appear_Back:
    if(CanSetMotion(PokeTool::MODEL_ANIME_LONELY))
      selector->AddAppearCom(AI_SUBSTATE_APPEAR_PLAYMOTION,PokeTool::MODEL_ANIME_LONELY);
    else
      selector->AddAppearCom(AI_SUBSTATE_APPEAR_BACK);    
  }
  
  else if(GetPlayfulLevel() >= PLAYFUL_LEVEL1 && GetFullnessLevel()>= FULLNESS_LEVEL1)
  {
    if(CanSetMotion(PokeTool::MODEL_ANIME_SLEEP_A))
      selector->AddAppearCom(AI_SUBSTATE_APPEAR_SLEEP);    

    selector->AddAppearCom(AI_SUBSTATE_APPEAR_DROWSEY);        
  }

  else if(GetAffectionLevel() >= AFFECTION_LEVEL1)
  {
    selector->AddAppearCom(AI_SUBSTATE_APPEAR_BACK);
    selector->AddAppearCom(AI_SUBSTATE_APPEAR_LOWER);
    selector->AddAppearCom(AI_SUBSTATE_APPEAR_UPPER);
    selector->AddAppearCom(AI_SUBSTATE_APPEAR_LEFT);
    selector->AddAppearCom(AI_SUBSTATE_APPEAR_RIGHT);
    selector->AddAppearCom(AI_SUBSTATE_APPEAR_LOOKAROUND);

    selector->AddAppearCom(AI_SUBSTATE_APPEAR_PLAYMOTION,PokeTool::MODEL_ANIME_MOVE_A);
    selector->AddAppearCom(AI_SUBSTATE_APPEAR_PLAYMOTION,PokeTool::MODEL_ANIME_MOVE_B);
    selector->AddAppearCom(AI_SUBSTATE_APPEAR_PLAYMOTION,PokeTool::MODEL_ANIME_MOVE_C);

    selector->AddAppearCom(AI_SUBSTATE_APPEAR_PLAYMOTION,PokeTool::MODEL_ANIME_QUESTION);

    selector->AddAppearCom(AI_SUBSTATE_APPEAR_PLAYMOTION,PokeTool::MODEL_ANIME_PLAY_A);
    selector->AddAppearCom(AI_SUBSTATE_APPEAR_PLAYMOTION,PokeTool::MODEL_ANIME_PLAY_B);

    selector->AddAppearCom(AI_SUBSTATE_APPEAR_PLAYMOTION,PokeTool::MODEL_ANIME_HAPPY_B);
    selector->AddAppearCom(AI_SUBSTATE_APPEAR_PLAYMOTION,PokeTool::MODEL_ANIME_HAPPY_C);

    if(GetAffectionLevel() >= AFFECTION_LEVEL220) selector->AddAppearCom(AI_SUBSTATE_APPEAR_DO_UP);
    
  }

  else if(GetAffectionLevel() >= AFFECTION_LEVEL2)
  {
    selector->AddAppearCom(AI_SUBSTATE_APPEAR_BACK);
    selector->AddAppearCom(AI_SUBSTATE_APPEAR_LOWER);
    selector->AddAppearCom(AI_SUBSTATE_APPEAR_LEFT);
    selector->AddAppearCom(AI_SUBSTATE_APPEAR_RIGHT);
    selector->AddAppearCom(AI_SUBSTATE_APPEAR_LOOKAROUND);

    selector->AddAppearCom(AI_SUBSTATE_APPEAR_PLAYMOTION,PokeTool::MODEL_ANIME_MOVE_A);
    selector->AddAppearCom(AI_SUBSTATE_APPEAR_PLAYMOTION,PokeTool::MODEL_ANIME_MOVE_B);
    selector->AddAppearCom(AI_SUBSTATE_APPEAR_PLAYMOTION,PokeTool::MODEL_ANIME_MOVE_C);

    selector->AddAppearCom(AI_SUBSTATE_APPEAR_PLAYMOTION,PokeTool::MODEL_ANIME_QUESTION);

    selector->AddAppearCom(AI_SUBSTATE_APPEAR_PLAYMOTION,PokeTool::MODEL_ANIME_PLAY_A);

    selector->AddAppearCom(AI_SUBSTATE_APPEAR_PLAYMOTION,PokeTool::MODEL_ANIME_HAPPY_B);
  
  }

  else if(GetAffectionLevel() >= AFFECTION_LEVEL3)
  {
    selector->AddAppearCom(AI_SUBSTATE_APPEAR_LEFT);
    selector->AddAppearCom(AI_SUBSTATE_APPEAR_RIGHT);
    selector->AddAppearCom(AI_SUBSTATE_APPEAR_LOOKAROUND);

    selector->AddAppearCom(AI_SUBSTATE_APPEAR_PLAYMOTION,PokeTool::MODEL_ANIME_MOVE_A);
    selector->AddAppearCom(AI_SUBSTATE_APPEAR_PLAYMOTION,PokeTool::MODEL_ANIME_MOVE_B);
    selector->AddAppearCom(AI_SUBSTATE_APPEAR_PLAYMOTION,PokeTool::MODEL_ANIME_MOVE_C);

    selector->AddAppearCom(AI_SUBSTATE_APPEAR_PLAYMOTION,PokeTool::MODEL_ANIME_QUESTION);

    selector->AddAppearCom(AI_SUBSTATE_APPEAR_PLAYMOTION,PokeTool::MODEL_ANIME_HAPPY_A);    
    selector->AddAppearCom(AI_SUBSTATE_APPEAR_PLAYMOTION,PokeTool::MODEL_ANIME_HAPPY_B);    
  }

  else if(GetAffectionLevel() >= AFFECTION_LEVEL4)
  {
    selector->AddAppearCom(AI_SUBSTATE_APPEAR_LOOKAROUND);
    
    selector->AddAppearCom(AI_SUBSTATE_APPEAR_PLAYMOTION,PokeTool::MODEL_ANIME_MOVE_A);
    selector->AddAppearCom(AI_SUBSTATE_APPEAR_PLAYMOTION,PokeTool::MODEL_ANIME_MOVE_B);
    selector->AddAppearCom(AI_SUBSTATE_APPEAR_PLAYMOTION,PokeTool::MODEL_ANIME_MOVE_C);

    selector->AddAppearCom(AI_SUBSTATE_APPEAR_PLAYMOTION,PokeTool::MODEL_ANIME_QUESTION);

    selector->AddAppearCom(AI_SUBSTATE_APPEAR_PLAYMOTION,PokeTool::MODEL_ANIME_HAPPY_A);    
    selector->AddAppearCom(AI_SUBSTATE_APPEAR_PLAYMOTION,PokeTool::MODEL_ANIME_HAPPY_B);    
  }

  else
  {
    selector->AddAppearCom(AI_SUBSTATE_APPEAR_LOOKAROUND);
    selector->AddAppearCom(AI_SUBSTATE_APPEAR_PLAYMOTION,PokeTool::MODEL_ANIME_QUESTION);
    selector->AddAppearCom(AI_SUBSTATE_APPEAR_PLAYMOTION,PokeTool::MODEL_ANIME_HAPPY_A);        
  }

  int action = 0,motion = 0;

  if(selector->SelectRandomAction(action))
  {      
    if(action==AI_SUBSTATE_APPEAR_PLAYMOTION && selector->GetSelectedMotion(motion)) SetCurrentMotion(motion);
    return(action);      
  }


  // no action could be selected, so go to end of appear:
  return(AI_SUBSTATE_APPEAR_FINISH);
}

void KawaigariPokemon::State_Appear_Sick(void)
{
  // Just play motion over and over...
  switch(mStateRegs[0])
  {
    case 0:
      SetSlowMotion(UseSlowMotion());
      coreSys->GetUIManager()->DisableMainButtons();
      mStateRegs[0] = 1; 
    case 1:
      coreSys->GetRenderSystem()->CalculateCurrentCameraMatrix(true);
      coreSys->GetCamera()->SetLocationDirect(KG_CAMERA_FAR);    

      if(!mStateRegs[0] && !mModel->IsAnimationLastFrame()) break;
      
      if(CheckMotionAvailable(PokeTool::MODEL_ANIME_DROWSE_B))
      {
        SetSlowMotion(UseSlowMotion());
        mModel->UpdateAnimation();
        mStateRegs[0] = 3; // Wait until Animation End
      }
      else // No Drowse Animation, so use IDLE Animation
      {
        SetSlowMotion(UseSlowMotion());
        ChangeFace( TEX_EYE, FACE_EYE_EYECLOSE, true );
      //    ChangeAnimationNotSmooth(PokeTool::MODEL_ANIME_KW_WAIT);
        if( mStateRegs[0] ) mModel->UpdateAnimation();
        mStateRegs[0] = 2;
        mStateRegs[2] = WAIT_SECOND +  (WAIT_SECOND/2); // Update for one second + 1/2
      }      

      break;

    case 2: // Idle Animation, wait one second:
      if(mStateRegs[1]==0)
      {
        mStateRegs[1]=1;             
        mSleepEffect = coreSys->CreateCareEffect(KAWAIGARI_EFFECT_SLEEPY);
      }

      if(mSleepEffect!=-1) coreSys->UpdateSleepEffectPos(mSleepEffect);   

      // Main Sick loop
      mStateRegs[2]--;

      if(mStateRegs[2]<=0)
      {
        mStateRegs[1]++;

        if(mStateRegs[1]<3) 
        {
          PlaySE(KW_ROW);
          mStateRegs[0] = 2; // Loop 3 times!
          mStateRegs[2] = WAIT_SECOND + (WAIT_SECOND/2); // Update for one second + 1/2
        }
        else 
        {
           if(mSleepEffect!=-1) coreSys->GetCareEffect(mSleepEffect)->StopParticle();
           StopSE(KW_ROW);
           SetState(AI_STATE_END);      
        }
      }
      break;

    case 3: // Waiting for Drowse Animation:
      if(mStateRegs[1]==0)
      {
        mStateRegs[1]=1;                
        gfl2::math::Vector3 nodePos;        
        if(mSleepEffect==-1) mSleepEffect = coreSys->CreateCareEffect(KAWAIGARI_EFFECT_SLEEPY);
      }

      if(mSleepEffect!=-1) coreSys->UpdateSleepEffectPos(mSleepEffect);   

      if(mModel->IsAnimationLastFrame())
      {
        mStateRegs[1]++;

        if(mStateRegs[1]<3)
        {
            mStateRegs[0] = 3; // Loop 3 times!
            ChangeAnimation(PokeTool::MODEL_ANIME_DROWSE_B);
            SetSlowMotion(UseSlowMotion());            
        }
        else 
        {
          if(mSleepEffect!=-1) coreSys->GetCareEffect(mSleepEffect)->StopParticle();
          ChangeAnimation(PokeTool::MODEL_ANIME_DROWSE_B,true);
          SetSlowMotion(UseSlowMotion());
          StopSE(KW_ROW);
          SetState(AI_STATE_END);        
        }
      }
      break;

      
  }
}

void KawaigariPokemon::State_Appear_Upper(void)
{
  const int incFrame = 4;

  switch(mStateRegs[0])
  {
  case 0:
    //  p_DisplayUpper = false; ????????
    PlaySE(KW_APPEAR_DO_UP);
    SetPhysics(true);
    coreSys->GetCamera()->SetLocationDirect(KG_CAMERA_PEEP);    
    coreSys->GetCamera()->SetCameraUpVector(0.0f,-1.0f,0.0f);
    UpdateAgain();

    mStateRegs[0]++;
    break;

  case 1:

    // Move Y Position:
    {
      float yPos = GetAppearYPosition();
      if(mStateRegs[1]>90) mStateRegs[1] = 90;
      yPos *= gfl2::math::CosDeg(mStateRegs[1]);      
      mModel->SetPosition(0.0f,yPos,0.0f);
    }

    mStateRegs[1]+=incFrame;


    if(mStateRegs[1]>90 && Sound::IsVoiceFinished(0))
    {
      // Done, now wait 2 seconds:
      CallPlayerName(KW_APPEAR_UPPER);
      mModel->SetPosition(0.0f,0.0f,0.0f);      
      mStateRegs[1] = 0;
      mStateRegs[0]++;
    }
    break;

  case 2:
    
    {
      float cosVal = gfl2::math::CosDeg(mStateRegs[1]);
      float sinVal = gfl2::math::SinDeg(mStateRegs[1]);
      coreSys->GetCamera()->SetCameraUpVector(sinVal,-cosVal,0.0f);
    }

    mStateRegs[1]+=incFrame<<1;

    if(mStateRegs[1]>180)
    {
      coreSys->GetCamera()->SetCameraUpVector(0.0f,1.0f,0.0f);
      coreSys->GetCamera()->SetNewTarget(KG_CAMERA_WHOLE_BODY,0.07f);      
      SetPhysics(false);
      //  p_DisplayUpper = true; ????????
      mStateRegs[1] = WAIT_SECOND;
      mStateRegs[0]++;
    }

    break;  

  case 3:
    mStateRegs[1]--;

    if(mStateRegs[1]<=0)
    {
      SetPhysics(false);
      SetSubState(AI_SUBSTATE_APPEAR_FINISH);
    }

    break;

  }

}

void KawaigariPokemon::State_Appear_Lower(void)
{

  const int incFrame = 4;

  switch(mStateRegs[0])
  {
  case 0:
    PlaySE(KW_APPEAR_DO_UP);
    SetPhysics(true);
    coreSys->GetCamera()->SetLocationDirect(KG_CAMERA_PEEP);    
    UpdateAgain();
    mStateRegs[0]++;
    break;

  case 1:
    
    // Move Y Position:
    {
      float yPos = GetAppearYPosition();
      if(mStateRegs[1]>90) mStateRegs[1]=90;
      yPos *= gfl2::math::CosDeg(mStateRegs[1]);      
      mModel->SetPosition(0.0f,yPos,0.0f);
    }

    mStateRegs[1]+=incFrame;
      
    if(mStateRegs[1]>90 && Sound::IsVoiceFinished(0))
    {
      // Done, now wait 2 seconds:

      CallPlayerName(KW_APPEAR_LOWER);
      mModel->SetPosition(0.0f,0.0f,0.0f);
      mStateRegs[1] = WAIT_SECOND*2;
      
      mStateRegs[0]++;
    }
    break;

  case 2:
    mStateRegs[1]--;

    if(mStateRegs[1]<=0)
    {
      coreSys->GetCamera()->SetNewTarget(KG_CAMERA_WHOLE_BODY,0.03f);      
      mStateRegs[1] = WAIT_SECOND;
      mStateRegs[0]++;
    }

    break;

  case 3:
    mStateRegs[1]--;

    if(mStateRegs[1]<=0)
    {
      SetPhysics(false);
      SetSubState(AI_SUBSTATE_APPEAR_FINISH);
    }
    
    break;

  }

}

void KawaigariPokemon::State_Appear_Behind(void)
{
  // NEVER USED!
}

void KawaigariPokemon::State_Appear_Left(void)
{
  const int incFrame = 4;
  f32 ang = -30.0f;

  switch(mStateRegs[0])
  {
  case 0:
    CallPlayerName(KW_APPEAR_LEFT);    
    coreSys->GetCamera()->SetLocationDirect(KG_CAMERA_LEFT);    
    UpdateAgain();
    mStateRegs[0]++;
    break;

  case 1:
    
    mStateRegs[1]+=incFrame;
    
    {
      f32 angRad = gfl2::math::ConvDegToRad( (ang * gfl2::math::SinDeg(mStateRegs[1]) ) ); 
      mModel->SetRotation(0.0f,0.0f, angRad ); 
    }

    if(mStateRegs[1]>90)
    {
      mStateRegs[1] = WAIT_SECOND;
      mStateRegs[0]++;
    }

    break;

  case 2: // Waiting one second:
    mStateRegs[1]--;

    if(mStateRegs[1]<=0)
    {
      mStateRegs[1] = 0;
      mStateRegs[0]++;
    }
    break;

  case 3: // Rotate Back:

    mStateRegs[1]+=incFrame;

    {
      f32 angRad = gfl2::math::ConvDegToRad( (ang - ang * gfl2::math::SinDeg(mStateRegs[1]) ) ); 
      mModel->SetRotation(0.0f,0.0f, angRad ); 
    }
    
    if(mStateRegs[1]>90)
    {
      coreSys->GetCamera()->SetNewTarget(KG_CAMERA_WHOLE_BODY);    

      mStateRegs[1] = WAIT_SECOND;
      mStateRegs[0]++;
    }

    break;

  case 4: // Finish
    mStateRegs[1]--;

    if(mStateRegs[1]<=0)
    {
      SetSubState(AI_SUBSTATE_APPEAR_FINISH);
    }
    break;
  }

}

void KawaigariPokemon::State_Appear_Right(void)
{
  const int incFrame = 4;
  f32 ang = 30.0f;

  switch(mStateRegs[0])
  {
  case 0:
    CallPlayerName(KW_APPEAR_RIGHT);    
    coreSys->GetCamera()->SetLocationDirect(KG_CAMERA_RIGHT);    
    UpdateAgain();
    mStateRegs[0]++;
    break;

  case 1:

    mStateRegs[1]+=incFrame;

    {
      f32 angRad = gfl2::math::ConvDegToRad( (ang * gfl2::math::SinDeg(mStateRegs[1]) ) ); 
      mModel->SetRotation(0.0f,0.0f, angRad ); 
    }

    if(mStateRegs[1]>90)
    {
      mStateRegs[1] = WAIT_SECOND;
      mStateRegs[0]++;
    }

    break;

  case 2: // Waiting one second:
    mStateRegs[1]--;

    if(mStateRegs[1]<=0)
    {
      mStateRegs[1] = 0;
      mStateRegs[0]++;
    }
    break;

  case 3: // Rotate Back:

    mStateRegs[1]+=incFrame;

    {
      f32 angRad = gfl2::math::ConvDegToRad( (ang - ang * gfl2::math::SinDeg(mStateRegs[1]) ) ); 
      mModel->SetRotation(0.0f,0.0f, angRad ); 
    }

    if(mStateRegs[1]>90)
    {
      coreSys->GetCamera()->SetNewTarget(KG_CAMERA_WHOLE_BODY);    

      mStateRegs[1] = WAIT_SECOND;
      mStateRegs[0]++;
    }

    break;

  case 4: // Finish
    mStateRegs[1]--;

    if(mStateRegs[1]<=0)
    {
      SetSubState(AI_SUBSTATE_APPEAR_FINISH);
    }
    break;
  }

}

void KawaigariPokemon::State_Appear_Do_Up(void)
{

  switch(mStateRegs[0])
  {
    case 0:
      // PlaySE(KW_APPEAR_DO_UP);
      coreSys->GetCamera()->SetLocationDirect(KG_CAMERA_DO_UP);
      mStateRegs[1] = WAIT_SECOND * 2;
      mStateRegs[0]++;
      break;

    case 1:
      mStateRegs[1]--;

      if(mStateRegs[1]<=0)
      {
        coreSys->GetCamera()->SetNewTarget(KG_CAMERA_WHOLE_BODY,0.01f);
        mStateRegs[1] = WAIT_SECOND * 2;
        mStateRegs[0]++;
      }
      break;

    case 2:
      mStateRegs[1]--;

      if(mStateRegs[1]<=0)
      {
        SetSubState(AI_SUBSTATE_APPEAR_FINISH);
      }
      break;

  }

}

void KawaigariPokemon::State_Appear_Back(void)
{
  const int frameInc = 2;

  switch(mStateRegs[0])
  {
    case 0:
      {      
        coreSys->GetCamera()->SetLocationDirect(KG_CAMERA_FAR);
        f32 angRad = gfl2::math::ConvDegToRad( 180.0f ); 
        mModel->SetRotation(0.0f,angRad, 0.0f ); 
        mStateRegs[0]++;
      }
      break;

    case 1: // Start of Loop
      if(IsTouch())
      {
        mStateRegs[0]++;
      }
      break;

    case 2:
      if(IsTouching())
      {
        SetTouchIcon(ICON_ID_STOP_BECKON);
      }
      else
      {
        SetTouchIcon(ICON_ID_FADE_BECKON_30F);
        mStateRegs[2] = 30; // Loop 30 Times
        mStateRegs[0]++;
      }
      break;

    case 3:
      if(IsTouch()) 
      {
        SetTouchIcon( ICON_ID_ANIME_BECKON );
        PlaySE(KW_BECKON);
        mStateRegs[0] = 100; // Touch Program        
        UpdateAgain();
      }

      mStateRegs[2]-=frameInc;

      if(mStateRegs[2]<=0)
      {
        mStateRegs[0] = 1; // Go back to start of loop
      }
      break;

    case 100:
      if(ChangeAnimationNotSmooth(PokeTool::MODEL_ANIME_RESPOND))
      {
        f32 angRad = gfl2::math::ConvDegToRad( 180.0f ); 
        mModel->SetRotation(0.0f,angRad, 0.0f ); 
        PlaySE(KW_APPEAR_TURN);        
        mStateRegs[0]++;
      }
      else
      {
        mStateRegs[0]++;
        UpdateAgain();
      }
      break;

    case 101:
      SetTouchIcon( ICON_ID_ANIME_BECKON, false );
      {
      
        int motion = 0;
        if(GetAffectionLevel()>100 && CanSetMotion(PokeTool::MODEL_ANIME_TURN_B) ) motion = PokeTool::MODEL_ANIME_TURN_B;
        else
        {
            if(CanSetMotion(PokeTool::MODEL_ANIME_TURN_A)) motion = PokeTool::MODEL_ANIME_TURN_A;
            ///SetParticle(GARC_love_eff_exclamation_r, 0, 0, true, MODEL_NODE_OVERHEAD );
            coreSys->CreateEffect(KAWAIGARI_EFFECT_EXCLAMATIONMARK);          
        }

        if(motion)
        {
          if(ChangeAnimationNotSmooth(motion))
          {
            f32 angRad = gfl2::math::ConvDegToRad( 180.0f ); 
            mModel->SetRotation(0.0f,angRad, 0.0f ); 
            CallPlayerName(0);
            mStateRegs[0] = 103; // Wait until end of motion          
          }
          else mStateRegs[0] = 104; // End of wait
        }

        else
        {
          mStateRegs[1] = WAIT_SECOND * 2;
          mStateRegs[0] = 102; // Wait 2 seconds
        }

      }
      
      break;

      case 102: // Wait 2 seconds...
        mStateRegs[1]--;

        if(mStateRegs[1]<=0)
        {
          mStateRegs[0]=104; // End of wait
        }      
        break;

      case 103: // Wait until end of motion:
        if(mModel->IsAnimationLastFrame())
        {
          mStateRegs[0] = 104; // End of wait
        }
        break;

      case 104: // End of wait
        // Fade to white:
        {        
          gfl2::math::Vector4 whiteCol(255.0f,255.0f,255.0f,0.0f);					
          gfl2::math::Vector4 whiteCol2(255.0f,255.0f,255.0f,255.0f);					
          gfl2::math::Vector4 *col = &whiteCol;
          gfl2::math::Vector4 *col2 = &whiteCol2;				
          GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA, col, col2,20);			
          GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_ALPHA, col, col2,20);			
        }
        
        mStateRegs[1] = 25;
        mStateRegs[0]++;
        break;

      case 105:
        mStateRegs[1]--;

        if(mStateRegs[1]<=0)
        {
          ChangeAnimationNotSmooth(PokeTool::MODEL_ANIME_KW_WAIT);
          mModel->SetRotation(0.0f,0.0f, 0.0f ); 
          coreSys->GetCamera()->SetLocationDirect(KG_CAMERA_WHOLE_BODY);                   
          mStateRegs[0]++;
        }

        break;
      
      case 106:
        GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA,20); 
        GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_ALPHA,20);


        SetSubState(AI_SUBSTATE_APPEAR_FINISH); // Exit
        break;

      
  }
}

void KawaigariPokemon::State_Appear_Drowsey(void)
{
   const int frameInc = 2;

  switch(mStateRegs[0])
  {
    case 0:
      // Turn OFF Face/Mouth Animation Smoothing:
      mModel->SetFaceAnimeSmoothEnable(false);

      coreSys->GetCamera()->SetLocationDirect(KG_CAMERA_FAR);
      ChangeAnimation(PokeTool::MODEL_ANIME_DROWSE_B,true);

      mStateRegs[1] = 10; // Loop 10 Times
      mStateRegs[0]++;
      break;

    case 1:
      if(mStateRegs[5]==0)
      {
        mStateRegs[5]=1;
        gfl2::math::Vector3 nodePos;
        mModel->GetEffectNodePosition(PokeTool::MODEL_NODE_OVERHEAD,0,&nodePos);
        mSleepEffect = coreSys->CreateCareEffect(KAWAIGARI_EFFECT_SLEEPY,&nodePos);
      }

      if(mSleepEffect!=-1) coreSys->UpdateSleepEffectPos(mSleepEffect);    

      mStateRegs[1]--;
      if(mStateRegs[1]<=0)
      {
        mStateRegs[0]++;
      }
      break;

    case 2:
      if(!CanSetMotion(PokeTool::MODEL_ANIME_DROWSE_B))
      {
        mStateRegs[0] = 100; // No Drowse Motion
        UpdateAgain();
      }
      else
      {
        mStateRegs[0] = 200; // Drowse Motion available
        UpdateAgain();
      }
      break;


    case 100: // while(!exitCondition) Drowse Motion Not available:
      
      if( (mStateRegs[1] & 7) == 0) PlaySE(KW_ROW); // Play Only Sound
      
      mStateRegs[2] = 30; // Loop 30 times
      mStateRegs[0]++;
      UpdateAgain();
      break;

    case 101:
      if(IsTouch())
      {
        // Turn ON Face/Mouth Animation Smoothing:
        mModel->SetFaceAnimeSmoothEnable(true);

        SetTouchIcon( ICON_ID_ANIME_BECKON );
        PlaySE(KW_BECKON);
        ChangeAnimation(PokeTool::MODEL_ANIME_DROWSE_C);
        mStateRegs[3] = WAIT_SECOND;
        mStateRegs[0] = 105; // ExitCondition = true, so execute exit program
        coreSys->GetCareEffect(mSleepEffect)->StopParticle();
      }
      else
      {
        mStateRegs[2] -= frameInc;
        if(mStateRegs[2]<=0) mStateRegs[0]++;
      }
      break;

    case 102:           
      mStateRegs[1]++;
      mStateRegs[0] = 100; // Loop back to while(!exitCondition)
      break;

    case 105: // Wait Second Loop and Exit Prog
      mStateRegs[3]--;

      if(mStateRegs[3]<=0)
      {        
        SetTouchIcon( ICON_ID_ANIME_BECKON, false );
        mStateRegs[0]++;
      }
      break;
    case 106:
      ChangeFace( TEX_EYE, FACE_EYE_DEFAULT );
      mStateRegs[0] = 999; // Exit Sub State
      break;



    case 200: // Drowse motion available
      coreSys->GetCamera()->SetLocationDirect(KG_CAMERA_FAR);
      ChangeAnimation(PokeTool::MODEL_ANIME_DROWSE_B);      
      mStateRegs[0]++;
      break;

    case 201:

      if(IsTouch()) // Exit on Touch
      {
        // Turn ON Face/Mouth Animation Smoothing:
        mModel->SetFaceAnimeSmoothEnable(true);

        coreSys->GetCareEffect(mSleepEffect)->StopParticle();
        mStateRegs[0]++;        
        UpdateAgain();   
      }

      else if(mModel->IsAnimationLastFrame()) ChangeAnimation(PokeTool::MODEL_ANIME_DROWSE_B);
      break;

    case 202:
      SetTouchIcon( ICON_ID_ANIME_BECKON );
      PlaySE(KW_BECKON);
      ChangeAnimation( PokeTool::MODEL_ANIME_DROWSE_C );
      mStateRegs[0]++;
      break;

    case 203: // Wait until end of animation
      if(mModel->IsAnimationLastFrame())
      {
        SetTouchIcon( ICON_ID_ANIME_BECKON, false );
        ChangeFace( TEX_EYE, FACE_EYE_DEFAULT );
        mStateRegs[0] = 999; // Finish state
        UpdateAgain();
      }
      break;

    case 999: // Exit to APPEAR_FINISH:
      coreSys->KillCareEffect(mSleepEffect); mSleepEffect = -1;
      SetSubState(AI_SUBSTATE_APPEAR_FINISH);
      break;

  }
}

void KawaigariPokemon::State_Appear_Sleep(void)
{
  const int incFrame = 2;

  switch(mStateRegs[0])
  {
    case 0:
      // Turn OFF Face/Mouth Animation Smoothing:
      mModel->SetFaceAnimeSmoothEnable(false);

      ChangeAnimationNotSmooth(PokeTool::MODEL_ANIME_SLEEP_A);
      coreSys->GetCamera()->SetLocationDirect(KG_CAMERA_FAR);
      mStateRegs[0]++;
      break;

    case 1:
      if(mStateRegs[5]==0)
      {
        mStateRegs[5]=1;
        mSleepEffect = coreSys->CreateCareEffect(KAWAIGARI_EFFECT_SLEEPY);
      }

      if(mSleepEffect!=-1) coreSys->UpdateSleepEffectPos(mSleepEffect);
     
      if(IsTouch())
      {
        mStateRegs[0]++;
        UpdateAgain();
        break;
      }

      else if(mStateRegs[1]<=0)
      {
        mStateRegs[1] = 90; // Reset for 90 frames...
        if(IsCharacterID(MONSNO_PIKATYUU)) // Pikachu!
        {
          PlaySpecialVoice(SPECIAL_VOICE_DEEPSLEEPING);
        }
      }

      mStateRegs[1]--;

      break;

    case 2:
      if(!IsTouching())
      {
        SetTouchIcon( ICON_ID_FADE_BECKON_30F );
        UpdateAgain();
        mStateRegs[2] = 30; // Loop 30 frames
        mStateRegs[0]++;        
      }

      else // Touching()
      {
        SetTouchIcon( ICON_ID_STOP_BECKON );
      }

      break;

    case 3: // Looping 30 Frames here

      if(IsTouch())
      {
        coreSys->GetCareEffect(mSleepEffect)->StopParticle();
        SetTouchIcon( ICON_ID_ANIME_BECKON );
        PlaySE(KW_BECKON);
        mStateRegs[1]=WAIT_SECOND;
        mStateRegs[0]=100; // Touch Exit Program
        break;
      }

      mStateRegs[1]--; // Sleep Voice Counter Dec
      mStateRegs[2]-=incFrame;

      if(mStateRegs[2]<=0)
      {
        mStateRegs[0]=1; // Exit
        break;
      }
      break;

    case 100: // Touch Exit Program
      
      // Turn ON Face/Mouth Animation Smoothing:
      mModel->SetFaceAnimeSmoothEnable(true);

   //   mStateRegs[1]--; // Waiting 1 second

  //    if(mStateRegs[1]<=0)
      {
        
        SetTouchIcon( ICON_ID_ANIME_BECKON, false );
        PlaySE(KW_BECKON);
        
        // Change animation if available, and then wait for end of animation:
        if(ChangeAnimation( PokeTool::MODEL_ANIME_SLEEP_B ))
        {
           if(IsCharacterID(MONSNO_PIKATYUU))
          {           
            PlaySpecialVoice( SPECIAL_VOICE_DEEPWAKEUP ); // Pikachu
          }
          mStateRegs[0]++;
          break;
        }

        else mStateRegs[0]=102;
      }

      break;

    case 101: // Wait until End of Animation
      if(mModel->IsAnimationLastFrame())
      {
        mStateRegs[0]++;
      }
      break;

    case 102: // Fade out:
      // Fade to white:
      {        
        gfl2::math::Vector4 whiteCol(255.0f,255.0f,255.0f,0.0f);					
        gfl2::math::Vector4 whiteCol2(255.0f,255.0f,255.0f,255.0f);					
        gfl2::math::Vector4 *col = &whiteCol;
        gfl2::math::Vector4 *col2 = &whiteCol2;				
        GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA, col, col2,20);			
        GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_ALPHA, col, col2,20);			
      }

      mStateRegs[1] = 25;
      mStateRegs[0]++;

      break;

    case 103: // Wait for fade to white
      mStateRegs[1]--;

      if(mStateRegs[1]<=0)
      {
        coreSys->KillCareEffect(mSleepEffect); mSleepEffect = -1;
        GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA,20); 
        GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_ALPHA,20);
        mStateRegs[0]=999;
        UpdateAgain();
      }
      
      break;

    case 999: // Exit to APPEAR_FINISH:
      SetSubState(AI_SUBSTATE_APPEAR_FINISH);            
      UpdateAgain();
      break;
  }

}

void KawaigariPokemon::State_Appear_LookAround(void)
{
  switch(mStateRegs[0])
  {
    case 0:
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
      break;

    case 999: // Exit to APPEAR_FINISH:
      SetInterestPosition(NULL);
      SetSubState(AI_SUBSTATE_APPEAR_FINISH);
      break;

  }
}

void KawaigariPokemon::State_Appear_PlayMotion(void)
{
  switch(mStateRegs[0])
  {
    case 0: 
      ChangeToCurrentMotion();        
      coreSys->GetCamera()->SetLocationDirect(KG_CAMERA_FAR);
      mStateRegs[0]++;
      break;

    case 1:
      if(mModel->IsAnimationLastFrame())
      {
        ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT);                
        SetSubState(AI_SUBSTATE_APPEAR_FINISH);               
      }
      break;

  }
}

void KawaigariPokemon::State_Appear_Finish(void)
{
  switch(mStateRegs[0])
  {
    case 0:
      ChangeAnimationNotSmooth(PokeTool::MODEL_ANIME_KW_WAIT);
      mStateRegs[0]++;
      UpdateAgain();      
      break;

    case 1: 
      
      if(!Sound::IsVoiceFinished(0)) break;
      
      if(CanCallPlayerName())
      {
        CallPlayerName(KW_VOICE);

        if(ChangeAnimationNotSmooth(PokeTool::MODEL_ANIME_HAPPY_A,true))
        {                 
          mStateRegs[0]++;
          break;
        }
      }
        
      coreSys->GetUIManager()->ChangeToMainMenu();
      SetState(AI_STATE_IDLE);
      
      break;

      // Wait for Happy Animation to end:
   case 2:
     AddPlayfulnessAmount( PLAYABLE_IDLE_FIRST,false );          
     mStateRegs[0]++;
     break;

   case 3:

      if(mModel->IsAnimationLastFrame())
      {        
        coreSys->GetUIManager()->ChangeToMainMenu();

        if(mForceAppearState==-1)
          SetState(AI_STATE_IDLE);       
        else
        {
          mForceAppearState = -1;
          coreSys->GetPokemonAI()->SetState(AI_STATE_CAMERAWAIT);
        }
     }
     break;

  }
  
}

  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)