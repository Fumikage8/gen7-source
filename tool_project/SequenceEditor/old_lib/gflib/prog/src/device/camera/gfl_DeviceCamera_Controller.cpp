/////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   gfl_DeviceCamera_Controller.cpp
 * @brief  �J�����R���g���[��
 * @author obata_toshihiro
 * @date   2010.11.19
 */
/////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <nn.h>
#include "gfl_DeviceCamera_BufferManager.h"
#include <device/camera/gfl_DeviceCamera_System.h>
#include <device/camera/gfl_DeviceCamera_Controller.h>

namespace gfl {
  namespace device { 
    namespace camera {


      //---------------------------------------------------------------------------------
      /**
       * @brief �J�������N������
       */
      //---------------------------------------------------------------------------------
      void Controller::Activate( void )
      {
        m_pCameraSystem->ActivateCamera( m_cameraID );
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief ���ׂẴJ�������~����
       */
      //---------------------------------------------------------------------------------
      void Controller::DeactivateAllCamera( void )
      {
        m_pCameraSystem->DeactivateAllCamera();
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief �L���v�`�����J�n����
       */
      //---------------------------------------------------------------------------------
      void Controller::StartCapture( void )
      {
        m_pCameraSystem->StartCapture( m_cameraID );
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief �L���v�`�����~����
       */
      //---------------------------------------------------------------------------------
      void Controller::StopCapture( void )
      {
        m_pCameraSystem->StopCapture( m_cameraID );
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief �L���v�`�������ǂ����𒲂ׂ�
       */
      //---------------------------------------------------------------------------------
      bool Controller::IsCapturing( void ) const
      {
        return m_pCameraSystem->IsCapturing( m_cameraID );
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief �L���v�`�������摜�f�[�^( RGB�t�H�[�}�b�g )���擾����
       */
      //---------------------------------------------------------------------------------
      const void* Controller::GetImage( void ) const
      {
        const BufferManager* p_buffer = m_pCameraSystem->GetBufferManager( m_cameraID );
        const void* p_rgb_data = p_buffer->GetRgbBuffer_Latest();
        return p_rgb_data;
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief �L���v�`���摜( RGB�t�H�[�}�b�g )�̃T�C�Y���擾����
       */
      //---------------------------------------------------------------------------------
      u32 Controller::GetImageSize( void ) const
      {
        const BufferManager* p_buffer = m_pCameraSystem->GetBufferManager( m_cameraID );
        return p_buffer->GetRgbBufferSize();
      }



      //---------------------------------------------------------------------------------
      /**
       * @brief �L���v�`�������摜�f�[�^( YUV�t�H�[�}�b�g )���擾����
       */
      //---------------------------------------------------------------------------------
      const void* Controller::GetYuvImage( void ) const
      {
        const BufferManager* p_buffer = m_pCameraSystem->GetBufferManager( m_cameraID );
        const void* p_yuv_data = p_buffer->GetYuvBuffer_Latest();
        return p_yuv_data;
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief �L���v�`���摜( YUV�t�H�[�}�b�g )�̃T�C�Y���擾����
       */
      //---------------------------------------------------------------------------------
      u32 Controller::GetYuvImageSize( void ) const
      {
        const BufferManager* p_buffer = m_pCameraSystem->GetBufferManager( m_cameraID );
        return p_buffer->GetYuvBufferSize();
      }


      //-------------------------------------------------------------------------------
      /**
       * @brief �I����ݒ肵�܂��B
       * @param[in] exposure  �I�o�ʂ��w�肵�܂��B�w��\�Ȓl�͈̔͂� [-5, +5] �ł��B
       */
      //-------------------------------------------------------------------------------
      void Controller::SetExposure(s8 exposure)
      {
        m_pCameraSystem->SetExposure( m_cameraID, exposure );
      }



      //---------------------------------------------------------------------------------
      /**
       * @brief �R���X�g���N�^
       */
      //---------------------------------------------------------------------------------
      Controller::Controller( const SetupParam& param ) : 
        m_cameraID( param.cameraID ),
        m_pCameraSystem( param.pCameraSystem )
      {
      }


      //---------------------------------------------------------------------------------
      /**
       * @brief �f�X�g���N�^
       */
      //---------------------------------------------------------------------------------
      Controller::~Controller()
      {
      }


    } // namespace camera
  } // namespace device
} // namespace gfl 
