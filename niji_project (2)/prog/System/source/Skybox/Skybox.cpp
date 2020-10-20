//===================================================================
/**
 * @file    Skybox.cpp
 * @brief   天球クラス
 * @author  saita_kazuki
 * @date    2015.05.01
 */
//===================================================================

#include <System/include/Skybox/Skybox.h>

#include "GameSys/include/NijiTime.h"

#include "./SkyboxResourceManager.h"
#include "./SkyboxPartsBase.h"
#include "./SkyboxSky.h"
#include "./SkyboxSunMoon.h"
#include "./SkyboxCloudySky.h"
#include "./SkyboxCloud.h"

// system
#include <System/include/HeapDefine.h>
#include <System/include/HeapSize.h>
#include "System/include/nijiAllocator.h"

// gflib2
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>
#include <math/include/gfl2_Float.h>
#include <system\include\Date\\gfl2_Date.h>

// poke_3d
#include <model/include/gfl2_BaseModel.h>


#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"

// flag
#include "Savedata/include/EventWork.h"
#include "niji_conv_header/field/flagwork/FlagDefine.h"
#include "niji_conv_header/field/flagwork/WorkDefine.h"


GFL_NAMESPACE_BEGIN( System )
GFL_NAMESPACE_BEGIN( Skybox )

// 定数
const f32 Skybox::ALL_TIME_ANIMATION_FRAME_MAX        = 480.0f;  // アニメーションリソースの最大フレーム数と合わせる
const f32 Skybox::ALL_TIME_ANIMATION_ONE_HOUR_FRAME   = ALL_TIME_ANIMATION_FRAME_MAX / 24.0f;     // 24 = 1日の合計時間数
const f32 Skybox::ALL_TIME_ANIMATION_ONE_MINUTE_FRAME = ALL_TIME_ANIMATION_FRAME_MAX / 1440.0f;   // 1440 = 1日の合計分数
const f32 Skybox::ALL_TIME_ANIMATION_ONE_SECOND_FRAME = ALL_TIME_ANIMATION_FRAME_MAX / 86400.0f;  // 86400 = 1日の合計秒数

/**
 * @brief コンストラクタ
 * @param pEventWork フラグチェックに使用するイベントワーク
 */
Skybox::Skybox( Field::EventWork* pEventWork  ) :
  m_pGLAllocator( NULL),
  m_pDeviceHeap( NULL),
  m_pResourceManager( NULL),
  m_pSky( NULL),
  m_pSunMoon( NULL),
  m_pCloudySky( NULL),
  m_pCloud( NULL),
  m_setupState( SETUP_STATE_NONE),
  m_description(),
  m_isEnable( false),
  m_beforeUpdateTime(),
  m_skyWork(),
  m_pEventWork( pEventWork )
{
  GFL_ASSERT( m_pEventWork );
}

/**
 * @brief デストラクタ
 */
Skybox::~Skybox()
{
  this->DestroyAll();
}

/**
 * @brief ヒープ生成
 * @param pParentDeviceHeap 親ヒープ。NULLの場合はHEAPID_BATTLE_TRANSITION
 */
void Skybox::CreateHeap( gfl2::heap::HeapBase* pParentDeviceHeap)
{
  if( pParentDeviceHeap == NULL)
  {
    pParentDeviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_BATTLE_TRANSITION);
  }
  this->CreateLocalHeap( pParentDeviceHeap);
}

/**
 * @brief ヒープ破棄
 */
void Skybox::DeleteHeap()
{
  this->DeleteLocalHeap();
}

/**
 * @brief リソース生成開始
 * @param desc 設定情報
 * @param pTemporaryHeap リソース読み込みに使用する一時確保用ヒープ
 * @note 親ヒープはAPPです
 */
void Skybox::StartCreateResource( const Description& desc, gfl2::heap::HeapBase* pTemporaryHeap)
{
  GFL_ASSERT( m_setupState == SETUP_STATE_NONE);

  m_description = desc;

  m_pGLAllocator = GFL_NEW( m_pDeviceHeap) System::nijiAllocator( m_pDeviceHeap);

  m_pResourceManager = GFL_NEW( m_pDeviceHeap) ResourceManager();
  m_pResourceManager->Initialize( m_pDeviceHeap, pTemporaryHeap);
  m_pResourceManager->StartAsyncLoad();
}

/**
 * @brief リソース生成更新
 * @retval true 完了
 * @retval false 実行中
 */
bool Skybox::UpdateCreateResource()
{
  if( this->IsCreateResource())
  {
    return true;
  }

  if( m_pResourceManager->WaitAsyncLoad())
  {
    m_setupState = SETUP_STATE_CREATED_RESOURCE;
    return true;
  }
  return false;
}

/**
 * @brief リソース生成が完了しているか
 * @retval true 完了
 * @retval false 未完了
 */
bool Skybox::IsCreateResource() const
{
  return ( m_setupState >= SETUP_STATE_CREATED_RESOURCE );
}

/**
 * @brief 破棄
 * @note 部品もリソースも全て破棄されます。リソースを残す場合はDestroyPartsのみ呼んでください
 */
void Skybox::DestroyAll()
{
  this->Terminate();

  this->DestroyParts();

  if( m_pResourceManager)
  {
    m_pResourceManager->Terminate();
    GFL_DELETE( m_pResourceManager);
    m_pResourceManager = NULL;
  }

  GFL_SAFE_DELETE( m_pGLAllocator);

  m_description.rotateRadY = 0.0f;

  m_setupState = SETUP_STATE_NONE;
}

/**
 * @brief 部品(空などのインスタンス)生成
 * @param 設定情報。NULLの場合、StartCreateResoureで設定されたものを使用します
 * @param isAllocateByVRAM trueでテクスチャをVRAMに乗せる
 * @param vramMallocMode isAllocateByVRAMがtrueのときに指定されるVRAM確保タイプ
 */
void Skybox::CreateParts( const Description* pDesc, bool isAllocateByVRAM, System::nijiAllocator::VramMallocMode vramMallocMode )
{
  GFL_ASSERT( m_setupState == SETUP_STATE_CREATED_RESOURCE);

  if( pDesc)
  {
    m_description = *pDesc;
  }

  {
    Sky::ResourceData data;
    data.pModelData[Sky::MODEL_TYPE_NORMAL]       = m_pResourceManager->GetData( ResourceManager::RESOURCE_TYPE_COMMON, ResourceManager::PACK_COMMON_INDEX_SUNNY_SKY_MODEL);
    data.pMotionData[Sky::ANIMATION_TYPE_ALLTIME] = m_pResourceManager->GetData( ResourceManager::RESOURCE_TYPE_COMMON, ResourceManager::PACK_COMMON_INDEX_SUNNY_SKY_ALLTIME_MOTION);
    m_pSky = GFL_NEW( m_pDeviceHeap) Sky( m_pDeviceHeap, reinterpret_cast<void*>( &data));
    m_pSky->GetModel( Sky::MODEL_TYPE_NORMAL)->SetRotation( 0.0f, m_description.rotateRadY, 0.0f);
  }

  {
    SunMoon::ResourceData data;
    data.pModelData[SunMoon::MODEL_TYPE_MOON]                     = m_pResourceManager->GetData( ResourceManager::RESOURCE_TYPE_COMMON, ResourceManager::PACK_COMMON_INDEX_MOON_MODEL);
    data.pModelData[SunMoon::MODEL_TYPE_SUN]                      = m_pResourceManager->GetData( ResourceManager::RESOURCE_TYPE_COMMON, ResourceManager::PACK_COMMON_INDEX_SUN_MODEL);
    data.pMotionData[SunMoon::ANIMATION_TYPE_MOON_ALLTIME_JOINT]  = m_pResourceManager->GetData( ResourceManager::RESOURCE_TYPE_COMMON, ResourceManager::PACK_COMMON_INDEX_MOON_ALLTIME_JOINT_MOTION);
    data.pMotionData[SunMoon::ANIMATION_TYPE_MOON_ALLTIME_SHADER] = m_pResourceManager->GetData( ResourceManager::RESOURCE_TYPE_COMMON, ResourceManager::PACK_COMMON_INDEX_MOON_ALLTIME_SHADER_MOTION);
    data.pMotionData[SunMoon::ANIMATION_TYPE_MOON_CHANGE_UV]      = m_pResourceManager->GetData( ResourceManager::RESOURCE_TYPE_COMMON, ResourceManager::PACK_COMMON_INDEX_MOON_CHANGE_UV_MOTION);
    data.pMotionData[SunMoon::ANIMATION_TYPE_SUN_ALLTIME_JOINT]   = m_pResourceManager->GetData( ResourceManager::RESOURCE_TYPE_COMMON, ResourceManager::PACK_COMMON_INDEX_SUN_ALLTIME_JOINT_MOTION);
    data.pMotionData[SunMoon::ANIMATION_TYPE_SUN_UV]              = m_pResourceManager->GetData( ResourceManager::RESOURCE_TYPE_COMMON, ResourceManager::PACK_COMMON_INDEX_SUN_UV_MOTION);
    m_pSunMoon = GFL_NEW( m_pDeviceHeap) SunMoon( m_pDeviceHeap, reinterpret_cast<void*>( &data));
    m_pSunMoon->GetModel( SunMoon::MODEL_TYPE_SUN)->SetRotation( 0.0f, m_description.rotateRadY, 0.0f);
    m_pSunMoon->GetModel( SunMoon::MODEL_TYPE_MOON)->SetRotation( 0.0f, m_description.rotateRadY, 0.0f);
  }

  {
    CloudySky::ResourceData data;
    data.pModelData[CloudySky::MODEL_TYPE_NORMAL] =  m_pResourceManager->GetData( ResourceManager::RESOURCE_TYPE_COMMON, ResourceManager::PACK_COMMON_INDEX_CLOUDY_SKY_MODEL);
    m_pCloudySky = GFL_NEW( m_pDeviceHeap) CloudySky( m_pDeviceHeap, reinterpret_cast<void*>( &data));
    m_pCloudySky->GetModel( CloudySky::MODEL_TYPE_NORMAL)->SetRotation( 0.0f, m_description.rotateRadY, 0.0f);
  }

  {
    Cloud::ResourceData data;
    data.pModelData[Cloud::MODEL_TYPE_NORMAL]               = m_pResourceManager->GetData( ResourceManager::RESOURCE_TYPE_COMMON, ResourceManager::PACK_COMMON_INDEX_CLOUD_MODEL);
    data.pMotionData[Cloud::ANIMATION_TYPE_ALLTIME_SHADER]  = m_pResourceManager->GetData( ResourceManager::RESOURCE_TYPE_COMMON, ResourceManager::PACK_COMMON_INDEX_CLOUD_ALLTIME_SHADER_MOTION);
    data.pMotionData[Cloud::ANIMATION_TYPE_UV]              = m_pResourceManager->GetData( ResourceManager::RESOURCE_TYPE_COMMON, ResourceManager::PACK_COMMON_INDEX_CLOUD_UV_MOTION);
    m_pCloud = GFL_NEW( m_pDeviceHeap) Cloud( m_pDeviceHeap, reinterpret_cast<void*>( &data), isAllocateByVRAM, vramMallocMode);
    m_pCloud->GetModel( Cloud::MODEL_TYPE_NORMAL)->SetRotation( 0.0f, m_description.rotateRadY, 0.0f);
  }

  m_setupState = SETUP_STATE_CREATED_PARTS;
}

/**
 * @brief 部品(空などのインスタンス)破棄
 */
void Skybox::DestroyParts()
{
  GFL_SAFE_DELETE( m_pCloud);
  GFL_SAFE_DELETE( m_pCloudySky);
  GFL_SAFE_DELETE( m_pSunMoon);
  GFL_SAFE_DELETE( m_pSky);

  m_setupState = SETUP_STATE_CREATED_RESOURCE;
}

/**
 * @brief 初期化
 * @param skyType 空の種類
 */
void Skybox::Initialize( SkyType skyType)
{
  GFL_ASSERT( m_setupState == SETUP_STATE_CREATED_PARTS);

  GameSys::DeviceDate date;

  m_skyWork.now = SKY_TYPE_NONE;
  m_skyWork.req = SKY_TYPE_NONE;
  m_skyWork.fade = IDLE;
  m_skyWork.changeFrame = 0;
  //m_skyWork.changeFrameCnt = 0;

  // @fix GFNMCat[1677] zonetableで天球をＯＦＦにしても空のモデルが表示されてしまう
  // 有効フラグをセットするタイミングをChangeSkyの前にする
  m_isEnable = true;

  this->ChangeSky( skyType);

  // 月の満ち欠け
  // 要望対応：フラグとワークによって月の見た目を固定する
  if( m_pEventWork &&
      Field::EventWork::CheckIDFlagRenge( SYS_FLAG_MOON_FORM_FIX ) &&
      m_pEventWork->CheckEventFlag( SYS_FLAG_MOON_FORM_FIX ) &&
      ( Field::EventWork::CheckIDFlagRenge( WK_SYS_MOON_FORM ) == false )
    )
  {
    u16 pattern =  m_pEventWork->GetEventWork( WK_SYS_MOON_FORM );
    m_pSunMoon->ChangeMoonPattern( static_cast<MoonPattern>( pattern ) );
  }
  else
  {
    s32 day = date.GetDay();
    m_pSunMoon->ChangeMoonPattern( day );
  }

  m_beforeUpdateTime.hour   = -1;
  m_beforeUpdateTime.minute = -1;
  m_beforeUpdateTime.second = -1;

  m_setupState = SETUP_STATE_ACTIVE;

#if defined( DEBUG_ONLY_FOR_saita_kazuki )
  m_pDeviceHeap->PrintHeapInfo( "Skybox");
#endif
}

/**
 * @brief 終了
 */
void Skybox::Terminate()
{
  m_isEnable = false;

  m_beforeUpdateTime.hour   = -1;
  m_beforeUpdateTime.minute = -1;
  m_beforeUpdateTime.second = -1;

  m_skyWork.now = SKY_TYPE_NONE;
  m_skyWork.req = SKY_TYPE_NONE;
  m_skyWork.changeFrame = 0;
  //m_skyWork.changeFrameCnt = 0;

  m_setupState = SETUP_STATE_CREATED_PARTS;
}

/**
 * @brief 更新
 * @param pos 天球に設定する位置
 * @param hour 時間
 * @param minute 分
 * @param second 秒
 */
void Skybox::Update( const gfl2::math::Vector3& pos, s32 hour, s32 minute, s32 second)
{
  if( IsEnable() == false)
  {
    return;
  }

  // 空の切り替え更新
  this->UpdateChangeSky();

  // 1日アニメーションのフレーム更新
  if( this->IsUpdateAllTimeAnimation( hour, minute))
  {
    m_beforeUpdateTime.hour = hour;
    m_beforeUpdateTime.minute = minute;
    m_beforeUpdateTime.second = second;

    f32 allTimeAnimationFrame = this->CalculateTimeToAllTimeAnimationFrame( hour, minute, second);

    m_pSky->UpdateAllTimeAnimationFrame( allTimeAnimationFrame);
    m_pSunMoon->UpdateAllTimeAnimationFrame( allTimeAnimationFrame);
    m_pCloud->UpdateAllTimeAnimationFrame( allTimeAnimationFrame);
  }

  // 更新
#if 0
  switch( m_skyWork.now)
  {
  case SKY_TYPE_SUNNY:
    m_pSky->Update( pos);
    break;
  case SKY_TYPE_CLOUDY:
    m_pCloudySky->Update( pos);
    break;
  default:
    GFL_ASSERT( 0);
    break;
  }
#endif
  m_pSky->Update( pos);
  m_pCloudySky->Update( pos);
  m_pSunMoon->Update( pos);
  m_pCloud->Update( pos);
}

/**
 * @brief 有効無効フラグの設定
 * @param isEnable trueで有効
 */
void Skybox::SetEnable( bool isEnable)
{
  if( m_isEnable != isEnable)
  {
    m_isEnable = isEnable;
    m_pSunMoon->SetEnable( isEnable);
    m_pCloud->SetEnable( isEnable);

    switch( m_skyWork.now)
    {
    case SKY_TYPE_SUNNY:
      m_pSky->SetEnable( isEnable);
      break;
    case SKY_TYPE_CLOUDY:
      m_pCloudySky->SetEnable( isEnable);
      break;
    default:
      GFL_ASSERT( 0);
      break;
    }
  }
}

/**
 * @brief 部品ごとの有効無効フラグの設定
 * @param type 部品の種類
 * @param isEnable trueで有効
 */
void Skybox::SetEnableParts( PartsType type, bool isEnable)
{
  // @fix GFNMCat[1677] zonetableで天球をＯＦＦにしても空のモデルが表示されてしまう
  // 有効でないなら受け付けない
  if( IsEnable() == false)
  {
    return;
  }

  switch( type)
  {
  case PARTS_TYPE_SKY:
    m_pSky->SetEnable( isEnable);
    break;
  case PARTS_TYPE_SUN:
    m_pSunMoon->SetEnable( isEnable);
    break;
  case PARTS_TYPE_MOON:
    m_pSunMoon->SetEnable( isEnable);
    break;
  case PARTS_TYPE_CLOUDY_SKY:
    m_pCloudySky->SetEnable( isEnable);
    break;
  case PARTS_TYPE_CLOUD:
    m_pCloud->SetEnable( isEnable);
    break;
  default:
    GFL_ASSERT( 0);
    break;
  }
}

/**
 * @brief 天球が有効か
 * @retval true 有効
 * @retval false 無効
 */
bool Skybox::IsEnable() const
{
  return m_isEnable;
}

/**
 * @brief 指定した部品が有効か
 * @retval true 有効
 * @retval false 無効
 */
bool Skybox::IsEnableParts( PartsType type) const
{
  switch( type)
  {
  case PARTS_TYPE_SKY:
    return m_pSky->IsEnable();
  case PARTS_TYPE_SUN:
    return m_pSunMoon->IsEnable();
  case PARTS_TYPE_MOON:
    return m_pSunMoon->IsEnable();
  case PARTS_TYPE_CLOUDY_SKY:
    return m_pCloudySky->IsEnable();
  case PARTS_TYPE_CLOUD:
    return m_pCloud->IsEnable();
  default:
    GFL_ASSERT( 0);
    return false;
  }
}

/**
 * @brief 空の変更リクエスト
 * @param type 空の種類
 * @param changeFrame 変更するまでにかけるフレーム数
 */
void Skybox::ChangeSkyRequest( SkyType type, u32 changeFrame)
{
  // @fix GFNMCat[1677] zonetableで天球をＯＦＦにしても空のモデルが表示されてしまう
  // 有効でないなら受け付けない
  if( IsEnable() == false)
  {
    return;
  }

  if( m_skyWork.now != type && m_skyWork.req == SKY_TYPE_NONE)
  {
    m_skyWork.now = type;
    m_skyWork.req = type;
    m_skyWork.fade = REQUEST;
    m_skyWork.changeFrame = changeFrame;
    //m_skyWork.changeFrameCnt = 0;
  }
}

/**
 * @brief 空の種類を取得
 * @return 現在の空の種類
 */
Skybox::SkyType Skybox::GetSkyType() const
{
  return m_skyWork.now;
}

/**
 * @brief 部品のベースモデルインスタンスの取得
 * @param type 部品の種類
 * @return ベースモデル
 */
poke_3d::model::BaseModel* Skybox::GetPartsModel( PartsType type)
{
  switch( type)
  {
  case PARTS_TYPE_SKY:
    return m_pSky->GetModel( Sky::MODEL_TYPE_NORMAL);
  case PARTS_TYPE_SUN:
    return m_pSunMoon->GetModel( SunMoon::MODEL_TYPE_SUN);
  case PARTS_TYPE_MOON:
    return m_pSunMoon->GetModel( SunMoon::MODEL_TYPE_MOON);
  case PARTS_TYPE_CLOUDY_SKY:
    return m_pCloudySky->GetModel( CloudySky::MODEL_TYPE_NORMAL);
  case PARTS_TYPE_CLOUD:
    return m_pCloud->GetModel( Cloud::MODEL_TYPE_NORMAL);
  default:
    GFL_ASSERT( 0);
    return NULL;
  }
}

/**
 * @brief 太陽の位置を取得
 * @return 太陽の位置
 */
const gfl2::math::Vector3& Skybox::GetSunPosition() const
{
  return m_pSunMoon->GetSunPosition();
}

/**
 * @brief 月の位置を取得
 * @return 月の位置
 */
const gfl2::math::Vector3& Skybox::GetMoonPosition() const
{
  return m_pSunMoon->GetMoonPosition();
}

/**
 * @brief スカイボックスが晴れ曇りの変化中か？
 * @return tureで変更中
 */
bool Skybox::IsChanging(void)
{
  if( IsEnable() == false) return false;
  if( m_skyWork.fade == IDLE ) return false;
  return true;
}

/**
 * @brief ヒープ作成
 */
void Skybox::CreateLocalHeap( gfl2::heap::HeapBase* pParentDeviceHeap)
{
  if( m_pDeviceHeap)
  {
    return;
  }

  const u32 HEAP_SKYBOX_DEVICE = 0xB4800;

  m_pDeviceHeap = GFL_CREATE_LOCAL_HEAP_NAME(
    pParentDeviceHeap,
    HEAP_SKYBOX_DEVICE,
    gfl2::heap::HEAP_TYPE_EXP,
    false,
    "Skybox"
    );
}

/**
 * @brief ヒープ削除
 */
void Skybox::DeleteLocalHeap()
{
  if( m_pDeviceHeap)
  {
    GFL_DELETE_HEAP( m_pDeviceHeap);
    m_pDeviceHeap = NULL;
  }
}

/**
 * @brief 1日アニメーションのフレームを更新する必要があるか
 * @param hour 時間
 * @param minute 分
 * @retval true 更新する
 * @retval false 更新しない
 */
bool Skybox::IsUpdateAllTimeAnimation( s32 hour, s32 minute)
{
#if PM_DEBUG
  if( FIELD_DEBUG_SWITCH( Field::Debug::DebugTypes::Switch::ENABLE_SKYBOX_ALWAYS_UPDATE) == 1)
  {
    return true;
  }
#endif // PM_DEBUG

  // 1分以上経過 もしくは 時間が変わっていたら更新する
  // @fix 時間が同じで分がbeforeより小さいときに対応できていなかった。+-1分差なら変更するように修正
  // @fix NMCat[694] NMCat[725] 地形の見た目の変化と合わせるため、更新間隔を1分に修正
  if( gfl2::math::Abs( minute - m_beforeUpdateTime.minute ) >= 1 ||
      hour != m_beforeUpdateTime.hour
    )
  {
    return true;
  }
  return false;
}

/**
 * @brief 時間からアニメーションフレームに変換
 * @param hour 時間
 * @param minute 分
 * @param second 秒
 */
f32 Skybox::CalculateTimeToAllTimeAnimationFrame( s32 hour, s32 minute, s32 second)
{
  f32 hourFrame   = static_cast<f32>(hour) * ALL_TIME_ANIMATION_ONE_HOUR_FRAME;
  f32 minuteFrame = static_cast<f32>(minute) * ALL_TIME_ANIMATION_ONE_MINUTE_FRAME;
  f32 secondFrame = static_cast<f32>(second) * ALL_TIME_ANIMATION_ONE_SECOND_FRAME;
  f32 allTimeAnimationFrame = hourFrame + minuteFrame + secondFrame;

  return gfl2::math::Float::Floor( allTimeAnimationFrame, 2);
}

/**
 * @brief 空の切り替え更新処理
 */
void Skybox::UpdateChangeSky()
{
  if( m_skyWork.fade == IDLE )
  {
    return;
  }
  else if ( m_skyWork.fade == REQUEST )
  {
    if ( m_skyWork.req == SKY_TYPE_SUNNY )
    {
      m_pSky->SetEnable(true);
      m_pCloudySky->FadeoutRequest(m_skyWork.changeFrame);
      m_skyWork.fade = FADEWAIT;
    }
    else if ( m_skyWork.req == SKY_TYPE_CLOUDY )
    {
      m_pCloudySky->FadeinRequest(m_skyWork.changeFrame);
      m_skyWork.fade = FADEWAIT;
    }
    else
    {
      GFL_ASSERT(0);
      m_skyWork.fade = IDLE;
      m_skyWork.req = SKY_TYPE_NONE;
    }
  }
  else
  {
    //切り替え待ち
    if ( m_pCloudySky->IsFadeIdle() )
    {
      if (m_skyWork.req == SKY_TYPE_CLOUDY)
      {
        m_pSky->SetEnable(false);
      }
      m_skyWork.fade = IDLE;
      m_skyWork.req = SKY_TYPE_NONE;
    }
  }
}

/**
 * @brief 空の切り替え
 */
void Skybox::ChangeSky( SkyType type)
{
  // @fix GFNMCat[1677] zonetableで天球をＯＦＦにしても空のモデルが表示されてしまう
  // 有効でないなら受け付けない
  if( IsEnable() == false)
  {
    return;
  }

  switch( type)
  {
  case SKY_TYPE_SUNNY:
    m_pSky->SetEnable( true);
    ///m_pCloudySky->SetEnable( false);
    m_pCloudySky->SetVisible(false);
    break;

  case SKY_TYPE_CLOUDY:
    ///m_pCloudySky->SetEnable( true);
    m_pSky->SetEnable( false);
    m_pCloudySky->SetVisible(true);
    break;

  default:
    GFL_ASSERT( 0);
    break;
  }

  m_skyWork.now = type;
  m_skyWork.req = SKY_TYPE_NONE;
}

GFL_NAMESPACE_END( Skybox )
GFL_NAMESPACE_END( System )

