//======================================================================
/**
 * @file FieldEffectBaseParticle.cpp
 * @date 2016/02/08 19:03:55
 * @author miyachi_soichi
 * @brief エフェクト：パーティクル単体呼出
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// effect
#include "Field/FieldRo/include/Effect/content/FieldEffectBaseParticle.h"
// sound
#include "Sound/include/Sound.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

/**
 *  @class  EffectBaseParticle
 *  @brief  エフェクト：モンスターボール演出
 */
/**
 *  @brief  コンストラクタ
 */
EffectBaseParticle::EffectBaseParticle( void )
: IEffectBase()
, m_pEffectHeap( NULL )
, m_pEffectSystem( NULL )
, m_pEffectHandle( NULL )
, m_bSuicide( false )
{
}

/**
 *  @brief  デストラクタ
 */
EffectBaseParticle::~EffectBaseParticle( void )
{
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  初期化
 */
void EffectBaseParticle::Initialize( SetupData &setupData )
{
  m_vPosition = setupData.position;
  m_pEffectHeap = setupData.pEffectHeap;
  m_pEffectSystem = setupData.pEffectSystem;
  m_pEffectHandle = GFL_NEW( m_pEffectHeap->GetGFHeapBase() ) gfl2::Effect::Handle;
  m_bSuicide = setupData.bSuicide;

  m_pEffectSystem->CreateEmitterSetID( m_pEffectHandle, m_vPosition, 0, setupData.eResource );

  if( m_pEffectHandle && m_pEffectHandle->GetEmitterSet() )
  {
    // trans
    nw::math::VEC3 nwPos( m_vPosition.x, m_vPosition.y, m_vPosition.z );
    // rot
    nw::math::VEC3 nwRot( setupData.rotation.x, setupData.rotation.y, setupData.rotation.z );
    // scale
    nw::math::VEC3 nwScale( 1.0f, 1.0f, 1.0f );
    // 設定
    nw::math::MTX34 srt;
#if defined(GF_PLATFORM_CTR)
    nw::eft::MTX34MakeSRT(&srt,nwScale,nwRot,nwPos);
#elif defined(GF_PLATFORM_WIN32)
    nw::math::MTX34MakeSRT(&srt,nwScale,nwRot,nwPos);
#endif
    m_pEffectHandle->GetEmitterSet()->SetMtx( srt );
  }

  if( IsPlaySe() && setupData.nSe != 0 )
  {
    Sound::PlaySE( setupData.nSe );
  }
}

/**
 *  @brief  破棄
 */
bool EffectBaseParticle::Terminate( void )
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

/**
 *  @brief  終了しているか.
 */
bool EffectBaseParticle::IsAnimationLastFrame( void )
{
  if( !m_pEffectHandle ){ return true; }
  if( !m_pEffectHandle->GetEmitterSet() ){ return true; }
  if( !m_pEffectHandle->GetEmitterSet()->IsAlive() ){ return true; }

  // まだ再生中
  return false;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Private Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  更新処理
 */
void EffectBaseParticle::updateNormal( void )
{
  if( m_bSuicide )
  {
    if( IsAnimationLastFrame() )
    {
      // 再生終了したので自殺する
      RequestDelete();
    }
  }
}

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)
