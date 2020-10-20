//=============================================================================
/**
 * @file   gfl_UI_CTR_Stick.cpp
 * @brief  3DSのアナログスティック入力を検出するクラス
 * @author obata_toshihiro
 * @date   2010.10.21
 */
//=============================================================================
#include <nn.h>
#include "ui/gfl_UI_VectorDevice.h"
#include "./gfl_UI_CTR_Stick.h"

namespace gfl {
  namespace ui {


    // モード別のクランプパラメータ上限値・下限値
    const s16 VectorDevice::MAX_STICK_CLAMP_PARAM_ON_CIRCLE_MODE  = nn::hid::LIMIT_OF_STICK_CLAMP_MAX;
    const s16 VectorDevice::MIN_STICK_CLAMP_PARAM_ON_CIRCLE_MODE  = nn::hid::MIN_OF_STICK_CLAMP_MODE_CIRCLE;
    const s16 VectorDevice::MAX_STICK_CLAMP_PARAM_ON_CROSS_MODE   = nn::hid::LIMIT_OF_STICK_CLAMP_MAX;
    const s16 VectorDevice::MIN_STICK_CLAMP_PARAM_ON_CROSS_MODE   = nn::hid::MIN_OF_STICK_CLAMP_MODE_CROSS;
    const s16 VectorDevice::MAX_STICK_CLAMP_PARAM_ON_MINIMUM_MODE = nn::hid::LIMIT_OF_STICK_CLAMP_MAX;
    const s16 VectorDevice::MIN_STICK_CLAMP_PARAM_ON_MINIMUM_MODE = nn::hid::MIN_OF_STICK_CLAMP_MODE_CIRCLE;


    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     *
     * @param p_reader  入力の読み取りを実行するインスタンス
     * @param p_status  読み取った入力を取得するバッファ
     */
    //-------------------------------------------------------------------------
    CTR_Stick::CTR_Stick(
        nn::hid::PadReader* p_reader,
        nn::hid::PadStatus* p_status ) :
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
    CTR_Stick::~CTR_Stick()
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief メイン処理
     */
    //-------------------------------------------------------------------------
    void CTR_Stick::Main( void )
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
    void CTR_Stick::GetDetectiveData( void* p_buffer ) const
    {
      VectorDevice::DetectiveData* data = reinterpret_cast<VectorDevice::DetectiveData*>(p_buffer);
      data->x = m_pPadReader->NormalizeStick( m_pPadStatus->stick.x );
      data->y = m_pPadReader->NormalizeStick( m_pPadStatus->stick.y );
    }

    //-------------------------------------------------------------------------------
    /**
     * @brief アナログスティックのクランプモードを取得する
     */
    //-------------------------------------------------------------------------------
    VectorDevice::StickClampMode CTR_Stick::GetStickClampMode( void ) const
    {
      nn::hid::PadReader::StickClampMode nn_mode = m_pPadReader->GetStickClampMode();
      switch( nn_mode ) {
      case nn::hid::PadReader::STICK_CLAMP_MODE_CIRCLE:  return VectorDevice::STICK_CLAMP_MODE_CIRCLE;
      case nn::hid::PadReader::STICK_CLAMP_MODE_CROSS:  return VectorDevice::STICK_CLAMP_MODE_CROSS;
      case nn::hid::PadReader::STICK_CLAMP_MODE_MINIMUM:  return VectorDevice::STICK_CLAMP_MODE_MINIMUM;
      default: 
        GFL_ASSERT(0); //@check
        break;
      }
      return VectorDevice::STICK_CLAMP_MODE_CIRCLE;
    }

    //-------------------------------------------------------------------------------
    /**
     * @brief アナログスティックのクランプモードを設定する
     */
    //-------------------------------------------------------------------------------
    void CTR_Stick::SetStickClampMode( VectorDevice::StickClampMode clamp_mode )
    {
      nn::hid::PadReader::StickClampMode nn_mode;
      switch( clamp_mode ) {
      case VectorDevice::STICK_CLAMP_MODE_CIRCLE:  nn_mode = nn::hid::PadReader::STICK_CLAMP_MODE_CIRCLE;  break;
      case VectorDevice::STICK_CLAMP_MODE_CROSS:  nn_mode = nn::hid::PadReader::STICK_CLAMP_MODE_CROSS;  break;
      case VectorDevice::STICK_CLAMP_MODE_MINIMUM:  nn_mode = nn::hid::PadReader::STICK_CLAMP_MODE_MINIMUM;  break;
      default:
        GFL_ASSERT(0); //@check
        return;
      }
      m_pPadReader->SetStickClampMode( nn_mode );
    }

    //-------------------------------------------------------------------------------
    /**
     * @brief アナログスティックのクランプパラメータを取得する
     *
     * @param[out] min  最小クランプ値の格納先
     * @param[out] max  最大クランプ値の格納先
     */
    //-------------------------------------------------------------------------------
    void CTR_Stick::GetStickClampParam( s16* min, s16* max ) const
    {
      m_pPadReader->GetStickClamp( min, max );
    }

    //-------------------------------------------------------------------------------
    /**
     * @brief アナログスティックのクランプパラメータを設定する
     *
     * @param[in] min  最小クランプ値
     * @param[in] max  最大クランプ値
     */
    //-------------------------------------------------------------------------------
    void CTR_Stick::SetStickClampParam( s16 min, s16 max )
    {
      m_pPadReader->SetStickClamp( min, max );
    }


  } //namespace ui
} //namespace gfl 
