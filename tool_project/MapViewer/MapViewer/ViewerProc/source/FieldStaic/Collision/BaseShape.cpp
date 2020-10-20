#pragma once
//=================================================================================================
/**
 *  @file   BaseShape.cpp
 *  @brief  コリジョン向け基本形状
 *  @author yy-ikeuchi@gamefreak.co.jp
 *  @date   2015.02.17
 */
//=================================================================================================
#include "ViewerProc/include/FieldStatic/Collision/BaseShape.h"

// RTTIデバッグシステム
///#include "../../include/RttiDebug/RttiManager.h"

//----------------------------------------------------------------------------------------
/**
*  @brief コリジョン向けシリンダー形状
*/
//----------------------------------------------------------------------------------------
CylinderShape::CylinderShape()
: m_r(0.0f)
, m_pos(0.0f,0.0f,0.0f)
{
}

//-----------------------------------------------------------------------------
/**
*  @brief  RTTIマネージャーに自身のプロパティを登録
*/
//-----------------------------------------------------------------------------
void CylinderShape::RegistRtti(void)
{
#if USE_RTTI_DEBUG

  // この記述がなければ親のメンバ変数はエディタに登録されません
  RTTI_BASE_PARENT(CylinderShape,BaseShape);

  RTTI_START_REGIST( 256 );
    RTTI_PARAM_REGIST( CylinderShape , m_r     , L"シリンダー:半径" );
    RTTI_PARAM_REGIST( CylinderShape , m_pos.x , L"シリンダー:座標X" );
    RTTI_PARAM_REGIST( CylinderShape , m_pos.y , L"シリンダー:座標Y" );
    RTTI_PARAM_REGIST( CylinderShape , m_pos.z , L"シリンダー:座標Z" );
  RTTI_END_REGIST( CylinderShape );

#endif
}

//----------------------------------------------------------------------------------------
/**
*  @brief コリジョン向け球形状
*/
//----------------------------------------------------------------------------------------
SphereShape::SphereShape()
: m_r(0.0f)
, m_pos(0.0f,0.0f,0.0f)
{
}

//-----------------------------------------------------------------------------
/**
*  @brief  RTTIマネージャーに自身のプロパティを登録
*/
//-----------------------------------------------------------------------------
void SphereShape::RegistRtti(void)
{
#if USE_RTTI_DEBUG

  // この記述がなければ親のメンバ変数はエディタに登録されません
  RTTI_BASE_PARENT(SphereShape,BaseShape);

  RTTI_START_REGIST( 256 );
    RTTI_PARAM_REGIST( SphereShape , m_r     , L"スフィア:半径"  );
    RTTI_PARAM_REGIST( SphereShape , m_pos.x , L"スフィア:座標X" );
    RTTI_PARAM_REGIST( SphereShape , m_pos.y , L"スフィア:座標Y" );
    RTTI_PARAM_REGIST( SphereShape , m_pos.z , L"スフィア:座標Z" );
  RTTI_END_REGIST( SphereShape );

#endif
}

//----------------------------------------------------------------------------------------
/**
/* @brief コリジョン向け箱形状
*/
//----------------------------------------------------------------------------------------
BoxShape::BoxShape()
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
void BoxShape::RegistRtti(void)
{
#if USE_RTTI_DEBUG

  // この記述がなければ親のメンバ変数はエディタに登録されません
  RTTI_BASE_PARENT(BoxShape,BaseShape);

  RTTI_START_REGIST( 256 );
    RTTI_PARAM_REGIST( BoxShape , m_e0    , L"ボックス:X軸の長さの半分" );
    RTTI_PARAM_REGIST( BoxShape , m_e1    , L"ボックス:Y軸の長さの半分" );
    RTTI_PARAM_REGIST( BoxShape , m_pos.x , L"ボックス:座標X" );
    RTTI_PARAM_REGIST( BoxShape , m_pos.y , L"ボックス:座標Y" );
    RTTI_PARAM_REGIST( BoxShape , m_pos.z , L"ボックス:座標Z" );
  RTTI_END_REGIST( BoxShape );

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

  // この記述がなければ親のメンバ変数はエディタに登録されません
  RTTI_BASE_PARENT(MeshShape,BaseShape);

  RTTI_START_REGIST( 256 );
    RTTI_PARAM_REGIST( MeshShape , m_pos.x , L"メッシュ:座標X" );
    RTTI_PARAM_REGIST( MeshShape , m_pos.y , L"メッシュ:座標Y" );
    RTTI_PARAM_REGIST( MeshShape , m_pos.z , L"メッシュ:座標Z" );
  RTTI_END_REGIST( MeshShape );

#endif
}

//----------------------------------------------------------------------------------------
/**
*  @brief コリジョン向けメッシュ形状
*/
//----------------------------------------------------------------------------------------
LineShape::LineShape()
: m_pos0(0.0f,0.0f,0.0f)
, m_pos1(0.0f,0.0f,0.0f)
{
}

//-----------------------------------------------------------------------------
/**
*  @brief  RTTIマネージャーに自身のプロパティを登録
*/
//-----------------------------------------------------------------------------
void LineShape::RegistRtti(void)
{
#if USE_RTTI_DEBUG

  // この記述がなければ親のメンバ変数はエディタに登録されません
  RTTI_BASE_PARENT(LineShape,BaseShape);

  RTTI_START_REGIST( 256 );
    RTTI_PARAM_REGIST( LineShape , m_pos0.x , L"ライン0:座標X" );
    RTTI_PARAM_REGIST( LineShape , m_pos0.y , L"ライン0:座標Y" );
    RTTI_PARAM_REGIST( LineShape , m_pos0.z , L"ライン0:座標Z" );
    RTTI_PARAM_REGIST( LineShape , m_pos1.x , L"ライン1:座標X" );
    RTTI_PARAM_REGIST( LineShape , m_pos1.y , L"ライン1:座標Y" );
    RTTI_PARAM_REGIST( LineShape , m_pos1.z , L"ライン1:座標Z" );
  RTTI_END_REGIST( LineShape );

#endif
}