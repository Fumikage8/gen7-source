//=============================================================================
/**
 * @file   gfl2_UI_WIN32_Stick.cpp
 * @brief  3DSのアナログスティック入力を検出するクラス
 * @author obata_toshihiro
 * @date   2010.10.21
 */
//=============================================================================
#ifdef  GF_PLATFORM_WIN32
#include <debug/include/gfl2_Assert.h>
#include <system/include/InputDevice/gfl2_Pad.h>

#include "ui/include/gfl2_UI_VectorDevice.h"
#include "./gfl2_UI_WIN32_Stick.h"

namespace gfl2 {
  namespace ui {



    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     *
     * @param p_reader  入力の読み取りを実行するインスタンス
     * @param p_status  読み取った入力を取得するバッファ
     */
    //-------------------------------------------------------------------------
    WIN32_Stick::WIN32_Stick() :
      DeviceImplementerForVectorDevice()
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    WIN32_Stick::~WIN32_Stick()
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief メイン処理
     */
    //-------------------------------------------------------------------------
    void WIN32_Stick::Main( void )
    {
      // 何もしない
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 入力データを取得する
     *
     * @param p_buffer  取得したデータの格納先 
     */
    //-------------------------------------------------------------------------
    void WIN32_Stick::GetDetectiveData( void* p_buffer ) const
    {
      VectorDevice::DetectiveData* data = reinterpret_cast<VectorDevice::DetectiveData*>(p_buffer);

      const gfl2::system::InputDevice::StickPos& pos = gfl2::system::InputDevice::Pad::GetStick( 0, gfl2::system::InputDevice::Stick::Left );

      data->x = pos.x;
      data->y = -pos.y;
    }

    //-------------------------------------------------------------------------------
    /**
     * @brief アナログスティックのクランプモードを取得する
     */
    //-------------------------------------------------------------------------------
    VectorDevice::StickClampMode WIN32_Stick::GetStickClampMode( void ) const
    {
      return VectorDevice::STICK_CLAMP_MODE_CIRCLE;
    }

    //-------------------------------------------------------------------------------
    /**
     * @brief アナログスティックのクランプモードを設定する
     */
    //-------------------------------------------------------------------------------
    void WIN32_Stick::SetStickClampMode( VectorDevice::StickClampMode clamp_mode )
    {
    }

    //-------------------------------------------------------------------------------
    /**
     * @brief アナログスティックのクランプパラメータを取得する
     *
     * @param[out] min  最小クランプ値の格納先
     * @param[out] max  最大クランプ値の格納先
     */
    //-------------------------------------------------------------------------------
    void WIN32_Stick::GetStickClampParam( s16* min, s16* max ) const
    {
    }

    //-------------------------------------------------------------------------------
    /**
     * @brief アナログスティックのクランプパラメータを設定する
     *
     * @param[in] min  最小クランプ値
     * @param[in] max  最大クランプ値
     */
    //-------------------------------------------------------------------------------
    void WIN32_Stick::SetStickClampParam( s16 min, s16 max )
    {
    }


  } //namespace ui
} //namespace gfl2 
#endif