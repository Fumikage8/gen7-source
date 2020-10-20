﻿//=============================================================================
/**
 * @file   gfl2_UI_DeviceImplementerForVectorDevice.h
 * @brief  ベクター型デバイスの入力を検出するクラスのインターフェース
 * @author obata_toshihiro
 * @date   2011.04.04
 */
//=============================================================================
#include "ui/include/gfl2_UI_VectorDevice.h"
#include "./gfl2_UI_DeviceImplementer.h"
#include "./gfl2_UI_DeviceImplementerForVectorDevice.h"

namespace gfl2 {
  namespace ui {


    //-------------------------------------------------------------------------
    /**
     * @brief デフォルトコンストラクタ
     */
    //------------------------------------------------------------------------- 
    DeviceImplementerForVectorDevice::DeviceImplementerForVectorDevice( void ) : 
      DeviceImplementer()
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    DeviceImplementerForVectorDevice::~DeviceImplementerForVectorDevice()
    {
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 入力データのサイズを取得する
     */
    //-------------------------------------------------------------------------
    u32 DeviceImplementerForVectorDevice::GetDetectiveDataSize( void ) const
    {
      return sizeof(VectorDevice::DetectiveData);
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 実効データのサイズを取得する
     */
    //-------------------------------------------------------------------------
    u32 DeviceImplementerForVectorDevice::GetEffectiveDataSize( void ) const
    {
      return sizeof( VectorDevice::EffectiveData );
    }


  } //namespace ui
} //namespace gfl2
