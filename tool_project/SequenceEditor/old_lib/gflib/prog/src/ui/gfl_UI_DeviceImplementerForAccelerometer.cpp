//=============================================================================
/**
 * @file   gfl_UI_DeviceImplementerForAccelerometer.cpp
 * @brief  �����x�Z���T���͂̌��o�N���X��p�C���^�[�t�F�[�X
 * @author obata_toshihiro
 * @date   2011.03.25
 */
//=============================================================================
#include "ui/gfl_UI_Accelerometer.h"
#include "./gfl_UI_DeviceImplementerForAccelerometer.h"


namespace gfl {
  namespace ui {


    //-------------------------------------------------------------------------
    /**
     * @brief �R���X�g���N�^
     */
    //-------------------------------------------------------------------------
    DeviceImplementerForAccelerometer::DeviceImplementerForAccelerometer( void ) : 
      DeviceImplementer()
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �f�X�g���N�^
     */
    //-------------------------------------------------------------------------
    DeviceImplementerForAccelerometer::~DeviceImplementerForAccelerometer( void )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���o�f�[�^�̃T�C�Y���擾����
     */
    //-------------------------------------------------------------------------
    u32 DeviceImplementerForAccelerometer::GetDetectiveDataSize( void ) const
    {
      return sizeof(Accelerometer::DetectiveData);
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �����f�[�^�̃T�C�Y���擾����
     */
    //-------------------------------------------------------------------------
    u32 DeviceImplementerForAccelerometer::GetEffectiveDataSize( void ) const
    {
      return sizeof( Accelerometer::EffectiveData );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���C������
     */
    //-------------------------------------------------------------------------
    void DeviceImplementerForAccelerometer::Main( void )
    {
      // do nothing
    }


  } //namespace ui
} //namespace gfl
