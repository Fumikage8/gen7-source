//=============================================================================
/**
 * @file   gfl_UI_DeviceImplementerForVectorDevice.h
 * @brief  �x�N�^�[�^�f�o�C�X�̓��͂����o����N���X�̃C���^�[�t�F�[�X
 * @author obata_toshihiro
 * @date   2011.04.04
 */
//=============================================================================
#include "ui/gfl_UI_VectorDevice.h"
#include "./gfl_UI_DeviceImplementer.h"
#include "./gfl_UI_DeviceImplementerForVectorDevice.h"

namespace gfl {
  namespace ui {


    //-------------------------------------------------------------------------
    /**
     * @brief �f�t�H���g�R���X�g���N�^
     */
    //------------------------------------------------------------------------- 
    DeviceImplementerForVectorDevice::DeviceImplementerForVectorDevice( void ) : 
      DeviceImplementer()
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �f�X�g���N�^
     */
    //-------------------------------------------------------------------------
    DeviceImplementerForVectorDevice::~DeviceImplementerForVectorDevice()
    {
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief ���̓f�[�^�̃T�C�Y���擾����
     */
    //-------------------------------------------------------------------------
    u32 DeviceImplementerForVectorDevice::GetDetectiveDataSize( void ) const
    {
      return sizeof(VectorDevice::DetectiveData);
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �����f�[�^�̃T�C�Y���擾����
     */
    //-------------------------------------------------------------------------
    u32 DeviceImplementerForVectorDevice::GetEffectiveDataSize( void ) const
    {
      return sizeof( VectorDevice::EffectiveData );
    }


  } //namespace ui
} //namespace gfl
