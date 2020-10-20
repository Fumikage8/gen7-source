//=============================================================================
/**
 * @file   PokemonValidationListener.h
 *
 * @brief  ポケモン不正チェック用イベントリスナ
 *
 * @author hisanao suzuki
 */
//=============================================================================
#if defined(GF_PLATFORM_CTR)
#ifndef __POKEMON_VALIDATION_LISTENER_H__
#define __POKEMON_VALIDATION_LISTENER_H__
#pragma once

#include <nex.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <heap/include/gfl2_Heap.h>

#include <NetStatic/NetLib/include/Validation/PokemonValidationDefine.h>


namespace NetLib {
namespace Validation {


/**
 * @brief ポケモン不正チェック非同期通信の完了通知用イベントリスナクラス
 */
class PokemonValidationListener
{
public:
  //------------------------------------------------------------------
  /**
   * @brief ポケモン不正チェック処理の完了イベントを検知
   *
   * @note  本イベント終了後、[checkResult]配列、および[certData]は解放される
   *
   * @param[in]  checkResult  個々のポケモンの検査結果が配列で格納される。
   * @param[in]  resultNum    検査結果配列の数
   * @param[in]  certData     署名データ
   * @param[in]  certDataSize 署名データサイズ。この値が0の場合は、不正なポケモンデータであることを指す。
   */
  //------------------------------------------------------------------
   virtual void OnPokemonValidationSuccess( const nn::nex::qInt32* checkResult, const u32 resultNum,
                                            const u8* certData, const u32 certDataSize ) = 0;

   //------------------------------------------------------------------
   /**
   * @brief ポケモン不正チェック処理のエラーイベントを検知
   *
   * @note  HTTP通信のステータスコードが200番台外の場合のエラー通知
   *
   * @param[in] httpStatus サーバから返却されたHTTPステータスコード
   */
   //------------------------------------------------------------------
   virtual void OnPokemonValidationError( const s32 httpStatus ) = 0;

   //------------------------------------------------------------------
   /**
   * @brief ポケモン不正チェック処理のエラーイベントを検知
   *
   * @param[in] result    エラー内容
   * @param[in] errorCode ネットワークエラーコード
   */
   //------------------------------------------------------------------
   virtual void OnPokemonValidationError( const nn::nex::qResult &result, u32 errorCode ) = 0;

   //------------------------------------------------------------------
   /**
   * @brief ポケモン不正チェック処理のキャンセルイベントを検知
   */
   //------------------------------------------------------------------
   virtual void OnPokemonValidationCanceled( void ){};
};


}	// validation
}	// netapp


#endif // __POKEMON_VALIDATION_LISTENER_H__
#endif