//======================================================================
/**
 * @file	KawaigariPokemon_StateIdle.C++
 * @brief	Kawaigari Pokemon Actor - Touch Pokemon State Code
 * @author	PETE
 * @data	2015.11.13 .... FRiDaY tHE THiRTeeNTh!?
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

// Fade
#include <Fade/include/gfl2_FadeManager.h>


// Sound:
#include <niji_conv_header/sound/VoiceID.h>
#include <niji_conv_header/sound/SoundMiddleID.h>
#include "Sound/include/Sound.h"

// Diary
#include "GameSys/include/GameData.h"
#include "Savedata/include/MyItemSaveDef.h"

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

void KawaigariPokemon::State_Touch(void)
{
  
  switch(mStateRegSub)
  {
    case AI_SUBSTATE_TOUCH_INIT: State_Touch_Init(); break;
    case AI_SUBSTATE_TOUCH_MAIN: State_Touch_Main(); break;    
    case AI_SUBSTATE_TOUCH_BODYTOUCH: State_Touch_BodyTouch(); break;    
    case AI_SUBSTATE_TOUCH_HITANGER: State_Touch_HitAnger(); break;    
  }

  // Fix for holding down touch screen:
  if(mStateReg!=AI_STATE_TOUCH) mTouchStart = true;
 
}

void KawaigariPokemon::State_Touch_Init(void)
{
  coreSys->GetRenderSystem()->GetRenderingPipeLine()->SetUseTouchIDTexture(true);
///  SetSubState(AI_SUBSTATE_TOUCH_TESTTOUCH); // 11 ROM TEST
  SetSubState(AI_SUBSTATE_TOUCH_MAIN);
  UpdateAgain();  
}

void KawaigariPokemon::State_Touch_Main(void)
{
  const int DOUBLE_TOUCH_FRAME_COUNT = 4;
  
  // Registers:
  enum
  {
     DOUBLE_TOUCH_FRAME = 1,
     TOUCH_FRAME = 2,
     BODYTOUCH_FRAME = 3,
     BODYTOUCH_OUT_FRAME = 4,
     TOUCH_TRY_FRAME = 5,
     TOUCH_ICON = 6,
     ACTION_COMMAND = 7,               
     TOUCH_SUCCESS = 8,     
     LOOP_TIMER = 9,
  };

  bool highFiveMotionAvailable = CanSetMotion(PokeTool::MODEL_ANIME_TOUCH);

  switch(mStateRegs[0])
  {
    case 0: // Initialize:        
      ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT); // Idle Animation       
      mStateRegs[DOUBLE_TOUCH_FRAME] = (GetTouchColor())?1:0;
      mStateRegs[TOUCH_TRY_FRAME] = GetMinuteHighFiveValue();
      mStateRegs[TOUCH_ICON] = -1;
      SetAdjustHeadAngle(GetAdjustAngleLook());      
      ChangeFace(TEX_EYE,FACE_EYE_DEFAULT);
      mStateRegs[0]++;
      UpdateAgain();
      break;

    case 1: // while(touching)
      
      if(mStateRegs[ACTION_COMMAND]==1) LookAtTouch();             
      else Func_ResetLook();

      if(!IsTouching())
      {        
        mStateRegs[0]=50; // After Touch Program
        UpdateAgain();
        break;
      }      

      if(WAIT_SECOND/3 == mStateRegs[TOUCH_FRAME])
      {
        // DO COM_LOOK HERE
        mStateRegs[ACTION_COMMAND] = 1; // Look at touch position ON
        ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT); // Idle Animation        
        mStateRegs[TOUCH_ICON] = GetTouchColor()?ICON_ID_STOP_POINT:ICON_ID_ANIME_POINT;        
      }
      
      if( mStateRegs[TOUCH_ICON] == ICON_ID_ANIME_POINT && 1*WAIT_SECOND == mStateRegs[TOUCH_FRAME])
      {
        coreSys->GetCamera()->SetNewTarget(KG_CAMERA_UP);
        ChangeFace(TEX_MOUTH,FACE_MOUTH_LOOK,true);
      }

      // Go to BodyTouch State?
      if(GetTouchColor()!=0)
      {
        mStateRegs[BODYTOUCH_OUT_FRAME] = 0;

        if(WAIT_SECOND/3 < ++mStateRegs[BODYTOUCH_FRAME] || 
          (mStateRegs[TOUCH_ICON]==ICON_ID_STOP_POINT && IsTouchSlide(3)) )
        {
          SetSubState(AI_SUBSTATE_TOUCH_BODYTOUCH);
          break;
        }
      }

      else if(10 < ++mStateRegs[BODYTOUCH_OUT_FRAME]) mStateRegs[BODYTOUCH_FRAME] = mStateRegs[BODYTOUCH_OUT_FRAME] = 0;

      // Move to High Five?
      if(mStateRegs[TOUCH_ICON]==ICON_ID_ANIME_POINT && --mStateRegs[TOUCH_TRY_FRAME] <= 0 && 
        GetAffectionLevel() >= BRANCH_TOUCH_HIGHFIVE_LOVEAMOUNT && highFiveMotionAvailable)
      {      
        mStateRegs[0]=100; // High Five Routine
        coreSys->GetUIManager()->DisableMainButtons();
        break;
      }

      SetTouchIcon(mStateRegs[TOUCH_ICON]);

      UpdateBlink();
      mStateRegs[0]++;
      break;

    case 2:
      mStateRegs[TOUCH_FRAME]++;

      if(WAIT_SECOND<mStateRegs[TOUCH_FRAME]) ChangeFace(TEX_MOUTH,FACE_MOUTH_LOOK);

      if(DOUBLE_TOUCH_FRAME_COUNT < ++mStateRegs[DOUBLE_TOUCH_FRAME]) mStateRegs[DOUBLE_TOUCH_FRAME] = 0;

      mStateRegs[0] = 1;
      UpdateAgain();
    
      break;


      // After Touch:
    case 50: 

      LookEnd();

      // Show Hand Tap Animation?
      if(mStateRegs[TOUCH_FRAME]<17)
      {
        SetTouchIcon(ICON_ID_TAP);
        PlaySE(KW_TAP);
      }
      else SetTouchIcon(-1);

      mStateRegs[0]++;
      UpdateAgain();
      break;

    case 51:      
      ChangeFace(TEX_MOUTH,FACE_MOUTH_DEFAULT,true);
      mStateRegs[LOOP_TIMER] = WAIT_SECOND;
      mStateRegs[0]++;      
      UpdateAgain();
      break;
   
    case 52:
      mStateRegs[LOOP_TIMER]--;

      if(mStateRegs[LOOP_TIMER]<=0)
      {        
        mStateRegs[0]=999; // Exit
        break;
      }

      // Go into Hit Anger Mode?
      if(IsTouch()) 
      {        
        if(mStateRegs[DOUBLE_TOUCH_FRAME] && mStateRegs[DOUBLE_TOUCH_FRAME]<=DOUBLE_TOUCH_FRAME_COUNT && GetTouchColor())
        {
          coreSys->GetUIManager()->SetHitTouch(touchX,touchY);
          SetTouchIcon(ICON_ID_HIT);          
          
          PlaySE(KW_HIT);
          SetAdjustHeadAngle(0);
          SetSubState(AI_SUBSTATE_TOUCH_HITANGER);
          break;
        }
        mStateRegs[0]=999; // Exit
        break;
      }
      mStateRegs[0]++;
      break;
    case 53:
      if(mStateRegs[DOUBLE_TOUCH_FRAME] && DOUBLE_TOUCH_FRAME_COUNT < ++mStateRegs[DOUBLE_TOUCH_FRAME]) 
        mStateRegs[DOUBLE_TOUCH_FRAME]=0;

      mStateRegs[0]=52;
      UpdateAgain();
      UpdateBlink(); /// OK?!

      break;


      // High Five Routine:
    case 100:
      LookAtTouch();
      ChangeAnimation(PokeTool::MODEL_ANIME_TOUCH);
      mStateRegs[0]++;
      break;

    case 101:

      LookAtTouch();

      if(GetTouchColor()==COL_HAND_L && !mStateRegs[TOUCH_SUCCESS])
      {
        mStateRegs[TOUCH_SUCCESS] = 1;
        PlaySE(KW_AWAKE);
        ///SetParticle( GARC_love_eff_pop_BCPTL );
        {
          gfl2::ui::TouchPanel * touchPanel;
          touchPanel = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetTouchPanel( gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD );
          u32 touchX = 0,touchY = 0;

          touchX = touchPanel->GetX();
          touchY = touchPanel->GetY();

          gfl2::math::Vector3 pos;  
          coreSys->Get3DTouchPositionByXY(&pos,touchX,touchY,TPT_PARTICLE);
          coreSys->CreateEffect(KAWAIGARI_EFFECT_HIGHFIVE,&pos);
        }

        /*
        {
          gfl2::math::Vector3 pos;
          pos.x = (float)-10.0f;          
          coreSys->CreateEffect(KAWAIGARI_EFFECT_MUSICNOTE,&pos);
        }
        {
          gfl2::math::Vector3 pos;
          pos.x = (float)10.0f;         
          coreSys->CreateEffect(KAWAIGARI_EFFECT_MUSICNOTE,&pos);
        }*/
      }

      SetTouchIcon(mStateRegs[TOUCH_ICON]);

      // Play One More Time?
      if(mModel->IsAnimationLastFrame())
      {
        ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT); // Idle Animation    

        mStateRegs[TOUCH_TRY_FRAME] = GetMinuteHighFiveValue();
        if(mStateRegs[TOUCH_SUCCESS]) mStateRegs[0] = 105; // High Five Success End
        else
        {        
          ChangeAnimation(PokeTool::MODEL_ANIME_TOUCH);
          mStateRegs[0]++;
        }
      }
      break;

    case 102: // 2nd Loop:

      LookAtTouch();

      if(GetTouchColor()==COL_HAND_L && !mStateRegs[TOUCH_SUCCESS])
      {
        mStateRegs[TOUCH_SUCCESS] = 1;
        PlaySE(KW_AWAKE);

        {
          gfl2::ui::TouchPanel * touchPanel;
          touchPanel = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetTouchPanel( gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD );
          u32 touchX = 0,touchY = 0;

          touchX = touchPanel->GetX();
          touchY = touchPanel->GetY();

          gfl2::math::Vector3 pos;  
          coreSys->Get3DTouchPositionByXY(&pos,touchX,touchY,TPT_PARTICLE);
          coreSys->CreateEffect(KAWAIGARI_EFFECT_HIGHFIVE,&pos);
        }

        ///SetParticle( GARC_love_eff_pop_BCPTL );
        /*
        {
          gfl2::math::Vector3 pos;
          pos.x = (float)-10.0f;
          coreSys->CreateEffect(KAWAIGARI_EFFECT_MUSICNOTE,&pos);
        }
        {
          gfl2::math::Vector3 pos;
          pos.x = (float)10.0f;
          coreSys->CreateEffect(KAWAIGARI_EFFECT_MUSICNOTE,&pos);
        }
        */
      }

      SetTouchIcon(mStateRegs[TOUCH_ICON]);

      if(mModel->IsAnimationLastFrame())
      {
        ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT); // Idle Animation    
        mStateRegs[TOUCH_TRY_FRAME] = GetMinuteHighFiveValue();
        if(mStateRegs[TOUCH_SUCCESS]) mStateRegs[0] = 105; // High Five Success End
        else
        {    
          ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT);
          mStateRegs[0]++;
        }
      }
      break;

    case 103: // Finish
     // Func_ResetLook();
      mStateRegs[0]=1; // Back to main Loop
      break;

    case 105: // High Five Success Finish:
      mStateRegs[TOUCH_SUCCESS] = 0;
      ///AddPlayable( PLAYABLE_TOUCH_HIGHTOUCH_VALUE );
      AddPlayfulnessAmount(PLAYABLE_TOUCH_HIGHTOUCH_VALUE);
      
      Func_ResetLook();
      if(Func_SetDelightMotion())
      {        
        mStateRegs[0]++;      
      }
      else
      {
        coreSys->GetUIManager()->EnableMainButtons();
          mStateRegs[0]=1; // Back to main loop
      }

      break;

    case 106: // Wait for end of Delight Motion:
      if(mModel->IsAnimationLastFrame())
      {
        Func_ResetLook();
        ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT);
        coreSys->GetUIManager()->EnableMainButtons();
        mStateRegs[0]=1; // Back to main Loop
      }
      break;

    case 999: // Exit
      coreSys->GetUIManager()->EnableMainButtons();
      coreSys->GetRenderSystem()->GetRenderingPipeLine()->SetUseTouchIDTexture(false);
      LookEnd();
      SetTouchIcon(-1);
      //SetState(AI_STATE_IDLE);
      RestoreState();
      UpdateAgain();
      break;
  }
}

#define PLAYABLE_TOUCH_ETC_SUCCESS_VALUE01 255//50
#define PLAYABLE_TOUCH_ETC_SUCCESS_VALUE02 206//35
#define PLAYABLE_TOUCH_ETC_SUCCESS_VALUE03 0//20

#define PLAYABLE_TOUCH_LIKE_SUCCESS_VALUE01 255//42
#define PLAYABLE_TOUCH_LIKE_SUCCESS_VALUE02 214//16
#define PLAYABLE_TOUCH_LIKE_SUCCESS_VALUE03 0 // NA

#define PLAYABLE_TOUCH_HATE_SUCCESS_VALUE -20//20

#define LOVEAMOUNT_TOUCH_ETC_SUCCESS_VALUE01 0//3
#define LOVEAMOUNT_TOUCH_ETC_SUCCESS_VALUE02 2//2
#define LOVEAMOUNT_TOUCH_ETC_SUCCESS_VALUE03 4//1

#define LOVEAMOUNT_TOUCH_LIKE_SUCCESS_VALUE01 0//3
#define LOVEAMOUNT_TOUCH_LIKE_SUCCESS_VALUE02 2//1
#define LOVEAMOUNT_TOUCH_LIKE_SUCCESS_VALUE03 4//1
#define LOVEAMOUNT_TOUCH_LIKE_SUCCESS_VALUE 0


void KawaigariPokemon::State_Touch_BodyTouch(void)
{

  
  if(mDontTouchMe)
  {
    SetState(AI_STATE_IDLE);
    return;
  }

  if(mStateRegs[0]<30 && !mStopLook) LookAtTouch(); // Execute Unless End State
  //SetInterestPosition(NULL);

  int touchColor = GetTouchColor();
  static int nowTouchMode = TM_NONE;
  int touchCheckFrame = 45;//WAIT_SECOND*2;

  static int icon = ICON_ID_OPEN_HAND;
  static int icon2 = 0;

  switch(mStateRegs[0])
  {
    case 0:
      coreSys->GetUIManager()->DisableMainButtons();
      icon = ICON_ID_OPEN_HAND;
      ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT);
      coreSys->GetCamera()->SetNewTarget(KG_CAMERA_BUST_UP);
      SetTouchIcon(ICON_ID_OPEN_HAND);

      mTouchUpperFrame = 0;
      mTouchLowerFrame = 0;
      mTouchOutFrame = 0;
      nowTouchMode = TM_NONE;

      mEyes = FACE_EYE_DEFAULT;
      mMouth = FACE_MOUTH_DEFAULT;
      mExitFlag = false;
      mTagSpringOffset[0] = mTagSpringOffset[1] =0.0f;
      mSpringOffset[0] = mSpringOffset[1] = 0.0f;
      
      mTouchFrame	= 0;
      mTouchAllFrame = 0;
      mTouchChangeFrame	= 3;
      mTouchMode = TM_NONE;
      
      mEyeTouchFrame = 0;
      mNoIcon = false;

      mSuccess = false;
      mFail	= false;
      mAddLoveAmount	= 0;
      mAddPlayable	= 0;
      mStopLook = false;

      SetPhysics(true); // 耳を物理に
      mIcon2Flag = false;
      SetSlowMotion( UseSlowMotion() );

      mStateRegs[0]++;

      break;

    case 1: // While(!exit) {}          

      if(mExitFlag)
      {
        mStateRegs[0]=99; // Exit While()
        break;
      }
     
      mExitFlag = !IsTouching();

      // Reset Icon:
      icon = ICON_ID_OPEN_HAND;

      
      
      for( int i = 0; i < TOUCH_EAR_MAX; ++i ){ mTagSpringOffset[i] = 0.0; }


      //体の上下で耳の挙動をしたり、顔を上や下向きにする処理
      //以前はこの中で撫で時間も判定していたが、修正後は別な場所
      switch( touchColor ){
      case COL_FACE: case COL_EYE_L_U: case COL_EYE_R_U: case COL_EAR_L: case COL_EAR_R: case COL_SLUDGE_U: case COL_GEL_U:
      case COL_COLD_U: case COL_HATE_U: case COL_HURTS_U: case COL_LIKE_U: case COL_GHOST_U: case COL_GHOST_HATE_U: case COL_GHOST_LIKE_U: case COL_GEL_HATE_U: case COL_GEL_LIKE_U: case COL_SLUDGE_HATE_U: case COL_SLUDGE_LIKE_U:  // 顎より上
        {
          if( touchColor == COL_EAR_L ){
            mTagSpringOffset[ TOUCH_EAR_L ] = GetOffsetEar(); // 耳の垂れ具合
          }else if( touchColor == COL_EAR_R ){
            mTagSpringOffset[ TOUCH_EAR_R ] = GetOffsetEar(); // 耳の垂れ具合
          }

          PlayTouchSound(touchColor);
          mTouchOutFrame = 0;
          SetAdjustHeadAngle( GetAdjustAngleLower() ); // 下向きに補正

          if( touchColor == COL_EYE_L_U || touchColor == COL_EYE_R_U){            
              mEyeTouchFrame = WAIT_SECOND;
          }
          mEyes = FACE_EYE_TOUCHU;
          mMouth = FACE_MOUTH_TOUCHU;
        }
        break;
        case COL_CHIN: case COL_EYE_L_D: case COL_EYE_R_D : case COL_BODY: case COL_SLUDGE_D: case COL_GEL_D: case COL_COLD_D: case COL_HATE_D: case COL_LIKE_D: case COL_HURTS_D: case COL_GHOST_D: case COL_GHOST_HATE_D: case COL_GHOST_LIKE_D: case COL_GEL_HATE_D: case COL_GEL_LIKE_D: case COL_SLUDGE_HATE_D: case COL_SLUDGE_LIKE_D: case COL_HAND_L:
        { // 顎より下

          PlayTouchSound(touchColor);
          mTouchOutFrame = 0;
          ////SetAdjustHeadAngle( FACE_MOUTH_TOUCHD1 ); // 上向きに補正  ** ORIGINAL BUG!!!**
          SetAdjustHeadAngle( GetAdjustAngleUpper() ); // 上向きに補正

          mEyes = FACE_EYE_TOUCHU;
          mMouth = ( ++mTouchLowerFrame < 2 * WAIT_SECOND ) ? FACE_MOUTH_TOUCHD1 : FACE_MOUTH_TOUCHD2;
        }
        break;

        case COL_BG: // 体を触ってない
          {
            if(WAIT_SECOND < ++mTouchOutFrame){ // １秒以上体を触っていなかったら
              mExitFlag = true; // 抜ける
            }
          }
          break;
      }

      switch( touchColor ){
      case COL_BODY: case COL_HAND_L: case COL_GEL_U: case COL_GEL_D: case COL_SLUDGE_U: case COL_SLUDGE_D: case COL_GHOST_U: case COL_GHOST_D:      
        //体
        {
          nowTouchMode = TM_BODY;
        }
        break;
      case COL_EAR_L: case COL_EAR_R:
        //耳
        {
          nowTouchMode = TM_EAR;
        }
        break;
      case COL_EYE_L_U: case COL_EYE_L_D: case COL_EYE_R_U: case COL_EYE_R_D:
        //目
        {
          nowTouchMode = TM_EYE;
        }
        break;
      case COL_FACE: case COL_CHIN:
        //顔
        {
          nowTouchMode = TM_FACE;
        }
        break;
        case COL_HATE_U: case COL_HATE_D: case COL_GEL_HATE_U: case COL_GEL_HATE_D: case COL_SLUDGE_HATE_U: case COL_SLUDGE_HATE_D: case COL_GHOST_HATE_U: case COL_GHOST_HATE_D:      
        //嫌い
        {
          if( GetAffectionLevel() >= 1 ){
            nowTouchMode = TM_HATE;
          }else{
            nowTouchMode = TM_BODY;
          }
        }
        break;
        case COL_LIKE_U: case COL_LIKE_D: case COL_GEL_LIKE_U: case COL_GEL_LIKE_D: case COL_SLUDGE_LIKE_U: case COL_SLUDGE_LIKE_D: case COL_GHOST_LIKE_U: case COL_GHOST_LIKE_D:      
        //好き
        {
          if( GetAffectionLevel() >= 1 ) { 
            nowTouchMode = TM_LIKE;
          } else {
            nowTouchMode = TM_BODY;
          }
        }
        break;
      }

      if( nowTouchMode != TM_NONE ) {

        if( nowTouchMode != TM_HATE ) {
          if( IsTouchSlide(4) ){
            mTouchAllFrame++;
          }
        }

        if( nowTouchMode == mTouchMode && mTouchMode != TM_NONE ) {
          mTouchChangeFrame	= 0;
          if( IsTouchSlide(4) ){
            mTouchFrame++;
          }
        } else{
          if( mTouchChangeFrame++ > 2 ){
            //PrintScreen("ID Change pre=%d now=%d", TouchMode, NowTouchMode)
            mTouchFrame = 0;
            mTouchChangeFrame	= 0;
            mTouchMode	= nowTouchMode;
          }
        }
      }

      if( mTouchMode == mTouchTiredMode )
      {
        //PrintScreen("TiredCount %d pre %d now%d", TouchTiredCount, TouchTiredMode, TouchMode )
        touchCheckFrame += mTouchTiredCount * (WAIT_SECOND*2);
      }
      if( mTouchMode == TM_HATE )
      {
        touchCheckFrame = WAIT_SECOND*1;
      }
      if( mTouchAllFrame >= WAIT_SECOND*5 ) {
        
        mTouchAllFrame = 0;
        mSuccess = true;	
        if( GetPlayfulLevel() >= PLAYABLE_TOUCH_ETC_SUCCESS_VALUE01 ){
          mAddLoveAmount	= LOVEAMOUNT_TOUCH_ETC_SUCCESS_VALUE01;
          mAddPlayable	= 0;							
        }else if( GetPlayfulLevel() >= PLAYABLE_TOUCH_ETC_SUCCESS_VALUE02 ){
          mAddLoveAmount	= LOVEAMOUNT_TOUCH_ETC_SUCCESS_VALUE02;
          mAddPlayable = 30;
        }else if( GetPlayfulLevel() >= PLAYABLE_TOUCH_ETC_SUCCESS_VALUE03 ){
          mAddLoveAmount	= LOVEAMOUNT_TOUCH_ETC_SUCCESS_VALUE03;
          mAddPlayable = 40;
        }
      } else if( mTouchFrame >= touchCheckFrame ) {
        mTouchFrame	= 0;
        mTouchAllFrame = 0;
        
        switch( mTouchMode )
        {
          case TM_HATE:
            {
              mFail = true;
            }
            break;
          case TM_LIKE:
            {
              mSuccess = true;
              if( GetPlayfulLevel() >= PLAYABLE_TOUCH_LIKE_SUCCESS_VALUE01 ){
                mAddLoveAmount	= LOVEAMOUNT_TOUCH_LIKE_SUCCESS_VALUE01;
                mAddPlayable	= 0;
              }	else if( GetPlayfulLevel() >= PLAYABLE_TOUCH_LIKE_SUCCESS_VALUE02 ){
                mAddLoveAmount	= LOVEAMOUNT_TOUCH_LIKE_SUCCESS_VALUE02;
                mAddPlayable = 20;
              }else if( GetPlayfulLevel() >= PLAYABLE_TOUCH_LIKE_SUCCESS_VALUE03 ){
                mAddLoveAmount	= LOVEAMOUNT_TOUCH_LIKE_SUCCESS_VALUE03;
                mAddPlayable = 30;
              }
            }
            break;
          default:
            {
              mSuccess = true;
              if( GetPlayfulLevel() >= PLAYABLE_TOUCH_ETC_SUCCESS_VALUE01 ){
                mAddLoveAmount	= LOVEAMOUNT_TOUCH_ETC_SUCCESS_VALUE01;
                mAddPlayable	= 0;							
              }else if( GetPlayfulLevel() >= PLAYABLE_TOUCH_ETC_SUCCESS_VALUE02 ){
                mAddLoveAmount	= LOVEAMOUNT_TOUCH_ETC_SUCCESS_VALUE02;
                mAddPlayable = 30;
              }else if( GetPlayfulLevel() >= PLAYABLE_TOUCH_ETC_SUCCESS_VALUE03 ){
                mAddLoveAmount	= LOVEAMOUNT_TOUCH_ETC_SUCCESS_VALUE03;
                mAddPlayable = 40;
              }
            }
            break;
        }
      }

      //好き、嫌いの表情変化
      // 嫌なところを触った
      if( TM_HATE == mTouchMode ){
        bool excp = ( CheckGroup(GRP_DRAGON) || CheckGroup(GRP_DRAGONF) || CheckGroup(GRP_SNAKE) );
        mEyes = (excp)? FACE_EYE_TOUCH_HATE:FACE_EYE_TOUCH_HATE;
        mMouth = (excp)? FACE_MOUTH_TOUCH_HATE:FACE_MOUTH_TOUCH_HATE;
        // 好きなところを触った
      }else if( TM_LIKE == mTouchMode ){
        bool excp = ( CheckGroup(GRP_DRAGON) || CheckGroup(GRP_DRAGONF) || CheckGroup(GRP_SNAKE) );
        mEyes = (excp)? FACE_EYE_TOUCH_LIKE:FACE_EYE_TOUCH_LIKE;
        mMouth = (excp)? FACE_MOUTH_TOUCH_LIKE:FACE_MOUTH_TOUCH_LIKE;
      }

      switch( touchColor ){
      case COL_THUNDER_U: case COL_THUNDER_D: // アイコンセット ビリビリ
        {
          coreSys->GetUIManager()->SetHitTouch(touchX,touchY);
          icon = ICON_ID_SHOCK;
          PlaySE( SEQ_SE_KW_ELSHOCK );
          mEyes = FACE_EYE_ELECTRIC;
          mMouth = FACE_MOUTH_ELECTRIC;
          mTouchFrame = 0;
          mTouchAllFrame	= 0;
        }
        break;
      case COL_SLUDGE_U: case COL_SLUDGE_D: case COL_SLUDGE_HATE_D: case COL_SLUDGE_HATE_U: case COL_SLUDGE_LIKE_D: case COL_SLUDGE_LIKE_U: // ヘドロ ベチャベチャ
        {
          icon = ICON_ID_JUNK;
      ///    p_TouchX = p_OldTouchX + (( p_TouchX - p_OldTouchX )>>4); ???????
      ///    p_TouchY = p_OldTouchY + (( p_TouchY - p_OldTouchY )>>4); ???????
          touchX = lastTouchX + (( touchX - lastTouchX )>>4); 
          touchY = lastTouchY + (( touchY - lastTouchY )>>4);        


          static int BechaCount = 0;
          if(!(BechaCount++ & 0x3)){
            icon2 = ICON_ID_FILTH_00 + ((BechaCount/0x4)%5); // べちゃあと
          }
        }
        break;
      case COL_COLD_U: case COL_COLD_D: 	// カキコキカキコキコオリ
        {
          coreSys->GetUIManager()->SetHitTouch(touchX,touchY);
          icon = ICON_ID_FREEZE;
          PlaySE( SEQ_SE_KW_STROKE5 );
          mEyes = FACE_EYE_ICE;
          mMouth = FACE_MOUTH_ICE;
          mTouchFrame = 0;
          mTouchAllFrame	= 0;
        }
        break;
      case COL_GEL_U: case COL_GEL_D: case COL_GEL_HATE_U: case COL_GEL_HATE_D: case COL_GEL_LIKE_U: case COL_GEL_LIKE_D: // ミジュ　ユラユラ
        {
          icon = ICON_ID_WATER;
        ///  p_TouchX = p_OldTouchX + (( p_TouchX - p_OldTouchX )>>2); ????
        ///  p_TouchY = p_OldTouchY + (( p_TouchY - p_OldTouchY )>>2); ????

          touchX = lastTouchX + (( touchX - lastTouchX )>>2); 
          touchY = lastTouchY + (( touchY - lastTouchY )>>2);  
  

        }
        break;
      case COL_BURN_U: case COL_BURN_D: // やけど
        {
          coreSys->GetUIManager()->SetHitTouch(touchX,touchY);
          icon = ICON_ID_BURN_HAND;
          PlaySE( SEQ_SE_KW_BURN );
          mTouchFrame = 0;
          mTouchAllFrame	= 0;
        }
        break;
      case COL_HURTS_U: case COL_HURTS_D: // いたい
        {
          coreSys->GetUIManager()->SetHitTouch(touchX,touchY);
          icon = ICON_ID_HURT_HAND;
          PlaySE( SEQ_SE_KW_INJURE );
          mTouchFrame = 0;
        }
        break;
      case COL_GHOST_U: case COL_GHOST_D: case COL_GHOST_LIKE_U: case COL_GHOST_LIKE_D: case COL_GHOST_HATE_U: case COL_GHOST_HATE_D: // おばけ
        {
          //PlaySE( KW_STROKE );
          icon = ICON_ID_GHOST;
        }
        break;
      }
      SetTouchIcon(icon);

      if(icon2){
        SetTouchIcon2( icon2, true ); // べちゃあと
        mIcon2Flag = true;
      }

      if(-1 != mMouth ){
        ChangeFace( TEX_MOUTH, mMouth, true );
      }
      if( -1 != mEyes ){
        ChangeFace( TEX_EYE, mEyes );
      }

      if(icon==ICON_ID_SHOCK || icon==ICON_ID_FREEZE || icon==ICON_ID_BURN_HAND || icon==ICON_ID_HURT_HAND)
      { 
        //ビリビリアニメーションを再生させる
        mStateRegs[0] = 30;        
        break;
      }
      else
      {
        // 徐々に耳を傾ける　最後の係数が傾ける速度
        UpdateSpringOffset( mSpringOffset, mTagSpringOffset );
        mStateRegs[0]++;        
        UpdateAgain();
      }

      break;

    case 2:
      
      // End of Touch:
      if( mSuccess == true || mFail == true )
      {
        mStateRegs[0]++;

        SetTouchIcon( icon, false );
        break;
        //Suspend(); // wait one frame
      }

      else mStateRegs[0]=1; // Still Touching, go back to top of while() Loop
      

      break;
    

      // End of Touch Program:
    case 3: /// if( success == true || fail == true )      

      mTagSpringOffset[ TOUCH_EAR_R ] = mTagSpringOffset[ TOUCH_EAR_L ] = 0.0;
      
      ///DestroyLoveCommand();// 完全にコマンドを終了させる
      LookEnd(); // 注視処理終了補完処理をしない
      mStopLook = true;

      ///_SetSlowMotion(false);
      TurnOffSlowMotion();

      coreSys->GetRenderSystem()->GetRenderingPipeLine()->SetUseTouchIDTexture(false);

      // モーション補完とAdjustJointが終了するのを待つ
      if( IsCharacterID(MONSNO_BUNYATTO) )
      {
        mStateRegs[0]++; // Go to loop
        mStateRegs[1] = 24;        
      }
      else
      {
        // Pass Through:
        mStateRegs[0]=5;
        UpdateAgain();    
        break;
      }

      break;

    case 4: // CharaID 432 LOOP
      ///for( new frame = 0; frame < 24; ++frame )
      mStateRegs[1]--;
      if(mStateRegs[1]<=0) mStateRegs[0]++;

      if(!CheckMotion(PokeTool::MODEL_ANIME_KW_WAIT) && mModel->IsAnimationLastFrame())
      {
        ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT); // Set IDLE Motion if needed
      }
      break;

    case 5:
      if( mSuccess == true ){

        Savedata::IncRecord( Savedata::Record::RECID_PARLE_NADE );

        //同じ所を撫でると飽きカウント増加
        if( mTouchTiredMode == mTouchMode ){
          mTouchTiredCount++;
        }else{
          mTouchTiredMode = mTouchMode;
          mTouchTiredCount=0;
        }
        
        SetDelightMotion();
        if( mAddLoveAmount == 0 ){
       
          ///   SetParticle( GARC_love_eff_onpu_l_BCPTL, 0, 0, true, MODEL_NODE_OVERHEAD );
          coreSys->CreateEffect(KAWAIGARI_EFFECT_MUSICNOTE);
          
        }else{
          AddAffectionAmount( mAddLoveAmount ); // AddLoveAmount <-- Creates Effects too
          RemovePlayfulnessAmount(-mAddPlayable ); // Just adds the number
        }
      }else if( mFail == true ) {        
        PlaySpecialVoice( SPECIAL_VOICE_BADFEEL );
        ChangeAnimation(PokeTool::MODEL_ANIME_HATE,true);        
        RemoveAffectionAmount( LOVEAMOUNT_TOUCH_LIKE_SUCCESS_VALUE );        
        RemovePlayfulnessAmount( PLAYABLE_TOUCH_HATE_SUCCESS_VALUE );
      }

      // Motion is Set, so loop until motion finished:
      mStateRegs[0]++;
      break;

    case 6: // Loop until motion complete:    
      if(mModel->IsAnimationLastFrame()) mStateRegs[0]++;

      UpdateSpringOffset( mSpringOffset, mTagSpringOffset );
      break;

    case 7:
      
      // JUST LOVE EFFECT:
      mStateRegs[2] = GetJustLoveAmountIndex();
      if(mStateRegs[2]==-1)
      {
        mStateRegs[0] = 10;
        UpdateAgain();
        break;
      }

      mStateRegs[1] = GetJustLoveMotionPlayCount( mStateRegs[2]); // How many times to loop
      mStateRegs[3] = 1; // Play big sound ON
      
      mStateRegs[0]++;
      UpdateAgain();
      break;

    case 8:
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

    case 9:
      if(mModel->IsAnimationLastFrame())
      {
        mStateRegs[1]--;

        if(mStateRegs[1]==0) mStateRegs[0]++; // Done!
        else mStateRegs[0] = 8; // Loop
      }
      break;

    case 10:

      SetPhysics(false);
      ///SetMenu( true );
      ///ClearIcon2( icon2_flag );
      SetState(AI_STATE_IDLE);
      LookEnd();
      UpdateAgain();
      coreSys->GetUIManager()->EnableMainButtons();
      break;




    case 30: //ビリビリアニメーションを再生させる
      coreSys->GetCamera()->SaveCurrentCamera();
      coreSys->GetCamera()->SaveCameraMoveSpeed();
      coreSys->GetCamera()->SetCameraMoveSpeed(0.0f);
      coreSys->GetCamera()->GetCamDirect(mCamPos,mCamTgt);

      if(icon==ICON_ID_SHOCK || icon==ICON_ID_BURN_HAND) //ビリビリ、ヤケドカメラ
         mStateRegs[0] = 40;
      else if(icon==ICON_ID_FREEZE || icon==ICON_ID_HURT_HAND)//氷、トゲカメラ
        mStateRegs[0] = 50;

      break;


    case 40: // Shock & Burn Loops
      mStateRegs[1] = WAIT_SECOND;
      mStateRegs[0]++;
      break;

    case 41:
      mStateRegs[1]--;
      if(mStateRegs[1]<=0) mStateRegs[0]= 60; // End

      // Shake the Camera:
      {
        gfl2::math::Vector3 pos,tgt;

        pos = mCamPos;
        tgt = mCamTgt;

        pos.x += (float)coreSys->GetRandom(2);
        pos.y += (float)coreSys->GetRandom(2);
        tgt.x += (float)coreSys->GetRandom(2);
        tgt.y += (float)coreSys->GetRandom(2);
        coreSys->GetCamera()->SetCamDirect(pos,tgt);
        coreSys->GetRenderSystem()->CalculateCurrentCameraMatrix(true);

      }
     
      break;

    case 50: // Freeze & Hurt Hand Loops
      mStateRegs[1] = WAIT_SECOND/16;
      mStateRegs[0]++;
      UpdateAgain();
      break;

    case 51:
      mStateRegs[1]--;
      if(mStateRegs[1]<=0) mStateRegs[0]++; // Next Camera Shake

      // Shake the Camera:
      {
        gfl2::math::Vector3 pos,tgt;

        pos = mCamPos;
        tgt = mCamTgt;
        
        pos.y += 2.0f;
        tgt.y += 2.0f;
        coreSys->GetCamera()->SetCamDirect(pos,tgt);
        coreSys->GetRenderSystem()->CalculateCurrentCameraMatrix(true);

      }
      break;

    case 52:
      mStateRegs[1] = WAIT_SECOND/2;
      mStateRegs[0]++;
      UpdateAgain();
      break;

    case 53:
      mStateRegs[1]--;
      if(mStateRegs[1]<=0) mStateRegs[0]=60; // End

      // Shake the Camera:
      {
        gfl2::math::Vector3 pos,tgt;

        pos = mCamPos;
        tgt = mCamTgt;

        pos.y -= 2.0f;
        tgt.y -= 2.0f;
        coreSys->GetCamera()->SetCamDirect(pos,tgt);
        coreSys->GetRenderSystem()->CalculateCurrentCameraMatrix(true);

      }

      break;

    case 60: // End?   
      // Restore Camera:
      coreSys->GetCamera()->RestoreCurrentCamera();
      coreSys->GetCamera()->RestoreCameraMoveSpeed();
      coreSys->GetRenderSystem()->CalculateCurrentCameraMatrix(true);

      if(5 <= ++ mBiri2Count) // Too many times so exit
      {
        coreSys->GetRenderSystem()->GetRenderingPipeLine()->SetUseTouchIDTexture(false);
        LookEnd();
        SetTouchIcon( icon, false );
        mStopLook = true;
        ///SetMenu(false);
        TurnOffSlowMotion();
        if(ChangeAnimation(PokeTool::MODEL_ANIME_QUESTION)) mStateRegs[0]++;
        else
        {
          mStateRegs[0]=65;
          break;
        }
        
      }
      
      else
      {
        if(!IsTouching())
        {
          coreSys->GetRenderSystem()->GetRenderingPipeLine()->SetUseTouchIDTexture(false);
          mStateRegs[0]=99; // End Touch Program
          UpdateAgain();
          break;
        }      
        else mStateRegs[0] = 1; // Continue normal procedure...
      }

      // Reset Icon:
      icon = ICON_ID_OPEN_HAND;
      UpdateAgain();

      break;

    case 61:
      if(mModel->IsAnimationLastFrame()) mStateRegs[0]=65; // End
      break;

    case 65:
      ///SetMenu(true);
      mBiri2Count = 0;
      LookEnd();
      SetState(AI_STATE_IDLE);
      coreSys->GetUIManager()->EnableMainButtons();
      
      // Reset Ear Springs & Turn off Ear Springs:
      mTagSpringOffset[ TOUCH_EAR_R ] = mTagSpringOffset[ TOUCH_EAR_L ] = 0.0;
      SetPhysics(false);

      break;

    case 99: // End of While()

      coreSys->GetRenderSystem()->GetRenderingPipeLine()->SetUseTouchIDTexture(false);
      SetTouchIcon(ICON_ID_OPEN_HAND,false);

      mTagSpringOffset[ TOUCH_EAR_R ] = mTagSpringOffset[ TOUCH_EAR_L ] = 0.0;

      /*
      for( ;; ){ // 耳を徐々に戻す do while はないので仕方ない
        UpdateSpringOffset( SpringOffset, TagSpringOffset, 0.125 );
        new ear_end_flag = false;
        for( new i = 0; i < TOUCH_EAR_MAX; ++i ){
          ear_end_flag |= ( 0.4 < Float:floatabs( TagSpringOffset[i] - SpringOffset[i] ) );
        }
        if(!ear_end_flag){
          break;
        }

        Update();
      }
      */
      mStateRegs[0]++;

      break;

    case 100:

      TurnOffSlowMotion();
      
      /// ??? FinishLoveCommand(); // 終了処理開始

      SetAdjustHeadAngle(0);
      ///DestroyLoveCommand();// 完全にコマンドを終了させる

      SetPhysics(false);
      ChangeFace(TEX_MOUTH, FACE_MOUTH_DEFAULT, true );
      ///ClearIcon2( icon2_flag );
      SetState(AI_STATE_IDLE);
      LookEnd();
      coreSys->GetUIManager()->EnableMainButtons();
      
      SetTouchIcon(-1,false); // Turn Off Touch Icon?!

      break;
  }

  GFL_PRINT("*TouchFrame: %i/%i  ChangeFrame: %i  TouchOUt:%i\n",mTouchFrame,touchCheckFrame,mTouchChangeFrame,mTouchOutFrame);

}

void KawaigariPokemon::State_Touch_HitAnger(void)
{
  

  switch(mStateRegs[0])
  {
    case 0:
      coreSys->GetUIManager()->DisableMainButtons();
      coreSys->GetRenderSystem()->GetRenderingPipeLine()->SetUseTouchIDTexture(false);
      SetAdjustHeadAngle(0);
      coreSys->GetCamera()->SetNewTarget(KG_CAMERA_HIT1,0.1f);
      ///SubtractPlayfulValue(PLAYABLE_TOUCH_DOUBLETOUCH_VALUE); // NIJI=0

      if(CanSetMotion(PokeTool::MODEL_ANIME_MAD))
      {
        if(coreSys->GetRandom(2)>0 && ChangeAnimation(PokeTool::MODEL_ANIME_MAD))
        {
          mStateRegs[0]=50; // Wait for End of Motion
          break;          
        }
        else if(ChangeAnimation(PokeTool::MODEL_ANIME_HATE,true))
        {
          PlaySpecialVoice( SPECIAL_VOICE_VILLAINOUS );
          mStateRegs[0]=50; // Wait for End of Motion
          break;
        }        
      }
      else
      {
        if(ChangeAnimation(PokeTool::MODEL_ANIME_HATE,true))
        {
          PlaySpecialVoice( SPECIAL_VOICE_VILLAINOUS );
          mStateRegs[0]=50; // Wait for End of Motion
          break;
        }
      }

      mStateRegs[0] = 999;
      break;


    case 50: // Wait for End of Animation:
      coreSys->GetUIManager()->DisableMainButtons();

      if(mModel->IsAnimationLastFrame())
      {
        mStateRegs[0]=999;
      }
      break;


    case 999: // End State:
      coreSys->GetUIManager()->EnableMainButtons();
      ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT);
      SetState(AI_STATE_IDLE);
      break;
  }

}



  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)