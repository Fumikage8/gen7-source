//=============================================================================
/**
 * @brief  ポケモンパラメータ
 * @file   pml_PokemonParam.h
 * @author obata_toshihiro
 * @date   2010.12.28
 */
//=============================================================================
#ifndef __PML_POKEMONPARAM_H__
#define __PML_POKEMONPARAM_H__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include "pml/include/pokepara/pml_PokemonParamSpec.h"
#include "pml/include/pokepara/pml_PokemonCoreParam.h"


namespace pml {

  namespace pokepara {

    //------------------------------------------------------------------------
    /**
     * @brief ポケモンパラメータ
     */
    //------------------------------------------------------------------------
    class PokemonParam : public CoreParam
    {
      GFL_FORBID_COPY_AND_ASSIGN( PokemonParam ); 

      public: 
      static const size_t DATASIZE = 260;  ///< データのバッファ確保用のサイズ定義

      //------------------------------------------------------------------------
      // コンストラクタ・デストラクタ
      //------------------------------------------------------------------------
      PokemonParam( gfl2::heap::HeapBase* heap );
      PokemonParam( gfl2::heap::HeapBase* heap, MonsNo monsno, u16 level, u64 id );
      PokemonParam( gfl2::heap::HeapBase* heap, const InitialSpec& spec );
      PokemonParam( gfl2::heap::HeapBase* heap, const PokemonParam& src );
      PokemonParam( gfl2::heap::HeapBase* heap, const CoreParam& src );
      virtual ~PokemonParam();

      //-----------------------------------------------------------------------
      // セットアップ
      //-----------------------------------------------------------------------
      void Setup( const InitialSpec& spec );

      //-----------------------------------------------------------------------
      // シリアライズ・デシリアライズ
      //-----------------------------------------------------------------------
      /**
       * @brief シリアライズ処理（PokemonParam版）
       * @param[out]  buffer    展開先
       * @note  DATASIZE分領域を確保すること
       */
      void Serialize_Full( void* buffer ) const; 
      /**
       * @brief シリアライズ処理（CoreParam版）
       * @param[out]  buffer    展開先
       * @note  DATASIZE分領域を確保すること
       */
      void Serialize_Core( void* buffer ) const; 
      /**
       * @brief シリアライズデータから復元する(PokemonParam版）
       * @param[in] data  シリアライズデータ
       */
      void Deserialize_Full( const void* data );         // シリアライズデータにより復元する
      /**
       * @brief シリアライズデータから復元する（CoreParam版）
       * @param[in] data  シリアライズデータ
       */
      void Deserialize_Core( const void* data );
      /**
       * @brief   パラメータをコピーする
       * @param[in]   src_param コピー元データ
       */
      void CopyFrom( const PokemonParam& src_param );

#if PML_DEBUG
      //! パラメータのデバッグ出力
      void Dump() const;
#endif

      private:
      void CreateAndAttachCalcData( gfl2::heap::HeapBase* );
      void InitCalcData( void );


      // クラスの静的な設定 ///////////////////////////////////////////////////

      public:

      /**
       * @brief バッファ確保用に定義した DATASIZE の正当性チェック
       */
      static void CheckPublicDataSize( void );
    }; 

  }  // namespace pokepara
}  // namespace pml


#endif  // __PML_POKEMONPARAM_H__
