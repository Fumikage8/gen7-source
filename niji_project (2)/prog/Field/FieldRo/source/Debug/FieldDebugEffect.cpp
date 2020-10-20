//======================================================================
/**
 * @file FieldDebugEffect.cpp
 * @date 2015/08/28 12:53:12
 * @author miyachi_soichi
 * @brief デバッグ：エフェクト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

// gfl2
#include <Debug/include/gfl2_DebugWinItemTypes.h>
// GameSys
#include "GameSys/include/GameManager.h"
// field
#include "Field/FieldRo/include/Fieldmap.h"
// MoveModel
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"
// Effect
#include "Field/FieldRo/include/Effect/FieldEffectManager.h"
#include "Field/FieldRo/include/Effect/FieldEffectBase.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectExclamation2.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectFade.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectDashSmoke.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectSharkRun.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectSharkWait.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectTrainerEncount.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectRideLight.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectFadeSpot.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectBallDemo.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectRotom.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectFestivalFire.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectKairikyRockSmoke.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectKairikyRockDown.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectGetItem.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectTrial5.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectDenjumokuThunder.h"
// self
#include "Field/FieldRo/include/Debug/FieldDebugEffect.h"
// メッセージのインデックス
#include "niji_conv_header/message/debug/msg_debug_menu.h"
#include "niji_conv_header/sound/SoundMiddleID.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Local Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// エフェクト生成
static void PlayEffectExclamation1( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugEffect *pDebug = reinterpret_cast<DebugEffect*>( userWork );

  pDebug->CheckCurrentEffect( Field::Effect::EFFECT_TYPE_EXCLAMATION_ICON1_OVERLAY, false );
  Field::Effect::IEffectBase *pEffect = pDebug->GetEffect();
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pDebug->RequestPlayEffect( Field::Effect::EFFECT_TYPE_EXCLAMATION_ICON1_OVERLAY );
  }
}
static void PlayEffectExclamation2( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugEffect *pDebug = reinterpret_cast<DebugEffect*>( userWork );

  pDebug->CheckCurrentEffect( Field::Effect::EFFECT_TYPE_EXCLAMATION_ICON2_OVERLAY, false );
  Field::Effect::IEffectBase *pEffect = pDebug->GetEffect();
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pDebug->RequestPlayEffect( Field::Effect::EFFECT_TYPE_EXCLAMATION_ICON2_OVERLAY );
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_Y ) )
  {
    pDebug->RequestPlayEffect( Field::Effect::EFFECT_TYPE_EXCLAMATION_ICON2_OVERLAY );
    pEffect = pDebug->GetEffect();
    Effect::EffectExclamation2* pExc =reinterpret_cast<Effect::EffectExclamation2*>( pEffect );
    pExc->SetMode( Effect::EffectExclamation2::MODE_WAIT );
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_X ) && pEffect )
  {
    Effect::EffectExclamation2* pExc =reinterpret_cast<Effect::EffectExclamation2*>( pEffect );
    pExc->RequestClose();
  }
}
static void PlayEffectQuestion( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugEffect *pDebug = reinterpret_cast<DebugEffect*>( userWork );

  pDebug->CheckCurrentEffect( Field::Effect::EFFECT_TYPE_QUESTION_ICON_OVERLAY, true );
  if( !pButton->IsTrigger( gfl2::ui::BUTTON_A ) ){ return; }
  pDebug->RequestPlayEffect( Field::Effect::EFFECT_TYPE_QUESTION_ICON_OVERLAY );
}
static void PlayEffectActionButton( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugEffect *pDebug = reinterpret_cast<DebugEffect*>( userWork );

  pDebug->CheckCurrentEffect( Field::Effect::EFFECT_TYPE_ACTION_BUTTON, false );
  if( !pButton->IsTrigger( gfl2::ui::BUTTON_A ) ){ return; }
  pDebug->RequestPlayEffect( Field::Effect::EFFECT_TYPE_ACTION_BUTTON );
}
static void PlayEffectFade( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugEffect *pDebug = reinterpret_cast<DebugEffect*>( userWork );

  pDebug->CheckCurrentEffect( Field::Effect::EFFECT_TYPE_FADE, false );
  Field::Effect::IEffectBase *pEffect = pDebug->GetEffect();
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pDebug->RequestPlayEffect( Field::Effect::EFFECT_TYPE_FADE );
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_X ) && pEffect )
  {
    Field::Effect::EffectFade* pFade = reinterpret_cast<Field::Effect::EffectFade*>( pEffect );
    if( pFade )
    {
      pFade->RequestIn( 30 );
    }
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_Y ) && pEffect )
  {
    Field::Effect::EffectFade* pFade = reinterpret_cast<Field::Effect::EffectFade*>( pEffect );
    if( pFade )
    {
      gfl2::math::Vector4 start( 0.0f, 0.0f, 0.0f, 0.0f );
      gfl2::math::Vector4 end( 0.0f, 0.0f, 0.0f, 255.0f );
      pFade->RequestOut( &start, &end, 30 );
    }
  }
}
static void PlayEffectDashSmoke( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugEffect *pDebug = reinterpret_cast<DebugEffect*>( userWork );

  pDebug->CheckCurrentEffect( Field::Effect::EFFECT_TYPE_DASH_SMOKE, false );
  Field::Effect::IEffectBase *pEffect = pDebug->GetEffect();
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pDebug->RequestPlayEffect( Field::Effect::EFFECT_TYPE_DASH_SMOKE );
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_X ) && pEffect )
  {
    Field::Effect::EffectDashSmoke* pSmoke = reinterpret_cast<Field::Effect::EffectDashSmoke*>( pEffect );
    Field::MoveModel::FieldMoveModel *pPlayer = pGameManager->GetFieldmap()->GetPlayerCharacter();
    gfl2::math::Vector3 vPos_foot;
    poke_3d::model::BaseModel *pBaseModel = pPlayer->GetCharaDrawInstance();
    vPos_foot = pBaseModel->GetPosition();

    pSmoke->CreateSmoke( vPos_foot );
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_Y ) && pEffect )
  {
    //Field::Effect::EffectDashSmoke* pSmoke = reinterpret_cast<Field::Effect::EffectDashSmoke*>( pEffect );
    //bool bVisible = pSmoke->IsVisibleRush();
    //pSmoke->SetVisibleRush( !bVisible );
  }
}
static void PlayEffectSharkRun( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugEffect *pDebug = reinterpret_cast<DebugEffect*>( userWork );

  pDebug->CheckCurrentEffect( Field::Effect::EFFECT_TYPE_SHARK_RUN, false );
  Field::Effect::IEffectBase *pEffect = pDebug->GetEffect();
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pDebug->RequestPlayEffect( Field::Effect::EFFECT_TYPE_SHARK_RUN );
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_Y ) && pEffect )
  {
    //Field::Effect::EffectSharkRun* pSmoke = reinterpret_cast<Field::Effect::EffectSharkRun*>( pEffect );
    //bool bVisible = pSmoke->IsVisible();
    //pSmoke->SetVisible( !bVisible );
  }
}
static void PlayEffectSharkWait( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugEffect *pDebug = reinterpret_cast<DebugEffect*>( userWork );

  pDebug->CheckCurrentEffect( Field::Effect::EFFECT_TYPE_SHARK_WAIT, false );
  Field::Effect::IEffectBase *pEffect = pDebug->GetEffect();
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pDebug->RequestPlayEffect( Field::Effect::EFFECT_TYPE_SHARK_WAIT );
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_Y ) && pEffect )
  {
    Field::Effect::EffectSharkWait* pSmoke = reinterpret_cast<Field::Effect::EffectSharkWait*>( pEffect );
    bool bVisible = pSmoke->IsVisible();
    pSmoke->SetVisible( !bVisible );
  }
}
static void PlayEffectSharkJet( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugEffect *pDebug = reinterpret_cast<DebugEffect*>( userWork );

  pDebug->CheckCurrentEffect( Field::Effect::EFFECT_TYPE_SHARK_JET, false );
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pDebug->RequestPlayEffect( Field::Effect::EFFECT_TYPE_SHARK_JET );
  }
}
static void PlayEffectPlaceName( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pGameManager->GetFieldmap()->GetEffectManager()->RequestStartPlaceName( true );
  }
}
static void PlayEffectTrainerEncount( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugEffect *pDebug = reinterpret_cast<DebugEffect*>( userWork );

  pDebug->CheckCurrentEffect( Field::Effect::EFFECT_TYPE_TRAINER_ENCOUNT, false );
  Field::Effect::IEffectBase *pEffect = pDebug->GetEffect();
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pDebug->RequestPlayEffect( Field::Effect::EFFECT_TYPE_TRAINER_ENCOUNT );
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_X ) && pEffect )
  {
    gfl2::math::VEC2 pos(0,0);
    Field::Effect::EffectTrainerEncount *pEncount = reinterpret_cast<Field::Effect::EffectTrainerEncount*>( pEffect );
    pEncount->StartAnimation( pos, 0.0f, SEQ_SE_FLD_SHISEN_IN_RANGE );
  }
}
static void PlayEffectFadeSpot( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugEffect *pDebug = reinterpret_cast<DebugEffect*>( userWork );

  pDebug->CheckCurrentEffect( Field::Effect::EFFECT_TYPE_FADE_SPOT, false );
  Field::Effect::IEffectBase *pEffect = pDebug->GetEffect();
  gfl2::math::VEC2 pos( 150.0f, -15.0f );
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pDebug->RequestPlayEffect( Field::Effect::EFFECT_TYPE_FADE_SPOT );
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_X ) && pEffect )
  {
    Field::Effect::EffectFadeSpot *pFadeSpot = reinterpret_cast<Field::Effect::EffectFadeSpot*>( pEffect );
    pFadeSpot->StartAnimation( pos, 30.0f, false );
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_Y ) && pEffect )
  {
    Field::Effect::EffectFadeSpot *pFadeSpot = reinterpret_cast<Field::Effect::EffectFadeSpot*>( pEffect );
    pFadeSpot->StartAnimation( pos, 15.0f, true );
  }
}
static void PlayEffectTrialStart( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugEffect *pDebug = reinterpret_cast<DebugEffect*>( userWork );

  pDebug->CheckCurrentEffect( Field::Effect::EFFECT_TYPE_TRIAL_START, true );
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pDebug->RequestPlayEffect( Field::Effect::EFFECT_TYPE_TRIAL_START );
  }
}
static void PlayEffectBallDemo( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugEffect *pDebug = reinterpret_cast<DebugEffect*>( userWork );

  pDebug->CheckCurrentEffect( Field::Effect::EFFECT_TYPE_B_DEMO, false );
  Field::Effect::IEffectBase *pEffect = pDebug->GetEffect();
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pDebug->RequestPlayEffect( Field::Effect::EFFECT_TYPE_B_DEMO );
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_X ) && pEffect )
  {
    Field::Effect::EffectBallDemo *pFadeSpot = reinterpret_cast<Field::Effect::EffectBallDemo*>( pEffect );
    pFadeSpot->Start();
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_Y ) && pEffect )
  {
    Field::Effect::EffectBallDemo *pFadeSpot = reinterpret_cast<Field::Effect::EffectBallDemo*>( pEffect );
    pFadeSpot->End();
  }
}
static void PlayEffectRotom( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugEffect *pDebug = reinterpret_cast<DebugEffect*>( userWork );

  pDebug->CheckCurrentEffect( Field::Effect::EFFECT_TYPE_ROTOM, true );

  Field::Effect::IEffectBase *pEffect = pDebug->GetEffect();
  u32 eEffectType = pDebug->GetEffectType();
  if( pEffect && eEffectType == Field::Effect::EFFECT_TYPE_ROTOM)
  {
    // 既にロトム絵が出ている場合は次の絵に遷移
    if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
    {
      if( pEffect)
      {
        Field::Effect::EffectRotom *pRotom = reinterpret_cast<Field::Effect::EffectRotom*>( pEffect );
        pRotom->NextAnimation();
      }
    }
  }
  else
  {
    // まだロトム絵が出ていない場合はエフェクト生成
    if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
    {
      pDebug->RequestPlayEffect( Field::Effect::EFFECT_TYPE_ROTOM );
    }
  }
}
static void PlayEffectFootWater( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugEffect *pDebug = reinterpret_cast<DebugEffect*>( userWork );

  pDebug->CheckCurrentEffect( Field::Effect::EFFECT_TYPE_FOOT_WATER, false );
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pDebug->RequestPlayEffect( Field::Effect::EFFECT_TYPE_FOOT_WATER );
  }
}
static void PlayEffectFootSand( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugEffect *pDebug = reinterpret_cast<DebugEffect*>( userWork );

  pDebug->CheckCurrentEffect( Field::Effect::EFFECT_TYPE_FOOT_SAND, false );
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pDebug->RequestPlayEffect( Field::Effect::EFFECT_TYPE_FOOT_SAND );
  }
}
static void PlayEffectFootLand( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugEffect *pDebug = reinterpret_cast<DebugEffect*>( userWork );

  pDebug->CheckCurrentEffect( Field::Effect::EFFECT_TYPE_FOOT_LAND, false );
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pDebug->RequestPlayEffect( Field::Effect::EFFECT_TYPE_FOOT_LAND );
  }
}
static void PlayEffectFootRideKenta( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugEffect *pDebug = reinterpret_cast<DebugEffect*>( userWork );

  pDebug->CheckCurrentEffect( Field::Effect::EFFECT_TYPE_FOOT_RIDE_KENTA, false );
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pDebug->RequestPlayEffect( Field::Effect::EFFECT_TYPE_FOOT_RIDE_KENTA );
  }
}
static void PlayEffectFootRideMurando( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugEffect *pDebug = reinterpret_cast<DebugEffect*>( userWork );

  pDebug->CheckCurrentEffect( Field::Effect::EFFECT_TYPE_FOOT_RIDE_MURANDO, false );
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pDebug->RequestPlayEffect( Field::Effect::EFFECT_TYPE_FOOT_RIDE_MURANDO );
  }
}
static void PlayEffectFootRideKairiky( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugEffect *pDebug = reinterpret_cast<DebugEffect*>( userWork );

  pDebug->CheckCurrentEffect( Field::Effect::EFFECT_TYPE_FOOT_RIDE_KAIRIKY, false );
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pDebug->RequestPlayEffect( Field::Effect::EFFECT_TYPE_FOOT_RIDE_KAIRIKY );
  }
}
static void PlayEffectFootRideZimenba( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugEffect *pDebug = reinterpret_cast<DebugEffect*>( userWork );

  pDebug->CheckCurrentEffect( Field::Effect::EFFECT_TYPE_FOOT_RIDE_ZIMENBA, false );
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pDebug->RequestPlayEffect( Field::Effect::EFFECT_TYPE_FOOT_RIDE_ZIMENBA );
  }
}
static void PlayEffectFesFire( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugEffect *pDebug = reinterpret_cast<DebugEffect*>( userWork );

  pDebug->CheckCurrentEffect( Field::Effect::EFFECT_TYPE_FESTIVAL_FIRE, false );
  Field::Effect::IEffectBase *pEffect = pDebug->GetEffect();
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pDebug->RequestPlayEffect( Field::Effect::EFFECT_TYPE_FESTIVAL_FIRE );
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_X ) && pEffect )
  {
    Field::Effect::EffectFestivalFire *pFadeSpot = reinterpret_cast<Field::Effect::EffectFestivalFire*>( pEffect );
    pFadeSpot->Start();
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_Y ) && pEffect )
  {
    Field::Effect::EffectFestivalFire *pFadeSpot = reinterpret_cast<Field::Effect::EffectFestivalFire*>( pEffect );
    pFadeSpot->End();
  }
}
static void PlayEffectRockSmoke( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugEffect *pDebug = reinterpret_cast<DebugEffect*>( userWork );

  pDebug->CheckCurrentEffect( Field::Effect::EFFECT_TYPE_KAIRIKY_ROCK_SMOKE, false );
  Field::Effect::IEffectBase *pEffect = pDebug->GetEffect();
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pDebug->RequestPlayEffect( Field::Effect::EFFECT_TYPE_KAIRIKY_ROCK_SMOKE );
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_X ) && pEffect )
  {
    Field::Effect::EffectKairikyRockSmoke *pFadeSpot = reinterpret_cast<Field::Effect::EffectKairikyRockSmoke*>( pEffect );
    pFadeSpot->Start();
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_Y ) && pEffect )
  {
    Field::Effect::EffectKairikyRockSmoke *pFadeSpot = reinterpret_cast<Field::Effect::EffectKairikyRockSmoke*>( pEffect );
    pFadeSpot->EndKill();
  }
}
static void PlayEffectRockDown( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugEffect *pDebug = reinterpret_cast<DebugEffect*>( userWork );

  pDebug->CheckCurrentEffect( Field::Effect::EFFECT_TYPE_KAIRIKY_ROCK_DOWN, false );
  Field::Effect::IEffectBase *pEffect = pDebug->GetEffect();
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pDebug->RequestPlayEffect( Field::Effect::EFFECT_TYPE_KAIRIKY_ROCK_DOWN );
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_X ) && pEffect )
  {
    Field::Effect::EffectKairikyRockDown *pFadeSpot = reinterpret_cast<Field::Effect::EffectKairikyRockDown*>( pEffect );
    pFadeSpot->Start();
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_Y ) && pEffect )
  {
    Field::Effect::EffectKairikyRockDown *pFadeSpot = reinterpret_cast<Field::Effect::EffectKairikyRockDown*>( pEffect );
    pFadeSpot->End();
  }
}
static void PlayEffectCharaBreath( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugEffect *pDebug = reinterpret_cast<DebugEffect*>( userWork );

  pDebug->CheckCurrentEffect( Field::Effect::EFFECT_TYPE_CHARA_BREATH, false );
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pDebug->RequestPlayEffect( Field::Effect::EFFECT_TYPE_CHARA_BREATH );
  }
}

static void PlayEffectGetItemCommon( u32 index,  void *userWork, gfl2::debug::DebugWinItem *item  )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugEffect *pDebug = reinterpret_cast<DebugEffect*>( userWork );

  Field::Effect::Type type = static_cast<Field::Effect::Type>(Field::Effect::EFFECT_TYPE_GET_ITEM_1 + index);
  pDebug->CheckCurrentEffect( type, true );

  Field::Effect::IEffectBase *pEffect = pDebug->GetEffect();
  u32 eEffectType = pDebug->GetEffectType();
  if( pEffect && eEffectType == type )
  {
    // 既に出ている場合は次の絵に遷移
    if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
    {
      if( pEffect)
      {
        Field::Effect::EffectGetItem *pGetItem= reinterpret_cast<Field::Effect::EffectGetItem*>( pEffect );
        u32 animeIndex = pGetItem->GetIndex() + 1;
        if( animeIndex >= Field::Effect::EffectGetItem::INDEX_MAX)
        {
          animeIndex = 0;
        }
        pGetItem->Change( animeIndex);
      }
    }
  }
  else
  {
    // まだ出ていない場合はエフェクト生成
    if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
    {
      pDebug->RequestPlayEffect( type );
      Field::Effect::EffectGetItem *pGetItem= reinterpret_cast<Field::Effect::EffectGetItem*>( pDebug->GetEffect() );
      pGetItem->Start( 0);
    }
  }
}
static void PlayEffectGetItem1( void *userWork, gfl2::debug::DebugWinItem *item )
{
  PlayEffectGetItemCommon( 0, userWork, item);
}
static void PlayEffectGetItem2( void *userWork, gfl2::debug::DebugWinItem *item )
{
  PlayEffectGetItemCommon( 1, userWork, item);
}
static void PlayEffectGetItem3( void *userWork, gfl2::debug::DebugWinItem *item )
{
  PlayEffectGetItemCommon( 2, userWork, item);
}
static void PlayEffectGetItem4( void *userWork, gfl2::debug::DebugWinItem *item )
{
  PlayEffectGetItemCommon( 3, userWork, item);
}
static void PlayEffectDenjumokuThunder(void *userWork, gfl2::debug::DebugWinItem *item)
{
  GameSys::GameManager*    pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugEffect *pDebug = reinterpret_cast<DebugEffect*>(userWork);

  pDebug->CheckCurrentEffect(Field::Effect::EFFECT_TYPE_EF_PH0301_DEN, false);
  Field::Effect::IEffectBase *pEffect = pDebug->GetEffect();
  if (pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    pDebug->RequestPlayEffect(Field::Effect::EFFECT_TYPE_EF_PH0301_DEN);
  }
  else if (pButton->IsTrigger(gfl2::ui::BUTTON_X) && pEffect)
  {
    Field::Effect::EffectDenjumokuThunder *pDenThunder = reinterpret_cast<Field::Effect::EffectDenjumokuThunder*>(pEffect);
    pDenThunder->Start();
  }
  else if (pButton->IsTrigger(gfl2::ui::BUTTON_Y) && pEffect)
  {
    Field::Effect::EffectDenjumokuThunder *pDenThunder = reinterpret_cast<Field::Effect::EffectDenjumokuThunder*>(pEffect);
    pDenThunder->End();
  }

}


/**
 *  @brief  Group ctor
 */
static void CreateDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugEffect *pDebug = reinterpret_cast<DebugEffect*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    gfl2::str::MsgData *pMsgData = pDebug->GetMessageData();
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_effect_excl1,       pDebug, PlayEffectExclamation1,   NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_effect_excl2,       pDebug, PlayEffectExclamation2,   NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_effect_quest,       pDebug, PlayEffectQuestion,       NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_effect_act_btn,     pDebug, PlayEffectActionButton,   NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_effect_fade,        pDebug, PlayEffectFade,           NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_effect_dash_smoke,  pDebug, PlayEffectDashSmoke,      NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_effect_shark_run,   pDebug, PlayEffectSharkRun,       NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_effect_shark_wait,  pDebug, PlayEffectSharkWait,      NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_effect_shark_jet,   pDebug, PlayEffectSharkJet,       NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_effect_place_name,  pDebug, PlayEffectPlaceName,      NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_effect_trainer_enc, pDebug, PlayEffectTrainerEncount, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_effect_fade_spot,   pDebug, PlayEffectFadeSpot,       NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_effect_trial_start, pDebug, PlayEffectTrialStart,     NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_effect_rotom,       pDebug, PlayEffectRotom,          NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_effect_b_demo,      pDebug, PlayEffectBallDemo,       NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_effect_foot_water,  pDebug, PlayEffectFootWater,      NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_effect_foot_sand,   pDebug, PlayEffectFootSand,       NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_effect_foot_land,   pDebug, PlayEffectFootLand,       NULL );

    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_effect_footride_kenta,   pDebug, PlayEffectFootRideKenta,       NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_effect_footride__murando,   pDebug, PlayEffectFootRideMurando,       NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_effect_footride__kairiky,   pDebug, PlayEffectFootRideKairiky,       NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_effect_footride__zimenba,   pDebug, PlayEffectFootRideZimenba,       NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_effect_fesfire,   pDebug, PlayEffectFesFire,       NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_effect_rock_smoke,   pDebug, PlayEffectRockSmoke,     NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_effect_rock_down,   pDebug, PlayEffectRockDown,      NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_effect_chara_breath,   pDebug, PlayEffectCharaBreath,      NULL );

    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_effect_get_item_1, pDebug, PlayEffectGetItem1, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_effect_get_item_2, pDebug, PlayEffectGetItem2, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_effect_get_item_3, pDebug, PlayEffectGetItem3, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_effect_get_item_4, pDebug, PlayEffectGetItem4, NULL );

    gfl2::debug::DebugWin_AddItemUpdateFunc(pGroup, pHeap, pMsgData, msg_dmenu_field_effect_denjumoku_thunder, pDebug, PlayEffectDenjumokuThunder, NULL);

    pDebug->ClearDynamicLoadControl();
  }
}
/**
 *  @brief  Group dtor
 */
static void DestroyDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  DebugEffect * pEffect = reinterpret_cast<DebugEffect *>(userWork);
  if( pGroup )
  {
    pGroup->RemoveAllItem();


    // 読み込んだ非常駐リソースを破棄
    pEffect->UnloadAllDynamicLoadControl();
  }
}

/**
 *  @class  DebugEffect
 *  @brief  デバッグ：エフェクト
 */
/**
 *  @brief  コンストラクタ
 */
DebugEffect::DebugEffect( void )
{
}

/**
 *  @brief  デストラクタ
 */
DebugEffect::~DebugEffect( void )
{
  Terminate();
}
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  初期化
 */
void DebugEffect::Initialize( gfl2::heap::HeapBase *pHeap, Field::Effect::EffectManager *pEffectManager, gfl2::str::MsgData* pMsgData )
{
  m_pHeap = pHeap;
  m_pEffectManager = pEffectManager;
  m_pEffect = NULL;
  m_eEffectType = Field::Effect::EFFECT_TYPE_ERROR;
  m_pMsgData = pMsgData;
}

/**
 *  @brief  解放
 */
void DebugEffect::Terminate( void )
{
}

/**
 *  @brief  メニューの作成
 */
void DebugEffect::CreateDebugMenu( gfl2::debug::DebugWinGroup *root )
{
  gfl2::debug::DebugWinGroup *pGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_field_effect_top, root );
  pGroup->SetCreateDestroyGroupFunc( this, CreateDebugGroupFunc, DestroyDebugGroupFunc );
}

/**
 *  @brief  再生中のエフェクトチェック
 */
void DebugEffect::CheckCurrentEffect( u32 type, bool bCheckAnim )
{
  // ないのでチェックしない
  if( !m_pEffect ){ return; }

  if( m_eEffectType != type )
  {
    // 指定タイプではない
    m_pEffectManager->DeleteEffect( m_pEffect );
    m_pEffect = NULL;
    m_eEffectType = Field::Effect::EFFECT_TYPE_ERROR;
  }
  else if( m_pEffect->IsAnimationLastFrame() && bCheckAnim )
  {
    // 再生が終了している
    m_pEffectManager->DeleteEffect( m_pEffect );
    m_pEffect = NULL;
    m_eEffectType = Field::Effect::EFFECT_TYPE_ERROR;
  }
}
/**
 *  @brief  エフェクトの再生
 */
void DebugEffect::RequestPlayEffect( u32 type )
{
  // 再生中のものがあれば破棄しておく
  if( m_pEffect )
  {
    m_pEffectManager->DeleteEffect( m_pEffect );

    m_pEffect = NULL;
    m_eEffectType = Field::Effect::EFFECT_TYPE_ERROR;
  }

  // プレイヤーとその座標
  GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  Field::MoveModel::FieldMoveModel *pPlayer = pGameManager->GetFieldmap()->GetPlayerCharacter();
  gfl2::math::Vector3 vPos_zero = gfl2::math::Vector3::GetZero();
  gfl2::math::Vector3 vPos_foot;
  gfl2::math::Vector3 vPos_head;
  {
    poke_3d::model::BaseModel *pBaseModel = pPlayer->GetCharaDrawInstance();
    vPos_foot = pBaseModel->GetPosition();
    s32 joint_index = pBaseModel->GetJointIndex( "CamTgtHead" );
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *pNode = pBaseModel->GetModelInstanceNode();
    gfl2::renderingengine::scenegraph::instance::JointInstanceNode  *pJointNode = pNode->GetJointInstanceNode( joint_index );
    vPos_head = pJointNode->GetWorldMatrix().GetElemPosition();
  }

  // 読み込み
  this->loadDynaicLoadResource( static_cast<Field::Effect::Type>(type) );

  // 各エフェクトごと
  switch( type )
  {
  case Field::Effect::EFFECT_TYPE_EXCLAMATION_ICON1_OVERLAY:
    m_pEffect = m_pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_EXCLAMATION_ICON1_OVERLAY, pPlayer, true );
    m_eEffectType = type;
    break;
  case Field::Effect::EFFECT_TYPE_EXCLAMATION_ICON2_OVERLAY:
    m_pEffect = m_pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_EXCLAMATION_ICON2_OVERLAY, pPlayer, true );
    m_eEffectType = type;
    break;
  case Field::Effect::EFFECT_TYPE_QUESTION_ICON_OVERLAY:
    m_pEffect = m_pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_QUESTION_ICON_OVERLAY, pPlayer, true );
    m_eEffectType = type;
    break;
  case Field::Effect::EFFECT_TYPE_ACTION_BUTTON:
    m_pEffect = m_pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_ACTION_BUTTON, vPos_head );
    m_eEffectType = type;
    break;
  case Field::Effect::EFFECT_TYPE_FADE:
    m_pEffect = m_pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_FADE, vPos_head );
    m_eEffectType = type;
    break;
  case Field::Effect::EFFECT_TYPE_DASH_SMOKE:
    m_pEffect = m_pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_DASH_SMOKE, pPlayer );
    m_eEffectType = type;
    break;
  case Field::Effect::EFFECT_TYPE_SHARK_RUN:
    m_pEffect = m_pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_SHARK_RUN, pPlayer );
    m_eEffectType = type;
    break;
  case Field::Effect::EFFECT_TYPE_SHARK_WAIT:
    m_pEffect = m_pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_SHARK_WAIT, pPlayer );
    m_eEffectType = type;
    break;
  case Field::Effect::EFFECT_TYPE_SHARK_JET:
    m_pEffect = m_pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_SHARK_JET, pPlayer );
    m_eEffectType = type;
    break;
  case Field::Effect::EFFECT_TYPE_TRAINER_ENCOUNT:
    m_pEffect = m_pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_TRAINER_ENCOUNT, vPos_zero );
    m_eEffectType = type;
    break;
  case Field::Effect::EFFECT_TYPE_FADE_SPOT:
    m_pEffect = m_pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_FADE_SPOT, vPos_zero );
    m_eEffectType = type;
    break;
  case Field::Effect::EFFECT_TYPE_TRIAL_START:
    m_pEffect = m_pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_TRIAL_START, vPos_zero, true );
    m_eEffectType = type;
    break;
  case Field::Effect::EFFECT_TYPE_B_DEMO:
    m_pEffect = m_pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_B_DEMO, vPos_head, true );
    m_eEffectType = type;
    break;
  case Field::Effect::EFFECT_TYPE_ROTOM:
    m_pEffect = m_pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_ROTOM, vPos_zero );
    m_eEffectType = type;
    break;
  case Field::Effect::EFFECT_TYPE_FOOT_WATER:
    m_pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_FOOT_WATER, vPos_foot );
    m_eEffectType = type;
    break;
  case Field::Effect::EFFECT_TYPE_FOOT_SAND:
    m_pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_FOOT_SAND, vPos_foot );
    m_eEffectType = type;
    break;
  case Field::Effect::EFFECT_TYPE_FOOT_LAND:
    m_pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_FOOT_LAND, pPlayer );
    m_eEffectType = type;
    break;
  case Field::Effect::EFFECT_TYPE_FOOT_RIDE_KENTA:
    m_pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_FOOT_RIDE_KENTA, pPlayer );
    m_eEffectType = type;
    break;
  case Field::Effect::EFFECT_TYPE_FOOT_RIDE_MURANDO:
    m_pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_FOOT_RIDE_MURANDO, pPlayer );
    m_eEffectType = type;
    break;
  case Field::Effect::EFFECT_TYPE_FOOT_RIDE_KAIRIKY:
    m_pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_FOOT_RIDE_KAIRIKY, pPlayer );
    m_eEffectType = type;
    break;
  case Field::Effect::EFFECT_TYPE_FOOT_RIDE_ZIMENBA:
    m_pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_FOOT_RIDE_ZIMENBA, pPlayer );
    m_eEffectType = type;
    break;
  case Field::Effect::EFFECT_TYPE_FESTIVAL_FIRE:
    m_pEffect = m_pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_FESTIVAL_FIRE, vPos_foot );
    m_eEffectType = type;
    break;
  case Field::Effect::EFFECT_TYPE_KAIRIKY_ROCK_SMOKE:
    m_pEffect = m_pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_KAIRIKY_ROCK_SMOKE, vPos_foot );
    m_eEffectType = type;
    break;
  case Field::Effect::EFFECT_TYPE_KAIRIKY_ROCK_DOWN:
    m_pEffect = m_pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_KAIRIKY_ROCK_DOWN, vPos_foot );
    m_eEffectType = type;
    break;
  case Field::Effect::EFFECT_TYPE_CHARA_BREATH:
    m_pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_CHARA_BREATH, pPlayer );
    m_eEffectType = type;
    break;
  case Field::Effect::EFFECT_TYPE_GET_ITEM_1:
    m_pEffect = m_pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_GET_ITEM_1, vPos_zero );
    m_eEffectType = type;
    break;
  case Field::Effect::EFFECT_TYPE_GET_ITEM_2:
    m_pEffect = m_pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_GET_ITEM_2, vPos_zero );
    m_eEffectType = type;
    break;
  case Field::Effect::EFFECT_TYPE_GET_ITEM_3:
    m_pEffect = m_pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_GET_ITEM_3, vPos_zero );
    m_eEffectType = type;
    break;
  case Field::Effect::EFFECT_TYPE_GET_ITEM_4:
    m_pEffect = m_pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_GET_ITEM_4, vPos_zero );
    m_eEffectType = type;
    break;
  case Field::Effect::EFFECT_TYPE_EF_PH0301_DEN:
    m_pEffect = m_pEffectManager->CreateEffect(Field::Effect::EFFECT_TYPE_EF_PH0301_DEN, vPos_foot );
    m_eEffectType = type;
    break;
  }
}



/** 
 *  @brief 非常駐リソース読み込み　コントロール　初期化
 */
void DebugEffect::ClearDynamicLoadControl( void )
{
  for( u32 i=0; i<Effect::EFFECT_TYPE_MAX; ++i )
  {
    m_IsLoad[ i ] = false;
  }
}


/** 
 *  @brief 非常駐リソース読み込み　コントロール　リソース全破棄
 */
void DebugEffect::UnloadAllDynamicLoadControl( void )
{
  for( u32 i=0; i<Effect::EFFECT_TYPE_MAX; ++i )
  {
    if( m_IsLoad[i] )
    {
      m_pEffectManager->UnLoadDynamicEffectResource( static_cast<Field::Effect::Type>(i), m_pHeap );
    }
  }
}

// 読み込み
void DebugEffect::loadDynaicLoadResource( Effect::Type type )
{
  if( !m_pEffectManager->IsLoadedEffectResource( type ) )
  {
    gfl2::heap::HeapBase * pheap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

    m_pEffectManager->LoadDynamicEffectResource( type, pheap, true );
    m_pEffectManager->SetupDynamicEffectResource( type, pheap );
    m_IsLoad[ type ] = true;
  }
}

// 破棄
void DebugEffect::unloadDynamicLoadResource( Effect::Type type )
{
  if( m_IsLoad[type] )
  {
    gfl2::heap::HeapBase * pheap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
    m_pEffectManager->UnLoadDynamicEffectResource( type, pheap );
    m_IsLoad[type] = false;
  }
}


GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // PM_DEBUG
