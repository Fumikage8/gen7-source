#pragma once
//=================================================================================================
/**
 *  @file   BaseShape.cpp
 *  @brief  コリジョン向け基本形状
 *  @author yy-ikeuchi@gamefreak.co.jp
 *  @date   2015.02.17
 */
//=================================================================================================
#include "../../include/Collision/BaseShape.h"

// RTTIデバッグシステム
#include "../../include/RttiDebug/RttiManager.h"

//----------------------------------------------------------------------------------------
/**
*  @brief コリジョン向け2D球形状
*/
//----------------------------------------------------------------------------------------
SphereShape2D::SphereShape2D()
: m_r(0.0f)
, m_pos(0.0f,0.0f,0.0f)
{
}

//-----------------------------------------------------------------------------
/**
*  @brief  RTTIマネージャーに自身のプロパティを登録
*/
//-----------------------------------------------------------------------------
void SphereShape2D::RegistRtti(void)
{
#if USE_RTTI_DEBUG

  static bool isInit = false;
  if( isInit )
  {
    return;
  }
  isInit = true;

  // この記述がなければ親のメンバ変数はエディタに登録されません
  RTTI_BASE_PARENT(SphereShape2D,BaseShape);

  RTTI_START_REGIST( 256 );
    RTTI_PARAM_REGIST( SphereShape2D , m_r     , L"スフィア2D:半径" );
    RTTI_PARAM_REGIST( SphereShape2D , m_pos.x , L"スフィア2D:座標X" );
    RTTI_PARAM_REGIST( SphereShape2D , m_pos.y , L"スフィア2D:座標Y" );
    RTTI_PARAM_REGIST( SphereShape2D , m_pos.z , L"スフィア2D:座標Z" );
  RTTI_END_REGIST( SphereShape2D );

#endif
}

//----------------------------------------------------------------------------------------
/**
*  @brief コリジョン向け3D球形状
*/
//----------------------------------------------------------------------------------------
SphereShape3D::SphereShape3D()
: m_r(0.0f)
, m_pos(0.0f,0.0f,0.0f)
{
}

//-----------------------------------------------------------------------------
/**
*  @brief  RTTIマネージャーに自身のプロパティを登録
*/
//-----------------------------------------------------------------------------
void SphereShape3D::RegistRtti(void)
{
#if USE_RTTI_DEBUG

  static bool isInit = false;
  if( isInit )
  {
    return;
  }
  isInit = true;

  // この記述がなければ親のメンバ変数はエディタに登録されません
  RTTI_BASE_PARENT(SphereShape3D,BaseShape);

  RTTI_START_REGIST( 256 );
    RTTI_PARAM_REGIST( SphereShape3D , m_r     , L"スフィア3D:半径"  );
    RTTI_PARAM_REGIST( SphereShape3D , m_pos.x , L"スフィア3D:座標X" );
    RTTI_PARAM_REGIST( SphereShape3D , m_pos.y , L"スフィア3D:座標Y" );
    RTTI_PARAM_REGIST( SphereShape3D , m_pos.z , L"スフィア3D:座標Z" );
  RTTI_END_REGIST( SphereShape3D );

#endif
}

//----------------------------------------------------------------------------------------
/**
/* @brief コリジョン向け2D箱形状
*/
//----------------------------------------------------------------------------------------
BoxShape2D::BoxShape2D()
: m_u0 (1.0f,0.0f,0.0f)
, m_u1 (0.0f,0.0f,1.0f)
, m_e0 (10.0f)
, m_e1 (10.0f)
, m_pos(0.0f,0.0f,0.0f)
{
}

//-----------------------------------------------------------------------------
/**
*  @brief  RTTIマネージャーに自身のプロパティを登録
*/
//-----------------------------------------------------------------------------
void BoxShape2D::RegistRtti(void)
{
#if USE_RTTI_DEBUG

  static bool isInit = false;
  if( isInit )
  {
    return;
  }
  isInit = true;

  // この記述がなければ親のメンバ変数はエディタに登録されません
  RTTI_BASE_PARENT(BoxShape2D,BaseShape);

  RTTI_START_REGIST( 256 );
    RTTI_PARAM_REGIST( BoxShape2D , m_e0    , L"ボックス2D:X軸の長さの半分" );
    RTTI_PARAM_REGIST( BoxShape2D , m_e1    , L"ボックス2D:Y軸の長さの半分" );
    RTTI_PARAM_REGIST( BoxShape2D , m_pos.x , L"ボックス2D:座標X" );
    RTTI_PARAM_REGIST( BoxShape2D , m_pos.y , L"ボックス2D:座標Y" );
    RTTI_PARAM_REGIST( BoxShape2D , m_pos.z , L"ボックス2D:座標Z" );
  RTTI_END_REGIST( BoxShape2D );

#endif
}

//----------------------------------------------------------------------------------------
/**
*  @brief コリジョン向けメッシュ形状
*/
//----------------------------------------------------------------------------------------
MeshShape::MeshShape()
: m_pCollisionModel(0)
, m_pos(0.0f,0.0f,0.0f)
{
}

//-----------------------------------------------------------------------------
/**
*  @brief  RTTIマネージャーに自身のプロパティを登録
*/
//-----------------------------------------------------------------------------
void MeshShape::RegistRtti(void)
{
#if USE_RTTI_DEBUG

  static bool isInit = false;
  if( isInit )
  {
    return;
  }
  isInit = true;

  // この記述がなければ親のメンバ変数はエディタに登録されません
  RTTI_BASE_PARENT(MeshShape,BaseShape);

  RTTI_START_REGIST( 256 );
    RTTI_PARAM_REGIST( MeshShape , m_pos.x , L"メッシュ:座標X" );
    RTTI_PARAM_REGIST( MeshShape , m_pos.y , L"メッシュ:座標Y" );
    RTTI_PARAM_REGIST( MeshShape , m_pos.z , L"メッシュ:座標Z" );
  RTTI_END_REGIST( MeshShape );

#endif
}