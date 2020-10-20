//=============================================================================
/**
 * @file   gfl2_UI_CTR_DeviceManager.cpp
 * @brief  3DSのデバイス管理
 * @author obata_toshihiro
 * @date   2010.10.13
 */
//=============================================================================
#ifdef  GF_PLATFORM_CTR
#include <nn.h>
#include <heap/include/gfl2_Heap.h>
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_DebugPrint.h>
#include "ui/include/gfl2_UI_VectorDevice.h"
#include "ui/include/gfl2_UI_Button.h"
#include "ui/include/gfl2_UI_TouchPanel.h"
#include "ui/include/ctr/gfl2_UI_CTR_DeviceManager.h"
#include "./gfl2_UI_CTR_CrossKey.h"
#include "./gfl2_UI_CTR_Stick.h"
#include "./gfl2_UI_CTR_Button.h"
#include "./gfl2_UI_CTR_TouchPanel.h"
#include "./gfl2_UI_CTR_Accelerometer.h"
#include "./gfl2_UI_CTR_Gyroscope.h"
#ifdef GFL2_UI_USE_DEBUG_PAD
#include "./gfl2_UI_CTR_DebugCrossKey.h"
#include "./gfl2_UI_CTR_DebugStick.h"
#include "./gfl2_UI_CTR_DebugButton.h"
#endif


namespace gfl2 {
  namespace ui {


    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param heap  デバイスの生成に使用するヒープ
     */
    //-------------------------------------------------------------------------
    CTR_DeviceManager::CTR_DeviceManager( gfl2::heap::HeapBase* heap ) :
      DeviceManager(),
      m_pad_reader(),
      m_pad_status(),
      m_touchpanel_reader(),
      m_touchpanel_status(),
      m_accelerometer_reader( NULL ),
      m_accelerometer_status( NULL ),
#ifdef GFL2_UI_USE_GYROSCOPE
      m_gyroscope_reader( NULL ),
      m_gyroscope_status( NULL ),
#endif  // GFL2_UI_USE_GYROSCOPE
      m_debug_pad_reader(),
      m_debug_pad_status()
    {
      memset( m_pad_status_buffer , 0 , sizeof(m_pad_status_buffer) );
      memset( m_touch_status_buffer , 0 , sizeof(m_touch_status_buffer) );
      
      this->CreateInitialDevices( heap );
    }

    /**
     * @brief 初期状態で必要なデバイスを作成する
     * @param heap  各種インスタンスの生成に使用するヒープ
     */
    void CTR_DeviceManager::CreateInitialDevices( gfl2::heap::HeapBase* heap )
    {
      // 十字キー
      CTR_CrossKey* crosskey_imp = 
        GFL_NEW( heap ) CTR_CrossKey( &m_pad_reader, &m_pad_status ); 
      crosskey_imp->SetStickEmulationDisable();
      this->AddCrossKey( heap, CROSSKEY_STANDARD, crosskey_imp );
      
      // スティックエミュ有りの十字キー
      CTR_CrossKey* crosskey_emu_imp = 
        GFL_NEW( heap ) CTR_CrossKey( &m_pad_reader, &m_pad_status ); 
      crosskey_emu_imp->SetStickEmulationEnable();
      this->AddCrossKey( heap, CROSSKEY_STICK_EMU, crosskey_emu_imp );

      // ボタン
      CTR_Button* button_imp = 
        GFL_NEW( heap ) CTR_Button( &m_pad_reader, &m_pad_status );
      button_imp->SetStickEmulationDisable();
      this->AddButton( heap, BUTTON_STANDARD, button_imp );

      // スティックエミュ有りのボタン
      CTR_Button* button_emu_imp = 
        GFL_NEW( heap ) CTR_Button( &m_pad_reader, &m_pad_status );
      button_emu_imp->SetStickEmulationEnable();
      this->AddButton( heap, BUTTON_STICK_EMU, button_emu_imp );

      // スティック
      CTR_Stick* stick_imp = 
        GFL_NEW( heap ) CTR_Stick( &m_pad_reader, &m_pad_status );
      this->AddStick( heap, STICK_STANDARD, stick_imp );

      // タッチパネル
      CTR_TouchPanel* tp_imp = 
        GFL_NEW( heap ) CTR_TouchPanel( &m_touchpanel_reader, &m_touchpanel_status ); 
      this->AddTouchPanel( heap, TOUCHPANEL_STANDARD, tp_imp ); 


#ifdef GFL2_UI_USE_DEBUG_PAD
      // デバッグパッド
      CTR_DebugCrossKey* debug_crosskey_imp = 
        GFL_NEW( heap ) CTR_DebugCrossKey( &m_debug_pad_reader, &m_debug_pad_status );
      this->AddCrossKey( heap, CROSSKEY_DEBUGPAD, debug_crosskey_imp );

      // 左デバッグスティック
      CTR_DebugStick* debug_left_stick_imp = 
        GFL_NEW( heap ) CTR_DebugStick( 
            CTR_DebugStick::STICK_ID_LEFT, &m_debug_pad_reader, &m_debug_pad_status );
      this->AddStick( heap, STICK_DEBUGPAD_L, debug_left_stick_imp );

      // 右デバッグスティック
      CTR_DebugStick* debug_right_stick_imp = 
        GFL_NEW( heap ) CTR_DebugStick( 
            CTR_DebugStick::STICK_ID_RIGHT, &m_debug_pad_reader, &m_debug_pad_status );
      this->AddStick( heap, STICK_DEBUGPAD_R, debug_right_stick_imp );

      // デバッグボタン
      CTR_DebugButton* debug_button_imp = 
        GFL_NEW( heap ) CTR_DebugButton( &m_debug_pad_reader, &m_debug_pad_status ); 
      this->AddButton( heap, BUTTON_DEBUGPAD, debug_button_imp );
#endif
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    CTR_DeviceManager::~CTR_DeviceManager()
    {
      this->RemoveCrossKey( CROSSKEY_STANDARD );
      this->RemoveStick( STICK_STANDARD );
      this->RemoveButton( BUTTON_STANDARD );
      this->RemoveTouchPanel( TOUCHPANEL_STANDARD );
      this->RemoveAccelerometer( ACCELEROMETER_STANDARD );
#ifdef GFL2_UI_USE_GYROSCOPE
      this->RemoveGyroscope( GYROSCOPE_STANDARD );
#endif  // GFL2_UI_USE_GYROSCOPE
#ifdef GFL2_UI_USE_DEBUG_PAD
      this->RemoveCrossKey( CROSSKEY_DEBUGPAD );
      this->RemoveButton( BUTTON_DEBUGPAD );
      this->RemoveStick( STICK_DEBUGPAD_L );
      this->RemoveStick( STICK_DEBUGPAD_R );
#endif
      this->DeleteAccelerometerReaderAndStatus();
#ifdef GFL2_UI_USE_GYROSCOPE
      this->DeleteGyroscopeReaderAndStatus();
#endif  // GFL2_UI_USE_GYROSCOPE
    }

    /**
     * @brief 加速度センサのコアオブジェクトを破棄する
     */
    void CTR_DeviceManager::DeleteAccelerometerReaderAndStatus( void )
    {
      if( m_accelerometer_reader ) {
        GFL_DELETE m_accelerometer_reader;
        m_accelerometer_reader = NULL;
      }

      if( m_accelerometer_status ) {
        GFL_DELETE m_accelerometer_status;
        m_accelerometer_status = NULL;
      }
    }

#ifdef GFL2_UI_USE_GYROSCOPE
    /**
     * @brief ジャイロセンサのコアオブジェクトを破棄する
     */
    void CTR_DeviceManager::DeleteGyroscopeReaderAndStatus( void )
    {
      if( m_gyroscope_reader ) {
        GFL_DELETE m_gyroscope_reader;
        m_gyroscope_reader = NULL;
      }

      if( m_gyroscope_status ) {
        GFL_DELETE m_gyroscope_status;
        m_gyroscope_status = NULL;
      }
    }
#endif  // GFL2_UI_USE_GYROSCOPE

    //-------------------------------------------------------------------------
    /**
     * @brief 加速度センサを起動する
     * @param heap      加速度センサの生成に使用するヒープ
     * @param deviceID  起動する加速度センサのID
     */
    //-------------------------------------------------------------------------
    void CTR_DeviceManager::BootAccelerometer( gfl2::heap::HeapBase* heap, DeviceId deviceID )
    {
      this->CreateAccelerometerReaderAndStatus( heap ); 
      if( this->GetAccelerometer( deviceID ) == NULL ) {
        CTR_Accelerometer* imple = GFL_NEW( heap ) 
          CTR_Accelerometer( m_accelerometer_reader, m_accelerometer_status );
        this->AddAccelerometer( heap, deviceID, imple );
      }
    }

    /**
     * @brief 加速度センサのコアオブジェクトを生成する
     * @param heap  インスタンスの生成に使用するヒープ
     */
    void CTR_DeviceManager::CreateAccelerometerReaderAndStatus( gfl2::heap::HeapBase* heap )
    {
      if( m_accelerometer_reader == NULL ) {
        m_accelerometer_reader = GFL_NEW( heap ) nn::hid::AccelerometerReader();
      } 
      if( m_accelerometer_status == NULL ) {
        m_accelerometer_status = GFL_NEW( heap ) nn::hid::AccelerometerStatus();
      }
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 加速度センサを停止する
     * @param deviceID  停止する加速度センサのID
     */
    //-------------------------------------------------------------------------
    void CTR_DeviceManager::StopAccelerometer( DeviceId deviceID )
    {
      this->RemoveAccelerometer( deviceID ); 
      if( this->GetAccelerometerNum() == 0 ) {
        this->DeleteAccelerometerReaderAndStatus();
      }
    }

#ifdef GFL2_UI_USE_GYROSCOPE
    //-------------------------------------------------------------------------
    /**
     * @brief ジャイロセンサを起動する
     * @param heap      ジャイロセンサの生成に使用するヒープ
     * @param deviceID  起動するジャイロセンサのID
     */
    //-------------------------------------------------------------------------
    void CTR_DeviceManager::BootGyroscope( gfl2::heap::HeapBase* heap, DeviceId deviceID )
    {
      this->CreateGyroscopeReaderAndStatus( heap ); 
      if( this->GetGyroscope( deviceID ) == NULL ) {
        CTR_Gyroscope* imple = GFL_NEW( heap ) 
          CTR_Gyroscope( m_gyroscope_reader, m_gyroscope_status );
        this->AddGyroscope( heap, deviceID, imple );
      }
    }

    /**
     * @brief ジャイロセンサのコアオブジェクトを生成する
     * @param heap  インスタンスの生成に使用するヒープ
     */
    void CTR_DeviceManager::CreateGyroscopeReaderAndStatus( gfl2::heap::HeapBase* heap )
    {
      if( m_gyroscope_reader == NULL ) {
        m_gyroscope_reader = GFL_NEW( heap ) nn::hid::GyroscopeReader();
      } 
      if( m_gyroscope_status == NULL ) {
        m_gyroscope_status = GFL_NEW( heap ) nn::hid::GyroscopeStatus();
      }
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief ジャイロセンサを停止する
     * @param deviceID  停止するジャイロセンサのID
     */
    //-------------------------------------------------------------------------
    void CTR_DeviceManager::StopGyroscope( DeviceId deviceID )
    {
      this->RemoveGyroscope( deviceID ); 
      if( this->GetGyroscopeNum() == 0 ) {
        this->DeleteGyroscopeReaderAndStatus();
      }
    }
#endif  // GFL2_UI_USE_GYROSCOPE

    //-------------------------------------------------------------------------
    /**
     * @brief メイン処理
     */
    //-------------------------------------------------------------------------
    void CTR_DeviceManager::Main( void )
    {
      // サンプリングバッファから入力を取得
      s32 outputNum = 0;
      m_pad_reader.Read( m_pad_status_buffer , &outputNum , nn::hid::PadReader::MAX_READ_NUM );
      m_pad_reader.ReadLatest( &m_pad_status );//最新を設定
      if( outputNum > 0 )
      {//サンプリングデータあり
        
        for( s32 index = 0 ; index < outputNum ; ++index )
        {
          m_pad_status.hold |= m_pad_status_buffer[index].hold;//holdのみマージ（現状、holdしか使用されていないので）
        }
      }

      //サンプリングバッファから入力を取得
      nn::hid::TouchPanelStatus oldTouchStatus = m_touchpanel_status;
      outputNum = 0;
      m_touchpanel_reader.Read( m_touch_status_buffer , &outputNum , nn::hid::PadReader::MAX_READ_NUM );
      m_touchpanel_reader.ReadLatest( &m_touchpanel_status );//最新を設定
      if( m_touchpanel_status.touch == true && m_touchpanel_status.x == 0 && m_touchpanel_status.y == 0 )
      {//無効なデータ
        m_touchpanel_status.touch = false;
      }

      if( outputNum > 0 )
      {//サンプリングデータあり
        for( s32 index = 0 ; index < outputNum ; ++index )
        {
          if( oldTouchStatus.touch == true )
          {//前のフレームでタッチしている
            if( m_touch_status_buffer[index].x == 0 && m_touch_status_buffer[index].y == 0 && m_touch_status_buffer[index].touch == false )
            {
              m_touchpanel_status = m_touch_status_buffer[index];
            }
          }
          else
          {//前のフレームjでタッチしていない
            if( m_touch_status_buffer[index].x != 0 && m_touch_status_buffer[index].y != 0 && m_touch_status_buffer[index].touch == true )
            {
              m_touchpanel_status = m_touch_status_buffer[index];
            }
          }
        }
      }

#ifdef GFL2_UI_USE_DEBUG_PAD
      m_debug_pad_reader.ReadLatest( &m_debug_pad_status );
#endif

      // 取得した入力データを各デバイス管理オブジェクトに反映させる
      this->UpdateAllDevices();
    }


  }  //namespace ui
}  //namespace gfl2
#endif