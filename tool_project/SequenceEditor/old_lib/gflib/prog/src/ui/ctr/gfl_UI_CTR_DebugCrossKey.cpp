//=============================================================================
/**
 * @file   gfl_UI_CTR_DebugCrossKey.cpp
 * @brief  3DSのデバッグパッドの十字キー入力を検出するクラス
 * @author obata_toshihiro
 * @date   2011.02.09
 */
//=============================================================================
#ifdef GFL_UI_USE_DEBUG_PAD
#include <nn.h>
#include "ui/gfl_UI_Types.h"
#include "ui/gfl_UI_VectorDevice.h"
#include "./gfl_UI_CTR_DebugCrossKey.h"

namespace gfl {
  namespace ui {


    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     *
     * @param p_reader  パッド入力の読み取りを行うインスタンス
     * @param p_status  読み取った入力を取得するバッファ
     */
    //-------------------------------------------------------------------------
    CTR_DebugCrossKey::CTR_DebugCrossKey( 
        nn::hid::DebugPadReader* p_reader, nn::hid::DebugPadStatus* p_status ) :
      DeviceImplementerForVectorDevice(),
      m_pPadReader( p_reader ),
      m_pPadStatus( p_status )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    CTR_DebugCrossKey::~CTR_DebugCrossKey()
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief メイン処理
     */
    //-------------------------------------------------------------------------
    void CTR_DebugCrossKey::Main( void )
    {
      // 何もしない
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 入力データを取得する
     *
     * @param  p_buffer 取得したデータの格納先 
     */
    //-------------------------------------------------------------------------
    void CTR_DebugCrossKey::GetDetectiveData( void* p_buffer ) const
    {
      VectorDevice::DetectiveData* data = reinterpret_cast<VectorDevice::DetectiveData*>(p_buffer);

      // 左右キー
      if( m_pPadStatus->hold & nn::hid::DEBUG_PAD_BUTTON_LEFT ) {
        data->x = -1.0f;
      }
      else if( m_pPadStatus->hold & nn::hid::DEBUG_PAD_BUTTON_RIGHT ) {
        data->x = 1.0f;
      }
      else {
        data->x = 0.0f;
      }

      // 上下キー
      if( m_pPadStatus->hold & nn::hid::DEBUG_PAD_BUTTON_DOWN ) {
        data->y =  -1.0f;
      }
      else if( m_pPadStatus->hold & nn::hid::DEBUG_PAD_BUTTON_UP ) {
        data->y =  1.0f;
      }
      else {
        data->y = 0.0f;
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief アナログスティックのクランプモードを取得する
     */
    //-------------------------------------------------------------------------
    VectorDevice::StickClampMode CTR_DebugCrossKey::GetStickClampMode( void ) const
    {
      return VectorDevice::STICK_CLAMP_MODE_CROSS;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief アナログスティックのクランプモードを設定する
     */
    //-------------------------------------------------------------------------
    void CTR_DebugCrossKey::SetStickClampMode( VectorDevice::StickClampMode clamp_mode )
    {
      GFL_UNUSED(clamp_mode);
    }

    //-------------------------------------------------------------------------
    /**
     * @brief アナログスティックのクランプパラメータを取得する
     *
     * @param[out] min  最小クランプ値の格納先
     * @param[out] max  最大クランプ値の格納先
     */
    //-------------------------------------------------------------------------
    void CTR_DebugCrossKey::GetStickClampParam( s16* min, s16* max ) const
    {
      GFL_UNUSED(min);
      GFL_UNUSED(max);
    }

    //-------------------------------------------------------------------------
    /**
     * @brief アナログスティックのクランプパラメータを設定する
     *
     * @param[in] min  最小クランプ値
     * @param[in] max  最大クランプ値
     */
    //-------------------------------------------------------------------------
    void CTR_DebugCrossKey::SetStickClampParam( s16 min, s16 max )
    {
      GFL_UNUSED(min);
      GFL_UNUSED(max);
    }


  } //namespace ui
} //namespace gfl 

#endif //GFL_UI_USE_DEBUG_PAD
