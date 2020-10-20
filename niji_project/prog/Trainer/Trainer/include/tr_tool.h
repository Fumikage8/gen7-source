//============================================================================================
/**
 * @file  tr_tool.h
 * @brief トレーナーデータ処理ツール群
 * @author obata_toshihiro
 */
//============================================================================================
#ifndef __TR_TOOL_H__
#define __TR_TOOL_H__

#include <heap/include/gfl2_Heap.h>
#include <pml/include/pmlib.h>

#include <niji_conv_header/trainer/trid_def.h>
#include <niji_conv_header/trainer/trtype_def.h>

#include <PokeTool/include/PokeTool.h>


class BSP_TRAINER_DATA;

namespace Field {
  class EventWork;
}

namespace trainer {


//-----------------------------------------------------------------------------
/**
 * @brief 通常エンカウントトレーナーのバトルセットアップ用データを構築
 *
 * @param[in]  trainerId          トレーナーID
 * @param[out] battleTrainerData  セットアップ対象のトレーナーデータ
 * @param[in]  tempHeap           作業用ヒープ
 */
//-----------------------------------------------------------------------------
void EncountTrainerPersonalDataMake( TrainerID trainerId, BSP_TRAINER_DATA* battleTrainerData, gfl2::heap::HeapBase* tempHeap );

//-----------------------------------------------------------------------------
/**
 * @brief 通常エンカウントトレーナーのポケモンパーティデータを構築
 *
 * @param[in]  trainerId  トレーナーID
 * @param[out] pokeParty  パーティ格納クラス
 * @param[in]  heap       ポケモンパラメータの生成に使用するヒープ
 */
//-----------------------------------------------------------------------------
void EncountTrainerPokeDataMake( TrainerID trainerId, pml::PokeParty* pokeParty, gfl2::heap::HeapBase* heap );

//-----------------------------------------------------------------------------
/**
 * @brief トレーナーフラグを取得
 *
 * @param trainerId      トレーナーID
 * @param fieldEventWork イベントワークへのポインタ
 *
 * @retval true   対戦済み
 * @retval false  対戦前
 */
//-----------------------------------------------------------------------------
bool GetTrainerFlag( TrainerID trainerId, const Field::EventWork* fieldEventWork );



//-----------------------------------------------------------------------------
/**
 * @brief トレーナーデータパラメータ
 */
//-----------------------------------------------------------------------------
struct TRAINER_DATA
{
  u16 tr_type;              // 0x00 トレーナー分類
  u8  fight_type;           // 0x02 1vs1 or 2vs2 or 3vs3 or rotate : BTL_RULE_XXX : include/battle/battle_def.h
  u8  poke_count;           // 0x03 所持ポケモン数
  u16 use_item[4];          // 0x04 使用道具
  u32 aibit;                // 0x0c AIパターン
  u8  hp_recover_flag :1;   // 0x10 戦闘後回復するか？
  u8                  :7;   // 0x10
  u8  gold;                 // 0x11 おこづかい係数
  u16 gift_item;            // 0x12 戦闘後もらえるアイテム
};

//-----------------------------------------------------------------------------
/**
 * @briefトレーナーデータを読み込む
 *
 * @param[in]  tempHeap     アーカイブ一時オープン用ヒープハンドル
 * @param[in]  trainerId    取得したいトレーナーID
 * @param[out] trainerData  取得したトレーナーデータの格納先を指定
 */
//-----------------------------------------------------------------------------
void LoadTrainerData( gfl2::heap::HeapBase* tempHeap, TrainerID trainerId, TRAINER_DATA* trainerData );

//-----------------------------------------------------------------------------
/**
 * @brief 指定IDのトレーナーデータとポケモンデータを取得
 *
 * @param tr_id トレーナーIDのテーブル
 * @param dst   トレーナーデータ格納場所
 * @param psp   ポケモンデータ格納場所
 * @param pHeap ヒープ
 *
 * ・マップナビで使用
 * ・ファイルのオープン/クローズをなるべく１回で済むようにした
 */
//-----------------------------------------------------------------------------
void GetEncountTrainerData( TrainerID * tr_id, BSP_TRAINER_DATA ** dst, PokeTool::SimpleParam * psp, u32 max, gfl2::heap::HeapBase * pHeap );

//-----------------------------------------------------------------------------
/**
 * @brief 2人分のトレーナータイプからタッグトレーナーかチェックし、タッグ用のトレーナータイプを返す。
 *        ふたごちゃん等をメッセージで分岐するため
 *
 * @param trtype1 １人目のトレーナータイプ
 * @param trtype2 ２人目のトレーナータイプ
 *
 * @return int -1の場合タッグトレーナーではない。それ以外はタイプが返ってくる
 */
//-----------------------------------------------------------------------------
int GetTagTrainerType( u16 trtype1 , u16 trtype2 );


} // end of namespace 'trainer'


#endif
