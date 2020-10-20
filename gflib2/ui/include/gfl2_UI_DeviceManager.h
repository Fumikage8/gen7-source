//=============================================================================
/**
 * @file   gfl2_UI_DeviceManager.h
 * @brief  デバイス管理の抽象クラス
 * @author obata_toshihiro
 * @date   2010.10.13
 */
//=============================================================================
#ifndef __gfl2_UI_DEVICEMANAGER_H__
#define __gfl2_UI_DEVICEMANAGER_H__
#pragma once


#include <heap/include/gfl2_Heap.h>
#include <ui/include/gfl2_UI_Types.h>
#include <ui/include/gfl2_UI_Device.h>

namespace gfl2 {
  namespace ui { 


    // デバイスを特定する型
    typedef u8 DeviceId;   

    class DeviceImplementer;
    class DeviceImplementerForVectorDevice;
    class DeviceImplementerForAccelerometer;
#ifdef GFL2_UI_USE_GYROSCOPE
    class DeviceImplementerForGyroscope;
#endif  // GFL2_UI_USE_GYROSCOPE
    class VectorDevice;
    class Button;
    class TouchPanel;
    class Accelerometer;
#ifdef GFL2_UI_USE_GYROSCOPE
    class Gyroscope; 
#endif  // GFL2_UI_USE_GYROSCOPE

    class DeviceManager 
    {
      GFL_FORBID_COPY_AND_ASSIGN( DeviceManager );

      public: 

              /**
       * @brief 十字キーの種類
       */
      enum CROSSKEY_ID {
        CROSSKEY_STANDARD,   // 十字キー単体
        CROSSKEY_STICK_EMU,  // アナログスティックによるエミュレーションを含む十字キー
#ifdef GFL2_UI_USE_DEBUG_PAD
        CROSSKEY_DEBUGPAD,   // デバッグパッドの十字キー
#endif
        CROSSKEY_NUM,
      }; 

      /**
       * @brief アナログスティックの種類
       */
      enum STICK_ID {
        STICK_STANDARD,    // スティック単体
#ifdef GFL2_UI_USE_DEBUG_PAD
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
#ifdef GFL2_UI_USE_DEBUG_PAD
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


      // 各デバイスの最大数
      static const u8 MAX_CROSSKEY_NUM      = 3;
      static const u8 MAX_STICK_NUM         = 3;
      static const u8 MAX_BUTTON_NUM        = 3;
      static const u8 MAX_TOUCH_PANEL_NUM   = 1;
      static const u8 MAX_ACCELEROMETER_NUM = 1;
      static const u8 MAX_GYROSCOPE_NUM     = 1;


      //---------------------------------------------------------------------
      // コンストラクタ・デストラクタ・基本動作
      //--------------------------------------------------------------------- 
      /**
       * @brief コンストラクタ
       */
      DeviceManager( void );

      /**
       * @brief デストラクタ
       */
      virtual ~DeviceManager();

      /**
       * @brief メイン処理( 毎フレーム呼んでください )
       */
      virtual void Main( void ) = 0; 

      //--------------------------------------------------------------------- 
      // デバイスの取得
      //--------------------------------------------------------------------- 
      /**
       * @brief 十字キーを取得する
       * @param keyID  取得する十字キーのID
       */
      VectorDevice* GetCrossKey( DeviceId keyID ) const;

      /**
       * @brief アナログスティックを取得する
       * @param stickID  取得するアナログスティックのID
       */
      VectorDevice* GetStick( DeviceId stickID ) const;

      /**
       * @brief ボタンを取得する
       * @param buttonID  取得するボタンのID
       */
      Button* GetButton( DeviceId buttonID ) const;

      /**
       * @brief タッチパネルを取得する
       * @param tpID  取得するタッチパネルのID
       */
      TouchPanel* GetTouchPanel( DeviceId tpID ) const;

      /**
       * @brief 加速度センサを取得する
       * @param accelID  取得する加速度センサのID
       */
      Accelerometer* GetAccelerometer( DeviceId accelID ) const;

#ifdef GFL2_UI_USE_GYROSCOPE
      /**
       * @brief ジャイロセンサを取得する
       * @param gyroID  取得するジャイロセンサのID
       */
      Gyroscope* GetGyroscope( DeviceId gyroID ) const;
#endif  // GFL2_UI_USE_GYROSCOPE

      //---------------------------------------------------------------------
      // デバイスの一括操作
      //---------------------------------------------------------------------
      /**
       * @brief 全デバイスの稼働状態を変更する
       * @param isEnable  true:デバイスを稼働する, false:デバイスの挙動を停止する
       */
      void SetAllDeviceRunningEnable( bool isEnable );

      //---------------------------------------------------------------------
      // 加速度センサ
      //---------------------------------------------------------------------
      /**
       * @brief 加速度センサを起動する
       * @param heap     加速度センサの生成に使用するヒープ
       * @param accelID  起動する加速度センサのID
       */
      virtual void BootAccelerometer( gfl2::heap::HeapBase* heap, DeviceId accelID ) = 0;

      /**
       * @brief 加速度センサを停止する
       * @param accelID  停止する加速度センサのID
       */
      virtual void StopAccelerometer( DeviceId accelID ) = 0;

#ifdef GFL2_UI_USE_GYROSCOPE
      //---------------------------------------------------------------------
      // ジャイロセンサ
      //---------------------------------------------------------------------
      /**
       * @brief ジャイロセンサを起動する
       * @param heap    ジャイロセンサの生成に使用するヒープ
       * @param gyroID  起動するジャイロセンサのID
       */
      virtual void BootGyroscope( gfl2::heap::HeapBase* heap, DeviceId gyroID ) = 0;

      /**
       * @brief ジャイロセンサを停止する
       * @param gyroID  停止するジャイロセンサのID
       */
      virtual void StopGyroscope( DeviceId gyroID ) = 0; 
#endif  // GFL2_UI_USE_GYROSCOPE

      //--------------------------------------------------------------------- 
      // フレームレート
      //--------------------------------------------------------------------- 
      /**
       * @brief 全てのデバイスの動作フレームレートを設定する
       * @param rate  新たに設定するフレームレート
       */
      void SetFrameRateForAllDevice( Device::FrameRate rate ); 

      /**
       * @brief 全てのデバイスの入力更新タイミングを同期する
       */
      void SynchronizeInputUpdateForAllDevice( void );


#if GFL_DEBUG
      void SetDebugKeyDisable( bool is_disalbe );
#endif // GFL_DEBUG


      protected:

      // 保持しているデバイス・オブジェクトの数
      u8 m_crosskeyNum;          // 十字キー
      u8 m_stickNum;             // アナログスティック
      u8 m_buttonNum;            // ボタン
      u8 m_touchpanelNum;        // タッチパネル
      u8 m_accelerometerNum;     // 加速度センサ
      u8 m_gyroscopeNum;         // ジャイロセンサ

      // デバイス・オブジェクト
      VectorDevice* m_crosskey[ MAX_CROSSKEY_NUM ];             // 十字キー
      VectorDevice* m_stick[ MAX_STICK_NUM ];                   // アナログスティック
      Button* m_button[ MAX_BUTTON_NUM ];                       // ボタン
      TouchPanel* m_touchpanel[ MAX_TOUCH_PANEL_NUM ];          // タッチパネル
      Accelerometer* m_accelerometer[ MAX_ACCELEROMETER_NUM ];  // 加速度センサ
#ifdef GFL2_UI_USE_GYROSCOPE
      Gyroscope* m_gyroscope[ MAX_GYROSCOPE_NUM ];              // ジャイロセンサ
#endif  // GFL2_UI_USE_GYROSCOPE

#if 0
      // 継承クラスの内部関数でしか使用していないので、インターフェイスから削除しました。
      /**
       * @brief 初期状態で必要なデバイスを作成する
       * @param heap  各種デバイスの生成に使用するヒープ
       */
      virtual void CreateInitialDevices( gfl2::heap::HeapBase* heap ) = 0;
#endif

      /**
       * @brief デバイスを追加する
       */
      void AddCrossKey( gfl2::heap::HeapBase* heap, DeviceId keyID, DeviceImplementerForVectorDevice* implementer ); 
      void AddStick( gfl2::heap::HeapBase* heap, DeviceId stickID, DeviceImplementerForVectorDevice* implementer ); 
      void AddButton( gfl2::heap::HeapBase* heap, DeviceId buttonID, DeviceImplementer* implementer ); 
      void AddTouchPanel( gfl2::heap::HeapBase* heap, DeviceId tpID, DeviceImplementer* implementer ); 
      void AddAccelerometer( gfl2::heap::HeapBase* heap, DeviceId accelID, DeviceImplementerForAccelerometer* implementer ); 
#ifdef GFL2_UI_USE_GYROSCOPE
      void AddGyroscope( gfl2::heap::HeapBase* heap, DeviceId gyroID, DeviceImplementerForGyroscope* implementer );
#endif  // GFL2_UI_USE_GYROSCOPE

      /**
       * @brief デバイスを削除する
       */
      void RemoveCrossKey( DeviceId keyID );
      void RemoveStick( DeviceId stickID );
      void RemoveButton( DeviceId buttonID );
      void RemoveTouchPanel( DeviceId tpID );
      void RemoveAccelerometer( DeviceId accelID );
#ifdef GFL2_UI_USE_GYROSCOPE
      void RemoveGyroscope( DeviceId gyroID );
#endif  // GFL2_UI_USE_GYROSCOPE

      /**
       * @brief 全てのデバイスを更新する
       */
      void UpdateAllDevices( void );

      /**
       * @brief デバイスの数を取得する
       */
      u8 GetCrossKeyNum( void ) const { return m_crosskeyNum; }
      u8 GetStickNum( void ) const { return m_stickNum; }
      u8 GetButtonNum( void ) const { return m_buttonNum; }
      u8 GetTouchPanelNum( void ) const { return m_touchpanelNum; }
      u8 GetAccelerometerNum( void ) const { return m_accelerometerNum; }
#ifdef GFL2_UI_USE_GYROSCOPE
      u8 GetGyroscopeNum( void ) const { return m_gyroscopeNum; }
#endif  // GFL2_UI_USE_GYROSCOPE



      private:
      void InitAllDevices( void );
      void DeleteAllDevices( void );
    };


  }  //namespace ui
}  //namespace gfl2

#endif //__gfl2_UI_DEVICEMANAGER_H__
