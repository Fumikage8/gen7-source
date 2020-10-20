//-----------------------------------------------------------------------------
/**
 * @file   TrainerTypeName.h
 * @brief  トレーナータイプ名( たんぱんこぞう、ミニスカート　など )
 * @author obata_toshihiro
 * @date   2015.06.15
*/
//-----------------------------------------------------------------------------
#ifndef TRAINERTYPENAME_H_INCLUDED
#define TRAINERTYPENAME_H_INCLUDED

#include <heap/include/gfl2_Heap.h>
#include <str/include/gfl2_Str.h>

#include <niji_conv_header/trainer/trtype_def.h>


namespace trainer {


  class TrainerTypeName
  {
  public:

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
    static void GetTrainerTypeName( gfl2::str::StrBuf* destBuffer, u8* grammer, gfl2::heap::HeapBase* heap, TrType trainerType );


  };


} // namespace trainer

#endif // TRAINERTYPENAME_H_INCLUDED