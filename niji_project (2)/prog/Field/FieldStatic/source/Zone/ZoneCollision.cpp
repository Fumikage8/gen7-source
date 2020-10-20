//===================================================================
/**
* @file    ZoneCollisionData.h
* @brief   ゾーンコリジョンデータの衝突チェック機能
* @author  yuya_ikeuchi
* @date    2015.04.01
*/
//===================================================================
#include "Field/FieldStatic/include/Zone/ZoneCollision.h"

GFL_NAMESPACE_BEGIN( Field )

//-----------------------------------------------------------------------------
/**
*  @brief 自身がどのゾーンにいるかチェック
*
* @param  st    線分の始点
* @param  ed    線分の終点
* @param  pZoneCollisionDataList  ゾーンコリジョンデータリスト
* @retval ゾーンID
*/
//-----------------------------------------------------------------------------
s32 ZoneCollision::Check(gfl2::math::Vector& start,gfl2::math::Vector& end,gfl2::util::List<ZoneCollisionData*>* pZoneCollisionDataList)
{
  if( !pZoneCollisionDataList || pZoneCollisionDataList->Size() == 0 )
  {
    return -1;
  }

  gfl2::math::Vector vertexPos0;
  gfl2::math::Vector vertexPos1;
  gfl2::math::Vector vertexPos2;
  gfl2::math::Vector normal;
  gfl2::math::Vector cross1;
  gfl2::math::Vector cross2;
  gfl2::math::Vector cross3;
  gfl2::math::Vector v0ToV1;
  gfl2::math::Vector v0ToV2;
  gfl2::math::Vector intersection;
  gfl2::math::Vector v10;
  gfl2::math::Vector vi0;
  gfl2::math::Vector v21;
  gfl2::math::Vector vi1;
  gfl2::math::Vector v02;
  gfl2::math::Vector vi2;
  gfl2::math::Vector v1;
  gfl2::math::Vector v2;
  f32                d1;
  f32                d2;
  f32                rate;

  // ゾーンの領域チェック
  for( gfl2::util::List<ZoneCollisionData*>::Iterator ite = pZoneCollisionDataList->Begin() ; ite != pZoneCollisionDataList->End() ; ++ite )
  {
    ZoneCollisionData* pZoneCollisionData = *ite;

    for( u32 i=0 ; i<pZoneCollisionData->m_triangleCount ; i++ )
    {
      vertexPos0 = pZoneCollisionData->m_pVertex[ pZoneCollisionData->m_pIndex[i*3+0] ];
      vertexPos1 = pZoneCollisionData->m_pVertex[ pZoneCollisionData->m_pIndex[i*3+1] ];
      vertexPos2 = pZoneCollisionData->m_pVertex[ pZoneCollisionData->m_pIndex[i*3+2] ];
      v1 = start - vertexPos0;
      v2 = end   - vertexPos0;

      // 線分と平面が当たっているか
      if( v1.Dot3(normal) * v2.Dot3(normal) > 0.0f)
      {
        continue;
      }

      // 法線を計算
      v0ToV1  = vertexPos1 - vertexPos0;
      v0ToV2  = vertexPos2 - vertexPos0;
      normal  = v0ToV1.Cross(v0ToV2).Normalize3();

      // 交点を計算
      d1   = gfl2::math::FAbs( normal.Dot3( v1)) / normal.Length3();
      d2   = gfl2::math::FAbs( normal.Dot3( v2)) / normal.Length3();
      if( d1 + d2 == 0.0f )
      {
        continue;
      }
      rate = d1 / (d1 + d2);
      intersection = vertexPos0 + (v1 * (1.0f - rate) + v2 * rate);

      // 交点がポリゴン内に含まれているか
      v10    = vertexPos1   - vertexPos0;
      vi0    = intersection - vertexPos0;
      cross1 = v10.Cross(vi0);

      v21    = vertexPos2   - vertexPos1;
      vi1    = intersection - vertexPos1;
      cross2 = v21.Cross(vi1);

      v02    = vertexPos0   - vertexPos2;
      vi2    = intersection - vertexPos2;
      cross3 = v02.Cross(vi2);

      if( cross1.Dot3( cross2) < 0.0f || cross1.Dot3( cross3) < 0.0f)
      {
        continue;
      }

#if 1
      // 各辺の延長線上に交点が含まれる場合があるので、辺内に交点があるかもチェック
      if (cross1.Dot3(cross1) < 0.0001f) // 適当な誤差
      {
        f32 l1 = v10.Length3();
        gfl2::math::Vector n1 = v10 / l1; // 正規化
        f32 e1 = n1.Dot3(vi0) / l1;  // 射影して辺の長さに対する割合を求める
        if (e1 < 0.0f || e1 > 1.0f)
        {
          continue;
        }
      }

      if (cross2.Dot3(cross2) < 0.0001f) // 適当な誤差
      {
        f32 l2 = v21.Length3();
        gfl2::math::Vector n2 = v21 / l2; // 正規化
        f32 e2 = n2.Dot3(vi1) / l2;  // 射影して辺の長さに対する割合を求める
        if (e2 < 0.0f || e2 > 1.0f)
        {
          continue;
        }
      }

      if (cross3.Dot3(cross3) < 0.0001f) // 適当な誤差
      {
        f32 l3 = v02.Length3();
        gfl2::math::Vector n3 = v02 / l3; // 正規化
        f32 e3 = n3.Dot3(vi2) / l3;  // 射影して辺の長さに対する割合を求める
        if (e3 < 0.0f || e3 > 1.0f)
        {
          continue;
        }
      }
#endif

      // 当たってる
      return pZoneCollisionData->m_id;
    }
  }

  return -1;
}

GFL_NAMESPACE_END( Field )
