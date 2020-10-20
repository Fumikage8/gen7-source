//=============================================================================
/**
 * @file   gfl_UI_DeviceImplementerForGyroscope.cpp
 * @brief  �W���C���Z���T���͂����o����N���X�̃C���^�[�t�F�[�X
 * @author obata_toshihiro
 * @date   2011.03.31
 */
//=============================================================================
#ifdef GFL_UI_USE_GYROSCOPE

#include "ui/gfl_UI_Gyroscope.h"
#include "./gfl_UI_DeviceImplementer.h"
#include "./gfl_UI_DeviceImplementerForGyroscope.h"

namespace gfl {
  namespace ui {


    //-------------------------------------------------------------------------
    /**
     * @brief �f�t�H���g�R���X�g���N�^
     */
    //-------------------------------------------------------------------------
    DeviceImplementerForGyroscope::DeviceImplementerForGyroscope( void ) :
      DeviceImplementer()
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �f�X�g���N�^
     */
    //-------------------------------------------------------------------------
    DeviceImplementerForGyroscope::~DeviceImplementerForGyroscope()
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���̓f�[�^�̃T�C�Y���擾����
     */
    //-------------------------------------------------------------------------
    u32 DeviceImplementerForGyroscope::GetDetectiveDataSize( void ) const
    {
      return sizeof(Gyroscope::DetectiveData);
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �����f�[�^�̃T�C�Y���擾����
     */
    //-------------------------------------------------------------------------
    u32 DeviceImplementerForGyroscope::GetEffectiveDataSize( void ) const
    {
      return sizeof( Gyroscope::EffectiveData );
    }


  } //namespace ui
} //namespace gfl

#endif  // GFL_UI_USE_GYROSCOPE
