/**
* @file    FieldCameraUtility.h
* @brief   フィールド上のカメラ便利関数群
* @author  ikuta_junya
* @date    2014.09.26
*/

#if !defined( FIELDCAMERAUTILITY_H_INCLUDED )
#define FIELDCAMERAUTILITY_H_INCLUDED
#pragma once

// gfl2
#include <math/include/gfl2_Vector.h>
#include <heap/include/gfl2_Heap.h>

#include "FieldCameraTypes.h"


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );

void ConvertDegreeToRadian( gfl2::math::Vector* pVec );
void ConvertRadianToDegree( gfl2::math::Vector* pVec );
u32 GetLimitLineRotate( const gfl2::math::Vector& rCenter, f32 radius, f32 x, f32* pOut1, f32* pOut2 );


const c8* GetLabel( CoordinateSystemType type );


/**
* @brief   デフォルトカメラの設定データを生成する
*
* @param   pHeap                      生成に使用するヒープ
*
* @return  無し
*/
Field::Camera::CAMERA_DATA* CreateDefaultCameraData( gfl2::heap::HeapBase* pHeap );


GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );

#endif // #if !defined( FIELDCAMERAUTILITY_H_INCLUDED )

