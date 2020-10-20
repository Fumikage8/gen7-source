//=============================================================================
/**
 * @file   gfl_UI_DeviceManager.h
 * @brief  �f�o�C�X�Ǘ��̒��ۃN���X
 * @author obata_toshihiro
 * @date   2010.10.13
 */
//=============================================================================
#ifndef __GFL_UI_DEVICEMANAGER_H__
#define __GFL_UI_DEVICEMANAGER_H__
#pragma once


#include <gfl_Heap.h>
#include <ui/gfl_UI_Types.h>
#include <ui/gfl_UI_Device.h>

namespace gfl {
  namespace ui { 


    // �f�o�C�X����肷��^
    typedef u8 DeviceId;   

    class DeviceImplementer;
    class DeviceImplementerForVectorDevice;
    class DeviceImplementerForAccelerometer;
#ifdef GFL_UI_USE_GYROSCOPE
    class DeviceImplementerForGyroscope;
#endif  // GFL_UI_USE_GYROSCOPE
    class VectorDevice;
    class Button;
    class TouchPanel;
    class Accelerometer;
#ifdef GFL_UI_USE_GYROSCOPE
    class Gyroscope; 
#endif  // GFL_UI_USE_GYROSCOPE

    class DeviceManager 
    {
      GFL_FORBID_COPY_AND_ASSIGN( DeviceManager );

      public: 

      // �e�f�o�C�X�̍ő吔
      static const u8 MAX_CROSSKEY_NUM      = 3;
      static const u8 MAX_STICK_NUM         = 3;
      static const u8 MAX_BUTTON_NUM        = 3;
      static const u8 MAX_TOUCH_PANEL_NUM   = 1;
      static const u8 MAX_ACCELEROMETER_NUM = 1;
      static const u8 MAX_GYROSCOPE_NUM     = 1;


      //---------------------------------------------------------------------
      // �R���X�g���N�^�E�f�X�g���N�^�E��{����
      //--------------------------------------------------------------------- 
      /**
       * @brief �R���X�g���N�^
       */
      DeviceManager( void );

      /**
       * @brief �f�X�g���N�^
       */
      virtual ~DeviceManager();

      /**
       * @brief ���C������( ���t���[���Ă�ł������� )
       */
      virtual void Main( void ) = 0; 

      //--------------------------------------------------------------------- 
      // �f�o�C�X�̎擾
      //--------------------------------------------------------------------- 
      /**
       * @brief �\���L�[���擾����
       * @param keyID  �擾����\���L�[��ID
       */
      VectorDevice* GetCrossKey( DeviceId keyID ) const;

      /**
       * @brief �A�i���O�X�e�B�b�N���擾����
       * @param stickID  �擾����A�i���O�X�e�B�b�N��ID
       */
      VectorDevice* GetStick( DeviceId stickID ) const;

      /**
       * @brief �{�^�����擾����
       * @param buttonID  �擾����{�^����ID
       */
      Button* GetButton( DeviceId buttonID ) const;

      /**
       * @brief �^�b�`�p�l�����擾����
       * @param tpID  �擾����^�b�`�p�l����ID
       */
      TouchPanel* GetTouchPanel( DeviceId tpID ) const;

      /**
       * @brief �����x�Z���T���擾����
       * @param accelID  �擾��������x�Z���T��ID
       */
      Accelerometer* GetAccelerometer( DeviceId accelID ) const;

#ifdef GFL_UI_USE_GYROSCOPE
      /**
       * @brief �W���C���Z���T���擾����
       * @param gyroID  �擾����W���C���Z���T��ID
       */
      Gyroscope* GetGyroscope( DeviceId gyroID ) const;
#endif  // GFL_UI_USE_GYROSCOPE

      //---------------------------------------------------------------------
      // �f�o�C�X�̈ꊇ����
      //---------------------------------------------------------------------
      /**
       * @brief �S�f�o�C�X�̉ғ���Ԃ�ύX����
       * @param isEnable  true:�f�o�C�X���ғ�����, false:�f�o�C�X�̋������~����
       */
      void SetAllDeviceRunningEnable( bool isEnable );

      //---------------------------------------------------------------------
      // �����x�Z���T
      //---------------------------------------------------------------------
      /**
       * @brief �����x�Z���T���N������
       * @param heap     �����x�Z���T�̐����Ɏg�p����q�[�v
       * @param accelID  �N����������x�Z���T��ID
       */
      virtual void BootAccelerometer( gfl::heap::HeapBase* heap, DeviceId accelID ) = 0;

      /**
       * @brief �����x�Z���T���~����
       * @param accelID  ��~��������x�Z���T��ID
       */
      virtual void StopAccelerometer( DeviceId accelID ) = 0;

#ifdef GFL_UI_USE_GYROSCOPE
      //---------------------------------------------------------------------
      // �W���C���Z���T
      //---------------------------------------------------------------------
      /**
       * @brief �W���C���Z���T���N������
       * @param heap    �W���C���Z���T�̐����Ɏg�p����q�[�v
       * @param gyroID  �N������W���C���Z���T��ID
       */
      virtual void BootGyroscope( gfl::heap::HeapBase* heap, DeviceId gyroID ) = 0;

      /**
       * @brief �W���C���Z���T���~����
       * @param gyroID  ��~����W���C���Z���T��ID
       */
      virtual void StopGyroscope( DeviceId gyroID ) = 0; 
#endif  // GFL_UI_USE_GYROSCOPE

      //--------------------------------------------------------------------- 
      // �t���[�����[�g
      //--------------------------------------------------------------------- 
      /**
       * @brief �S�Ẵf�o�C�X�̓���t���[�����[�g��ݒ肷��
       * @param rate  �V���ɐݒ肷��t���[�����[�g
       */
      void SetFrameRateForAllDevice( Device::FrameRate rate ); 

      /**
       * @brief �S�Ẵf�o�C�X�̓��͍X�V�^�C�~���O�𓯊�����
       */
      void SynchronizeInputUpdateForAllDevice( void );


#if GFL_DEBUG
      void SetDebugKeyDisable( bool is_disalbe );
#endif // GFL_DEBUG


      protected:

      // �ێ����Ă���f�o�C�X�E�I�u�W�F�N�g�̐�
      u8 m_crosskeyNum;          // �\���L�[
      u8 m_stickNum;             // �A�i���O�X�e�B�b�N
      u8 m_buttonNum;            // �{�^��
      u8 m_touchpanelNum;        // �^�b�`�p�l��
      u8 m_accelerometerNum;     // �����x�Z���T
      u8 m_gyroscopeNum;         // �W���C���Z���T

      // �f�o�C�X�E�I�u�W�F�N�g
      VectorDevice* m_crosskey[ MAX_CROSSKEY_NUM ];             // �\���L�[
      VectorDevice* m_stick[ MAX_STICK_NUM ];                   // �A�i���O�X�e�B�b�N
      Button* m_button[ MAX_BUTTON_NUM ];                       // �{�^��
      TouchPanel* m_touchpanel[ MAX_TOUCH_PANEL_NUM ];          // �^�b�`�p�l��
      Accelerometer* m_accelerometer[ MAX_ACCELEROMETER_NUM ];  // �����x�Z���T
#ifdef GFL_UI_USE_GYROSCOPE
      Gyroscope* m_gyroscope[ MAX_GYROSCOPE_NUM ];              // �W���C���Z���T
#endif  // GFL_UI_USE_GYROSCOPE


      /**
       * @brief ������ԂŕK�v�ȃf�o�C�X���쐬����
       * @param heap  �e��f�o�C�X�̐����Ɏg�p����q�[�v
       */
      virtual void CreateInitialDevices( gfl::heap::HeapBase* heap ) = 0;

      /**
       * @brief �f�o�C�X��ǉ�����
       */
      void AddCrossKey( gfl::heap::HeapBase* heap, DeviceId keyID, DeviceImplementerForVectorDevice* implementer ); 
      void AddStick( gfl::heap::HeapBase* heap, DeviceId stickID, DeviceImplementerForVectorDevice* implementer ); 
      void AddButton( gfl::heap::HeapBase* heap, DeviceId buttonID, DeviceImplementer* implementer ); 
      void AddTouchPanel( gfl::heap::HeapBase* heap, DeviceId tpID, DeviceImplementer* implementer ); 
      void AddAccelerometer( gfl::heap::HeapBase* heap, DeviceId accelID, DeviceImplementerForAccelerometer* implementer ); 
#ifdef GFL_UI_USE_GYROSCOPE
      void AddGyroscope( gfl::heap::HeapBase* heap, DeviceId gyroID, DeviceImplementerForGyroscope* implementer );
#endif  // GFL_UI_USE_GYROSCOPE

      /**
       * @brief �f�o�C�X���폜����
       */
      void RemoveCrossKey( DeviceId keyID );
      void RemoveStick( DeviceId stickID );
      void RemoveButton( DeviceId buttonID );
      void RemoveTouchPanel( DeviceId tpID );
      void RemoveAccelerometer( DeviceId accelID );
#ifdef GFL_UI_USE_GYROSCOPE
      void RemoveGyroscope( DeviceId gyroID );
#endif  // GFL_UI_USE_GYROSCOPE

      /**
       * @brief �S�Ẵf�o�C�X���X�V����
       */
      void UpdateAllDevices( void );

      /**
       * @brief �f�o�C�X�̐����擾����
       */
      u8 GetCrossKeyNum( void ) const { return m_crosskeyNum; }
      u8 GetStickNum( void ) const { return m_stickNum; }
      u8 GetButtonNum( void ) const { return m_buttonNum; }
      u8 GetTouchPanelNum( void ) const { return m_touchpanelNum; }
      u8 GetAccelerometerNum( void ) const { return m_accelerometerNum; }
#ifdef GFL_UI_USE_GYROSCOPE
      u8 GetGyroscopeNum( void ) const { return m_gyroscopeNum; }
#endif  // GFL_UI_USE_GYROSCOPE



      private:
      void InitAllDevices( void );
      void DeleteAllDevices( void );
    };


  }  //namespace ui
}  //namespace gfl

#endif //__GFL_UI_DEVICEMANAGER_H__
