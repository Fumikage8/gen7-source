//=============================================================================
/**
 * @file   gfl2_UI_CTR_DebugStick.cpp
 * @brief  3DSのデバッグパッドのアナログスティック入力を検出するクラス
 * @author obata_toshihiro
 * @date   2011.02.09
 */
//=============================================================================
#ifdef  GF_PLATFORM_CTR
#include <ui/include/gfl2_UI_Types.h>
#ifdef GFL2_UI_USE_DEBUG_PAD
#include <nn.h>

#include "ui/include/gfl2_UI_VectorDevice.h"
#include "./gfl2_UI_CTR_DebugStick.h"

namespace gfl2 {
  namespace ui {


    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     *
     * @param stick_id  どのスティックか
     * @param p_reader  入力の読み取りを実行するインスタンス
     * @param p_status  読み取った入力を取得するバッファ
     */
    //-------------------------------------------------------------------------
    CTR_DebugStick::CTR_DebugStick( 
        StickID stick_id,
        nn::hid::DebugPadReader* p_reader, nn::hid::DebugPadStatus* p_status ) :
      DeviceImplementerForVectorDevice(),
      m_stickId( stick_id ),
      m_pPadReader( p_reader ),
      m_pPadStatus( p_status )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    CTR_DebugStick::~CTR_DebugStick()
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief メイン処理
     */
    //-------------------------------------------------------------------------
    void CTR_DebugStick::Main( void )
    {
      // 何もしない
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 入力データを取得する
     *
     * @param void* p_buffer 取得したデータの格納先 
     */
    //-------------------------------------------------------------------------
    void CTR_DebugStick::GetDetectiveData( void* p_buffer ) const
    {
      VectorDevice::DetectiveData* data = reinterpret_cast<VectorDevice::DetectiveData*>(p_buffer);

      if( m_stickId == STICK_ID_LEFT )
      {
        data->x = m_pPadStatus->leftStickX;
        data->y = m_pPadStatus->leftStickY;
      }
      else if( m_stickId == STICK_ID_RIGHT )
      {
        data->x = m_pPadStatus->rightStickX;
        data->y = m_pPadStatus->rightStickY;
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief アナログスティックのクランプモードを取得する
     */
    //-------------------------------------------------------------------------
    VectorDevice::StickClampMode CTR_DebugStick::GetStickClampMode( void ) const
    {
      return VectorDevice::STICK_CLAMP_MODE_CIRCLE;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief アナログスティックのクランプモードを設定する
     */
    //-------------------------------------------------------------------------
    void CTR_DebugStick::SetStickClampMode( VectorDevice::StickClampMode clamp_mode )
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
    void CTR_DebugStick::GetStickClampParam( s16* min, s16* max ) const
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
    void CTR_DebugStick::SetStickClampParam( s16 min, s16 max )
    {
      GFL_UNUSED(min);
      GFL_UNUSED(max);
    }


  } //namespace ui
} //namespace gfl2 

#endif //GFL2_UI_USE_DEBUG_PAD
#endif