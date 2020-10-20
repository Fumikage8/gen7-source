//=============================================================================
/**
 * @file   gfl2_UI_CTR_CrossKey.cpp
 * @brief  3DSの十字キー入力を検出するクラス
 * @author obata_toshihiro
 * @date   2010.10.21
 */
//=============================================================================
#ifdef  GF_PLATFORM_CTR
#include <nn.h>
#include "ui/include/gfl2_UI_VectorDevice.h"
#include "./gfl2_UI_CTR_CrossKey.h"

namespace gfl2 {
  namespace ui {


    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param[in] reader  ボタン入力の読み取りを行うインスタンス
     * @param[in] status  読み取ったボタン入力を取得するバッファ
     */
    //-------------------------------------------------------------------------
    CTR_CrossKey::CTR_CrossKey( 
        nn::hid::PadReader* reader, nn::hid::PadStatus* status ) :
      DeviceImplementerForVectorDevice(),
      m_pPadReader( reader ),
      m_pPadStatus( status ),
      m_isStickEmulationEnable( false )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    CTR_CrossKey::~CTR_CrossKey()
    {
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief メイン処理
     */
    //-------------------------------------------------------------------------
    void CTR_CrossKey::Main( void )
    {
      // 何もしない
    }

    //-------------------------------------------------------------------------
    /**
     * @brief アナログスティックによるエミュレーションを有効にする
     */
    //-------------------------------------------------------------------------
    void CTR_CrossKey::SetStickEmulationEnable( void )
    {
      m_isStickEmulationEnable = true;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief アナログスティックによるエミュレーションを無効にする
     */
    //-------------------------------------------------------------------------
    void CTR_CrossKey::SetStickEmulationDisable( void )
    {
      m_isStickEmulationEnable = false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 入力データを取得する
     * @param buffer  取得したデータの格納先 
     */
    //-------------------------------------------------------------------------
    void CTR_CrossKey::GetDetectiveData( void* buffer ) const
    {
      VectorDevice::DetectiveData* data = 
        reinterpret_cast<VectorDevice::DetectiveData*>(buffer);

      // 左右キー
      if( m_pPadStatus->hold & nn::hid::BUTTON_LEFT ) {
        data->x = -1.0f;
      }
      else if( m_pPadStatus->hold & nn::hid::BUTTON_RIGHT ) {
        data->x = 1.0f;
      }
      else {
        data->x = 0.0f;
      }

      // 左右キーのスティックエミュレーション
      if( ( data->x == 0.0f ) && ( m_isStickEmulationEnable ) )
      {
        if( m_pPadStatus->hold & nn::hid::BUTTON_EMULATION_LEFT ) {
          data->x = -1.0f; 
        }
        else if( m_pPadStatus->hold & nn::hid::BUTTON_EMULATION_RIGHT ) {
          data->x = 1.0f;
        }
      }

      // 上下キー
      if( m_pPadStatus->hold & nn::hid::BUTTON_DOWN ) {
        data->y =  -1.0f;
      }
      else if( m_pPadStatus->hold & nn::hid::BUTTON_UP ) {
        data->y =  1.0f;
      }
      else {
        data->y = 0.0f;
      }

      // 上下キーのスティックエミュレーション
      if( ( data->y == 0.0f ) && ( m_isStickEmulationEnable ) )
      {
        if( m_pPadStatus->hold & nn::hid::BUTTON_EMULATION_DOWN ) {
          data->y = -1.0f; 
        }
        else if( m_pPadStatus->hold & nn::hid::BUTTON_EMULATION_UP ) {
          data->y = 1.0f;
        }
      }
    }

    //---------------------------------------------------------------------
    /**
     * @brief アナログスティックのクランプモードを取得する
     */
    //---------------------------------------------------------------------
    VectorDevice::StickClampMode CTR_CrossKey::GetStickClampMode( void ) const
    {
      return VectorDevice::STICK_CLAMP_MODE_CROSS;
    }

    //---------------------------------------------------------------------
    /**
     * @brief アナログスティックのクランプモードを設定する
     */
    //---------------------------------------------------------------------
    void CTR_CrossKey::SetStickClampMode( VectorDevice::StickClampMode clamp_mode )
    {
      GFL_UNUSED(clamp_mode);
    }

    //---------------------------------------------------------------------
    /**
     * @brief アナログスティックのクランプパラメータを取得する
     * @param[out] min  最小クランプ値の格納先
     * @param[out] max  最大クランプ値の格納先
     */
    //---------------------------------------------------------------------
    void CTR_CrossKey::GetStickClampParam( s16* min, s16* max ) const
    {
      GFL_UNUSED(min);
      GFL_UNUSED(max);
    }

    //---------------------------------------------------------------------
    /**
     * @brief アナログスティックのクランプパラメータを設定する
     * @param[in] min  最小クランプ値
     * @param[in] max  最大クランプ値
     */
    //---------------------------------------------------------------------
    void CTR_CrossKey::SetStickClampParam( s16 min, s16 max )
    {
      GFL_UNUSED(min);
      GFL_UNUSED(max);
    }


  } // namespace ui
} // namespace gfl2 
#endif