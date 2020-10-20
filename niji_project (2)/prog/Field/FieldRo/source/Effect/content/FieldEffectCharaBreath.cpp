//======================================================================
/**
 * @file FieldEffectCharaBreath.cpp
 * @date 2016/01/15 13:06:33
 * @author miyachi_soichi
 * @brief エフェクト：といき
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
// effect
#include "Field/FieldRo/include/Effect/content/FieldEffectCharaBreath.h"
#include "Field/FieldRo/include/Effect/FieldEffectResource_define.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

/**
 *  @brief  コンストラクタ
 */
EffectCharaBreath::EffectCharaBreath( void )
: IEffectBase()
, m_pEffectHeap( NULL )
, m_pEffectSystem( NULL )
, m_pEffectHandle( NULL )
{
}

/**
 *  @brief  デストラクタ
 */
EffectCharaBreath::~EffectCharaBreath( void )
{
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  初期化
 */
void EffectCharaBreath::Initialize( SetupData &setupData )
{
  m_pEffectHeap   = setupData.pEffectHeap;
  m_pEffectSystem = setupData.pEffectSystem;
  m_pEffectHandle = GFL_NEW( m_pEffectHeap->GetGFHeapBase() ) gfl2::Effect::Handle;
  m_pParent       = setupData.pParentObject->SafeCast<Field::MoveModel::FieldMoveModel>();

  // rootPos
  gfl2::math::Vector3 rootPos = m_pParent->GetLocalSRT().translate;

  // offsetTrans
  u32 mouthIndex = m_pParent->GetCharaDrawInstance()->GetJointIndex("Loc_Mouth");
  m_offsetPos    = m_pParent->GetCharaDrawInstance()->GetJointWorldMatrix(mouthIndex).GetElemPosition() - rootPos;

  m_pEffectSystem->CreateEmitterSetID( m_pEffectHandle, rootPos + m_offsetPos, 0, EFFECT_RESOURCE_CHARA_BREATH );

  updatePosition();
}

/**
 *  @brief  破棄
 */
bool EffectCharaBreath::Terminate( void )
{
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

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Protected Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  更新処理
 */
void EffectCharaBreath::updateNormal( void )
{
  if( !m_pEffectHandle 
   || !m_pEffectHandle->GetEmitterSet()
   || !m_pEffectHandle->GetEmitterSet()->IsAlive() )
  {
    RequestDelete();
  }
}

/**
 *  @brief  座標更新
 */
void EffectCharaBreath::updatePosition( void )
{
  if( m_pEffectHandle && m_pEffectHandle->GetEmitterSet() && m_pEffectHandle->GetEmitterSet()->IsAlive() )
  {
    // pos
    gfl2::math::Vector3 rootPos = m_pParent->GetLocalSRT().translate;
    nw::math::VEC3 nwPos( rootPos.x + m_offsetPos.x,
                          rootPos.y + m_offsetPos.y,
                          rootPos.z + m_offsetPos.z );

    // rotate
    f32 x, y, z;
    m_pParent->GetLocalSRT().rotation.QuaternionToRadian( &x, &y, &z );
    nw::math::VEC3 nwRot( x, y, z );

    // scale
    gfl2::math::VEC3 nwScale(1.0f,1.0f,1.0f);

    // 設定
    nw::math::MTX34 srt;
#if defined(GF_PLATFORM_CTR)
    nw::eft::MTX34MakeSRT(&srt,nwScale,nwRot,nwPos);
#elif defined(GF_PLATFORM_WIN32)
    nw::math::MTX34MakeSRT(&srt,nwScale,nwRot,nwPos);
#endif
    m_pEffectHandle->GetEmitterSet()->SetMtx( srt );
  }
}

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)
