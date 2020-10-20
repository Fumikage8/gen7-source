//======================================================================
/**
 * @file FieldGimmickEncountManager.cpp
 * @date 2015/09/04 16:59:34
 * @author saita_kazuki
 * @brief ギミックエンカウントマネージャー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountManager.h"
#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountLottery.h"

#include <fs/include/gfl2_BinLinkerAccessor.h>
#include <util/include/gfl2_std_string.h>

#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountDataAccessor.h"
#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountManager.h"
#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountData.h"

#include "Field/FieldStatic/include/FieldWorld.h"
#include "Field/FieldStatic/include/FieldArea.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
#include "Field/FieldRo/include/PlacementData/FieldPlacementDataManager.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldGimmickEncountAccessor.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldGimmickEncountActorBase.h"

#include "Sound/include/Sound.h"

// savedata
#include "Savedata/include/EventWork.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( GimmickEncount )

/**
 * @brief コンストラクタ
 */
Manager::Manager()
  : m_work()
  , m_localWork()
  , m_desc()
{
  gfl2::std::MemClear( m_pActor, sizeof( m_pActor ) );
}

/**
 * @brief デストラクタ
 */
Manager::~Manager()
{
}

/**
 * @brief 初期化
 * @param workDesc ワークの初期化情報
 */
void Manager::Initialize( const Description& desc)
{
  m_desc = desc;
  m_work.Initialize();
  gfl2::std::MemClear( m_pActor, sizeof( m_pActor));
}

/**
 * @brief 破棄
 */
void Manager::Terminate()
{
  gfl2::std::MemClear( m_pActor, sizeof( m_pActor));
  m_work.Terminate();
  for( u32 i = 0; i < GIMMICK_ENCOUNT_MAX; ++i)
  {
    m_localWork[i].Clear();
  }
}

/**
 * @brief イベントチェック前更新
 */
void Manager::EventCheckPreUpdate()
{
  for( u32 i = 0; i < GIMMICK_ENCOUNT_MAX; ++i)
  {
    m_localWork[i].EventCheckPreUpdate();
  }
}

/**
 * @brief イベントチェック後更新
 */
void Manager::EventCheckPostUpdate()
{
  for( u32 i = 0; i < GIMMICK_ENCOUNT_MAX; ++i)
  {
    m_localWork[i].EventCheckPostUpdate();
  }
}

/**
 * @brief 更新
 * @param workDesc ワークの更新情報
 */
void Manager::Update( const Work::UpdateDescription& workDesc)
{
  m_work.Update( workDesc);

  for( u32 i = 0; i < GIMMICK_ENCOUNT_MAX; ++i)
  {
    if( m_pActor[i])
    {
      m_pActor[i]->Update();
    }
  }

  this->UpdateSE();
}

/**
 * @brief 空いているローカルワーク取得
 */
LocalWork* Manager::GetEmptyLocalWork()
{
  for( u32 i = 0; i < GIMMICK_ENCOUNT_MAX; ++i)
  {
    if( m_localWork[i].IsActive() == false)
    {
      return &m_localWork[i];
    }
  }
  GFL_ASSERT( 0); // ここにはこない
  return NULL;
}

/**
 * @brief ゾーンのギミックフラグ抽選
 * @param zoneID ゾーンID
 */
void Manager::LotteryZoneGimmickFlag( ZoneID zoneID)
{
  u32 index = m_desc.pWorld->GetZoneIndexInArea( zoneID);
  if( index < m_desc.pWorld->GetZoneNum())
  {
    gfl2::fs::BinLinkerAccessor accessor;
    accessor.Initialize( m_desc.pArea->GetAccessor( Area::AREA_DATA_IDX_PLACEMENTDATA)->GetData( PlacementDataManager::PACK_IDX_GIMMICK_ENCOUNT));

    if( accessor.GetDataSize( index) != 0)
    {
      this->LotteryGimmickFlag(
        m_desc.pEventWork,
        accessor.GetData( index),
        m_desc.pResident->GetResource( BL_IDX_FIELD_RESIDENT__GIMMICK_ENCOUNT_PACK)
        );
    }
  }
}

/**
 * @brief ギミックフラグ抽選
 * @param pZoneGimmickEncountPlacementData そのゾーンのギミックエンカウント配置データ
 * @param pGimmickEncountData ギミックエンカウントテーブルデータ
 */
void Manager::LotteryGimmickFlag( EventWork* pEventWork, void* pZoneGimmickEncountPlacementData, void* pGimmickEncountData)
{
  FieldGimmickEncountAccessor gimmickEncountAccessor;
  gimmickEncountAccessor.Initialize( pZoneGimmickEncountPlacementData);

  GimmickEncount::DataAccessor gimmickEncountDataAccessor;
  gimmickEncountDataAccessor.SetData( pGimmickEncountData);

  u32 max = gimmickEncountAccessor.GetDataMax();

  GFL_ASSERT_STOP( max <= GIMMICK_ENCOUNT_MAX );

  for( u32 i = 0; i < max; ++i)
  {
    const FieldGimmickEncountAccessor::Data *gimmickEncountData = gimmickEncountAccessor.GetData( i );

    u16 gimmickFlagNo = FZ_GIMMICK_ENCOUNT_00 + i;
    void* pTypesData = gimmickEncountDataAccessor.GetData( gimmickEncountData->presetID);

    GimmickEncount::GimmickType gimmickType = gimmickEncountDataAccessor.GetGimmickType( gimmickEncountData->presetID);

    u8 prob = 0;
    switch( gimmickType)
    {
    case GimmickEncount::GIMMICK_TYPE_STANDBY:
      prob = reinterpret_cast<GimmickEncount::StandbyData*>( pTypesData)->prob;
      break;
    case GimmickEncount::GIMMICK_TYPE_CHASE:
      prob = reinterpret_cast<GimmickEncount::ChaseData*>( pTypesData)->prob;
      break;
    case GimmickEncount::GIMMICK_TYPE_PATROL:
      prob = reinterpret_cast<GimmickEncount::PatrolData*>( pTypesData)->prob;
      break;
    case GimmickEncount::GIMMICK_TYPE_ESCAPE:
      prob = reinterpret_cast<GimmickEncount::EscapeData*>( pTypesData)->prob;
      break;
    default:
      GFL_ASSERT( 0);
      prob = 0;
      break;
    };

    GimmickEncount::Lottery lottery;
    lottery.LotteryGimmickFlag( prob, gimmickFlagNo, pEventWork);
  }
}

/**
 * @brief このクラスに対してアクターを登録
 */
void Manager::RegistActor( IFieldGimmickEncountActorBase* pActor)
{
  for( u32 i = 0; i < GIMMICK_ENCOUNT_MAX; ++i)
  {
    if( m_pActor[i] == NULL)
    {
      m_pActor[i] = pActor;
      return;
    }
  }

  GFL_ASSERT_MSG( 0, "GimmickEncountManager Regist Failed.\n");
}

/**
 * @brief このクラスに対してアクターを削除
 */
void Manager::UnRegistActor( IFieldGimmickEncountActorBase* pActor)
{
  for( u32 i = 0; i < GIMMICK_ENCOUNT_MAX; ++i)
  {
    if( m_pActor[i] == pActor)
    {
      m_pActor[i] = NULL;
      break;
    }
  }
}

/**
 * @brief SEの更新
 */
void Manager::UpdateSE()
{
  f32 minLength = FLT_MAX;
  u32 playSeID = SMID_NULL;
  gfl2::math::Vector3 playerPos = m_work.GetPlayerPosition();
  for( u32 i = 0; i < GIMMICK_ENCOUNT_MAX; ++i)
  {
    if( m_localWork[i].IsActive() == false)
    {
      continue;
    }

    u32 seID = SMID_NULL;
    if( m_localWork[i].IsPlaySE( &seID) == false)
    {
      continue;
    }

    gfl2::math::Vector3 pos = m_localWork[i].GetNowPosition();
    f32 length = (playerPos - pos).Length();
    if( length < minLength)
    {
      minLength = length;
      playSeID = seID;
    }
  }

  // @fix GFNMCat[930] ギミックエンカウントのSEと見た目があっていない。
  // アニメーションフレームとSEの尺やギミックエンカウントの状態遷移次第で、
  // 再生プレイヤーに空きがあるのにSEが鳴らないタイミングが発生していたので後勝ち方式に変更
  // SEIDが無効値でなかったら再生する
  // そのSEが再生されていた場合は、停止させてから再生する
  if( playSeID != SMID_NULL )
  {
    if( Sound::IsSEPlaying( playSeID) )
    {
      Sound::StopSE( playSeID, Sound::BGM_FADE_VERY_FAST);
    }
    Sound::PlaySE( playSeID);
  }
}

GFL_NAMESPACE_END( GimmickEncount )
GFL_NAMESPACE_END( Field )

