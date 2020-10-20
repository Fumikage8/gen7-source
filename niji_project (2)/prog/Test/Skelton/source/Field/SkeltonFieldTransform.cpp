//=================================================================================================
/**
@file field_transform.cpp
@brief
@date 2014.03.07
@author tamada@gamefreak.co.jp

*/
//=================================================================================================

#if defined(GF_PLATFORM_WIN32)
#include "Test/Skelton/include/Field/SkeltonFieldTransform.h"

GFL_NAMESPACE_BEGIN(Test)
  GFL_NAMESPACE_BEGIN(Skelton)


  SkeltonFieldTransform::SkeltonFieldTransform():
  m_trans(gfl2::math::Vector4::GetZero()),
  m_scale(gfl2::math::Vector4::GetZero()),
  m_rot(gfl2::math::Vector4::GetZero()),
  m_offset(gfl2::math::Vector4::GetZero()),
  m_worldMtx(gfl2::math::Matrix::GetIdentity())
{
}

SkeltonFieldTransform::~SkeltonFieldTransform()
{
}


//-----------------------------------------------------------------------------------------------
//　ワールド行列を設定
//-----------------------------------------------------------------------------------------------

void SkeltonFieldTransform::SetWorldMatrix(const gfl2::math::Matrix & mtx)
{
  m_worldMtx = mtx;
}

gfl2::math::Matrix SkeltonFieldTransform::GetWorldMatrix( void ) const
{
  return m_worldMtx;
}

void SkeltonFieldTransform::Clear( void )
{	
  static const gfl2::math::Vector4 scale( 1.0f, 1.0f, 1.0f );
  static const gfl2::math::Vector4 data( 0.f, 0.f, 0.f, 1.0f );
  //	static const gfl2::math::Vector4 rot_vec( cos(0.0f), 0.0f, sin(0.0f) );
  static const gfl2::math::Vector4 rot_vec( 0.0f, 0.0f, 0.0f );
  SetTranslation( data );
  SetAngle( 0.0f );
  SetRotation( rot_vec );
  SetScale( scale );
}

void SkeltonFieldTransform::Update( void )
{
  gfl2::math::Vector4 pos = m_trans + m_offset;
  gfl2::math::Matrix mtxTrans = gfl2::math::Matrix::GetTranslation( pos );
  gfl2::math::Matrix mtxScale = gfl2::math::Matrix::GetScaling( m_scale );
  gfl2::math::Matrix mtxRotate = gfl2::math::Matrix::GetRotationY( m_rot.y );
  gfl2::math::Matrix mtxWorld =  mtxTrans * mtxRotate * mtxScale;
  m_worldMtx = mtxWorld;
}

void SkeltonFieldTransform::SetTranslation( const gfl2::math::Vector4 & vec)
{
  m_trans = vec;
}

void SkeltonFieldTransform::SetScale( const gfl2::math::Vector4 & vec )
{
  m_scale = vec;
}

void SkeltonFieldTransform::SetRotation( const gfl2::math::Vector4 & vec )
{
  m_rot = vec;
}

void SkeltonFieldTransform::SetAngle( float angle )
{
  m_rot.y = angle;
}
gfl2::math::Vector4 SkeltonFieldTransform::GetTranslation( void ) const
{
  return m_trans;
}
gfl2::math::Vector4 SkeltonFieldTransform::GetScale( void ) const
{
  return m_scale;
}
gfl2::math::Vector4 SkeltonFieldTransform::GetRotation( void ) const
{
  return m_rot;
}
float SkeltonFieldTransform::GetAngle( void ) const
{
  return m_rot.y;
}

void SkeltonFieldTransform::SetOffset( const gfl2::math::Vector4 & vec )
{
  m_offset = vec;
}
gfl2::math::Vector4 SkeltonFieldTransform::GetOffset( void ) const
{
  return m_offset;
}

gfl2::math::SRT SkeltonFieldTransform::GetSRT() const
{
  gfl2::math::SRT srt;
  srt.scale.Set(m_scale.x, m_scale.y, m_scale.z);
  srt.rotation.RadianYToQuaternion(m_rot.y);
  gfl2::math::Vector4 tmp = m_trans + m_offset;
  srt.translate.Set(tmp.x, tmp.y, tmp.z); 

  return srt;
}

GFL_NAMESPACE_END(Skelton)
  GFL_NAMESPACE_END(Test)
#endif // GF_PLATFORM_WIN32