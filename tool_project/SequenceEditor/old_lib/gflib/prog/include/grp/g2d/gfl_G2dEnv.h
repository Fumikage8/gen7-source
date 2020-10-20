#ifndef __GFL_G2DENV_H__
#define __GFL_G2DENV_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G2dEnv.h
 *	@brief  2D�����_�����O�����
 *	@author	tomoya takahashi
 *	@date		2010.10.06
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include <nn.h>
#include <nw.h>
#include <nn/gx.h>
#include <nn/ulcd.h>
#include <nw/gfx.h>
#include <GLES2/gl2.h>

#include <grp/gfl_Camera.h>
#include <grp/gfl_FragmentOperation.h>

#include <gfl_Macros.h>



namespace gfl {
namespace grp {
namespace g2d {

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
  
//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 *    ViewPort�ݒ�
 */
//-----------------------------------------------------------------------------
typedef struct{
  u16 x;
  u16 y;
  u16 width;
  u16 height;
} EnvViewPort;

struct Parallax{
  f32 parallax_half;        ///< �����l�̔����̒l
  f32 dLevel;               ///< ��ʂ܂ł̋���
  f32 addDist;              ///< �J�����Ƃ̋����ɒǉ�����l 
} ;
  
//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	2D�`����Ǘ��N���X
//=====================================
class Env
{
  // �R�s�[�R���X�g���N�^�{������Z�q�֎~
  GFL_FORBID_COPY_AND_ASSIGN(Env);

public:
  enum CameraType
  {
    CAMERA_NORMAL,    // �ʏ�or���ڗp
    CAMERA_RIGHT,     // �E�ڗp

    CAMERA_NUM,
    ENUM_FORCE_DWORD(CameraType)
  };

  // 2D���f�t�H���g�@�p�����[�^
  static const ProjectionParam  DefaultProjection[DISPLAY_NORMAL_MAX];
  static const ViewParam        DefaultView[DISPLAY_NORMAL_MAX];
  
public:
  //-----------------------------------------------------------------------------
  /*
   * �R���X�g���N�^�E�f�X�g���N�^
   *
   * ���f�t�H���g�̐ݒ������ōs���܂��B
   *
   */
  //-----------------------------------------------------------------------------
  Env(void);
  ~Env();

  //----------------------------------------------------------------------------
  /**
   *	@brief  �J�����p�����[�^��ݒ�
   *
   *  @param  type    �J�����^�C�v
   *	@param	proj    �ˉe�ݒ�
   *	@param	view    View�ݒ�
   */
  //-----------------------------------------------------------------------------
  void SetCameraParam( Env::CameraType type, const ProjectionParam& proj, const ViewParam& view );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �t���O�����g�I�y���[�V�����p�����[�^��ݒ�
   *
   *	@param	param   �p�����[�^
   */
  //-----------------------------------------------------------------------------
  void SetFragmentOperation( const FragmentOperationParam& param );

  //----------------------------------------------------------------------------
  /**
   *	@brief  ViewPort�̐ݒ�
   *
   *  @param  use       true�@�r���[�|�[�g�ݒ���g�p����B false  �t���[���o�b�t�@�̃T�C�Y��ViewPort�ƈ����B
   *	@param	ViewPort  �ݒ���
   */
  //-----------------------------------------------------------------------------
  void SetViewPort( bool use, const EnvViewPort* viewport = NULL );


  //----------------------------------------------------------------------------
  /**
   *	@brief  ���ˉe�����̐ݒ�
   *
   *	@param  parallax    �����̒����̒l�iStereoCamera�Ɠ����l�j
   *	@param  dLevel      ��ʂւ̋����iStereoCamera�Ɠ����l�j
   *	@param  addDist     �J�����Ƃ̋����ɒǉ�����l 
   *
   *	@note ���ˉe�̂܂܁A�������������ꍇ�ɂ́A��������g�p���Ă��������B
   *	�@    
   */
  //-----------------------------------------------------------------------------
  void SetParallax( f32 parallax, f32 dLevel, f32 addDist );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �������̎擾
   */
  //-----------------------------------------------------------------------------
  const Parallax& GetParallax( void ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  ViewPort�̎擾
   *
   *	@retval	ViewPort���
   */
  //-----------------------------------------------------------------------------
  const EnvViewPort& GetViewPort( void ) const;


  //----------------------------------------------------------------------------
  /**
   *	@brief  3dCommand���J�����g�̃R�}���h���X�g�ɐݒ�
   *
   *	@param  �����_�[�^�[�Q�b�g
   */
  //-----------------------------------------------------------------------------
  void Add3DCommand( const RenderTarget& renderTarget ) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief  �J���������擾
   */
  //-----------------------------------------------------------------------------
  const gfl::grp::Camera& GetCamera( Env::CameraType type ) const ;
  gfl::grp::Camera* GetCameraPtr( Env::CameraType type );

  //-----------------------------------------------------------------------------
  /**
   *  @brief  �t���O�����g�I�y���[�V���������擾
   */
  //-----------------------------------------------------------------------------
  const gfl::grp::FragmentOperation& GetFragmentOperation( void ) const ;
  gfl::grp::FragmentOperation* GetFragmentOperationPtr( void );
  
  
private:
  gfl::grp::Camera                  m_camera[CAMERA_NUM];
  gfl::grp::FragmentOperation       m_fragmentOperation;
  EnvViewPort                       m_viewPort;
  Parallax                          m_parallax;
  bool                              m_viewPortUse;

  static bool                        m_sStereoCameraReady;
  static nn::ulcd::CTR::StereoCamera m_sStereoCamera;

};

  
} // g2d
} // grp
} // gfl
#endif // __GFL_G2DENV_H__
