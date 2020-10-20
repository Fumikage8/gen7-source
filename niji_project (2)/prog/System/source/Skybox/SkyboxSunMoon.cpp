//===================================================================
/**
 * @file    SkyboxSunMoon.cpp
 * @brief   天球太陽と月クラス
 * @author  saita_kazuki
 * @date    2015.05.07
 */
//===================================================================

#include "./SkyboxSunMoon.h"
#include <model/include/gfl2_BaseModel.h>

// debug
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

GFL_NAMESPACE_BEGIN( System )
GFL_NAMESPACE_BEGIN( Skybox )

const u8 SunMoon::MOON_PATTERN_TABLE[31] =
{
  MOON_PATTERN_NONE_DRAW,         // 1
  MOON_PATTERN_NONE_DRAW,         // 2
  MOON_PATTERN_CRESCENT_BEGIN,    // 3
  MOON_PATTERN_CRESCENT_BEGIN,    // 4
  MOON_PATTERN_CRESCENT_BEGIN,    // 5
  MOON_PATTERN_CRESCENT_BEGIN,    // 6
  MOON_PATTERN_HALF_BEGIN,        // 7
  MOON_PATTERN_HALF_BEGIN,        // 8
  MOON_PATTERN_HALF_BEGIN,        // 9
  MOON_PATTERN_HALF_BEGIN,        // 10
  MOON_PATTERN_3_4_BEGIN,         // 11
  MOON_PATTERN_3_4_BEGIN,         // 12
  MOON_PATTERN_3_4_BEGIN,         // 13
  MOON_PATTERN_FULL,              // 14
  MOON_PATTERN_FULL,              // 15
  MOON_PATTERN_FULL,              // 16
  MOON_PATTERN_FULL,              // 17
  MOON_PATTERN_FULL,              // 18
  MOON_PATTERN_FULL,              // 19
  MOON_PATTERN_3_4_END,           // 20
  MOON_PATTERN_3_4_END,           // 21
  MOON_PATTERN_3_4_END,           // 22
  MOON_PATTERN_HALF_END,          // 23
  MOON_PATTERN_HALF_END,          // 24
  MOON_PATTERN_HALF_END,          // 25
  MOON_PATTERN_HALF_END,          // 26
  MOON_PATTERN_CRESCENT_END,      // 27
  MOON_PATTERN_CRESCENT_END,      // 28
  MOON_PATTERN_CRESCENT_END,      // 29
  MOON_PATTERN_NONE_DRAW,         // 30
  MOON_PATTERN_NONE_DRAW,         // 31
};

/**
 * @brief コンストラクタ
 */
SunMoon::SunMoon( gfl2::heap::HeapBase* pParent, void* pResourceData) :
  PartsBase( pParent, 0xC500, "SunMoon", MODEL_TYPE_MAX, ANIMATION_TYPE_MAX)
  ,m_nowMoonPattern( MOON_PATTERN_NONE_DRAW)
  ,m_sunJointIndex( -1)
  ,m_moonJointIndex( -1)
  ,m_sunJointPos( gfl2::math::Vector3::GetZero())
  ,m_moonJointPos( gfl2::math::Vector3::GetZero())
{
  this->Initialize( pResourceData);
}

/**
 * @brief デストラクタ
 */
SunMoon::~SunMoon()
{
  this->Terminate();
}

/**
 * @brief 初期化
 */
void SunMoon::Initialize( void* pResourceData)
{
  ResourceData* pData = reinterpret_cast<ResourceData*>( pResourceData);

  // 月と太陽のモデル、アニメーションリソースノード作成
  for( u32 i = 0; i < MODEL_TYPE_MAX; ++i)
  {
    m_ppModelResourceRootNode[i] = this->CreateModelResourceNode( pData->pModelData[i]);
  }
  for( u32 i = 0; i < ANIMATION_TYPE_MAX; ++i)
  {
    m_ppMotionResourceRootNode[i] = this->CreateMotionResourceNode( pData->pMotionData[i]);
  }

  // 月と太陽のモデルを生成
  m_ppModel[MODEL_TYPE_MOON] = this->CreateModel( m_ppModelResourceRootNode[MODEL_TYPE_MOON]);
  m_ppModel[MODEL_TYPE_SUN] = this->CreateModel( m_ppModelResourceRootNode[MODEL_TYPE_SUN]);

  // 月のアニメーションアタッチ
  this->AttachAnimation( m_ppModel[MODEL_TYPE_MOON], m_ppMotionResourceRootNode[ANIMATION_TYPE_MOON_ALLTIME_JOINT], false, ANIMATION_INDEX_MOON_ALLTIME_JOINT, poke_3d::model::BaseModel::ANIMATION_BIT_JOINT);
  m_ppModel[MODEL_TYPE_MOON]->SetAnimationStepFrame( 0.0f, ANIMATION_INDEX_MOON_ALLTIME_JOINT);

  this->AttachAnimation( m_ppModel[MODEL_TYPE_MOON], m_ppMotionResourceRootNode[ANIMATION_TYPE_MOON_ALLTIME_SHADER], false, ANIMATION_INDEX_MOON_ALLTIME_SHADER, poke_3d::model::BaseModel::ANIMATION_BIT_MATERIAL);
  m_ppModel[MODEL_TYPE_MOON]->SetAnimationStepFrame( 0.0f, ANIMATION_INDEX_MOON_ALLTIME_SHADER);

  this->AttachAnimation( m_ppModel[MODEL_TYPE_MOON], m_ppMotionResourceRootNode[ANIMATION_TYPE_MOON_CHANGE_UV], false, ANIMATION_INDEX_MOON_CHANGE_UV, poke_3d::model::BaseModel::ANIMATION_BIT_MATERIAL);
  m_ppModel[MODEL_TYPE_MOON]->SetAnimationStepFrame( 0.0f, ANIMATION_INDEX_MOON_CHANGE_UV);

  // 太陽のアニメーションアタッチ
  this->AttachAnimation( m_ppModel[MODEL_TYPE_SUN], m_ppMotionResourceRootNode[ANIMATION_TYPE_SUN_ALLTIME_JOINT], false, ANIMATION_INDEX_SUN_ALLTIME_JOINT, poke_3d::model::BaseModel::ANIMATION_BIT_JOINT);
  m_ppModel[MODEL_TYPE_SUN]->SetAnimationStepFrame( 0.0f, ANIMATION_INDEX_SUN_ALLTIME_JOINT);

  this->AttachAnimation( m_ppModel[MODEL_TYPE_SUN], m_ppMotionResourceRootNode[ANIMATION_TYPE_SUN_UV], true, ANIMATION_INDEX_SUN_UV, poke_3d::model::BaseModel::ANIMATION_BIT_MATERIAL);

  // 有効化
  this->SetEnable( true);

  m_sunJointIndex = m_ppModel[MODEL_TYPE_SUN]->GetJointIndex( "sun_joint");
  m_moonJointIndex = m_ppModel[MODEL_TYPE_MOON]->GetJointIndex( "moon_joint");

#if defined( DEBUG_ONLY_FOR_saita_kazuki )
  m_pHeap->PrintHeapInfo( "Skybox Sun Moon");
#endif 
}

/**
 * @brief 破棄
 */
void SunMoon::Terminate()
{
  for( u32 i = 0; i < MODEL_TYPE_MAX; ++i)
  {
    m_ppModel[i]->Destroy();
    GFL_SAFE_DELETE( m_ppModel[i]);
  }

  for( u32 i = 0; i < ANIMATION_TYPE_MAX; ++i)
  {
    GFL_SAFE_DELETE( m_ppMotionResourceRootNode[i]);
  }

  for( u32 i = 0; i < MODEL_TYPE_MAX; ++i)
  {
    GFL_SAFE_DELETE( m_ppModelResourceRootNode[i]);
  }

  m_sunJointIndex = -1;
  m_moonJointIndex = -1;
}

/**
 * @brief 更新
 */
void SunMoon::Update( const gfl2::math::Vector3& pos)
{
  if( m_isEnable == false)
  {
    return;
  }

  if( m_isUpdateAnimation)
  {
    m_ppModel[MODEL_TYPE_MOON]->SetAnimationFrame( m_allTimeAnimationFrame, ANIMATION_INDEX_MOON_ALLTIME_JOINT, poke_3d::model::BaseModel::ANIMATION_BIT_JOINT);
    m_ppModel[MODEL_TYPE_MOON]->SetAnimationFrame( m_allTimeAnimationFrame, ANIMATION_INDEX_MOON_ALLTIME_SHADER, poke_3d::model::BaseModel::ANIMATION_BIT_MATERIAL);
    m_ppModel[MODEL_TYPE_SUN]->SetAnimationFrame( m_allTimeAnimationFrame, ANIMATION_INDEX_SUN_ALLTIME_JOINT, poke_3d::model::BaseModel::ANIMATION_BIT_JOINT);

    poke_3d::model::BaseModel::AnimationBit bit = static_cast<poke_3d::model::BaseModel::AnimationBit>( poke_3d::model::BaseModel::ANIMATION_BIT_JOINT | poke_3d::model::BaseModel::ANIMATION_BIT_MATERIAL );
    for( u32 i = 0; i < MODEL_TYPE_MAX; ++i)
    {
      m_ppModel[i]->SetPosition( pos);
      m_ppModel[i]->UpdateAnimation( bit);
    }

    m_isUpdateAnimation = false;
  }
  else
  {
    for( u32 i = 0; i < MODEL_TYPE_MAX; ++i)
    {
      m_ppModel[i]->SetPosition( pos);
    }

    m_ppModel[MODEL_TYPE_SUN]->UpdateAnimation( poke_3d::model::BaseModel::ANIMATION_BIT_MATERIAL);
  }

}

/**
 * @brief 月の満ち欠けパターン変更
 * @param day 日にち 1～31
 */
void SunMoon::ChangeMoonPattern( s32 day)
{
  GFL_ASSERT( day >= 1 && day <= 31);
  u8 pattern = MOON_PATTERN_TABLE[day - 1];
  m_ppModel[MODEL_TYPE_MOON]->SetAnimationFrame( static_cast<f32>( pattern), ANIMATION_INDEX_MOON_CHANGE_UV, poke_3d::model::BaseModel::ANIMATION_BIT_MATERIAL);
  this->RequestUpdateAnimation();
  m_nowMoonPattern = static_cast<MoonPattern>( pattern);
}

/**
 * @brief 月の満ち欠けパターン変更 月の種類指定版
 * @param pattern 8種類あります
 */
void SunMoon::ChangeMoonPattern( MoonPattern pattern )
{
  s32 fixday = -1;
  for( s32 day = 0 ; day < 31; ++day )
  {
    if( MOON_PATTERN_TABLE[ day ] == pattern )
    {
      fixday = (day + 1);
      break;
    }
  }

  if( fixday == -1 )
  {
    GFL_ASSERT( 0 );
    fixday = 1; // safety
  }
  this->ChangeMoonPattern( fixday );
}

/**
 * @brief 太陽の位置を取得
 * @return 太陽の位置
 */
const gfl2::math::Vector3& SunMoon::GetSunPosition() const
{
  return m_sunJointPos;
}

/**
 * @brief 月の位置を取得
 * @return 月の位置
 */
const gfl2::math::Vector3& SunMoon::GetMoonPosition() const
{
  return m_moonJointPos;
}

GFL_NAMESPACE_END( Skybox )
GFL_NAMESPACE_END( System )
