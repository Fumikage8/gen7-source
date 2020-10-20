//=============================================================================
/**
 * @file   gfl2_UI_WIN32_DeviceManager.cpp
 * @brief  3DSのデバイス管理
 * @author obata_toshihiro
 * @date   2010.10.13
 */
//=============================================================================
#ifdef  GF_PLATFORM_WIN32
#include <heap/include/gfl2_Heap.h>


#include "ui/include/gfl2_UI_VectorDevice.h"
#include "ui/include/gfl2_UI_Button.h"
#include "ui/include/gfl2_UI_TouchPanel.h"
#include "./gfl2_UI_WIN32_CrossKey.h"
#include "./gfl2_UI_WIN32_Stick.h"
#include "./gfl2_UI_WIN32_Button.h"
#include "./gfl2_UI_WIN32_TouchPanel.h"
#include "ui/include/win32/gfl2_UI_WIN32_DeviceManager.h"


#include <system/include/InputDevice/gfl2_Pad.h>
#include <system/include/InputDevice/gfl2_Mouse.h>
#include <system/include/InputDevice/gfl2_KeyBoard.h>

namespace gfl2 {
  namespace ui {


    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param heap  デバイスの生成に使用するヒープ
     * @param hWnd  ウィンドウハンドル
     */
    //-------------------------------------------------------------------------
    WIN32_DeviceManager::WIN32_DeviceManager( gfl2::heap::HeapBase* heap, void* hWnd ) :
      DeviceManager()
    {
      this->CreateInitialDevices( heap, hWnd );
    }

    /**
     * @brief 初期状態で必要なデバイスを作成する
     * @param heap  各種インスタンスの生成に使用するヒープ
     * @param hWnd  ウィンドウハンドル
     */
    void WIN32_DeviceManager::CreateInitialDevices( gfl2::heap::HeapBase* heap, void* hWnd )
    {
      //システム初期化
      gfl2::system::InputDevice::Pad::Initialize( heap );
      gfl2::system::InputDevice::Mouse::Initialize( heap );
      gfl2::system::InputDevice::KeyBoard::Initialize( heap, static_cast<HWND>( hWnd ) );

      // 十字キー
      WIN32_CrossKey* crosskey_imp = 
        GFL_NEW( heap ) WIN32_CrossKey(); 
      crosskey_imp->SetStickEmulationDisable();
      this->AddCrossKey( heap, CROSSKEY_STANDARD, crosskey_imp );
      
      // スティックエミュ有りの十字キー
      WIN32_CrossKey* crosskey_emu_imp = 
        GFL_NEW( heap ) WIN32_CrossKey(); 
      crosskey_emu_imp->SetStickEmulationEnable();
      this->AddCrossKey( heap, CROSSKEY_STICK_EMU, crosskey_emu_imp );

      // ボタン
      WIN32_Button* button_imp = 
        GFL_NEW( heap ) WIN32_Button();
      button_imp->SetStickEmulationDisable();
      this->AddButton( heap, BUTTON_STANDARD, button_imp );

      // スティックエミュ有りのボタン
      WIN32_Button* button_emu_imp = 
        GFL_NEW( heap ) WIN32_Button();
      button_emu_imp->SetStickEmulationEnable();
      this->AddButton( heap, BUTTON_STICK_EMU, button_emu_imp );

      // スティック
      WIN32_Stick* stick_imp = 
        GFL_NEW( heap ) WIN32_Stick();
      this->AddStick( heap, STICK_STANDARD, stick_imp );

      // タッチパネル
      WIN32_TouchPanel* tp_imp = 
        GFL_NEW( heap ) WIN32_TouchPanel(); 
      this->AddTouchPanel( heap, TOUCHPANEL_STANDARD, tp_imp ); 
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    WIN32_DeviceManager::~WIN32_DeviceManager()
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

      gfl2::system::InputDevice::Pad::Terminate();
      gfl2::system::InputDevice::Mouse::Terminate();
      gfl2::system::InputDevice::KeyBoard::Terminate();
    }

    /**
     * @brief 加速度センサのコアオブジェクトを破棄する
     */
    void WIN32_DeviceManager::DeleteAccelerometerReaderAndStatus( void )
    {
      //未実装
    }

#ifdef GFL2_UI_USE_GYROSCOPE
    /**
     * @brief ジャイロセンサのコアオブジェクトを破棄する
     */
    void WIN32_DeviceManager::DeleteGyroscopeReaderAndStatus( void )
    {
      //未実装
    }
#endif  // GFL2_UI_USE_GYROSCOPE

    //-------------------------------------------------------------------------
    /**
     * @brief 加速度センサを起動する
     * @param heap      加速度センサの生成に使用するヒープ
     * @param deviceID  起動する加速度センサのID
     */
    //-------------------------------------------------------------------------
    void WIN32_DeviceManager::BootAccelerometer( gfl2::heap::HeapBase* heap, DeviceId deviceID )
    {
      //未実装
    }

    /**
     * @brief 加速度センサのコアオブジェクトを生成する
     * @param heap  インスタンスの生成に使用するヒープ
     */
    void WIN32_DeviceManager::CreateAccelerometerReaderAndStatus( gfl2::heap::HeapBase* heap )
    {
      //未実装
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 加速度センサを停止する
     * @param deviceID  停止する加速度センサのID
     */
    //-------------------------------------------------------------------------
    void WIN32_DeviceManager::StopAccelerometer( DeviceId deviceID )
    {
      //未実装
    }

#ifdef GFL2_UI_USE_GYROSCOPE
    //-------------------------------------------------------------------------
    /**
     * @brief ジャイロセンサを起動する
     * @param heap      ジャイロセンサの生成に使用するヒープ
     * @param deviceID  起動するジャイロセンサのID
     */
    //-------------------------------------------------------------------------
    void WIN32_DeviceManager::BootGyroscope( gfl2::heap::HeapBase* heap, DeviceId deviceID )
    {
      //未実装
    }

    /**
     * @brief ジャイロセンサのコアオブジェクトを生成する
     * @param heap  インスタンスの生成に使用するヒープ
     */
    void WIN32_DeviceManager::CreateGyroscopeReaderAndStatus( gfl2::heap::HeapBase* heap )
    {
      //未実装
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief ジャイロセンサを停止する
     * @param deviceID  停止するジャイロセンサのID
     */
    //-------------------------------------------------------------------------
    void WIN32_DeviceManager::StopGyroscope( DeviceId deviceID )
    {
      //未実装
    }
#endif  // GFL2_UI_USE_GYROSCOPE

    //-------------------------------------------------------------------------
    /**
     * @brief メイン処理
     */
    //-------------------------------------------------------------------------
    void WIN32_DeviceManager::Main( void )
    {
      gfl2::system::InputDevice::Pad::Update();
      gfl2::system::InputDevice::Mouse::Update();
      gfl2::system::InputDevice::KeyBoard::Update();

      // 取得した入力データを各デバイス管理オブジェクトに反映させる
      this->UpdateAllDevices();
    }


  }  //namespace ui
}  //namespace gfl2
#endif