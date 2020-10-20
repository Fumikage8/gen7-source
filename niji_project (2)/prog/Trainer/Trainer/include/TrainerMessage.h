//-----------------------------------------------------------------------------
/**
 * @file   TrainerMessage.h
 * @brief  トレーナーメッセージ
 * @author obata_toshihiro
 * @date   2015.05.30
*/
//-----------------------------------------------------------------------------
#ifndef TRAINERMESSAGE_H_INCLUDED
#define TRAINERMESSAGE_H_INCLUDED

#include <heap/include/gfl2_Heap.h>
#include <str/include/gfl2_Str.h>

#include <niji_conv_header/trainer/trid_def.h>
#include <niji_conv_header/trainer/trtype_def.h>
#include <niji_conv_header/trainer/TrainerMessageKind.h>


namespace trainer {


  class TrainerMessage
  {
  public:

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
    static bool GetString( gfl2::str::StrBuf* dst, TrainerID trainerId, trainer::MessageKind messageKind, gfl2::heap::HeapBase* heapForWork );

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
    static s32 GetStringID( TrainerID trainerId, trainer::MessageKind messageKind, gfl2::heap::HeapBase* heapForWork );


  };


} // namespace trainer

#endif // TRAINERMESSAGE_H_INCLUDED