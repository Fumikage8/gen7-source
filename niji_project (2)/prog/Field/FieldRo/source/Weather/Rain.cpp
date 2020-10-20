//======================================================================
/**
 * @file Rain.cpp
 * @date 2015/10/20 17:20:18
 * @author saito_nozomu
 * @brief 天候　雨
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

#include "Field/FieldRo/include/Weather/Rain.h"
#include "Field/FieldRo/include/Weather/Thunder.h"
#include "Field/FieldRo/include/Weather/BillBoardfade.h"
// system
#include "System/include/GflUse.h"

#include "Field/FieldRo/include/Effect/FieldEffectManager.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectRain.h"
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"

#include "Field/FieldRo/include/Collision/BaseCollisionScene.h"
#include "Sound/include/Sound.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(weather)

static const float RAIN_EMITTER_LENGTH = 800;


Rain::Rain(gfl2::heap::HeapBase* pHeap) :WeatherBase()
{
  m_pHeap = pHeap;
  m_pThunder = GFL_NEW(m_pHeap) Thunder();
  m_pRainFade = GFL_NEW(m_pHeap) BillBoardFade();
  m_pMistFade = GFL_NEW(m_pHeap) BillBoardFade();
  m_pEffectManager = NULL;
  m_pCameraManager = NULL;
  m_generate = false;
  m_generate_margine = 0;
  m_pRainEff = NULL;
  m_pCollisionSceneContainer = NULL;
  for(int i=0; i<RAIN_EFF_TBL_MAX;i++)
  {
    m_pEffectTable[i] = NULL;
  }
}

Rain::~Rain()
{
  if (m_pMistFade != NULL ) GFL_SAFE_DELETE(m_pMistFade);
  if (m_pRainFade != NULL ) GFL_SAFE_DELETE(m_pRainFade);
  if (m_pThunder != NULL ) GFL_SAFE_DELETE(m_pThunder);
}

void Rain::Initialize(InitParam &param)
{
  m_pEffectManager = param.pEffectManager;
  m_pCameraManager = param.pCameraManager;
  m_pCollisionSceneContainer = param.pCollisionSceneContainer; 
  m_generate = false;
  m_generate_margine = 0;
  for(int i=0; i<RAIN_EFF_TBL_MAX;i++)
  {
    m_pEffectTable[i] = NULL;
  }
  m_pRainEff = NULL;

  m_Type = WEATHER_NONE;

}

/**
  * @brief セットアップ
  */
bool Rain::SetupFunc(void)
{
  m_FadeOutSeq = 0;
  //天候作成
  Camera::CameraUnit* pCameraUnit = m_pCameraManager->GetMainGamePlayCamera();
  gfl2::math::Vector3 pos = pCameraUnit->GetBaseCamera()->GetPosition();
  gfl2::math::Quaternion rot = pCameraUnit->GetBaseCamera()->GetRotationQuat();
  gfl2::math::Vector3 dir( 0.0f, 0.0f, -1.0f );
  dir = rot.Transform( dir );

  gfl2::math::Vector3 emitter_pos = pos + (dir.Normalize() * RAIN_EMITTER_LENGTH);
  Effect::IEffectBase *pEffect = m_pEffectManager->CreateEffect(Effect::EFFECT_TYPE_RAIN, emitter_pos);
  Effect::EffectRain *eff = static_cast<Effect::EffectRain*>(pEffect);
  m_pRainEff = eff;
  if (m_pRainEff == NULL )
  {
    return true;
  }

  eff->CreateRain(emitter_pos, pos, RAIN_EMITTER_LENGTH);

  m_pThunder->Initialize( m_pRainEff->GetThunderModel() );
  m_pRainFade->Initialize( m_pRainEff->GetRainModel(), 0 );
  m_pMistFade->Initialize( m_pRainEff->GetMistModel(), 0 );

  switch(m_Type)
  {
  case WEATHER_RAIN:
    break;
  case WEATHER_MIST:
    m_pRainEff->GetMistModel()->SetVisible(true);
    m_pRainEff->VisibleEmitter(false);
    break;
  case WEATHER_THUNDER:
    m_pRainEff->GetRainModel()->SetVisible(true);
    m_pThunder->SetActive(true);
    break;
  }
  m_pMistFade->SetVisible(false);
  m_pRainFade->SetVisible(false);

  return true;
}

void Rain::StartFunc(void)
{
  if (m_pRainEff == NULL )
  {
    return;
  }

  m_generate = false;
  m_pMistFade->SetVisible(false);
  m_pRainFade->SetVisible(false);
}

void Rain::MainFunc(void)
{
  if (m_pRainEff == NULL )
  {
    return ;
  }

  //ビルボードのフェードインフェードアウト処理
  m_pRainFade->Update();
  m_pMistFade->Update();

  //雨エフェクトのデリート処理
  for (int i=0;i<RAIN_EFF_TBL_MAX;i++)
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
  //雷の発生タイミング更新
  m_pThunder->Update();

  Camera::CameraUnit* pCameraUnit = m_pCameraManager->GetMainGamePlayCamera();
  gfl2::math::Vector3 pos = pCameraUnit->GetBaseCamera()->GetPosition();
  gfl2::math::Quaternion rot = pCameraUnit->GetBaseCamera()->GetRotationQuat();
  gfl2::math::Vector3 dir( 0.0f, 0.0f, -1.0f );
  dir = rot.Transform( dir );
  gfl2::math::Vector3 emitter_pos = pos + (dir.Normalize() * RAIN_EMITTER_LENGTH );

  //雨エミッターの位置を更新
  if (m_pRainEff != NULL)
  {
    m_pRainEff->UpdatePosition( pos, dir.Normalize() );
  }

  //雨を抽選して生成
  if (m_generate)
  {
    m_generate_margine++;
    if (m_generate_margine <= 1)
    {
      return;
    }
    m_generate_margine = 0;
    for (int i=0;i<RAIN_EFF_TBL_MAX;i++)
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
        Effect::IEffectBase *pEffect = m_pEffectManager->CreateEffect(Effect::EFFECT_TYPE_RAIN, particle_pos);
        if (pEffect == NULL)
        {
          NOZOMU_PRINT("RAIN_EFFECT_EMPTY\n");
          break; //もう空きがない
        }
        Effect::EffectRain *eff = static_cast<Effect::EffectRain*>(pEffect);
        eff->CreateSprash(particle_pos);
        m_pEffectTable[i] = eff;
        ///GFL_PRINT("%d create\n",i);
        break;
      }
    }
  }
}

void Rain::StopFunc(void)
{
  //雨の生成を停止し、終了リクエストを発効
  m_generate = false;
}

bool Rain::FadeOutFunc(void)
{
  switch(m_FadeOutSeq)
  {
  case 0:
    FadeoutWeatherRequest(m_Type);
    m_FadeOutSeq++;
    break;
  case 1:
    if ( IsFinishedFadeOut(m_Type) )
    {
      m_FadeOutSeq++;
    }
    break;
  case 2:
    if (m_pRainEff != NULL)
    {
      m_pEffectManager->DeleteEffect( m_pRainEff );
      m_pRainEff = NULL;
    }
    int count = 0;
    for (int i=0;i<RAIN_EFF_TBL_MAX;i++)
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
    if (count >= RAIN_EFF_TBL_MAX)
    {
      m_Type = WEATHER_NONE;
      return true;
    }

    break;
  }

  return false;
}

/**
 * @brief ＳＥ再生
 */
void Rain::StartSE(void)
{
  switch(m_Type)
  {
  case WEATHER_RAIN:
    Sound::PlaySE(SEQ_SE_FLD_AMB_TENKOU_AME);
    break;
  case WEATHER_MIST:
    Sound::PlaySE(SEQ_SE_FLD_AMB_TENKOU_KIRI);
    break;
  case WEATHER_THUNDER:
    Sound::PlaySE(SEQ_SE_FLD_AMB_TENKOU_RAIU);
    break;
  }
}

/**
 * @brief 天候のフェードアウト
 * @param[in] type 天候種類
 */
void Rain::FadeoutWeatherRequest(WEATHER_TYPE type)
{
  if (m_pRainEff == NULL )
  {
    return;
  }

  //現在の天候のフェードアウト
  switch(type)
  {
  case WEATHER_RAIN:
    //地面エミッターを止める
    m_generate = false;
    break;
  case WEATHER_MIST:
    //霧ビルボードをフェードアウト
    m_pMistFade->RequestFadeOut();
    break;
  case WEATHER_THUNDER:
    //地面エミッターをとめる
    m_generate = false;
    //雷更新を停止
    m_pThunder->SetActive(false);
    //雨ビルボードをフェードアウト
    m_pRainFade->RequestFadeOut();
    break;
  }
}

/**
 * @brief 天候の変更要請
 * @param[in] type 天候種類
 */
void Rain::ChangeWeatherRequestCore(WEATHER_TYPE type)
{
  if (m_pRainEff == NULL )
  {
    m_Type = type;
    return;
  }

  FadeoutWeatherRequest(m_Type);

  //次の天候のフェードイン
  switch(type)
  {
  case WEATHER_RAIN:
    m_pRainEff->VisibleEmitter(true);
    //地面エミッター開始
    m_generate = true;
    break;
  case WEATHER_MIST:
    m_pRainEff->VisibleEmitter(false);
    //霧ビルボードフェードイン
    m_pMistFade->RequestFadeIn();
    break;
  case WEATHER_THUNDER:
    m_pRainEff->VisibleEmitter(true);
    //地面エミッター開始
    m_generate = true;
    //雷更新開始
    m_pThunder->SetActive(true);
    //雨ビルボードをフェードイン
    m_pRainFade->RequestFadeIn();
    break;
  }
  m_Type = type;
}

/**
 * @brief タイプ別フェードアウト終了チェック
 * @param[in] type
 * @return bool ture でフェードアウト終了
 */
bool Rain::IsFinishedFadeOut(WEATHER_TYPE type)
{
  if (m_pRainEff == NULL )
  {
    return true;
  }

  switch(type)
  {
  case WEATHER_RAIN:
    //地面エミッターがとまっているか
    if (m_generate) return false;
    break;
  case WEATHER_MIST:
    //霧ビルボードがフェードアウトしているか
    if (m_pMistFade->IsVisible()) return false;
    break;
  case WEATHER_THUNDER:
    //地面エミッターがとまっているか
    if (m_generate) return false;
    //雷更新とまっているか
    if (m_pThunder->IsVisible()) return false;
    //雨ビルボードがフェードアウトしているか
    if (m_pRainFade->IsVisible()) return false;
    break;
  }

  return true;
}

GFL_NAMESPACE_END(weather)
GFL_NAMESPACE_END(Field)