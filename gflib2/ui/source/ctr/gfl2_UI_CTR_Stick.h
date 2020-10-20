//=============================================================================
/**
 * @file   gfl2_UI_CTR_Stick.h
 * @brief  3DSのアナログスティック入力を検出するクラス
 * @author obata_toshihiro
 * @date   2010.10.14
 */
//=============================================================================
#ifdef  GF_PLATFORM_CTR
#ifndef __gfl2_UI_CTR_STICK_H__
#define __gfl2_UI_CTR_STICK_H__
#include <nn.h>
#include <macro/include/gfl2_Macros.h>
#include "../gfl2_UI_DeviceImplementerForVectorDevice.h"

namespace gfl2 {
  namespace ui {


    class CTR_Stick : public DeviceImplementerForVectorDevice 
    {
      GFL_FORBID_COPY_AND_ASSIGN( CTR_Stick );

      public:

      //-----------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       *
       * @param p_reader  入力の読み取りを実行するインスタンス
       * @param p_status  読み取った入力を取得するバッファ
       */
      //-----------------------------------------------------------------------
      CTR_Stick( nn::hid::PadReader* p_reader, nn::hid::PadStatus* p_status );

      //-----------------------------------------------------------------------
      /**
       * @brief デストラクタ
       */
      //-----------------------------------------------------------------------
      virtual ~CTR_Stick(); 

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
       * @brief アナログスティックのクランプモードを取得する
       */
      //-----------------------------------------------------------------------
      virtual VectorDevice::StickClampMode GetStickClampMode( void ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief アナログスティックのクランプモードを設定する
       */
      //-----------------------------------------------------------------------
      virtual void SetStickClampMode( VectorDevice::StickClampMode clamp_mode );

      //-----------------------------------------------------------------------
      /**
       * @brief アナログスティックのクランプパラメータを取得する
       *
       * @param[out] min  最小クランプ値の格納先
       * @param[out] max  最大クランプ値の格納先
       */
      //-----------------------------------------------------------------------
      virtual void GetStickClampParam( s16* min, s16* max ) const;

      //-----------------------------------------------------------------------
      /**
       * @brief アナログスティックのクランプパラメータを設定する
       *
       * @param[in] min  最小クランプ値
       * @param[in] max  最大クランプ値
       */
      //-----------------------------------------------------------------------
      virtual void SetStickClampParam( s16 min, s16 max );



      protected:
      nn::hid::PadReader* m_pPadReader;
      nn::hid::PadStatus* m_pPadStatus;
    };


  } //namespace ui
} //namespace gfl2

#endif //__gfl2_UI_CTR_STICK_H__
#endif