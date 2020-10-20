#pragma once
//=================================================================================================
/**
 *  @file   BaseShape.cpp
 *  @brief  �R���W����������{�`��
 *  @author yy-ikeuchi@gamefreak.co.jp
 *  @date   2015.02.17
 */
//=================================================================================================
#include "ViewerProc/include/FieldStatic/Collision/BaseShape.h"

// RTTI�f�o�b�O�V�X�e��
///#include "../../include/RttiDebug/RttiManager.h"

//----------------------------------------------------------------------------------------
/**
*  @brief �R���W���������V�����_�[�`��
*/
//----------------------------------------------------------------------------------------
CylinderShape::CylinderShape()
: m_r(0.0f)
, m_pos(0.0f,0.0f,0.0f)
{
}

//-----------------------------------------------------------------------------
/**
*  @brief  RTTI�}�l�[�W���[�Ɏ��g�̃v���p�e�B��o�^
*/
//-----------------------------------------------------------------------------
void CylinderShape::RegistRtti(void)
{
#if USE_RTTI_DEBUG

  // ���̋L�q���Ȃ���ΐe�̃����o�ϐ��̓G�f�B�^�ɓo�^����܂���
  RTTI_BASE_PARENT(CylinderShape,BaseShape);

  RTTI_START_REGIST( 256 );
    RTTI_PARAM_REGIST( CylinderShape , m_r     , L"�V�����_�[:���a" );
    RTTI_PARAM_REGIST( CylinderShape , m_pos.x , L"�V�����_�[:���WX" );
    RTTI_PARAM_REGIST( CylinderShape , m_pos.y , L"�V�����_�[:���WY" );
    RTTI_PARAM_REGIST( CylinderShape , m_pos.z , L"�V�����_�[:���WZ" );
  RTTI_END_REGIST( CylinderShape );

#endif
}

//----------------------------------------------------------------------------------------
/**
*  @brief �R���W�����������`��
*/
//----------------------------------------------------------------------------------------
SphereShape::SphereShape()
: m_r(0.0f)
, m_pos(0.0f,0.0f,0.0f)
{
}

//-----------------------------------------------------------------------------
/**
*  @brief  RTTI�}�l�[�W���[�Ɏ��g�̃v���p�e�B��o�^
*/
//-----------------------------------------------------------------------------
void SphereShape::RegistRtti(void)
{
#if USE_RTTI_DEBUG

  // ���̋L�q���Ȃ���ΐe�̃����o�ϐ��̓G�f�B�^�ɓo�^����܂���
  RTTI_BASE_PARENT(SphereShape,BaseShape);

  RTTI_START_REGIST( 256 );
    RTTI_PARAM_REGIST( SphereShape , m_r     , L"�X�t�B�A:���a"  );
    RTTI_PARAM_REGIST( SphereShape , m_pos.x , L"�X�t�B�A:���WX" );
    RTTI_PARAM_REGIST( SphereShape , m_pos.y , L"�X�t�B�A:���WY" );
    RTTI_PARAM_REGIST( SphereShape , m_pos.z , L"�X�t�B�A:���WZ" );
  RTTI_END_REGIST( SphereShape );

#endif
}

//----------------------------------------------------------------------------------------
/**
/* @brief �R���W�����������`��
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
*  @brief  RTTI�}�l�[�W���[�Ɏ��g�̃v���p�e�B��o�^
*/
//-----------------------------------------------------------------------------
void BoxShape::RegistRtti(void)
{
#if USE_RTTI_DEBUG

  // ���̋L�q���Ȃ���ΐe�̃����o�ϐ��̓G�f�B�^�ɓo�^����܂���
  RTTI_BASE_PARENT(BoxShape,BaseShape);

  RTTI_START_REGIST( 256 );
    RTTI_PARAM_REGIST( BoxShape , m_e0    , L"�{�b�N�X:X���̒����̔���" );
    RTTI_PARAM_REGIST( BoxShape , m_e1    , L"�{�b�N�X:Y���̒����̔���" );
    RTTI_PARAM_REGIST( BoxShape , m_pos.x , L"�{�b�N�X:���WX" );
    RTTI_PARAM_REGIST( BoxShape , m_pos.y , L"�{�b�N�X:���WY" );
    RTTI_PARAM_REGIST( BoxShape , m_pos.z , L"�{�b�N�X:���WZ" );
  RTTI_END_REGIST( BoxShape );

#endif
}

//----------------------------------------------------------------------------------------
/**
*  @brief �R���W�����������b�V���`��
*/
//----------------------------------------------------------------------------------------
MeshShape::MeshShape()
: m_pCollisionModel(0)
, m_pos(0.0f,0.0f,0.0f)
{
}

//-----------------------------------------------------------------------------
/**
*  @brief  RTTI�}�l�[�W���[�Ɏ��g�̃v���p�e�B��o�^
*/
//-----------------------------------------------------------------------------
void MeshShape::RegistRtti(void)
{
#if USE_RTTI_DEBUG

  // ���̋L�q���Ȃ���ΐe�̃����o�ϐ��̓G�f�B�^�ɓo�^����܂���
  RTTI_BASE_PARENT(MeshShape,BaseShape);

  RTTI_START_REGIST( 256 );
    RTTI_PARAM_REGIST( MeshShape , m_pos.x , L"���b�V��:���WX" );
    RTTI_PARAM_REGIST( MeshShape , m_pos.y , L"���b�V��:���WY" );
    RTTI_PARAM_REGIST( MeshShape , m_pos.z , L"���b�V��:���WZ" );
  RTTI_END_REGIST( MeshShape );

#endif
}

//----------------------------------------------------------------------------------------
/**
*  @brief �R���W�����������b�V���`��
*/
//----------------------------------------------------------------------------------------
LineShape::LineShape()
: m_pos0(0.0f,0.0f,0.0f)
, m_pos1(0.0f,0.0f,0.0f)
{
}

//-----------------------------------------------------------------------------
/**
*  @brief  RTTI�}�l�[�W���[�Ɏ��g�̃v���p�e�B��o�^
*/
//-----------------------------------------------------------------------------
void LineShape::RegistRtti(void)
{
#if USE_RTTI_DEBUG

  // ���̋L�q���Ȃ���ΐe�̃����o�ϐ��̓G�f�B�^�ɓo�^����܂���
  RTTI_BASE_PARENT(LineShape,BaseShape);

  RTTI_START_REGIST( 256 );
    RTTI_PARAM_REGIST( LineShape , m_pos0.x , L"���C��0:���WX" );
    RTTI_PARAM_REGIST( LineShape , m_pos0.y , L"���C��0:���WY" );
    RTTI_PARAM_REGIST( LineShape , m_pos0.z , L"���C��0:���WZ" );
    RTTI_PARAM_REGIST( LineShape , m_pos1.x , L"���C��1:���WX" );
    RTTI_PARAM_REGIST( LineShape , m_pos1.y , L"���C��1:���WY" );
    RTTI_PARAM_REGIST( LineShape , m_pos1.z , L"���C��1:���WZ" );
  RTTI_END_REGIST( LineShape );

#endif
}