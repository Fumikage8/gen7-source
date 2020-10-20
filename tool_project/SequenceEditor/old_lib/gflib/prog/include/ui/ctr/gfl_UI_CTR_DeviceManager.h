//=============================================================================
/**
 * @file gfl_UI_CTR_DeviceManager.h
 * @brief 3DS �̃f�o�C�X�Ǘ�
 * @author obata_toshihiro
 * @date 2010.10.13
 */
//=============================================================================
#ifndef __GFL_UI_CTR_DEVICEMANAGER_H__
#define __GFL_UI_CTR_DEVICEMANAGER_H__
#pragma once

#include <gfl_Macros.h>
#include <ui/gfl_UI_DeviceManager.h>


namespace gfl {
  namespace ui {


    class CTR_DeviceManager : public DeviceManager 
    {
      GFL_FORBID_COPY_AND_ASSIGN( CTR_DeviceManager ); 

      public: 

      /**
       * @brief �\���L�[�̎��
       */
      enum CROSSKEY_ID {
        CROSSKEY_STANDARD,   // �\���L�[�P��
        CROSSKEY_STICK_EMU,  // �A�i���O�X�e�B�b�N�ɂ��G�~�����[�V�������܂ޏ\���L�[
#ifdef GFL_UI_USE_DEBUG_PAD
        CROSSKEY_DEBUGPAD,   // �f�o�b�O�p�b�h�̏\���L�[
#endif
        CROSSKEY_NUM,
      }; 

      /**
       * @brief �A�i���O�X�e�B�b�N�̎��
       */
      enum STICK_ID {
        STICK_STANDARD,    // �X�e�B�b�N�P��
#ifdef GFL_UI_USE_DEBUG_PAD
        STICK_DEBUGPAD_L,  // �f�o�b�O�p�b�h�̍��X�e�B�b�N
        STICK_DEBUGPAD_R,  // �f�o�b�O�p�b�h�̉E�X�e�B�b�N
#endif
        STICK_NUM,
      }; 

      /**
       * @brief �{�^���̎��
       */
      enum BUTTON_ID {
        BUTTON_STANDARD,    // �{�^���P��
        BUTTON_STICK_EMU,   // �A�i���O�X�e�B�b�N�ɂ��\���{�^���G�~�����[�V�������܂ޏ\���L�[
#ifdef GFL_UI_USE_DEBUG_PAD
        BUTTON_DEBUGPAD,    // �f�o�b�O�p�b�h�̃{�^��
#endif
        BUTTON_NUM,
      }; 

      /**
       * @brief �^�b�`�p�l���̎��
       */
      enum TOUCHPANEL_ID {
        TOUCHPANEL_STANDARD,  // �ʏ�̃^�b�`�p�l��
        TOUCHPANEL_NUM,
      }; 

      /**
       * @brief �����x�Z���T�̎��
       */
      enum ACCELEROMETER_ID {
        ACCELEROMETER_STANDARD,  // �ʏ�̉����x�Z���T
        ACCELEROMETER_NUM,
      }; 

      /**
       * @brief �W���C���Z���T�̎��
       */
      enum GYROSCOPE_ID {
        GYROSCOPE_STANDARD,  // �ʏ�̃W���C���Z���T
        GYROSCOPE_NUM,
      };




      //-----------------------------------------------------------------------
      // �R���X�g���N�^�E�f�X�g���N�^�E��{����
      //-----------------------------------------------------------------------
      /**
       * @brief �R���X�g���N�^
       * @param heap  �f�o�C�X�̐����Ɏg�p����q�[�v
       */
      CTR_DeviceManager( gfl::heap::HeapBase* heap );

      /**
       * @brief �f�X�g���N�^
       */
      virtual ~CTR_DeviceManager(); 

      /**
       * @brief ���C������
       */
      virtual void Main( void );


      //-----------------------------------------------------------------------
      // �����x�Z���T
      //-----------------------------------------------------------------------
      /**
       * @brief �����x�Z���T���N������
       * @param heap      �����x�Z���T�̐����Ɏg�p����q�[�v
       * @param deviceID  �N����������x�Z���T��ID
       */
      virtual void BootAccelerometer( gfl::heap::HeapBase* heap, DeviceId deviceID );

      /**
       * @brief �����x�Z���T���~����
       * @param deviceID  ��~��������x�Z���T��ID
       */
      virtual void StopAccelerometer( DeviceId deviceID );


#ifdef GFL_UI_USE_GYROSCOPE
      //-----------------------------------------------------------------------
      // �W���C���Z���T
      //-----------------------------------------------------------------------
      /**
       * @brief �W���C���Z���T���N������
       * @param heap      �W���C���Z���T�̐����Ɏg�p����q�[�v
       * @param deviceID  �N������W���C���Z���T��ID
       */
      virtual void BootGyroscope( gfl::heap::HeapBase* heap, DeviceId deviceID );

      /**
       * @brief �W���C���Z���T���~����
       * @param deviceID  ��~����W���C���Z���T��ID
       */
      virtual void StopGyroscope( DeviceId deviceID ); 
#endif  // GFL_UI_USE_GYROSCOPE


      //-----------------------------------------------------------------------
      // �f�o�b�O�p�b�h
      //----------------------------------------------------------------------- 
      /**
       * �f�o�b�O�p�b�h�擾
       * @return �p�b�h�X�e�[�^�X
       */ // kimura add
      nn::hid::DebugPadStatus* GetDebugPadStatus( void ) {
        return &m_debug_pad_status;
      }




      private:

      nn::hid::PadReader m_pad_reader;
      nn::hid::PadStatus m_pad_status;
      nn::hid::TouchPanelReader m_touchpanel_reader;
      nn::hid::TouchPanelStatus m_touchpanel_status;
      nn::hid::AccelerometerReader* m_accelerometer_reader;
      nn::hid::AccelerometerStatus* m_accelerometer_status;
#ifdef GFL_UI_USE_GYROSCOPE
      nn::hid::GyroscopeReader* m_gyroscope_reader;
      nn::hid::GyroscopeStatus* m_gyroscope_status;
#endif  // GFL_UI_USE_GYROSCOPE
      nn::hid::DebugPadReader m_debug_pad_reader;
      nn::hid::DebugPadStatus m_debug_pad_status;


      /**
       * @brief ������ԂŕK�v�ȃf�o�C�X���쐬����
       * @param heap  �e��C���X�^���X�̐����Ɏg�p����q�[�v
       */
      virtual void CreateInitialDevices( gfl::heap::HeapBase* heap );

      /**
       * @brief �����x�Z���T�̃R�A�I�u�W�F�N�g�𐶐�����
       * @param heap  �C���X�^���X�̐����Ɏg�p����q�[�v
       */
      void CreateAccelerometerReaderAndStatus( gfl::heap::HeapBase* heap );

      /**
       * @brief �����x�Z���T�̃R�A�I�u�W�F�N�g��j������
       */
      void DeleteAccelerometerReaderAndStatus( void );

#ifdef GFL_UI_USE_GYROSCOPE
      /**
       * @brief �W���C���Z���T�̃R�A�I�u�W�F�N�g�𐶐�����
       * @param heap  �C���X�^���X�̐����Ɏg�p����q�[�v
       */
      void CreateGyroscopeReaderAndStatus( gfl::heap::HeapBase* heap );

      /**
       * @brief �W���C���Z���T�̃R�A�I�u�W�F�N�g��j������
       */
      void DeleteGyroscopeReaderAndStatus( void );
#endif  // GFL_UI_USE_GYROSCOPE
    };


  }  //namespace ui
}  //namespace gfl


#endif //__GFL_UI_CTR_DEVICEMANAGER_H__
