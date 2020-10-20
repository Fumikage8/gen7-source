//======================================================================
/**
 * @file	KawaigariPokemon.h
 * @brief	Kawaigari Pokemon Actor/AI Object
 * @author	PETE
 * @data	2015.10.22
 */
//======================================================================

#if !defined __KAWAIGARI_POKEMON_H_INCLUDED__
#define __KAWAIGARI_POKEMON_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <gfx/include/gfl2_types.h>

#define USESANGOCALC (1)
#define CAKE_DECLINE_XROT (25.0f)


class PokeTool::PokeModel;

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

class KawaigariMain;
class KawaigariTroubleObject;
class KawaigariObject_Effect;

const int WAIT_SECOND = 30;
const int KW_ANIMATION_SMOOTH_FRAME = 8;
const int KW_ANIMATION_SMOOTH_FRAME_LOVELOOK = 8;

const int PLAYFUL_LEVEL1 = 150;
const int FULLNESS_LEVEL1 = 150;

const int AFFECTION_LEVEL220 = 220;
const int AFFECTION_LEVEL1 = 150;
const int AFFECTION_LEVEL2 = 100;
const int AFFECTION_LEVEL3 = 50;
const int AFFECTION_LEVEL4 = 3;

const int PLAYABLE_TOUCH_HIGHTOUCH_VALUE = -2; // SANGO=10

const int PLAYABLE_IDLE_FIRST = -2; // SANGO=10

const int BRANCH_TOUCH_HIGHFIVE_LOVEAMOUNT = 5;
const int BRANCH_FOOD_FULLNESS_SLOW = 230;

const int MAX_POKEMAME_TYPE = 15;
const int MAX_POKEMAME_VARIATION = 5+1;
const int MAX_POKEMAME_MODEL = MAX_POKEMAME_VARIATION * MAX_POKEMAME_TYPE;//6;  ADD ONE MORE PIECE! 5 Normal 1 Piece!
const int MAX_POKEMAME_PIECE_MODEL_SET = 3; // 3 Pieces Max
const int MAX_POKEMAME_PIECE_MODEL = MAX_POKEMAME_PIECE_MODEL_SET * MAX_POKEMAME_TYPE; // 3 Pieces Max * Types

const int MAX_STATE_REGS = 10;

enum
{
  SPECIAL_VOICE_DEEPSLEEPING = 0, 
  SPECIAL_VOICE_DEEPWAKEUP,
  SPECIAL_VOICE_BADFEEL,
  SPECIAL_VOICE_SADDNLY,
  SPECIAL_VOICE_WAKEUP,
  SPECIAL_VOICE_VILLAINOUS,
};

// Touch ID Color:
enum TouchIDColor
{
  TOUCH_COLOR_NONE = 0,
  TOUCH_COLOR_BODY = 1,
  TOUCH_COLOR_HAND_L = 2,
};

// Touch Icon
enum
{
  ICON_ID_TAP = 0,					//!< タップ
  ICON_ID_HIT,							//!< ヒット
  ICON_ID_STOP_POINT,				//!< 指カーソル（アニメなし）
  ICON_ID_ANIME_POINT,			//!< 指カーソル（アニメあり）
  ICON_ID_OPEN_HAND,				//!< 手カーソル（アニメなし）
  ICON_ID_ANIME_HAND,				//!< 手カーソル（アニメあり）
  ICON_ID_SHOCK,						//!< ビリビリ
  ICON_ID_STOP_BECKON,			//!< 手招き（アニメなし）
  ICON_ID_ANIME_BECKON,			//!< 手招き（アニメあり）
  ICON_ID_FADE_BECKON_30F,	//!< 手招き（30frmのフェードアニメ）
  ICON_ID_FADE_BECKON_15F,	//!< 手招き（15frmのフェードアニメ）
  ICON_ID_WATER,						//!< ゆらゆらアイコン（アニメあり）
  ICON_ID_FREEZE,						//!< 氷アイコン（アニメあり）
  ICON_ID_JUNK,							//!< ぐちゃぐちゃアイコン（アニメあり）
  ICON_ID_BURN_HAND,				//!< 火傷した時の手（アニメあり）
  ICON_ID_HURT_HAND,				//!< トゲなどを触った時の手（アニメあり）
  ICON_ID_FILTH_00,					//!< 痕１（アニメあり）
  ICON_ID_FILTH_01,					//!< 痕２（アニメあり）
  ICON_ID_FILTH_02,					//!< 痕３（アニメあり）
  ICON_ID_FILTH_03,					//!< 痕４（アニメあり）
  ICON_ID_FILTH_04,					//!< 痕５（アニメあり）
  ICON_ID_GHOST,						//!< ゴーストタイプ（アニメあり）
  ICON_ID_MAX
};

// Touch ID Types:
enum {
  COL_BG, // 0
  COL_BODY, // 1
  COL_EAR_L, // 2
  COL_EAR_R, // 3
  COL_EYE_L_U, // 4
  COL_EYE_L_D, // 5
  COL_EYE_R_U, // 6
  COL_EYE_R_D, // 7
  COL_FACE, // 8
  COL_CHIN, // 9
  COL_HAND_L, // 10
  COL_HURTS_U, // 11
  COL_HURTS_D, // 12
  COL_GHOST_U, // 13
  COL_GHOST_D, // 14
  COL_BURN_U, // 15
  COL_BURN_D, // 16
  COL_THUNDER_U, // 17
  COL_THUNDER_D, // 18
  COL_GEL_U, // 19
  COL_GEL_D, // 20
  COL_SLUDGE_U, // 21
  COL_SLUDGE_D, // 22
  COL_COLD_U, // 23
  COL_COLD_D, // 24
  COL_HATE_U, // 25
  COL_HATE_D, // 26
  COL_LIKE_U, // 27
  COL_LIKE_D, // 28
  COL_GHOST_HATE_U,
  COL_GHOST_HATE_D,
  COL_GHOST_LIKE_U,
  COL_GHOST_LIKE_D,
  COL_GEL_HATE_U,
  COL_GEL_HATE_D,
  COL_GEL_LIKE_U,
  COL_GEL_LIKE_D,
  COL_SLUDGE_HATE_U,
  COL_SLUDGE_HATE_D,
  COL_SLUDGE_LIKE_U,
  COL_SLUDGE_LIKE_D,
};

enum 
{
  TOUCH_EAR_L,
  TOUCH_EAR_R,
  TOUCH_EAR_MAX,
};

// Face Tex Type:
enum 
{
  TEX_OTHER,
  TEX_EYE,
  TEX_MOUTH,
  TEX_MAX,
};

// Face Setting Types:
enum
{
  FACE_EYE_DEFAULT,
  FACE_EYE_BLINK,
  FACE_MOUTH_DEFAULT,
  FACE_MOUTH_LOOK,
  FACE_MOUTH_LISTEN,
  FACE_EYE_TOUCHU,
  FACE_MOUTH_TOUCHU,
  FACE_EYE_TOUCHD,
  FACE_MOUTH_TOUCHD1,
  FACE_MOUTH_TOUCHD2,
  FACE_EYE_TOUCH_LIKE,
  FACE_MOUTH_TOUCH_LIKE,
  FACE_EYE_TOUCH_HATE,
  FACE_MOUTH_TOUCH_HATE,
  FACE_EYE_FULL,
  FACE_MOUTH_FULL,
  FACE_EYE_ELECTRIC,
  FACE_MOUTH_ELECTRIC,
  FACE_EYE_ICE,
  FACE_MOUTH_ICE,
  FACE_EYE_EYECLOSE,
};

enum
{
  AI_STATE_INIT= 0,
  AI_STATE_MAIN,

  AI_STATE_IDLE,  
  AI_STATE_APPEAR,
  AI_STATE_TOUCH,
  AI_STATE_EAT,
  AI_STATE_CARE,
  AI_STATE_CAREWAIT, // Waiting for care...
  AI_STATE_CAMERAWAIT, // Waiting for camera set
  AI_STATE_TUTORIAL, // Do Start Tutorial Info
  AI_STATE_END,
  AI_STATE_FADEEND,
#if PM_DEBUG
  AI_STATE_PLAY_ALL_MOTION,  
#endif
  AI_STATE_CLEANUP,
  AI_STATE_FINISH,
};

// Appear States:
enum AI_SUBSTATE_APPEAR
{
  AI_SUBSTATE_INIT= 0,  
  AI_SUBSTATE_APPEAR_SICK,      
  AI_SUBSTATE_APPEAR_UPPER,    
  AI_SUBSTATE_APPEAR_LOWER,    
  AI_SUBSTATE_APPEAR_BEHIND,    
  AI_SUBSTATE_APPEAR_LEFT,    
  AI_SUBSTATE_APPEAR_RIGHT,    
  AI_SUBSTATE_APPEAR_DO_UP,    
  AI_SUBSTATE_APPEAR_BACK,    
  AI_SUBSTATE_APPEAR_DROWSEY,    
  AI_SUBSTATE_APPEAR_SLEEP,    
  AI_SUBSTATE_APPEAR_LOOKAROUND,    
  AI_SUBSTATE_APPEAR_PLAYMOTION,  
  AI_SUBSTATE_APPEAR_FINISH,  
  AI_SUBSTATE_APPEAR_NONE,  
};

// Idle States:
enum AI_SUBSTATE_IDLE
{
  AI_SUBSTATE_IDLE_INIT= 0,  
  AI_SUBSTATE_IDLE_MAIN,      
  AI_SUBSTATE_IDLE_PLAYMOTION,    
  AI_SUBSTATE_IDLE_LOOKAROUND,      
  AI_SUBSTATE_IDLE_SLEEP,      
};

// Touch States:
enum AI_SUBSTATE_TOUCH
{
  AI_SUBSTATE_TOUCH_INIT= 0,  
  AI_SUBSTATE_TOUCH_MAIN,  
  AI_SUBSTATE_TOUCH_BODYTOUCH,  
  AI_SUBSTATE_TOUCH_HITANGER,    
};

// Eat States:
enum AI_SUBSTATE_EAT
{
  AI_SUBSTATE_EAT_INIT= 0,  
  AI_SUBSTATE_EAT_BITE,  
  AI_SUBSTATE_EAT_THINK,  
  AI_SUBSTATE_EAT_TRY,  
  AI_SUBSTATE_EAT_FULL,  
  AI_SUBSTATE_EAT_HATE,  
  AI_SUBSTATE_EAT_FALL,  
  AI_SUBSTATE_EAT_BEGIN,  
  AI_SUBSTATE_EAT_IDLE,  
  AI_SUBSTATE_EAT_WAIT,  
};

// Care States:
enum AI_SUBSTATE_CARE
{
  AI_SUBSTATE_CARE_INIT = 0,
  AI_SUBSTATE_CARE_MAIN,
  AI_SUBSTATE_CARE_WAIT,
};

// Group Types:
enum {
  GRP_SPECIAL,
  GRP_GIANT,
  GRP_FAIRY,
  GRP_FAIRYF,
  GRP_LAND,
  GRP_LANDF,
  GRP_FLY,
  GRP_FLYF,
  GRP_GHOST,
  GRP_DRAGON,
  GRP_DRAGONF,
  GRP_SNAKE,
  GRP_ROCK,
  GRP_ROCKF,
  GRP_MONSTER,
  GRP_MONSTERF,
  GRP_HUMAN,
  GRP_HUMANF,
  GRP_PLANT,
  GRP_PLANTF,
  GRP_WATER1,
  GRP_WATER1F,
  GRP_WATER2,
  GRP_WATER2F,
  GRP_BLOB,
  GRP_BLOB2,
  GRP_INSECT,
  GRP_INSECTF,
  GRP_WATER3,
  GRP_METAMON,
  GRP_WHALEOH,
  GRP_SAMEREACT,
  GRP_NOREACT,
  GRP_MAX
};

enum {
  TM_NONE,
  TM_BODY,
  TM_EAR,
  TM_EYE,
  TM_FACE,
  TM_HATE,
  TM_LIKE,
};

enum
{
  CARETOOL_NONE = -1,
  CARETOOL_BANDAGE = 0,
  CARETOOL_HAIR,
  CARETOOL_DIRT,
  CARETOOL_DUST,
  CARETOOL_WATER,
};

class KawaigariPokemon
{

public:
  KawaigariPokemon(void);
  ~KawaigariPokemon(void);
  void SetCoreSys(KawaigariMain *core) { coreSys = core; }
  void SetPokeModel(PokeTool::PokeModel *model) { mModel = model; }
  void SetMonsterID(int monsNo,int formNo);

  void SetInterestPosition(const gfl2::math::Vector3* pPosition,float speed = 0.0125f);

  void Update(void);

  bool CanChangeMode(void);

  void TouchTest(void);
  void CallPlayerName(int soundID);
  void PlaySE(int soundID,int controlID=0);
  void StopSE(int soundID,int controlID=0);
  void PlayCareSE(int soundID);
  bool SEDonePlaying(int soundID);
  void PlaySpecialVoice(int soundID);

  // State Functions:
  int GetState(void) { return(mStateReg); }
  int GetSubState(void) { return(mStateRegSub); }
  void SetState(int s);
  void SetSubState(int s);
  void SaveState(void);
  void RestoreState(void);
  void PrintState(void);
  
  // Main States:
  void State_Init(void);
  void State_Main(void);  
  void State_CareWait(void);
  void State_Tutorial(void);
  void State_CameraWait(void);
  void State_End(void);  
  void State_FadeEnd(void);  
#if PM_DEBUG
  void State_PlayAllMotion(void);
#endif
  void State_Cleanup(void);
  void State_Finish(void);

  // Idle States:
  void State_Idle(void);
  void State_Idle_Init(void);
  void State_Idle_Main(void);
  void State_Idle_PlayMotion(void);
  void State_Idle_LookAround(void);
  void State_Idle_Sleep(void);

  // Appear State:
  void State_Appear(void);
  void State_Appear_Sick(void);    
  void State_Appear_Upper(void);
  void State_Appear_Lower(void);
  void State_Appear_Behind(void);
  void State_Appear_Left(void);
  void State_Appear_Right(void);
  void State_Appear_Do_Up(void);
  void State_Appear_Back(void);
  void State_Appear_Drowsey(void);
  void State_Appear_Sleep(void);
  void State_Appear_LookAround(void);
  void State_Appear_PlayMotion(void);
  void State_Appear_Finish(void);

  // Touch State: (Touching Pokemon)
  void State_Touch(void);
  void State_Touch_Init(void);
  void State_Touch_Main(void);
  void State_Touch_BodyTouch(void);
  void State_Touch_HitAnger(void);  

  // Eat State: (Food)
  void State_Eat(void);
  void State_Eat_Init(void);
  void State_Eat_Bite(void);
  void State_Eat_Think(void);
  void State_Eat_Try(void);
  void State_Eat_Full(void);
  void State_Eat_Hate(void);
  void State_Eat_Fall(void);
  void State_Eat_Begin(void);
  void State_Eat_Idle(void);
  void State_Eat_Wait(void);

  // Care State:
  void State_Care(void);
  void State_Care_Init(void);
  void State_Care_Main(void);
  void State_Care_Wait(void);
    
  // Logic Choosing Functions:
  int ChooseAppearState(void);

  // Helper Functions:
  void Func_LookAtTouchPosition(void);
  void Func_ResetLook(void);
  bool Func_SetDelightMotion(void);

  // Check Functions:
  bool CanSetMotion(int n);
  bool CanCallPlayerName(void);
  bool CanSleepFirstTime(void);

  bool CheckGroup(int n);
  
  bool IsSick(void);
  bool IsCalledByMonster(void);
  bool IsIgnoredTooLong(void);
  bool IsTutorialShow(void);

  bool IsCharacterID(int num);
  int GetMonsNo(void) { return(mMonsNo); }
  int GetFormNo(void) { return(mFormNo); }
  
  // Animation Util:
  void AnimatonUtil_PokeShader(int frame,int startFrame,int endFrame,float r,float g,float b,float startStrength,float endStrength);
  void AnimationUtil_PokeVisibility(int frame,int startFrame,bool visible);
  void AnimationUtil_PokeScale(int frame,int startFrame,int endFrame,float startScale,float endScale);
  void AnimationUtil_PokeMove(int frame,int startFrame,int endFrame,float sx,float sy,float sz,float tx,float ty,float tz);

  void AnimationUtil_MonsterBallVisibility(int frame,int startFrame,bool visible);

  void AnimationUtil_EffectScale(KawaigariObject_Effect *obj,int frame,int startFrame,int endFrame,float startScale,float endScale);


  // Touch:
  bool IsTouch(void); // Touch?
  bool IsTouchStart(void); // Start touch?
  bool IsTouching(void); // Currently Touching?
  bool IsTouchRelease(void); // Released Touching?
  bool IsTouchSlide(int speed); // Sliding Touch?
  void SetTouchIcon(int type,bool flag=true);
  void SetTouchIcon2(int type,bool flag=true) {}


  float GetAppearYPosition(void);
  int GetFullnessLevel(void);
  void AddFullnessLevel(int n);

  int GetPlayfulLevel(void);
  int GetAffectionLevel(void);
  int GetStartAffectionLevel(void) { return(mStartAffection); }
  int GetTouchColor(void);
  int GetMinuteHighFiveValue(void);

  void SetCurrentMotion(int n);
  bool CheckMotion(int n) { return(setMotion==n); }
  void ChangeToCurrentMotion(void);
  void SetPhysics(bool enable);

  // Set Love Motion:
  bool ChangeAnimationNotSmooth(int n,bool noEffect=false);
  bool ChangeAnimation(int n,bool noEffect=false,bool smooth=true);



  bool ChangeFace(int texType,int faceType,bool force=false);
  void SetFace(int texType,int faceType);

  void LookAtFood(void);
  void LookAwayFromFood(void);
  void LookAtTouch(void);
  bool WaitEating(void);
  bool CheckEating(void);
  
  void CalculateFoodRotationSango(gfl2::math::Vector3 &dest);
  void CalculateFoodRotationSango_MotionB(gfl2::math::Vector3 &dest,int frame);
  float CalculateFoodRotationX(void);
  void SetFoodPosition(bool rotate=false);

  // Logic Helpers: (From LoveMotion)
  void Eat_FallProgram_Init(void);
  bool Eat_FallProgram_Update(void);
  
  void Eat_BiteProgram_Init(int motion);
  bool Eat_BiteProgram_Update(int motion);
  void Eat_BiteProgram_Finish(void);

  void LookEnd(void);
  bool CheckFoodRange(void);

  int GetFullStuffedValue(void) { return(230); }
  int FoodAnimMax(void);
  

  bool UseSlowMotion(void);// FROM DATA (OK)
  void SetSlowMotion(bool enable);
  void TurnOffSlowMotion(void);

  float GetOffsetEar(void);// FROM DATA (OK)
  float GetAdjustAngleLower(void);// FROM DATA (OK)
  float GetAdjustAngleUpper(void);// FROM DATA (OK)
  float GetAdjustAngleEat(void);// FROM DATA (OK)
  float GetAdjustAngleLook(void);// FROM DATA (OK)
  int GetRestrictMotionLoveAmount(int m); // FROM DATA! (OK)
  
  void PlayTouchSound(int touchColor);
  void SetAdjustHeadAngle(float ang);
  void UpdateSpringOffset(float *springOffset,float *tagOffset,float rate=0.05f);
  void SetSpringOffsetY(int n,float val);

  bool CheckMotionAvailable(int m);
  bool SetDelightMotion2(void);
  bool SetDelightMotion(void);

  void AddAffectionAmount(int n,bool capAmount=true); 
  void RemoveAffectionAmount(int n);
  void AddPlayfulnessAmount(int n,bool playSound=true);
  void RemovePlayfulnessAmount(int n);
  void SetFullness(int n) { mFullness = n;}

  float GetFoodRotScale(int n);
  void SetFoodRotScale(float f,int n);
  void GetFoodRotVal(int frame,float &x,float &z);
  void SetFoodRotVal(float x,float z,int frame);


  // Heart Effect Amount:
  void SetJustLoveAmountEffect(void);
  int GetJustLoveAmountIndex(void);    
  void SetJustLoveMotion(int index);
  int GetJustLoveMotionPlayCount(int index);

  // Parameter Setting:
  void SetParameters(int affectionV,int playfulV,int fullnessV);
  void GetParameters(u8 &affectionV,u8 &playfulV,u8 &fullnessV);
  void SetParametersByMode(void);

  // Care Mode:
  void SetCareToolType(int t) { mCareToolType=t; }
  int GetCareToolType(void) { return(mCareToolType); }

  // Appear State:
  void SetForceAppearState(int n) { mForceAppearState=n; }

  void SetIsSick(bool b) { mSick = b; }
  
  void SetIsDualScreenMonster(bool b) { mIsDualScreenMonster = b; }
  void SetDualScreenMonsterCamera(int mode,bool force=false);
  void SetupDualMonsterCamera(void);
  bool IsDualScreenMonster(void) { return(mIsDualScreenMonster); }

  void GetCurrentTouchPos(float &x,float &y) { x = (float)touchX; y = (float)touchY; }

  void SetTouchOK(bool b) { mTouchOK = b; }

private:

  void UpdateAgain(void) { mRequireUpdate = true; }  
  bool mRequireUpdate;

  bool ConvertTouchTypeToPane(int type,int &pane);
  bool ConvertTouchTypeToAnimation(int type,int &ani);

  PokeTool::PokeModel *mModel;
  KawaigariMain *coreSys;

  // State:  
  int mStateReg,mStateRegSub,mStateRegs[MAX_STATE_REGS];
  int mStateRegBackup,mStateRegSubBackup,mStateRegsBackup[MAX_STATE_REGS];

  int currentMotion,lastMotion,setMotion;
  int mSleepEffect;  

  gfl2::math::Vector3 lookPos;

  int lastTouchPane,lastTouchAni;

  bool mCanSleepFirstTime; // First Time Sleep OK?

  int touchX,touchY;  
  int lastTouchX,lastTouchY;
  bool mDontTouchMe;
  int mTouchFrame;
  int mShakeFrame;

  int mMonsNo,mFormNo;

  int mAffection,mPlayfulness,mFullness;
  int mStartAffection;

  // Food State Related:  
  bool mFallMode; 
  bool mFell;
  int mBiteCount;
  int mFallCatch;
  bool mFallCompleted;
  bool mRevert;
  f32 mFoodRotateX;
  gfl2::math::Vector3 mFallSpeed;
  gfl2::math::Vector3 mPieceVelocity[MAX_POKEMAME_PIECE_MODEL];
  gfl2::math::Vector3 mPieceAng[MAX_POKEMAME_PIECE_MODEL];
  gfl2::math::Vector3 mPieceRotation[MAX_POKEMAME_PIECE_MODEL];
  
  // Body Touch State Related:
  int mTouchUpperFrame,mTouchLowerFrame,mTouchOutFrame;
  int mEyes,mMouth,mExitFlag;
  f32 mTagSpringOffset[2];
  f32 mSpringOffset[2];
  int mBiri2Count;
  int mTouchAllFrame,mTouchChangeFrame,mTouchMode,mTouchTiredMode,mEyeTouchFrame;
  bool mNoIcon,mSuccess,mFail;
  int mAddLoveAmount,mAddPlayable,mTouchTiredCount;
  bool mIcon2Flag;

  bool mStopLook;
  bool mSick;
  
  // Heart Amount Effect Related:
  bool mLoveAmountOnce[5];

  // Appear State:
  int mForceAppearState;

  // Camera:
  gfl2::math::Vector3 mCamPos,mCamTgt;

  // Care Mode:
  int mCareToolType;
  bool mCryOnce; // Play Special Voice only one time..

  // Facial Expression:
  void UpdateBlink(void);
  int GetBlink(void);
  int mFacialExpressionWait[ TEX_MAX ];
  int mFacialExpression[ TEX_MAX ];
  int mBlinkFrame;

  bool mIsDualScreenMonster;
  bool mTouchStart;
  bool mTouchOK;
  int mTouchOKCount;
  bool mCanCatchFood;
  bool mForceEatFinish;
  
  int mSleepSoundTimer;
};

  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)

#endif // __KAWAIGARI_Pokemon_H_INCLUDED__
