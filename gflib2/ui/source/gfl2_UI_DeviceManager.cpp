//=============================================================================
/**
 * @file gfl2_UI_DeviceManager.cpp
 * @brief デバイス管理の抽象クラス
 * @author obata_toshihiro
 * @date 2010.10.13
 */
//=============================================================================
#include "debug/include/gfl2_Assert.h"
#include "ui/include/gfl2_UI_VectorDevice.h"
#include "ui/include/gfl2_UI_Button.h"
#include "ui/include/gfl2_UI_TouchPanel.h"
#include "ui/include/gfl2_UI_Accelerometer.h"
#include "ui/include/gfl2_UI_Gyroscope.h"
#include "ui/include/gfl2_UI_DeviceManager.h"

namespace gfl2 {
  namespace ui {


    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     */
    //-------------------------------------------------------------------------
    DeviceManager::DeviceManager( void ) :
      m_crosskeyNum( 0 ),
      m_stickNum( 0 ),
      m_buttonNum( 0 ),
      m_touchpanelNum( 0 ),
      m_accelerometerNum( 0 )
#ifdef GFL2_UI_USE_GYROSCOPE
      ,m_gyroscopeNum( 0 )
#endif  // GFL2_UI_USE_GYROSCOPE
    {
      this->InitAllDevices();
    }

    /**
     * @brief すべてのデバイス配列を初期化する
     */
    void DeviceManager::InitAllDevices( void )
    {
      // 十字キー
      for( DeviceId i=0; i<MAX_CROSSKEY_NUM; i++ ) {
        m_crosskey[i] = NULL;
      }

      // アナログスティック
      for( DeviceId i=0; i<MAX_STICK_NUM; i++ ) {
        m_stick[i] = NULL;
      }

      // ボタン
      for( DeviceId i=0; i<MAX_BUTTON_NUM; i++ ) {
        m_button[i] = NULL;
      }

      // タッチパネル
      for( DeviceId i=0; i<MAX_TOUCH_PANEL_NUM; i++ ) {
        m_touchpanel[i] = NULL;
      }

      // 加速度センサ
      for( DeviceId i=0; i<MAX_ACCELEROMETER_NUM; i++ ) {
        m_accelerometer[i] = NULL;
      }

#ifdef GFL2_UI_USE_GYROSCOPE
      // ジャイロセンサ
      for( DeviceId i=0; i<MAX_GYROSCOPE_NUM; i++ ) {
        m_gyroscope[i] = NULL;
      }
#endif  // GFL2_UI_USE_GYROSCOPE
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    DeviceManager::~DeviceManager()
    {
      this->DeleteAllDevices();
    }

    /**
     * @brief すべてのデバイスを破棄する
     */
    void DeviceManager::DeleteAllDevices( void )
    {
      // 十字キー
      for( DeviceId i=0; i<MAX_CROSSKEY_NUM; i++ ) {
        this->RemoveCrossKey(i);
      }

      // アナログスティック
      for( DeviceId i=0; i<MAX_STICK_NUM; i++ ) {
        this->RemoveStick(i);
      }

      // ボタン
      for( DeviceId i=0; i<MAX_BUTTON_NUM; i++ ) {
        this->RemoveButton(i);
      }

      // タッチパネル
      for( DeviceId i=0; i<MAX_TOUCH_PANEL_NUM; i++ ) {
        this->RemoveTouchPanel(i);
      }

      // 加速度センサ
      for( DeviceId i=0; i<MAX_ACCELEROMETER_NUM; i++ ) {
        this->RemoveAccelerometer(i);
      }

#ifdef GFL2_UI_USE_GYROSCOPE
      // ジャイロセンサ
      for( DeviceId i=0; i<MAX_GYROSCOPE_NUM; i++ ) {
        this->RemoveGyroscope(i);
      }
#endif  // GFL2_UI_USE_GYROSCOPE
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 十字キーを取得する
     * @param keyID  取得する十字キーのID
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
     * @brief 十字キーを追加する
     * @param heap         デバイスの生成に使用するヒープ
     * @param keyID        追加する十字キーのID
     * @param implementer  デバイスの実装b部
     */
    //-------------------------------------------------------------------------
    void DeviceManager::AddCrossKey( 
        gfl2::heap::HeapBase* heap,
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
     * @brief 十字キーを削除する
     * @param keyID  削除する十字キーのID
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
     * @brief アナログスティックを取得する
     * @param stickID  取得するアナログスティックのID
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
     * @brief アナログスティックを追加する
     * @param heap         デバイスの生成に使用するヒープ
     * @param stickID      追加するスティックのID
     * @param implementer  デバイスの実装b部
     */
    //-------------------------------------------------------------------------
    void DeviceManager::AddStick( 
        gfl2::heap::HeapBase* heap,
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
     * @brief アナログスティックを削除する
     * @param stickID  削除するスティックのID
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
     * @brief ボタンを取得する
     * @param buttonID  取得するボタンのID
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
     * @brief ボタンを追加する
     * @param heap         デバイスの生成に使用するヒープ
     * @param buttonID     追加するボタンのID
     * @param implementer  デバイスの実装b部
     */
    //-------------------------------------------------------------------------
    void DeviceManager::AddButton( 
        gfl2::heap::HeapBase* heap,
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
     * @brief ボタンを削除する
     * @param buttonID  削除するボタンのID
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
     * @brief タッチパネルを取得する
     * @param tpID  取得するタッチパネルのID
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
     * @brief タッチパネルを追加する
     * @param heap         デバイスの生成に使用するヒープ
     * @param tpID         追加するタッチパネルのID
     * @param implementer  デバイスの実装b部
     */
    //-------------------------------------------------------------------------
    void DeviceManager::AddTouchPanel( 
        gfl2::heap::HeapBase* heap,
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
     * @brief タッチパネルを削除する
     * @param tpID  削除するタッチパネルのID
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
     * @brief 加速度センサを取得する
     * @param accelID  取得する加速度センサのID
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
     * @brief 加速度センサを追加する
     * @param heap         デバイスの生成に使用するヒープ
     * @param accelID      追加する加速度センサのID
     * @param implementer  デバイスの実装b部
     */
    //-------------------------------------------------------------------------
    void DeviceManager::AddAccelerometer( 
        gfl2::heap::HeapBase* heap, 
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
     * @brief 加速度センサを削除する
     * @param accelID  削除する加速度センサのID
     */
    void DeviceManager::RemoveAccelerometer( DeviceId accelID )
    {
      if( m_accelerometer[ accelID ] ) {
        GFL_DELETE m_accelerometer[ accelID ];
        m_accelerometer[ accelID ] = NULL;
        m_accelerometerNum--;
      }
    } 

#ifdef GFL2_UI_USE_GYROSCOPE
    //-------------------------------------------------------------------------
    /**
     * @brief ジャイロセンサを取得する
     * @param gyroID  取得するジャイロセンサのID
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
     * @brief ジャイロセンサを追加する
     * @param heap         デバイスの生成に使用するヒープ
     * @param gyroID       追加するジャイロセンサのID
     * @param implementer  デバイスの実装b部
     */
    //-------------------------------------------------------------------------
    void DeviceManager::AddGyroscope(
        gfl2::heap::HeapBase* heap,
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
     * @brief ジャイロセンサを削除する
     * @param gyroID  削除するジャイロセンサのID
     */
    void DeviceManager::RemoveGyroscope( DeviceId gyroID )
    {
      if( m_gyroscope[ gyroID ] ) {
        GFL_DELETE m_gyroscope[ gyroID ];
        m_gyroscope[ gyroID ] = NULL;
        m_gyroscopeNum--;
      }
    }
#endif  // GFL2_UI_USE_GYROSCOPE

    //-------------------------------------------------------------------------
    /**
     * @brief 全てのデバイスの動作フレームレートを設定する
     * @param rate  新たに設定するフレームレート
     */
    //-------------------------------------------------------------------------
    void DeviceManager::SetFrameRateForAllDevice( Device::FrameRate rate )
    {
      // 十字キー
      for( DeviceId i=0; i<m_crosskeyNum; i++ ) {
        m_crosskey[i]->SetFrameRate( rate );
      }

      // アナログスティック
      for( DeviceId i=0; i<m_stickNum; i++ ) {
        m_stick[i]->SetFrameRate( rate );
      }

      // ボタン
      for( DeviceId i=0; i<m_buttonNum; i++ ) {
        m_button[i]->SetFrameRate( rate );
      }

      // タッチパネル
      for( DeviceId i=0; i<m_touchpanelNum; i++ ) {
        m_touchpanel[i]->SetFrameRate( rate );
      }

      // 加速度センサ
      for( DeviceId i=0; i<m_accelerometerNum; i++ ) {
        m_accelerometer[i]->SetFrameRate( rate );
      }

#ifdef GFL2_UI_USE_GYROSCOPE
      // ジャイロセンサ
      for( DeviceId i=0; i<m_gyroscopeNum; i++ ) {
        m_gyroscope[i]->SetFrameRate( rate );
      }
#endif  // GFL2_UI_USE_GYROSCOPE
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 全てのデバイスの入力更新タイミングを同期する
     */
    //-------------------------------------------------------------------------
    void DeviceManager::SynchronizeInputUpdateForAllDevice( void )
    {
      // 十字キー
      for( DeviceId i=0; i<m_crosskeyNum; i++ ) {
        m_crosskey[i]->SynchronizeInputUpdate();
      }

      // アナログスティック
      for( DeviceId i=0; i<m_stickNum; i++ ) {
        m_stick[i]->SynchronizeInputUpdate();
      }

      // ボタン
      for( DeviceId i=0; i<m_buttonNum; i++ ) {
        m_button[i]->SynchronizeInputUpdate();
      }

      // タッチパネル
      for( DeviceId i=0; i<m_touchpanelNum; i++ ) {
        m_touchpanel[i]->SynchronizeInputUpdate();
      }

      // 加速度センサ
      for( DeviceId i=0; i<m_accelerometerNum; i++ ) {
        m_accelerometer[i]->SynchronizeInputUpdate();
      }

#ifdef GFL2_UI_USE_GYROSCOPE
      // ジャイロセンサ
      for( DeviceId i=0; i<m_gyroscopeNum; i++ ) {
        m_gyroscope[i]->SynchronizeInputUpdate();
      }
#endif  // GFL2_UI_USE_GYROSCOPE
    }

#if GFL_DEBUG
    //----------------------------------------------------------------------------
    /**
     *	@brief  キー情報の取得　無効化
     */
    //-----------------------------------------------------------------------------
    void DeviceManager::SetDebugKeyDisable( bool is_disalbe )
    {
      // 十字キー
      for( DeviceId i=0; i<m_crosskeyNum; i++ ) {
        m_crosskey[i]->SetDebugKeyDisable( is_disalbe );
      }

      // アナログスティック
      for( DeviceId i=0; i<m_stickNum; i++ ) {
        m_stick[i]->SetDebugKeyDisable( is_disalbe );
      }

      // ボタン
      for( DeviceId i=0; i<m_buttonNum; i++ ) {
        m_button[i]->SetDebugKeyDisable( is_disalbe );
      }

      // タッチパネル
      for( DeviceId i=0; i<m_touchpanelNum; i++ ) {
        m_touchpanel[i]->SetDebugKeyDisable( is_disalbe );
      }

      // 加速度センサ
      for( DeviceId i=0; i<m_accelerometerNum; i++ ) {
        m_accelerometer[i]->SetDebugKeyDisable( is_disalbe );
      }

#ifdef GFL2_UI_USE_GYROSCOPE
      // ジャイロセンサ
      for( DeviceId i=0; i<m_gyroscopeNum; i++ ) {
        m_gyroscope[i]->SetDebugKeyDisable( is_disalbe );
      }
#endif  // GFL2_UI_USE_GYROSCOPE
    }
#endif // GFL_DEBUG

    //-------------------------------------------------------------------------
    /**
     * @brief 全デバイスの稼働状態を変更する
     * @param isEnable  true:デバイスを稼働する, false:デバイスの挙動を停止する
     */
    //-------------------------------------------------------------------------
    void DeviceManager::SetAllDeviceRunningEnable( bool isEnable )
    {
      // 十字キー
      for( DeviceId i=0; i<m_crosskeyNum; i++ ) {
        m_crosskey[i]->SetDeviceRunningEnable( isEnable );
      }

      // アナログスティック
      for( DeviceId i=0; i<m_stickNum; i++ ) {
        m_stick[i]->SetDeviceRunningEnable( isEnable );
      }

      // ボタン
      for( DeviceId i=0; i<m_buttonNum; i++ ) {
        m_button[i]->SetDeviceRunningEnable( isEnable );
      }

      // タッチパネル
      for( DeviceId i=0; i<m_touchpanelNum; i++ ) {
        m_touchpanel[i]->SetDeviceRunningEnable( isEnable );
        m_touchpanel[i]->UpdateTouchPanel();
      }

      // 加速度センサ
      for( DeviceId i=0; i<m_accelerometerNum; i++ ) {
        m_accelerometer[i]->SetDeviceRunningEnable( isEnable );
      }

#ifdef GFL2_UI_USE_GYROSCOPE
      // ジャイロセンサ
      for( DeviceId i=0; i<m_gyroscopeNum; i++ ) {
        m_gyroscope[i]->SetDeviceRunningEnable( isEnable );
      }
#endif  // GFL2_UI_USE_GYROSCOPE
    }

    //-------------------------------------------------------------------------
    /**
     * @brief すべてのデバイスを更新する
     */
    //-------------------------------------------------------------------------
    void DeviceManager::UpdateAllDevices( void )
    {
      // 十字キー
      for( DeviceId i=0; i<m_crosskeyNum; i++ ) {
        m_crosskey[i]->UpdateDevice();
      }

      // アナログスティック
      for( DeviceId i=0; i<m_stickNum; i++ ) {
        m_stick[i]->UpdateDevice();
      }

      // ボタン
      for( DeviceId i=0; i<m_buttonNum; i++ ) {
        m_button[i]->UpdateDevice();
      }

      // タッチパネル
      for( DeviceId i=0; i<m_touchpanelNum; i++ ) {
        m_touchpanel[i]->UpdateDevice();
        m_touchpanel[i]->UpdateTouchPanel();
      }

      // 加速度センサ
      for( DeviceId i=0; i<m_accelerometerNum; i++ ) {
        m_accelerometer[i]->UpdateDevice();
      }

#ifdef GFL2_UI_USE_GYROSCOPE
      // ジャイロセンサ
      for( DeviceId i=0; i<m_gyroscopeNum; i++ ) {
        m_gyroscope[i]->UpdateDevice();
      }
#endif  // GFL2_UI_USE_GYROSCOPE
    }



  } //namespace ui
} //namespace gfl2
