//-----------------------------------------------------------------------------
/**
 * @file   TrainerMessage.cpp
 * @brief  トレーナーメッセージ
 * @author obata_toshihiro
 * @date   2015.05.30
*/
//-----------------------------------------------------------------------------
#include <Print/include/PrintSystem.h>
#include <AppLib/include/Util/app_util_FileAccessor.h>
#include <Trainer/Trainer/include/TrainerMessage.h>

#include <arc_def_index/arc_def.h>
#include <arc_index/message.gaix>
#include <niji_conv_header/trainer/trid_def.h>



namespace trainer {


  namespace {


    /**
     * @brief トレーナー毎のインデックスデータ
     */
    struct IndexData
    {
      u8  messageKindNum;  // 0x00 トレーナーに登録されているメッセージの、種類数
      u8  padding;         // 0x01
      u16 headDescIndex;   // 0x02 トレーナーに登録されているメッセージの、設定データの先頭インデックス
    };


    /**
     * @brief メッセージ毎の設定データ
     */
    struct MessageDesc
    {
      u8  kind;       // 0x00 メッセージ種類( TRMSG_xxxx )
      u8  padding;    // 0x01
      u16 stringId;   // 0x02 文字列ID
    };


    /**
     * @brief トレーナー毎のインデックスデータを読み込む
     * @param[out] outIndexData  読み込んだインデックスデータの格納先
     * @param[in]  trainerId     読み込むトレーナーのID
     * @param[in]  heapForWork   作業ワークとして使用するヒープ
     * @retval true   インデックスデータを読み込んだ
     * @retval false  インデックスデータが存在しない
     */
    bool LoadIndexData( IndexData* outIndexData, TrainerID trainerId, gfl2::heap::HeapBase* heapForWork )
    {
      if( TRID_MAX <= trainerId ) {
        GFL_ASSERT(0);
        return false;
      }

      static const bool CLOSE_SOON = true;
      static const bool IS_COMPRESSED = false;
      gfl2::heap::HeapBase* heap = heapForWork->GetLowerHandle();
      app::util::FileAccessor::FileOpenSync( ARCID_TRAINER_MESSAGE_INDEX, heap, CLOSE_SOON );
      if( !( app::util::FileAccessor::IsFileOpen( ARCID_TRAINER_MESSAGE_INDEX ) ) ) {
        GFL_ASSERT(0);
        return false;
      }

      app::util::FileAccessor::FileLoadBufSync( ARCID_TRAINER_MESSAGE_INDEX, trainerId, static_cast<void*>( outIndexData ), sizeof(IndexData), heap, IS_COMPRESSED );
      app::util::FileAccessor::FileCloseSync( ARCID_TRAINER_MESSAGE_INDEX );
      return true;
    }

    /**
     * @brief トレーナーメッセージの設定データを読み込む
     * @param[out] outMessageDesc  読み込んだ設定データの格納先
     * @param[in]  indexData       読み込み対象の、インデックスデータ
     * @param[in]  messageKind     読み込み対象の、メッセージの種類
     * @param[in]  heapForWork     作業ワークとして使用するヒープ
     * @retval true   設定データを読み込んだ
     * @retval false  設定データが存在しない
     */
    bool LoadMessageDesc( MessageDesc* outMessageDesc, const IndexData& indexData, trainer::MessageKind messageKind, gfl2::heap::HeapBase* heapForWork )
    {
      if( trainer::TRMSG_KIND_MAX <= messageKind ) {
        GFL_ASSERT(0);
        return false;
      }

      static const bool CLOSE_SOON = true;
      static const bool IS_COMPRESSED = false;
      gfl2::heap::HeapBase* heap = heapForWork->GetLowerHandle();
      app::util::FileAccessor::FileOpenSync( ARCID_TRAINER_MESSAGE_DESC, heap, CLOSE_SOON );
      if( !( app::util::FileAccessor::IsFileOpen( ARCID_TRAINER_MESSAGE_DESC ) ) ) {
        GFL_ASSERT(0);
        return false;
      }

      bool isFindTarget = false;
      for( u32 i=0; i<indexData.messageKindNum; ++i )
      {
        u32 dataIndex = indexData.headDescIndex + i;
        app::util::FileAccessor::FileLoadBufSync( ARCID_TRAINER_MESSAGE_DESC, dataIndex, static_cast<void*>( outMessageDesc ), sizeof(MessageDesc), heap, IS_COMPRESSED );
        if( outMessageDesc->kind == messageKind ) {
          isFindTarget = true;
          break;
        }
      }

      app::util::FileAccessor::FileCloseSync( ARCID_TRAINER_MESSAGE_DESC );
      return isFindTarget;
    }

  }


  //-------------------------------------------------------------------------
  /**
   * @brief  トレーナーメッセージデータを取得
   *
   * @param[out] destBuffer   取得したメッセージの格納先
   * @param[in]  trainerId    取得したいメッセージのトレーナーID
   * @param[in]  messageKind  取得したいメッセージの種類
   * @param[in]  heapForWork  テンポラリ領域として使用するヒープ
   *
   * @retval true   メッセージを読み込んだ場合
   * @retval false  メッセージが存在しない場合
   */
  //-------------------------------------------------------------------------
  bool TrainerMessage::GetString( gfl2::str::StrBuf* destBuffer, TrainerID trainerId, trainer::MessageKind messageKind, gfl2::heap::HeapBase* heapForWork )
  {
    s32 stringId = TrainerMessage::GetStringID( trainerId, messageKind, heapForWork );
    if( stringId < 0 ) {
      return false;
    }

    gfl2::heap::HeapBase* heap = heapForWork->GetLowerHandle();
    gfl2::str::MsgData* messageData = GFL_NEW( heap ) gfl2::str::MsgData( print::GetMessageArcId(), GARC_message_trmsg_DAT, heap );
    messageData->GetString( stringId, *destBuffer );
    GFL_SAFE_DELETE( messageData );
    return true;
  }

  //-------------------------------------------------------------------------
  /**
   * @brief トレーナーメッセージの DATID を取得
   *
   * @param[in]  trainerId    取得したいトレーナーID
   * @param[in]  messageKind  取得したいメッセージの種類
   * @param[in]  heapForWork  テンポラリ領域として使用するヒープ
   *
   * @retval -1  メッセージデータが存在しない場合
   */
  //-------------------------------------------------------------------------
  s32 TrainerMessage::GetStringID( TrainerID trainerId, trainer::MessageKind messageKind, gfl2::heap::HeapBase* heapForWork )
  {
    if( ( TRID_MAX <= trainerId ) ||
        ( TRMSG_KIND_MAX <= messageKind ) ) {
      GFL_ASSERT(0);
      return -1;
    }

    IndexData indexData;
    if( !( LoadIndexData( &indexData, trainerId, heapForWork ) ) ) {
      return -1;
    }

    MessageDesc messageDesc;
    if( !( LoadMessageDesc( &messageDesc, indexData, messageKind, heapForWork ) ) ) {
      return -1;
    }

    return messageDesc.stringId;
  }


} // end of namespace 'trainer'

