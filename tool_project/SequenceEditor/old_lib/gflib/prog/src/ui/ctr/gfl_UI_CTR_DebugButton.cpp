//=============================================================================
/**
 * @file   gfl_UI_CTR_DebugButton.cpp
 * @brief  3DSのデバッグパッドボタンの入力を検出するクラス
 * @author obata_toshihiro
 * @date   2011.02.09
 */
//=============================================================================
#ifdef GFL_UI_USE_DEBUG_PAD
#include <nn.h>
#include "ui/gfl_UI_Types.h"
#include "ui/gfl_UI_Button.h"
#include "./gfl_UI_CTR_DebugButton.h"

namespace gfl {
  namespace ui {


    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     *
     * @param p_reader  デバッグパッド入力の読み取りを実行するインスタンス
     * @param p_status  読み込んだ入力を取得するバッファ
     */
    //-------------------------------------------------------------------------
    CTR_DebugButton::CTR_DebugButton( 
        nn::hid::DebugPadReader* p_reader, nn::hid::DebugPadStatus* p_status ) :
      DeviceImplementer(),
      m_pPadReader( p_reader ),
      m_pPadStatus( p_status )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    CTR_DebugButton::~CTR_DebugButton()
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief メイン処理
     */
    //-------------------------------------------------------------------------
    void CTR_DebugButton::Main( void )
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
    void CTR_DebugButton::GetDetectiveData( void* p_buffer ) const
    {
      Button::DetectiveData* data = reinterpret_cast<Button::DetectiveData*>(p_buffer);

      data->hold = 0;
      data->hold |= (m_pPadStatus->hold & nn::hid::DEBUG_PAD_BUTTON_A) ? BUTTON_A : 0;
      data->hold |= (m_pPadStatus->hold & nn::hid::DEBUG_PAD_BUTTON_B) ? BUTTON_B : 0;
      data->hold |= (m_pPadStatus->hold & nn::hid::DEBUG_PAD_BUTTON_X) ? BUTTON_X : 0;
      data->hold |= (m_pPadStatus->hold & nn::hid::DEBUG_PAD_BUTTON_Y) ? BUTTON_Y : 0;
      data->hold |= (m_pPadStatus->hold & nn::hid::DEBUG_PAD_TRIGGER_L) ? BUTTON_L : 0;
      data->hold |= (m_pPadStatus->hold & nn::hid::DEBUG_PAD_TRIGGER_R) ? BUTTON_R : 0;
      data->hold |= (m_pPadStatus->hold & nn::hid::DEBUG_PAD_TRIGGER_ZL) ? BUTTON_ZL : 0;
      data->hold |= (m_pPadStatus->hold & nn::hid::DEBUG_PAD_TRIGGER_ZR) ? BUTTON_ZR : 0;
      data->hold |= (m_pPadStatus->hold & nn::hid::DEBUG_PAD_BUTTON_HOME) ? BUTTON_HOME : 0;
      data->hold |= (m_pPadStatus->hold & nn::hid::DEBUG_PAD_BUTTON_PLUS) ? BUTTON_PLUS : 0;
      data->hold |= (m_pPadStatus->hold & nn::hid::DEBUG_PAD_BUTTON_MINUS) ? BUTTON_MINUS : 0;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 入力データのサイズを取得する
     */
    //-------------------------------------------------------------------------
    u32 CTR_DebugButton::GetDetectiveDataSize( void ) const
    {
      return sizeof(Button::DetectiveData);
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 実効データのサイズを取得する
     */
    //-------------------------------------------------------------------------
    u32 CTR_DebugButton::GetEffectiveDataSize( void ) const
    {
      return sizeof(Button::EffectiveData);
    }


  } //namespace ui
} //namespace gfl 

#endif //GFL_UI_USE_DEBUG_PAD
