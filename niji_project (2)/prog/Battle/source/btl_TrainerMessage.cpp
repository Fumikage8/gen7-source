//=============================================================================
/**
 * @file    btl_TrainerMessage.cpp
 * @brief   戦闘中に表示するトレーナーメッセージの管理
 * @author  obata_toshihiro
 * @date    2015.03.11
 */
//=============================================================================
#include <Trainer/Trainer/include/tr_tool.h>
#include <Trainer/Trainer/include/TrainerMessage.h>

#include "btl_TrainerMessage.h"


GFL_NAMESPACE_BEGIN( btl )


//-----------------------------------------------------------------------------
/**
 * @brief メッセージ文字列を取得する
 * @param[out] pDest           取得したメッセージの格納先
 * @param[in]  pTemporaryHeap  一時的に使用するヒープ
 * @param[in]  trainerId       メッセージ取得対象のトレーナーID
 * @param[in]  messageId       取得対象メッセージのID
 */
//-----------------------------------------------------------------------------
void TrainerMessage::GetString( 
  gfl2::str::StrBuf* pDest,
  gfl2::heap::HeapBase* pTemporaryHeap,
  trainer::TrainerID trainerId,
  MessageID messageId )
{
  trainer::MessageKind messageKind = TrainerMessage::GetTrainerMessageKind( messageId );
  trainer::TrainerMessage::GetString( pDest, trainerId, messageKind, pTemporaryHeap );
}

//---------------------------------------------------------------------------
/**
 * @brief メッセージのメッセージデータ内IDを取得する
 * @param[in]  pTemporaryHeap  一時的に使用するヒープ
 * @param[in]  trainerId       メッセージ取得対象のトレーナーID
 * @param[in]  messageId       取得対象メッセージのID
 *
 * @retval -1  メッセージが存在しない場合
 */
//---------------------------------------------------------------------------
s32 TrainerMessage::GetDataId( 
    gfl2::heap::HeapBase* pTemporaryHeap,
    trainer::TrainerID trainerId,
    MessageID messageId )
{
  trainer::MessageKind messageKind = TrainerMessage::GetTrainerMessageKind( messageId );
  s32 datid = trainer::TrainerMessage::GetStringID( trainerId, messageKind, pTemporaryHeap );
  return datid;
}

//---------------------------------------------------------------------------
/**
 * @brief メッセージが存在するか？
 * @param[in]  pTemporaryHeap  一時的に使用するヒープ
 * @param[in]  trainerId       メッセージ取得対象のトレーナーID
 * @param[in]  messageId       取得対象メッセージのID
 */
//---------------------------------------------------------------------------
bool TrainerMessage::IsExist( 
    gfl2::heap::HeapBase* pTemporaryHeap,
    trainer::TrainerID trainerId,
    MessageID messageId )
{
  s32 datid = TrainerMessage::GetDataId( pTemporaryHeap, trainerId, messageId );
  return ( 0 <= datid );
}

/**
 * @brief 戦闘中のトレーナーメッセージIDから、対応するトレーナーデータのメッセージIDを取得する
 * @param messageId  戦闘中のトレーナーメッセージID
 * @retval trainer::TRMSG_NONE  対応するメッセージIDが存在しない場合
 */
trainer::MessageKind TrainerMessage::GetTrainerMessageKind( btl::TrainerMessage::MessageID messageId )
{
  switch( messageId )
  {
  case TrainerMessage::MESSAGE_FIRST_ATTACK_DONE:  return trainer::TRMSG_FIGHT_FIRST_DAMAGE;
  case TrainerMessage::MESSAGE_LAST_POKE_IN:       return trainer::TRMSG_FIGHT_POKE_LAST;
  case TrainerMessage::MESSAGE_LAST_POKE_HP_HALF:  return trainer::TRMSG_FIGHT_POKE_LAST_HP_HALF;
  case TrainerMessage::MESSAGE_ZENRYOKU:           return trainer::TRMSG_FIGHT_USE_ZENRYOKUWAZA;
  default:
    GFL_ASSERT(0);
    return trainer::TRMSG_NONE;
  }
}

GFL_NAMESPACE_END( btl )