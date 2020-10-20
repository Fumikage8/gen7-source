//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_Env.cpp
 *	@brief  2D�`������
 *	@author	tomoya takahashi
 *	@date		2010.10.06
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <grp/g2d/gfl_G2dEnv.h>
#include <debug/gfl_Assert.h>
#include <debug/gfl_DebugPrint.h>

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
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					����
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          ���J
 */
//-----------------------------------------------------------------------------
// 2D���f�t�H���g�@�p�����[�^
const ProjectionParam Env::DefaultProjection[DISPLAY_NORMAL_MAX] = {
  {
    PROJECTION_ORTHO,
    { -200, 200, -120, 120 },
    0.10f,
    800.0f,
    gfl::math::PIVOT_UPSIDE_TO_TOP
  },
  {
    PROJECTION_ORTHO,
    { -160, 160, -120, 120 },
    0.10f,
    800.0f,
    gfl::math::PIVOT_UPSIDE_TO_TOP
  },
};
const ViewParam Env::DefaultView[DISPLAY_NORMAL_MAX] = {
  {
    gfl::math::VEC3(0,0,1),
    gfl::math::VEC3(0,1,0),
    gfl::math::VEC3(0,0,0)
  },
  {
    gfl::math::VEC3(0,-290,1),
    gfl::math::VEC3(0,1,0),
    gfl::math::VEC3(0,-290,0)
  },
};

bool                        Env::m_sStereoCameraReady = false;
nn::ulcd::CTR::StereoCamera Env::m_sStereoCamera;


//----------------------------------------------------------------------------
/**
 *	@brief  �R���X�g���N�^    �f�t�H���g�ݒ�
 */
//-----------------------------------------------------------------------------
Env::Env(void)
{
  SetCameraParam( CAMERA_NORMAL, DefaultProjection[DISPLAY_UPPER], DefaultView[DISPLAY_UPPER] );
  SetCameraParam( CAMERA_RIGHT, DefaultProjection[DISPLAY_UPPER], DefaultView[DISPLAY_UPPER] );

  m_parallax.parallax_half = 0.0f;
  m_parallax.dLevel        = 0.0f;
  m_parallax.addDist       = 0.0f;

  // StereoCamera�̏���
  if( !m_sStereoCameraReady ){
    m_sStereoCamera.Initialize();
    m_sStereoCamera.SetBaseFrustum(
        -0.04f,
         0.04f,
        -0.04f * static_cast<f32>(DISPLAY_UPPER_HEIGHT) / static_cast<f32>(DISPLAY_UPPER_WIDTH),
         0.04f * static_cast<f32>(DISPLAY_UPPER_HEIGHT) / static_cast<f32>(DISPLAY_UPPER_WIDTH),
         0.2f,
         3000.0f);
    m_sStereoCameraReady = true;
  }

  // FragmentOperation�̓R���X�g���N�^���Őݒ肳���f�t�H���g�l���g�p
}



//----------------------------------------------------------------------------
/**
 *	@brief  �f�X�g���N�^
 */
//-----------------------------------------------------------------------------
Env::~Env()
{
}


//----------------------------------------------------------------------------
/**
 *	@brief  �J�����p�����[�^��ݒ�
 *
 *  @param  type    �J�����^�C�v
 *	@param	proj    �ˉe�ݒ�
 *	@param	view    View�ݒ�
 */
//-----------------------------------------------------------------------------
void Env::SetCameraParam( Env::CameraType type, const ProjectionParam& proj, const ViewParam& view )
{
  if( type < CAMERA_NUM )
  {
    m_camera[type].SetUpProjectionParam( proj );
    m_camera[type].SetUpViewParam( view );
  }
  else
  {
    GFL_ASSERT(0);
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t���O�����g�I�y���[�V�����p�����[�^��ݒ�
 *
 *	@param	param   �p�����[�^
 */
//-----------------------------------------------------------------------------
void Env::SetFragmentOperation( const FragmentOperationParam& param )
{
  m_fragmentOperation.SetParam( param );
}


//----------------------------------------------------------------------------
/**
 *	@brief  ViewPort�̐ݒ�
 *
 *  @param  use       true�@�r���[�|�[�g�ݒ���g�p����B false  �t���[���o�b�t�@�̃T�C�Y��ViewPort�ƈ����B
 *	@param	ViewPort  �ݒ���
 */
//-----------------------------------------------------------------------------
void Env::SetViewPort( bool use, const EnvViewPort* viewport )
{
  m_viewPortUse = use;
  if(viewport){
    m_viewPort    = *viewport;
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  �����̐ݒ�
 *
 *	@param  parallax    �����̒����̒l�iStereoCamera�Ɠ����l�j
 *	@param  dLevel      ��ʂւ̋����iStereoCamera�Ɠ����l�j
 *	@param  addDist     �J�����Ƃ̋����ɒǉ�����l 
 */
//-----------------------------------------------------------------------------
void Env::SetParallax( f32 parallax, f32 dLevel, f32 addDist )
{
  m_sStereoCamera.SetBaseCamera(&m_camera[CAMERA_NORMAL].GetViewMtx());

  f32 l_parallax = 0.0f;
  if( GraphicsSystem::IsGameStereoFlag() ){
    l_parallax = parallax;
  }



  nn::math::MTX44 projL, projR;
  nn::math::MTX34 viewL, viewR;
  m_sStereoCamera.CalculateMatrices(
      &projL,
      &viewL,
      &projR,
      &viewR,
      dLevel,
      l_parallax,
      nn::math::PIVOT_UPSIDE_TO_TOP);
  
  f32 wkParallax = m_sStereoCamera.GetCoefficientForParallax();

  m_parallax.parallax_half  = wkParallax * static_cast<f32>(DISPLAY_UPPER_WIDTH);
  m_parallax.dLevel         = dLevel;
  m_parallax.addDist        = addDist;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �������̎擾
 */
//-----------------------------------------------------------------------------
const Parallax& Env::GetParallax( void ) const
{
  return m_parallax;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ViewPort�̎擾
 *
 *	@retval	ViewPort���
 */
//-----------------------------------------------------------------------------
const EnvViewPort& Env::GetViewPort( void ) const
{
  return m_viewPort;
}


//----------------------------------------------------------------------------
/**
 *	@brief  3dCommand���J�����g�̃R�}���h���X�g�ɐݒ�
 *
 *	@param  �t���[���o�b�t�@
 */
//-----------------------------------------------------------------------------
void Env::Add3DCommand( const RenderTarget& renderTarget ) const
{
  if( m_viewPortUse ){
  
    m_fragmentOperation.Add3DCommand( m_viewPort.x, m_viewPort.y, 
        m_viewPort.width, m_viewPort.height, renderTarget.GetRenderDepthFormat() );
  }else{
    m_fragmentOperation.Add3DCommand( renderTarget );
  }
}


//-----------------------------------------------------------------------------
/**
 *  @brief  �J���������擾
 */
//-----------------------------------------------------------------------------
const gfl::grp::Camera& Env::GetCamera( Env::CameraType type ) const 
{
  if( type < CAMERA_NUM )
  {
    return m_camera[type];
  }
  GFL_ASSERT(0);
  return m_camera[0];
}
gfl::grp::Camera* Env::GetCameraPtr( Env::CameraType type )
{
  if( type < CAMERA_NUM )
  {
    return &m_camera[type];
  }
  GFL_ASSERT(0);
  return &m_camera[0];
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �t���O�����g�I�y���[�V���������擾
 */
//-----------------------------------------------------------------------------
const gfl::grp::FragmentOperation& Env::GetFragmentOperation( void ) const 
{
  return m_fragmentOperation;
}
gfl::grp::FragmentOperation* Env::GetFragmentOperationPtr( void )
{
  return &m_fragmentOperation;
}




} /* end of namespace g2d */
} /* end of namespace grp */
} /* end of namespace gfl */
