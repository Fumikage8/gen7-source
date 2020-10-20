//=============================================================================
/**
 * @file    btl_TrainerMessage.h
 * @brief   トレーナーメッセージ
 * @author  obata_toshihiro
 * @date    2015.03.10
 */
//=============================================================================
#if !defined( BTL_TRAINERMESSAGE_H_INCLUDED )
#define BTL_TRAINERMESSAGE_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <str/include/gfl2_Str.h>

#include <Trainer/Trainer/include/tr_tool.h>
#include <Trainer/Trainer/include/TrainerMessage.h>

#include "btl_const.h"


GFL_NAMESPACE_BEGIN( btl )


/**
 * @class TrainerMessage
 * @brief トレーナーメッセージ
 */
class TrainerMessage
{
public:

  /**
   * @brief 戦闘中に表示するトレーナーメッセージ一覧
   */
  enum MessageID
  {
    MESSAGE_FIRST_ATTACK_DONE,   // 最初のわざを出した後
    MESSAGE_LAST_POKE_IN,        // 最後のポケモンを出す直前
    MESSAGE_LAST_POKE_HP_HALF,   // 最後のポケモンのHPが半分以下になった時
    MESSAGE_ZENRYOKU,            // ぜんりょくわざを出す直前
    MESSAGE_NUM,
  };

  //---------------------------------------------------------------------------
  /**
   * @brief メッセージ文字列を取得する
   * @param[out] pDest           取得したメッセージの格納先
   * @param[in]  pTemporaryHeap  一時的に使用するヒープ
   * @param[in]  trainerId       メッセージ取得対象のトレーナーID
   * @param[in]  messageId       取得対象メッセージのID
   */
  //---------------------------------------------------------------------------
  static void GetString( 
    gfl2::str::StrBuf* pDest,
    gfl2::heap::HeapBase* pTemporaryHeap,
    trainer::TrainerID trainerId,
    MessageID messageId );

  //---------------------------------------------------------------------------
  /**
   * @brief メッセージのメッセージデータ内IDを取得する
   * @param[in]  pTemporaryHeap  一時的に使用するヒープ
   * @param[in]  trainerId       メッセージ取得対象のトレーナーID
   * @param[in]  messageId       取得対象メッセージのID
   *
   * @retval -1  メッセージデータが存在しない場合
   */
  //---------------------------------------------------------------------------
  static s32 GetDataId( 
    gfl2::heap::HeapBase* pTemporaryHeap,
    trainer::TrainerID trainerId,
    MessageID messageId );

  //---------------------------------------------------------------------------
  /**
   * @brief メッセージが存在するか？
   * @param[in]  pTemporaryHeap  一時的に使用するヒープ
   * @param[in]  trainerId       メッセージ取得対象のトレーナーID
   * @param[in]  messageId       取得対象メッセージのID
   */
  //---------------------------------------------------------------------------
  static bool IsExist( 
    gfl2::heap::HeapBase* pTemporaryHeap,
    trainer::TrainerID trainerId,
    MessageID messageId );

  //---------------------------------------------------------------------------
  /**
   * @brief 戦闘中のトレーナーメッセージIDから、対応するトレーナーデータのメッセージIDを取得する
   * @param messageId  戦闘中のトレーナーメッセージID
   * @retval trainer::TRMSG_NONE  対応するメッセージIDが存在しない場合
   */
  //---------------------------------------------------------------------------
  static trainer::MessageKind GetTrainerMessageKind( btl::TrainerMessage::MessageID );


private:
  TrainerMessage( void );
  virtual ~TrainerMessage();
};


GFL_NAMESPACE_END( btl )

#endif  // BTL_TRAINERMESSAGE_H_INCLUDED