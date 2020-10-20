//======================================================================
/**
 * @file BgattleInstTool.cpp
 * @date 2015/01/12
 * @author saita_kazuki
 * @brief バトル施設データ作成などの便利関数群 (sangoから移植)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Field/FieldStatic/include/BattleInst/BattleInstTool.h"
#include "Field/FieldStatic/include/BattleInst/BattleInstData.h"

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

#include <fs/include/gfl2_FsAsyncFileManager.h>
#include <fs/include/gfl2_FsArcFile.h>
#include "AppLib/include/Util/app_util_FileAccessor.h"

#include "Field/FieldStatic/include/Event/EventAppCall.h"
#include "FieldScript/include/FieldScriptSystem.h"
#include "FieldScript/include/ScriptObject.h"
#include "FieldScript/include/ScriptWork.h"

#include "System/include/GflUse.h"

#include "Battle/include/battle_def.h"

#include <arc_def_index/arc_def.h>

GFL_NAMESPACE_BEGIN( BattleInstTool )

#define PARA_EXP_TOTAL_MAX  ( 510 )
#define FRIEND_MONS_MAX     ( 216 )

//--------------------------------------------------------------
/**
 *  @brief ナンバーをビットに変換
 *  @param no ナンバー
 *  @retval ビット値
 */
//--------------------------------------------------------------
static u32 No2Bit( u32 no )
{
  if ( no >= 32 )
  {
    GFL_ASSERT( 0 );
    no = 31;
  }
  return ( 1 << ( no ) );
}

//--------------------------------------------------------------
/**
 * @brief   ポケモン作成　中でメモリアロケーションします
 * @param   mons_no   モンスターナンバー
 * @param   form_no   フォルムナンバー
 * @param   level     レベル
 * @param   pow       パワー乱数
 * @param   pHeap     ヒープ
 * @param   tokusei   特性指定  @fix BTS[1911]
 * @param   sex       性別指定  @fix BTS[2522]
 * @return  ポケモンパラム
 */
//--------------------------------------------------------------
static pml::pokepara::PokemonParam* CreatePokemon( MonsNo mons_no, u8 form_no, u8 level, u8 pow, gfl2::heap::HeapBase* pHeap, int tokusei, int sex )
{
  gfl2::math::Random           randContext;
  pml::pokepara::InitialSpec  initSpec;
  u32  personal_rand;

  randContext.Initialize( (u32)(pow + level + mons_no) );
  personal_rand = randContext.Next();

  initSpec.personal_rnd   = personal_rand;                        // 個性乱数
  initSpec.rare_rnd       = pml::pokepara::INIT_SPEC_RARE_FALSE;  // レア乱数
  initSpec.id             = personal_rand;                        // IDNo.
  initSpec.monsno         = mons_no;                              // モンスターNo.
  initSpec.formno         = form_no;                              // フォルムNo.
  initSpec.level          = level;                                // レベル

  //@note性別のないポケモンに性別渡ししても内部でコレクトしてくれる
  initSpec.sex = sex;     // 性別

  if ( tokusei < 0 )
  {
    initSpec.tokusei_index = System::GflUse::GetPublicRand(3);
  }
  else
  {
    if (tokusei >= 3 ) tokusei = 0;
    initSpec.tokusei_index = tokusei;
  }
  SAITA_PRINT("tokusei = %d\n",tokusei);
  // 個体値
  for( u32 i=0; i<pml::pokepara::POWER_NUM; i++ )
  {
    initSpec.talentPower[i] = pow;
  }

  return GFL_NEW_LOW( pHeap ) pml::pokepara::PokemonParam( pHeap, initSpec );
}

//--------------------------------------------------------------
/**
 * @brief   ポケモン作成　中でメモリアロケーションします
 * @param   pd        作成するポケモンデータポインタ
 * @param   level     レベル
 * @param   pow       パワー乱数
 * @param   pHeap     ヒープ
 * @param   tokusei   特性指定 @fix BTS[1911]
 * @param   sex  　   性別指定 @fix BTS[2522]
 * @return  ポケモンパラム
 */
//--------------------------------------------------------------
pml::pokepara::PokemonParam* MakePokemonParam(
    BattleInst::BINST_POKEMON_ROM_DATA *pd,
    int level,
    u8 pow,
    gfl2::heap::HeapBase* pHeap,
    int tokusei,
    int sex )
{
  int i;
  pml::pokepara::PokemonParam* pp;
  pp = CreatePokemon( (MonsNo)(pd->mons_no), pd->form_no, level, pow, pHeap, tokusei, sex );
  pp->SetItem( pd->item_no );
  for(i=0; i<PTL_WAZA_MAX; ++i)
  {
    pp->SetWaza( i, (WazaNo)(pd->waza[i]) );
  }
  //なつきど・性格
  {
    bool bFastMode = pp->StartFastMode();

    u8 friend_pow = (pp->HaveWaza(WAZANO_YATUATARI))? 0 : 255;
    pp->SetFamiliarity( friend_pow );
    pml::pokepara::Seikaku seikaku = (pml::pokepara::Seikaku)(pd->chr);
    pp->ChangeSeikaku( seikaku );  // 性格を変更する

    pp->EndFastMode( bFastMode );
  }

  //努力値
  {
    int exp = 0;
    // 努力値振り分けビットが０だと０除算が発生するため
    GFL_ASSERT_STOP(pd->exp_bit > 0);
    for(i=0;i<6;i++)
    {
      if( pd->exp_bit & No2Bit(i) )
      {
        exp++;
      }
    }

    GFL_ASSERT_STOP(exp > 0);
    if( (PARA_EXP_TOTAL_MAX/exp) > 255 )
    {
      exp=255;
    }
    else
    {
      exp=PARA_EXP_TOTAL_MAX/exp;
    }

    //@fix BTS[3927][3928]  ビットの並びとライブラリの列挙の並びが異なるため変換が必要
    for(i = 0;i < 6;i++)
    {
      if(pd->exp_bit&No2Bit(i))
      {
        bool set = true;
        pml::pokepara::PowerID id =  pml::pokepara::POWER_HP;
        //ビットデータを実インデックスに変換
        switch(i)
        {
        case 0://BIT HP
          id =  pml::pokepara::POWER_HP;
          break;
        case 1://BIT_ATK
          id =  pml::pokepara::POWER_ATK;
          break;
        case 2://BIT_DEF
          id =  pml::pokepara::POWER_DEF;
          break;
        case 3://BIT_AGI
          id =  pml::pokepara::POWER_AGI;
          break;
        case 4://BIT_SPATK
          id =  pml::pokepara::POWER_SPATK;
          break;
        case 5://BIT_APDEF
          id =  pml::pokepara::POWER_SPDEF;
          break;
        default:
          set = false;
        }
        if ( set )
        {
          pp->ChangeEffortPower( id, exp );  ///< 努力値を変更する
        }
      }
    }
  }

  return pp;
}

//--------------------------------------------------------------
/**
 * @brief  バトル施設トレーナーデータのサイズを取得する
 * @param   arc_id    アーカイブＩＤ
 * @param   data_idx  データインデックス
 */
//--------------------------------------------------------------
size_t GetTrainerRomDataSize( gfl2::heap::HeapBase* p_heap, gfl2::fs::AsyncFileManager* pAsyncFileManager, int arc_id, int data_idx )
{
  app::util::FileAccessor::FileOpenSync( arc_id, p_heap);

  size_t size = 0;
  pAsyncFileManager->GetArcFile( arc_id)->GetDataSize( &size, data_idx, NULL);

  app::util::FileAccessor::FileCloseSync( arc_id);

  return size;
}

//--------------------------------------------------------------
/**
 * @brief  バトル施設トレーナーデータを取得する
 * @param   arc_id    アーカイブＩＤ
 * @param   data_idx  データインデックス
 * @param   pd　格納バッファ
 */
//--------------------------------------------------------------
void GetTrainerRomData( gfl2::heap::HeapBase* p_heap, gfl2::fs::AsyncFileManager* pAsyncFileManager, int arc_id, int data_idx, BattleInst::BINST_TRAINER_ROM_DATA  *td, size_t size )
{
  app::util::FileAccessor::FileOpenSync( arc_id, p_heap);

  if( size > 0)
  {
    app::util::FileAccessor::FileLoadBufSync( arc_id, data_idx, reinterpret_cast<void*>( td), size, p_heap, false);
  }
  else
  {
    GFL_ASSERT_MSG( 0, "data size <= 0\n");
  }

  app::util::FileAccessor::FileCloseSync( arc_id);
}

//--------------------------------------------------------------
/**
 * @brief   バトル施設モンスターデータを取得する
 * @param   p_heap    ヒープ
 * @param   arc_id    アーカイブＩＤ
 * @param   data_idx  データインデックス
 * @param   pd　格納バッファ
 */
//--------------------------------------------------------------
void GetPokemonRomData( gfl2::heap::HeapBase* p_heap, gfl2::fs::AsyncFileManager* pAsyncFileManager, int arc_id, int data_idx, BattleInst::BINST_POKEMON_ROM_DATA  *pd )
{
  app::util::FileAccessor::FileOpenSync( arc_id, p_heap);

  size_t size = 0;
  pAsyncFileManager->GetArcFile( arc_id)->GetDataSize( &size, data_idx, NULL);
  if( size > 0)
  {
    app::util::FileAccessor::FileLoadBufSync( arc_id, data_idx, reinterpret_cast<void*>( pd), size, p_heap, false);
  }
  else
  {
    GFL_ASSERT_MSG( 0, "data size <= 0\n");
  }

  app::util::FileAccessor::FileCloseSync( arc_id);
}

GFL_NAMESPACE_END( BattleInstTool )

