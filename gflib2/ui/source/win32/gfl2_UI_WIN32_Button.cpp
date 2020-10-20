//=============================================================================
/**
 * @file   gfl2_UI_WIN32_Button.cpp
 * @brief  3DSのボタン入力を検出するクラス
 * @author obata_toshihiro
 * @date   2010.10.21
 */
//=============================================================================
#ifdef  GF_PLATFORM_WIN32

#include <system/include/InputDevice/gfl2_Pad.h>

#include <ui/include/gfl2_UI_Types.h>
#include "ui/include/gfl2_UI_Button.h"
#include "./gfl2_UI_WIN32_Button.h"

namespace gfl2 {
  namespace ui {


    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param[in] reader  ボタン入力の読み取りを行うインスタンス
     * @param[in] status  読み取ったボタン入力を取得するバッファ
     */
    //-------------------------------------------------------------------------
    WIN32_Button::WIN32_Button() :
      DeviceImplementer(),
      m_isStickEmulationEnable( false )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    WIN32_Button::~WIN32_Button()
    {
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief メイン処理
     */
    //-------------------------------------------------------------------------
    void WIN32_Button::Main( void )
    {
      // 何もしない
    }

    //-------------------------------------------------------------------------
    /**
     * @brief アナログスティックによるエミュレーションを有効にする
     */
    //-------------------------------------------------------------------------
    void WIN32_Button::SetStickEmulationEnable( void )
    {
      m_isStickEmulationEnable = true;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief アナログスティックによるエミュレーションを無効にする
     */
    //-------------------------------------------------------------------------
    void WIN32_Button::SetStickEmulationDisable( void )
    {
      m_isStickEmulationEnable = false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 入力データを取得する
     *
     * @param[out] buffer  取得したデータの格納先 
     */
    //-------------------------------------------------------------------------
    void WIN32_Button::GetDetectiveData( void* buffer ) const
    {
      Button::DetectiveData* data = reinterpret_cast<Button::DetectiveData*>( buffer );

      data->hold = 0;
      data->hold |= gfl2::system::InputDevice::Pad::IsPress( 0, gfl2::system::InputDevice::Button::Left ) ? BUTTON_LEFT : 0;
      data->hold |= gfl2::system::InputDevice::Pad::IsPress( 0, gfl2::system::InputDevice::Button::Right ) ? BUTTON_RIGHT : 0;
      data->hold |= gfl2::system::InputDevice::Pad::IsPress( 0, gfl2::system::InputDevice::Button::Up ) ? BUTTON_UP : 0;
      data->hold |= gfl2::system::InputDevice::Pad::IsPress( 0, gfl2::system::InputDevice::Button::Down ) ? BUTTON_DOWN : 0; 
      data->hold |= gfl2::system::InputDevice::Pad::IsPress( 0, gfl2::system::InputDevice::Button::A ) ? BUTTON_A : 0;
      data->hold |= gfl2::system::InputDevice::Pad::IsPress( 0, gfl2::system::InputDevice::Button::B ) ? BUTTON_B : 0;
      data->hold |= gfl2::system::InputDevice::Pad::IsPress( 0, gfl2::system::InputDevice::Button::X ) ? BUTTON_X : 0;
      data->hold |= gfl2::system::InputDevice::Pad::IsPress( 0, gfl2::system::InputDevice::Button::Y ) ? BUTTON_Y : 0;
      data->hold |= gfl2::system::InputDevice::Pad::IsPress( 0, gfl2::system::InputDevice::Button::L1 ) ? BUTTON_L : 0;
      data->hold |= gfl2::system::InputDevice::Pad::IsPress( 0, gfl2::system::InputDevice::Button::R1 ) ? BUTTON_R : 0;
      data->hold |= gfl2::system::InputDevice::Pad::IsPress( 0, gfl2::system::InputDevice::Button::Start ) ? BUTTON_START : 0;
      data->hold |= gfl2::system::InputDevice::Pad::IsPress( 0, gfl2::system::InputDevice::Button::Select ) ? BUTTON_SELECT : 0;
      data->hold |= gfl2::system::InputDevice::Pad::IsPress( 0, gfl2::system::InputDevice::Button::L2 ) ? BUTTON_ZL : 0; 
      data->hold |= gfl2::system::InputDevice::Pad::IsPress( 0, gfl2::system::InputDevice::Button::R2 ) ? BUTTON_ZR : 0;

      if( m_isStickEmulationEnable ) {
        const gfl2::system::InputDevice::StickPos& pos = gfl2::system::InputDevice::Pad::GetStick( 0, gfl2::system::InputDevice::Stick::Left );


        data->hold |= pos.x < -0.5f ? BUTTON_LEFT : 0;
        data->hold |= pos.x >  0.5f ? BUTTON_RIGHT : 0;
        data->hold |= pos.y < -0.5f ? BUTTON_UP : 0;
        data->hold |= pos.y >  0.5f ? BUTTON_DOWN : 0; 
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 入力データのサイズを取得する
     */
    //-------------------------------------------------------------------------
    u32 WIN32_Button::GetDetectiveDataSize( void ) const
    {
      return sizeof(Button::DetectiveData);
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 実効データのサイズを取得する
     */
    //-------------------------------------------------------------------------
    u32 WIN32_Button::GetEffectiveDataSize( void ) const
    {
      return sizeof(Button::EffectiveData);
    }


  } //namespace ui
} //namespace gfl2 
#endif