#if defined(GF_PLATFORM_WIN32)

/**
* @file    SkeltonCameraUtility.h
* @brief   フィールド上のカメラ便利関数群
* @author  ikuta_junya
* @date    2014.09.26
*/

#if !defined( FIELDCAMERAUTILITY_H_INCLUDED )
#define FIELDCAMERAUTILITY_H_INCLUDED
#pragma once

#include <math/include/gfl2_Vector4.h>
#include "SkeltonCameraType.h"


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Skelton)

void ConvertDegreeToRadian( gfl2::math::Vector4* pVec );
void ConvertRadianToDegree( gfl2::math::Vector4* pVec );
u32 GetLimitLineRotate( const gfl2::math::Vector4& rCenter, f32 radius, f32 x, f32* pOut1, f32* pOut2 );


const c8* GetLabel( CoordinateSystemType type );

GFL_NAMESPACE_END(Skelton)
GFL_NAMESPACE_END(Test)

#endif
#endif // GF_PLATFORM_WIN32