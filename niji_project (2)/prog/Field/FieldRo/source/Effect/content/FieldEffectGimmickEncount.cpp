//=================================================================================================
/**
*  @file   FieldEffectGimmickEncount.cpp
*  @brief  ギミックエンカウントの定義
*  @author saita_kazuki
*  @date   2015.10.06
*/
//=================================================================================================

// gfl2
#include <fs/include/gfl2_BinLinkerAccessor.h>
// effect
#include "Field/FieldRo/include/Effect/content/FieldEffectGimmickEncount.h"
#include "Field/FieldRo/include/Effect/FieldEffectResource_define.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

/**
 *  @brief  コンストラクタ
 */
EffectGimmickEncount::EffectGimmickEncount( void )
  : IEffectBase()
  , m_pEffectHeap( NULL)
  , m_pEffectSystem( NULL)
  , m_pEffectHandle( NULL)
  , m_vPos( gfl2::math::Vector3::GetZero())
{
}

/**
 *  @brief  デストラクタ
 */
EffectGimmickEncount::~EffectGimmickEncount( void )
{
}

/**
 *  @brief  初期化
 */
void EffectGimmickEncount::Initialize( SetupData &setupData )
{
  m_pEffectHeap   = setupData.pEffectHeap;
  m_pEffectSystem = setupData.pEffectSystem;
  m_vPos          = setupData.vPos;

  m_pEffectHandle = GFL_NEW( m_pEffectHeap->GetGFHeapBase() ) gfl2::Effect::Handle;
}

/**
 *  @brief  破棄
 */
bool EffectGimmickEncount::Terminate( void )
{
  // 再生停止
  if( m_pEffectHandle && m_pEffectHandle->GetEmitterSet() )
  {
    m_pEffectHandle->GetEmitterSet()->Kill();
  }
  GFL_SAFE_DELETE( m_pEffectHandle );

  if( m_pEffectSystem)
  {
    m_pEffectSystem = NULL;
  }
  if( m_pEffectHeap)
  {
    m_pEffectHeap = NULL;
  }

  return true;
}

/**
 *  @brief  各エフェクトで必要な破棄作業
 */
void EffectGimmickEncount::Delete( void )
{
  // 再生終了しているなら削除
  if( m_pEffectHandle->GetEmitterSet() )
  {
    m_pEffectHandle->GetEmitterSet()->Kill();
  }
}

/**
 *  @brief  エフェクト生成
 */
void EffectGimmickEncount::Create( EffectType type )
{
  // 再生終了しているなら削除
  if( m_pEffectHandle->GetEmitterSet() &&
      m_pEffectHandle->GetEmitterSet()->IsAlive() == false
    )
  {
    m_pEffectHandle->GetEmitterSet()->Kill();
  }

  // 再生可能状態か
  bool isAlive = true;
  if( m_pEffectHandle->GetEmitterSet() == NULL )
  {
    isAlive = false;
  }
  else if( m_pEffectHandle->GetEmitterSet()->IsAlive() == false )
  {
    isAlive = false;
  }

  if( !isAlive )
  {
    switch( type)
    {
    case EFFECT_TYPE_SCATTER_GRASS_TREE_0:
      m_pEffectSystem->CreateEmitterSetID( m_pEffectHandle, m_vPos, 0, EFFECT_RESOURCE_SCATTER_GRASS_TREE_0 );
      break;

    case EFFECT_TYPE_SCATTER_GRASS_PLANT_0:
      m_pEffectSystem->CreateEmitterSetID( m_pEffectHandle, m_vPos, 0, EFFECT_RESOURCE_SCATTER_GRASS_PLANT_0 );
      break;

    default:
      GFL_ASSERT( 0);
      break;
    }
  }
}

//----------------------------------------------------------------
// protected
//----------------------------------------------------------------

/**
 *  @brief  更新処理
 */
void EffectGimmickEncount::updateNormal( void )
{
  // 再生が終わったら自身を切る
  nw::eft::EmitterSet* pEmitterSet = m_pEffectHandle->GetEmitterSet();
  if( pEmitterSet && pEmitterSet->IsAlive() == false )
  {
    RequestDelete();
  }
}

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)
