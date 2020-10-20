//=============================================================================
/**
 * @file   gfl_UI_TouchPanel_CTR.h
 * @brief  3DSのタッチパネル入力を検出するクラス
 * @author toshihiro_obata
 * @date   2010.10.21
 */
//=============================================================================
#ifndef __GFL_UI_CTR_TOUCHPANEL_H__
#define __GFL_UI_CTR_TOUCHPANEL_H__
#include "gfl_Macros.h"
#include "../gfl_UI_DeviceImplementer.h"


namespace gfl {
  namespace ui {


    class CTR_TouchPanel : public DeviceImplementer 
    {
      GFL_FORBID_COPY_AND_ASSIGN( CTR_TouchPanel );

      public:

      //-----------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       *
       * @param p_reader  入力の読み取りを実行するインスタンス
       * @param p_status  読み取った入力を取得するバッファ
       */
      //-----------------------------------------------------------------------
      CTR_TouchPanel( nn::hid::TouchPanelReader* p_reader, nn::hid::TouchPanelStatus* p_status );

      //-----------------------------------------------------------------------
      /**
       * @brief デストラクタ
       */
      //-----------------------------------------------------------------------
      virtual ~CTR_TouchPanel();

      //-----------------------------------------------------------------------
      /**
       * @brief メイン処理
       */
      //-----------------------------------------------------------------------
      virtual void Main( void );

      //-----------------------------------------------------------------------
      /**
       * @brief 入力データを取得する
       *
       * @param p_buffer  取得したデータの格納先 
       */
      //-----------------------------------------------------------------------
      virtual void GetDetectiveData( void* p_buffer ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief 入力データのサイズを取得する
       */
      //-----------------------------------------------------------------------
      virtual u32 GetDetectiveDataSize( void ) const;

      //------------------------------------------------------------------------
      /**
       * @brief 実効データのサイズを取得する
       */
      //------------------------------------------------------------------------
      virtual u32 GetEffectiveDataSize( void ) const;


      protected:
      nn::hid::TouchPanelReader* m_pTPReader;
      nn::hid::TouchPanelStatus* m_pTPStatus;
    };


  } // namespace ui
} // namespace gfl

#endif //__GFL_UI_CTR_TOUCHPANEL_H__
