//=============================================================================
/**
 * @brief  ポケモンパラメータの暗号化, 復号化
 * @file   pml_PokemonParamEncoder.cpp
 * @author obata_toshihiro
 * @date   2010.12.24
 */
//=============================================================================
#include "pml_PokemonParamEncoder.h"


namespace pml {
  namespace pokepara {


    //-------------------------------------------------------------------------
    /**
     * @brief チェックサムを生成する
     *
     * @param pData     データ
     * @param dataSize  データサイズ
     */
    //-------------------------------------------------------------------------
    u16 Encoder::CalcChecksum( const void* pData, u32 dataSize )
    {
      const u16 *p = reinterpret_cast<const u16*>( pData );
      u16 sum = 0;

      for( u32 i=0 ; i<dataSize/2 ; i++ )
      {
        sum += p[i];
      } 
      return sum;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 暗号化する
     *
     * @param pData     データ
     * @param dataSize  データサイズ
     * @param key       暗号化キー
     */
    //-------------------------------------------------------------------------
    void Encoder::Encode( void* pData, u32 dataSize, u32 key )
    {
      u16 *p = reinterpret_cast<u16*>( pData );
      u32 code = key;

      for( u32 i=0 ; i<dataSize/2 ; i++ )
      {
        p[i] ^= CalcNextRandCode( &code );
      }
    }

    /**
     * @brief 乱数暗号キーを生成する
     */
    u16 Encoder::CalcNextRandCode( u32* code )
    {
      code[0] = code[0] *1103515245L + 24691;
      return ( u16 )( code[0] / 65536L ) ;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 復号化する
     *
     * @param pData     データ
     * @param dataSize  データサイズ
     * @param key       暗号に使用したキー
     */
    //-------------------------------------------------------------------------
    void Encoder::Decode( void* pData, u32 dataSize, u32 key )
    {
      Encode( pData, dataSize, key ); // 暗号化と同じ処理を施す
    }



  } // namespace pokepara
} // namespace pml
