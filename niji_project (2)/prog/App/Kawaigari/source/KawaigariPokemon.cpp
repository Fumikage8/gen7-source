//======================================================================
/**
 * @file	KawaigariPokemon.C++
 * @brief	Kawaigari Pokemon Actor/Object A.I Code
 * @author	PETE
 * @data	2015.10.22
 */
//======================================================================
#include <macro/include/gfl2_Macros.h>
#include <gfx/include/gfl2_types.h>
#include <GameSys/include/GameProcManager.h>

#include "PokeTool/include/PokeModel.h"

#include "KawaigariMain.h"
#include "KawaigariCamera.h"
#include "KawaigariDataManager.h"
#include "KawaigariRenderSystem.h"
#include "KawaigariPokemon.h"

#include "niji_conv_header/app/kawaigari/res2d/kw_layout.h"
#include "niji_conv_header/app/kawaigari/res2d/kw_layout_pane.h"
#include "niji_conv_header/app/kawaigari/res2d/kw_layout_anim_list.h"

// Fade
#include <Fade/include/gfl2_FadeManager.h>

#include <niji_conv_header/sound/VoiceID.h>
#include <niji_conv_header/sound/SoundMiddleID.h>
#include "Sound/include/Sound.h"

// Diary:
#include "GameSys/include/GameData.h"
#include "Savedata/include/MyItemSaveDef.h"

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

KawaigariPokemon::KawaigariPokemon(void)
{  
  mForceEatFinish = false;
  mCanCatchFood = true;
  mTouchOK = true;
  mTouchOKCount = 0;
  mTouchStart = true;
  mIsDualScreenMonster = false;
  mSick = false;
  mCryOnce = false;
  mSleepEffect = -1;
  mForceAppearState = -1;
  mModel = NULL;
  mCanSleepFirstTime = true;
  setMotion = -1;
  lastMotion = currentMotion = PokeTool::MODEL_ANIME_KW_WAIT;
  SetState(AI_STATE_INIT);
  SaveState();

  lastTouchPane = lastTouchAni = -1;

  // Food Related:
  mFallSpeed.x = mFallSpeed.y = mFallSpeed.z = 0.0f;
  mFoodRotateX = 0.0f;
  
  // Body Touch Statics:
  mBiri2Count = 0; // 累計で取りたいので static に
  mTouchTiredMode = TM_NONE;
  mTouchTiredCount = 0;
  mStopLook = false;

  // These values replaced by pokemon parameter values on startup...
  mFullness = 0;
  mAffection = 0;//120;
  mPlayfulness = 0;

  // Care Mode:
  mCareToolType = CARETOOL_NONE;

  // Heart Effect Amount calc:
  for(int z=0;z<5;z++) mLoveAmountOnce[z] = false;

  // Facial Expression:
  for(int z=0;z<TEX_MAX;z++)
  {
    mFacialExpressionWait[z] = 0;
    mFacialExpression[z] = -1;
  }

  mBlinkFrame = 3*WAIT_SECOND;

  mSleepSoundTimer = 0;
}

KawaigariPokemon::~KawaigariPokemon(void)
{  
}

bool KawaigariPokemon::CanChangeMode(void)
{
  switch(GetState())
  {
    case AI_STATE_IDLE:
      switch(mStateRegSub)
      {
        case AI_SUBSTATE_IDLE_MAIN: return(true);
      }
      break;

    case AI_STATE_CAREWAIT:
      return(true);
  }

  return(false);
}

void KawaigariPokemon::SetupDualMonsterCamera(void)
{
  if(CheckGroup(GRP_GIANT)) SetIsDualScreenMonster(true); 
  else if(CheckGroup(GRP_WHALEOH)) SetIsDualScreenMonster(true);
  else SetIsDualScreenMonster(false);

  if(mIsDualScreenMonster)
  {
    coreSys->GetRenderSystem()->SetDualScreenMonsterCameraMode(KawaigariRenderSystem::DUAL_SCREEN_MONSTER_CAMERA,true);
  }
  else
  {
    coreSys->GetRenderSystem()->SetDualScreenMonsterCameraMode(KawaigariRenderSystem::NOT_DUAL_SCREEN_MONSTER_CAMERA,true);
  }
}

void KawaigariPokemon::SetMonsterID(int monsNo,int formNo)
{ 
  mMonsNo = monsNo; mFormNo = formNo; 
}

void KawaigariPokemon::SetDualScreenMonsterCamera(int mode,bool force)
{
  if(mIsDualScreenMonster)
  {
    coreSys->GetRenderSystem()->SetDualScreenMonsterCameraMode(mode,force);
  }
}

void KawaigariPokemon::SetState(int s) 
{ 
  mStateReg=s; 
  mStateRegSub=0; 
  for(int r=0;r<MAX_STATE_REGS;r++) mStateRegs[r] = 0;
  
}
void KawaigariPokemon::SetSubState(int s) 
{ 
  mStateRegSub=s; 
  for(int r=0;r<MAX_STATE_REGS;r++) mStateRegs[r] = 0;
}

void KawaigariPokemon::PrintState(void)
{

  switch(mStateReg)
  {    
  case AI_STATE_INIT: GFL_PRINT("STATE_INIT "); break;
  case AI_STATE_MAIN: GFL_PRINT("STATE_MAIN ");  break;
  case AI_STATE_APPEAR: GFL_PRINT("STATE_APPEAR ");  break;
  case AI_STATE_IDLE: GFL_PRINT("STATE_IDLE "); break;
  case AI_STATE_TOUCH: GFL_PRINT("STATE_TOUCH "); break;
  case AI_STATE_EAT: GFL_PRINT("STATE_EAT "); break;
  case AI_STATE_CARE: GFL_PRINT("STATE_CARE "); break;
  case AI_STATE_CAREWAIT: GFL_PRINT("STATE_CAREWAIT "); break;
  case AI_STATE_END: GFL_PRINT("STATE_END "); break;
  }

  GFL_PRINT("(%i) -> ",mStateRegSub);

  for(int r=0;r<MAX_STATE_REGS;r++)
  {
    if(r && (r&3)==0) GFL_PRINT("  ");
    GFL_PRINT("%03i ",mStateRegs[r]);    
  }

  GFL_PRINT("\n");

  if(mStateReg==AI_STATE_EAT)
  {
    switch(mStateRegSub)
    {
        case AI_SUBSTATE_EAT_INIT: GFL_PRINT("Sub:EAT_INIT\n");  break;
        case AI_SUBSTATE_EAT_BITE: GFL_PRINT("Sub:EAT_BITE\n");  break;
        case AI_SUBSTATE_EAT_THINK: GFL_PRINT("Sub:EAT_THINK\n");  break;
        case AI_SUBSTATE_EAT_TRY: GFL_PRINT("Sub:EAT_TRY\n");  break;
        case AI_SUBSTATE_EAT_FULL: GFL_PRINT("Sub:EAT_FULL\n");  break;
        case AI_SUBSTATE_EAT_HATE: GFL_PRINT("Sub:EAT_HATE\n");  break;
        case AI_SUBSTATE_EAT_FALL: GFL_PRINT("Sub:EAT_FALL\n");  break;
        case AI_SUBSTATE_EAT_BEGIN: GFL_PRINT("Sub:EAT_BEGIN\n");  break;
        case AI_SUBSTATE_EAT_IDLE: GFL_PRINT("Sub:EAT_IDLE\n");  break;
    }
     GFL_PRINT("\n");
  }

  else if(mStateReg==AI_STATE_TOUCH)
  {
    switch(mStateRegSub)
    {
    case AI_SUBSTATE_TOUCH_INIT: GFL_PRINT("Sub:TOUCH_INIT\n");  break;
    case AI_SUBSTATE_TOUCH_MAIN: GFL_PRINT("Sub:TOUCH_MAIN\n");  break;
    case AI_SUBSTATE_TOUCH_BODYTOUCH: GFL_PRINT("Sub:TOUCH_BODY\n");  break;
    case AI_SUBSTATE_TOUCH_HITANGER: GFL_PRINT("Sub:TOUCH_HITANGER\n");  break;
    }
     GFL_PRINT("\n");
  }

   
  
}

bool KawaigariPokemon::CanSleepFirstTime(void)
{
  return(false);
  /*
  bool result = mCanSleepFirstTime;
  mCanSleepFirstTime = false;
  return(result);*/
}

bool KawaigariPokemon::CheckGroup(int n) 
{
  if(coreSys->GetDataManager()->GetGroupID()==n) return(true);
  return(false); 
}

void KawaigariPokemon::SaveState(void)
{ 
  mStateRegBackup=mStateReg; 
  mStateRegSubBackup=mStateRegSub; 
  for(int r=0;r<MAX_STATE_REGS;r++) mStateRegsBackup[r] = mStateRegs[r];  
}

void KawaigariPokemon::RestoreState(void)
{ 
  mStateReg=mStateRegBackup; 
  mStateRegSub=mStateRegSubBackup; 
  for(int r=0;r<MAX_STATE_REGS;r++) mStateRegs[r] = mStateRegsBackup[r];  
}

void KawaigariPokemon::SetInterestPosition(const gfl2::math::Vector3* pPosition,float speed)
{
 /* if(pPosition!=NULL)
  {
    GFL_PRINT("*SET INTEREST Pos: %f,%f,%f\n",pPosition->GetX(),pPosition->GetY(),pPosition->GetZ());
  }
  else
  {
    GFL_PRINT("*RESET INTEREST Pos!\n");
  }*/

  mModel->SetInterestPosition(pPosition);

  for (u32 jointIndex = 0; jointIndex < mModel->GetInterestJointCount(); ++jointIndex)
  {    
    mModel->SetInterestSpeed(jointIndex,speed);
  }

}

bool KawaigariPokemon::CanCallPlayerName(void)
{
  // Should only happen once!
  static bool firstTime = true;
  if(firstTime)
  {
    firstTime = false;
    return(true);
  }
  return(firstTime);
}

void KawaigariPokemon::CallPlayerName(int soundID)
{
    /*
  if( CHARACTER_ID == 25 ){ // ピカチュウ
  new call_name = false;
  if( p_LoveAmount >= 200 ){
  if( Random( 100 ) < 60 ){ // 200以上は60%の確率		
  call_name = true;
  }
  }else if( p_LoveAmount >= 100 ) {
  if( Random( 100 ) < 30 ){ // 100以上は30%の確率		
  call_name = true;
  }
  }
  if( call_name == true ){
  g_LastSoundEffect = _CallPlayerName();
  return;
  }
  }
  //PrintScreen("voice %d",voice)
  if(voice){
  PlaySE(voice);
  }
  */
  // PlaySE::SoundID

  if(soundID){
    PlaySE(soundID);
  }
}

void KawaigariPokemon::PlayCareSE(int soundID)
{
  if(!SEDonePlaying(soundID)) return;
  PlaySE(soundID,1);
}

void KawaigariPokemon::StopSE(int soundID,int controlID)
{
  if(soundID<SOUND_MAX)
    soundID = coreSys->GetDataManager()->GetSoundID(soundID);

  if(soundID == -1 || soundID == 0) return;

  if(soundID<VOICE_PV001)
  {
    Sound::StopSE(soundID,0,controlID);
  }
  else
  {
    Sound::StopVoice(0);
  }

}

void KawaigariPokemon::PlaySE(int soundID,int controlID)
{
  if(soundID<SOUND_MAX)
    soundID = coreSys->GetDataManager()->GetSoundID(soundID);

  if(soundID == -1 || soundID == 0) return;

  if(soundID<VOICE_PV001)
  {
    Sound::PlaySE(soundID,0,controlID);
  }
  else
  {
    Sound::PlayVoice(0,(VoiceID)soundID);
  }
    
}

void KawaigariPokemon::PlaySpecialVoice(int soundID)
{
  if( IsCharacterID(MONSNO_PIKATYUU) ){ // ピカチュウ

    if(mFormNo==0 || mFormNo>6)
    {    
      switch( soundID ){
      case SPECIAL_VOICE_BADFEEL: //きもちよくない
        PlaySE( VOICE_PV_PIKA_KW_11 ); break;
      case SPECIAL_VOICE_SADDNLY: //かわいそう
        PlaySE( VOICE_PV_PIKA_KW_13 ); break;
      case SPECIAL_VOICE_VILLAINOUS:  //うらみがましい
        PlaySE( VOICE_PV_PIKA_KW_14 ); break;
      case SPECIAL_VOICE_WAKEUP:  //ねむりからさめる
        PlaySE( VOICE_PV_PIKA_KW_20 ); break;
      case SPECIAL_VOICE_DEEPSLEEPING:  //熟睡している
        PlaySE( VOICE_PV_PIKA_KW_21 ); break;
      case SPECIAL_VOICE_DEEPWAKEUP:  //熟睡から起こされたとき
        PlaySE( VOICE_PV_PIKA_KW_22 ); break;
      }
    }
    else // SATO PIKACHUU:
    {
      switch( soundID ){
      case SPECIAL_VOICE_BADFEEL: //きもちよくない
        PlaySE( VOICE_PV_PIKA_F01_KW_11 ); break;
      case SPECIAL_VOICE_SADDNLY: //かわいそう
        PlaySE( VOICE_PV_PIKA_F01_KW_13 ); break;
      case SPECIAL_VOICE_VILLAINOUS:  //うらみがましい
        PlaySE( VOICE_PV_PIKA_F01_KW_14 ); break;
      case SPECIAL_VOICE_WAKEUP:  //ねむりからさめる
        PlaySE( VOICE_PV_PIKA_F01_KW_20 ); break;
      case SPECIAL_VOICE_DEEPSLEEPING:  //熟睡している
        PlaySE( VOICE_PV_PIKA_F01_KW_21 ); break;
      case SPECIAL_VOICE_DEEPWAKEUP:  //熟睡から起こされたとき
        PlaySE( VOICE_PV_PIKA_F01_KW_22 ); break;
      }
    }
  }else{
    switch( soundID ){
    case SPECIAL_VOICE_BADFEEL: //きもちよくない
      PlaySE(KW_HATE); break;
    case SPECIAL_VOICE_SADDNLY: //かわいそう
      PlaySE( KW_LONELY ); break;
    case SPECIAL_VOICE_VILLAINOUS:  //うらみがましい
      PlaySE(KW_HATE); break;
    case SPECIAL_VOICE_WAKEUP:  //ねむりからさめる
      PlaySE(KW_AWAKE); break;
    case SPECIAL_VOICE_DEEPSLEEPING:  //熟睡している
      PlaySE(KW_ROW); break;
    case SPECIAL_VOICE_DEEPWAKEUP:  //熟睡から起こされたとき
      PlaySE(KW_AWAKE); break;
    }
  }
}

bool KawaigariPokemon::CanSetMotion(int n)
{  
  if(n == PokeTool::MODEL_ANIME_TOUCH)
  {
    const s32 cur = coreSys->GetRenderSystem()->GetDualScreenMonsterCameraMode();

    if( (mMonsNo == MONSNO_HUUPA) && (mFormNo == FORMNO_HUUPA_EARNEST)  &&
      ( cur != KawaigariRenderSystem::DUAL_SCREEN_MONSTER_CAMERA_UPPER) ) // フーパ は カメラを上にしないとタッチをしない
    {
      return false;
    }
  }

  if(GetRestrictMotionLoveAmount(n)>GetAffectionLevel()) return(false);
  return(mModel->IsAvailableAnimation( (PokeTool::MODEL_ANIME)n));
}

bool KawaigariPokemon::IsCharacterID(int num)
{
  if(num==mMonsNo) return(true);
  return(false);
}

void KawaigariPokemon::SetPhysics(bool enable)
{
  mModel->SetSpringEnable(enable);
}

float KawaigariPokemon::GetAppearYPosition(void)
{
  float yPos = 0.0f;
  coreSys->GetDataManager()->GetYAppearPosition(yPos);
  return(yPos);
}

void KawaigariPokemon::AddFullnessLevel(int n)
{
  if(n<0 && coreSys->Mode()==KAWAIGARI_MODE_FIELD)  return;
  if(n<0 && coreSys->Mode()==KAWAIGARI_MODE_FESTIVAL_PLAYER)  return;
  if(n<0 && coreSys->Mode()==KAWAIGARI_MODE_FESTIVAL_EVENT)  return;
  if(n<0 && coreSys->Mode()==KAWAIGARI_MODE_FESTIVAL_NPC)  return;

  mFullness+=n;
  if(mFullness>255) mFullness = 255;
}

int KawaigariPokemon::GetFullnessLevel(void)
{
  return(mFullness);
  //return(coreSys->GetRandom(255+64));
}

void KawaigariPokemon::AddPlayfulnessAmount(int n,bool playSound)
{ 
  if(n<0 && coreSys->Mode()==KAWAIGARI_MODE_FIELD)  return;
  if(n<0 && coreSys->Mode()==KAWAIGARI_MODE_FESTIVAL_PLAYER)  return;
  if(n<0 && coreSys->Mode()==KAWAIGARI_MODE_FESTIVAL_EVENT)  return;
  if(n<0 && coreSys->Mode()==KAWAIGARI_MODE_FESTIVAL_NPC)  return;

  mPlayfulness+=n; 
  
  if(mPlayfulness>255) mPlayfulness=255;
  else if(mPlayfulness<0) mPlayfulness = 0;

  int num = 0;
  if(n!=0)
  {
    if(n==PLAYABLE_IDLE_FIRST || n==PLAYABLE_TOUCH_HIGHTOUCH_VALUE) n=10; // 2 Music note for HighTouch & Appear Finish
    else if(n<0) n=-n;

    num = n / 5;
    if( n > 0 && n < 4 ) num = 1;
    else if( num > 4 ) num = 4;
  
    for( int i = 0; i < num; ++i ) 
    {
    //SetParticle( GARC_love_eff_onpu_BCPTL, -20+Random(40), -5+Random(10), se, MODEL_NODE_OVERHEAD );
    //			Update();                 
      gfl2::math::Vector3 pos;
      //pos.x = (float)(-10+coreSys->GetRandom(20));        
      
      if(num<4) // 2 - 3
      {      
        switch(i)
        {
        case 0: pos.x = -7.5f; break;
        case 1: pos.x = 7.5f; break;
        case 2: pos.x = 0.0f; break;
        }
      }
      else if(num==4) // 4
      {
        switch(i)
        {
        case 0: pos.x = -5.0f; break;
        case 1: pos.x = 5.0f; break;
        case 2: pos.x = -7.5f; break;
        case 3: pos.x = 7.5f; break;
        }
      }

      else if(num==1) pos.x = 0.0f; // Just One, Center
      
      coreSys->CreateEffect(KAWAIGARI_EFFECT_MUSICNOTE,&pos,0,NULL,playSound);                
    }
    
    if( 255 <= GetAffectionLevel() )
    {
      //SetParticle( GARC_love_eff_kirakira_BCPTL, 0, 0, se, MODEL_NODE_CENTER );
   //   coreSys->CreateEffect(KAWAIGARI_EFFECT_CLEANSUCCESS,NULL,0,NULL,playSound);
      coreSys->CreateEffect(KAWAIGARI_EFFECT_AFFECTIONSUCCESS,NULL,0,NULL,playSound);
    }
    
  }
  
}

void KawaigariPokemon::RemovePlayfulnessAmount(int n)
{ 
  if(n>0 && coreSys->Mode()==KAWAIGARI_MODE_FIELD)  return;
  if(n>0 && coreSys->Mode()==KAWAIGARI_MODE_FESTIVAL_PLAYER)  return;
  if(n>0 && coreSys->Mode()==KAWAIGARI_MODE_FESTIVAL_EVENT)  return;
  if(n>0 && coreSys->Mode()==KAWAIGARI_MODE_FESTIVAL_NPC)  return;

  mPlayfulness-=n; 
  
  if(mPlayfulness>255) mPlayfulness=255;
  else if(mPlayfulness<0) mPlayfulness=0;
}

void KawaigariPokemon::SetParameters(int affectionV,int playfulV,int fullnessV)
{
  mAffection = affectionV;
  mPlayfulness = playfulV;
  mFullness = fullnessV;
  mStartAffection = mAffection;
}

void KawaigariPokemon::GetParameters(u8 &affectionV,u8 &playfulV,u8 &fullnessV)
{
  affectionV = (u8)(mAffection&0xFF);
  playfulV = (u8)(mPlayfulness&0xFF);
  fullnessV = (u8)(mFullness&0xFF);  
}

void KawaigariPokemon::SetParametersByMode(void)
{
  switch(coreSys->Mode())
  {
    case KAWAIGARI_MODE_NORMAL:
    case KAWAIGARI_MODE_RESORT:
      break;
  
    case KAWAIGARI_MODE_FIELD: SetParameters(254,254,254); break;
    
    case KAWAIGARI_MODE_FESTIVAL_PLAYER: SetParameters(254,254,254); break;
          
    case KAWAIGARI_MODE_FESTIVAL_EVENT: SetParameters(254,254,254); break;

    case KAWAIGARI_MODE_FESTIVAL_NPC: SetParameters(254,254,254); break;          
  }
}

int KawaigariPokemon::GetPlayfulLevel(void)
{
  return(mPlayfulness);  
}

int KawaigariPokemon::GetAffectionLevel(void)
{
  return(mAffection);  
}

int KawaigariPokemon::GetMinuteHighFiveValue(void)
{
  // #define MINUTE_TOUCH_HITOUCH_VALUE	(((255-p_LoveAmount) / 50 +  1) * SECOND + Random( 1 * SECOND ))
  return(((255-GetAffectionLevel()) / 50 +  1) * WAIT_SECOND + coreSys->GetRandom( 1 * WAIT_SECOND ));
}

int KawaigariPokemon::GetTouchColor(void)
{
  if(!IsTouching()) return(0);
  return(coreSys->GetRenderSystem()->GetRenderingPipeLine()->GetTouchColor());
}

bool KawaigariPokemon::IsSick(void)
{
  return(mSick);
}

bool KawaigariPokemon::IsCalledByMonster(void)
{
  // False for now
  return(false);
}

bool KawaigariPokemon::IsIgnoredTooLong(void)
{
  Savedata::Misc* pMiscSaveData = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetMisc();  

  int count = pMiscSaveData->GetKawaigariPastDays();

  GFL_PRINT("**** Ignore Days: %i *****\n",count);

  if( count >= 5 ) 
  {    
    return(true);  
  }

  return(false);
}

bool KawaigariPokemon::IsTutorialShow(void)
{
  Savedata::Misc* pMiscSaveData = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetMisc();  

  if(!pMiscSaveData->GetKawaigariTutorial())
  {
    pMiscSaveData->SetKawaigariTutorial();
    GFL_PRINT("**** Show Kawaigari Tutorial *****\n");
    return(true);
  }

  GFL_PRINT("**** No Kawaigari Tutorial *****\n");
  return(false);
}

bool KawaigariPokemon::IsTouch(void)
{
  // Touch Pen:
  gfl2::ui::TouchPanel * touchPanel;

  touchPanel = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetTouchPanel( gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD );

  return(touchPanel->IsTouch());
}

bool KawaigariPokemon::IsTouching(void)
{
  // Touch Pen:
  gfl2::ui::TouchPanel * touchPanel;

  touchPanel = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetTouchPanel( gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD );

  if(touchPanel->IsTouchRelease()) return(false);

  return(touchPanel->IsTouch());
  
}

bool KawaigariPokemon::IsTouchStart(void)
{
  
  // Touch Pen:
  gfl2::ui::TouchPanel * touchPanel;
  touchPanel = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetTouchPanel( gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD );
  

  if(mTouchStart && touchPanel->IsTouch())
  {
    mTouchStart = false;
    return(true);
  }

  return(false);
}

bool KawaigariPokemon::IsTouchRelease(void)
{
  // Touch Pen:
  gfl2::ui::TouchPanel * touchPanel;

  touchPanel = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetTouchPanel( gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD );
  
  return(touchPanel->IsTouchRelease());
}

bool KawaigariPokemon::IsTouchSlide(int speed)
{
  if(touchX == lastTouchX && touchY == lastTouchY) return(false);
  return (speed < abs( touchX - lastTouchX ) || speed < abs( touchY - lastTouchY ) );  
}

bool KawaigariPokemon::ConvertTouchTypeToPane(int type,int &pane)
{

  switch(type)
  {
    case ICON_ID_STOP_BECKON:
    case ICON_ID_FADE_BECKON_30F:
    case ICON_ID_FADE_BECKON_15F:
    case ICON_ID_ANIME_BECKON:
      pane = PANENAME_KAWAIGARI_ICN_LOW_000_PANE_OIDEICON; return(true);

    case ICON_ID_TAP:
      pane = PANENAME_KAWAIGARI_ICN_LOW_000_PANE_ICON_TAP0_00; return(true);

    case ICON_ID_HIT:
      pane = PANENAME_KAWAIGARI_ICN_LOW_000_PANE_ICON_HIT0_00; return(true);
    
    case ICON_ID_STOP_POINT:
    case ICON_ID_ANIME_POINT:
      pane = PANENAME_KAWAIGARI_ICN_LOW_000_PANE_FINGERICON; return(true);

    case ICON_ID_OPEN_HAND:
    case ICON_ID_ANIME_HAND:
      pane = PANENAME_KAWAIGARI_ICN_LOW_000_PANE_HANDICON; return(true);

    case ICON_ID_SHOCK:
      pane = PANENAME_KAWAIGARI_ICN_LOW_000_PANE_ICON_BIRIBIRI_00; return(true);

    case ICON_ID_WATER:
      pane = PANENAME_KAWAIGARI_ICN_LOW_000_PANE_ICON_WATER0_00; return(true);
    
    case ICON_ID_FREEZE:
      pane = PANENAME_KAWAIGARI_ICN_LOW_000_PANE_ICON_FREEZE0_00; return(true);
    
    case ICON_ID_JUNK:
      pane = PANENAME_KAWAIGARI_ICN_LOW_000_PANE_ICON_JUNK0_00; return(true);
    
    case ICON_ID_BURN_HAND:
      pane = PANENAME_KAWAIGARI_ICN_LOW_000_PANE_ICON_BURN0_00; return(true);

    case ICON_ID_HURT_HAND:
      pane = PANENAME_KAWAIGARI_ICN_LOW_000_PANE_ICON_HURT0_00; return(true);
      /*
    case ICON_ID_FILTH_00:
      pane = PANENAME_KAWAIGARI_ICN_LOW_000_PANE_FILTH_00; return(true);

    case ICON_ID_FILTH_01:
      pane = PANENAME_KAWAIGARI_ICN_LOW_000_PANE_FILTH_01; return(true);

    case ICON_ID_FILTH_02:
      pane = PANENAME_KAWAIGARI_ICN_LOW_000_PANE_FILTH_02; return(true);

    case ICON_ID_FILTH_03:
      pane = PANENAME_KAWAIGARI_ICN_LOW_000_PANE_FILTH_03; return(true);

    case ICON_ID_FILTH_04:
      pane = PANENAME_KAWAIGARI_ICN_LOW_000_PANE_FILTH_04; return(true);
      */
    case ICON_ID_GHOST:
      pane = PANENAME_KAWAIGARI_ICN_LOW_000_PANE_HANDICON; return(true);
         
  }

   return(false);
}

bool KawaigariPokemon::ConvertTouchTypeToAnimation(int type,int &ani)
{
  switch(type)
  {
  case ICON_ID_STOP_BECKON: // HMM... 
 ///   ani = LA_KW_LAYOUT_KAWAIGARI_ICN_LOW_000_OIDE_KEEP; return(true);
    ani = LA_KW_LAYOUT_KAWAIGARI_ICN_LOW_000_OIDE_KEEP; return(true);

  case ICON_ID_FADE_BECKON_30F:
    ani = LA_KW_LAYOUT_KAWAIGARI_ICN_LOW_000_OIDE_FEDE_30F; return(true);

  case ICON_ID_FADE_BECKON_15F:
    ani = LA_KW_LAYOUT_KAWAIGARI_ICN_LOW_000_OIDE_FEDE_15F; return(true);
  
  case ICON_ID_ANIME_BECKON:
    ani = LA_KW_LAYOUT_KAWAIGARI_ICN_LOW_000_OIDE_KEEP_00; return(true);

  case ICON_ID_TAP:
    ani = LA_KW_LAYOUT_KAWAIGARI_ICN_LOW_000_TAP_ANIM; return(true);

  case ICON_ID_HIT:
    ani = LA_KW_LAYOUT_KAWAIGARI_ICN_LOW_000_HIT_ANIM; return(true);

  case ICON_ID_STOP_POINT:
    ani = LA_KW_LAYOUT_KAWAIGARI_ICN_LOW_000_YUBI_KEEP; return(true);

  case ICON_ID_ANIME_POINT:
    ani = LA_KW_LAYOUT_KAWAIGARI_ICN_LOW_000_YUBI_ANIM; return(true);

  case ICON_ID_OPEN_HAND:
    ani = LA_KW_LAYOUT_KAWAIGARI_ICN_LOW_000_HAND_KEEP; return(true);

  case ICON_ID_ANIME_HAND: // HMM (NOT USED?!)
    ani = LA_KW_LAYOUT_KAWAIGARI_ICN_LOW_000_HAND_KEEP; return(true);

  case ICON_ID_SHOCK:
    ani = LA_KW_LAYOUT_KAWAIGARI_ICN_LOW_000_BIRIBIRI_ANIM; return(true);

  case ICON_ID_WATER:
    ani = LA_KW_LAYOUT_KAWAIGARI_ICN_LOW_000_WATER_ANIM; return(true);

  case ICON_ID_FREEZE:
    ani = LA_KW_LAYOUT_KAWAIGARI_ICN_LOW_000_FREEZE_ANIM; return(true);

  case ICON_ID_JUNK:
    ani = LA_KW_LAYOUT_KAWAIGARI_ICN_LOW_000_JUNK_ANIM; return(true);

  case ICON_ID_BURN_HAND:
    ani = LA_KW_LAYOUT_KAWAIGARI_ICN_LOW_000_BURN_ANM; return(true);

  case ICON_ID_HURT_HAND:
    ani = LA_KW_LAYOUT_KAWAIGARI_ICN_LOW_000_HURT_ANM; return(true);

  case ICON_ID_FILTH_00:
  case ICON_ID_FILTH_01:
  case ICON_ID_FILTH_02:
  case ICON_ID_FILTH_03:
  case ICON_ID_FILTH_04:
    ani = LA_KW_LAYOUT_KAWAIGARI_ICN_LOW_000_FILTH_ANM; return(true);

  case ICON_ID_GHOST:
    ani = LA_KW_LAYOUT_KAWAIGARI_ICN_LOW_000_GHOST_ANM; return(true);    

  }

  return(false);
}

void KawaigariPokemon::SetTouchIcon(int type,bool flag)
{
  int pane,ani;

  if(type==-1)
  {
    // Just turn off current animation:
    if(lastTouchAni!=-1) coreSys->GetUIManager()->StopIconAnimation(lastTouchAni);
    if(lastTouchPane!=-1) coreSys->GetUIManager()->SetIconPaneVisible(lastTouchPane,false);    
    lastTouchAni = lastTouchPane = -1;
    return;
  }

  if(!ConvertTouchTypeToPane(type,pane) ) return; // Can't find valid pane..
  
  // Pane has changed:
  if(lastTouchPane!=pane && lastTouchPane!=-1 || !flag)
  {
    // Turn Off Last Pane:
    if(lastTouchPane!=-1) coreSys->GetUIManager()->SetIconPaneVisible(lastTouchPane,false);    
  }

  lastTouchPane = pane;

  // Turn ON/OFF Current Pane:
  coreSys->GetUIManager()->SetIconPaneVisible(pane,flag);

  if(ConvertTouchTypeToAnimation(type,ani))
  {

    // Animation has changed, should set animation for icon:
    if(lastTouchAni!=ani || lastTouchAni==-1)
    {      
      if(lastTouchAni!=-1) coreSys->GetUIManager()->StopIconAnimation(lastTouchAni);
      coreSys->GetUIManager()->SetIconAnimation(ani);
    }

    lastTouchAni = ani;

  }
  
}

/*
typedef enum
{
EYETYPE_OPEN = 0,    ///通常
EYETYPE_HALFOPEN,    ///半目
EYETYPE_CLOSE,       ///通常閉じ
EYETYPE_CLOSE2,      ///ぎゅっと閉じる
EYETYPE_ANGRY,       ///怒り
EYETYPE_SMILE,       ///ニコ目
EYETYPE_SAD,         ///悲しみ
EYETYPE_OTHER,       ///その他

MOUTHTYPE_CLOSE1 = 0,  ///閉じ口
MOUTHTYPE_OPEN_S,      ///開き 小
MOUTHTYPE_OPEN_L,      ///開き 大
MOUTHTYPE_CLOSE2,      ///閉じ ニュートラル
MOUTHTYPE_OPEN_TRI,    ///開き △
MOUTHTYPE_CLOSE3,      ///閉じ 逆U字
MOUTHTYPE_OTHER1,      ///その他
MOUTHTYPE_OTHER2,      ///その他
}FACE_TYPE;
*/


bool KawaigariPokemon::ChangeFace(int texType,int faceType,bool force)
{
  SetFace(texType,faceType);
  
  if(force) mFacialExpression[texType] = -1;  
  else if( TEX_EYE != texType && mFacialExpressionWait[texType] ) return false;
      
  if( mFacialExpression[texType] != faceType )
  {
    mFacialExpression[texType] = faceType;
    mFacialExpressionWait[texType] = 2*WAIT_SECOND ;
    SetFace(texType,faceType);
    return true;
  }

  return false;
}


void KawaigariPokemon::SetFace(int texType,int faceType)
{
  // GFMMCat[7]：ポケリフレ時のバイバニラの待機中の目の挙動がおかしい
  int eyeTarget = -1; // Target all eyes by default

  // Bai-Vanilla Exception: (Only Blink Right Side) MONSNO 584
  if( GetMonsNo() == MONSNO_BAIBANIRA && faceType == FACE_EYE_BLINK )
  {
    eyeTarget = 1; // Right Side Only Blink
  }

  faceType = coreSys->GetDataManager()->GetFaceAnimationIndex(faceType);
  if(faceType==-1) return; // Invalid Type or No Data Found!

  if(texType==TEX_MOUTH)
  {
//    GFL_PRINT("ChangeMouth: %i\n",faceType);
    if(faceType) mModel->SetFaceAnime_Mouth(faceType);
    else mModel->ResetFaceAnime_Mouth();
  }
  
  else if(texType==TEX_EYE)
  {
//    GFL_PRINT("ChangeEye: %i\n",faceType);
    mModel->SetFaceAnime_Eye(faceType,eyeTarget);
  }

//  SetInterestPosition(NULL,1.0f);
  
}

int KawaigariPokemon::GetBlink(void)
{
  if( --mBlinkFrame < 0 )
  {
    if(mBlinkFrame < -5 )
    { 
      // Eyes Closed during these frames:
      mBlinkFrame = 3 * WAIT_SECOND + coreSys->GetRandom(2 * WAIT_SECOND);
    }
    return FACE_EYE_BLINK;
  }
  return FACE_EYE_DEFAULT;
}

void KawaigariPokemon::UpdateBlink(void)
{
  ChangeFace( TEX_EYE, GetBlink( ) );
}

void KawaigariPokemon::State_Init(void)
{
  mFallSpeed.x = mFallSpeed.y = mFallSpeed.z = 0.0f;

  // Turn off Animation Smooth for first Animation Setting Always:
  mModel->SetFaceAnimeSmoothEnable(false);

  if(IsSick())
  {
    // Turn OFF Face/Mouth Animation Smoothing:
    mModel->SetFaceAnimeSmoothEnable(false);

    if(!ChangeAnimation(PokeTool::MODEL_ANIME_DROWSE_B))
      ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT);    

    mModel->UpdateAnimation();

    // Always Appear Sick: (FORCE)
    SetState(AI_STATE_APPEAR);
    UpdateAgain();
    return;
  }
  else ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT); 
  mModel->UpdateAnimation();

  // Turn Animation Smoothing Setting back on by DEFAULT:
  mModel->SetFaceAnimeSmoothEnable(true);

  if(IsTutorialShow())
  {
    if(coreSys->GetTroubleManager()->HasObjects())
    {
      // Turn OFF Face/Mouth Animation Smoothing:
      mModel->SetFaceAnimeSmoothEnable(false);
    }

    ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT);
    if(coreSys->GetTroubleManager()->HasObjects()) SetSlowMotion(UseSlowMotion());
    SetState(AI_STATE_TUTORIAL);

    
  }
  else if(coreSys->Mode()==KAWAIGARI_MODE_FORCE_IDLE)
  {
    coreSys->GetUIManager()->ChangeToMainMenu();
    ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT);
    SetState(AI_STATE_IDLE);

    coreSys->SetMode(KAWAIGARI_MODE_NORMAL); // Restore Mode to Normal

    // Fade In:
    GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA,15); 
    GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_ALPHA,15);
    
  }
  else if(coreSys->GetTroubleManager()->HasObjects())
  {
    // Turn OFF Face/Mouth Animation Smoothing:
    mModel->SetFaceAnimeSmoothEnable(false);

    if(coreSys->GetTroubleManager()->HasSleepyStatus()) coreSys->GetCamera()->SetLocationDirect(KG_CAMERA_ALL_MOTION);
    else coreSys->GetCamera()->SetLocationDirect(KG_CAMERA_WHOLE_BODY);

    coreSys->GetUIManager()->ChangeToMainMenu();
    ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT);
    SetState(AI_STATE_CAREWAIT);
    SetSlowMotion(UseSlowMotion());    
  }
  else SetState(AI_STATE_APPEAR);
  UpdateAgain();

}

void KawaigariPokemon::State_Main(void)
{
  TouchTest();
}

void KawaigariPokemon::State_CareWait(void)
{  

  if(mStateRegSub==0)
  {
    coreSys->GetUIManager()->DisableEatModeButton();
    //coreSys->GetUIManager()->EnableCareButton();
    //coreSys->GetUIManager()->EnableBackButton();
    //coreSys->GetUIManager()->EnablePokemonSelectButton();

    SetSlowMotion(UseSlowMotion());
    // Fade In:
    GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA,15); 
    GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_ALPHA,15);

    mStateRegSub=1;

    if(mCryOnce == false)
    {
      if(coreSys->GetTroubleManager()->HasStatusEffect() && !coreSys->GetTroubleManager()->HasSleepyStatus() && !coreSys->IsIceStatus()) 
      {
        if(IsCharacterID(MONSNO_PIKATYUU))
        {
          if(mFormNo==0 || mFormNo>6) PlaySE( VOICE_PV_PIKA_KW_12 );
          else PlaySE( VOICE_PV_PIKA_F01_KW_12 );
        }
        else PlaySpecialVoice(SPECIAL_VOICE_SADDNLY);
      }
      mCryOnce = true;
    }
  }

  if(coreSys->GetTroubleManager()->HasSleepyStatus()) coreSys->GetCamera()->SetLocationDirect(KG_CAMERA_ALL_MOTION);
  else coreSys->GetCamera()->SetLocationDirect(KG_CAMERA_WHOLE_BODY);

  coreSys->GetCamera()->ResetIdleCamera();

  if(coreSys->GetTroubleManager()->HasSleepyStatus())
  {
    if(!CheckMotion(PokeTool::MODEL_ANIME_DROWSE_B) || mModel->IsAnimationLastFrame()) 
    {
      ChangeAnimationNotSmooth(PokeTool::MODEL_ANIME_DROWSE_B);      
      SetSlowMotion(UseSlowMotion());
    }
  }
  
  else if(!CheckMotion(PokeTool::MODEL_ANIME_KW_WAIT))
  {
    ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT);           
    SetSlowMotion(UseSlowMotion());
  }

  if(coreSys->GetTroubleManager()->HasStatusEffect() && !coreSys->GetTroubleManager()->HasSleepyStatus())
  {  
    ChangeFace( TEX_MOUTH, FACE_MOUTH_TOUCH_HATE, true );
    ChangeFace( TEX_EYE, FACE_EYE_TOUCH_HATE, true );
  }
  else if(!coreSys->GetTroubleManager()->HasSleepyStatus()) UpdateBlink(); 
    
}

void KawaigariPokemon::State_CameraWait(void)
{
  if(mStateRegSub==0)
  {
    // Fade In:
    GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA,15); 
    GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_ALPHA,15);    
    mStateRegSub=1;
  }

  if(coreSys->GetCamera()->CheckCameraType(KG_CAMERA_PEEP))
  {
    float yPos = 0.0f;
    coreSys->GetDataManager()->GetYAppearPosition(yPos);
    mModel->SetPosition(0.0f,yPos,0.0f);
  }
  else mModel->SetPosition(0.0f,0.0f,0.0f);

  if(!CheckMotion(PokeTool::MODEL_ANIME_KW_WAIT))
  {
    ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT);              
  }

  UpdateBlink(); 

}

void KawaigariPokemon::State_Tutorial(void)
{

  switch(mStateRegSub)
  {
    case 0:
      if(!CheckMotion(PokeTool::MODEL_ANIME_KW_WAIT))
      {
        ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT);     
        if(coreSys->GetTroubleManager()->HasObjects()) SetSlowMotion(UseSlowMotion());
      }

      // Set Camera Direct if has Status or Dirt Objects:
      if(coreSys->GetTroubleManager()->HasSleepyStatus()) coreSys->GetCamera()->SetLocationDirect(KG_CAMERA_ALL_MOTION);
      else if(coreSys->GetTroubleManager()->HasObjects()) coreSys->GetCamera()->SetLocationDirect(KG_CAMERA_WHOLE_BODY);

      // Appear Normally:
      else coreSys->GetCamera()->SetNewTarget(KG_CAMERA_WHOLE_BODY);
      coreSys->GetCamera()->ResetIdleCamera();

      // Fade In:
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA,15); 
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_ALPHA,15);    
      mStateRegSub++;
      mStateRegs[0]=60;
      break;

    case 1:
      if(coreSys->GetTroubleManager()->HasObjects()) State_CareWait();

      mStateRegs[0]--;

      if(mStateRegs[0]<=0)
      {
        coreSys->GetUIManager()->OpenTutorialWindow();
        mStateRegSub++;
      }
      break;

    case 2: // Wait for tutorial window to close
      if(coreSys->GetUIManager()->IsTutorialOver()) mStateRegSub++;
      break;

    case 3:
      coreSys->GetUIManager()->ChangeToMainMenu();
      SetState(AI_STATE_IDLE);
      break;

  }

  
}

void KawaigariPokemon::State_End(void)
{


  int curFrame = mStateRegs[0];

  KawaigariObject_Effect *ballCaptureEffect = NULL,*ballInEffect = NULL;

  // Just idle for 5 seconds and then go back to appear:
  switch(mStateRegSub)
  {
  case 0:
    // SetDualMonsterCamera( DUAL_SCREEN_MONSTER_CAMERA, true );

    SetDualScreenMonsterCamera( KawaigariRenderSystem::DUAL_SCREEN_MONSTER_CAMERA,true );

    if(!coreSys->GetTroubleManager()->HasObjects())
      coreSys->GetCamera()->SetNewTarget(KG_CAMERA_FAR,0.2f);

    mStateRegSub++;
    mModel->SetScale(1.0f,1.0f,1.0f);
    mModel->SetPosition(0.0f,0.0f,0.0f);
    mStateRegs[0] = 0;

    mStateRegs[1] = -1; // Ball Capture Effect
    mStateRegs[2] = -1; // Ball In Effect

    // Reset Shader Color:
    {
      gfl2::math::Vector4 col( 1.0f,1.0f,1.0f,0.0f);
      mModel->SetShaderColor(col);
    }
    break;

  case 1:
    if(mStateRegs[0]>=60) mStateRegSub=100; // End

    AnimationUtil_PokeVisibility(curFrame,0,true);

    AnimationUtil_PokeVisibility(curFrame,22,false);
    AnimationUtil_PokeScale(curFrame,18,22,1.0f,0.70f);
    AnimationUtil_PokeScale(curFrame,22,41,0.70f,0.01f);
 //   AnimationUtil_PokeScale(curFrame,22,23,0.70f,0.01f);
  //  AnimationUtil_PokeMove(curFrame,22,41,0.0f,0.0f,0.0f,0.0f,5.0f,0.0f);

    AnimationUtil_MonsterBallVisibility(curFrame,40,true);
    //AnimationUtil_MonsterBallVisibility(curFrame,60,false);    

    // Create Ball Capture Effect:
    if(curFrame == 16)
    {
      if(mSleepEffect!=-1) coreSys->KillCareEffect(mSleepEffect);
      mSleepEffect = -1;
      mStateRegs[1] = coreSys->CreateCareEffect(KAWAIGARI_EFFECT_BALLCAPTURE);
      PlaySE( SEQ_SE_KW_BALL );
    }
    else if(curFrame==41) // Kill On frame 41..
    {
      coreSys->KillCareEffect(mStateRegs[1]);
      mStateRegs[1] = -1;      
    }

    // Create Ball In Effect:
    if(curFrame == 41)
    {      
      mStateRegs[2] = coreSys->CreateCareEffect(KAWAIGARI_EFFECT_BALLIN);
    }
    else if(curFrame==52) // Kill On frame 52..
    {
      coreSys->KillCareEffect(mStateRegs[2]);
      mStateRegs[2] = -1;      
    }


    // Shadow:
    if(curFrame == 40)
    {
      mStateRegs[3] = coreSys->CreateCareEffect(KAWAIGARI_EFFECT_BALLSHADOW);
      coreSys->GetCareEffect(mStateRegs[3])->SetPosition(0.0f,-4.0f,0.0f);
      coreSys->GetCareEffect(mStateRegs[3])->SetScale(1.05f,1.05f,1.05f);
    }
    else if(curFrame==60) // Kill On frame 60
    {
     // coreSys->KillCareEffect(mStateRegs[3]);
     // mStateRegs[3] = -1;    
    }

    if(mStateRegs[1] != -1) ballCaptureEffect = coreSys->GetCareEffect(mStateRegs[1]);    
    if(mStateRegs[2] != -1) ballInEffect = coreSys->GetCareEffect(mStateRegs[2]);
    
    if(ballCaptureEffect!=NULL)
    {
      // Attach To Waist:      
      gfl2::math::Vector3 pos;
      mModel->GetEffectNodePosition(PokeTool::MODEL_NODE_CENTER,0,&pos); 
      ballCaptureEffect->SetOffset(pos);

      AnimationUtil_EffectScale(ballCaptureEffect,curFrame,16,16,1.0f,2.0f);
      AnimationUtil_EffectScale(ballCaptureEffect,curFrame,17,23,2.0f,0.6f);
      AnimationUtil_EffectScale(ballCaptureEffect,curFrame,23,41,0.6f,0.02f);
    }

    if(ballInEffect!=NULL)
    {  
      // Attach To Waist:      
      gfl2::math::Vector3 pos;

      gfl2::renderingengine::scenegraph::instance::JointInstanceNode* jointNode;
      jointNode = coreSys->GetMonsterBall()->GetJointNode("Waist");
      if( jointNode )
      {
        const gfl2::math::Matrix34 mtx = jointNode->GetWorldMatrix();
        pos = mtx.GetElemPosition();
      }

      ballInEffect->SetOffset(pos);

      AnimationUtil_EffectScale(ballInEffect,curFrame,41,41,0.4f,0.4f);
    }

    mStateRegs[0]++; // Inc Current Frame

   // if(mStateRegs[0]==59) mStateRegSub = 99;

  ///  if(mStateRegs[0]>60) mStateRegSub=0; // Loop
    
    break;
    /*
  case 99:
    {
      static float XPOS = 0.0f,YPOS=-4.0f,ZPOS=0.0f;
      coreSys->GetCareEffect(mStateRegs[3])->SetPosition(XPOS,YPOS,ZPOS);
    }
    break;
    */
  case 100:
    {
      // Fade to black:
      gfl2::math::Vector4 blackCol(0.0f,0.0f,0.0f,0.0f);					
      gfl2::math::Vector4 blackCol2(0.0f,0.0f,0.0f,255.0f);					
      gfl2::math::Vector4 *col = &blackCol;
      gfl2::math::Vector4 *col2 = &blackCol2;				
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA, col, col2,15);			
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_ALPHA, col, col2,15);			
      mStateRegs[0] = 20;

      mStateRegSub++;

    }
    break;

  case 101:
    mStateRegs[0]--;

    if(mStateRegs[0]<=0)
    {
      // Kill Ball Shadow:
      if(mStateRegs[3]!=-1)
      {      
        coreSys->KillCareEffect(mStateRegs[3]);
      }

      // Kill Ball:
      if(mStateRegs[2]!=-1)
      {      
        coreSys->KillCareEffect(mStateRegs[2]);
      }

      if(coreSys->IsDebugMode())
      {        
        mModel->SetScale(1.0f,1.0f,1.0f);
        mModel->SetPosition(0.0f,0.0f,0.0f);
        coreSys->GetMonsterBall()->SetVisible(false);
        coreSys->GetMonsterBall()->SetPosition(0.0f,0.0f,0.0f);
        mModel->SetVisible(true);
        SetState(AI_STATE_APPEAR);

        coreSys->GetUIManager()->EnableMainButtons();

        // Reset Shader Color:
        {
          gfl2::math::Vector4 col( 1.0f,1.0f,1.0f,0.0f);
          mModel->SetShaderColor(col);
        }
      }
      else mStateRegSub++;
    }
    break;

  case 102:
    // End the mode
    coreSys->SetFinished();
    break;
  }
}

void KawaigariPokemon::State_FadeEnd(void)
{

  // Just idle for 5 seconds and then go back to appear:
  switch(mStateRegSub)
  {
  case 0:
    // SetDualMonsterCamera( DUAL_SCREEN_MONSTER_CAMERA, true );
///    coreSys->GetCamera()->SetNewTarget(KG_CAMERA_FAR,0.2f);

    mStateRegSub++;
    break;

  case 1:
    {
      // Fade to black:
      gfl2::math::Vector4 blackCol(0.0f,0.0f,0.0f,0.0f);					
      gfl2::math::Vector4 blackCol2(0.0f,0.0f,0.0f,255.0f);					
      gfl2::math::Vector4 *col = &blackCol;
      gfl2::math::Vector4 *col2 = &blackCol2;				
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA, col, col2,15);			
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_ALPHA, col, col2,15);			
      mStateRegs[0] = 20;

      mStateRegSub++;

    }
    break;

  case 2:
    mStateRegs[0]--;

    if(mStateRegs[0]<=0)
    {        
      if(coreSys->IsDebugMode())
      {        
        SetState(AI_STATE_APPEAR);
      }
      else mStateRegSub++;
    }
    break;

  case 3:
    // End the mode
    coreSys->SetFinished();
    break;
  }
}

#if PM_DEBUG

void KawaigariPokemon::State_PlayAllMotion(void)
{
  int motionTable[] = {     
    PokeTool::MODEL_ANIME_KW_WAIT,    ///待機
    PokeTool::MODEL_ANIME_RESPOND,        ///反応する
    PokeTool::MODEL_ANIME_TURN_A,         ///振り向き１
    PokeTool::MODEL_ANIME_TURN_B,         ///振り向き２
    PokeTool::MODEL_ANIME_DROWSE_A,       ///うたたね(開始)
    PokeTool::MODEL_ANIME_DROWSE_B,       ///うたたね(ループ)
    PokeTool::MODEL_ANIME_DROWSE_C,       ///うたたね(起きる)
    PokeTool::MODEL_ANIME_SLEEP_A,        ///ねむる(ループ)
    PokeTool::MODEL_ANIME_SLEEP_B,        ///ねむる(目を覚ます)
    PokeTool::MODEL_ANIME_HATE,           ///いやがる
    PokeTool::MODEL_ANIME_QUESTION,       ///わからない
    PokeTool::MODEL_ANIME_HAPPY_A,        ///よろこぶ１
    PokeTool::MODEL_ANIME_HAPPY_B,        ///よろこぶ２
    PokeTool::MODEL_ANIME_HAPPY_C,        ///よろこぶ３
    PokeTool::MODEL_ANIME_MOVE_A,         ///くせ１
    PokeTool::MODEL_ANIME_MOVE_B,         ///くせ２
    PokeTool::MODEL_ANIME_MOVE_C,         ///くせ３
    PokeTool::MODEL_ANIME_MOVE_D,         ///くせ４
    PokeTool::MODEL_ANIME_LONELY,         ///さみしがる
    PokeTool::MODEL_ANIME_PLAY_A,         ///かまってほしい１
    PokeTool::MODEL_ANIME_PLAY_B,         ///かまってほしい２
    PokeTool::MODEL_ANIME_MAD,            ///怒る
    PokeTool::MODEL_ANIME_EAT_A,          ///かぶりつく(開始)
    PokeTool::MODEL_ANIME_EAT_B,          ///かぶりつく(ループ)
    PokeTool::MODEL_ANIME_EAT_C,          ///かぶりつく(終了)
    PokeTool::MODEL_ANIME_TOUCH,          ///タッチ
    -1
  };

  wchar_t *menuStr[] = {
    L"KW_WAIT",    ///待機
    L"RESPOND",        ///反応する
    L"TURN_A",         ///振り向き１
    L"TURN_B",         ///振り向き２
    L"ANIME_DROWSE_A",       ///うたたね(開始)
    L"ANIME_DROWSE_B",       ///うたたね(ループ)
    L"ANIME_DROWSE_C",       ///うたたね(起きる)
    L"ANIME_SLEEP_A",        ///ねむる(ループ)
    L"ANIME_SLEEP_B",        ///ねむる(目を覚ます)
    L"ANIME_HATE",           ///いやがる
    L"ANIME_QUESTION",       ///わからない
    L"ANIME_HAPPY_A",        ///よろこぶ１
    L"ANIME_HAPPY_B",        ///よろこぶ２
    L"ANIME_HAPPY_C",        ///よろこぶ３
    L"ANIME_MOVE_A",         ///くせ１
    L"ANIME_MOVE_B",         ///くせ２
    L"ANIME_MOVE_C",         ///くせ３
    L"ANIME_MOVE_D",         ///くせ４
    L"ANIME_LONELY",         ///さみしがる
    L"ANIME_PLAY_A",         ///かまってほしい１
    L"ANIME_PLAY_B",         ///かまってほしい２
    L"ANIME_MAD",            ///怒る
    L"ANIME_EAT_A",          ///かぶりつく(開始)
    L"ANIME_EAT_B",          ///かぶりつく(ループ)
    L"ANIME_EAT_C",          ///かぶりつく(終了)
    L"ANIME_TOUCH",          ///タッチ
    L"NONE" };

  static int preAffection = -1;

  switch(mStateRegSub)
  {
    case 0:
      preAffection = GetAffectionLevel();
      mAffection = 255;
      coreSys->GetUIManager()->DisableMainButtons();
      coreSys->GetCamera()->SetLocationDirect(KG_CAMERA_ALL_MOTION);      
      mModel->ResetFaceAnime_Eye();
      mModel->ResetFaceAnime_Mouth();
      mStateRegSub++;
      break;

    case 1: // Choose Motion
      {
        int motion = mStateRegs[0];

        if(motionTable[motion] == -1) { mStateRegs[0] = 0; motion = mStateRegs[0]; }

        // Motion Doesn't exist, go to next motion:
        if(!mModel->IsAvailableAnimation((PokeTool::MODEL_ANIME)motionTable[motion]))
        {
          mStateRegs[0]++;
          break;
        }

        setMotion = -1;
        ChangeAnimationNotSmooth(motionTable[motion],false);
        ///mModel->ChangeAnimation((PokeTool::MODEL_ANIME)motionTable[motion],true);
        
        coreSys->TextUtil()->ClearDrawText();
        coreSys->TextUtil()->SetTextScale( 0.8f, 0.8f );
        coreSys->TextUtil()->DrawText(0,0,menuStr[motion]);
        
        mStateRegSub++;
      }
      break;

    case 2:

      // Play Eat Sound if Needed:
      {

        int motion = motionTable[mStateRegs[0]];

        if(motion==PokeTool::MODEL_ANIME_EAT_C && mMonsNo==MONSNO_PIKATYUU) 
        {
          if( (int)mModel->GetAnimationFrame()  == mModel->GetKeyTiming( (PokeTool::MODEL_ANIME)motion,"AK_SEStart03"))
          {            
            PlaySE( KW_SEStart03 ); 
          }
        }

        // MotionB Only:
        else if( motion==PokeTool::MODEL_ANIME_EAT_B && (int)mModel->GetAnimationFrame()  == mModel->GetKeyTiming( (PokeTool::MODEL_ANIME)motion,"AK_SEStart03"))
        {          
          PlaySE( KW_SEStart03 ); 
        }

      }

      if(mModel->IsAnimationLastFrame())
      {
        mStateRegs[0]++;
        mStateRegSub = 1;
        break;
      }

      // Exit on Touch:
      if(IsTouch())
      {
        mStateRegSub=999;
        break;
      }
      break;

    case 999: // Exit to IDLE
      mAffection = preAffection;
      coreSys->TextUtil()->ClearDrawText();
      coreSys->GetUIManager()->EnableMainButtons();
      coreSys->GetUIManager()->ChangeToMainMenu();
      mModel->ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT,true);
      SetState(AI_STATE_IDLE);
      break;
  }
}

#endif

void KawaigariPokemon::State_Cleanup(void)
{

}

void KawaigariPokemon::State_Finish(void)
{

}

void KawaigariPokemon::Update(void)
{ 
  
  

  if(mModel==NULL) return; // Need a valid pokemon model to proceed!

  if(mFacialExpressionWait[TEX_MOUTH])
  {
    mFacialExpressionWait[TEX_MOUTH]--;
  }

  gfl2::ui::TouchPanel * touchPanel;
  touchPanel = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetTouchPanel( gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD );
  
  if(touchPanel->IsTouch())
  {
    lastTouchX = touchX;
    lastTouchY = touchY;
    touchX = touchPanel->GetX();
    touchY = touchPanel->GetY();         
  }
  else if(touchPanel->IsTouchRelease()) 
  {
    mTouchStart = true;
  }

  if(!touchPanel->IsTouch())
  {
    mTouchOKCount++;   
  }  

  do 
  {
    mRequireUpdate = false;
  
    switch(mStateReg)
    {
      case AI_STATE_INIT: State_Init(); break;
      case AI_STATE_MAIN: State_Main(); break;
    
      case AI_STATE_APPEAR: State_Appear(); break;
      case AI_STATE_IDLE: State_Idle(); break;
      case AI_STATE_TOUCH: State_Touch(); break;
      case AI_STATE_EAT: coreSys->UpdateFoodPositionLook(); State_Eat(); break;
      case AI_STATE_CARE: State_Care(); break;
      case AI_STATE_CAREWAIT: State_CareWait(); break;
      case AI_STATE_CAMERAWAIT: State_CameraWait(); break;
      case AI_STATE_TUTORIAL: State_Tutorial(); break;

      case AI_STATE_END: State_End(); break;      
      case AI_STATE_FADEEND: State_FadeEnd(); break;

#if PM_DEBUG
      case AI_STATE_PLAY_ALL_MOTION: State_PlayAllMotion(); break;
#endif
  
      case AI_STATE_CLEANUP: State_Cleanup(); break;
      case AI_STATE_FINISH: State_Finish(); break;
    }

    // Print State Debug:
    // PrintState();

  } while (mRequireUpdate);


  // Freeze Animation during Ice Status:
  if(coreSys->GetTroubleManager()->HasFreezeStatus())
  {
    mModel->SetAnimationFrame(0.0f);
    mModel->SetAnimationStepFrame(0.0f);
  }

}

void KawaigariPokemon::Func_LookAtTouchPosition(void)
{
  // Change to look around based on Touch Pos!
  static int timer = 0;
  if(timer == 0)
  {        
  /*  lookPos.x = -100.0f + (coreSys->GetRandom(200));
    lookPos.y = -100.0f + (coreSys->GetRandom(200));
    lookPos.z = 100.0f;
    mModel->SetInterestPosition(&lookPos); */
    LookAtTouch();
  }

  // Move Pokemon Head Around here!
  timer++;

  if(timer>WAIT_SECOND*2)
  {
    timer = 0;
  }

}

void KawaigariPokemon::Func_ResetLook(void)
{    
  SetInterestPosition(NULL,0.03f);  
}

bool KawaigariPokemon::Func_SetDelightMotion(void)
{
  /*
  new excp = ( GROUP_ID == GRP_DRAGON || GROUP_ID == GRP_DRAGONF || GROUP_ID == GRP_SNAKE );
  new mot_exist = false;
  if( !excp ) {
    mot_exist = SetLoveMotion( MOT_DELIGHT1 );
  }
  if( mot_exist == false ){
    mot_exist = SetLoveMotion( MOT_DELIGHT2 );
  }
  return mot_exist; */

  if(CanSetMotion(PokeTool::MODEL_ANIME_HAPPY_A))
  {
    ChangeAnimation(PokeTool::MODEL_ANIME_HAPPY_A);
    return(true);
  }

  else if(CanSetMotion(PokeTool::MODEL_ANIME_HAPPY_B))
  {
    ChangeAnimation(PokeTool::MODEL_ANIME_HAPPY_B);
    return(true);
  }

  return(false);
}

void KawaigariPokemon::TouchTest(void)
{
  // Touch Pen:
  gfl2::ui::TouchPanel * touchPanel;            //!< タッチパネル

  touchPanel = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetTouchPanel( gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD );

  static bool happyMotion = false;

  if(touchPanel->IsTouch() && !happyMotion)
  {
    GFL_PRINT("Touch Pos: %i,%i\n",touchPanel->GetX(),touchPanel->GetY());

    ChangeAnimation(PokeTool::MODEL_ANIME_HAPPY_A);        
    happyMotion = true;
  }

  if(happyMotion && mModel->IsAnimationLastFrame())
  {
    if(!touchPanel->IsTouch())
    {    
      ChangeAnimation(PokeTool::MODEL_ANIME_KW_WAIT);      
      happyMotion = false;
    }
    else
    {
      int rndMot = coreSys->GetRandom(3);
      ChangeAnimation( (PokeTool::MODEL_ANIME)(PokeTool::MODEL_ANIME_HAPPY_A+rndMot),true);          
    }
  }

}

void KawaigariPokemon::SetCurrentMotion(int n)
{
  lastMotion = currentMotion;
  currentMotion = n;  
}

void KawaigariPokemon::ChangeToCurrentMotion(void)
{
  // Cut Motion blending for LONELY Motion:
  if(currentMotion==PokeTool::MODEL_ANIME_LONELY)
  {
    ChangeAnimationNotSmooth(currentMotion);
  }
  else ChangeAnimation(currentMotion);
}

bool KawaigariPokemon::ChangeAnimationNotSmooth(int n,bool noEffect)
{
  return(ChangeAnimation(n,noEffect,false));
}

void KawaigariPokemon::LookAtFood(void)
{
  // Position Convert:
  static gfl2::math::Vector3 vec;
  coreSys->Get3DTouchPositionByXY(&vec,touchX,touchY,TPT_FOOD);  
  SetInterestPosition(&vec,0.03f);
}

void KawaigariPokemon::LookAwayFromFood(void)
{
  // Position Convert:
  static gfl2::math::Vector3 vec;
  
  vec.y = 0.0f;
  vec.z = 50.0f;
  vec.x = -1000.0f;

  if(touchX<160)
  {
      vec.x = 1000.0f;
  }
    
  SetInterestPosition(&vec,0.05f);
  
}


void KawaigariPokemon::LookAtTouch(void)
{
  // Position Convert:
  static gfl2::math::Vector3 vec;
  coreSys->Get3DTouchPositionByXY(&vec,touchX,touchY,TPT_LOOK);  
  SetInterestPosition(&vec,0.03f);
}

void KawaigariPokemon::LookEnd(void)
{
  SetInterestPosition(NULL,0.03f);
  SetAdjustHeadAngle(0);
}

void KawaigariPokemon::SetAdjustHeadAngle(float ang)
{  
  mModel->SetInterestAdjustAngleEnable(true);
  ang = gfl2::math::ConvDegToRad(ang);
  mModel->SetInterestTagAdjustAngle(ang);
}

void KawaigariPokemon::RemoveAffectionAmount(int n)
{ 
  ///if(coreSys->Mode()!=KAWAIGARI_MODE_NORMAL && coreSys->Mode()!=KAWAIGARI_MODE_RESORT) return;

  mAffection-=n; 
}

void KawaigariPokemon::AddAffectionAmount(int n,bool capAmount)
{
  int addAmount = n;
  if(n > 5) n = 5;

  if(capAmount) addAmount = n; 

  if(n && coreSys->Mode()!=KAWAIGARI_MODE_NORMAL && coreSys->Mode()!=KAWAIGARI_MODE_RESORT) n = 1;

  for( int z = 0; z < n ; z++ ) 
  {
  ///  SetParticle( GARC_love_eff_k_heart_BCPTL, -20+Random(40), -5+Random(10), true, MODEL_NODE_OVERHEAD );
    coreSys->CreateEffect(KAWAIGARI_EFFECT_HEART_TOUCH);
    PlaySE( SEQ_SE_KW_HEART1 );

    gfl2::math::Vector3 pos,rot;

    pos.x = (float)(coreSys->GetRandom(40)-20);
    pos.y = 50.0f +(float)(coreSys->GetRandom(10)-5);
    pos.z = 20.0f;
    rot.x = rot.y = 0.0f;
    rot.z = gfl2::math::ConvDegToRad((float)(coreSys->GetRandom(40)-20));

    //coreSys->CreateEffect(KAWAIGARI_EFFECT_HEART_SINGLE,&pos,0,&rot);
    
  }

///  if(coreSys->Mode()!=KAWAIGARI_MODE_NORMAL && coreSys->Mode()!=KAWAIGARI_MODE_RESORT) return;

  if(mAffection<255 && mAffection+addAmount>=255)
  {
    // Record:
    Savedata::IncRecord( Savedata::Record::RECID_NAKAYOSHI );

    // Diary:
    PokeDiary::Set( PDID_COUNT_NAKAYOSHI, mMonsNo);  
  }

  mAffection+=addAmount; 
  if(mAffection>255) mAffection=255;

  if(GetAffectionLevel()>=255) coreSys->CreateEffect(KAWAIGARI_EFFECT_AFFECTIONSUCCESS);



  /*
  new num = value;
  if( num > 5 ){
  num = 5
  }

  for( new i = 0; i < num; ++i ) {
  SetParticle( GARC_love_eff_k_heart_BCPTL, -20+Random(40), -5+Random(10), true, MODEL_NODE_OVERHEAD );
  //      Update();
  }
  if( 255 <= p_LoveAmount ) {
  SetParticle( GARC_love_eff_kirakira_BCPTL, 0, 0, false, MODEL_NODE_FRONT );
  }
  }
  */
}

int KawaigariPokemon::GetJustLoveAmountIndex(void)
{
  int result = -1;
  const int loveAmountValue[ 4 ] = { 50, 100, 150, 255 };

  for(int z=0;z<4;z++)
  {
    if( mStartAffection < loveAmountValue[z] && GetAffectionLevel() >= loveAmountValue[z]  && !mLoveAmountOnce[z]) 
    {
      mLoveAmountOnce[z] = true; // Never Show again
      result = z; // Use Highest Result
    }
  }

  return(result);
}


void KawaigariPokemon::SetJustLoveMotion(int index)
{
  int mot[ 4 ] = { PokeTool::MODEL_ANIME_HAPPY_B, PokeTool::MODEL_ANIME_HAPPY_B, PokeTool::MODEL_ANIME_HAPPY_C, PokeTool::MODEL_ANIME_HAPPY_C };

  if(index<0 || index>=4) return; // Invalid Index...

  setMotion = -1; // Make sure animation is changed..

  if(!ChangeAnimation(mot[index]))
  {    
    setMotion = -1; // Make sure animation is changed.. (FORCE)
    ChangeAnimation(PokeTool::MODEL_ANIME_HAPPY_B);
  }
}

int KawaigariPokemon::GetJustLoveMotionPlayCount(int index)
{
  if(index<0 || index>=4) return(0); // Invalid Index...
  int motCount[4] =  { 2, 2, 2, 3 };
  return(motCount[index]);
}

void KawaigariPokemon::SetJustLoveAmountEffect(void)
{
  /*
  //仲良し度255になったときのご褒美
  // 50, 100, 150, 200, 255
  static LoveAmountOnce[ 5 ] = { false, false, false, false, false };
  static LoveAmountValue[ 5 ] = { 50, 100, 150, 200, 255 }
  static LoveAmountMot[ 5 ] = { MOT_DELIGHT2, MOT_DELIGHT2, MOT_DELIGHT3, MOT_DELIGHT3, MOT_DELIGHT3 }
  static LoveAmountMotCount[5] =  { 2, 2, 2, 2, 3 }

  for( new i = 0; i < 5; ++i ) 
  {
    if( p_StartLoveAmount < LoveAmountValue[i] && p_LoveAmount >= LoveAmountValue[i] ) 
    {
      if( !LoveAmountOnce[i] )
      {
        PrintScreen("LoveAmount just %d!", LoveAmountValue[i] )
          LoveAmountOnce[i] = true;
        for( new j = 0; j < LoveAmountMotCount[i]; ++j ) 
        {
          if( LoveAmountValue[i] == 255 &&  j == 0 ) 
          {
            PlaySE( SEQ_SE_KW_HEART3 ); //255は最初だけ特別音
          }
          else
          {
            PlaySE( SEQ_SE_KW_HEART2 );
          }
          SetParticle( GARC_love_eff_kirakira_BCPTL , 0, 0, false, MODEL_NODE_CENTER );Update();
          SetParticle( GARC_love_eff_k_heart_2_BCPTL, -20+Random(40), -5+Random(10), false, MODEL_NODE_OVERHEAD );Update();
          SetParticle( GARC_love_eff_k_heart_2_BCPTL, -20+Random(40), -5+Random(10), false, MODEL_NODE_OVERHEAD );Update();
          SetParticle( GARC_love_eff_k_heart_2_BCPTL, -20+Random(40), -5+Random(10), false, MODEL_NODE_OVERHEAD );
          if(!SetLoveMotion( LoveAmountMot[i] ))
          {
            SetLoveMotion( MOT_DELIGHT2 );
          }
          Update( WAIT_END );
        }
        break;
      }
    }
  }
  */
}

bool KawaigariPokemon::CheckMotionAvailable(int m)
{
  if(m == PokeTool::MODEL_ANIME_TOUCH)
  {
    const s32 cur = coreSys->GetRenderSystem()->GetDualScreenMonsterCameraMode();
    
    if( (mMonsNo == MONSNO_HUUPA) && (mFormNo == FORMNO_HUUPA_EARNEST)  &&
      ( cur != KawaigariRenderSystem::DUAL_SCREEN_MONSTER_CAMERA_UPPER) ) // フーパ は カメラを上にしないとタッチをしない
    {
      return false;
    }
  }

  return(mModel->IsAvailableAnimation((PokeTool::MODEL_ANIME)m));
}

bool KawaigariPokemon::SetDelightMotion2(void)
{
  int mot = PokeTool::MODEL_ANIME_HAPPY_B;
  if(CheckMotionAvailable(mot))
  {
    mModel->ChangeAnimation( (PokeTool::MODEL_ANIME)mot,true);
    setMotion = mot;

    mModel->ResetFaceAnime_Eye();
    mModel->ResetFaceAnime_Mouth();
    return true;
  }  

  mModel->ResetFaceAnime_Eye();
  mModel->ResetFaceAnime_Mouth();
  // Can't play motion:
  return false;  
}


bool KawaigariPokemon::SetDelightMotion(void)
{
  bool excp = ( CheckGroup(GRP_DRAGON) || CheckGroup(GRP_DRAGONF) || CheckGroup(GRP_SNAKE) );
  bool mot_exist = false;
  if( !excp ) {
    mot_exist = ChangeAnimation(PokeTool:: MODEL_ANIME_HAPPY_A );
  }
  if( mot_exist == false ){
    mot_exist = ChangeAnimation( PokeTool:: MODEL_ANIME_HAPPY_B );
  }

  mModel->ResetFaceAnime_Eye();
  mModel->ResetFaceAnime_Mouth();

  return mot_exist;
}

void KawaigariPokemon::UpdateSpringOffset(float *springOffset,float *tagOffset,float rate)
{
  for( int i = 0; i < TOUCH_EAR_MAX; ++i )
  {
    springOffset[i] += ( tagOffset[i] - springOffset[i] ) * rate;
    if( springOffset[i] != 0.0f )
    {
      SetSpringOffsetY(TOUCH_EAR_MAX-i-1,springOffset[i]);      
    }
  }
}

void KawaigariPokemon::SetSpringOffsetY(int n,float val)
{
  const s32 side = n;
  
  for( s32 i = 0; i < 2 /*proj::motion::SpringJointControl::EACH_SIDE_MAX*/; ++i )
  {
    gfl2::math::Vector3 force;
    force.x = force. z = 0.0f;
    force.y = val;
    mModel->SetSpringDownForce((poke_3d::model::PokemonModel::SpringSide)side,val);  
  }
}
/*
float foodRotMotion_1[10][3]= {
  { 0.0, 0.000000, 0.000000 }, // Frame 0
  { 0.392699f-0.436332f, 0.000000f, -0.002222f }, // Frame 1
  { 0.349066f-0.436332f, 0.000000f, -0.004444f }, // Frame 2
  { 0.399971f-0.436332f, 0.000000f, -0.005454f }, // Frame 3
  { 0.450877f-0.436332f, 0.000000f, -0.002626f }, // Frame 4
  { 0.501782f-0.436332f, 0.000000f, 0.002626f }, // Frame 5
  { 0.488692f-0.436332f, 0.000000f, 0.005454f }, // Frame 6
  { 0.475602f-0.436332f, 0.000000f, 0.004858f }, // Frame 7
  { 0.462512f-0.436332f, 0.000000f, 0.003409f }, // Frame 8
  { 0.449422f-0.436332f, 0.000000f, 0.001619f }, // Frame 9
};

float foodRotScale[5] = { 1.0f,
                                    -0.002666f / -0.002222f, 
                                    -0.003111  / -0.002222f,  
                                    -0.003555  / -0.002222f,  
                                    -0.004000  / -0.002222f };
                                    */

float foodRotMotion_1[10][3]= {
  { 0.0f,0.0f,0.0f }, // Frame 0
  { 0.240849f,0.0f,-0.073225f}, // Frame 1
  { 0.224048f,0.0f,-0.067042f }, // Frame 2
  { 0.163347f,0.0f,-0.048217f }, // Frame 3
  { 0.090140f,0.0f,-0.025253f }, // Frame 4
  { 0.002932f,0.0f,-0.003001f }, // Frame 5
  { -0.036658f,0.0f,0.021025f }, // Frame 6
  { -0.027194f,0.0f,0.014813f }, // Frame 7
  { -0.010438f,0.0f,0.006354f }, // Frame 8
  { 0.001098f,0.0f,-0.000159f }, // Frame 9
};
float foodRotScale[5] = {
  1.000000f, 
  1.400000f, 
  1.600000f, 
  1.200000f, 
  1.000000f, 
};

/*

float foodRotMotion_B[10][3]= {
  { 0.0, 0.000000, 0.000000 }, // Frame 0
  { 0.392699f-0.436332f, 0.000000f, -0.002222f }, // Frame 1
  { 0.349066f-0.436332f, 0.000000f, -0.004444f }, // Frame 2
  { 0.399971f-0.436332f, 0.000000f, -0.005454f }, // Frame 3
  { 0.450877f-0.436332f, 0.000000f, -0.002626f }, // Frame 4
  { 0.501782f-0.436332f, 0.000000f, 0.002626f }, // Frame 5
  { 0.488692f-0.436332f, 0.000000f, 0.005454f }, // Frame 6
  { 0.475602f-0.436332f, 0.000000f, 0.004858f }, // Frame 7
  { 0.462512f-0.436332f, 0.000000f, 0.003409f }, // Frame 8
  { 0.449422f-0.436332f, 0.000000f, 0.001619f }, // Frame 9
};

float foodRotScaleB[5] = { 1.0f,
  -0.002666f / -0.002222f, 
  -0.003111  / -0.002222f,  
  -0.003555  / -0.002222f,  
  -0.004000  / -0.002222f };

  */


float foodRotMotion_B[10][3]= {
  { 0.0f,0.0f,0.0f }, // Frame 0
  { 0.035773f,0.0f,-0.030484f }, // Frame 1
  { 0.061038f,0.0f,-0.052382f }, // Frame 2
  { 0.075045f,0.0f,-0.062875f }, // Frame 3
  { 0.041886f,0.0f,-0.034884f }, // Frame 4
  { 0.034989f,0.0f,-0.028764f }, // Frame 5
  { 0.018870f,0.0f,-0.012147f }, // Frame 6
  { 0.009313f,0.0f,-0.003511f }, // Frame 7
  { 0.003857f,0.0f,0.003874f }, // Frame 8
  { 0.000509f,0.0f,0.003473f }, // Frame 9
};
float foodRotScaleB[5] = {
  1.000000f, 
  1.400000f, 
  1.600000f, 
  1.200000f, 
  1.000000f, 
};

/*
float foodRotMotion_2[11][3]= {
  { 0.436332, 0.000000, 0.000000 }, // Frame 0
  { 0.383972, 0.000000, -0.002666 }, // Frame 1
  { 0.331613, 0.000000, -0.005333 }, // Frame 2
  { 0.392699, 0.000000, -0.006545 }, // Frame 3
  { 0.453786, 0.000000, -0.003151 }, // Frame 4
  { 0.514872, 0.000000, 0.003151 }, // Frame 5
  { 0.499164, 0.000000, 0.006545 }, // Frame 6
  { 0.483456, 0.000000, 0.005829 }, // Frame 7
  { 0.467748, 0.000000, 0.004091 }, // Frame 8
  { 0.452040, 0.000000, 0.001943 }, // Frame 9
  { 0.436332, 0.000000, 0.000000 }, // Frame 10

};

float foodRotMotion_3[11][3]= {
  { 0.436332, 0.000000, 0.000000 }, // Frame 0
  { 0.375246, 0.000000, -0.003111 }, // Frame 1
  { 0.314159, 0.000000, -0.006222 }, // Frame 2
  { 0.385427, 0.000000, -0.007636 }, // Frame 3
  { 0.456695, 0.000000, -0.003677 }, // Frame 4
  { 0.527962, 0.000000, 0.003677 }, // Frame 5
  { 0.509636, 0.000000, 0.007636 }, // Frame 6
  { 0.491310, 0.000000, 0.006801 }, // Frame 7
  { 0.472984, 0.000000, 0.004772 }, // Frame 8
  { 0.454658, 0.000000, 0.002267 }, // Frame 9
  { 0.436332, 0.000000, 0.000000 }, // Frame 10

};

float foodRotMotion_4[11][3]= {
  { 0.436332, 0.000000, 0.000000 }, // Frame 0
  { 0.366519, 0.000000, -0.003555 }, // Frame 1
  { 0.296706, 0.000000, -0.007111 }, // Frame 2
  { 0.378155, 0.000000, -0.008727 }, // Frame 3
  { 0.459603, 0.000000, -0.004202 }, // Frame 4
  { 0.541052, 0.000000, 0.004202 }, // Frame 5
  { 0.520108, 0.000000, 0.008727 }, // Frame 6
  { 0.499164, 0.000000, 0.007772 }, // Frame 7
  { 0.478220, 0.000000, 0.005454 }, // Frame 8
  { 0.457276, 0.000000, 0.002591 }, // Frame 9
  { 0.436332, 0.000000, 0.000000 }, // Frame 10

};

float foodRotMotion_5[11][3]= {
  { 0.436332, 0.000000, 0.000000 }, // Frame 0
  { 0.357793, 0.000000, -0.004000 }, // Frame 1
  { 0.279253, 0.000000, -0.007999 }, // Frame 2
  { 0.370883, 0.000000, -0.009817 }, // Frame 3
  { 0.462512, 0.000000, -0.004727 }, // Frame 4
  { 0.554142, 0.000000, 0.004727 }, // Frame 5
  { 0.530580, 0.000000, 0.009817 }, // Frame 6
  { 0.507018, 0.000000, 0.008744 }, // Frame 7
  { 0.483456, 0.000000, 0.006136 }, // Frame 8
  { 0.459894, 0.000000, 0.002915 }, // Frame 9
  { 0.436332, 0.000000, 0.000000 }, // Frame 10

};
*/

float KawaigariPokemon::GetFoodRotScale(int n)
{
  if(n<0) n=0;
  if(n>4) n=4;
  return(foodRotScaleB[n]);
}

void KawaigariPokemon::SetFoodRotScale(float f,int n)
{
  if(n<0 || n>4) return;

  if(f<0.01f) f =0.0f;
  foodRotScaleB[n] = f;
}

void KawaigariPokemon::GetFoodRotVal(int frame,float &x,float &z)
{
  if(frame<0) frame=0;
  if(frame>=9) frame=0;
  x = foodRotMotion_B[frame+1][0];
  z = foodRotMotion_B[frame+1][2];
}

void KawaigariPokemon::SetFoodRotVal(float x,float z,int frame)
{
  if(frame<0 || frame>=9) return;
  foodRotMotion_B[frame+1][0] = x;
  foodRotMotion_B[frame+1][2] = z;
}

static float sinAng = 0.0f;

void KawaigariPokemon::CalculateFoodRotationSango(gfl2::math::Vector3 &dest)
{  
  dest.x = dest.y = dest. z = 0.0f;
#ifdef USESANGOCALC

  int frame = mShakeFrame;
  if(frame<0) frame = 0;
  if(frame>9) frame = 0;
  dest.x = foodRotMotion_1[frame][0] * foodRotScale[mBiteCount-1];
  dest.y = foodRotMotion_1[frame][1] * foodRotScale[mBiteCount-1];
  dest.z = foodRotMotion_1[frame][2] * foodRotScale[mBiteCount-1];
    
  GFL_PRINT("CalcFoodRot: %i BitCount:%i Rot: %f %f %f\n",mShakeFrame,mBiteCount,dest.x,dest.y,dest.z);

#endif
}

void KawaigariPokemon::CalculateFoodRotationSango_MotionB(gfl2::math::Vector3 &dest,int frame)
{  
  dest.x = dest.y = dest. z = 0.0f;
  
  if(frame<0) frame = 0;
  if(frame>9) frame = 0;
  dest.x = foodRotMotion_B[frame][0] * foodRotScaleB[mBiteCount];
  dest.y = foodRotMotion_B[frame][1] * foodRotScaleB[mBiteCount];
  dest.z = foodRotMotion_B[frame][2] * foodRotScaleB[mBiteCount];

  GFL_PRINT("CalcFoodRot_MotionB: %i BitCount:%i Rot: %f %f %f\n",frame,mBiteCount,dest.x,dest.y,dest.z);

}


float KawaigariPokemon::CalculateFoodRotationX(void)
{  
  f32 result = 0.0f;
  /*
  f32 cake_decline_x = 25.0f; // In Degrees!
  f32 scl = 1.0f - 0.5f * ( ( 4.0f - mBiteCount ) / 4.0f );

  result *= scl;
  result += cake_decline_x; */
  static float ROT_SIZE = 7.0f;
  result = gfl2::math::SinDeg(sinAng)*ROT_SIZE;

  if(sinAng<0.0f) result = 0.0f;
    
  
  /*
  f32 scaleV = gfl2::math::SinDeg( (int)sinAng%180)*-0.09f;
  scaleV += 1.0f;
  */
  static float SPEED1 = 4.0f;
  static float SPEED2 = 8.0f;
  static float SPEED3 = 16.0f;
  static float SPEED4 = 32.0f;

  static float SPEED_TIME1 = -10.0f;
  static float SPEED_TIME2 = 60.0f;
  static float SPEED_TIME3 = 120.0f;
       
  ///scale->Set(scaleV,scaleV,scaleV);
  
  if(sinAng<SPEED_TIME1)
    sinAng += SPEED1;  
  else if(sinAng<SPEED_TIME2)
    sinAng += SPEED2;  
  else if(sinAng<SPEED_TIME3)
    sinAng += SPEED3;  
  else
    sinAng += SPEED4;

  return(result);

}




// Only Sets Rotation for now:
void KawaigariPokemon::SetFoodPosition(bool rotate)
{
  static float SIN_START = -30.0f;
  
  if(!rotate)
  {
    gfl2::math::Vector3 *rot = coreSys->GetFoodRotation();

#ifdef USESANGOCALC
    rot->x = 0.0f;//CAKE_DECLINE_XROT;
#else
    rot->x = 0.0f;
#endif
    rot->y = 0.0f;    
    rot->z = 0.0f;
    sinAng = SIN_START;

  }
  else
  {

    CalculateFoodRotationX();
    

    gfl2::math::Vector3 *rot = coreSys->GetFoodRotation();

#ifdef USESANGOCALC
    gfl2::math::Vector3 sangoRot;
    CalculateFoodRotationSango(sangoRot);
    rot->x = gfl2::math::ConvRadToDeg( sangoRot.x );
    rot->y = gfl2::math::ConvRadToDeg( sangoRot.y );
    rot->z = gfl2::math::ConvRadToDeg( sangoRot.z );

    // Update Rotate Frame Motion!
    mShakeFrame++;
    if(mShakeFrame>10) mShakeFrame=10;
#else
    rot->y = 0.0f;//rotY;
    rot->x = rotY;
    rot->z = 0.0f;//rotY;
#endif
  }
}

void KawaigariPokemon::SetSlowMotion(bool enable)
{
  if(enable) mModel->SetAnimationStepFrame(0.5f);
  else mModel->SetAnimationStepFrame(1.0f);
}

void KawaigariPokemon::TurnOffSlowMotion(void)
{
  mModel->SetAnimationStepFrame(1.0f);
}

float KawaigariPokemon::GetOffsetEar(void)
{
  return(coreSys->GetDataManager()->GetOffsetEar());
}


float KawaigariPokemon::GetAdjustAngleEat(void)
{
  return(coreSys->GetDataManager()->GetAdjustAngleEat());
}

float KawaigariPokemon::GetAdjustAngleUpper(void)
{
  return(coreSys->GetDataManager()->GetAdjustAngleUpper());
}

float KawaigariPokemon::GetAdjustAngleLower(void)
{
  return(coreSys->GetDataManager()->GetAdjustAngleLower());
}

bool KawaigariPokemon::UseSlowMotion(void)
{
  return(coreSys->GetDataManager()->UseSlowMotion());
}


float KawaigariPokemon::GetAdjustAngleLook(void)
{
  return(coreSys->GetDataManager()->GetAdjustAngleLook());
}

int KawaigariPokemon::GetRestrictMotionLoveAmount(int m)
{ 
  return(coreSys->GetDataManager()->GetMotionAffectionRequired(m));  
}

int KawaigariPokemon::FoodAnimMax(void)
{
  ///return(5);
  return(coreSys->GetDataManager()->GetFoodAnimMax());
}

void KawaigariPokemon::PlayTouchSound(int touchColor)
{
  
  int se=KW_STROKE;
  f32 pitch=1.0f;
  switch(touchColor){
  case COL_SLUDGE_U:
  case COL_SLUDGE_D:	se = SEQ_SE_KW_STROKE4;
    break;

  case COL_GEL_U: case COL_GEL_D:	se = SEQ_SE_KW_STROKE3; break;

  case COL_GHOST_U: case COL_GHOST_D:	se = SEQ_SE_KW_STROKE2; break;
  
  case COL_HATE_U: case COL_HATE_D:
    {
      se = KW_STROKE;
      //pitch = 128-20;
      pitch = 1.0f - 20.0f/127.0f;
    }
    break;

  case COL_LIKE_U: case COL_LIKE_D:
    {
      se = KW_STROKE;
      //pitch = 128+20;
      pitch = 1.0f + 20.0f/127.0f;

    }
    break;

  default: se = KW_STROKE; break;
  }

  if( IsTouchSlide(4) && SEDonePlaying(se) )
  {    
    //PlaySE(se);
    if(se<SOUND_MAX)
      se = coreSys->GetDataManager()->GetSoundID(se);

    if(se == -1 || se == 0) return;

    Sound::PlaySE(se,0,1);
    Sound::ChangeSEPitch(se,pitch,1);
    
    
    ///_PlaySEPich( se, pitch );
  }
  
}

bool KawaigariPokemon::SEDonePlaying(int soundID)
{
  if(soundID<SOUND_MAX)
    soundID = coreSys->GetDataManager()->GetSoundID(soundID);

  if(soundID == -1 || soundID == 0) return(true);
  
  return(!Sound::IsSEPlaying(soundID,1));  
}

bool KawaigariPokemon::ChangeAnimation(int n,bool noEffect,bool smooth)
{
  GFL_PRINT("*Change Animation %i\n",n);



//  if(n != PokeTool::MODEL_ANIME_KW_WAIT)// && n!=PokeTool::MODEL_ANIME_EAT_A && n!=PokeTool::MODEL_ANIME_EAT_B && n!=PokeTool::MODEL_ANIME_EAT_C)
//  if(n != PokeTool::MODEL_ANIME_KW_WAIT || GetState()==AI_STATE_EAT)
/*  {  
    mModel->ResetFaceAnime_Eye();
    mModel->ResetFaceAnime_Mouth();
  }*/
  // This needs a huge exception for when motion is not available!
  /*
 if( mModel->GetAnimationType()==n && mModel->IsAnimationLoop()) return(true);
 
  if(smooth)
    mModel->ChangeAnimationSmooth( (PokeTool::MODEL_ANIME)n,KW_ANIMATION_SMOOTH_FRAME ,true);    
  else
    mModel->ChangeAnimation( (PokeTool::MODEL_ANIME)n,true);    
  
  if(n == PokeTool::MODEL_ANIME_KW_WAIT || n == PokeTool::MODEL_ANIME_DROWSE_B || n==PokeTool::MODEL_ANIME_SLEEP_A)
    mModel->SetAnimationLoop(true);
  else
    mModel->SetAnimationLoop(false);   

  setMotion = n;
  
  

  return(true);
  */
  if(setMotion>=PokeTool::MODEL_ANIME_EAT_A && setMotion<=PokeTool::MODEL_ANIME_EAT_C) setMotion=-1;
  if(setMotion == PokeTool::MODEL_ANIME_DROWSE_B) setMotion = -1;

  int mot = n;
  bool motExist=false;
  
  if(CheckMotionAvailable(mot) && GetRestrictMotionLoveAmount(mot) <= mAffection) 
  {       
    if(setMotion!=mot)
    {
      {  
        mModel->ResetFaceAnime_Eye();
        mModel->ResetFaceAnime_Mouth();
      }

      if(smooth)
        mModel->ChangeAnimationSmooth( (PokeTool::MODEL_ANIME)mot,KW_ANIMATION_SMOOTH_FRAME ,true);    
      else
        mModel->ChangeAnimation( (PokeTool::MODEL_ANIME)mot,true);    

      if(n == PokeTool::MODEL_ANIME_KW_WAIT  || n==PokeTool::MODEL_ANIME_SLEEP_A)
        mModel->SetAnimationLoop(true);
      else
        mModel->SetAnimationLoop(false);   
    }

    setMotion = mot;
    motExist = true;
  }
  else
  {
    motExist = false;
  }

  if(n == PokeTool::MODEL_ANIME_KW_WAIT)
    mModel->SetAnimationLoop(true);

  switch( mot )
  {
    case PokeTool::MODEL_ANIME_KW_WAIT:
    {
    }
    break;

    case PokeTool::MODEL_ANIME_TOUCH:
    {
      PlaySE(KW_SMACK);
    }
    break;

    case PokeTool::MODEL_ANIME_HAPPY_A:
    {
      if(!motExist) motExist = SetDelightMotion2();    
      if( !noEffect ) CallPlayerName( KW_DELIGHT1 );    
    }
    break;

    case PokeTool::MODEL_ANIME_HAPPY_B:
    {
      if( !noEffect ) CallPlayerName( KW_DELIGHT2 );    
    }
    break;

    case PokeTool::MODEL_ANIME_HAPPY_C:
    {	
      if( !motExist ) motExist = SetDelightMotion2();  
      if( !noEffect ) PlaySE( KW_DELIGHT3 );  
    }
    break;

    case PokeTool::MODEL_ANIME_QUESTION:
    {    
      if(!motExist)
      {
        // 記号で対応             
        if(setMotion!=PokeTool::MODEL_ANIME_KW_WAIT)
        {
          if(smooth)
            mModel->ChangeAnimationSmooth( PokeTool::MODEL_ANIME_KW_WAIT,KW_ANIMATION_SMOOTH_FRAME ,true);    
          else
            mModel->ChangeAnimation( PokeTool::MODEL_ANIME_KW_WAIT,true);    
        
          mModel->SetAnimationLoop(true);
        }
        setMotion = PokeTool::MODEL_ANIME_KW_WAIT;        
        coreSys->RequestCreateEffect(KAWAIGARI_EFFECT_QUESTIONMARK);
      }

      PlaySE( KW_NO_IDEA );
    }
    break;
    case PokeTool::MODEL_ANIME_MOVE_A :
    {
      if( motExist ) PlaySE (KW_HABIT1);  
    }
    break;

    case PokeTool::MODEL_ANIME_MOVE_B :
    {
      if( motExist ) PlaySE (KW_HABIT2);  
    }
    break;

    case PokeTool::MODEL_ANIME_MOVE_C :
    {
      CallPlayerName( KW_HABIT3 );
    }
    break;

    case PokeTool::MODEL_ANIME_PLAY_A :
    {
      if(!motExist) motExist = SetDelightMotion2();      
      PlaySE(KW_CARE1);
    }
    break;

    case PokeTool::MODEL_ANIME_PLAY_B :
    {
      if(!motExist) motExist = SetDelightMotion2();    
      CallPlayerName(KW_CARE2);
    }
    break;

    case PokeTool::MODEL_ANIME_EAT_C :
    {
    }
    break;

    case PokeTool::MODEL_ANIME_MAD:
    {
      if( motExist && !noEffect) PlaySE(KW_ANGER);
    }              
    break;

    case PokeTool::MODEL_ANIME_HATE:
    {
      if( motExist ) 
      {
        if( mMonsNo == MONSNO_TYOBOMAKI )
        {
          // Reset Head Adjust Angle: (STOP POLYGON OVERLAP)
          mModel->ResetInterestTagAdjustAngle();
          ChangeFace( TEX_MOUTH, FACE_MOUTH_DEFAULT, true ); // チョボマキ                           
        }
        if( !noEffect ) PlaySE(KW_HATE);        
      }
    }
    break;

    case PokeTool::MODEL_ANIME_LONELY:
    {
      if( motExist ) 
      {
        if( coreSys->GetRandom(2) > 0 ) PlaySE(KW_LONELY);        
        else PlaySpecialVoice(SPECIAL_VOICE_SADDNLY);        
      }
    }
    break;

    case PokeTool::MODEL_ANIME_DROWSE_B:
    {
      if(!motExist)
      {
        mot = PokeTool::MODEL_ANIME_KW_WAIT;
        
        if(setMotion!=PokeTool::MODEL_ANIME_KW_WAIT)
        {        
          if(smooth)
            mModel->ChangeAnimationSmooth( PokeTool::MODEL_ANIME_KW_WAIT,KW_ANIMATION_SMOOTH_FRAME ,true);    
          else
            mModel->ChangeAnimation( PokeTool::MODEL_ANIME_KW_WAIT,true);    

          mModel->SetAnimationLoop(true);
        }
        setMotion = PokeTool::MODEL_ANIME_KW_WAIT;                
        ChangeFace( TEX_EYE, FACE_EYE_EYECLOSE, true );
        ///ChangeFace( TEX_EYE, FACE_EYE_BLINK, true );
        
      }

      if( !noEffect )
      {
        ////SetParticle( GARC_love_eff_k_ibiki_BCPTL , 0, 0, true, MODEL_NODE_OVERHEAD );

        if(IsCharacterID(MONSNO_TESSIIDO))
        {          
          if( (mSleepSoundTimer % 90) == 0) PlaySE(KW_ROW);          
          mSleepSoundTimer++;
        }
        else PlaySE(KW_ROW);
      }
    }
    break;

    case PokeTool::MODEL_ANIME_DROWSE_C:
    {
      if(!motExist)
      {
        ChangeFace( TEX_EYE, FACE_EYE_DEFAULT );
      }
      ///SetParticle( GARC_love_eff_exclamation_r , 0, 0, true, MODEL_NODE_OVERHEAD );      
      if(!noEffect) coreSys->CreateEffect(KAWAIGARI_EFFECT_EXCLAMATIONMARK);
      if(!noEffect) PlaySpecialVoice( SPECIAL_VOICE_WAKEUP );
    }
    break;

    case PokeTool::MODEL_ANIME_TURN_B:
    {
      if(!motExist)
      {        
        mot = PokeTool::MODEL_ANIME_TURN_A;

        if(CheckMotionAvailable(mot))
        {          
          if(smooth)
            mModel->ChangeAnimationSmooth( (PokeTool::MODEL_ANIME)mot,KW_ANIMATION_SMOOTH_FRAME ,true);    
          else
            mModel->ChangeAnimation( (PokeTool::MODEL_ANIME)mot,true);    

          setMotion = mot;  
          motExist = true;
        }
        else motExist = false;        

      }
    }
    break;
  
  }
  
  setMotion = n;

  return motExist;
  
}

void KawaigariPokemon::AnimatonUtil_PokeShader(int frame,int startFrame,int endFrame,float r,float g,float b,float startStrength,float endStrength)
{
  float strength = 0.0f;

  if(frame==startFrame) strength = startStrength;
  else if(frame==endFrame) strength = endStrength;
  else if(frame<startFrame || frame>endFrame) return;

  float dist = endStrength - startStrength;
  float per = (float)(frame - startFrame ) / (endFrame - startFrame);

  strength = startStrength + (dist * per);
  gfl2::math::Vector4 col( r,g,b,strength);
  mModel->SetShaderColor(col);
}

void KawaigariPokemon::AnimationUtil_PokeVisibility(int frame,int startFrame,bool visible)
{
  if(frame==startFrame) mModel->SetVisible(visible);
}

void KawaigariPokemon::AnimationUtil_MonsterBallVisibility(int frame,int startFrame,bool visible)
{
  if(frame==startFrame) coreSys->GetMonsterBall()->SetVisible(visible);
}

void KawaigariPokemon::AnimationUtil_PokeScale(int frame,int startFrame,int endFrame,float startScale,float endScale)
{  
  if(frame<startFrame || frame>endFrame) return;
  
  if(startFrame == endFrame)
  {
    mModel->SetScale(endScale,endScale,endScale);
    return;
  }

  float dist = endScale - startScale;
  float per = (float)(frame - startFrame ) / (endFrame - startFrame);
  float scale = startScale + (dist * per);

  mModel->SetScale(scale,scale,scale);

}

void KawaigariPokemon::AnimationUtil_EffectScale(KawaigariObject_Effect *obj,int frame,int startFrame,int endFrame,float startScale,float endScale)
{
  if(frame<startFrame || frame>endFrame) return;

  if(startFrame == endFrame)
  {
    obj->SetScale(endScale,endScale,endScale);
    return;
  }

  float dist = endScale - startScale;
  float per = (float)(frame - startFrame ) / (endFrame - startFrame);
  float scale = startScale + (dist * per);

  obj->SetScale(scale,scale,scale);
}

void KawaigariPokemon::AnimationUtil_PokeMove(int frame,int startFrame,int endFrame,float sx,float sy,float sz,float tx,float ty,float tz)
{
  if(frame<startFrame || frame>endFrame) return;

  float per = (float)(frame - startFrame ) / (endFrame - startFrame);

  float distx = tx - sx;  
  float x = sx + (distx * per);

  float disty = ty - sy;  
  float y = sy + (disty * per);

  float distz = tz - sz;  
  float z = sz + (distz * per);

  mModel->SetPosition(x,y,z);

}

  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)