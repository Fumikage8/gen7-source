//=============================================================================
/**
 * @file   gfl_UI_CTR_TouchPanel.cpp
 * @brief  3DSのタッチパネル入力を検出するクラス
 * @author toshihiro_obata
 * @date   2010.10.21
 */
//=============================================================================
#include <nn.h>
#include "ui/gfl_UI_TouchPanel.h"
#include "./gfl_UI_CTR_TouchPanel.h"

namespace gfl {
  namespace ui {


    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     *
     * @param p_reader  入力の読み取りを実行するインスタンス
     * @param p_status  読み取った入力を取得するバッファ
     */
    //-------------------------------------------------------------------------
    CTR_TouchPanel::CTR_TouchPanel( 
        nn::hid::TouchPanelReader* p_reader,
        nn::hid::TouchPanelStatus* p_status ) : 
      DeviceImplementer(),
      m_pTPReader( p_reader ),
      m_pTPStatus( p_status )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    CTR_TouchPanel::~CTR_TouchPanel()
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief メイン処理
     */
    //-------------------------------------------------------------------------
    void CTR_TouchPanel::Main( void )
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
    void CTR_TouchPanel::GetDetectiveData( void* p_buffer ) const
    {
      TouchPanel::DetectiveData* data = reinterpret_cast<TouchPanel::DetectiveData*>(p_buffer);

      data->x = m_pTPStatus->x;
      data->y = m_pTPStatus->y; 
      data->touch = (m_pTPStatus->touch != 0);
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 入力データのサイズを取得する
     */
    //-------------------------------------------------------------------------
    u32 CTR_TouchPanel::GetDetectiveDataSize( void ) const
    {
      return sizeof(TouchPanel::DetectiveData);
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 実効データのサイズを取得する
     */
    //-------------------------------------------------------------------------
    u32 CTR_TouchPanel::GetEffectiveDataSize( void ) const
    {
      return sizeof( TouchPanel::EffectiveData );
    }


  } //namespace ui
} //namespace gfl 
