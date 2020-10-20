#if !defined(GFL2_FSCALC_H_INCLUDED)
#define GFL2_FSCALC_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    gfl2_FsCalc.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.03.31
 @brief   ファイルシステムで扱う計算
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


// fsのインクルード
#include <fs/include/gfl2_FsToolDefine.h>
#include <fs/include/gfl2_FsDefine.h>
#include <fs/include/gfl2_FsResult.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


//==============================================================================
/**
 @class     Calc
 @brief     ファイルシステムで扱う計算
 */
//==============================================================================
class Calc
{
  GFL_FORBID_COPY_AND_ASSIGN(Calc);

public:
  //! @brief  数値が2のべき乗であるか
  //! @param[in] val  数値
  //! @return  valが2のべき乗ならtrueを返す。
  //!          valが2のべき乗でないときfalseを返す。valが0のときfalseを返す。
  static bool IsPowerOf2(u32 val);

  //! @brief  数値をアラインメント調整する
  //! @param[in] val    数値
  //! @param[in] align  アラインメント。2のべき乗であること。2のべき乗か否かのチェックはこの関数内ではしていません。
  //! @return  valをalignでアラインメント調整した数値を返す。
  static u32 AlignVal(u32 val, u32 align);

public:
  //! @brief  ループ中に行うスリープ
  static void SleepInLoop(void);

public:
  //! @brief  「T_TYPE型のsizeof」+「余分なサイズ」を得る
  template<typename T_TYPE>
  static size_t GetTypeSizeIncludedExtraSize(void)
  {
    return ( sizeof(T_TYPE) + Define::MEM_REQUIRED_EXTRA_SIZE );
  }
};


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GFL2_FSCALC_H_INCLUDED
