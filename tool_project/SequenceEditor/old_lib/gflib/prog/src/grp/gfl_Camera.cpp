//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_Camera.cpp
 *	@brief  �V���v���ȃJ�����N���X
 *	@author	tomoya takahashi
 *	@date		2010.10.26
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <debug/gfl_Assert.h>
#include <debug/gfl_DebugPrint.h>

#include <grp/gfl_Camera.h>

namespace gfl {
namespace grp {




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

//----------------------------------------------------------------------------
/**
 *	@brief  �R���X�g���N�^
 */
//-----------------------------------------------------------------------------
Camera::Camera(void)
{
  m_viewParam.pos       = gfl::math::VEC3( 0,0,1 );
  m_viewParam.up        = gfl::math::VEC3( 0,1,0 );
  m_viewParam.target    = gfl::math::VEC3( 0,0,0 );

  m_projParam.proj_type         = PROJECTION_PERSPECTIVE;
  m_projParam.perspective.fovy  = CAMERA_DEFAULT_FOVY;
  m_projParam.perspective.aspect= CAMERA_DEFAULT_ASPECT;
  m_projParam.near              = CAMERA_DEFAULT_NEAR;
  m_projParam.far               = CAMERA_DEFAULT_FAR;
  m_projParam.pivot             = CAMERA_DEFAULT_PIVOT; 


  m_const_proj = false;
  m_const_view = false;

  UpdateLookAt();
  UpdateProjection();
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�X�g���N�^
 */
//-----------------------------------------------------------------------------
Camera::~Camera()
{
}


//----------------------------------------------------------------------------
/**
 *	@brief  �ˉe�s��̏������@Frustum����
 *
 *	@param	l         // �j�A�N���b�s���O�ʂ̎����䍶�ӂ�X���W
 *	@param  r         // �j�A�N���b�s���O�ʂ̎�����E�ӂ�X���W
 *	@param  b         // �j�A�N���b�s���O�ʂ̎����䉺�ӂ�Y���W
 *	@param  t         // �j�A�N���b�s���O�ʂ̎������ӂ�Y���W
 *	@param  near      // �j�A�ʂ̋���
 *	@param  far       // �t�@�[�ʂ̋���
 *	@param  pivot     // ��]�����ݒ�
 */
//-----------------------------------------------------------------------------
void Camera::SetUpFrustum( f32 l, f32 r, f32 b, f32 t, f32 near, f32 far, gfl::math::PivotDirection pivot )
{
  m_projParam.proj_type = PROJECTION_FRUSTUM;
  m_projParam.frustum.l = l;
  m_projParam.frustum.r = r;
  m_projParam.frustum.b = b;
  m_projParam.frustum.t = t;
  m_projParam.near      = near;
  m_projParam.far       = far;
  m_projParam.pivot     = pivot;

  m_const_proj = false;

  UpdateProjection();
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ˉe�s��̏������@Perspective����
 *
 *	@param	fovy      // �c�����̎���p(Radian)
 *	@param  aspect    // ����̃A�X�y�N�g��i��/�����j
 *	@param  near      // �j�A�ʂ̋���
 *	@param  far       // �t�@�[�ʂ̋���
 *	@param  pivot     // ��]�����ݒ�
 */
//-----------------------------------------------------------------------------
void Camera::SetUpPerspective( f32 fovy, f32 aspect, f32 near, f32 far, gfl::math::PivotDirection pivot )
{
  m_projParam.proj_type = PROJECTION_PERSPECTIVE;
  m_projParam.perspective.fovy    = fovy;
  m_projParam.perspective.aspect  = aspect;
  m_projParam.near      = near;
  m_projParam.far       = far;
  m_projParam.pivot     = pivot;

  m_const_proj = false;

  UpdateProjection();
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ˉe�s��̏������@Ortho����
 *
 *	@param	l         // �j�A�N���b�s���O�ʂ̎����䍶�ӂ�X���W
 *	@param  r         // �j�A�N���b�s���O�ʂ̎�����E�ӂ�X���W
 *	@param  b         // �j�A�N���b�s���O�ʂ̎����䉺�ӂ�Y���W
 *	@param  t         // �j�A�N���b�s���O�ʂ̎������ӂ�Y���W
 *	@param  near      // �j�A�ʂ̋���
 *	@param  far       // �t�@�[�ʂ̋���
 *	@param  pivot     // ��]�����ݒ�
 */
//-----------------------------------------------------------------------------
void Camera::SetUpOrtho( f32 l, f32 r, f32 b, f32 t, f32 near, f32 far, gfl::math::PivotDirection pivot )
{
  m_projParam.proj_type = PROJECTION_FRUSTUM;
  m_projParam.ortho.l   = l;
  m_projParam.ortho.r   = r;
  m_projParam.ortho.b   = b;
  m_projParam.ortho.t   = t;
  m_projParam.near      = near;
  m_projParam.far       = far;
  m_projParam.pivot     = pivot;

  m_const_proj = false;

  UpdateProjection();
}

//----------------------------------------------------------------------------
/**
 *	@brief  �s�񒼐ړn���ɂ��@�ˉe�s��̏�����
 *
 *	@param	projection  �ˉe�s��
 *	�s�񒼐ړn���̏ꍇ�Aconst_data���[�h�ƂȂ�A�l�̕ύX�����f����܂���B
 */
//-----------------------------------------------------------------------------
void Camera::SetUpProjection( const gfl::math::MTX44& projection )
{
  m_proj = projection;
  m_const_proj = true;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �J�����s��̏������@
 *
 *	@param	pos         // �J�������W
 *	@param	up          // �J���������
 *	@param	target      // �^�[�Q�b�g���W
 */
//-----------------------------------------------------------------------------
void Camera::SetUpLookAt( const gfl::math::VEC3& pos, const gfl::math::VEC3& up, const gfl::math::VEC3& target )
{
  m_viewParam.pos     = pos;
  m_viewParam.up      = up;
  m_viewParam.target  = target;

  m_const_view = false;

  UpdateLookAt();
}

//----------------------------------------------------------------------------
/**
 *	@brief  �s�񒼐ړn���ɂ��@�J�����s��̏�����
 *
 *	@param	view  �r���[�s��
 *	�s�񒼐ړn���̏ꍇ�Aconst_data���[�h�ƂȂ�A�l�̕ύX�����f����܂���B
 */
//-----------------------------------------------------------------------------
void Camera::SetUpLookAt( const gfl::math::MTX34& view )
{
  m_view = view;
  m_const_view = true;
}

//----------------------------------------------------------------------------
/**
 *	@brief  Projection�p�����[�^���g�p����Projection�s�񏉊���
 *
 *	@param	param Projection�p�����[�^
 */
//-----------------------------------------------------------------------------
void Camera::SetUpProjectionParam( const ProjectionParam& param )
{
  m_projParam = param;
  m_const_proj = false;
  UpdateProjection();
}

//----------------------------------------------------------------------------
/**
 *	@brief  View�p�����[�^���g�p����View�s�񏉊���
 *
 *	@param	param View�p�����[�^
 */
//-----------------------------------------------------------------------------
void Camera::SetUpViewParam( const ViewParam& param )
{
  m_viewParam = param;
  m_const_view = false;
  UpdateLookAt();
}


//----------------------------------------------------------------------------
/**
 *	@brief  ViewParam���擾
 *
 *	@return ViewParam
 */
//-----------------------------------------------------------------------------
const ViewParam& Camera::GetViewParam( void ) const
{
  return m_viewParam;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ProjectionParam���擾
 *
 *	@return ProjectionParam
 */
//-----------------------------------------------------------------------------
const ProjectionParam& Camera::GetProjectionParam( void ) const
{
  return m_projParam;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ViewMtx���擾
 *
 *	@return ViewMtx
 */
//-----------------------------------------------------------------------------
const gfl::math::MTX34& Camera::GetViewMtx( void ) const
{
  return m_view;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ProjectionMtx���擾
 *
 *	@return ProjectionMtx
 */
//-----------------------------------------------------------------------------
const gfl::math::MTX44& Camera::GetProjectionMtx( void ) const
{
  return m_proj;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �p�����[�^�̕ύX��View�s��ɔ��f����B
 */
//-----------------------------------------------------------------------------
void Camera::UpdateLookAt(void)
{
  //const�f�[�^���[�h�̏ꍇ�͍Čv�Z���s��Ȃ�
  if( m_const_view ){
    return ;
  }
  gfl::math::MTX34LookAt( &m_view, 
      &m_viewParam.pos, &m_viewParam.up, &m_viewParam.target );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �p�����[�^�̕ύX��Projection�s��ɔ��f����B
 */
//-----------------------------------------------------------------------------
void Camera::UpdateProjection(void)
{
  //const�f�[�^���[�h�̏ꍇ�͍Čv�Z���s��Ȃ�
  if( m_const_proj ){
    return ;
  }

  if( m_projParam.proj_type == PROJECTION_FRUSTUM ){
    gfl::math::MTX44FrustumPivot( &m_proj, 
        m_projParam.frustum.l,
        m_projParam.frustum.r,
        m_projParam.frustum.b,
        m_projParam.frustum.t,
        m_projParam.near,
        m_projParam.far,
        m_projParam.pivot );
  }
  else if( m_projParam.proj_type == PROJECTION_PERSPECTIVE ){
    gfl::math::MTX44PerspectivePivotRad( &m_proj, 
        m_projParam.perspective.fovy,
        m_projParam.perspective.aspect,
        m_projParam.near,
        m_projParam.far,
        m_projParam.pivot );
  }
  else{
    gfl::math::MTX44OrthoPivot( &m_proj, 
        m_projParam.ortho.l,
        m_projParam.ortho.r,
        m_projParam.ortho.b,
        m_projParam.ortho.t,
        m_projParam.near,
        m_projParam.far,
        m_projParam.pivot );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t�@�[�N���b�v�ʂ̋�����ݒ�
 *
 *	@param	far
 */
//-----------------------------------------------------------------------------
void Camera::SetFar( f32 far )
{
  m_projParam.far = far;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t�@�[�N���b�v�ʂ̋������擾
 *
 *	@return�@�t�@�[�N���b�v�ʂ̋���
 */
//-----------------------------------------------------------------------------
f32 Camera::GetFar( void ) const
{
  return m_projParam.far;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �j�A�N���b�v�ʂ̋�����ݒ�
 *
 *	@param	near
 */
//-----------------------------------------------------------------------------
void Camera::SetNear( f32 near )
{
  m_projParam.near = near;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �j�A�N���b�v�ʂ̋������擾
 *
 *	@return�@�j�A�N���b�v�ʂ̋���
 */
//-----------------------------------------------------------------------------
f32 Camera::GetNear( void ) const
{
  return m_projParam.near;
}

//----------------------------------------------------------------------------
/**
 *	@brief  Frustum�`���̏���ݒ�
 *
 *	@param	l         // �j�A�N���b�s���O�ʂ̎����䍶�ӂ�X���W
 *	@param  r         // �j�A�N���b�s���O�ʂ̎�����E�ӂ�X���W
 *	@param  b         // �j�A�N���b�s���O�ʂ̎����䉺�ӂ�Y���W
 *	@param  t         // �j�A�N���b�s���O�ʂ̎������ӂ�Y���W
 */
//-----------------------------------------------------------------------------
void Camera::SetFrustum( f32 l, f32 r, f32 b, f32 t )
{
  m_projParam.proj_type = PROJECTION_FRUSTUM;
  m_projParam.frustum.l = l;
  m_projParam.frustum.r = r;
  m_projParam.frustum.b = b;
  m_projParam.frustum.t = t;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ˉe�s��̏������@Perspective����
 *
 *	@param	fovy      // �c�����̎���p(Radian)
 *	@param  aspect    // ����̃A�X�y�N�g��i��/�����j
 */
//-----------------------------------------------------------------------------
void Camera::SetPerspective( f32 fovy, f32 aspect )
{
  m_projParam.proj_type         = PROJECTION_PERSPECTIVE;
  m_projParam.perspective.fovy  = fovy;
  m_projParam.perspective.aspect= aspect;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ˉe�s��̏������@Ortho����
 *
 *	@param	l         // �j�A�N���b�s���O�ʂ̎����䍶�ӂ�X���W
 *	@param  r         // �j�A�N���b�s���O�ʂ̎�����E�ӂ�X���W
 *	@param  b         // �j�A�N���b�s���O�ʂ̎����䉺�ӂ�Y���W
 *	@param  t         // �j�A�N���b�s���O�ʂ̎������ӂ�Y���W
 */
//-----------------------------------------------------------------------------
void Camera::SetOrtho( f32 l, f32 r, f32 b, f32 t )
{
  m_projParam.proj_type = PROJECTION_ORTHO;
  m_projParam.ortho.l = l;
  m_projParam.ortho.r = r;
  m_projParam.ortho.b = b;
  m_projParam.ortho.t = t;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �J�������W��ݒ�
 *
 *	@param	pos   �J�������W
 */
//-----------------------------------------------------------------------------
void Camera::SetCameraPos( const gfl::math::VEC3& pos )
{
  m_viewParam.pos = pos;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�������W���擾
 *
 *	@retval	�J�������W
 */
//-----------------------------------------------------------------------------
const gfl::math::VEC3& Camera::GetCameraPos( void ) const
{
  return m_viewParam.pos;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�����������ݒ�
 *
 *  @param  up  �J���������
 */
//-----------------------------------------------------------------------------
void Camera::SetCameraUp( const gfl::math::VEC3& up )
{
  m_viewParam.up = up;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J������������擾
 *
 *  @retval  �J���������
 */
//-----------------------------------------------------------------------------
const gfl::math::VEC3& Camera::GetCameraUp( void ) const
{
  return m_viewParam.up;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�����^�[�Q�b�g��ݒ�
 *
 *  @param  target  �J�����^�[�Q�b�g
 */
//-----------------------------------------------------------------------------
void Camera::SetCameraTarget( const gfl::math::VEC3& target )
{
  m_viewParam.target = target;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�����^�[�Q�b�g���擾
 *
 *  @retval  �J�����^�[�Q�b�g
 */
//-----------------------------------------------------------------------------
const gfl::math::VEC3& Camera::GetCameraTarget( void ) const
{
  return m_viewParam.target;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�o�b�O�p�p�����[�^�\��
 *
 *  @retval  �J�����^�[�Q�b�g
 */
//-----------------------------------------------------------------------------
void Camera::Dump( void ) const
{
  GFL_PRINT("Camera::Dump\n");
  GFL_PRINT("ProjType[%d]\n",m_projParam.proj_type);
  GFL_PRINT("param[%.2f][%.2f][%.2f][%.2f]\n",m_projParam.frustum.l,m_projParam.frustum.r,m_projParam.frustum.b,m_projParam.frustum.t);
  GFL_PRINT("near[%.2f] far[%.2f]\n",m_projParam.near,m_projParam.far);
  GFL_PRINT("-------------------\n");
  GFL_PRINT("pos[%.2f][%.2f][%.2f]\n",m_viewParam.pos.x,m_viewParam.pos.y,m_viewParam.pos.z);
  GFL_PRINT("up [%.2f][%.2f][%.2f]\n",m_viewParam.up.x,m_viewParam.up.y,m_viewParam.up.z);
  GFL_PRINT("trg[%.2f][%.2f][%.2f]\n",m_viewParam.target.x,m_viewParam.target.y,m_viewParam.target.z);
  GFL_PRINT("-------------------\n");
  GFL_PRINT("m_view\n");
  m_view.Report();
  GFL_PRINT("-------------------\n");
  GFL_PRINT("m_proj\n");
  m_proj.Report();
}




} /* namespace end grp */
} /* namespace end gfl */
