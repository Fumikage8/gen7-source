//================================================================================
/**
 *	@file		SodateyaSave.h
 *	@brief  育て屋関連セーブデータ
 *	@author iwasawa	
 *	@date		2015.12.06
 */
//================================================================================
#pragma once
#if !defined( __SODATEYA_SV_H__ )
#define __SODATEYA_SV_H__

//==========================================
//プログラム用
//==========================================
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <util/include/gfl2_std_string.h>
#include <pml/include/pokepara/pml_PokemonParam.h>

#include "Savedata/include/SaveDataInterface.h"
#include "niji_reference_files/script/FieldPawnTypes.h"  //SODATEYA_POKE_MAX,SodateyaID

#include <math/include/gfl2_math_random.h>

namespace Savedata {

class Sodateya : public SaveDataInterface
{
public:

private:
  ///育て屋ポケモンデータ
  struct SodateyaPokeData
  {
    bool  valid_f;  ///<有効フラグ
    u8    coreParam[pml::pokepara::CoreParam::DATASIZE];  ///<PokemonCoreParamシリアライズデータ 232byte
  };
  ///育て屋タマゴデータ
  struct SodateyaEggData
  {
    bool  valid_f; //<有効フラグ
    gfl2::math::Random::State randomState;          ///< 乱数の状態
    u64   padding;                                     ///< padding
  };
  ///育て屋セーブデータ
  struct SodateyaSaveData
  {
    SodateyaPokeData  pokeData[SODATEYA_POKE_MAX];  ///<預けているポケモンデータ
    SodateyaEggData   eggData;                      ///<タマゴのデータ
    u8    data[ 8 ];                                ///< padding
  };
  
  SodateyaSaveData m_SodateyaSaveData[SODATEYA_ID_MAX]; ///< 育て屋用セーブデータ

public:

  /* -------------------------------------------------------------------------*/
  /**
   * @brief コンストラクタ
   */
  /* -------------------------------------------------------------------------*/
  Sodateya(void);

  /* -------------------------------------------------------------------------*/
  /**
   * @brief デストラクタ
   */
  /* -------------------------------------------------------------------------*/
  virtual ~Sodateya(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief    読み込んだデータをセットする関数
    * @param    pData   先頭のポインタ
   */
  //-----------------------------------------------------------------------------
	virtual void SetData(void * pData){ gfl2::std::MemCopy( pData, reinterpret_cast<void*>(m_SodateyaSaveData), sizeof(SodateyaSaveData)*SODATEYA_ID_MAX); };

  //-----------------------------------------------------------------------------
  /**
   *  @brief    データの先頭ポインタを得る関数
    * @return   先頭のポインタ
   */
  //-----------------------------------------------------------------------------
	virtual void * GetData(void){ return reinterpret_cast<void*>(m_SodateyaSaveData); };

  //-----------------------------------------------------------------------------
  /**
   *  @brief    データサイズ
   *  @retval   バイト
   */
  //-----------------------------------------------------------------------------
	virtual size_t GetDataSize(void){ return sizeof(SodateyaSaveData)*SODATEYA_ID_MAX; };

  //-----------------------------------------------------------------------------
  /**
   * @brief セーブデータのクリア
   * @param heap　クリアに使用するテンポラリヒープ
   */
  //-----------------------------------------------------------------------------
	virtual void Clear( gfl2::heap::HeapBase * heap ){ this->InitSodateyaData(heap); };

public:
 
  /* -------------------------------------------------------------------------*/
  /**
   * @brief タマゴ生成用乱数状態を取得する
   *
   * @return タマゴ生成用乱数状態
   */
  /* -------------------------------------------------------------------------*/
  gfl2::math::Random::State GetEggCreateRandState(SodateyaID id){ return this->m_SodateyaSaveData[id].eggData.randomState; };
 
  /* -------------------------------------------------------------------------*/
  /**
   * @brief タマゴ生成用乱数状態を保存する
   * @param heap　クリアに使用するテンポラリヒープ
   */
  /* -------------------------------------------------------------------------*/
  void SetEggCreateRandState(SodateyaID id,gfl2::math::Random::State state){ this->m_SodateyaSaveData[id].eggData.randomState = state; };

  /* -------------------------------------------------------------------------*/
  /**
   * @brief タマゴの有無をチェックする
   *
   * @retval  true   タマゴがある
   * @retval  false  タマゴがない
   */
  /* -------------------------------------------------------------------------*/
  bool IsEggExist(SodateyaID id);

  /* -------------------------------------------------------------------------*/
  /**
   * @brief タマゴをセットする
   */
  /* -------------------------------------------------------------------------*/
  void EggSet(SodateyaID id);

  /* -------------------------------------------------------------------------*/
  /**
   * @brief タマゴを削除する
   */
  /* -------------------------------------------------------------------------*/
  void EggClear(SodateyaID id);

  /* -------------------------------------------------------------------------*/
  /**
   * @brief 預けているポケモン数を取得する
   */
  /* -------------------------------------------------------------------------*/
  u32 PokeNumGet(SodateyaID id);

  /* -------------------------------------------------------------------------*/
  /**
   * @brief 飼育ポケモンパラメータを取得する
   */
  /* -------------------------------------------------------------------------*/
  void PokemonParamGet( SodateyaID id, u32 idx, pml::pokepara::CoreParam* pcp );

  /* -------------------------------------------------------------------------*/
  /**
   * @brief ポケモンを預ける　飼育パラメータを設定する
   */
  /* -------------------------------------------------------------------------*/
  void PokemonLeave( SodateyaID id, u32 idx, pml::pokepara::PokemonParam* pp);

  /* -------------------------------------------------------------------------*/
  /**
   * @brief ポケモンを引き取る　飼育ポケモンパラメータを取得後にクリアする(引き取る)
   */
  /* -------------------------------------------------------------------------*/
  void PokemonTakeBack( SodateyaID id, u32 idx, pml::pokepara::PokemonParam* pp );

private:

  /* -------------------------------------------------------------------------*/
  /**
   * @brief 初期データ設定
   *
   * @param heap
   */
  /* -------------------------------------------------------------------------*/
  void InitSodateyaData( gfl2::heap::HeapBase * heap );
  
  /* -------------------------------------------------------------------------*/
  /**
   * @brief タマゴ生成乱数をリセットする
   *
   * @param SodateyaID id 育て屋ID
   */
  /* -------------------------------------------------------------------------*/
  void ResetEggCreateSeed( SodateyaID id );
 
  /* -------------------------------------------------------------------------*/
  /**
   * @brief タマゴ生成乱数を更新する
   *
   * @param SodateyaID id 育て屋ID
   */
  /* -------------------------------------------------------------------------*/
  void UpdateEggCreateSeed( SodateyaID id );

};  //class Sodateya

}		// Savedata

#endif		// __SODATEYA_SV_H__

