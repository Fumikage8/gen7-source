//======================================================================
/**
 * @file FieldEffectKairikyRockSmoke2.cpp
 * @date 2017/01/19 15:25:13
 * @author kawazoe_shinichi
 * @brief カイリキ岩押しエフェクト（フェローチェマップの大きい岩専用）
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// effect
#include "Field/FieldRo/include/Effect/content/FieldEffectKairikyRockSmoke2.h"
#include "Field/FieldRo/include/Effect/FieldEffectResource_define.h"
// model
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

/**
 *  @class  EffectKairikyRockSmoke2
 *  @brief  カイリキ岩押しエフェクト（フェローチェマップの大きい岩専用）
 */
/**
 *  @brief  コンストラクタ
 */
 EffectKairikyRockSmoke2::EffectKairikyRockSmoke2(void)
: IEffectBase()
, m_pEffectHeap     ( NULL )
, m_pEffectSystem   ( NULL )
, m_pEffectHandle   ( NULL )
, m_pParent         ( NULL )
, m_quaternion      ( 0.0f,0.0f,0.0f,1.0f )
{
}

/**
 *  @brief  デストラクタ
 */
EffectKairikyRockSmoke2::~EffectKairikyRockSmoke2(void)
{
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  初期化
 */
void EffectKairikyRockSmoke2::Initialize(SetupData &setupData)
{
  m_pEffectHeap = setupData.pEffectHeap;
  m_pEffectSystem = setupData.pEffectSystem;
  m_pEffectHandle = GFL_NEW( m_pEffectHeap->GetGFHeapBase() ) gfl2::Effect::Handle;
  Field::MoveModel::FieldMoveModel *pMoveModel = setupData.pParentObject->SafeCast<Field::MoveModel::FieldMoveModel>();
  m_pParent = pMoveModel->GetCharaDrawInstance();
}

/**
 *  @brief  破棄
 */
bool EffectKairikyRockSmoke2::Terminate(void)
{
  EndKill();

  GFL_SAFE_DELETE( m_pEffectHandle );
  if( m_pEffectSystem && m_pEffectHeap )
  {
    m_pEffectSystem = NULL;
    m_pEffectHeap = NULL;
  }
  return true;
}

/**
 *  @brief  再生
 */
void EffectKairikyRockSmoke2::Start(void)
{
  if( !IsAlive() )
  {
    m_pEffectSystem->CreateEmitterSetID( m_pEffectHandle, m_pParent->GetPosition(), 0, EFFECT_RESOURCE_KAIRIKY_ROCK_SMOKE_L );
  }
}

/**
 *  @brief  停止
 */
void EffectKairikyRockSmoke2::EndKill(void)
{
  if( IsAlive() )
  {
    m_pEffectHandle->GetEmitterSet()->Kill();
  }
}

/**
 *  @brief  停止(フェード)
 */
void EffectKairikyRockSmoke2::EndFade(void)
{
  if( IsAlive() )
  {
    m_pEffectHandle->GetEmitterSet()->Fade();
  }
}

/**
 *  @brief  再生確認
 */
bool EffectKairikyRockSmoke2::IsAlive(void)
{
  if( !m_pEffectHandle->GetEmitterSet() )             { return false; }
  if( !m_pEffectHandle->GetEmitterSet()->IsAlive() )  { return false; }

  return true;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Protected Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  更新処理
 */
void EffectKairikyRockSmoke2::updateNormal(void)
{
  // エミッター
  if( m_pEffectHandle && m_pEffectHandle->GetEmitterSet() && m_pEffectHandle->GetEmitterSet()->IsAlive() )
  {
    gfl2::math::Vector3 vPos = m_pParent->GetPosition();
    gfl2::math::Quaternion quat = m_quaternion;

    // trans
    gfl2::math::Vector3 glPos( m_pParent->GetPosition()  );
    nw::math::VEC3      nwPos( glPos.x, glPos.y, glPos.z );

    // rotate
    f32 x, y, z;
    quat.QuaternionToRadian( &x, &y, &z );
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

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)
