
//=============================================================================
/**
 * @file   CameraUtility.h
 * @brief  ユーティリティマクロ・クラスヘッダ
 * @author 池田 哲平
 * @date   2012.05.29
 * @auther kawada_koji@gamefreak.co.jp
 * @date   2015.11.24
 */
//=============================================================================

#if !defined( __CAMERA_UTILITY_H__ )
#define __CAMERA_UTILITY_H__
#pragma once

#include <debug/include/gfl2_Assert.h>
#include <math/include/gfl2_math.h>
#include <heap/include/gfl2_Heap.h>
#include "System/source/Camera/internal/CameraMisc.h"

#define DIFF_TIME_30FRAME (1.0f/30.0f) ///< 30フレーム時のフレーム間経過時間(秒)
#define GFL_ASSERT_ALWAYS GFL_ASSERT(NULL)    ///< 必ずアサートで落とす
#define GET_ARRAY_NUMBER(a) (sizeof(a)/sizeof(a[0])) ///< 静的配列の配列数を取得する
#define UTIL_METER_TO_UNIT(value) (value*20.0f) ///< メートルをゲーム世界の単位に変換する(1.0 -> 5cmくらいらしい)
#define UTIL_METER_TO_UNIT_BATTLE(value) (value*100.0f) ///< メートルをゲーム世界の単位に変換するのバトル版(1.0 -> 1cm)
#define UTIL_ZERO_ONE_TO_MINUS_ONE_ONE(value) (value*2.0f - 1.0f) ///< 0～1を-1～1に変換する
#define UTIL_MINUS_ONE_ONE_TO_ZERO_ONE(value) (value*0.5f + 0.5f) ///< -1～1を0～1に変換する

// 近似イプシロン
#define NEARLY_EQUAL_EPSILON_MOVE   (UTIL_METER_TO_UNIT(0.000034f)) /* 1mm    0.001f  /30.0f */
#define NEARLY_EQUAL_EPSILON_VEL    (UTIL_METER_TO_UNIT(0.000034f)) /*                       */
#define NEARLY_EQUAL_EPSILON_RATIO  (0.000034f)                     /* 0.1％  0.001f  /30.0f */
#define NEARLY_EQUAL_EPSILON_RADIAN (0.000029f)                     /* 0.1度  0.00087f/30.0f */
#define NEARLY_EQUAL_EPSILON_LENGTH (UTIL_METER_TO_UNIT(0.0001f))   /* 0.1mm                 */
#define NEARLY_EQUAL_EPSILON_DIR    (0.001f)                        /*                       */
#define NEARLY_EQUAL_ZERO_EPSILON_COS (0.000001f)                   /* コサインは曲線であるため比較する値によってイプシロンは変化しないといけない */
#define NEARLY_EQUAL_ONE_EPSILON_COS  (0.999999f)                   /* これはもっと厳しくしないといけないかもしれない */

namespace System { namespace Camera { namespace field
{
  extern const gfl2::math::VEC4 VEC4_COLOR_WHITE;
  extern const gfl2::math::VEC4 VEC4_COLOR_RED;
  extern const gfl2::math::VEC4 VEC4_COLOR_GREEN;
  extern const gfl2::math::VEC4 VEC4_COLOR_BLUE;
  extern const gfl2::math::VEC4 VEC4_COLOR_YELLOW;
  extern const gfl2::math::VEC4 VEC4_COLOR_THIN_WHITE;
  extern const gfl2::math::VEC4 VEC4_COLOR_THIN_RED;
  extern const gfl2::math::VEC4 VEC4_COLOR_THIN_GREEN;
  extern const gfl2::math::VEC4 VEC4_COLOR_THIN_BLUE;
  extern const gfl2::math::VEC4 VEC4_COLOR_THIN_YELLOW;
  extern const gfl2::math::VEC4 VEC4_COLOR_ORANGE;
  extern const gfl2::math::VEC4 VEC4_COLOR_SKY;
  extern const gfl2::math::VEC4 VEC4_COLOR_THIN_SKY;
  extern const gfl2::math::VEC4 VEC4_COLOR_PURPLE;
  extern const gfl2::math::VEC4 VEC4_COLOR_PEACH;
  extern const gfl2::math::VEC4 VEC4_COLOR_BROWN;

  //-----------------------------------------------------------------------------
  /**
   * @brief ユーティリティクラス
   */
  //-----------------------------------------------------------------------------
  class CUtil
  {
  public:
    template<typename T> static void MaxClip   (T* src, const T& max) { if(*src > max) *src = max; } ///< 最大クリップ
    template<typename T> static void MinClip   (T* src, const T& min) { if(*src < min) *src = min; } ///< 最少クリップ
    template<typename T> static void ZeroClip  (T* src)               { MinClip(src, static_cast<T>(0)); } ///< ゼロクリップ
    template<typename T> static bool MaxMinClip(T* src, const T& max, const T& min) ///< 最大・最少クリップ
    {
      if     (*src > max) { *src = max; return true;  }
      else if(*src < min) { *src = min; return true;  }
      else                {             return false; }
    }
    template<typename T> static void Exchange(T* value1, T* value2) { T temp = (*value1); (*value1) = (*value2); (*value2) = temp; } ///< 交換

    //-----------------------------------------------------------------------------
    /**
     * @brief 入力値がほとんど０であるか判定する
     *
     * @param fValue   判定値
     * @param fEpsilon 近似イプシロン
     *
     * @retval true  判断してよい
     * @retval false 判断してはいけない
     */
    //-----------------------------------------------------------------------------
    static inline bool IsNearlyEqualZero(const float fValue, const float fEpsilon) { return (gfl2::math::FAbs(fValue) <= fEpsilon); }

    //-----------------------------------------------------------------------------
    /**
     * @brief 入力値がほとんど０であるか判定する(３次元版)
     *
     * @param v        判定値
     * @param fEpsilon 近似イプシロン
     *
     * @retval true  判断してよい
     * @retval false 判断してはいけない
     */
    //-----------------------------------------------------------------------------
    static inline bool IsNearlyEqualZeroXYZ(const gfl2::math::VEC3& v, const float fEpsilon)
    {
      // 多分距離判定ではなく要素別判定の方が高速でかつ値が壊れづらいと思われる
      return ( IsNearlyEqualZero(v.x, fEpsilon) && IsNearlyEqualZero(v.y, fEpsilon) && IsNearlyEqualZero(v.z, fEpsilon) );
    }

    static float GetRandom(); ///< 乱数を取得する(0 <= ret < 1)
    static bool  GetRandomBool(); ///< ブール型の乱数を取得する
    static float GetRandomRangeRatio(const float fBase, const float fRangeRatio) ///< 基準値の比率を範囲とした乱数を取得する
    { return (1.0f + (GetRandom() - 0.5f)*fRangeRatio)*fBase; }

    /// 二つのベクタが逆を向いているか判定する
    static inline bool IsInverseVector(const gfl2::math::VEC3& v1, const gfl2::math::VEC3& v2) { return (gfl2::math::VEC3Dot(&v1, &v2) < 0.0f); }

    static float ConvertFromMinusPiToPlusPi(const float fRadi); ///< 入力ラジアン値を-πから+πの間に変換する
    static inline float AddRadian(const float fRadi1, const float fRadi2) { return ConvertFromMinusPiToPlusPi(fRadi1+fRadi2); } ///< ラジアンの加算値を取得する(解は-π～+π)

    //-----------------------------------------------------------------------------
    /**
     * @brief リミットクリップ処理を加えた上でアークサイン値を取得する
     *
     * @param fSin サイン値
     *
     * @return アークサイン値
     */
    //-----------------------------------------------------------------------------
    static float GetASinLimitClip(const float fSin)
    {
      if     ( fSin >=  1.0f ) return  System::Camera::internal::F_PI_2;
      else if( fSin <= -1.0f ) return -System::Camera::internal::F_PI_2;
      else                     return  gfl2::math::AsinRad(fSin);
    }

    //-----------------------------------------------------------------------------
    /**
     * @brief リミットクリップ処理を加えた上でアークコサイン値を取得する
     *
     * @param fCos コサイン値
     *
     * @return アークコサイン値
     */
    //-----------------------------------------------------------------------------
    static inline float GetACosLimitClip(const float fCos)
    {
      if     ( fCos >=  1.0f ) return 0.0f;
      else if( fCos <= -1.0f ) return System::Camera::internal::F_PI;
      else                     return gfl2::math::AcosRad(fCos);
    }

    //-----------------------------------------------------------------------------
    /**
     * @brief 入・出力方向間コサイン値を取得する
     *
     * @param inSrcDir  入力方向(正規化必須)
     * @param inDestDir 出力方向(正規化必須)
     *
     * @return コサイン値
     */
    //-----------------------------------------------------------------------------
    static inline float GetCos(const gfl2::math::VEC3& inSrcDir, const gfl2::math::VEC3& inDestDir) { return gfl2::math::VEC3Dot(&inSrcDir, &inDestDir); }

    //-----------------------------------------------------------------------------
    /**
     * @brief 軸成分の長さを取得する
     *
     * @param inVec  入力ベクタ
     * @param inAxis 入力軸(用正規化)
     *
     * @return 軸成分の長さ
     */
    //-----------------------------------------------------------------------------
    static inline float GetAxisLength(const gfl2::math::VEC3& inVec, const gfl2::math::VEC3& inAxis) { return gfl2::math::VEC3Dot(&inVec, &inAxis); }

    //-----------------------------------------------------------------------------
    /**
     * @brief 軸成分をカットしたベクタを取得する
     *
     * @param pDestVec 出力ベクタ
     * @param inSrcVec 入力ベクタ
     * @param inAxis   カットする軸(用正規化)
     */
    //-----------------------------------------------------------------------------
    static inline void GetAxisCutVector(gfl2::math::VEC3* pDestVec, const gfl2::math::VEC3& inSrcVec, const gfl2::math::VEC3& inAxis)
    {
      gfl2::math::VEC3 inSubVec; gfl2::math::VEC3Scale(&inSubVec, &inAxis, GetAxisLength(inSrcVec, inAxis));
      gfl2::math::VEC3Sub(pDestVec, &inSrcVec, &inSubVec);
    }

    //-----------------------------------------------------------------------------
    /**
     * @brief ローカル空間レベルの位置を取得する(空間のスケーリング不可)
     *
     * @param pDestPosi 出力ローカル位置
     * @param inSrcPosi 入力ワールド位置
     * @param inMat     ローカル空間
     */
    //-----------------------------------------------------------------------------
    static void GetLocalPosition(gfl2::math::VEC3* pDestPosi, const gfl2::math::VEC3& inSrcPosi, const gfl2::math::MTX34& inMat);

    //-----------------------------------------------------------------------------
    /**
     * @brief 正規化ベクトルの取得可能チェックを行い、取得可能時は取得する
     *
     * @param pNormVec 正規化ベクタ(取得不可時は設定されない)
     * @param pfLength 入力ベクタ長(取得不可時は設定されない)
     * @param inSrcVec 入力ベクタ
     *
     * @retval true  取得可
     * @retval false 取得不可
     */
    //-----------------------------------------------------------------------------
    static bool CheckAndGetNormalizeVector(gfl2::math::VEC3* pNormVec, float* pfLength, const gfl2::math::VEC3& inSrcVec);

    //-----------------------------------------------------------------------------
    /**
     * @brief 入力方向から出力方向へ到達する回転角度を取得する
     *
     * @param inSrcDir  入力方向(正規化必須)
     * @param inDestDir 出力方向(正規化必須)
     * @param inRotAxis 回転軸
     *
     * @return 回転角度
     */
    //-----------------------------------------------------------------------------
    static float GetReachRadian(const gfl2::math::VEC3& inSrcDir, const gfl2::math::VEC3& inDestDir, const gfl2::math::VEC3& inRotAxis);

    //-----------------------------------------------------------------------------
    /**
     * @brief 入力方向から出力方向へ到達する回転角度を取得する(回転軸算出版)
     *
     * @param pRotAxis  出力回転軸(未正規化)
     * @param inSrcDir  入力方向(正規化必須)
     * @param inDestDir 出力方向(正規化必須)
     *
     * @return 回転角度(回転軸の向きが動的に変わるため値は常に正が返る)
     */
    //-----------------------------------------------------------------------------
    static float GetReachRadian(gfl2::math::VEC3* pRotAxis, const gfl2::math::VEC3& inSrcDir, const gfl2::math::VEC3& inDestDir);

    //-----------------------------------------------------------------------------
    /**
     * @brief 入力方向から出力方向へ到達する回転マトリクスを取得する
     *
     * @param pMat      出力回転マトリクス
     * @param inSrcDir  入力方向(正規化必須)
     * @param inDestDir 出力方向(正規化必須)
     */
    //-----------------------------------------------------------------------------
    static void GetReachMatrix(gfl2::math::MTX33* pMat, const gfl2::math::VEC3& inSrcDir, const gfl2::math::VEC3& inDestDir);

    //-----------------------------------------------------------------------------
    /**
     * @brief 傾斜角を取得する
     *
     * @param inNormDir 法線方向(正規化必須)
     *
     * @return 傾斜角
     */
    //-----------------------------------------------------------------------------
    static inline float GetSlopeRadian(const gfl2::math::VEC3& inNormDir) { return GetACosLimitClip(inNormDir.y); }

    //-----------------------------------------------------------------------------
    /**
     * @brief 最大・最少位置を取得する
     *
     * @param pMax     出力最大位置
     * @param pMin     出力最少位置
     * @param ainPosi  位置配列
     * @param nPosiNum 配列数
     */
    //-----------------------------------------------------------------------------
    static void GetMaxMinPosition(gfl2::math::VEC3* pMax, gfl2::math::VEC3* pMin, const gfl2::math::VEC3 ainPosi[], const int nPosiNum);

    //-----------------------------------------------------------------------------
    /**
     * @brief 上昇ラインを描く比率配列を設定する(nArrayNum=1の時は1.0f)
     *
     * @param afArray　 出力先配列
     * @param nArrayNum 配列数
     */
    //-----------------------------------------------------------------------------
    static void SetUpLineRatioArray(float afArray[], const int nArrayNum);

    /// 線形比率をコサイン曲線比率に変換する
    static inline float ConvertLinearToCosCurveRatio(const float fRatio) { return (0.5f - 0.5f*gfl2::math::CosRad(System::Camera::internal::F_PI*fRatio)); }

    /// 有効な範囲内(0 < ratio < 1)でのみ線形比率をコサイン曲線比率に変換する
    static inline float ConvertLinearToCosCurveRatioValidRange(const float fRatio) { return (fRatio > 0.0f && fRatio < 1.0f) ? ConvertLinearToCosCurveRatio(fRatio) : fRatio; }

    // 逆コサイン曲線
    /*
    if( fRatio <= 0.5f ) fRatio = gfl2::math::CosRad(gfl2::math::F_PI*(0.5f - fRatio))*0.5f;
    else                 fRatio = gfl2::math::CosRad(gfl2::math::F_PI*(1.5f - fRatio))*0.5f + 1.0f;
    */

    //-----------------------------------------------------------------------------
    /**
     * @brief ３次元空間レベルにおけるスクリーンの長さの半分を取得する
     *
     * @param pfLenHalfX   長さの半分X
     * @param pfLenHalfY   長さの半分Y
     * @param fDist        カメラからスクリーンまでの(最短)距離
     * @param fViewAngle   画角
     * @param fAspectRatio アスペクト比率
     */
    //-----------------------------------------------------------------------------
    static inline void GetScreenLengthHalf3D(float* pfLenHalfX, float* pfLenHalfY, float fDist, const float fViewAngle, const float fAspectRatio)
    {
      if( fDist <= 0.0f )
      {
        GFL_ASSERT_ALWAYS;
        fDist = 0.000001f;
      }
      (*pfLenHalfY) = fDist*gfl2::math::TanRad(0.5f*fViewAngle);
      (*pfLenHalfX) = fAspectRatio*(*pfLenHalfY);
    }

    /// 遊び(０範囲)を加えた入力値を取得する
    static float GetAddMarginInputValue(const float fSrc, const float fMargin = 0.1f);

    /// XとYを取り換えるマトリクスを取得する(3dsはスクリーンの横がYで縦がXである仕様の対処)
    static void GetExchangeXYMatrix(gfl2::math::MTX44* pMat);

    static void ConvertFromToolToGameYPB(float* pfYawRadi, float* pfPitchRadi, float* pfBankRadi); ///< よー・ぴっち・ばんくをツール用からゲーム用へ変換する
    static void ConvertFromGameToToolYPB(float* pfYawRadi, float* pfPitchRadi, float* pfBankRadi); ///< よー・ぴっち・ばんくをゲーム用からツール用へ変換する

    static float GetEffectiveRadiusAABB(const gfl2::math::VEC3& inLenHalf, const gfl2::math::VEC3& inDir) ///< 各軸の長さが入力値(inLenHalf)の２倍であるAABBの入力方向(inDir)における実効半径を取得する
    { return gfl2::math::FAbs(inLenHalf.x*inDir.x) + gfl2::math::FAbs(inLenHalf.y*inDir.y) + gfl2::math::FAbs(inLenHalf.z*inDir.z); }

    //-----------------------------------------------------------------------------
    /**
     * @brief ワールド位置をスクリーン位置に変換する
     *
     * @param pScreenPosi   スクリーン位置
     * @param inWorldPosi   ワールド位置
     * @param inViewProjMat ワールド空間からプロジェクション空間に変換するマトリクス
     * @param fScreenWidth  スクリーン横幅
     * @param fScreenHeight スクリーン縦幅
     */
    //-----------------------------------------------------------------------------
    static void ConvertFromWorldToScreenPosition(gfl2::math::VEC2* pScreenPosi, const gfl2::math::VEC3& inWorldPosi, const gfl2::math::MTX44& inViewProjMat, const float fScreenWidth, const float fScreenHeight);

    //-----------------------------------------------------------------------------
    /**
     * @brief stlのvector模倣クラス
     */
    //-----------------------------------------------------------------------------
    template<typename TYPE>
    class CSTLVector
    {
    private:
      int   m_nCapacity; ///< 予約サイズ
      int   m_nSize;     ///< 使用サイズ
      TYPE* m_ainData;   ///< データ配列

    public:
      CSTLVector() : m_nCapacity(0) , m_nSize(0) , m_ainData(NULL) { return ; } ///< コンストラクタ
      ~CSTLVector() { Clear(); } ///< デストラクタ

      /// 予約サイズ確保
      void Reserve(const int nCapacity, gfl2::heap::HeapBase* pHeap)
      {
        // メモリを再確保してコピーする処理は、コンストラクタやデストラクタの問題をうまく対処する方法が分からなかったため、仕様としてメモリの再確保を禁止する
        GFL_ASSERT(nCapacity > 0 && m_ainData == NULL);

        m_nCapacity = nCapacity;
        m_nSize     = 0;
        m_ainData   = GFL_NEW_ARRAY(pHeap) TYPE [nCapacity];
      }

      /// 使用サイズ確保
      void Resize(const int nSize, gfl2::heap::HeapBase* pHeap)
      {
        Reserve(nSize, pHeap);
        m_nSize = nSize;
      }
      /// 使用サイズ確保(初期値設定版)
      void Resize(const int nSize, const TYPE& inData, gfl2::heap::HeapBase* pHeap)
      {
        Resize(nSize, pHeap);
        for( int i = 0; i < m_nSize; i++ ) m_ainData[i] = inData;
      }
      /// 使用サイズ確保(Reserve済み版)
      void Resize(const int nSize)
      {
        GFL_ASSERT(nSize <= m_nCapacity);
        m_nSize = nSize;
      }

      int Capacity() const { return m_nCapacity; } ///< 予約サイズを取得する
      int Size()     const { return m_nSize;     } ///< 使用サイズを取得する

      /// 末尾追加(予約サイズを越えると警告停止する)
      void PushBack(const TYPE& inData)
      {
        GFL_ASSERT(m_nSize < m_nCapacity);

        m_ainData[m_nSize] = inData;
        m_nSize++;
      }

      /// クリア
      void Clear()
      {
        GFL_SAFE_DELETE_ARRAY(m_ainData);
        m_nCapacity = m_nSize = 0;
      }

      /// リセット(予約サイズは維持される)
      void Reset() { m_nSize = 0; }

      /// 空チェック
      bool Empty() const { return (m_nSize == 0); }

      /* オペレータ演算子 */
      TYPE& operator[](const int nIndex) { GFL_ASSERT(nIndex >= 0 && nIndex < m_nSize); return m_ainData[nIndex]; }
      TYPE& Front()                      { GFL_ASSERT(m_nSize > 0); return m_ainData[0];         }
      TYPE& Back()                       { GFL_ASSERT(m_nSize > 0); return m_ainData[m_nSize-1]; }

      const TYPE& operator[](const int nIndex) const { return (*const_cast<CSTLVector* >(this))[nIndex]; }
      const TYPE& front()                      const { return (*const_cast<CSTLVector* >(this)).front(); }
      const TYPE& back()                       const { return (*const_cast<CSTLVector* >(this)).back();  }
    };

    //-----------------------------------------------------------------------------
    /**
     * @brief 球クラス
     */
    //-----------------------------------------------------------------------------
    class CSphere
    {
    private:
      gfl2::math::VEC3 m_inPosi;  ///< 中心位置
      float           m_fRadius; ///< 半径

    public:
      CSphere() : m_inPosi(System::Camera::internal::VEC3_ZERO), m_fRadius(0.0f)  { return ; }                ///< コンストラクタ１
      CSphere(const gfl2::math::VEC3& inPosi, const float fRadius)  { Set(inPosi, fRadius); }   ///< コンストラクタ２
      CSphere(const gfl2::math::VEC3 ainPosi[], const int nPosiNum) { Set(ainPosi, nPosiNum); } ///< コンストラクタ３

      void Set(const gfl2::math::VEC3& inPosi, const float fRadius) { m_inPosi = inPosi; m_fRadius = fRadius; } ///< データを設定する
      //-----------------------------------------------------------------------------
      /**
       * @brief 複数の点を内包する球を設定する
       *
       * @param ainPosi  点の位置配列
       * @param nPosiNum 配列数
       */
      //-----------------------------------------------------------------------------
      void Set(const gfl2::math::VEC3 ainPosi[], const int nPosiNum);

      const gfl2::math::VEC3& GetPosition() const { return m_inPosi; }
      float GetRadius() const { return m_fRadius; }

      //-----------------------------------------------------------------------------
      /**
       * @brief 球との衝突判定を行う
       *
       * @param inSphere 判定する球
       *
       * @retval true  衝突している
       * @retval false 衝突していない
       */
      //-----------------------------------------------------------------------------
      bool IsHitSphere(const CSphere& inSphere) const ;

      /// デバッグ描画
      void DrawDebug(const gfl2::math::VEC4& inColor) const ;
    };
  };

} // namespace field
} } // namespace System namespace Camera

#endif // __CAMERA_UTILITY_H__

/*  EOF  */





