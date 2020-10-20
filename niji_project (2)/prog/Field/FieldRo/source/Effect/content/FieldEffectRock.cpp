//=================================================================================================
/**
*  @file   FieldEffectRock.cpp
*  @brief  岩の定義
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================

// gfl2
#include <fs/include/gfl2_BinLinkerAccessor.h>
// effect
#include "Field/FieldRo/include/Effect/content/FieldEffectRock.h"
#include "Field/FieldRo/include/Effect/FieldEffectResource_define.h"
// model
#include "Field/FieldRo/include/StaticModel/StaticModel.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

/**
 *  @class  EffectRock
 *  @brief  岩
 */

/**
 *  @brief  コンストラクタ
 */
EffectRock::EffectRock( void )
: IEffectBase()
, m_pEffectHeap  ( NULL )
, m_pEffectSystem( NULL )
, m_isCreateRock ( false )
, m_pEffectHandle( NULL )
, m_pParent      ( NULL )
{
}

/**
 *  @brief  デストラクタ
 */
EffectRock::~EffectRock( void )
{
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  初期化
 */
void EffectRock::Initialize( SetupData &setupData )
{
  m_pEffectHeap   = setupData.pEffectHeap;
  m_pEffectSystem = setupData.pEffectSystem;
  m_pEffectHandle = GFL_NEW( m_pEffectHeap->GetGFHeapBase() ) gfl2::Effect::Handle;

  Field::StaticModel::StaticModel* pStaticModel = setupData.pParentObject->SafeCast<Field::StaticModel::StaticModel>();
  if( pStaticModel->GetBaseModel(0) )
  {
    m_pParent = pStaticModel->GetBaseModel(0);
  }
}

/**
 *  @brief  破棄
 */
bool EffectRock::Terminate( void )
{
  // 再生停止
  if( m_isCreateRock && m_pEffectHandle->GetEmitterSet() )
  {
    m_pEffectHandle->GetEmitterSet()->Kill();
  }
  GFL_SAFE_DELETE( m_pEffectHandle );
  if( m_pEffectSystem && m_pEffectHeap )
  {
    m_pEffectSystem = NULL;
    m_pEffectHeap = NULL;
  }
  return true;
}

/**
 *  @brief  各エフェクトで必要な破棄作業
 */
void EffectRock::Delete( void )
{
}

/**
 *  @brief  岩 開始
 */
void EffectRock::CreateRock( b32 isGround )
{
  // 再生可能状態か
  if( !m_isCreateRock )
  {
    gfl2::math::Vector3 pos = m_pParent->GetPosition();
    if( isGround ) { m_pEffectSystem->CreateEmitterSetID( m_pEffectHandle, pos, 0, EFFECT_RESOURCE_ROCK_W ); }
    else           { m_pEffectSystem->CreateEmitterSetID( m_pEffectHandle, pos, 0, EFFECT_RESOURCE_ROCK_G ); }
    m_isCreateRock = true;
  }
}

/**
 *  @brief  岩 終了
 */
void EffectRock::DeleteRock( void )
{
  // 再生終了しているなら削除
  if( m_isCreateRock && m_pEffectHandle->GetEmitterSet() )
  {
    m_pEffectHandle->GetEmitterSet()->Kill();
    m_isCreateRock = false;
  }
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Protected Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  更新処理
 */
void EffectRock::updateNormal( void )
{
  // 座標更新
  updatePosition();

  // 再生が終わったら自身を切る
  if( !m_pEffectHandle->GetEmitterSet() )
  {
    RequestDelete();
  }
  else if( !m_pEffectHandle->GetEmitterSet()->IsAlive()  )
  {
    RequestDelete();
  }
}

/**
 *  @brief  座標更新
 */
void EffectRock::updatePosition( void )
{
  if( m_pEffectHandle && m_pEffectHandle->GetEmitterSet() && m_pEffectHandle->GetEmitterSet()->IsAlive() )
  {
    gfl2::math::Vector3 pos = m_pParent->GetPosition();
    nw::math::VEC3 nw_pos( pos.x, pos.y, pos.z );
    m_pEffectHandle->GetEmitterSet()->SetPos( nw_pos );

    gfl2::math::Quaternion qua = m_pParent->GetRotationQuat();
    gfl2::math::Vector3    unitZ(0.0f,0.0f,-1.0f); unitZ = qua.Transform(unitZ);
    nw::math::VEC3         nwUnitZ(unitZ.x,unitZ.y,unitZ.z);
    m_pEffectHandle->GetEmitterSet()->SetDirectional( nwUnitZ );
  }
}

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)
