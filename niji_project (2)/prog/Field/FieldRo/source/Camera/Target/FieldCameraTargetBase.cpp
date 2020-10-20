/**
* @brief  フィールドカメラのターゲット基底クラス
* @file   FieldCameraTargetBase.h
* @author ikuta_junya
* @data   2015.05.21
*/

// field
#include "Field/FieldRo/include/Camera/Target/FieldCameraTargetBase.h"

// debug
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"


// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Target );

/**
* @brief コンストラクタ
*
* @return 無し
*/ 
TargetBase::TargetBase( void ) :
  m_IsStackSetUp( false ),
  m_IsOverWrite( false ),
  m_OverWritePositon( gfl2::math::Vector3::GetZero() )
{
}

/**
* @brief デストラクタ
*
* @return 無し
*/
TargetBase::~TargetBase( void )
{

}

/**
* @brief カメラがみる座標を更新する
*
* @return 無し
*/
void TargetBase::Update( void )
{

}

/**
* @brief ターゲット座標を強制的に上書きする
*
* @param  rPos  上書き座標
*
* @return 無し
*/
void TargetBase::OverWrite( const gfl2::math::Vector3& rPos )
{
  m_IsOverWrite = true;
  m_OverWritePositon = rPos;
}


/**
* @brief ターゲット座標の上書きを取得
*
* @param  pPos  結果
*
* @return 無し
*/
void TargetBase::GetOverWrite( gfl2::math::Vector3* pPos ) const
{
  *pPos = m_OverWritePositon;
}

/**
* @brief ターゲット座標を強制的に上書きを無効にする
*
* @return 無し
*/
void TargetBase::ClearOverWrite( void )
{
  m_IsOverWrite = false;
  m_OverWritePositon = gfl2::math::Vector3::GetZero();
}



GFL_NAMESPACE_END( Target );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );