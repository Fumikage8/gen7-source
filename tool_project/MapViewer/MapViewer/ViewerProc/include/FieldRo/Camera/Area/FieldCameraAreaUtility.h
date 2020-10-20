//======================================================================
/**
* @file FieldCameraAreaUtility.h
* @date 2015/07/03 18:52:30
* @author ikuta_junya
* @brief フィールドカメラエリア関連のUtil関数定義
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#if !defined __FIELDCAMERAAREAERUTIL_H_INCLUDED__
#define __FIELDCAMERAAREAERUTIL_H_INCLUDED__
#pragma once

#include "ViewerProc/include/FieldRo/Camera/Area/FieldCameraAreaTypes.h"


// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Area );

class Utility
{
public:

  // カメラパラメーターの補間
  static void Lerp( MAIN_PARAM* pOut, const MAIN_PARAM& r1, const MAIN_PARAM& r2, f32 t );

  static void ConvertDegToRad( gfl2::math::Vector3* pOut )
  {
    pOut->x = gfl2::math::ConvDegToRad( pOut->x );
    pOut->y = gfl2::math::ConvDegToRad( pOut->y );
    pOut->z = gfl2::math::ConvDegToRad( pOut->z );
  }

  static void ConvertDegToRad( MAIN_PARAM* pOut )
  {
    pOut->fovY = gfl2::math::ConvDegToRad( pOut->fovY );
    ConvertDegToRad( &pOut->rotation );
  }

};

GFL_NAMESPACE_END( Area );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );


#endif // __FIELDCAMERAAREAERUTIL_H_INCLUDED__