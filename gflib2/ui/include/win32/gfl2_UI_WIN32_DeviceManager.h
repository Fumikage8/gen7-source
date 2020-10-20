//=============================================================================
/**
 * @file gfl2_UI_WIN32_DeviceManager.h
 * @brief 3DS のデバイス管理
 * @author obata_toshihiro
 * @date 2010.10.13
 */
//=============================================================================
#ifdef  GF_PLATFORM_WIN32
#ifndef __gfl2_UI_WIN32_DEVICEMANAGER_H__
#define __gfl2_UI_WIN32_DEVICEMANAGER_H__
#pragma once
#include <macro/include/gfl2_Macros.h>
#include <ui/include/gfl2_UI_DeviceManager.h>

namespace gfl2 {
  namespace ui {


    class WIN32_DeviceManager : public DeviceManager 
    {
      GFL_FORBID_COPY_AND_ASSIGN( WIN32_DeviceManager ); 

      public: 





      //-----------------------------------------------------------------------
      // コンストラクタ・デストラクタ・基本動作
      //-----------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       * @param heap   デバイスの生成に使用するヒープ
       * @param hWnd  ウィンドウハンドル
       */
      WIN32_DeviceManager( gfl2::heap::HeapBase* heap, void* hWnd );

      /**
       * @brief デストラクタ
       */
      virtual ~WIN32_DeviceManager(); 

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
	  virtual void BootAccelerometer( gfl2::heap::HeapBase* heap, DeviceId deviceID );

      /**
       * @brief 加速度センサを停止する
       * @param deviceID  停止する加速度センサのID
       */
	  virtual void StopAccelerometer( DeviceId deviceID );


#ifdef GFL2_UI_USE_GYROSCOPE
      //-----------------------------------------------------------------------
      // ジャイロセンサ
      //-----------------------------------------------------------------------
      /**
       * @brief ジャイロセンサを起動する
       * @param heap      ジャイロセンサの生成に使用するヒープ
       * @param deviceID  起動するジャイロセンサのID
       */
	  virtual void BootGyroscope( gfl2::heap::HeapBase* heap, DeviceId deviceID );

      /**
       * @brief ジャイロセンサを停止する
       * @param deviceID  停止するジャイロセンサのID
       */
	  virtual void StopGyroscope( DeviceId deviceID );
#endif  // GFL2_UI_USE_GYROSCOPE


      /**
       * @brief 初期状態で必要なデバイスを作成する
       * @param heap  各種インスタンスの生成に使用するヒープ
       * @param hWnd  ウィンドウハンドル
       */
      virtual void CreateInitialDevices( gfl2::heap::HeapBase* heap, void* hWnd );

      /**
       * @brief 加速度センサのコアオブジェクトを生成する
       * @param heap  インスタンスの生成に使用するヒープ
       */
      void CreateAccelerometerReaderAndStatus( gfl2::heap::HeapBase* heap );

      /**
       * @brief 加速度センサのコアオブジェクトを破棄する
       */
      void DeleteAccelerometerReaderAndStatus( void );

#ifdef GFL2_UI_USE_GYROSCOPE
      /**
       * @brief ジャイロセンサのコアオブジェクトを生成する
       * @param heap  インスタンスの生成に使用するヒープ
       */
      void CreateGyroscopeReaderAndStatus( gfl2::heap::HeapBase* heap );

      /**
       * @brief ジャイロセンサのコアオブジェクトを破棄する
       */
      void DeleteGyroscopeReaderAndStatus( void );
#endif  // GFL2_UI_USE_GYROSCOPE
    };


  }  //namespace ui
}  //namespace gfl2


#endif //__gfl2_UI_WIN32_DEVICEMANAGER_H__
#endif