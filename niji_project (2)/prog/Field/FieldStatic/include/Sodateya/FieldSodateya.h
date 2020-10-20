//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldSodateya.h
 *	@brief  育て屋チェック
 *	@author	iwasawa
 *	@date		2012.10.02
 *  @date   2015.12.12 niji用に移植 onoue_masayuki
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __FIELD_SODATEYA_H__
#define __FIELD_SODATEYA_H__

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

// field
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/Encount/FieldEncountPokeParam.h"
#include "niji_reference_files/script/FieldPawnTypes.h"   //SodateyaLoveLevel

#include "Print/include/WordSetLoader.h"

//#include "field/FieldmapWork.h"
//#include "player/FieldPlayerGrid.h"
//#include "savedata/sv_sodateya.h"

namespace Field
{


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//#include "FieldSodateyaDef.h"


//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

  // 育て屋チェック
  class FieldSodateya
  {
    GFL_FORBID_COPY_AND_ASSIGN(FieldSodateya); // コピーコンストラクタ＋代入禁止

  public:

  private:

    // 相性(タマゴができる確率[%])
    enum Love{
      LOVE_LV_GOOD = 70,    // 「とっても よい ようじゃ」
      LOVE_LV_NORMAL = 50,  // 「まずまずの ようじゃ」
      LOVE_LV_BAD = 20,     // 「それほど よくないがなぁ」
      LOVE_LV_WORST = 0,    // 「ちがう ポケモンたちと あそんでおるがなぁ」

      // 相性(タマゴができる確率[%])
      // ※「まるいおまもり」を所持していた場合
      LOVE_LV_GOOD_EX = 88,    // 4.375% → 5.5%
      LOVE_LV_NORMAL_EX = 80,  // 3.125% → 5%
      LOVE_LV_BAD_EX = 40,     // 1.25%  → 2.5%
    };
    enum{
      EGG_GROUP_MAX = 2,        //タマゴグループデータの数
      EGG_CHECK_INTERVAL = 180, //何歩ごとにタマゴ発生チェックをするか？ // 2016.05.12 タマゴ発生速度を180に変更
      POKE_EXP_PER_WALK = 1,    //一歩あたりの加算経験値
    };
    
    struct LoveCheckParam{
      pml::pokepara::PokemonParam* pp;
      MonsNo monsno;
      u8  sex;
      u32 id;
      u16 group[EGG_GROUP_MAX];
    };

    struct WalkCheckData{
      u32  mSodateyaWalkCount;                        ///<育て屋専用歩数カウンター
    };

  public:
    // コンストラクタ
    FieldSodateya(Fieldmap* p_fieldmap);

    // デストラクタ
    ~FieldSodateya();

  public:
  
    //----------------------------------------------------------------------------------------
    /**
     * @brief 預けてある二匹の相性をチェック 
     */
    //---------------------------------------------------------------------------------------- 
    SodateyaLoveLevel LoveCheck( Savedata::SodateyaID id );

   //----------------------------------------------------------------------------------------
    /**
     * @brief タマゴを生成して渡す(PokemonParamを生成して返すので呼び出し元で開放すること)
     * 
     * @retval NULL 受け取れなかった
     * @retval PokemonParam*
     * 
     */
    //----------------------------------------------------------------------------------------
    pml::pokepara::PokemonParam* EggAdopt( Savedata::SodateyaID id );

    //----------------------------------------------------------------------------------------
    /**
     * @brief 一歩ごとの育てる処理
     */
    //---------------------------------------------------------------------------------------- 
    void PokeBreed( void );

    //----------------------------------------------------------------------------
    /**
     *	@brief  育て屋 ポケモンを預ける
     */
    //-----------------------------------------------------------------------------
    void PokeLeave( Savedata::SodateyaID id, u32 party_pos );
  
    //----------------------------------------------------------------------------------------
    /**
     * @brief ポケモンを引き取る
     *
     * @param idx 引き取るポケモンのindex
    */
    //---------------------------------------------------------------------------------------- 
    void PokeTakeBack( Savedata::SodateyaID id, u32 idx );
  
    //----------------------------------------------------------------------------------------
    /**
     * @brief 預けているポケモンの引き取りメニューリスト用ワードセット
     * BTS[996] ニックネーム付き二ドラン♂♀の表記修正
     */
    //---------------------------------------------------------------------------------------- 
    pml::Sex PokeTakeBackMenuWordset( print::WordSet* wset, u32 buf_id, Savedata::SodateyaID id, u32 idx );
  
    //----------------------------------------------------------------------------------------
    /**
     * @brief 預けているポケモンのニックネームをワードセット展開
    */
    //---------------------------------------------------------------------------------------- 
    void PokeNameWordset( print::WordSet* wset, u32 buf_id, Savedata::SodateyaID id, u32 idx );
  
  private:
    SodateyaLoveLevel LoveCheckCore( pml::pokepara::PokemonParam* pp1,pml::pokepara::PokemonParam* pp2 );
    SodateyaLoveLevel LoveCheckCoreSame( LoveCheckParam* prm );
  
  private:

    Fieldmap* m_pFieldmap;
    gfl2::heap::HeapBase* m_heap;
    GameSys::GameManager* m_pGameManager;
    GameSys::GameData*    m_pGameData;
    Savedata::Sodateya*  m_pSodateyaSave;


    //====================================================================
    //ここからデバッグ用関数群
  public:
#if PM_DEBUG
    enum{
      DEBUG_PP_MAKE_POKE1,
      DEBUG_PP_MAKE_POKE2,
      DEBUG_PP_MAKE_EGG,
    };
    pml::pokepara::PokemonParam* DebugPokeParaMake( Savedata::SodateyaID id, int make_code, gfl2::heap::HeapBase* m_heap );
    bool DebugPokeParaSet( Savedata::SodateyaID id, int make_code, pml::pokepara::PokemonParam* pp );

#endif  //PM_DEBUG
    //ここまでデバッグ用関数群
    //====================================================================
  };


} // namespace Field


#endif	// __FIELD_SODATEYA_H__

