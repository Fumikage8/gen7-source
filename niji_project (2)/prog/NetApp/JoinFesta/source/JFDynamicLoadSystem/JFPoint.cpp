//==============================================================================
/**
 * @file	JFPoint.cpp
 * @brief	ジョインフェスタモデル設置ポイント
 * @author	ichiraku_katsuhiko
 * @date	2015/05/28 15:59:28
 */
// =============================================================================

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <math/include/gfl2_math_control.h>

// mine
#include "NetApp/JoinFesta/include/JFDynamicLoadSystem/JFPoint.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFesta)

// コンストラクタ
JFPoint::JFPoint() :
  m_index(INDEX_NULL)
  ,m_point( 0.0f, 0.0f, 0.0f )
  ,m_isModelView(false)
  ,m_pPersonalData(NULL)
  ,m_isSetPersonalData(false)
{
}

// デストラクタ
JFPoint::~JFPoint()
{
}

//------------------------------------------------------------------
/**
 * @brief   ポイントのインデックスを設定
 * @param   [in]index インデックス値
 */
//------------------------------------------------------------------
void JFPoint::SetPointIndex( const u32 index )
{
  //GFL_ASSERT( index < JoinFestaDefine::JFPOINT_MAX );
  //gfl2::math::Clamp( index, (u32)0, JoinFestaDefine::JFMODEL_DISP_MAX );
  m_index = index;
  ICHI_PRINT("JFPoint::SetPointIndex Index:%d \n", m_index );
}

//------------------------------------------------------------------
/**
 * @brief   ポイントのインデックスを取得
 * @return  インデックス値
 */
//------------------------------------------------------------------
u32 JFPoint::GetPointIndex() const
{
  return m_index;
}

//------------------------------------------------------------------
/**
 * @brief   ポイントの座標を設定
 * @param   [in]pos 座標
 */
//------------------------------------------------------------------
void JFPoint::SetPointPosition( const gfl2::math::Vector3 pos )
{
  ICHI_PRINT("JFPoint[%d] pos(%f:%f)->pos(%f:%f)\n", m_index, m_point.x, m_point.z, pos.x, pos.z);
  m_point = pos;
}

//------------------------------------------------------------------
/**
 * @brief   ポイントの座標を取得
 * @return  座標
 */
//------------------------------------------------------------------
gfl2::math::Vector3 JFPoint::GetPointPosition() const
{
  return m_point;
}

//------------------------------------------------------------------
/**
 * @brief   モデル表示中か
 * @retval  true:表示中 
 * @retval  false:表示していない 
 */
//------------------------------------------------------------------
bool JFPoint::IsModelView()
{
  return m_isModelView;
}

//------------------------------------------------------------------
/**
 * @brief   モデル表示中に設定
 */
//------------------------------------------------------------------
void JFPoint::ModelViewOn()
{
  ICHI_PRINT("JFPoint::ModelViewOn Index:%d \n", m_index );
  m_isModelView = true;
}

//------------------------------------------------------------------
/**
 * @brief   モデル非表示中に設定
 */
//------------------------------------------------------------------
void JFPoint::ModelViewOff()
{
  ICHI_PRINT("JFPoint::ModelViewOff Index:%d \n", m_index );
  m_isModelView = false;
}

//------------------------------------------------------------------
/**
 * @brief   パーソナルデータを設定
 * @param   [in]pPersonal パーソナルデータのポインタ
 */
//------------------------------------------------------------------
void JFPoint::SetPersonalData( NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonal )
{
  m_pPersonalData = pPersonal;
  m_isSetPersonalData = true;
}

//------------------------------------------------------------------
/**
 * @brief   パーソナルデータを取得
 * @return  パーソナルデータ
 */
//------------------------------------------------------------------
NetAppLib::JoinFesta::JoinFestaPersonalData* JFPoint::GetPersonalData() 
{
  return m_pPersonalData;
}

//------------------------------------------------------------------
/**
 * @brief   パーソナルデータがセットされているか
 * @return  trueでセットされている
 */
//------------------------------------------------------------------
bool JFPoint::IsSetPersonalData() const
{
  return m_isSetPersonalData;
}

GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetApp)

