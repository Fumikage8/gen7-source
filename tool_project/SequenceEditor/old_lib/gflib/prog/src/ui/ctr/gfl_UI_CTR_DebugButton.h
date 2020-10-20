//=============================================================================
/**
 * @file   gfl_UI_CTR_DebugButton.h
 * @brief  3DSのデバッグパッドボタンの入力を検出するクラス
 * @author obata_toshihiro
 * @date   2011.02.09
 */
//=============================================================================
#ifndef __GFL_UI_CTR_DEBUGBUTTON_H__
#define __GFL_UI_CTR_DEBUGBUTTON_H__
#include "gfl_Macros.h"
#include "../gfl_UI_DeviceImplementer.h"

namespace gfl {
  namespace ui {


    class CTR_DebugButton : public DeviceImplementer 
    { 
      GFL_FORBID_COPY_AND_ASSIGN( CTR_DebugButton );

      public:

      //-----------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       *
       * @param p_reader  デバッグパッド入力の読み取りを実行するインスタンス
       * @param p_status  読み込んだ入力を取得するバッファ
       */
      //-----------------------------------------------------------------------
      CTR_DebugButton( nn::hid::DebugPadReader* p_reader, nn::hid::DebugPadStatus* p_status );

      //-----------------------------------------------------------------------
      /**
       * @brief デストラクタ
       */
      //-----------------------------------------------------------------------
      virtual ~CTR_DebugButton();

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

      //-----------------------------------------------------------------------
      /**
       * @brief 実効データのサイズを取得する
       */
      //-----------------------------------------------------------------------
      virtual u32 GetEffectiveDataSize( void ) const;


      protected:
      nn::hid::DebugPadReader* m_pPadReader;
      nn::hid::DebugPadStatus* m_pPadStatus;
    };


  } //namespace ui
} //namespace gfl 

#endif //__GFL_UI_CTR_DEBUGBUTTON_H__
