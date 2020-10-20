/**
* @brief  フィールドカメラのターゲット、動作モデル
* @file   FieldCameraTargetTypeMoveModel.cpp
* @author ikuta_junya
* @data   2015.05.21
*/


// field
#include "ViewerProc/include/FieldStatic/MoveModel/FieldMoveModel.h"
#include "ViewerProc/include/FieldRo/Camera/Target/FieldCameraTargetTypeMoveModel.h"

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Target );


/**
* @brief コンストラクタ
*
* @param  pMoveModel 中身、寿命は管理しない
*
* @return 無し
*/ 
TargetTypeMoveModel::TargetTypeMoveModel( MoveModel::FieldMoveModel* pMoveModel ) :
  m_pMoveModel( pMoveModel )
{


}

/**
* @brief デストラクタ
*
* @return 無し
*/
TargetTypeMoveModel::~TargetTypeMoveModel( void )
{

}

/**
* @brief カメラがみる座標を取得する
*
* @param  pOut  格納先
*
* @return 成否
*/
b32 TargetTypeMoveModel::GetCameraTargetPosition( gfl2::math::Vector3* pOut ) const
{
  if( m_pMoveModel == NULL )
  {
    return false;
  }
  
  poke_3d::model::CharaModel* pCharaModel = m_pMoveModel->GetCharaDrawInstance();
  if( pCharaModel == NULL )
  {
    return false;
  }

  *pOut = pCharaModel->GetPosition();
  pOut->y += 133.0f;  // @todo 仮
  return true;
}

/**
* @brief 中身を設定
*
* @param  pMoveModel 中身、寿命は管理しない
*
* @return 無し
*/ 
void TargetTypeMoveModel::SetContent( MoveModel::FieldMoveModel* pMoveModel )
{
  m_pMoveModel = pMoveModel;
}



GFL_NAMESPACE_END( Target );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );