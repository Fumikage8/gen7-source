//===================================================================
/**
* @file    ZoneCollisionData.h
* @brief   ゾーンコリジョンデータの衝突チェック機能
* @author  yuya_ikeuchi
* @date    2015.04.01
*/
//===================================================================
#if !defined( ZONE_DATA_UTILITY_H_INCLUDED )
#define ZONE_DATA_UTILITY_H_INCLUDED

#include <types/include/gfl2_Typedef.h>
#include <math/include/gfl2_Vector.h>
#include <util/include/gfl2_List.h>

GFL_NAMESPACE_BEGIN( Field )

//-----------------------------------------------------------------------------
//! @brief ゾーンコリジョンデータ構造体
//-----------------------------------------------------------------------------
struct ZoneCollisionData
{
  ZoneCollisionData():m_pVertex(0),m_pIndex(0),m_triangleCount(0),m_id(0){}

  gfl2::math::Vector         *m_pVertex;
  u32                        *m_pIndex;
  u32                         m_triangleCount;
  u32                         m_id;
};

//-----------------------------------------------------------------------------
//! @brief ゾーンコリジョンクラス
//-----------------------------------------------------------------------------
class ZoneCollision
{
private:

  //------------------------------------------------------------------
  /**
  * @brief   コンストラクタ(使用禁止)
  */
  //------------------------------------------------------------------
  ZoneCollision();
  ZoneCollision(const ZoneCollision &other){}

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~ZoneCollision(){}

public:

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
  static s32 Check(gfl2::math::Vector& start,gfl2::math::Vector& end,gfl2::util::List<ZoneCollisionData*>* pZoneCollisionDataList);
};

GFL_NAMESPACE_END( Field )

#endif // ZONE_DATA_UTILITY_H_INCLUDED
