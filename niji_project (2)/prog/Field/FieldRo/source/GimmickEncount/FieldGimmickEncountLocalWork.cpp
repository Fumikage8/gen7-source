//======================================================================
/**
 * @file FieldGimmickEncountLocalWork.cpp
 * @date 2015/09/04 16:59:34
 * @author saita_kazuki
 * @brief ギミックエンカウント各々がアクセスできるワーク
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountLocalWork.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldGimmickEncountActorBase.h"
#include "Sound/include/Sound.h"

#include <debug/include/gfl2_Assert.h>

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( GimmickEncount )

/**
 * @brief コンストラクタ
 */
LocalWork::LocalWork()
  : m_isActive( false)
  , m_notifyState( NOTIFY_STATE_UNUSE)
  , m_notifyStateOld( NOTIFY_STATE_UNUSE)
  , m_pTypesData( NULL)
  , m_gimmickType( GIMMICK_TYPE_INVALID)
  , m_gimmickFlagNo( 0)
  , m_firstPosition( gfl2::math::Vector3::GetZero())
  , m_nowPosition( gfl2::math::Vector3::GetZero())
  , m_pActor( NULL)
  , m_nowSEID( SMID_NULL)
  , m_isPlaySE( false)
{
}

/**
 * @brief デストラクタ
 */
LocalWork::~LocalWork()
{
}

/**
 * @brief 初期化
 * @param desc 初期化情報
 */
void LocalWork::Initialize( const InitializeDescription& desc)
{
  m_isActive = true;
  m_notifyState = NOTIFY_STATE_NONE;
  m_pTypesData = desc.pTypesData;
  m_gimmickType = desc.gimmickType;
  m_gimmickFlagNo = desc.gimmickFlagNo;
  m_firstPosition = desc.firstPosition;
  m_pActor = desc.pActor;
  m_nowSEID = SMID_NULL;
  m_isPlaySE = false;
}

/**
 * @brief 更新
 * @param desc 更新情報
 */
void LocalWork::Update( const UpdateDescription& desc)
{
  m_nowPosition = desc.nowPosition;
}

/**
 * @brief クリア
 */
void LocalWork::Clear()
{
  m_isActive = false;
  m_notifyState = NOTIFY_STATE_UNUSE;
  m_pTypesData = NULL;
  m_gimmickType = GIMMICK_TYPE_INVALID;
  m_gimmickFlagNo = 0;
  m_firstPosition = gfl2::math::Vector3::GetZero();
  m_pActor = NULL;
  m_nowSEID = SMID_NULL;
  m_isPlaySE = false;
}

/**
 * @brief 破棄
 */
void LocalWork::Terminate()
{
  this->Clear();
}

/**
 * @brief イベントチェック前更新
 */
void LocalWork::EventCheckPreUpdate()
{
  if( m_isActive == false)
  {
    return;
  }

  m_notifyStateOld = m_notifyState;
  m_notifyState = NOTIFY_STATE_NONE;
}

/**
 * @brief イベントチェック後更新
 */
void LocalWork::EventCheckPostUpdate()
{
  if( m_isActive == false)
  {
    return;
  }
}

/**
 *  @brief イベント通知
 */
void LocalWork::Notify( NotifyState notify)
{
  if( m_isActive == false)
  {
    return;
  }

  m_notifyState = notify;
}

/**
 *  @brief SEパラメーターセット
 */
void LocalWork::SetSEParameter( bool isPlaySE, u32 nowSEID)
{
  m_isPlaySE = isPlaySE;
  m_nowSEID = nowSEID;
}

/**
 *  @brief SEを鳴らすか
 */
bool LocalWork::IsPlaySE( u32* pOutSEID) const
{
  *pOutSEID = m_nowSEID;

  // SEラベルが無効値ならfalseを返す
  if( m_nowSEID == SMID_NULL)
  {
    return false;
  }

  // SEラベルが設定されていればフラグから返す
  return m_isPlaySE;
}

GFL_NAMESPACE_END( GimmickEncount )
GFL_NAMESPACE_END( Field )

