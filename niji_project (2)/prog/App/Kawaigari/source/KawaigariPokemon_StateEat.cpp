//======================================================================
/**
 * @file	KawaigariPokemon_StateEat.C++
 * @brief	Kawaigari Pokemon Actor - Eat State Code
 * @author	PETE
 * @data	2015.12.07
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

#include "niji_conv_header/app/kawaigari/res2d/kw_layout_anim_list.h"

#include "Sound/include/Sound.h"

// Diary:
#include "GameSys/include/GameData.h"
#include "Savedata/include/MyItemSaveDef.h"

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

enum {
    FALL_FOOD_NONE,
    FALL_FOOD_CATCHED,
    FALL_FOOD_REVERTED,
};

void KawaigariPokemon::State_Eat(void)
{

  if(mStateRegSub!=AI_SUBSTATE_EAT_FALL && mCanCatchFood)///&& IsTouching())
  {    
    gfl2::math::Vector3 pos;
    coreSys->Get3DTouchPositionByXY( &pos, touchX, touchY, TPT_MAME );
    coreSys->GetFoodPosition()->x = pos.x;
    coreSys->GetFoodPosition()->y = pos.y;
    coreSys->GetFoodPosition()->z = pos.z;        
  }

  

  switch(mStateRegSub)
  {
    case AI_SUBSTATE_EAT_INIT: State_Eat_Init(); break;
    case AI_SUBSTATE_EAT_BITE: State_Eat_Bite(); break;
    case AI_SUBSTATE_EAT_THINK: State_Eat_Think(); break;
    case AI_SUBSTATE_EAT_TRY: State_Eat_Try(); break;
    case AI_SUBSTATE_EAT_FULL: State_Eat_Full(); break;
    case AI_SUBSTATE_EAT_HATE: State_Eat_Hate(); break;
    case AI_SUBSTATE_EAT_FALL: State_Eat_Fall(); break;
    case AI_SUBSTATE_EAT_BEGIN: State_Eat_Begin(); break;
    case AI_SUBSTATE_EAT_IDLE: State_Eat_Idle(); break;  
    case AI_SUBSTATE_EAT_WAIT: State_Eat_Wait(); break; 
  }
 
}

void KawaigariPokemon::State_Eat_Init(void)
{
  mForceEatFinish = false;
  mCanCatchFood = true;
  SetTouchIcon(-1);
  SetSubState(AI_SUBSTATE_EAT_WAIT);
  //coreSys->GetCamera()->SetNewTarget(KG_CAMERA_WHOLE_BODY);
  coreSys->GetCamera()->SetNewTarget(KG_CAMERA_EAT);

  mFallMode = false;
  mFell = false;
  mDontTouchMe = false;
  mBiteCount = 0;

  coreSys->GetRenderSystem()->GetRenderingPipeLine()->SetUseTouchIDTexture(false);
}

void KawaigariPokemon::State_Eat_Wait(void)
{
  switch(mStateRegs[0])
  {
    case 0: // Make sure in IDLE animation first:      
      coreSys->GetCamera()->ResetIdleCamera();
      coreSys->GetCamera()->SetNewTarget(KG_CAMERA_EAT);

      if(mModel->IsAnimationLastFrame() && !mModel->GetAnimationId()!=PokeTool::MODEL_ANIME_KW_WAIT)
      {
        ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT);           
        break;
      }
      break;

      
   //   SetSubState(AI_SUBSTATE_EAT_BEGIN);
//      UpdateAgain();
      
  }

  UpdateBlink();  
}

void KawaigariPokemon::State_Eat_Bite(void)
{
      
  switch(mStateRegs[0])
  {
    case 0:           

//      mModel->ResetFaceAnime_Mouth();

  /*    mStateRegs[1]++;

      if(mStateRegs[1]==3)
      {
        mStateRegs[1]=0;      */
        Eat_BiteProgram_Init(PokeTool::MODEL_ANIME_EAT_A);
        mStateRegs[0]++;
    //  }
      break;  
      
    case 1:
      if(Eat_BiteProgram_Update(PokeTool::MODEL_ANIME_EAT_A))
      {
        // Fall?
        if(CheckEating())
        {          
          GFL_PRINT("*Check Eating Failed!*\n");
          break; // Jumps to fall!
        }
        else if(!CheckFoodRange())
        {
          RestoreState();
          GFL_PRINT("*FOOD RESTORE Eating Failed!*\n");
          break;
        }
        else mStateRegs[0]++;
        UpdateAgain();
      }
      break;
      
    case 2: // Eat Loop:
      Eat_BiteProgram_Init(PokeTool::MODEL_ANIME_EAT_B);

      mStateRegs[0]++;

      // Calc Next Piece Resource and start pre-load if needed
      {        
        int show_id = mBiteCount+1;       
        if(show_id < FoodAnimMax() )
        {
          if(FoodAnimMax() <= 3)
          {
            if(show_id == 1)
            {
              show_id = 2;
            }
            else if( show_id == 2 )
            {
              show_id = 3;              
            }
          }
          coreSys->LoadFoodResource(show_id);
        }
      }

      UpdateAgain();
      break;

    case 3:
      if(Eat_BiteProgram_Update(PokeTool::MODEL_ANIME_EAT_B))
      {
        if(mForceEatFinish)
        {
          UpdateAgain();
          mStateRegs[0]++;
        }
        // Fall?
        else if(CheckEating())
        {
          GFL_PRINT("*Check Eating Failed!*\n");
          ChangeFace( TEX_EYE, FACE_EYE_DEFAULT );
          ChangeFace(TEX_MOUTH, FACE_MOUTH_LOOK,true);
          break; // Jumps to fall!
        }
        else if(!CheckFoodRange())
        {
          RestoreState();
          GFL_PRINT("*FOOD RESTORE Eating Failed!*\n");
          break;
        }
        else 
        {         
          UpdateAgain();
          mStateRegs[0]++;
        }
      }
      break;
    
    case 4:
      if(coreSys->WaitFoodResource())  mStateRegs[0]++;
      else 
      {
        if( BRANCH_FOOD_FULLNESS_SLOW <= GetFullnessLevel()  && 2<=FoodAnimMax())
        {                    
          // Go to C Motion
        }
        else if(FoodAnimMax() <= mBiteCount+1) {} // Finished
        else
        {                    
          // Going to Loop so set to frame 0
          mModel->SetAnimationFrame(0.0f);          
        }
        break;
      }
      // BREAK-THOUGH
      
    case 5:      
      {
        int erase_id = mBiteCount++;
        int show_id = mBiteCount;        

        f32 full = 102.0f; // SANGO=50.0f
        f32 each = full / FoodAnimMax();

        if(FoodAnimMax() <= show_id)
        {
          ///AddValue( p_Fullness, full - each * (FOOD_ANIM_MAX-1) );
          AddFullnessLevel(full - each * (FoodAnimMax()-1));
          mFallMode  = false;
          mStateRegs[0] = 99;
          ChangeAnimation(PokeTool::MODEL_ANIME_EAT_C);
          break;
        }
        else
        {
          AddFullnessLevel( each);
        }

        if(FoodAnimMax() <= 3)
        {
          if(show_id == 1)
          {
            show_id = 2;
          }
          else if( show_id == 2 )
          {
            show_id = 3;
            erase_id = 2;
          }
        }
        

        //ShowObject( OBJ_FOOD,erase_id,OFF );
        //ShowObject( OBJ_FOOD,show_id,ON );
        coreSys->ShowFoodObject(erase_id,false);

        coreSys->LoadFoodObject(show_id);
        
        coreSys->ShowFoodObject(show_id,true);

        // Set Food Pos here to LOOK:
        {
          gfl2::math::Vector3 pos;
          coreSys->Get3DTouchPositionByXY( &pos, touchX, touchY, TPT_MAMESHAKE );
          coreSys->GetFoodPosition()->x = pos.x;
          coreSys->GetFoodPosition()->y = pos.y;
          coreSys->GetFoodPosition()->z = pos.z;    
        }

        if( BRANCH_FOOD_FULLNESS_SLOW <= GetFullnessLevel()  && 2<=FoodAnimMax())
        {          
          mStateRegs[0]=10; // Mid Program
          ChangeAnimation(PokeTool::MODEL_ANIME_EAT_C);
        }
        else
        {                    
          mStateRegs[0] = 2; // LOOP
          // Going to Loop so set to frame 0
          mModel->SetAnimationFrame(0.0f);          
        }

        

      }
      break;
      
    case 10:

      Eat_BiteProgram_Init(PokeTool::MODEL_ANIME_EAT_C);

      mStateRegs[0]++;
      break;
      
    case 11:
      if(Eat_BiteProgram_Update(PokeTool::MODEL_ANIME_EAT_C))
      {
        // Fall?
        if(CheckEating())
        {          
          break; // Jumps to fall!
        }
        else
        {
          // POKEMON(724) Has EXCEPTION! (NO ANIMATION SMOOTH)
          if(!IsCharacterID(MONSNO_HUKUROU3) ) ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT);
          else  ChangeAnimationNotSmooth(PokeTool::MODEL_ANIME_KW_WAIT);
          mStateRegs[0]++;
          break;
        }
      }
      break;
      
    case 12:      
      if( !CheckFoodRange() )
      { 
        // 食べるのをやめる！
        //SetLoveCommand( COM_FOOD_FALL );
        Eat_FallProgram_Init();
        SetSubState(AI_SUBSTATE_EAT_FALL);
        mFallMode = true;
        break;
      }
      mStateRegs[0] = 0; // LOOP!
      
      break;
      
    case 99:
      // Turn off All Objects:
      ///ShowObject( OBJ_FOOD,-1,OFF );
      coreSys->ShowFoodObject(-1,false);

      Savedata::IncRecord( Savedata::Record::RECID_MAME_PRESENT_CNT );

      
      Eat_BiteProgram_Init(PokeTool::MODEL_ANIME_EAT_C);
      mStateRegs[0]++;
      break;

    case 100:
      if(Eat_BiteProgram_Update(PokeTool::MODEL_ANIME_EAT_C))
      {
        UpdateAgain();
        mStateRegs[0]++;
      }
      break;

    case 101:
      coreSys->GetCamera()->SetNewTarget(KG_CAMERA_WHOLE_BODY,0.05f);
      LookEnd();
      ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT);

      mStateRegs[1]=5;
      
      coreSys->GetUIManager()->DisableEatModeButton();
           
      mStateRegs[0]++;
      break;

    case 102:
      mStateRegs[1]--;
      if(mStateRegs[1]<=0)
      {
        int mameType = coreSys->GetMameType();
        int loveAmount = coreSys->GetDataManager()->GetSnackAffection(mameType);
        AddAffectionAmount(loveAmount,false); // No CAP!

        // Set Happy Motion:
        {      
          bool mot_exist = false;
          if( GetFullnessLevel() <= 102 ) {
            mot_exist = ChangeAnimation( PokeTool::MODEL_ANIME_HAPPY_C );
          }else if( GetFullnessLevel() >= 230 ) {
            mot_exist = ChangeAnimation( PokeTool::MODEL_ANIME_HAPPY_A );
          }
          if( mot_exist == false ) {
            ChangeAnimation( PokeTool::MODEL_ANIME_HAPPY_B );
          }

          ///AddLoveAmount( _GetCakePara( PRM_FRIEND ) ); <---- FROM DATA SNACK AFFECTION (OK)
          mStateRegs[1]=2;

        }
        mStateRegs[0]++;
      }
      break;

    case 103:
      if(mModel->IsAnimationLastFrame())
      {        
        ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT);
        UpdateAgain();
        mStateRegs[0]++;
      }
      break;

    case 104:

      // JUST LOVE EFFECT:
      mStateRegs[2] = GetJustLoveAmountIndex();
      if(mStateRegs[2]==-1)
      {
        mStateRegs[0] = 110;
        UpdateAgain();
        break;
      }

      mStateRegs[1] = GetJustLoveMotionPlayCount( mStateRegs[2]); // How many times to loop
      mStateRegs[3] = 1; // Play big sound ON

      mStateRegs[0]++;
      UpdateAgain();
      break;

    case 105:
      SetJustLoveMotion(mStateRegs[2]);

      // Play Sound
      {
        int loveAmountValue[ 4 ] = { 50, 100, 150, 255 };

        if(loveAmountValue[mStateRegs[2]]==255 && mStateRegs[3]==1)
        {
          mStateRegs[3] = 0; // BIG Sound OFF
          PlaySE(SEQ_SE_KW_HEART3);
        }
        else
        {
          PlaySE(SEQ_SE_KW_HEART2);
        }

        // Effects:
        coreSys->CreateEffect(KAWAIGARI_EFFECT_AFFECTIONSUCCESS);
        coreSys->CreateEffect(KAWAIGARI_EFFECT_HEART_TOUCH,NULL,0,NULL,false);
        coreSys->CreateEffect(KAWAIGARI_EFFECT_HEART_TOUCH,NULL,0,NULL,false);
        coreSys->CreateEffect(KAWAIGARI_EFFECT_HEART_TOUCH,NULL,0,NULL,false);

      }
      mStateRegs[0]++;
      break;    

    case 106:
      if(mModel->IsAnimationLastFrame())
      {
        mStateRegs[1]--;

        if(mStateRegs[1]==0) mStateRegs[0]=110; // Done!
        else mStateRegs[0] = 105; // Loop
      }
      break;

    case 110:
      ///_SetPokeMemory();
      coreSys->GetUIManager()->ChangeToMainMenu();      
            
      // Go back to IDLE:
      mStateRegs[0]++;
      ///break; // BREAK-THROUGH

    case 111:
      if(coreSys->UnloadFoodObjects())
        SetState(AI_STATE_IDLE);
      break;
  }
}

void KawaigariPokemon::State_Eat_Think(void)
{

  switch(mStateRegs[0])
  {
    case 0:
      // Force Idle Animation:
      if(mModel->IsAnimationLastFrame())
      {
        ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT);
      }

      if(IsTouching())
      {
        if( IsTouchSlide( 30 ) )
        { 
          mCanCatchFood = false;
          // Move too fast:
          Eat_FallProgram_Init();
          SetSubState(AI_SUBSTATE_EAT_FALL);
          UpdateAgain();
          break;
        }
        if( CheckFoodRange() )
        {
          SetSubState(AI_SUBSTATE_EAT_BITE);
          UpdateAgain();
          break;
        }
      }
      else
      { 
        // No Touching Anymore:
        Eat_FallProgram_Init();
        SetSubState(AI_SUBSTATE_EAT_FALL); // Food Fall Mode        
        break;
      }
      break;  
  }
}

void KawaigariPokemon::State_Eat_Try(void)
{
  int LOOK_FRAME =  WAIT_SECOND + coreSys->GetRandom(4 * WAIT_SECOND );

  if( !mBiteCount && touchX < 50 && touchY < 50 )
  {
    coreSys->GetUIManager()->SetMameButtonLarge(true);		
  }
  else 
  {
    coreSys->GetUIManager()->SetMameButtonLarge(false);
  }

  if(!IsTouching())
  {        
    coreSys->GetUIManager()->SetMameButtonLarge(false);        
  }

  switch(mStateRegs[0])
  {
    case 0: // for(;;)     
      ChangeFace(TEX_MOUTH, FACE_MOUTH_DEFAULT,true);
      ChangeAnimation(PokeTool::MODEL_ANIME_TOUCH);
      mStateRegs[0]++;
      break;  

    case 1: // Loop Until End of Motion:
      if(mModel->IsAnimationLastFrame())
      {
        ChangeAnimationNotSmooth(PokeTool::MODEL_ANIME_KW_WAIT);
        ChangeFace(TEX_MOUTH, FACE_MOUTH_LOOK,true);
        mStateRegs[0]= 10;
        mStateRegs[1] = LOOK_FRAME;
        break;
      }

      if(!IsTouching())
      {
        mStateRegs[0]=5; // Wait for Motion End and Fall
        Eat_FallProgram_Init();
        SetSubState(AI_SUBSTATE_EAT_FALL);
        break;
      }
      else if(CheckFoodRange())
      {
        SetSubState(AI_SUBSTATE_EAT_THINK);
        break;
      }
      break;


    case 5: // Wait for Motion End (TOUCH) and Fall:
      if(mModel->IsAnimationLastFrame())
      {
        ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT);
      }
      break;

    case 10: // 2nd Part:
      if( !IsTouching() )
      { 
        // Food Fall:
        Eat_FallProgram_Init();
        SetSubState(AI_SUBSTATE_EAT_FALL);
        break;
      }

      else if(CheckFoodRange())
      {
        SetSubState(AI_SUBSTATE_EAT_THINK);
        break;
      }

      // Loop Counter:
      mStateRegs[1]--;
      if(mStateRegs[1]<=0)
      {
        mStateRegs[0] = 0; // back to for(;;)
      }
      break;
  }
}

void KawaigariPokemon::State_Eat_Hate(void)
{

  switch(mStateRegs[0])
  {
    case 0:
      ChangeFace( TEX_EYE, FACE_EYE_DEFAULT, true );
      ChangeFace( TEX_MOUTH, FACE_MOUTH_DEFAULT, true );
      mStateRegs[0]++;

      // SetLoveCommand(COM_HATE); <---- Just looks at food every frame...     

      if( !mBiteCount && touchX < 50 && touchY < 50 )
      {
        coreSys->GetUIManager()->SetMameButtonLarge(true);		
      }
      else 
      {
        coreSys->GetUIManager()->SetMameButtonLarge(false);
      }

      if(!IsTouching())
      {        
        coreSys->GetUIManager()->SetMameButtonLarge(false);        
      }

      UpdateAgain();
      break;  

    case 1:

      ///LookAtFood();
      LookAwayFromFood();

      if( !mBiteCount && touchX < 50 && touchY < 50 )
      {
        coreSys->GetUIManager()->SetMameButtonLarge(true);		
      }
      else 
      {
        coreSys->GetUIManager()->SetMameButtonLarge(false);
      }

      if(!IsTouching())
      {        
        LookEnd();
        coreSys->GetUIManager()->SetMameButtonLarge(false);
        mStateRegs[0]=99;
        UpdateAgain();
        break;
      }

      // Weird UI Code:
      /*
      if( p_TouchX < 50 && p_TouchY < 50 ){
      if( !g_SmallOrBig ){
      _SetFoodIcon( 1 );
      g_SmallOrBig = 1;
      }		
      }else if( g_SmallOrBig ){
      _SetFoodIcon( 0 );
      g_SmallOrBig = 0;
      }
      */

      break;

    case 99:
      RestoreState();
      UpdateAgain();
      break;
  }
}

void KawaigariPokemon::State_Eat_Full(void)
{

  if( !mBiteCount && touchX < 50 && touchY < 50 )
  {
    coreSys->GetUIManager()->SetMameButtonLarge(true);		
  }
  else 
  {
    coreSys->GetUIManager()->SetMameButtonLarge(false);
  }

  if(!IsTouching())
  {        
    coreSys->GetUIManager()->SetMameButtonLarge(false);        
  }

  switch(mStateRegs[0])
  {
    case 0:
      mFallCatch = FALL_FOOD_NONE;
      mFallCompleted = false;
      mStateRegs[0]++;

      UpdateAgain();
      break;  

    case 1:
      if(mFallCatch == FALL_FOOD_REVERTED) { mStateRegs[0] = 99; UpdateAgain(); break; }
      else if(mFallCompleted) { mStateRegs[0] = 99; UpdateAgain(); break; }
      else mStateRegs[0]++;
      
      SaveState();
      SetSubState(AI_SUBSTATE_EAT_HATE);

      UpdateAgain();
      break;      

    case 2:
      // Comes back here after Hate();
      mStateRegs[0]=1;
      SaveState();

   ////   SetLoveCommand(COM_FOOD_FALL);
      Eat_FallProgram_Init();

      SetSubState(AI_SUBSTATE_EAT_FALL);

      UpdateAgain();
      break;

    case 3:
      // Comes here after FOOD_FALL:      
      mStateRegs[0] = 1; // Loop      
      break;

    case 99: // EXIT:
      if(coreSys->UnloadFoodObjects()) { mStateRegs[0]++; UpdateAgain(); }
      break;
    case 100:
      mFallCatch = FALL_FOOD_NONE;
      coreSys->GetUIManager()->ChangeToMainMenu();
      SetState(AI_STATE_IDLE);
      break;
  }
}

void KawaigariPokemon::State_Eat_Fall(void)
{

  if( !mBiteCount && touchX < 50 && touchY < 50 )
  {
    coreSys->GetUIManager()->SetMameButtonLarge(true);		
  }
  else 
  {
    coreSys->GetUIManager()->SetMameButtonLarge(false);
  }

  if(!IsTouching())
  {        
    coreSys->GetUIManager()->SetMameButtonLarge(false);        
  }

  switch(mStateRegs[0])
  {
    case 0:      
      coreSys->GetUIManager()->DisableEatModeButton();


      /* WEIRD GUI CODE:
      if( g_SmallOrBig )
      {
      _SetFoodIcon( 0 );
      g_SmallOrBig = 0;
      }
      */

      mFallCatch = FALL_FOOD_NONE;
      mFallCompleted = false;

      Eat_FallProgram_Update();

      mStateRegs[0]++;
      break;  

    case 1:
      // Loop Until Fall Program Complete:
      if(Eat_FallProgram_Update())
      {
        // Exit on Catch:
        if(mFallCatch == FALL_FOOD_CATCHED)
        {
          mFallMode = false;
          RestoreState();
          break;
        }
        else
        {
          coreSys->GetCamera()->SetNewTarget(KG_CAMERA_WHOLE_BODY);
          LookEnd();

          if(mFallCatch==FALL_FOOD_REVERTED) mStateRegs[0] = 99;
          else mStateRegs[0]++;
        }
      }
      break;

    case 2:
      
      ///UpdateCakeButton();
      PlaySE(KW_FALLSWEET);

      {
        bool angerFlag = CanSetMotion(PokeTool::MODEL_ANIME_MAD);
        bool hateFlag =  CanSetMotion(PokeTool::MODEL_ANIME_HATE);

        if( !CheckGroup(GRP_NOREACT) && ( angerFlag || hateFlag ) &&  GetFullnessLevel() < GetFullStuffedValue() )
        {
          int num;
          if( angerFlag && hateFlag )
          {
            num = coreSys->GetRandom(2) ? PokeTool::MODEL_ANIME_MAD : PokeTool::MODEL_ANIME_HATE;
          }
          else if( angerFlag ) num = PokeTool::MODEL_ANIME_MAD;
          else num = PokeTool::MODEL_ANIME_HATE;
          
          coreSys->GetUIManager()->GetG2DUtil()->StartAnime( 0, LA_KW_LAYOUT_KAWAIGARI_MIN_LOW_000_BUTTON_OUT);

          if(ChangeAnimation( num ))
          {
            mStateRegs[0] = 5;
            break;
          }         
          else mStateRegs[0] = 10;
        }
        
        else
        {
 ////         SetParticle(GARC_love_eff_question_r_BCPTL, 0, 0, true, MODEL_NODE_OVERHEAD);
          //coreSys->CreateEffect(KAWAIGARI_EFFECT_QUESTIONMARK);
          mStateRegs[0] = 89;
          mStateRegs[1] = 10; // Wait 10 frames
          
          // Turn Off Food Object:
          coreSys->ShowFoodObject(-1,false);
          break;
        }

        mStateRegs[0] = 10;
      }

      break;

    case 5: // Wait end of Motion:
      if(mModel->IsAnimationLastFrame())
      {
        mStateRegs[0] = 10;
        UpdateAgain();        
      }
      break;

    case 10:
      ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT);
      mStateRegs[0] = 99;
      break;

    case 89:
      mStateRegs[1]--;

      if(mStateRegs[1]<=0)
      {
        coreSys->CreateEffect(KAWAIGARI_EFFECT_QUESTIONMARK);
        mStateRegs[0]++;
        mStateRegs[1] = 30; // Wait 10 frames
        break;
      }
      break;
    case 90:
      mStateRegs[1]--;

      if(mStateRegs[1]<=0)
      {       
        mStateRegs[0] = 99;        
        break;
      }
      break;

    case 99:
      // Turn Off Food Object:
      coreSys->ShowFoodObject(-1,false);

      // Main Menu ON:
      coreSys->GetUIManager()->SetGUIMode(GUI_MODE_NORMAL);

      ChangeFace(TEX_MOUTH, FACE_MOUTH_DEFAULT, true );

      mFell = true;
      mFallMode = false;
      mFallCompleted = true;
    
      RestoreState();
      break;
  }
}

void KawaigariPokemon::State_Eat_Begin(void)
{

  if( !mBiteCount && touchX < 50 && touchY < 50 )
  {
    coreSys->GetUIManager()->SetMameButtonLarge(true);		
  }
  else 
  {
    coreSys->GetUIManager()->SetMameButtonLarge(false);
  }

  if(!IsTouching())
  {        
    coreSys->GetUIManager()->SetMameButtonLarge(false);        
  }

  switch(mStateRegs[0])
  {
    case 0:
      mCanCatchFood = true;
      SetDualScreenMonsterCamera( KawaigariRenderSystem::DUAL_SCREEN_MONSTER_CAMERA_GO_UPPER );
      PlaySE(KW_DRAW);
      SetAdjustHeadAngle( GetAdjustAngleEat() );

      coreSys->GetCamera()->SetNewTarget(KG_CAMERA_EAT);
      // ShowObject( OBJ_FOOD,0,ON );

      //   SetInterestPosition(coreSys->GetFoodPosition());
      SetInterestPosition(coreSys->GetFoodPositionLook(),0.03f);
      mStateRegs[1] = WAIT_SECOND; // One Second Wait

      mStateRegs[0]++;      
      break;  

    case 1: // Waiting:
      mStateRegs[1]--;

      if(mStateRegs[1]<=0 || !IsTouching())
      {
        mStateRegs[0]++;        
        break;
      }
      break;

    case 2:
      if(GetFullnessLevel() >= 255) // Full!
      {
        SetSubState(AI_SUBSTATE_EAT_FULL);
      }
      else
      {
        SetSubState(AI_SUBSTATE_EAT_IDLE);
      }
      break;
  }
}

void KawaigariPokemon::State_Eat_Idle(void)
{

  if( !mBiteCount && touchX < 50 && touchY < 50 )
  {
    coreSys->GetUIManager()->SetMameButtonLarge(true);		
  }
  else 
  {
    coreSys->GetUIManager()->SetMameButtonLarge(false);
  }

  if(!IsTouching())
  {        
    coreSys->GetUIManager()->SetMameButtonLarge(false);        
  }

  int OUTOFRANGEFRAME = 3 * WAIT_SECOND + coreSys->GetRandom(2 * WAIT_SECOND );

  mStateRegs[1] = mTouchFrame; // For Debug

  switch(mStateRegs[0])
  {
    case 0:
      ChangeFace( TEX_EYE, FACE_EYE_DEFAULT );
      ChangeFace(TEX_MOUTH, FACE_MOUTH_LOOK,true);
      mTouchFrame = 0;

      SetAdjustHeadAngle( GetAdjustAngleEat() );
      mStateRegs[0]++;
      UpdateAgain();
      break;  

    case 1:

      if(mModel->IsAnimationLastFrame())
      {
        ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT);
      }

      if(IsTouching())
      {
        if( !CheckGroup(GRP_NOREACT) )
        {          
          if( CheckFoodRange() )
          {            
            mStateRegs[0] = 15;
            SaveState();
            SetSubState(AI_SUBSTATE_EAT_THINK);
            UpdateAgain();
          //  ChangeFace(TEX_MOUTH, FACE_MOUTH_LOOK,true);
          }
          else	if( OUTOFRANGEFRAME < mTouchFrame  && CanSetMotion(PokeTool::MODEL_ANIME_TOUCH))
          {
            mStateRegs[0] = 15;
            SaveState();
            SetSubState(AI_SUBSTATE_EAT_TRY);            
            UpdateAgain();
          //  ChangeFace(TEX_MOUTH, FACE_MOUTH_LOOK,true);
          }
          else
          {
            mTouchFrame++;
            break;            
          }
        }
      }
      else
      {
        // SetLoveCommand( COM_FOOD_FALL ); // 食べ物落下モードへ	
        Eat_FallProgram_Init();
        mStateRegs[0]=10; // After Fall
        SaveState();
        SetSubState(AI_SUBSTATE_EAT_FALL);
        break;
      }


      mTouchFrame++;
      break;

    case 10: // After Fall Program:
      if(mModel->IsAnimationLastFrame())
      {
        ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT);
      }

      if(mFell) 
      {
        if(coreSys->UnloadFoodObjects())
        {
          SetState(AI_STATE_IDLE);
          coreSys->GetUIManager()->ChangeToMainMenu();
        }        
      }
      else
      {
        mTouchFrame = 0;
        ChangeFace(TEX_MOUTH, FACE_MOUTH_LOOK,true);
        mStateRegs[0] = 1;
        break;
      }
      break;
    
    case 15: // Mid Main Loop:

      if(mModel->IsAnimationLastFrame())
      {
        ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT);
      }

      if( mFallMode )
      {
        Eat_FallProgram_Init();
        mStateRegs[0]=10; // After Fall
        SaveState();
        SetSubState(AI_SUBSTATE_EAT_FALL);
        break;
      }
      else if( FoodAnimMax() <= mBiteCount || mFell )
      {
        if(coreSys->UnloadFoodObjects())
        {        
          ChangeFace(TEX_MOUTH, FACE_MOUTH_DEFAULT,true);
          SetState(AI_STATE_IDLE);
          coreSys->GetUIManager()->ChangeToMainMenu();
        }

        break;
      }
      else
      {
        mTouchFrame = 0;
      }
            
      mStateRegs[0] = 1; // LOOP
      break;

    case 99:
      if(coreSys->UnloadFoodObjects()) mStateRegs[0]++;
      break;

    case 100:
      SetState(AI_STATE_IDLE);
      coreSys->GetUIManager()->ChangeToMainMenu();

      break;

  }
}

void KawaigariPokemon::Eat_FallProgram_Init(void)
{
  gfl2::math::Vector3 pos,old;

  coreSys->Get3DTouchPositionByXY(&pos,touchX,touchY,TPT_MAME);
  coreSys->Get3DTouchPositionByXY(&old,lastTouchX,lastTouchY,TPT_MAME);

  if( !mBiteCount && touchX < 50 && touchY < 50 )
  {
    ///Graphic::GetInstance()->RevertCake();
    mRevert = true;
    coreSys->GetMameManager()->AddBean( coreSys->GetMameType() );

    coreSys->ShowFoodObject(-1,false); // Turn Off Food Object Immediately!
  }
  else mRevert = false;

  mFallSpeed = pos - old;

  mFallSpeed *= 0.3f;
  mFallSpeed.z = 0.0f;

  if(mFallSpeed.x > 10.0f) mFallSpeed.x = 10.0f;
  if(mFallSpeed.x < -10.0f) mFallSpeed.x = -10.0f;

  if(mFallSpeed.y > 10.0f) mFallSpeed.y = 10.0f;
  if(mFallSpeed.y < -10.0f) mFallSpeed.y = -10.0f;

  if(CheckMotion(PokeTool::MODEL_ANIME_EAT_A)) ChangeAnimation(PokeTool::MODEL_ANIME_EAT_C);
  else if(CheckMotion(PokeTool::MODEL_ANIME_EAT_B)) ChangeAnimation(PokeTool::MODEL_ANIME_EAT_C);
  else if(mModel->IsAnimationLastFrame()) ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT);

}



const f32 KW_DEFAULT_GRAVITY = (980.665f / (60.0f * 60.0f));
const f32 KW_PIECE_GRAVITY = 2.0f;

bool KawaigariPokemon::Eat_FallProgram_Update(void)
{

  if( mRevert )
  {
    mFallCatch = FALL_FOOD_REVERTED;
    return true;
  }

  if(mModel->IsAnimationLastFrame())
  {
    if(CheckMotion(PokeTool::MODEL_ANIME_EAT_A)) ChangeAnimation(PokeTool::MODEL_ANIME_EAT_C);
    else if(CheckMotion(PokeTool::MODEL_ANIME_EAT_B)) ChangeAnimation(PokeTool::MODEL_ANIME_EAT_C);
    else if(CheckMotion(PokeTool::MODEL_ANIME_EAT_C)) ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT);
    else ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT);
  }
  
  mFallSpeed.y -= KW_DEFAULT_GRAVITY * 0.15f;
  gfl2::math::Vector3 *foodPosition = coreSys->GetFoodPosition();

  *foodPosition += mFallSpeed;

  bool result = false; // Continue!

  if( foodPosition->y < 0.0f )
  {    
    foodPosition->y = 0.0f;
    mFallSpeed.Set(0.0f,0.0f,0.0f);

    SetInterestPosition(NULL);

    //rot.SetZero( );
    //rotp = &rot;    
    //ResetBodyDirection( );
    result = true;
  }

  else 
  {    
    // Get Current Touch Position:
    gfl2::ui::TouchPanel * touchPanel;
    touchPanel = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetTouchPanel( gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD );

    if( touchPanel->IsTouch() && mCanCatchFood)
    {                  
      float foodPosX,foodPosY;
      coreSys->ConvertWorldPointToScreenPoint_Mame(foodPosX,foodPosY,*foodPosition);
            
      u32 tx = touchPanel->GetX();
      u32 ty = touchPanel->GetY();
      
      bool hit = false;
      if(tx < foodPosX - 42) hit = false;
      else if(tx > foodPosX + 32) hit = false;
      else if(ty < foodPosY - 35) hit = false;
      else if(ty > foodPosY + 26) hit = false;
      else hit = true;

      if(hit)
      {        
        mFallCatch = true;
        result = true;
      }
      
    }
  }

  SetInterestPosition(coreSys->GetFoodPositionLook(),0.03f);

  return(result); // EXIT!

}

static const f32 PIECE_VX_MIN = -3.0f;
static const f32 PIECE_VX_MAX = 6.0f;
static const f32 PIECE_VY_MIN = 0.0f;
static const f32 PIECE_VY_MAX = 5.0f;
static const f32 PIECE_VZ_MIN = -3.0f;
static const f32 PIECE_VZ_MAX = 0.0f;
static const f32 PIECE_GRAVITY = 2.0f;
static const s32 PIECE_RX_MIN = 2;
static const s32 PIECE_RX_MAX = 10;
static const s32 PIECE_RY_MIN = 2;
static const s32 PIECE_RY_MAX = 10;
static const s32 PIECE_RZ_MIN = 2;
static const s32 PIECE_RZ_MAX = 10;

void KawaigariPokemon::Eat_BiteProgram_Init(int motion)
{
  // POKEMON(724) Has EXCEPTION! (NO ANIMATION SMOOTH)
  if(!IsCharacterID(MONSNO_HUKUROU3) || motion!=PokeTool::MODEL_ANIME_EAT_A) ChangeAnimation(motion);
  else ChangeAnimationNotSmooth(motion);

  mShakeFrame = 0;

  if(motion == PokeTool::MODEL_ANIME_EAT_B && mBiteCount<4)
  {
    gfl2::math::Vector3 pos;
    coreSys->Get3DTouchPositionByXY(&pos,touchX,touchY,TPT_MAME);  

    for(int n=0;n<MAX_POKEMAME_PIECE_MODEL_SET;n++)
    {
      coreSys->ShowFoodPieceObject(n,true);
      gfl2::math::Vector3 *piecePos = coreSys->GetFoodPiecePosition(n);
      gfl2::math::Vector3 *pieceRot = coreSys->GetFoodPieceRotation(n);
      *piecePos = pos;

      f32 vxmin, vxmax, vymin, vymax, vzmin, vzmax;
      s32 rxmin, rxmax, rymin, rymax, rzmin, rzmax;

      vxmin = PIECE_VX_MIN;
      vxmax = PIECE_VX_MAX;
      vymin = PIECE_VY_MIN;
      vymax = PIECE_VY_MAX;
      vzmin = PIECE_VZ_MIN;
      vzmax = PIECE_VZ_MAX;
      rxmin = PIECE_RX_MIN;
      rxmax = PIECE_RX_MAX;
      rymin = PIECE_RY_MIN;
      rymax = PIECE_RY_MAX;
      rzmin = PIECE_RZ_MIN;
      rzmax = PIECE_RZ_MAX;

      mPieceVelocity[n].Set(
        gfl2::math::Vector3(
        ( f32 )coreSys->GetRandom( ( s32 )( ( vxmax - vxmin )*10.0f ) )*0.1f + vxmin,
        ( f32 )coreSys->GetRandom( ( s32 )( ( vymax - vymin )*10.0f ) )*0.1f + vymin,
        ( f32 )coreSys->GetRandom( ( s32 )( ( vzmax - vzmin )*10.0f ) )*0.1f + vzmin
        ) );
      
      mPieceAng[n].Set(
        gfl2::math::Vector3(
        gfl2::math::ConvDegToRad( coreSys->GetRandom( rxmax - rxmin ) + rxmin ),
        gfl2::math::ConvDegToRad( coreSys->GetRandom( rymax - rymin ) + rymin ),
        gfl2::math::ConvDegToRad( coreSys->GetRandom( rzmax - rzmin ) + rzmin )
        ) );
      
      mPieceRotation[n].Set(1.0f,1.0f,1.0f);
      *pieceRot = mPieceRotation[n];
    }
  }
}

bool KawaigariPokemon::Eat_BiteProgram_Update(int motion)
{
  // Play Eat Sound:

  // Never Play Eat Sound on EatA
  if(motion==PokeTool::MODEL_ANIME_EAT_A) {} 

  // Play MotionC only if PIKACHU
  else if(motion==PokeTool::MODEL_ANIME_EAT_C && mMonsNo==MONSNO_PIKATYUU) 
  {
    if( (int)mModel->GetAnimationFrame()  == mModel->GetKeyTiming( (PokeTool::MODEL_ANIME)motion,"AK_SEStart03"))
    {
      GFL_PRINT("*** EAT SOUND ***\n");
      PlaySE( KW_SEStart03 ); 
    }
  }

  // MotionB Only:
  else if( motion!=PokeTool::MODEL_ANIME_EAT_C && (int)mModel->GetAnimationFrame()  == mModel->GetKeyTiming( (PokeTool::MODEL_ANIME)motion,"AK_SEStart03"))
  {
    GFL_PRINT("*** EAT SOUND ***\n");
    PlaySE( KW_SEStart03 ); 
  }
  
  // EXCEPTION FOR POKEMON 799: (Erase Mame After Bit Sound + 15 Frames)
  if( motion==PokeTool::MODEL_ANIME_EAT_B && (int)mModel->GetAnimationFrame()-15  == mModel->GetKeyTiming( (PokeTool::MODEL_ANIME)motion,"AK_SEStart03"))
  {
    if(mMonsNo==MONSNO_BIISUTOo) 
    {
      coreSys->ShowFoodObject(-1,false); // Turn Off Food Objects 12F
      mForceEatFinish = true;
    }
  }

  // Update Body and Look Position Here & Food Rotation:
#ifdef USESANGOCALC
  if(motion == PokeTool::MODEL_ANIME_EAT_C)
#else
  if(motion == PokeTool::MODEL_ANIME_EAT_B)
#endif  
  {
    SetFoodPosition(true);
  }
  else
  {
    SetFoodPosition(false); // Reset Food Rotation...

    // New Calcs: B-9

    // Only Shake if C Vibration is not going to play
    if(BRANCH_FOOD_FULLNESS_SLOW <= GetFullnessLevel()  && 2<=FoodAnimMax()) {}  

    else if(motion == PokeTool::MODEL_ANIME_EAT_B)
    {      
      GFL_PRINT("Eat B-Animation %f/%f\n",mModel->GetAnimationFrame(),mModel->GetAnimationEndFrame() );

      int cFrame = (int)mModel->GetAnimationFrame();
/*      int endFrame = (int)mModel->GetAnimationEndFrame();

      if(endFrame - 9 < cFrame)
      {
        int frame = 9-(endFrame-cFrame);

        if(frame < 0)
        {
          int dif = 9 - endFrame;
          frame += dif;
        }
        */

      int eatFrame = mModel->GetKeyTiming( (PokeTool::MODEL_ANIME)motion,"AK_SEStart03");
      int frame = -1;

      if(cFrame>=eatFrame)
      {
        frame = cFrame -eatFrame;
      }

      if(frame>0 && frame <= 9)
      {
        GFL_PRINT("*Vibration Frame: %i\n",frame);

       
        gfl2::math::Vector3 sangoRot;
        gfl2::math::Vector3 *rot = coreSys->GetFoodRotation();

        CalculateFoodRotationSango_MotionB(sangoRot,frame);
        rot->x = gfl2::math::ConvRadToDeg( sangoRot.x );
        rot->y = gfl2::math::ConvRadToDeg( sangoRot.y );
        rot->z = gfl2::math::ConvRadToDeg( sangoRot.z );
      }
    }
    
  }

  // Food Piece Movement:
  if(motion == PokeTool::MODEL_ANIME_EAT_B && mBiteCount<4)
  {
    
    f32 g = KW_DEFAULT_GRAVITY* PIECE_GRAVITY;

    for( int n = 0; n < MAX_POKEMAME_PIECE_MODEL_SET; n++ )
    {
      gfl2::math::Vector3 *pos,*rot;

      pos = coreSys->GetFoodPiecePosition(n);
      rot = coreSys->GetFoodPieceRotation(n);

      *pos += mPieceVelocity[n];      
      mPieceVelocity[n].y -= g;
      
      *rot += mPieceAng[n];
    }

    
    /// SetFoodBodyDirection(); INCLUDES RARELY USED BODY TURN
  }
  
  // Update Food Pieces here too!
  if(mModel->IsAnimationLastFrame()) 
  {
    // Finish: (Turn off All Piece Objects)
    for(int n=0;n<MAX_POKEMAME_PIECE_MODEL_SET;n++)
    {
      coreSys->ShowFoodPieceObject(n,false);
    }

    return(true);
  }

  return( false );
}

void KawaigariPokemon::Eat_BiteProgram_Finish(void)
{
  // Erase Food Pieces:
}

bool KawaigariPokemon::CheckFoodRange(void)
{
  int x = touchX;
  int y = touchY;

  int FOOD_RANGE_Y = 140;
  int FOOD_RANGE_X = 80;
  int SCREEN_X_SIZE = 320;

///  GFL_PRINT("CheckFoodRange: %i,%i\n",x,y);

  if( !mBiteCount && x < 50 && y < 50 )
  {
    coreSys->GetUIManager()->SetMameButtonLarge(true);		
  }
  else 
  {
    coreSys->GetUIManager()->SetMameButtonLarge(false);
  }
  
  return ( FOOD_RANGE_X < x && x < SCREEN_X_SIZE - FOOD_RANGE_X
    && y > FOOD_RANGE_Y ); // 食べられる範囲内

  ///return(true);
}

bool KawaigariPokemon::WaitEating(void)
{
  if(mModel->IsAnimationLastFrame()) return(true);
  return( false );
}

bool KawaigariPokemon::CheckEating(void)
{
  // Use with Wait Eating
  /*
  while( CheckLoveCommand() )
  { 
    // モーションは最後まで再生
    Update();
  }
  */

  if( !mFallMode && IsTouching() )//&& CheckFoodRange() )
  {
    // 食べ物が動きにくい   
  }
  else
  { 
    // Go to fall mode:
    Eat_FallProgram_Init();
    SetSubState(AI_SUBSTATE_EAT_FALL);
    mFallMode = true;
    return true;
  }
  return false;
  
}

  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)