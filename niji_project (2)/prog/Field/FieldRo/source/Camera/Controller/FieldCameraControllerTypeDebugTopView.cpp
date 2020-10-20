//==============================================================================
/**
 * @file FieldCameraControllerTypeDebugTopView.cpp
 * @brief デバッグ用トップビューカメラ
 * @author saita_kazuki
 * @date 2015/11/23
 */
// =============================================================================

#if PM_DEBUG

#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerTypeDebugTopView.h"

#if defined(GF_ENABLE_DEBUG_EDITOR)

// gfl2
#include <math/include/gfl2_SRT.h>

// field
#include "Field/FieldStatic/include/Field3DObjectNode.h"

// gamesys
#include "GameSys/include/GameManager.h"

// 前方宣言
GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Camera )
GFL_NAMESPACE_BEGIN( Controller )

/**
* @brief コンストラクタ
*
* @param 無し
*
* @return 無し
*/ 
ControllerTypeDebugTopView::ControllerTypeDebugTopView()
  : m_distance( 1000.0f) // 10メートル
  , m_pTargetObject( NULL)
  , m_pTargetPosition( NULL)
{
}

/**
* @brief デストラクタ
*
* @return 無し
*/
ControllerTypeDebugTopView::~ControllerTypeDebugTopView( void )
{
}

/**
* @brief 初期化処理
* @note  ControllerがCameraUnitの設定された直後に実行される
*
* @param  pCamera 操作するカメラの実体
*
* @return 無し
*/
void ControllerTypeDebugTopView::SetUp( poke_3d::model::BaseCamera* pCamera )
{
}

/**
* @brief 外したときの処理
* @note  ControllerがCameraUnitから交換される直前に実行される
*
* @param  pCamera 操作するカメラの実体
*
* @return 無し
*/
void ControllerTypeDebugTopView::Remove( poke_3d::model::BaseCamera* pCamera )
{
}

/**
* @brief 更新処理
*
* @param  pCamera 操作するカメラの実体
*
* @return 無し
*/
void ControllerTypeDebugTopView::Update( poke_3d::model::BaseCamera* pCamera )
{
  gfl2::math::Vector3 targetPosition  = this->GetTargetPosition();
  gfl2::math::Vector3 cameraPosition  = gfl2::math::Vector3( targetPosition.x, targetPosition.y + m_distance, targetPosition.z);

  pCamera->SetupCameraLookAt(
    cameraPosition,
    targetPosition,
    gfl2::math::Vector3( 0.0f, 0.0f, 1.0f)
    );
}

/**
* @brief ターゲットオブジェクトを設定
*
* @param  注視点を取得するフィールド3Dオブジェクト
*
* @return 無し
*/
void ControllerTypeDebugTopView::SetTargetObject( Field::IField3DObjectNode* pObject)
{
  m_pTargetObject = pObject;
}

/**
* @brief ターゲット位置を設定
*
* @param  注視点とする位置
*
* @return 無し
*/
void ControllerTypeDebugTopView::SetTargetPosition( gfl2::math::Vector3* pPosition)
{
  m_pTargetPosition = pPosition;
}

/**
* @brief 注視点とカメラ位置の距離を設定
*
* @param 距離
*
* @return 無し
*/
void ControllerTypeDebugTopView::SetDistance( f32 distance)
{
  m_distance = distance;
}

/**
* @brief 注視点を取得
*
* @param  無し
*
* @return 注視点
*/
gfl2::math::Vector3 ControllerTypeDebugTopView::GetTargetPosition()
{
  if( m_pTargetObject)
  {
    return m_pTargetObject->GetLocalSRT().translate;
  }

  if( m_pTargetPosition)
  {
    return *m_pTargetPosition;
  }

  return gfl2::math::Vector3::GetZero();
}

GFL_NAMESPACE_END( Controller )
GFL_NAMESPACE_END( Camera )
GFL_NAMESPACE_END( Field )

#endif // #if defined(GF_ENABLE_DEBUG_EDITOR)
#endif // PM_DEBUG

