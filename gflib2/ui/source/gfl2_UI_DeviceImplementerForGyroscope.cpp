//=============================================================================
/**
 * @file   gfl2_UI_DeviceImplementerForGyroscope.cpp
 * @brief  ジャイロセンサ入力を検出するクラスのインターフェース
 * @author obata_toshihiro
 * @date   2011.03.31
 */
//=============================================================================
#ifdef GFL2_UI_USE_GYROSCOPE

#include "ui/include/gfl2_UI_Gyroscope.h"
#include "./gfl2_UI_DeviceImplementer.h"
#include "./gfl2_UI_DeviceImplementerForGyroscope.h"

namespace gfl2 {
  namespace ui {


    //-------------------------------------------------------------------------
    /**
     * @brief デフォルトコンストラクタ
     */
    //-------------------------------------------------------------------------
    DeviceImplementerForGyroscope::DeviceImplementerForGyroscope( void ) :
      DeviceImplementer()
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    DeviceImplementerForGyroscope::~DeviceImplementerForGyroscope()
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 入力データのサイズを取得する
     */
    //-------------------------------------------------------------------------
    u32 DeviceImplementerForGyroscope::GetDetectiveDataSize( void ) const
    {
      return sizeof(Gyroscope::DetectiveData);
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 実効データのサイズを取得する
     */
    //-------------------------------------------------------------------------
    u32 DeviceImplementerForGyroscope::GetEffectiveDataSize( void ) const
    {
      return sizeof( Gyroscope::EffectiveData );
    }


  } //namespace ui
} //namespace gfl2

#endif  // GFL2_UI_USE_GYROSCOPE
