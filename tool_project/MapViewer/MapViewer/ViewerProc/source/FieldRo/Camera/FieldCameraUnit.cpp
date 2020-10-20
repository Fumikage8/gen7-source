/**
* @brief  �t�B�[���h�J�����A1����P�ʃN���X
* @file   FieldCameraUnit.cpp
* @author ikuta_junya
* @data   2015.05.21
*/


// gfl2
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>

// field
#include "ViewerProc/include/FieldRo/Camera/FieldCameraUnit.h"


// �O���錾
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );

// �v���O�����ɂ�鏉���l�A���t���͂��̌ナ�\�[�X�f�[�^�ɂ���ĕύX�����\��������
static const f32 DEFAULT_PARAM_NEAR( 30.0f );                     // �j�A(��)
static const f32 DEFAULT_PARAM_FAR( 30000.0f );                   // �t�@�[(��)
static const f32 DEFAULT_PARAM_ASPECT_RATIO( 400.0f / 240.0f );   // �A�X�y�N�g��
static const f32 DEFAULT_PARAM_FOVY_RAD( gfl2::math::ConvDegToRad( 35.0f ) ); // ��p

/**
* @brief �R���X�g���N�^
*
* @return ����
*/ 
CameraUnit::CameraUnit( void ) :

  m_IsUpdate( false )

  // �����I�u�W�F�N�g
  ,m_pBaseCamera( NULL )
  ,m_pController( NULL )
  ,m_pDrawEnvNode( NULL )
{
}

/**
* @brief �f�X�g���N�^
*
* @return ����
*/
CameraUnit::~CameraUnit( void )
{
  Terminate();
}

/**
* @brief ����������
*
* @param  rInitDesc   �������ݒ�
*
* @return ����
*/
void CameraUnit::Initialize( const InitializeDescription& rInitDesc )
{
  // �J�����m�[�h�쐬 @todo ���͕K����邪�A�A�j���[�V�������g�p���Ȃ��ꍇ�͕s�v�ɂȂ�
  {
    m_pDrawEnvNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode();
    gfl2::renderingengine::scenegraph::instance::CameraNode* pCameraNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateCameraNode();

    pCameraNode->SetFar( DEFAULT_PARAM_FAR );
    pCameraNode->SetNear( DEFAULT_PARAM_NEAR );
    pCameraNode->SetAspectRatio( DEFAULT_PARAM_ASPECT_RATIO );
    pCameraNode->SetFovy( DEFAULT_PARAM_FOVY_RAD );
    
    m_pDrawEnvNode->AddCamera( pCameraNode );
  }

  // �J�������̐���
  {
    m_pBaseCamera = GFL_NEW( rInitDesc.m_pSystemHeap ) poke_3d::model::BaseCamera;
    m_pBaseCamera->Create(
      rInitDesc.m_pDeviceAllocator,
      rInitDesc.m_pDeviceHeap,
      m_pDrawEnvNode,
      0
      );
  }

  // �ˉe�ݒ� @todo ��
  {
    // EnvNode��n���Ă���ꍇ�ACameraNode�̒l��Create���ɐݒ肳��Ă���
#if 0
    m_pBaseCamera->SetNear( 60000.0f );
    m_pBaseCamera->SetFar( 60000.0f );
    m_pBaseCamera->SetAspectRatio( 
      gfl2::gfx::GFGLUtil::GetDisplayWidthByDisplayMode( gfl2::gfx::DisplayMode::HD720P ) 
      / (f32)gfl2::gfx::GFGLUtil::GetDisplayHeightByDisplayMode( gfl2::gfx::DisplayMode::HD720P )
      );

    m_pBaseCamera->SetFovy( gfl2::math::ConvDegToRad( 35.0f ) );
#endif
  }

  // ���̑�
  {
    m_IsUpdate = true;
  }
}

/**
* @brief �I������
*
* @return ����
*/
void CameraUnit::Terminate( void )
{
  GFL_SAFE_DELETE( m_pController );
  GFL_SAFE_DELETE( m_pBaseCamera );
  GFL_SAFE_DELETE( m_pDrawEnvNode );
  m_IsUpdate = false;
}

/**
* @brief �X�V����
*
* @return ����
*/
void CameraUnit::Update( void )
{
  if( m_pController && m_IsUpdate )
  {
    m_pController->Update( m_pBaseCamera );
  }
}

/**
* @brief ����@�\�ύX
*
* @param  pController �ύX��̋@�\  (�������Ǘ�����)
*
* @return ����
*/
void CameraUnit::ChangeController( Controller::ControllerBase* pController )
{
  // @todo�@�����ŕ�Ԃ��s��


  GFL_SAFE_DELETE( m_pController );
  m_pController = pController;
  if( m_pController )
  {
    m_pController->SetUp( m_pBaseCamera );
  }
}

/**
* @brief �J������Y����]�ʂ��擾 @todo ���̊֐��͏�������
*
* @return Y����]��( ���W�A���AZ��������==0.0f )
*/
f32 CameraUnit::GetRotationY( void ) const
{
  gfl2::math::Matrix34 rot;
  gfl2::math::Vector3 vec( 0.0f, 0.0f, 1.0f );

  m_pBaseCamera->GetRotationQuat().QuaternionToMatrix( rot );
  vec = rot.TransformCoord( vec );
  ///return atan2( vec.z, vec.x ) * -1.0f - PI * 0.5f;
	return atan2( vec.z, vec.x ) * -1.0f + PI * 0.5f;

}

/**
* @brief �X�V�t���O��ݒ�
*
* @param  updateFlag  �X�V�t���O
*
* @return ����
*/
void CameraUnit::SetUpdateFlag( b32 updateFlag )
{
  m_IsUpdate = updateFlag;
}

/**
* @brief �J�����̊�{�p�����[�^���R�s�[����
* @note  View�s��A�ˉe�s��Ɋւ��p�����[�^�݂̂��R�s�[����B�A�j���[�V�����֘A�A�R���g���[���[�֘A�̒l�̓R�s�[����܂���
*
* @param  rSrc �R�s�[��
*
* @return ����
*/
void CameraUnit::CopyBaseParam( const CameraUnit& rSrc )
{
  // �ˉe
  m_pBaseCamera->SetFar( rSrc.m_pBaseCamera->GetFar() );
  m_pBaseCamera->SetNear( rSrc.m_pBaseCamera->GetNear() );
  m_pBaseCamera->SetFovy( rSrc.m_pBaseCamera->GetFovy() );
  m_pBaseCamera->SetAspectRatio( rSrc.m_pBaseCamera->GetAspectRatio() );
  
  // view
  m_pBaseCamera->SetPosition( rSrc.m_pBaseCamera->GetPosition() );
  m_pBaseCamera->SetTargetPosition( rSrc.m_pBaseCamera->GetTargetPosition() );
  m_pBaseCamera->SetRotationQuat( rSrc.m_pBaseCamera->GetRotationQuat() );
}


/**
* @brief ���݂̐ݒ�l���琶�������v���W�F�N�V�����s����擾����
*
* @param  pOut �s��i�[��
*
* @return �����œn�����|�C���^
*/
gfl2::math::Matrix44* CameraUnit::GetProjectionMatrix( gfl2::math::Matrix44* pOut ) const
{
  if( m_pBaseCamera )
  {
    if( m_pController && m_pController->GetProjectionMatrixOverride( pOut ) )
    {
      // �R���g���[���[���ݒ肵���s���Ԃ�

    }
    else
    {
      *pOut = m_pBaseCamera->GetProjectionMatrix();
    }
  }
  else
  {
    *pOut = gfl2::math::Matrix44::GetIdentity();
  }

  return pOut;
}

/**
* @brief ���݂̐ݒ�l���琶�������r���[�s����擾����
*
* @param  pOut �s��i�[��
*
* @return �����œn�����|�C���^
*/
gfl2::math::Matrix34* CameraUnit::GetViewMatrix( gfl2::math::Matrix34* pOut ) const
{
  if( m_pBaseCamera )
  {
    if( m_pController && m_pController->GetViewMatrixOverride( pOut ) )
    {
      // �R���g���[���[���ݒ肵���s���Ԃ�

    }
    else
    {
      *pOut = m_pBaseCamera->GetViewMatrix();
    }
  }
  else
  {
    *pOut = gfl2::math::Matrix34::GetIdentity();
  }
  
  return pOut;
}


GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );
