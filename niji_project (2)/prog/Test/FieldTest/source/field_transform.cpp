//=================================================================================================
/**
 *	@file field_transform.cpp
 *	@brief
 *	@date 2014.03.07
 *	@author tamada@gamefreak.co.jp
 */
//=================================================================================================
#include "../include/field_transform.h"

//-----------------------------------------------------------------------------
/**
*	@brief	コンストラクタ
*/
//-----------------------------------------------------------------------------
FieldTransform::FieldTransform()
: m_trans		(gfl2::math::Vector::GetZero())
, m_transEffect (gfl2::math::Vector::GetZero())
, m_scale		(gfl2::math::Vector::GetZero())
, m_quaternion	(0.0f,0.0f,0.0f,1.0f)
, m_worldMtx	(gfl2::math::Matrix::GetIdentity())
, m_isDirty(true)
{
}

//-----------------------------------------------------------------------------
/**
*	@brief	デストラクタ
*/
//-----------------------------------------------------------------------------
FieldTransform::~FieldTransform()
{
}

//----------------------------------------------------------------------------
/**
*	@brief  行列更新(座標→スケール→回転)
*/
//-----------------------------------------------------------------------------
void FieldTransform::Update( void )
{
  if( m_isDirty ){
	  gfl2::math::Matrix mtxTrans  = gfl2::math::Matrix::GetTranslation( m_trans );
	  gfl2::math::Matrix mtxScale  = gfl2::math::Matrix::GetScaling( m_scale );
	  gfl2::math::Matrix mtxRotate;
	  m_quaternion.QuaternionToMatrix(mtxRotate);
	  gfl2::math::Matrix mtxWorld  =  mtxTrans * mtxRotate * mtxScale;
	  m_worldMtx = mtxWorld;
    m_isDirty = false;
  }
}

//----------------------------------------------------------------------------
/**
*	@brief  SRT取得
*/
//-----------------------------------------------------------------------------
gfl2::math::SRT FieldTransform::GetSRT() const
{
	gfl2::math::SRT srt;
	srt.scale.Set(m_scale.x, m_scale.y, m_scale.z);
	srt.rotation	= m_quaternion;
	srt.translate.Set(m_trans.x, m_trans.y, m_trans.z);
	return srt;
}
