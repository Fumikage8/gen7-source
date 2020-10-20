//-----------------------------------------------------------------------------
/**
 * @file   TrainerName.cpp
 * @brief  トレーナー名
 * @author obata_toshihiro
 * @date   2015.06.12
*/
//-----------------------------------------------------------------------------
#include <Print/include/PrintSystem.h>
#include <Trainer/Trainer/include/TrainerName.h>

#include <arc_index/message.gaix>


namespace trainer {



  //-------------------------------------------------------------------------
  /**
    * @brief  トレーナーの名前を取得する
    *
    * @param[out] destBuffer   取得した名前の格納先
    * @param[in]  heap         取得に使用するヒープ
    * @param[in]  trainerId    取得対象トレーナーのID
    */
  //-------------------------------------------------------------------------
  void TrainerName::GetTrainerName( gfl2::str::StrBuf* destBuffer, gfl2::heap::HeapBase* heap, TrainerID trainerId )
  {
    if( TRID_MAX <= trainerId ) 
    {
      GFL_ASSERT(0);         // 引数が不正値
      trainerId = TRID_NULL; // ダミーデータに差し替える
    }

    const s32 messageId = trainerId;
    gfl2::heap::HeapBase* lowerHeap = heap->GetLowerHandle();
    gfl2::str::MsgData* messageData = GFL_NEW( lowerHeap ) gfl2::str::MsgData( print::GetMessageArcId(), GARC_message_trname_DAT, lowerHeap );
    messageData->GetString( messageId, *destBuffer );
    GFL_SAFE_DELETE( messageData );
  }



} // namespace trainer