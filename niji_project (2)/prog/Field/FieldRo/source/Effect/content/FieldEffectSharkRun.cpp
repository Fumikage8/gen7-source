//======================================================================
/**
 * @file FieldEffectSharkRun.cpp
 * @date 2015/09/01 17:32:39
 * @author miyachi_soichi
 * @brief サメハダーの走りしぶき
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
// gfl2
#include <fs/include/gfl2_BinLinkerAccessor.h>
// effect
#include "Field/FieldRo/include/Effect/content/FieldEffectSharkRun.h"
#include "Field/FieldRo/include/Effect/FieldEffectResource_define.h"
// model
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

/**
 *  @class  EffectSharkRun
 *  @brief  サメハダーの走りしぶき
 */
/**
 *  @brief  コンストラクタ
 */
EffectSharkRun::EffectSharkRun( void )
: IEffectBase()
, m_pEffectHeap( NULL )
, m_pEffectSystem( NULL )
{
}

/**
 *  @brief  デストラクタ
 */
EffectSharkRun::~EffectSharkRun( void )
{
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  初期化
 */
void EffectSharkRun::Initialize( SetupData &setupData )
{
  m_pEffectHeap = setupData.pEffectHeap;
  m_pEffectSystem = setupData.pEffectSystem;

  {
    Field::MoveModel::FieldMoveModel *pMoveModel = setupData.pParentObject->SafeCast<Field::MoveModel::FieldMoveModel>();
    m_pParent = pMoveModel->GetCharaDrawInstance();
  }

  m_pEffectHandle = GFL_NEW( m_pEffectHeap->GetGFHeapBase() ) gfl2::Effect::Handle;

}

/**
 *  @brief  破棄
 */
bool EffectSharkRun::Terminate( void )
{
  // 再生停止
  if( m_pEffectHandle->GetEmitterSet() )
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
void EffectSharkRun::Delete( void )
{
}

/**
 *  @brief  エミッター作成
 */
void EffectSharkRun::CreateEmitter( void )
{
  // 再生終了しているなら削除
  if( m_pEffectHandle->GetEmitterSet()
  && !m_pEffectHandle->GetEmitterSet()->IsAlive() )
  {
    m_pEffectHandle->GetEmitterSet()->Kill();
  }

  // 再生可能状態か
  bool isAlive = true;
  if( !m_pEffectHandle->GetEmitterSet() )
  {
    isAlive = false;
  }
  else if( !m_pEffectHandle->GetEmitterSet()->IsAlive() )
  {
    isAlive = false;
  }

  // 再生できる
  if( !isAlive )
  {
    gfl2::math::Vector3 pos(0.0f,0.0f,0.0f);
    m_pEffectSystem->CreateEmitterSetID( m_pEffectHandle, pos, 0, EFFECT_RESOURCE_SHARK_RUN );
    updatePosition();
  }
}

/**
 *  @brief  エミッター削除
 */
void EffectSharkRun::DeleteEmitter( void )
{
  // 再生終了しているなら削除
  if( m_pEffectHandle->GetEmitterSet() && m_pEffectHandle->GetEmitterSet()->IsAlive() )
  {
    m_pEffectHandle->GetEmitterSet()->Fade();
  }
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Protected Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  更新処理
 */
void EffectSharkRun::updateNormal( void )
{
  // 座標更新
  updatePosition();
}

/**
 *  @brief  座標更新
 */
void EffectSharkRun::updatePosition( void )
{
  if( m_pEffectHandle && m_pEffectHandle->GetEmitterSet() && m_pEffectHandle->GetEmitterSet()->IsAlive() )
  {
    gfl2::math::Vector3 offset( m_pParent->GetRotationQuat().Transform(m_offsetPosition) );
    gfl2::math::Vector3 pos = m_pParent->GetPosition() + offset;
    nw::math::VEC3 nwPos( pos.x, pos.y, pos.z );

    // rotate
    f32 x, y, z;
    gfl2::math::Quaternion qua = m_pParent->GetRotationQuat();
    qua.QuaternionToRadian( &x, &y, &z );
    nw::math::VEC3 nwRot  ( x, y, z );

    // scale
    gfl2::math::VEC3 nwScale(1.0f,1.0f,1.0f);

    // 設定
    nw::math::MTX34 srt;
#if defined(GF_PLATFORM_CTR)
    nw::eft::MTX34MakeSRT(&srt,nwScale,nwRot,nwPos);
#elif defined(GF_PLATFORM_WIN32)
    nw::math::MTX34MakeSRT(&srt,nwScale,nwRot,nwPos);
#endif  //プラットフォーム分岐
    m_pEffectHandle->GetEmitterSet()->SetMtx( srt );
  }
}

/**
 *  @brief  座標オフセットの設定
 */
void EffectSharkRun::setOffsetPosition( gfl2::math::Vector3 offsetPos )
{
  m_offsetPosition = offsetPos;
}

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)
