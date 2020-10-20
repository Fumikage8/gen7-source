//=============================================================================
/**
 * @file   gfl_UI_DeviceImplementerForVectorDevice.h
 * @brief  �x�N�^�[�^�f�o�C�X�̓��͂����o����N���X�̃C���^�[�t�F�[�X
 * @author obata_toshihiro
 * @date   2011.04.04
 */
//=============================================================================
#ifndef __GFL_UI_DEVICE_IMPLEMENTER_FOR_VECTORDEVICE_H__
#define __GFL_UI_DEVICE_IMPLEMENTER_FOR_VECTORDEVICE_H__
#include "ui/gfl_UI_VectorDevice.h"
#include "./gfl_UI_DeviceImplementer.h"

namespace gfl {
  namespace ui {


    class DeviceImplementerForVectorDevice : public DeviceImplementer
    { 
      GFL_FORBID_COPY_AND_ASSIGN( DeviceImplementerForVectorDevice );

      public:
      //-----------------------------------------------------------------------
      /**
       * @brief �f�t�H���g�R���X�g���N�^
       */
      //-----------------------------------------------------------------------
      DeviceImplementerForVectorDevice( void );

      //-----------------------------------------------------------------------
      /**
       * @brief �f�X�g���N�^
       */
      //-----------------------------------------------------------------------
      virtual ~DeviceImplementerForVectorDevice();

      //-----------------------------------------------------------------------
      /**
       * @brief ���̓f�[�^�̃T�C�Y���擾����
       */
      //-----------------------------------------------------------------------
      virtual u32 GetDetectiveDataSize( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief �����f�[�^�̃T�C�Y���擾����
       */
      //-----------------------------------------------------------------------
      virtual u32 GetEffectiveDataSize( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief �A�i���O�X�e�B�b�N�̃N�����v���[�h���擾����
       */
      //-----------------------------------------------------------------------
      virtual VectorDevice::StickClampMode GetStickClampMode( void ) const = 0;

      //-----------------------------------------------------------------------
      /**
       * @brief �A�i���O�X�e�B�b�N�̃N�����v���[�h��ݒ肷��
       * @param clamp_mode �ݒ肷��N�����v���[�h
       */
      //-----------------------------------------------------------------------
      virtual void SetStickClampMode( VectorDevice::StickClampMode clamp_mode ) = 0;

      //-----------------------------------------------------------------------
      /**
       * @brief �A�i���O�X�e�B�b�N�̃N�����v�p�����[�^���擾����
       *
       * @param[out] min  �ŏ��N�����v�l�̊i�[��
       * @param[out] max  �ő�N�����v�l�̊i�[��
       */
      //-----------------------------------------------------------------------
      virtual void GetStickClampParam( s16* min, s16* max ) const = 0;

      //-----------------------------------------------------------------------
      /**
       * @brief �A�i���O�X�e�B�b�N�̃N�����v�p�����[�^��ݒ肷��
       *
       * @param[in] min  �ŏ��N�����v�l
       * @param[in] max  �ő�N�����v�l
       */
      //-----------------------------------------------------------------------
      virtual void SetStickClampParam( s16 min, s16 max ) = 0;
    };


  } //namespace ui
} //namespace gfl

#endif //__GFL_UI_DEVICE_IMPLEMENTER_FOR_VECTORDEVICE_H__
