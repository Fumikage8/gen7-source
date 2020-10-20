//======================================================================
/**
 * @file FieldSodateyaWalkCheckData.h
 * @date 2015/12/15 19:15:59
 * @author onoue_masayuki
 * @brief 育てや歩数チェックデータ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELDSODATEYAWALKCHECKDATA_H_INCLUDED__
#define __FIELDSODATEYAWALKCHECKDATA_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN(Field)

  /**
   * @struct FieldSodateyaWalkCheckData
   * @brief  構造体の説明
   */
  struct FieldSodateyaWalkCheckData
  {
      //! 育て屋専用歩数カウンター
      u32 mSodateyaWalkCount;
  };

GFL_NAMESPACE_END(mynamespace)

#endif // __FIELDSODATEYAWALKCHECKDATA_H_INCLUDED__