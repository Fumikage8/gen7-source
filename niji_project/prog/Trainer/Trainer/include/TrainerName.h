//-----------------------------------------------------------------------------
/**
 * @file   TrainerName.h
 * @brief  トレーナー名
 * @author obata_toshihiro
 * @date   2015.06.12
*/
//-----------------------------------------------------------------------------
#ifndef TRAINERNAME_H_INCLUDED
#define TRAINERNAME_H_INCLUDED

#include <heap/include/gfl2_Heap.h>
#include <str/include/gfl2_Str.h>

#include <niji_conv_header/trainer/trid_def.h>


namespace trainer {


  class TrainerName
  {
  public:

    //-------------------------------------------------------------------------
    /**
     * @brief  トレーナーの名前を取得する
     *
     * @param[out] destBuffer   取得した名前の格納先
     * @param[in]  heap         取得に使用するヒープ
     * @param[in]  trainerId    取得対象トレーナーのID
     */
    //-------------------------------------------------------------------------
    static void GetTrainerName( gfl2::str::StrBuf* dst, gfl2::heap::HeapBase* heap, TrainerID trainerId );


  };


} // namespace trainer

#endif // TRAINERNAME_H_INCLUDED