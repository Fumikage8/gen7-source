#if defined(GF_PLATFORM_WIN32)

/**
* @file    SkeltonCameraType.h
* @brief   フィールド上のカメラ
* @author  ikuta_junya
* @date    2014.09.26
*/

#if !defined( FIELDCAMERATYPE_H_INCLUDED )
#define FIELDCAMERATYPE_H_INCLUDED
#pragma once

#include <math/include/gfl2_math.h>

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Skelton)

enum Type
{
  TYPE_RELATIVE_ROTATE = 0,
  TYPE_RELATIVE_ROTATE_RATIO,
  TYPE_STOP
};

enum CoordinateSystemType
{
  COORDINATE_SYSTEM_TYPE_WORLD = 0,    ///< ワールド座標系
  COORDINATE_SYSTEM_TYPE_LOCAL,        ///< ローカル座標系
};

static const u32 CAMERA_ID_DEFAULT( 0xFE );

struct PROJECTION_DATA
{
  f32 nearZ;
  f32 farZ;
  // f32 aspect;
  f32 aov;
};

struct UNIQUE_INTERPOLATION_DATA
{
  c8                  name[512];          // 識別名(今はmesh名) @todo id化
  u32                 frame;
};

// 制限情報
struct LIMIT_DATA
{
  gfl2::math::Vector4  min;
  gfl2::math::Vector4  max;

  // 各リミット値が有効なら1、無効なら0
  u8                  enableMinX : 1;
  u8                  enableMinY : 1;
  u8                  enableMinZ : 1;
  u8                  enableMaxX : 1;
  u8                  enableMaxY : 1;
  u8                  enableMaxZ : 1;
};

static const LIMIT_DATA POSITION_LIMIT_DATA_INITIAL_VALUE =
{
  gfl2::math::Vector4( 0.0f, 0.0f, 0.0f ),
  gfl2::math::Vector4( 0.0f, 0.0f, 0.0f ),
  0,0,0,0,0,0
};

struct INTERPOLATION_DATA
{
  u32 defaultFrame;
  UNIQUE_INTERPOLATION_DATA* pUniqueInterpolationDataTable;
  u32 uniqueInterpolationDataSize;
};

// カメラデータ基底
struct CAMERA_DATA
{
  u8    priority;                       // 優先度[0〜255]、低いほど優先される
  u32   id;                             // データID
  c8    name[512];                      // 識別名(今はmesh名)
  Type  type;                           // 識別子
  INTERPOLATION_DATA interpolationData; // 補間情報
  PROJECTION_DATA    projectionData;    // 射影情報

  void* pData;                          // データ本体、type毎に型が違う
};

// 相対回転指定
struct CAMERA_DATA_RELATIVE_ROTATE
{
  gfl2::math::Vector4  rotate;
  f32                 distance;
  CoordinateSystemType  targetOffsetType;
  gfl2::math::Vector4    targetOffset;
  gfl2::math::Vector4  rotateBufferMin;
  gfl2::math::Vector4  rotateBufferMax;
  LIMIT_DATA positionLimitData;
  LIMIT_DATA positionLimitDataEx;

  CoordinateSystemType  rotateLimitCoordinateSystemType;
  LIMIT_DATA            rotateLimitData;
};

struct CAMERA_DATA_RELATIVE_ROTATE_RATIO_POINT_DATA
{
  gfl2::math::Vector4  referencePoint;
  gfl2::math::Vector4  rotate;
  f32                 distance;
};

struct CAMERA_DATA_RELATIVE_ROTATE_RATIO
{
  CAMERA_DATA_RELATIVE_ROTATE_RATIO_POINT_DATA  startPointData;
  CAMERA_DATA_RELATIVE_ROTATE_RATIO_POINT_DATA  endPointData;
};

struct CAMERA_DATA_STOP
{
  u32 dummy;
};

inline void InitializeLimitData( LIMIT_DATA* pOut )
{
  *pOut = POSITION_LIMIT_DATA_INITIAL_VALUE;
}

inline void Clamp( const gfl2::math::Vector4& rSrc, const LIMIT_DATA& rData, gfl2::math::Vector4* pOut )
{
  // rSrc == pOutでも動作させる
  // @todo min > max の場合どうする？
 
  // min
  if( rData.enableMinX && pOut->x < rData.min.x )
  {
    pOut->x = rData.min.x;
  }
  if( rData.enableMinY && pOut->y < rData.min.y )
  {
    pOut->y = rData.min.y;
  }
  if( rData.enableMinZ && pOut->z < rData.min.z )
  {
    pOut->z = rData.min.z;
  }
  
  // max
  if( rData.enableMaxX && pOut->x > rData.max.x )
  {
    pOut->x = rData.max.x;
  }
  if( rData.enableMaxY && pOut->y > rData.max.y )
  {
    pOut->y = rData.max.y;
  }
  if( rData.enableMaxZ && pOut->z > rData.max.z )
  {
    pOut->z = rData.max.z;
  }
}

inline void Clamp( const gfl2::math::Vector4& rSrc, const LIMIT_DATA& rData, const LIMIT_DATA& rData2, gfl2::math::Vector4* pOut )
{
  // rSrc == pOutでも動作させる
  // @todo min > max の場合どうする？
 
  // min
  if( rData.enableMinX )
  {
    // 補間発動
    if( rData2.enableMinX && pOut->x < rData2.min.x )
    {

    }
    else if( rData.enableMinX && pOut->x < rData.min.x )
    {
      pOut->x = rData.min.x;
    }
  }
  if( rData.enableMinY && pOut->y < rData.min.y )
  {
    pOut->y = rData.min.y;
  }
  if( rData.enableMinZ && pOut->z < rData.min.z )
  {
    pOut->z = rData.min.z;
  }
  
  // max
  if( rData.enableMaxX && pOut->x > rData.max.x )
  {
    pOut->x = rData.max.x;
  }
  if( rData.enableMaxY && pOut->y > rData.max.y )
  {
    pOut->y = rData.max.y;
  }
  if( rData.enableMaxZ && pOut->z > rData.max.z )
  {
    pOut->z = rData.max.z;
  }
}

GFL_NAMESPACE_END(Skelton)
GFL_NAMESPACE_END(Test)

#endif
#endif // GF_PLATFORM_WIN32
