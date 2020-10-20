//=============================================================================
/**
 * @file   gfl_UI_DeviceImplementerForAccelerometer.cpp
 * @brief  加速度センサ入力の検出クラス専用インターフェース
 * @author obata_toshihiro
 * @date   2011.03.25
 */
//=============================================================================
#include "ui/gfl_UI_Accelerometer.h"
#include "./gfl_UI_DeviceImplementerForAccelerometer.h"


namespace gfl {
  namespace ui {


    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     */
    //-------------------------------------------------------------------------
    DeviceImplementerForAccelerometer::DeviceImplementerForAccelerometer( void ) : 
      DeviceImplementer()
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    DeviceImplementerForAccelerometer::~DeviceImplementerForAccelerometer( void )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 検出データのサイズを取得する
     */
    //-------------------------------------------------------------------------
    u32 DeviceImplementerForAccelerometer::GetDetectiveDataSize( void ) const
    {
      return sizeof(Accelerometer::DetectiveData);
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 実効データのサイズを取得する
     */
    //-------------------------------------------------------------------------
    u32 DeviceImplementerForAccelerometer::GetEffectiveDataSize( void ) const
    {
      return sizeof( Accelerometer::EffectiveData );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief メイン処理
     */
    //-------------------------------------------------------------------------
    void DeviceImplementerForAccelerometer::Main( void )
    {
      // do nothing
    }


  } //namespace ui
} //namespace gfl
