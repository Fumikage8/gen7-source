//===================================================================
/**
 * @file    SkyboxCloudySky.cpp
 * @brief   天球曇り空クラス
 * @author  saita_kazuki
 * @date    2015.05.07
 */
//===================================================================

#include "./SkyboxCloudySky.h"
#include <model/include/gfl2_BaseModel.h>

// debug
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

GFL_NAMESPACE_BEGIN( System )
GFL_NAMESPACE_BEGIN( Skybox )

/**
 * @brief コンストラクタ
 */
CloudySky::CloudySky( gfl2::heap::HeapBase* pParent, void* pResourceData) :
  PartsBase( pParent, 0x2600, "CloudySky", MODEL_TYPE_MAX, ANIMATION_TYPE_MAX)
{
  this->Initialize( pResourceData);
}

/**
 * @brief デストラクタ
 */
CloudySky::~CloudySky()
{
  this->Terminate();
}

/**
 * @brief 初期化
 */
void CloudySky::Initialize( void* pResourceData)
{
  ResourceData* pData = reinterpret_cast<ResourceData*>( pResourceData);

  for( u32 i = 0; i < MODEL_TYPE_MAX; ++i)
  {
    m_ppModelResourceRootNode[i] = this->CreateModelResourceNode( pData->pModelData[i]);
    m_ppModel[i] = this->CreateModel( m_ppModelResourceRootNode[i]);
  }

  m_State = IDLE;
  m_Alpha = 1.0;
  SetAlpha(m_Alpha);
  m_changeFrame = 1;
  m_nowFrame = 0;

#if defined( DEBUG_ONLY_FOR_saita_kazuki )
  m_pHeap->PrintHeapInfo( "Skybox Cloudy Sky");
#endif 
}

/**
 * @brief 破棄
 */
void CloudySky::Terminate()
{
  for( u32 i = 0; i < MODEL_TYPE_MAX; ++i)
  {
    m_ppModel[i]->Destroy();
    GFL_SAFE_DELETE( m_ppModel[i]);

    GFL_SAFE_DELETE( m_ppModelResourceRootNode[i]);
  }
}

/**
 * @brief 更新
 */
void CloudySky::Update( const gfl2::math::Vector3& pos)
{
#if 0
  if( m_isEnable == false)
  {
    return;
  }
#endif
  for( u32 i = 0; i < MODEL_TYPE_MAX; ++i)
  {
    m_ppModel[i]->SetPosition( pos);
  }
  Fade();
}

/**
 * @brief フェードインリクエスト
 * @param changeFrame 変更フレーム
 */
void CloudySky::FadeinRequest(u32 changeFrame)
{
  m_State = FADE_IN;
  if ( changeFrame  <= 0 ) changeFrame = 1;
  m_changeFrame = changeFrame;
  m_nowFrame = 0;
}

/**
 * @brief フェードアウトリクエスト
 * @param changeFrame 変更フレーム
 */
void CloudySky::FadeoutRequest(u32 changeFrame)
{
  m_State = FADE_OUT;
  if ( changeFrame  <= 0 ) changeFrame = 1;
  m_changeFrame = changeFrame;
  m_nowFrame = 0;
}

/**
 * @brief 表示状態
 * @param visible true で表示
 */
void CloudySky::SetVisible(bool visible)
{
  f32 alpha = 0;
  if (visible) alpha = 1;
  m_Alpha = alpha;
  SetAlpha(alpha);
  NOZOMU_PRINT("Fadeout %f\n",m_Alpha);
}

/**
 * @brief αセット
 * @param alpha α値
 */
void CloudySky::SetAlpha( f32 alpha )
{
  for( u32 i = 0; i < MODEL_TYPE_MAX; ++i)
  {
    gfl2::math::Vector4 col = m_ppModel[i]->GetMaterialColorConstant(0,0);
    col.w = alpha;
    m_ppModel[i]->SetMaterialColorConstant(0,0,col);
  }

  if(alpha > 0)
  {
    this->SetEnable(true);
  }
  else
  {
    this->SetEnable(false);
  }
}

/**
 * @brief フェード関数
 */
void CloudySky::Fade(void)
{
  switch(m_State)
  {
  case IDLE:
    break;
  case FADE_OUT:
    m_nowFrame++;
    m_Alpha = 1.0f - ( (1.0f * m_nowFrame) / m_changeFrame );
    if ( (m_Alpha < 0) || (m_nowFrame >= m_changeFrame) )
    {
      m_Alpha = 0;
      m_State = IDLE;
      m_nowFrame = 0;
      m_changeFrame = 1;
    }
    NOZOMU_PRINT("Fadeout %f\n",m_Alpha);
    SetAlpha(m_Alpha);
    break;
  case FADE_IN:
    m_nowFrame++;
    m_Alpha = (1.0f * m_nowFrame) / m_changeFrame;
    if ( (m_Alpha > 1) || (m_nowFrame >= m_changeFrame) )
    {
      m_Alpha = 1;
      m_State = IDLE;
      m_nowFrame = 0;
      m_changeFrame = 1;
    }
    NOZOMU_PRINT("Fadeout %f\n",m_Alpha);
    SetAlpha(m_Alpha);
    break;
  }
}

GFL_NAMESPACE_END( Skybox )
GFL_NAMESPACE_END( System )

