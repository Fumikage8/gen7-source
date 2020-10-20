//-----------------------------------------------------------------------------
/**
 * @file   TrainerTypeName.cpp
 * @brief  トレーナータイプ名( たんぱんこぞう、ミニスカート　など )
 * @author obata_toshihiro
 * @date   2015.06.15
*/
//-----------------------------------------------------------------------------
#include <Print/include/PrintSystem.h>
#include <Trainer/Trainer/include/TrainerTypeName.h>

#include <arc_index/message.gaix>


namespace trainer {



  //-------------------------------------------------------------------------
  /**
    * @brief  トレーナーの名前を取得する
    *
    * @param[out] destBuffer   取得した名前の格納先
    * @param[out] grammar      取得した名前の文法特性( 性、単複、最初の音、加算性 )
    * @param[in]  heap         取得に使用するヒープ
    * @param[in]  trainerType  取得対象トレーナータイプ
    */
  //-------------------------------------------------------------------------
  void TrainerTypeName::GetTrainerTypeName( 
    gfl2::str::StrBuf*    destBuffer, 
    u8*                   grammar, 
    gfl2::heap::HeapBase* heap,
    TrType                trainerType )
  {
    if( TRTYPE_MAX <= trainerType ) 
    {
      GFL_ASSERT(0); // 引数が不正値
      return;
    }

    const s32 messageId = trainerType;
    gfl2::heap::HeapBase* lowerHeap = heap->GetLowerHandle();
    gfl2::str::MsgData* messageData = GFL_NEW( lowerHeap ) gfl2::str::MsgData( print::GetMessageArcId(), GARC_message_trtype_DAT, lowerHeap );
    *grammar = messageData->GetString( messageId, *destBuffer );
    GFL_SAFE_DELETE( messageData );
  }



} // namespace trainer