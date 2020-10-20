/**
 *  GAME FREAK inc.
 *
 *  @file   FieldEffectBase.cpp
 *  @brief  エフェクト基底
 *  @author miyachi_soichi
 *  @date   2015.50.20
 */

// gfl2
#include <macro/include/gfl2_Macros.h>

// Field
#include "Field/FieldRo/include/Effect/FieldEffectBase.h"

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Effect );

/**
 *  @brief  コンストラクタ
 */
IEffectBase::IEffectBase( void )
  : m_pParent( NULL )
  , m_eState( STATE_EXEC )
{
}

/**
 *  @brief  デストラクタ
 */
IEffectBase::~IEffectBase( void )
{
}

/**
 *  @brief  初期化処理：オブジェクト指定
 */
void IEffectBase::Initialize( void *in_pResource, gfl2::heap::HeapBase *in_pHeap, Field::IField3DObjectNode *in_pParent )
{
}

/**
 *  @brief  初期化処理：座標指定
 */
void IEffectBase::Initialize( void *in_pResource, gfl2::heap::HeapBase *in_pHeap, const gfl2::math::Vector3 &in_vPosition )
{
}

/**
 *  @brief  破棄処理
 */
bool IEffectBase::Terminate( void )
{
  return true;
}

/**
 *  @brief  更新処理
 */
void IEffectBase::Update( void )
{
  // 更新処理
  switch( m_eState )
  {
  case STATE_REQUEST_DELETE:
    m_eState = STATE_DELETE;
    break;
  case STATE_EXEC:
    updateNormal();
    break;
  default:
    break;
  }
}

/**
 *  @brief  破棄リクエスト
 */
void IEffectBase::RequestDelete( void )
{
  // 各エフェクトで必要な破棄作業を呼出
  Delete();
  // 破棄状態へ移行
  if( m_eState == STATE_EXEC )
  {
    m_eState = STATE_REQUEST_DELETE;
  }
}

/**
 *  @brief  破棄許可確認
 */
bool IEffectBase::IsSafeDelete( void ) const
{
  return m_eState == STATE_DELETE;
}

/**
 *  @brief  親オブジェクトの比較
 */
bool IEffectBase::IsSameParent( Field::IField3DObjectNode *in_pParent ) const
{
  // ポインタの一致をもって同一とする
  return m_pParent == in_pParent;
}

GFL_NAMESPACE_END( Effect );
GFL_NAMESPACE_END( Field );
