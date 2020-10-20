//======================================================================
/**
 * @file FieldShadeAreaManager.h
 * @date 2016/12/06 18:05:03
 * @author kawazoe_shinichi
 * @brief 陰影領域管理
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================


#if !defined __FIELD_SHADE_AREA_MANAGER_H_INCLUDED__
#define __FIELD_SHADE_AREA_MANAGER_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_Vector2.h>
#include <math/include/gfl2_Vector3.h>

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Shade);

//-------------------------------------------------------------------------
/// @class FieldShadeAreaManager
/// @brief 陰影領域管理クラス
//-------------------------------------------------------------------------
class FieldShadeAreaManager
  {
    GFL_FORBID_COPY_AND_ASSIGN(FieldShadeAreaManager);

  public:

    // ランタイムコードとデータフォーマット統一用バージョン情報
    enum
    {
      VERSION_161213 = 0,

      // 現在のデータバージョン
      LATEST_VERSION = VERSION_161213
    };

    GFL_WARNING_PUSH
    GFL_WARNING_WIN32_IGNORE(4200) //C4200警告を抑止
    //-------------------------------------------------------------------------
    /// @brief 陰影領域ヘッダー
    //-------------------------------------------------------------------------
    struct ShadeAreaHeader
    {
      u32 version;          // データフォーマットバージョン
      u32 totalNum;         // データ数
      void* pData;          // データ部（陰影領域データ+頂点リスト）
    };
    GFL_WARNING_POP

    //-------------------------------------------------------------------------
    /// @brief 陰影領域データ
    //-------------------------------------------------------------------------
    struct ShadeAreaData
    {
      u32                   vartexCount;        //!< 頂点数
      f32                   color[3];           //!< 陰影RGBカラー
      f32                   priority;           //!< 優先度

      f32                   minBoundingArea[2]; //!< 判定領域開始地点
      f32                   maxBoundingArea[2]; //!< 判定領域終了地
    };

    //-------------------------------------------------------------------------
    /// @brief 陰影領域管理クラス初期化設定
    //-------------------------------------------------------------------------
    struct InitDesc
    {
      void* pShadeAreaData;         //!< 陰影領域データ

    };

    //-------------------------------------------------------------------------
    /// @brief  コンストラクタ
    //-------------------------------------------------------------------------
    FieldShadeAreaManager();
    //-------------------------------------------------------------------------
    /// @brief  デストラクタ
    //-------------------------------------------------------------------------
    ~FieldShadeAreaManager();


    //-------------------------------------------------------------------------
    /// @brief  初期化処理
    //-------------------------------------------------------------------------
    void Initialize(const InitDesc& desc);


    //-------------------------------------------------------------------------
    /// @brief  陰影適用領域内かどうか判定
    /// @param vPos   領域内かどうか調べる頂点
    /// @return 領域内の場合は領域ID（領域外の場合は『-1』）
    //-------------------------------------------------------------------------
    s32 CheckOverlap(const gfl2::math::Vector3& vPos) const;

    //-------------------------------------------------------------------------
    /// @brief  陰影領域データ数取得
    //-------------------------------------------------------------------------
    u32 GetNumData(void){ return m_NumData; } const

    //-------------------------------------------------------------------------
    /// @brief  指定陰影領域データ取得
    //-------------------------------------------------------------------------
    const ShadeAreaData* GetData(u32 idx);

  private:

    //-------------------------------------------------------------------------
    /// @brief  点と三角形の衝突判定
    /// @param vA   三角形の頂点A
    /// @param vB   三角形の頂点B
    /// @param vC   三角形の頂点C
    /// @param vP   三角形の内側か外側かを調べる頂点
    /// @return 衝突しているかどうか（三角形の内側）
    //-------------------------------------------------------------------------
    bool Intersect(gfl2::math::Vector2 vA, gfl2::math::Vector2 vB, gfl2::math::Vector2 vC, gfl2::math::Vector2 vP) const;


  private:
    u32                     m_NumData;            //!< データ数
    const ShadeAreaData*    m_pShadeAreaData;     //!< 陰影領域データ
    const f32*              m_pXZVertexList;      //!< 陰影適用領域を示すXZ頂点リスト（三角ポリゴンの集合データ）
  };

GFL_NAMESPACE_END(Shade)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_SHADE_AREA_MANAGER_H_INCLUDED__