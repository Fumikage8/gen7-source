//=============================================================================
/**
 * @file gfl_UI_DeviceManager.cpp
 * @brief �f�o�C�X�Ǘ��̒��ۃN���X
 * @author obata_toshihiro
 * @date 2010.10.13
 */
//=============================================================================
#include <nn.h>
#include "debug/gfl_Assert.h"
#include "ui/gfl_UI_VectorDevice.h"
#include "ui/gfl_UI_Button.h"
#include "ui/gfl_UI_TouchPanel.h"
#include "ui/gfl_UI_Accelerometer.h"
#include "ui/gfl_UI_Gyroscope.h"
#include "ui/gfl_UI_DeviceManager.h"

namespace gfl {
  namespace ui {


    //-------------------------------------------------------------------------
    /**
     * @brief �R���X�g���N�^
     */
    //-------------------------------------------------------------------------
    DeviceManager::DeviceManager( void ) :
      m_crosskeyNum( 0 ),
      m_stickNum( 0 ),
      m_buttonNum( 0 ),
      m_touchpanelNum( 0 ),
      m_accelerometerNum( 0 )
#ifdef GFL_UI_USE_GYROSCOPE
      ,m_gyroscopeNum( 0 )
#endif  // GFL_UI_USE_GYROSCOPE
    {
      this->InitAllDevices();
    }

    /**
     * @brief ���ׂẴf�o�C�X�z�������������
     */
    void DeviceManager::InitAllDevices( void )
    {
      // �\���L�[
      for( DeviceId i=0; i<MAX_CROSSKEY_NUM; i++ ) {
        m_crosskey[i] = NULL;
      }

      // �A�i���O�X�e�B�b�N
      for( DeviceId i=0; i<MAX_STICK_NUM; i++ ) {
        m_stick[i] = NULL;
      }

      // �{�^��
      for( DeviceId i=0; i<MAX_BUTTON_NUM; i++ ) {
        m_button[i] = NULL;
      }

      // �^�b�`�p�l��
      for( DeviceId i=0; i<MAX_TOUCH_PANEL_NUM; i++ ) {
        m_touchpanel[i] = NULL;
      }

      // �����x�Z���T
      for( DeviceId i=0; i<MAX_ACCELEROMETER_NUM; i++ ) {
        m_accelerometer[i] = NULL;
      }

#ifdef GFL_UI_USE_GYROSCOPE
      // �W���C���Z���T
      for( DeviceId i=0; i<MAX_GYROSCOPE_NUM; i++ ) {
        m_gyroscope[i] = NULL;
      }
#endif  // GFL_UI_USE_GYROSCOPE
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �f�X�g���N�^
     */
    //-------------------------------------------------------------------------
    DeviceManager::~DeviceManager()
    {
      this->DeleteAllDevices();
    }

    /**
     * @brief ���ׂẴf�o�C�X��j������
     */
    void DeviceManager::DeleteAllDevices( void )
    {
      // �\���L�[
      for( DeviceId i=0; i<MAX_CROSSKEY_NUM; i++ ) {
        this->RemoveCrossKey(i);
      }

      // �A�i���O�X�e�B�b�N
      for( DeviceId i=0; i<MAX_STICK_NUM; i++ ) {
        this->RemoveStick(i);
      }

      // �{�^��
      for( DeviceId i=0; i<MAX_BUTTON_NUM; i++ ) {
        this->RemoveButton(i);
      }

      // �^�b�`�p�l��
      for( DeviceId i=0; i<MAX_TOUCH_PANEL_NUM; i++ ) {
        this->RemoveTouchPanel(i);
      }

      // �����x�Z���T
      for( DeviceId i=0; i<MAX_ACCELEROMETER_NUM; i++ ) {
        this->RemoveAccelerometer(i);
      }

#ifdef GFL_UI_USE_GYROSCOPE
      // �W���C���Z���T
      for( DeviceId i=0; i<MAX_GYROSCOPE_NUM; i++ ) {
        this->RemoveGyroscope(i);
      }
#endif  // GFL_UI_USE_GYROSCOPE
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �\���L�[���擾����
     * @param keyID  �擾����\���L�[��ID
     */
    //-------------------------------------------------------------------------
    VectorDevice* DeviceManager::GetCrossKey( DeviceId keyID ) const
    {
      if( MAX_CROSSKEY_NUM <= keyID ) {
        GFL_ASSERT(0); //@check
        return NULL;
      }
      return m_crosskey[ keyID ];
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �\���L�[��ǉ�����
     * @param heap         �f�o�C�X�̐����Ɏg�p����q�[�v
     * @param keyID        �ǉ�����\���L�[��ID
     * @param implementer  �f�o�C�X�̎���b��
     */
    //-------------------------------------------------------------------------
    void DeviceManager::AddCrossKey( 
        gfl::heap::HeapBase* heap,
        DeviceId keyID,
        DeviceImplementerForVectorDevice* implementer )
    {
      if( m_crosskey[ keyID ] == NULL ) {
        m_crosskey[ keyID ] = GFL_NEW( heap ) VectorDevice( heap, implementer );
        m_crosskey[ keyID ]->SetDeviceRunningEnable( true );
        m_crosskeyNum++;
      }
    }

    /**
     * @brief �\���L�[���폜����
     * @param keyID  �폜����\���L�[��ID
     */
    void DeviceManager::RemoveCrossKey( DeviceId keyID )
    {
      if( m_crosskey[ keyID ] ) {
        GFL_DELETE m_crosskey[ keyID ];
        m_crosskey[ keyID ] = NULL;
        m_crosskeyNum--;
      }
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief �A�i���O�X�e�B�b�N���擾����
     * @param stickID  �擾����A�i���O�X�e�B�b�N��ID
     */
    //-------------------------------------------------------------------------
    VectorDevice* DeviceManager::GetStick( DeviceId stickID ) const
    {
      if( MAX_STICK_NUM <= stickID ) {
        GFL_ASSERT(0); //@check
        return NULL;
      }
      return m_stick[ stickID ];
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �A�i���O�X�e�B�b�N��ǉ�����
     * @param heap         �f�o�C�X�̐����Ɏg�p����q�[�v
     * @param stickID      �ǉ�����X�e�B�b�N��ID
     * @param implementer  �f�o�C�X�̎���b��
     */
    //-------------------------------------------------------------------------
    void DeviceManager::AddStick( 
        gfl::heap::HeapBase* heap,
        DeviceId stickID,
        DeviceImplementerForVectorDevice* implementer )
    {
      if( m_stick[ stickID ] == NULL ) {
        m_stick[ stickID ] = GFL_NEW( heap ) VectorDevice( heap, implementer );
        m_stick[ stickID ]->SetDeviceRunningEnable( true );
        m_stickNum++;
      }
    }

    /**
     * @brief �A�i���O�X�e�B�b�N���폜����
     * @param stickID  �폜����X�e�B�b�N��ID
     */
    void DeviceManager::RemoveStick( DeviceId stickID )
    {
      if( m_stick[ stickID ] ) {
        GFL_DELETE m_stick[ stickID ];
        m_stick[ stickID ] = NULL;
        m_stickNum--;
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �{�^�����擾����
     * @param buttonID  �擾����{�^����ID
     */
    //-------------------------------------------------------------------------
    Button* DeviceManager::GetButton( DeviceId buttonID ) const
    {
      if( MAX_BUTTON_NUM <= buttonID ) {
        GFL_ASSERT(0); //@check
        return NULL;
      }
      return m_button[ buttonID ];
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �{�^����ǉ�����
     * @param heap         �f�o�C�X�̐����Ɏg�p����q�[�v
     * @param buttonID     �ǉ�����{�^����ID
     * @param implementer  �f�o�C�X�̎���b��
     */
    //-------------------------------------------------------------------------
    void DeviceManager::AddButton( 
        gfl::heap::HeapBase* heap,
        DeviceId buttonID,
        DeviceImplementer* implementer )
    {
      if( m_button[ buttonID ] == NULL ) {
        m_button[ buttonID ] = GFL_NEW( heap ) Button( heap, implementer );
        m_button[ buttonID ]->SetDeviceRunningEnable( true );
        m_buttonNum++;
      }
    }

    /**
     * @brief �{�^�����폜����
     * @param buttonID  �폜����{�^����ID
     */
    void DeviceManager::RemoveButton( DeviceId buttonID )
    {
      if( m_button[ buttonID ] ) {
        GFL_DELETE m_button[ buttonID ];
        m_button[ buttonID ] = NULL;
        m_buttonNum--;
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �^�b�`�p�l�����擾����
     * @param tpID  �擾����^�b�`�p�l����ID
     */
    //-------------------------------------------------------------------------
    TouchPanel* DeviceManager::GetTouchPanel( DeviceId tpID ) const
    {
      if( MAX_TOUCH_PANEL_NUM <= tpID ) {
        GFL_ASSERT(0); //@check
        return NULL;
      }
      return m_touchpanel[ tpID ];
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �^�b�`�p�l����ǉ�����
     * @param heap         �f�o�C�X�̐����Ɏg�p����q�[�v
     * @param tpID         �ǉ�����^�b�`�p�l����ID
     * @param implementer  �f�o�C�X�̎���b��
     */
    //-------------------------------------------------------------------------
    void DeviceManager::AddTouchPanel( 
        gfl::heap::HeapBase* heap,
        DeviceId tpID,
        DeviceImplementer* implementer )
    {
      if( m_touchpanel[ tpID ] == NULL ) {
        m_touchpanel[ tpID ] = GFL_NEW( heap ) TouchPanel( heap, implementer );
        m_touchpanel[ tpID ]->SetDeviceRunningEnable( true );
        m_touchpanelNum++;
      }
    }

    /**
     * @brief �^�b�`�p�l�����폜����
     * @param tpID  �폜����^�b�`�p�l����ID
     */
    void DeviceManager::RemoveTouchPanel( DeviceId tpID )
    {
      if( m_touchpanel[ tpID ] ) {
        GFL_DELETE m_touchpanel[ tpID ];
        m_touchpanel[ tpID ] = NULL;
        m_touchpanelNum--;
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �����x�Z���T���擾����
     * @param accelID  �擾��������x�Z���T��ID
     */
    //-------------------------------------------------------------------------
    Accelerometer* DeviceManager::GetAccelerometer( DeviceId accelID ) const
    {
      if( MAX_ACCELEROMETER_NUM <= accelID ) {
        GFL_ASSERT(0); //@check
        return NULL;
      }
      return m_accelerometer[ accelID ];
    }

    //-------------------------------------------------------------------------
    /**
     * @brief �����x�Z���T��ǉ�����
     * @param heap         �f�o�C�X�̐����Ɏg�p����q�[�v
     * @param accelID      �ǉ���������x�Z���T��ID
     * @param implementer  �f�o�C�X�̎���b��
     */
    //-------------------------------------------------------------------------
    void DeviceManager::AddAccelerometer( 
        gfl::heap::HeapBase* heap, 
        DeviceId accelID, 
        DeviceImplementerForAccelerometer* accelerometer )
    {
      if( m_accelerometer[ accelID ] == NULL ) {
        m_accelerometer[ accelID ] = GFL_NEW( heap ) Accelerometer( heap, accelerometer );
        m_accelerometer[ accelID ]->SetDeviceRunningEnable( true );
        m_accelerometerNum++;
      }
    }

    /**
     * @brief �����x�Z���T���폜����
     * @param accelID  �폜��������x�Z���T��ID
     */
    void DeviceManager::RemoveAccelerometer( DeviceId accelID )
    {
      if( m_accelerometer[ accelID ] ) {
        GFL_DELETE m_accelerometer[ accelID ];
        m_accelerometer[ accelID ] = NULL;
        m_accelerometerNum--;
      }
    } 

#ifdef GFL_UI_USE_GYROSCOPE
    //-------------------------------------------------------------------------
    /**
     * @brief �W���C���Z���T���擾����
     * @param gyroID  �擾����W���C���Z���T��ID
     */
    //-------------------------------------------------------------------------
    Gyroscope* DeviceManager::GetGyroscope( DeviceId gyroID ) const
    {
      if( MAX_GYROSCOPE_NUM <= gyroID ) {
        GFL_ASSERT(0); //@check
        return NULL;
      }
      return m_gyroscope[ gyroID ];
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief �W���C���Z���T��ǉ�����
     * @param heap         �f�o�C�X�̐����Ɏg�p����q�[�v
     * @param gyroID       �ǉ�����W���C���Z���T��ID
     * @param implementer  �f�o�C�X�̎���b��
     */
    //-------------------------------------------------------------------------
    void DeviceManager::AddGyroscope(
        gfl::heap::HeapBase* heap,
        DeviceId gyroID,
        DeviceImplementerForGyroscope* implementer )
    {
      if( m_gyroscope[ gyroID ] == NULL ) {
        m_gyroscope[ gyroID ] = GFL_NEW( heap ) Gyroscope( heap, implementer );
        m_gyroscope[ gyroID ]->SetDeviceRunningEnable( true );
        m_gyroscopeNum++;
      }
    }

    /**
     * @brief �W���C���Z���T���폜����
     * @param gyroID  �폜����W���C���Z���T��ID
     */
    void DeviceManager::RemoveGyroscope( DeviceId gyroID )
    {
      if( m_gyroscope[ gyroID ] ) {
        GFL_DELETE m_gyroscope[ gyroID ];
        m_gyroscope[ gyroID ] = NULL;
        m_gyroscopeNum--;
      }
    }
#endif  // GFL_UI_USE_GYROSCOPE

    //-------------------------------------------------------------------------
    /**
     * @brief �S�Ẵf�o�C�X�̓���t���[�����[�g��ݒ肷��
     * @param rate  �V���ɐݒ肷��t���[�����[�g
     */
    //-------------------------------------------------------------------------
    void DeviceManager::SetFrameRateForAllDevice( Device::FrameRate rate )
    {
      // �\���L�[
      for( DeviceId i=0; i<m_crosskeyNum; i++ ) {
        m_crosskey[i]->SetFrameRate( rate );
      }

      // �A�i���O�X�e�B�b�N
      for( DeviceId i=0; i<m_stickNum; i++ ) {
        m_stick[i]->SetFrameRate( rate );
      }

      // �{�^��
      for( DeviceId i=0; i<m_buttonNum; i++ ) {
        m_button[i]->SetFrameRate( rate );
      }

      // �^�b�`�p�l��
      for( DeviceId i=0; i<m_touchpanelNum; i++ ) {
        m_touchpanel[i]->SetFrameRate( rate );
      }

      // �����x�Z���T
      for( DeviceId i=0; i<m_accelerometerNum; i++ ) {
        m_accelerometer[i]->SetFrameRate( rate );
      }

#ifdef GFL_UI_USE_GYROSCOPE
      // �W���C���Z���T
      for( DeviceId i=0; i<m_gyroscopeNum; i++ ) {
        m_gyroscope[i]->SetFrameRate( rate );
      }
#endif  // GFL_UI_USE_GYROSCOPE
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief �S�Ẵf�o�C�X�̓��͍X�V�^�C�~���O�𓯊�����
     */
    //-------------------------------------------------------------------------
    void DeviceManager::SynchronizeInputUpdateForAllDevice( void )
    {
      // �\���L�[
      for( DeviceId i=0; i<m_crosskeyNum; i++ ) {
        m_crosskey[i]->SynchronizeInputUpdate();
      }

      // �A�i���O�X�e�B�b�N
      for( DeviceId i=0; i<m_stickNum; i++ ) {
        m_stick[i]->SynchronizeInputUpdate();
      }

      // �{�^��
      for( DeviceId i=0; i<m_buttonNum; i++ ) {
        m_button[i]->SynchronizeInputUpdate();
      }

      // �^�b�`�p�l��
      for( DeviceId i=0; i<m_touchpanelNum; i++ ) {
        m_touchpanel[i]->SynchronizeInputUpdate();
      }

      // �����x�Z���T
      for( DeviceId i=0; i<m_accelerometerNum; i++ ) {
        m_accelerometer[i]->SynchronizeInputUpdate();
      }

#ifdef GFL_UI_USE_GYROSCOPE
      // �W���C���Z���T
      for( DeviceId i=0; i<m_gyroscopeNum; i++ ) {
        m_gyroscope[i]->SynchronizeInputUpdate();
      }
#endif  // GFL_UI_USE_GYROSCOPE
    }

#if GFL_DEBUG
    //----------------------------------------------------------------------------
    /**
     *	@brief  �L�[���̎擾�@������
     */
    //-----------------------------------------------------------------------------
    void DeviceManager::SetDebugKeyDisable( bool is_disalbe )
    {
      // �\���L�[
      for( DeviceId i=0; i<m_crosskeyNum; i++ ) {
        m_crosskey[i]->SetDebugKeyDisable( is_disalbe );
      }

      // �A�i���O�X�e�B�b�N
      for( DeviceId i=0; i<m_stickNum; i++ ) {
        m_stick[i]->SetDebugKeyDisable( is_disalbe );
      }

      // �{�^��
      for( DeviceId i=0; i<m_buttonNum; i++ ) {
        m_button[i]->SetDebugKeyDisable( is_disalbe );
      }

      // �^�b�`�p�l��
      for( DeviceId i=0; i<m_touchpanelNum; i++ ) {
        m_touchpanel[i]->SetDebugKeyDisable( is_disalbe );
      }

      // �����x�Z���T
      for( DeviceId i=0; i<m_accelerometerNum; i++ ) {
        m_accelerometer[i]->SetDebugKeyDisable( is_disalbe );
      }

#ifdef GFL_UI_USE_GYROSCOPE
      // �W���C���Z���T
      for( DeviceId i=0; i<m_gyroscopeNum; i++ ) {
        m_gyroscope[i]->SetDebugKeyDisable( is_disalbe );
      }
#endif  // GFL_UI_USE_GYROSCOPE
    }
#endif // GFL_DEBUG

    //-------------------------------------------------------------------------
    /**
     * @brief �S�f�o�C�X�̉ғ���Ԃ�ύX����
     * @param isEnable  true:�f�o�C�X���ғ�����, false:�f�o�C�X�̋������~����
     */
    //-------------------------------------------------------------------------
    void DeviceManager::SetAllDeviceRunningEnable( bool isEnable )
    {
      // �\���L�[
      for( DeviceId i=0; i<m_crosskeyNum; i++ ) {
        m_crosskey[i]->SetDeviceRunningEnable( isEnable );
      }

      // �A�i���O�X�e�B�b�N
      for( DeviceId i=0; i<m_stickNum; i++ ) {
        m_stick[i]->SetDeviceRunningEnable( isEnable );
      }

      // �{�^��
      for( DeviceId i=0; i<m_buttonNum; i++ ) {
        m_button[i]->SetDeviceRunningEnable( isEnable );
      }

      // �^�b�`�p�l��
      for( DeviceId i=0; i<m_touchpanelNum; i++ ) {
        m_touchpanel[i]->SetDeviceRunningEnable( isEnable );
        m_touchpanel[i]->UpdateTouchPanel();
      }

      // �����x�Z���T
      for( DeviceId i=0; i<m_accelerometerNum; i++ ) {
        m_accelerometer[i]->SetDeviceRunningEnable( isEnable );
      }

#ifdef GFL_UI_USE_GYROSCOPE
      // �W���C���Z���T
      for( DeviceId i=0; i<m_gyroscopeNum; i++ ) {
        m_gyroscope[i]->SetDeviceRunningEnable( isEnable );
      }
#endif  // GFL_UI_USE_GYROSCOPE
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ���ׂẴf�o�C�X���X�V����
     */
    //-------------------------------------------------------------------------
    void DeviceManager::UpdateAllDevices( void )
    {
      // �\���L�[
      for( DeviceId i=0; i<m_crosskeyNum; i++ ) {
        m_crosskey[i]->UpdateDevice();
      }

      // �A�i���O�X�e�B�b�N
      for( DeviceId i=0; i<m_stickNum; i++ ) {
        m_stick[i]->UpdateDevice();
      }

      // �{�^��
      for( DeviceId i=0; i<m_buttonNum; i++ ) {
        m_button[i]->UpdateDevice();
      }

      // �^�b�`�p�l��
      for( DeviceId i=0; i<m_touchpanelNum; i++ ) {
        m_touchpanel[i]->UpdateDevice();
        m_touchpanel[i]->UpdateTouchPanel();
      }

      // �����x�Z���T
      for( DeviceId i=0; i<m_accelerometerNum; i++ ) {
        m_accelerometer[i]->UpdateDevice();
      }

#ifdef GFL_UI_USE_GYROSCOPE
      // �W���C���Z���T
      for( DeviceId i=0; i<m_gyroscopeNum; i++ ) {
        m_gyroscope[i]->UpdateDevice();
      }
#endif  // GFL_UI_USE_GYROSCOPE
    }



  } //namespace ui
} //namespace gfl
