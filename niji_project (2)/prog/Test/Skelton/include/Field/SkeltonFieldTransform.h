#if defined(GF_PLATFORM_WIN32)
#pragma once
//=================================================================================================
/**
@file field_transform.h
@brief
@date 2014.03.07
@author tamada@gamefreak.co.jp

*/
//=================================================================================================

#include <math/include/gfl2_Vector4.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_SRT.h>
#include <macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN(Test)
  GFL_NAMESPACE_BEGIN(Skelton)

  //=================================================================================================
  //=================================================================================================
class SkeltonFieldTransform
{
public:
  SkeltonFieldTransform();
  ~SkeltonFieldTransform();

  void Clear( void );

  void SetWorldMatrix(const gfl2::math::Matrix & mtx);
  void SetTranslation( const gfl2::math::Vector4 & vec );
  void SetScale( const gfl2::math::Vector4 & vec );
  void SetRotation( const gfl2::math::Vector4 & vec );
  void SetAngle( float angle );

  gfl2::math::Matrix GetWorldMatrix( void ) const;
  gfl2::math::Vector4 GetTranslation( void ) const;
  gfl2::math::Vector4 GetScale( void ) const;
  gfl2::math::Vector4 GetRotation( void ) const;
  float GetAngle( void ) const;

  void SetOffset( const gfl2::math::Vector4 & vec );
  gfl2::math::Vector4 GetOffset( void ) const;

  void Update( void );

  gfl2::math::SRT GetSRT() const;

private:
  gfl2::math::Vector4 m_trans;
  gfl2::math::Vector4 m_scale;
  gfl2::math::Vector4 m_rot;
  gfl2::math::Vector4 m_offset;

  gfl2::math::Matrix m_worldMtx; 

};

//=================================================================================================
//=================================================================================================
class SkeltonFieldRange
{
public:
  SkeltonFieldRange() :
    m_pos(gfl2::math::Vector4()),
    m_radius(0.0f)
  {}

  ~SkeltonFieldRange(){}

  void SetCircleRange( const::gfl2::math::Vector4 center, f32 radius )
  {
    m_pos = center;
    m_radius = radius;
  }

  bool IsIn2D( const gfl2::math::Vector4 &pos ) const
  {
    if ( pow(pos.x - m_pos.x, 2) + pow(pos.z - m_pos.z, 2) <= pow(m_radius, 2) )
    {
      return true;
    }
    return false;
  }

private:
  gfl2::math::Vector4 m_pos;
  f32 m_radius;
};

GFL_NAMESPACE_END(Skelton)
  GFL_NAMESPACE_END(Test)
#endif // GF_PLATFORM_WIN32