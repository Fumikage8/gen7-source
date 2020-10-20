//=============================================================================
/**
 * @file   pml_PersonalUtil.h
 * @date   2015/12/28 15:10:38
 * @author obata_toshihiro
 * @brief  簡単な説明
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined PML_PERSONALUTIL_H_INCLUDED
#define PML_PERSONALUTIL_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <pml/include/pml_Type.h>
#include "./pml_PersonalConst.h"


GFL_NAMESPACE_BEGIN( pml )
GFL_NAMESPACE_BEGIN( personal )


/**
 * @brief パーソナルデータ関連のユーティリティ
 */
class Util
{
public:

  //---------------------------------------------------------------------------
  /**
   * @brief 指定した地方種のフォルムNo.を取得する
   * @param[out] formArray  取得したフォルムNo.の格納先( 要素数が MAX_FORM_NUM 以上である必要があります )
   * @param[out] formNum    取得したフォルムNo.の数
   * @param      monsno     取得対象のモンスターNo.
   * @param      regionId   取得対象の地方種ID
   */
  //---------------------------------------------------------------------------
  static void GetRegionFormNo( FormNo* formArray, u32* formNum, MonsNo monsno, RegionID regionId );

};


GFL_NAMESPACE_END( personal )
GFL_NAMESPACE_END( pml )

#endif // PML_PERSONALUTIL_H_INCLUDED