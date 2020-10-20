//===========================================================================
/**
 *	@file		SodateyaSave.cpp
 *	@brief  育て屋関連セーブデータ
 *	@author iwasawa	
 *	@date		2015.12.06
 */
//===========================================================================

#include "System/include/GflUse.h"
#include "Savedata/include/SodateyaSave.h"

namespace Savedata {

//============================================================================
//初期化/解放
//============================================================================

/* -------------------------------------------------------------------------*/
/**
 * @brief コンストラクタ
 */
/* -------------------------------------------------------------------------*/
Sodateya::Sodateya(void)
{
  gfl2::std::MemClear( &m_SodateyaSaveData, sizeof(SodateyaSaveData)*SODATEYA_ID_MAX );
  ONOUE_PRINT("Sodateya SaveData Size:%d\n",GetDataSize());
}

/* -------------------------------------------------------------------------*/
/**
 * @brief デストラクタ
 */
/* -------------------------------------------------------------------------*/
Sodateya::~Sodateya(void)
{
}

/* -------------------------------------------------------------------------*/
/**
 * @brief 初期データ設定
 *
 * @param heap
 */
/* -------------------------------------------------------------------------*/
void Sodateya::InitSodateyaData( gfl2::heap::HeapBase * heap )
{
  gfl2::std::MemClear( &m_SodateyaSaveData, sizeof(SodateyaSaveData)*SODATEYA_ID_MAX );
  
  //---------------------------
  /// タマゴ生成ランダムシード初期化
#if 0
  for( int i = 0;i < SODATEYA_ID_MAX;i++){
    this->ResetEggCreateSeed(static_cast<SodateyaID>(i));
  }
#else
  /*
   * GFCHECK[8501] セーブデータクリア処理後のCRCの不一致(Winのみ)　Sodateya
   * 
   *↑上のように書きたいが、Arm4コンパイラのループ最適化のバグにHitし
   *  Develop以上でビルドするとgfl2::math::Random::State randomStateが上手く初期化されない
   *  
   *  力技だがループをやめて直書きする
   *  nijiの育て屋は１箇所
   *  
   *  2016.06.03 by iwasawa
   */
  this->ResetEggCreateSeed(SODATEYA_ID_I02);
#endif
}

/****************************************************************************/
//============================================================================
//公開関数群
//============================================================================
/****************************************************************************/

//============================================================================
//タマゴ関数群
//============================================================================

/* -------------------------------------------------------------------------*/
/**
 * @brief タマゴの有無をチェックする
 *
 * @retval  true   タマゴがある
 * @retval  false  タマゴがない
 */
/* -------------------------------------------------------------------------*/
bool Sodateya::IsEggExist(SodateyaID id)
{
  SodateyaEggData* p_egg = &(m_SodateyaSaveData[id].eggData);

  return p_egg->valid_f;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief タマゴをセットする
 */
/* -------------------------------------------------------------------------*/
void Sodateya::EggSet(SodateyaID id)
{
  SodateyaEggData* p_egg = &(m_SodateyaSaveData[id].eggData);

  p_egg->valid_f = true;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief タマゴを削除する
 */
/* -------------------------------------------------------------------------*/
void Sodateya::EggClear(SodateyaID id)
{
  SodateyaEggData* p_egg = &(m_SodateyaSaveData[id].eggData);

  this->UpdateEggCreateSeed(id); //乱数を進める
  p_egg->valid_f = false;
}

//============================================================================
//ポケモン関数群
//============================================================================

/* -------------------------------------------------------------------------*/
/**
 * @brief 預けているポケモン数を取得する
 */
/* -------------------------------------------------------------------------*/
u32 Sodateya::PokeNumGet(SodateyaID id)
{
  int i,ct = 0;

  SodateyaPokeData* p_poke = m_SodateyaSaveData[id].pokeData;
  
  for( i = 0;i < SODATEYA_POKE_MAX; ++i ){
    ct += p_poke[i].valid_f;
  }
  return ct;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief 飼育ポケモンパラメータを取得する
 */
/* -------------------------------------------------------------------------*/
void Sodateya::PokemonParamGet( SodateyaID id, u32 idx, pml::pokepara::CoreParam* pcp )
{
  SodateyaPokeData* p_poke = &(m_SodateyaSaveData[id].pokeData[idx]);

  pcp->Deserialize_Core( p_poke->coreParam );
}

/* -------------------------------------------------------------------------*/
/**
 * @brief ポケモンを預ける　飼育パラメータを設定する
 */
/* -------------------------------------------------------------------------*/
void Sodateya::PokemonLeave( SodateyaID id, u32 idx, pml::pokepara::PokemonParam* pp )
{
  SodateyaPokeData* p_poke = &(m_SodateyaSaveData[id].pokeData[idx]);

  pp->Serialize_Core( p_poke->coreParam );
  
  p_poke->valid_f = true;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief ポケモンを引き取る　飼育ポケモンパラメータを取得後にクリアする(引き取る)
 */
/* -------------------------------------------------------------------------*/
void Sodateya::PokemonTakeBack( SodateyaID id, u32 idx, pml::pokepara::PokemonParam* pp )
{
  int i, j;

  SodateyaPokeData* p_poke = &(m_SodateyaSaveData[id].pokeData[idx]);

  this->PokemonParamGet( id, idx, pp );
  
  //育てていた間のデータをクリア
  p_poke->valid_f = false;

  //空き要素を前詰めする
  for( i = 0;i < SODATEYA_POKE_MAX;++i){
    if( m_SodateyaSaveData[id].pokeData[i].valid_f ) continue;
    
    for( j = i+1;j < SODATEYA_POKE_MAX; ++j ){
      if( m_SodateyaSaveData[id].pokeData[j].valid_f ){
        //前詰め
        m_SodateyaSaveData[id].pokeData[i] = m_SodateyaSaveData[id].pokeData[j];
        m_SodateyaSaveData[id].pokeData[j].valid_f = 0;
      }
    }
  }
}

/****************************************************************************/
//============================================================================
//非公開関数群
//============================================================================
/****************************************************************************/

//============================================================================
//タマゴ関数群
//============================================================================
/* -------------------------------------------------------------------------*/
/**
 * @brief タマゴ生成乱数をリセットする
 *
 * @param SodateyaID id 育て屋ID
 */
/* -------------------------------------------------------------------------*/
void Sodateya::ResetEggCreateSeed( SodateyaID id )
{
  gfl2::math::Random rand;
  rand.Initialize();
  rand.SaveState();
  m_SodateyaSaveData[id].eggData.randomState = rand.SaveState();
}

/* -------------------------------------------------------------------------*/
/**
* @brief タマゴ生成乱数を更新する
*
* @param SodateyaID id 育て屋ID
*/
/* -------------------------------------------------------------------------*/
void Sodateya::UpdateEggCreateSeed( SodateyaID id )
{
  gfl2::math::Random rand;
  rand.Initialize(GetEggCreateRandState(id));
  rand.Next();
  SetEggCreateRandState(id,rand.SaveState());
}

}		//namespace Savedata

