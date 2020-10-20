/**
* @brief  �t�B�[���h�J�����̑���A�e�X�g�p
* @file   FieldCameraControllerTypeDebugOrthoView.cpp
* @author ikuta_junya
* @data   2015.05.21
*/

// niji
#include "GameSystem/include/GameManager.h"

// field
#include "ViewerProc/include/FieldRo/Camera/Controller/FieldCameraControllerTypeDebugOrthoView.h"

// �O���錾
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Controller );

const static f32 TEST_CAMERA_DISTANCE( 579.0f );
const static gfl2::math::Vector3 TEST_CAMERA_ROTATION( gfl2::math::ConvDegToRad( -34.637991f ), 0.0f, 0.0f );

static const f32 sc_OrthoHeightTable[] = 
{
  800.0f,
  1600.0f,
  3200.0f,
  6400.0f,
  12800.0f,
  25600.0f,
  51200.0f,
};

/**
* @brief �R���X�g���N�^
*
* @param pTarget  �����Ώ�
*
* @return ����
*/ 
ControllerTypeDebugOrthoView::ControllerTypeDebugOrthoView( const Target::TargetBase* pTarget ) : 

  // ��������
  m_DistanceLevel( 1 )

  // �O���I�u�W�F�N�g
  ,m_pTarget( pTarget )
{

}

/**
* @brief �f�X�g���N�^
*
* @return ����
*/
ControllerTypeDebugOrthoView::~ControllerTypeDebugOrthoView( void )
{

}

/**
* @brief ����������
* @note  Controller��CameraUnit�̐ݒ肳�ꂽ����Ɏ��s�����
*
* @param  pCamera ���삷��J�����̎���
*
* @return ����
*/
void ControllerTypeDebugOrthoView::SetUp( poke_3d::model::BaseCamera* pCamera )
{

}

/**
* @brief �X�V����
*
* @param  pCamera ���삷��J�����̎���
*
* @return ����
*/
void ControllerTypeDebugOrthoView::Update( poke_3d::model::BaseCamera* pCamera )
{
  //
  // �ŐV�^�[�Q�b�g���W���擾
  //
  gfl2::math::Vector3 targetPosition;
  gfl2::math::Vector3 cameraPosition;
  if( m_pTarget == NULL || m_pTarget->GetCameraTargetPosition( &targetPosition ) == false )
  {
    return;
  }

  //
  // ��ԍX�V
  //
     
  // ����
  {
    GameSystem::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSystem::GameManager );
    gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
    gfl2::ui::VectorDevice* pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
    gfl2::ui::Button* pButton = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
    
    // scale�X�V
    if( pButton->IsTrigger( gfl2::ui::BUTTON_UP ) )
    {
      if( m_DistanceLevel == 0 )
      {
        m_DistanceLevel = GFL_NELEMS( sc_OrthoHeightTable ) - 1;
      }
      else
      {
        m_DistanceLevel--;
      }
    }
    else if( pButton->IsTrigger( gfl2::ui::BUTTON_DOWN ) )
    {
      if( m_DistanceLevel >= GFL_NELEMS( sc_OrthoHeightTable ) - 1 )
      {
        m_DistanceLevel = 0;
      }
      else
      {
        m_DistanceLevel++;
      }
    }
  }

  // �ŏI�ʒu���v�Z
  {
    gfl2::math::Vector3 cameraOffset( 0.0f, 4000.0f, 5.0f ); // �K��
    // gfl2::math::Vector3 cameraOffset( 0.0f, sc_OrthoHeightTable[ m_DistanceLevel ], 0.1f );
    cameraPosition = targetPosition + cameraOffset;
  }

  //
  // �J�����I�u�W�F�N�g�ɐݒ�
  //

  pCamera->SetupCameraLookAt(
    cameraPosition,
    targetPosition,
    gfl2::math::Vector3( 0.0f, 1.0f, 0.0f ) 
    );
}

/**
* @brief ���݂̐ݒ�l���琶�������v���W�F�N�V�����s����擾����
* @note  ���̊֐���false��Ԃ��ꍇ�́ABaseCamera����s����擾����
* @note  ���̊֐��ɂ��virtual������Ă΂��+�C���^�[�t�F�C�X����Ȃ��Ȃ�̂ō폜����\��������
*
* @param  pOut �s��i�[��( �߂�l true ���̂ݒl������)
*
* @return ����
*/
b32 ControllerTypeDebugOrthoView::GetProjectionMatrixOverride( gfl2::math::Matrix44* pOut ) const
{
  // f32 f = 400.0f / 240.0f;
  f32 f = gfl2::gfx::GFGLUtil::GetDisplayWidthByDisplayMode( gfl2::gfx::DisplayMode::HD720P ) 
      / (f32)gfl2::gfx::GFGLUtil::GetDisplayHeightByDisplayMode( gfl2::gfx::DisplayMode::HD720P ); // @todo
  b32 pivotFlag( true );

  gfl2::math::PivotDirection pivot = gfl2::math::PIVOT_NONE;
  if( pivotFlag )
  {
    pivot = gfl2::math::PIVOT_UPSIDE_TO_TOP;
  }

  f32 offset = sc_OrthoHeightTable[ m_DistanceLevel ];

  nw::math::MTX44 proj; // = gfl2::math::ConvertGfMtx44ToNwMtx44( m_proj );
  gfl2::math::MTX44OrthoPivot(
    &proj,
    offset * -0.5f * f,
    offset * 0.5f * f,
    offset * -0.5f,
    offset * 0.5f,
    1.0f,
    10000.0f,
    pivot );
  *pOut = gfl2::math::ConvertNwMtx44ToGfMtx44( proj );

  return true;
}

/**
* @brief ���݂̐ݒ�l���琶�������r���[�s����擾����
* @note  ���̊֐���false��Ԃ��ꍇ�́ABaseCamera����s����擾����
* @note  ���̊֐��ɂ��virtual������Ă΂��+�C���^�[�t�F�C�X����Ȃ��Ȃ�̂ō폜����\��������
*
* @param  pOut �s��i�[��( �߂�l true ���̂ݒl������)
*
* @return ����
*/
b32 ControllerTypeDebugOrthoView::GetViewMatrixOverride( gfl2::math::Matrix34* pOut ) const
{
  return false;
}

GFL_NAMESPACE_END( Controller );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );