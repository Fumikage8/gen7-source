//======================================================================
/**
 * @file Snow.cpp
 * @date 2015/10/20 17:20:18
 * @author saito_nozomu
 * @brief 天候　雪
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

#include "Field/FieldRo/include/Weather/Snow.h"
#include "Field/FieldRo/include/Weather/BillBoardfade.h"
// system
#include "System/include/GflUse.h"

#include "Field/FieldRo/include/Effect/FieldEffectManager.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectSnow.h"
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"

#include "Field/FieldRo/include/Collision/BaseCollisionScene.h"
#include "Sound/include/Sound.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(weather)

static const float SNOW_EMITTER_LENGTH = 800;


Snow::Snow(gfl2::heap::HeapBase* pHeap) :WeatherBase()
{
  m_pHeap = pHeap;
  m_pSnowFade = GFL_NEW(m_pHeap) BillBoardFade();
  m_pEffectManager = NULL;
  m_pCameraManager = NULL;
  m_generate = false;
  m_generate_margine = 0;
  m_pSnowEff = NULL;
  m_pCollisionSceneContainer = NULL;
  for(int i=0; i<SNOW_EFF_TBL_MAX;i++)
  {
    m_pEffectTable[i] = NULL;
  }
}

Snow::~Snow()
{
  if (m_pSnowFade != NULL ) GFL_SAFE_DELETE(m_pSnowFade);
}

void Snow::Initialize(InitParam &param)
{
  m_pEffectManager = param.pEffectManager;
  m_pCameraManager = param.pCameraManager;
  m_pCollisionSceneContainer = param.pCollisionSceneContainer; 
  m_generate = false;
  m_generate_margine = 0;
  for(int i=0; i<SNOW_EFF_TBL_MAX;i++)
  {
    m_pEffectTable[i] = NULL;
  }
  m_pSnowEff = NULL;

  m_Type = WEATHER_NONE;
}

/**
  * @brief セットアップ
  */
bool Snow::SetupFunc(void)
{
  m_FadeOutSeq = 0;
  //天候作成
  Camera::CameraUnit* pCameraUnit = m_pCameraManager->GetMainGamePlayCamera();
  gfl2::math::Vector3 pos = pCameraUnit->GetBaseCamera()->GetPosition();
  gfl2::math::Quaternion rot = pCameraUnit->GetBaseCamera()->GetRotationQuat();
  gfl2::math::Vector3 dir( 0.0f, 0.0f, -1.0f );
  dir = rot.Transform( dir );

  gfl2::math::Vector3 emitter_pos = pos + (dir.Normalize() * SNOW_EMITTER_LENGTH);
  Effect::IEffectBase *pEffect = m_pEffectManager->CreateEffect(Effect::EFFECT_TYPE_SNOW, emitter_pos);
  Effect::EffectSnow *eff = static_cast<Effect::EffectSnow*>(pEffect);

  m_pSnowEff = eff;
  if (m_pSnowEff == NULL )
  {
    return true;
  }
  
  switch(m_Type)
  {
  case WEATHER_SNOW:
    eff->CreateSkyEffect(Effect::EFFECT_RESOURCE_SKY_SNOW, emitter_pos, SNOW_EMITTER_LENGTH);
    m_pSnowFade->Initialize( m_pSnowEff->GetSnowModel(), 0 );
    break;
  case WEATHER_SNOWSTORM:
    eff->CreateSkyEffect(Effect::EFFECT_RESOURCE_SKY_SNOWSTORM, emitter_pos, SNOW_EMITTER_LENGTH);
    m_pSnowFade->Initialize( m_pSnowEff->GetSnowModel(), 0 );
    m_pSnowEff->GetSnowModel()->SetVisible(true);
    break;
  case WEATHER_DIAMONDDUST:
    eff->CreateSkyEffect(Effect::EFFECT_RESOURCE_SKY_DIAMONDDUST, emitter_pos, SNOW_EMITTER_LENGTH);
    m_pSnowFade->Initialize( m_pSnowEff->GetSnowModel(), 0 );
    break;
  }
  m_pSnowFade->SetVisible(false);

  return true;
}

void Snow::StartFunc(void)
{
  if (m_pSnowEff == NULL )
  {
    return;
  }

  //ダイヤモンドダストなら生成を開始。
  if (m_Type == WEATHER_DIAMONDDUST)
  {
    m_generate = true;
  }
  m_pSnowFade->SetVisible(false);
}

void Snow::MainFunc(void)
{
  if (m_pSnowEff == NULL )
  {
    return;
  }

  //ビルボードのフェードインフェードアウト処理
  m_pSnowFade->Update();

  //エフェクトのデリート処理
  for (int i=0;i<SNOW_EFF_TBL_MAX;i++)
  {
    if( m_pEffectTable[i] != NULL)
    {
      if ( m_pEffectTable[i]->IsDead() )
      {
        ///GFL_PRINT("%d kill\n",i);
        m_pEffectManager->DeleteEffect( m_pEffectTable[i] );
        m_pEffectTable[i] = NULL;
      }
    }
  }

  Camera::CameraUnit* pCameraUnit = m_pCameraManager->GetMainGamePlayCamera();
  gfl2::math::Vector3 pos = pCameraUnit->GetBaseCamera()->GetPosition();
  gfl2::math::Quaternion rot = pCameraUnit->GetBaseCamera()->GetRotationQuat();
  gfl2::math::Vector3 dir( 0.0f, 0.0f, -1.0f );
  dir = rot.Transform( dir );
  gfl2::math::Vector3 emitter_pos = pos + (dir.Normalize() * SNOW_EMITTER_LENGTH);

  //エミッターの位置を更新
  if (m_pSnowEff != NULL)
  {
    m_pSnowEff->UpdatePosition( pos, dir.Normalize() );
  }

  //ダイヤモンドダストを抽選して生成
  if (m_generate)
  {
    m_generate_margine++;
    if (m_generate_margine <= 16)
    {
      return;
    }
    m_generate_margine = 0;
    for (int i=0;i<SNOW_EFF_TBL_MAX;i++)
    {
      if( m_pEffectTable[i] == NULL)
      {
        int x = System::GflUse::GetPublicRand( 800 ) - 400;
        int z = System::GflUse::GetPublicRand( 800 ) - 400;

        gfl2::math::Vector3 particle_pos;
        //高さ取得
        RaycastCustomCallback::HIT_DATA hitData;
        gfl2::math::Vector4              startVec (emitter_pos.x+x, 20000, emitter_pos.z+z, 0.0f);
        gfl2::math::Vector4              endVec   (emitter_pos.x+x, -20000, emitter_pos.z+z, 0.0f);
        // レイが地面に当たったかどうか
        if( m_pCollisionSceneContainer->RaycastFromStaticActorsMesh( startVec,endVec,&hitData ) )
        {
          particle_pos.x = hitData.intersection.x;
          particle_pos.y = hitData.intersection.y;
          particle_pos.z = hitData.intersection.z;
        }
        else
        {
          break;
        }
        Effect::IEffectBase *pEffect = m_pEffectManager->CreateEffect(Effect::EFFECT_TYPE_SNOW, particle_pos);
        if (pEffect == NULL)
        {
          NOZOMU_PRINT("SNOW_EFFECT_EMPTY\n");
          break; //もう空きがない
        }
        Effect::EffectSnow *eff = static_cast<Effect::EffectSnow*>(pEffect);
        eff->CreateGroundDiamondDust(particle_pos);
        m_pEffectTable[i] = eff;
        ///GFL_PRINT("%d create\n",i);
        break;
      }
    }
  }
}

void Snow::StopFunc(void)
{
  //生成を停止し、終了リクエストを発効
  m_generate = false;
}

bool Snow::FadeOutFunc(void)
{
  switch(m_FadeOutSeq)
  {
  case 0:
    FadeoutWeatherRequest(m_Type, WEATHER_NONE);
    m_FadeOutSeq++;
    break;
  case 1:
    if ( IsFinishedFadeOut(m_Type) )
    {
      m_FadeOutSeq++;
    }
    break;
  case 2:
    if (m_pSnowEff != NULL)
    {
      m_pEffectManager->DeleteEffect( m_pSnowEff );
      m_pSnowEff = NULL;
    }

    int count = 0;
    for (int i=0;i<SNOW_EFF_TBL_MAX;i++)
    {
      if( m_pEffectTable[i] != NULL)
      {
        if ( m_pEffectTable[i]->IsDead() )
        {
          ///GFL_PRINT("%d kill\n",i);
          m_pEffectManager->DeleteEffect( m_pEffectTable[i] );
          m_pEffectTable[i] = NULL;
        }
      }
      else
      {
        count++;
      }
    }

    if (count >= SNOW_EFF_TBL_MAX)
    {
      m_Type = WEATHER_NONE;
      return true;
    }
  }

  return false;
}

/**
 * @brief ＳＥ再生
 */
void Snow::StartSE(void)
{
  switch(m_Type)
  {
  case WEATHER_SNOW:
    Sound::PlaySE(SEQ_SE_FLD_AMB_TENKOU_YUKI);
    break;
  case WEATHER_SNOWSTORM:
    Sound::PlaySE(SEQ_SE_FLD_AMB_TENKOU_FUBUKI);
    break;
  case WEATHER_DIAMONDDUST:
    Sound::PlaySE(SEQ_SE_FLD_AMB_TENKOU_DIAMOND);
    break;
  }
}

/**
 * @brief 天候のフェードアウト
 * @param[in] type 天候種類
 * @param[in] next_type 次の天候種類
 */
void Snow::FadeoutWeatherRequest(WEATHER_TYPE type, WEATHER_TYPE next_type)
{
  if (m_pSnowEff == NULL )
  {
    return;
  }

  //現在の天候のフェードアウト
  switch(m_Type)
  {
  case WEATHER_SNOW:
    //削除
    m_pSnowEff->KillEmitter();
    break;
  case WEATHER_SNOWSTORM:
    //削除
    m_pSnowEff->KillEmitter();
    //雪ビルボードをフェードアウト
    m_pSnowFade->RequestFadeOut();
    break;
  case WEATHER_DIAMONDDUST:
    //空エミッター削除
    m_pSnowEff->KillEmitter();
    //地面エミッターをとめる
    m_generate = false;
    break;
  }
}

/**
 * @brief 天候の変更要請
 * @param[in] type 天候種類
 */
void Snow::ChangeWeatherRequestCore(WEATHER_TYPE type)
{
  if (m_pSnowEff == NULL )
  {
    m_Type = type;
    return;
  }

  FadeoutWeatherRequest(m_Type, type);

  Camera::CameraUnit* pCameraUnit = m_pCameraManager->GetMainGamePlayCamera();
  gfl2::math::Vector3 pos = pCameraUnit->GetBaseCamera()->GetPosition();
  gfl2::math::Vector3 target_pos = pCameraUnit->GetBaseCamera()->GetTargetPosition();
  gfl2::math::Vector3 dir = target_pos - pos;
  gfl2::math::Vector3 emitter_pos = pos + (dir.Normalize() * SNOW_EMITTER_LENGTH);

  //次の天候のフェードイン
  switch(type)
  {
  case WEATHER_SNOW:
    m_pSnowEff->CreateEmitter(Effect::EFFECT_RESOURCE_SKY_SNOW, emitter_pos);
    break;
  case WEATHER_SNOWSTORM:
    m_pSnowEff->CreateEmitter(Effect::EFFECT_RESOURCE_SKY_SNOWSTORM, emitter_pos);
    //雪ビルボードフェードイン
    m_pSnowFade->RequestFadeIn();
    break;
  case WEATHER_DIAMONDDUST:
    //空エミッター作成
    m_pSnowEff->CreateEmitter(Effect::EFFECT_RESOURCE_SKY_DIAMONDDUST, emitter_pos);
    //地面エミッター開始
    m_generate = true;
    break;
  }
  m_Type = type;
}

/**
 * @brief タイプ別フェードアウト終了チェック
 * @param[in] type
 * @return bool ture でフェードアウト終了
 */
bool Snow::IsFinishedFadeOut(WEATHER_TYPE type)
{
  if (m_pSnowEff == NULL )
  {
    return true;
  }

  switch(type)
  {
  case WEATHER_SNOW:
    break;
  case WEATHER_SNOWSTORM:
    //雪ビルボードがフェードアウトしているか
    if (m_pSnowFade->IsVisible()) return false;
    break;
  case WEATHER_DIAMONDDUST:
    //エミッターがとまっているか
    if (m_generate) return false;
    break;
  }

  return true;
}

GFL_NAMESPACE_END(weather)
GFL_NAMESPACE_END(Field)