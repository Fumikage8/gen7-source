
#ifndef GFLIB2_MATH_CATMULL_ROM_H_INCLUDED
#define GFLIB2_MATH_CATMULL_ROM_H_INCLUDED

#include <types/include/gfl2_Typedef.h>
#include <math.h>
#include <math/include/gfl2_MathCommon.h>

#include <math/include/gfl2_Vector3.h>

#include <heap/include/gfl2_Heap.h>

namespace gfl2 { namespace math {

/**
 * @brief パスコントローラー
 */
class PathController
{
private:

  static const s32 DIVIDE_COUNT = 10;  // ノードの分割数

  // ノードの長さを保持する構造体
  struct NodeLength
  {
    f32 total;
    f32 len[DIVIDE_COUNT];
  };
  
public:

  /**
   * @brief コンストラクタ
   */
  PathController() :
    m_pValueList(NULL),
    m_ValueCount(0),
    m_TotalLength(0),
    m_pLengthList(NULL),
    m_LengthCount(0)
  {
  }

  /**
   * @brief デストラクタ
   */
  ~PathController()
  {
    this->Finalize();
  }

  /**
   * @brief 初期化
   *
   * @param pHeap ヒープ
   * @param pValueList 座標の配列、内部でコピーするのでオリジナルは保持しておく必要はありません
   * @param valueCount 座標数
   * @param loopEnabled trueなら最後にpValueList[0]に戻ってくるように加工します
   */
  void Initialize(heap::HeapBase* pHeap, const Vector3* pValueList, u32 valueCount, b32 loopEnabled)
  {
    this->Finalize();


    if (loopEnabled)
    {
      // ループする用の初期化

      m_ValueCount = valueCount + 3;
      m_pValueList = GFL_NEW_ARRAY(pHeap) Vector3[m_ValueCount];
      for (u32 i = 0; i < valueCount; ++i)
      {
        m_pValueList[i + 1] = pValueList[i];
      }

      m_pValueList[0] = pValueList[valueCount - 1];
      m_pValueList[valueCount + 1] = pValueList[0];
      m_pValueList[valueCount + 2] = pValueList[1];
    }
    else
    {
      // ループしない用の初期化

      m_ValueCount = valueCount + 2;
      m_pValueList = GFL_NEW_ARRAY(pHeap) Vector3[m_ValueCount];
      for (u32 i = 0; i < valueCount; ++i)
      {
        m_pValueList[i + 1] = pValueList[i];
      }

      m_pValueList[0] = pValueList[0];
      m_pValueList[valueCount + 1] = pValueList[valueCount - 1];
    }

    m_LengthCount = m_ValueCount - 3;
    m_pLengthList = GFL_NEW_ARRAY(pHeap) NodeLength[m_LengthCount];

    // 曲線の長さを計算
    m_TotalLength = 0;
    for (u32 i = 0; i < m_LengthCount; ++i)
    {
      const Vector3& p0 = m_pValueList[i];
      const Vector3& p1 = m_pValueList[i + 1];
      const Vector3& p2 = m_pValueList[i + 2];
      const Vector3& p3 = m_pValueList[i + 3];

      // 適当に分割して近似
      Vector3 oldValue = p1;
      m_pLengthList[i].total = 0;
      for (u32 t = 1; t <= DIVIDE_COUNT; ++t)
      {
        Vector3 newValue = CatmullRom(p0, p1, p2, p3, (f32)t / DIVIDE_COUNT);
        f32 len = (newValue - oldValue).Length();
        m_pLengthList[i].len[t - 1] = len;
        m_pLengthList[i].total += len;
        m_TotalLength += len;
        
        oldValue = newValue;
      }
    }
  }

  /**
   * @brief 終了処理
   */
  void Finalize()
  {
    GFL_SAFE_DELETE_ARRAY(m_pValueList);
    m_ValueCount = 0;
    m_TotalLength = 0;
    GFL_SAFE_DELETE_ARRAY(m_pLengthList);
    m_LengthCount = 0;
  }

  /**
   * @brief 値の取得
   *
   * @param t 補間時間 0.0 〜 1.0
   *
   * @return 座標のリストを補間した値を返します
   */
  Vector3 GetValue(f32 t)
  {
    // 開始と終了チェック
    if (t <= 0.0f)
    {
      return m_pValueList[1];
    }
    if (t >= 1.0f)
    {
      return m_pValueList[m_ValueCount - 2];
    }

    // 長さから曲線のどの地点かを計算する
    f32 t_length = t * m_TotalLength;

    f32 length = 0;
    u32 index0 = 0;
    for (index0 = 0; index0 < m_LengthCount; ++index0)
    {
      length = m_pLengthList[index0].total;
      
      if (t_length <= length)
      {
        // 見つけた
        break;
      }

      t_length -= length;
    }

    f32 length1 = 0;
    u32 index1 = 0;
    for (index1 = 0; index1 < DIVIDE_COUNT; ++index1)
    {
      length1 = m_pLengthList[index0].len[index1];

      if (t_length <= length1)
      {
        // 見つけた
        break;
      }

      t_length -= length1;
    }

    f32 new_t1 = (index1 + (t_length / length1)) / DIVIDE_COUNT;

    const Vector3& p0 = m_pValueList[index0];
    const Vector3& p1 = m_pValueList[index0 + 1];
    const Vector3& p2 = m_pValueList[index0 + 2];
    const Vector3& p3 = m_pValueList[index0 + 3];

    return CatmullRom(p0, p1, p2, p3, new_t1);
  }

  /**
   * @brief パス全体の長さを取得
   *
   * @return パス全体の長さを返します
   */
  f32 GetTotalLength() const
  {
    return m_TotalLength;
  }

  /**
   * @brief CatmullRom曲線
   *
   * @param p0 制御点0
   * @param p1 制御点1, t=0.0のときの値
   * @param p2 制御点2, t=1.0のときの値
   * @param p3 制御点3
   * @param t 補間係数
   *
   * @return 補間された値を返します
   */
  static Vector3 CatmullRom(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, f32 t)
  {
    const Vector3& v0 = (p2 - p0) * 0.5f;
    const Vector3& v1 = (p3 - p1) * 0.5f;

    f32 t2 = t * t;
    f32 t3 = t2 * t;

    return (p1 * 2.0f - p2 * 2.0f + v0 + v1) * t3
      + (p1 * -3.0f + p2 * 3.0f - v0 * 2.0f - v1) * t2
      + v0 * t
      + p1;
  }

private:

  Vector3* m_pValueList;          ///< 要素リスト
  u32 m_ValueCount;               ///< 要素数

  f32 m_TotalLength;              ///< トータル長さ
  NodeLength* m_pLengthList;      ///< 長さリスト
  u32 m_LengthCount;              ///< 長さ数
};

}}

#endif
