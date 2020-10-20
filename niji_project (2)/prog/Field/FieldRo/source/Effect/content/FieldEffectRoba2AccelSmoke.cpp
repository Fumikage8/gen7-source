//======================================================================
/**
 * @file FieldEffectRoba2AccelSmoke.cpp
 * @date 2015/11/13 12:55:04
 * @author miyachi_soichi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
// effect
#include "Field/FieldRo/include/Effect/content/FieldEffectRoba2AccelSmoke.h"
#include "Field/FieldRo/include/Effect/FieldEffectResource_define.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

/**
 *  @brief  コンストラクタ
 */
EffectRoba2AccelSmoke::EffectRoba2AccelSmoke( void )
: IEffectBase()
, m_pEffectHeap( NULL )
, m_pEffectSystem( NULL )
, m_pEffectHandle( NULL )
{
}

/**
 *  @brief  デストラクタ
 */
EffectRoba2AccelSmoke::~EffectRoba2AccelSmoke( void )
{
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  初期化
 */
void EffectRoba2AccelSmoke::Initialize( SetupData &setupData )
{
  m_pEffectHeap = setupData.pEffectHeap;
  m_pEffectSystem = setupData.pEffectSystem;
  m_pEffectHandle = GFL_NEW( m_pEffectHeap->GetGFHeapBase() ) gfl2::Effect::Handle;
}

/**
 *  @brief  破棄
 */
bool EffectRoba2AccelSmoke::Terminate( void )
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
 *  @brief  オフセットを設定
 */
void EffectRoba2AccelSmoke::SetOffset( gfl2::math::Vector3 offset,gfl2::math::Vector3 offsetQua )
{
  // エミッター
  if( m_pEffectHandle && m_pEffectHandle->GetEmitterSet() && m_pEffectHandle->GetEmitterSet()->IsAlive() )
  {
    // trans
    nw::math::VEC3 nwPos;
    m_pEffectHandle->GetEmitterSet()->GetPos(nwPos);
    nwPos.x = nwPos.x + offset.x;
    nwPos.y = nwPos.y + offset.y;
    nwPos.z = nwPos.z + offset.z;

    // rotate
    nw::math::VEC3 nwRot( offsetQua.x,offsetQua.y,offsetQua.z );

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
*  @brief  指定位置で再設定
*/
void EffectRoba2AccelSmoke::Reset( gfl2::math::Vector3 pos, gfl2::math::Vector3 offsetQua )
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
    m_pEffectSystem->CreateEmitterSetID( m_pEffectHandle, pos, 0, EFFECT_RESOURCE_ROBA2_ACCEL_SMOKE );

    if( m_pEffectHandle->GetEmitterSet() && m_pEffectHandle->GetEmitterSet()->IsAlive() )
    {
      // trans
      nw::math::VEC3 nwPos;
      m_pEffectHandle->GetEmitterSet()->GetPos(nwPos);

      // rotate
      nw::math::VEC3 nwRot( offsetQua.x,offsetQua.y,offsetQua.z );

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
}


//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Protected Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  更新処理
 */
void EffectRoba2AccelSmoke::updateNormal( void )
{
}

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)