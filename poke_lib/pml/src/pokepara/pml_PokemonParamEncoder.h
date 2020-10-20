//=============================================================================
/**
 * @brief  ポケモンパラメータの暗号化, 復号化
 * @file   pml_PokemonParamEncoder.h
 * @author obata_toshihiro
 * @date   2010.12.24
 */
//=============================================================================
#ifndef __PML_POKEMONPARAMENCODER_H__
#define __PML_POKEMONPARAMENCODER_H__

#include <macro/include/gfl2_Macros.h>
#include <pml/include/pml_Type.h>

namespace pml {
  namespace pokepara {


    class Encoder 
    {
      GFL_FORBID_COPY_AND_ASSIGN( Encoder ); 

      public:

      //-----------------------------------------------------------------------
      /**
       * @brief チェックサムを生成する
       *
       * @param pData     データ
       * @param dataSize  データサイズ
       */
      //-----------------------------------------------------------------------
      static u16 CalcChecksum( const void* pData, u32 size );

      //-----------------------------------------------------------------------
      /**
       * @brief 暗号化する
       *
       * @param pData     データ
       * @param dataSize  データサイズ
       * @param key       暗号化キー
       */
      //-----------------------------------------------------------------------
      static void Encode( void* pData, u32 dataSize, u32 key );

      //-----------------------------------------------------------------------
      /**
       * @brief 復号化する
       *
       * @param pData     データ
       * @param dataSize  データサイズ
       * @param key       暗号に使用したキー
       */
      //-----------------------------------------------------------------------
      static void Decode( void* pData, u32 dataSize, u32 key );



      private: 
      static void EncodeAct( void* data, u32 size, u32 code );  // 暗号化処理
      static void DecodeAct( void* data, u32 size, u32 code );  // 復号化処理
      static u16 CalcNextRandCode( u32* code );                 // 乱数暗号キーを生成する
    };


  } // namespace pokepara
} // namespace pml

#endif //__PML_POKEMONPARAMENCODER_H__
