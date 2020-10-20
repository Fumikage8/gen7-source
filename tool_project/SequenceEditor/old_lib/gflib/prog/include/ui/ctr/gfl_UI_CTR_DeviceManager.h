//=============================================================================
/**
 * @file gfl_UI_CTR_DeviceManager.h
 * @brief 3DS のデバイス管理
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
       * @brief 十字キーの種類
       */
      enum CROSSKEY_ID {
        CROSSKEY_STANDARD,   // 十字キー単体
        CROSSKEY_STICK_EMU,  // アナログスティックによるエミュレーションを含む十字キー
#ifdef GFL_UI_USE_DEBUG_PAD
        CROSSKEY_DEBUGPAD,   // デバッグパッドの十字キー
#endif
        CROSSKEY_NUM,
      }; 

      /**
       * @brief アナログスティックの種類
       */
      enum STICK_ID {
        STICK_STANDARD,    // スティック単体
#ifdef GFL_UI_USE_DEBUG_PAD
        STICK_DEBUGPAD_L,  // デバッグパッドの左スティック
        STICK_DEBUGPAD_R,  // デバッグパッドの右スティック
#endif
        STICK_NUM,
      }; 

      /**
       * @brief ボタンの種類
       */
      enum BUTTON_ID {
        BUTTON_STANDARD,    // ボタン単体
        BUTTON_STICK_EMU,   // アナログスティックによる十字ボタンエミュレーションを含む十字キー
#ifdef GFL_UI_USE_DEBUG_PAD
        BUTTON_DEBUGPAD,    // デバッグパッドのボタン
#endif
        BUTTON_NUM,
      }; 

      /**
       * @brief タッチパネルの種類
       */
      enum TOUCHPANEL_ID {
        TOUCHPANEL_STANDARD,  // 通常のタッチパネル
        TOUCHPANEL_NUM,
      }; 

      /**
       * @brief 加速度センサの種類
       */
      enum ACCELEROMETER_ID {
        ACCELEROMETER_STANDARD,  // 通常の加速度センサ
        ACCELEROMETER_NUM,
      }; 

      /**
       * @brief ジャイロセンサの種類
       */
      enum GYROSCOPE_ID {
        GYROSCOPE_STANDARD,  // 通常のジャイロセンサ
        GYROSCOPE_NUM,
      };




      //-----------------------------------------------------------------------
      // コンストラクタ・デストラクタ・基本動作
      //-----------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       * @param heap  デバイスの生成に使用するヒープ
       */
      CTR_DeviceManager( gfl::heap::HeapBase* heap );

      /**
       * @brief デストラクタ
       */
      virtual ~CTR_DeviceManager(); 

      /**
       * @brief メイン処理
       */
      virtual void Main( void );


      //-----------------------------------------------------------------------
      // 加速度センサ
      //-----------------------------------------------------------------------
      /**
       * @brief 加速度センサを起動する
       * @param heap      加速度センサの生成に使用するヒープ
       * @param deviceID  起動する加速度センサのID
       */
      virtual void BootAccelerometer( gfl::heap::HeapBase* heap, DeviceId deviceID );

      /**
       * @brief 加速度センサを停止する
       * @param deviceID  停止する加速度センサのID
       */
      virtual void StopAccelerometer( DeviceId deviceID );


#ifdef GFL_UI_USE_GYROSCOPE
      //-----------------------------------------------------------------------
      // ジャイロセンサ
      //-----------------------------------------------------------------------
      /**
       * @brief ジャイロセンサを起動する
       * @param heap      ジャイロセンサの生成に使用するヒープ
       * @param deviceID  起動するジャイロセンサのID
       */
      virtual void BootGyroscope( gfl::heap::HeapBase* heap, DeviceId deviceID );

      /**
       * @brief ジャイロセンサを停止する
       * @param deviceID  停止するジャイロセンサのID
       */
      virtual void StopGyroscope( DeviceId deviceID ); 
#endif  // GFL_UI_USE_GYROSCOPE


      //-----------------------------------------------------------------------
      // デバッグパッド
      //----------------------------------------------------------------------- 
      /**
       * デバッグパッド取得
       * @return パッドステータス
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
       * @brief 初期状態で必要なデバイスを作成する
       * @param heap  各種インスタンスの生成に使用するヒープ
       */
      virtual void CreateInitialDevices( gfl::heap::HeapBase* heap );

      /**
       * @brief 加速度センサのコアオブジェクトを生成する
       * @param heap  インスタンスの生成に使用するヒープ
       */
      void CreateAccelerometerReaderAndStatus( gfl::heap::HeapBase* heap );

      /**
       * @brief 加速度センサのコアオブジェクトを破棄する
       */
      void DeleteAccelerometerReaderAndStatus( void );

#ifdef GFL_UI_USE_GYROSCOPE
      /**
       * @brief ジャイロセンサのコアオブジェクトを生成する
       * @param heap  インスタンスの生成に使用するヒープ
       */
      void CreateGyroscopeReaderAndStatus( gfl::heap::HeapBase* heap );

      /**
       * @brief ジャイロセンサのコアオブジェクトを破棄する
       */
      void DeleteGyroscopeReaderAndStatus( void );
#endif  // GFL_UI_USE_GYROSCOPE
    };


  }  //namespace ui
}  //namespace gfl


#endif //__GFL_UI_CTR_DEVICEMANAGER_H__
