//======================================================================
/**
 * @file FieldShadeAreaManager.cpp
 * @date 2016/12/06 18:46:39
 * @author kawazoe_shinichi
 * @brief 陰影領域管理
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <debug/include/gfl2_Assert.h>

#include "Field/FieldRo/include/Shade/FieldShadeAreaManager.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Shade);

//-------------------------------------------------------------------------
/// @brief  コンストラクタ
//-------------------------------------------------------------------------
FieldShadeAreaManager::FieldShadeAreaManager() :
m_NumData         (0),
m_pShadeAreaData  (NULL),
m_pXZVertexList   (NULL)
{

}

//-------------------------------------------------------------------------
/// @brief  デストラクタ
//-------------------------------------------------------------------------
FieldShadeAreaManager::~FieldShadeAreaManager()
{

}

//-------------------------------------------------------------------------
/// @brief  初期化処理
//-------------------------------------------------------------------------
void FieldShadeAreaManager::Initialize(const InitDesc& desc)
{
  // データ展開
  ShadeAreaHeader* const pData = reinterpret_cast<ShadeAreaHeader*>(desc.pShadeAreaData);
  GFL_ASSERT(pData);
  // データフォーマットのバージョン確認
  GFL_ASSERT_STOP_MSG(pData->version == LATEST_VERSION, "this data is not the latest version [%d != %d]\n", pData->version, LATEST_VERSION);

  m_NumData = pData->totalNum;

  // バッファから領域データと頂点リストを分離
  u32* pU32 = reinterpret_cast<u32*>(pData);
  pU32 += 2;                                    // 領域データまでシーク
  m_pShadeAreaData  = reinterpret_cast<const ShadeAreaData*>(pU32);
  m_pXZVertexList   = reinterpret_cast<const f32*>(m_pShadeAreaData + m_NumData);

  // データチェック
  GFL_ASSERT( m_pShadeAreaData );
  GFL_ASSERT( m_pXZVertexList );

}


//-------------------------------------------------------------------------
/// @brief  陰影適用領域内かどうか判定
/// @param vPos   領域内かどうか調べる頂点
/// @return 領域内の場合は領域ID（領域外の場合は『-1』）
//-------------------------------------------------------------------------
s32 FieldShadeAreaManager::CheckOverlap(const gfl2::math::Vector3& vPos) const
{
  gfl2::math::Vector2 xzPos = gfl2::math::Vector2(vPos.x, vPos.z);
  s32 hitAreaID = -1;
  f32 hitPriority = 0.0f;

  u32 vertexStartIdx = 0;
  gfl2::math::Vector2 vMinBound;
  gfl2::math::Vector2 vMaxBound;
  for (u32 i = 0; i < m_NumData; i++)
  {
    const ShadeAreaData* const pData = &m_pShadeAreaData[i];


    // バウンディング領域取得
    vMinBound = gfl2::math::Vector2(pData->minBoundingArea[0], pData->minBoundingArea[1]);
    vMaxBound = gfl2::math::Vector2(pData->maxBoundingArea[0], pData->maxBoundingArea[1]);

    // 頂点データ取得
    const f32* const pXZVartexList = reinterpret_cast<const f32*>(&m_pXZVertexList[vertexStartIdx]);

    // バウンディング領域内外判定
    if (!(xzPos - vMinBound).MinPerElem(vMaxBound - xzPos).IsAnyMinus())
    {
      // バウンディング領域内の場合は各陰影領域との判定を行う　※三角ポリゴンで判定を行うので3頂点ずつ処理する
      u32 numVartex = pData->vartexCount;
      for (u32 k = 0; k < numVartex; k += 3)
      {
        // 衝突判定
        gfl2::math::Vector2 v1 = gfl2::math::Vector2(pXZVartexList[(k + 0) * 2 + 0], pXZVartexList[(k + 0) * 2 + 1]);
        gfl2::math::Vector2 v2 = gfl2::math::Vector2(pXZVartexList[(k + 1) * 2 + 0], pXZVartexList[(k + 1) * 2 + 1]);
        gfl2::math::Vector2 v3 = gfl2::math::Vector2(pXZVartexList[(k + 2) * 2 + 0], pXZVartexList[(k + 2) * 2 + 1]);
        if (Intersect(v1, v2, v3, xzPos))
        {
          if (hitAreaID > -1)
          {
            if (hitPriority >= pData->priority)
            {
              // 優先度が低い場合は無視
              break;
            }
          }
          hitAreaID = i;
          hitPriority = pData->priority;
          break;
        }
      }
    }
    vertexStartIdx += pData->vartexCount * 2;    // 開始頂点IDを調整（次の領域データへ）　※1頂点はXZ座標でセットなので2をかけている
  }
  return hitAreaID;
}

//-------------------------------------------------------------------------
/// @brief  指定陰影領域データ取得
//-------------------------------------------------------------------------
const FieldShadeAreaManager::ShadeAreaData* FieldShadeAreaManager::GetData(u32 idx)
{
  GFL_ASSERT(idx < GetNumData());

  return &m_pShadeAreaData[idx];
}



//-------------------------------------------------------------------------
/// @brief  点と三角形の衝突判定
/// @param vA   三角形の頂点A
/// @param vB   三角形の頂点B
/// @param vC   三角形の頂点C
/// @param vP   三角形の内側か外側かを調べる頂点
/// @return 衝突しているかどうか（三角形の内側）
//-------------------------------------------------------------------------
bool FieldShadeAreaManager::Intersect(gfl2::math::Vector2 vA, gfl2::math::Vector2 vB, gfl2::math::Vector2 vC, gfl2::math::Vector2 vP) const
{
  gfl2::math::Vector2 vAB = vB - vA;
  gfl2::math::Vector2 vBP = vP - vB;

  gfl2::math::Vector2 vBC = vC - vB;
  gfl2::math::Vector2 vCP = vP - vC;

  gfl2::math::Vector2 vCA = vA - vC;
  gfl2::math::Vector2 vAP = vP - vA;

  f32 cp1 = vAB.Cross(vBP);
  f32 cp2 = vBC.Cross(vCP);
  f32 cp3 = vCA.Cross(vAP);

  if ((cp1 > 0 && cp2 > 0 && cp3 > 0) || (cp1 < 0 && cp2 < 0 && cp3 < 0))
  {
    // 三角形の内側の場合
    return true;
  }

  return false;
}



GFL_NAMESPACE_END(Shade)
GFL_NAMESPACE_END(Field)
