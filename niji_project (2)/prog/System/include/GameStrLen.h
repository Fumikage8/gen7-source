//===================================================================
/**
 * @file    GameStrLen.h
 * @brief   ゲームで使う様々な文字列長の定義
 * @author  taya
 * @date    2011.5.13
*/
//===================================================================
#ifndef __GAMESYSTEM_STRLEN_H__
#define __GAMESYSTEM_STRLEN_H__

#include <macro/include/gfl2_Macros.h>
#include <pml/include/pml_Const.h>

GFL_NAMESPACE_BEGIN(System)

  /**
   *  以下に定義されているものは、バイト数ではなく「文字数」の最大値です。
   *  データをシリアライズ（セーブデータ化・通信データ化）する必要がある場合などに、お使いください。
   */
  enum {
    STRLEN_POKEMON_NAME = pml::MONS_NAME_LENGTH,     ///< ポケモンの名称・ニックネーム
    STRLEN_PLAYER_NAME  = pml::PERSON_NAME_LENGTH,   ///< プレイヤー入力名、プレイヤーニックネーム
    STRLEN_TRAINER_NAME = pml::PERSON_NAME_LENGTH,   ///< ゲーム内トレーナー名
    STRLEN_PERSON_NAME  = pml::PERSON_NAME_LENGTH,   ///< プレイヤー入力名・トレーナー名をどちらも格納できる十分な長さ
    STRLEN_ITEM_NAME    = 16,   ///< どうぐ名
    STRLEN_PSS_MESSAGE  = 16,   ///< PSS一言メッセージ
    STRLEN_SECRET_BASE_MESSAGE = 16,  ///< ひみつきちフリーワード
    STRLEN_TRIAL_HOUSE_TITLE = 22,  ///<トライアルハウス検定名
    STRLEN_TRAINER_TYPE = 20,       ///<トレーナー肩書き名
    STRLEN_INTERVIEW_WORD_LONG = 16,  ///< インタビュー：一言メッセージ（長い）
    STRLEN_INTERVIEW_WORD_SHORT = 16,  ///< インタビュー：一言メッセージ（短い）
    STRLEN_INTERVIEW_WORD_VERY_SHORT = 12,  ///< インタビュー：一言メッセージ（6文字）
    STRLEN_FREEWORD_MAX = 16, ///<フリーワード系文字列の最大長(スクリプトコマンドで利用する定義)
    EOM_LEN = 1,  ///< EOM
  };


GFL_NAMESPACE_END(System)


#endif /* for include guard */

