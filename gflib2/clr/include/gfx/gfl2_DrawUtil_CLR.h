#if !defined(GFL2_GFX_CLR_DRAWUTIL_H_INCLUDED)
#define GFL2_GFX_CLR_DRAWUTIL_H_INCLUDED
#pragma once

#include <gfx/include/gfl2_DrawUtil.h>
#include <clr/include/gfx/gfl2_GFGL_CLR.h>
#include <clr/include/math/gfl2_Vector_CLR.h>
#include <clr/include/math/gfl2_Matrix_CLR.h>
#include <clr/include/math/gfl2_MathAABB_CLR.h>

namespace gfl2 { namespace clr { namespace gfx {
/**
 * @brief DrawUtilラッパークラス
 */
public ref class DrawUtil
{
public:
  /**
   * @brief DrawUtil用頂点構造体
   */
  ref class VertexFormat
  {
  public:
    /**
     * @brief コンストラクタ
     */
    VertexFormat()
    {
      m_pNative = new gfl2::gfx::DrawUtil::VertexFormat();
    }

    /**
     * @brief コンストラクタ
     * @param pos_ 位置
     * @param color_ 色
     * @param uv_ UV座標
     */
    VertexFormat(clr::math::Vector^ pos, clr::math::Vector^ color, clr::math::Vector^ uv)
    {
      m_pNative = new gfl2::gfx::DrawUtil::VertexFormat(pos->GetNativeOldVector(), color->GetNativeOldVector(), uv->GetNativeOldVector());
    }

    /**
     * @brief デストラクタ
     */
    ~VertexFormat()
    {
      this->!VertexFormat();
    }

    /**
     * @brief ファイナライザ
     */
    !VertexFormat()
    {
      if (m_pNative)
      {
        delete m_pNative;
        m_pNative = NULL;
      }
    }

    /**
     * @brief ネイティブポインタのgetter
     * @return gfl2::gfx::DrawUtil::VertexFormat* ネイティブポインタ
     */
    gfl2::gfx::DrawUtil::VertexFormat* GetNative()
    {
      return m_pNative;
    }

  private:
    gfl2::gfx::DrawUtil::VertexFormat* m_pNative; //!< @brief ネイティブポインタ
  };

  /**
   * @brief 初期化
   * @param pAllocator gfxアロケータ
   * @param pDisplayDeviceHandle デバイスハンドル
   */
  static void Initialize(u32 screenWidth, u32 screenHeight)
  {
    gfl2::gfx::DrawUtil::Initialize(gfl2::clr::gfx::GFGL::GetGLAllocator(), screenWidth, screenHeight, 256);
  }

  /**
   * @brief 終了
   */
  static void Terminate()
  {
    gfl2::gfx::DrawUtil::Terminate();
  }

	/*
	 * @brief 簡易描画開始前の処理. BeginRender()〜EndRender()で簡易描画処理を挟む
	 */
	static void BeginRender()
  {
    gfl2::gfx::DrawUtil::BeginRender();
  }

	/*
	 * @brief 簡易描画開始後の処理. BeginRender()〜EndRender()で簡易描画処理を挟む
	 */
	static void EndRender()
  {
    gfl2::gfx::DrawUtil::EndRender();
  }

	/*
	 * @brief カリング設定
   * @param cullMode カリングモード
	 */
  static void SetCullMode(gfl2::clr::gfx::CullMode::Enum cullMode)
  {
    gfl2::gfx::CullMode mode = static_cast<gfl2::gfx::CullMode>((s32)cullMode);
    gfl2::gfx::DrawUtil::SetCullMode(mode);
  }
	
	/*
	 * @brief ワイヤーフレーム/通常描画切り替え設定
   * @param fillMode 塗りつぶしモード
	 */
	static void SetFillMode(gfl2::gfx::FillMode fillMode)
  {
    gfl2::gfx::DrawUtil::SetFillMode(fillMode);
  }

	/*
	 * @brief マテリアルカラー設定
   * @param color 色
	 */
	static void SetMaterialColor(clr::math::Vector^ color)
  {
    gfl2::gfx::DrawUtil::SetMaterialColor( (color->GetNativeOldVector()) );
  }

	/*
	 * @brief マテリアルカラーのアルファ値設定
   * @param f32 アルファ値
	 */
	static void SetMaterialAlpha(f32 alpha)
  {
    gfl2::gfx::DrawUtil::SetMaterialAlpha(alpha);
  }

	/*
	 * @brief プロジェクション行列の設定
   * @param mat 行列
	 */
  static void SetProjectionMatrix(clr::math::Matrix^ mat)
  {
    gfl2::gfx::DrawUtil::SetProjectionMatrix(*(mat->GetNative()));
  }

	/*
	 * @brief ビュー行列の設定
   * @param mat 行列
	 */
	static void SetViewMatrix(clr::math::Matrix^ mat)
  {
    gfl2::gfx::DrawUtil::SetViewMatrix(mat->GetNativeMatrix34());
  }

	/*
	 * @func SetModelMatrix
   * @param mat 行列
	 */
	static void SetModelMatrix(clr::math::Matrix^ mat)
  {
    gfl2::gfx::DrawUtil::SetModelMatrix(*mat->GetNative());
  }
	
	/*
	 * @brief 矩形描画
   * @param x x座標
   * @param y y座標
   * @param w 横幅
   * @param h 縦幅
	 */
	static void DrawRect(f32 x, f32 y, f32 w, f32 h)
  {
    gfl2::gfx::DrawUtil::DrawRect(x, y, w, h);
  }

	/*
	 * @brief 矩形描画
   * @param x x座標
   * @param y y座標
   * @param w 横幅
   * @param h 縦幅
   * @param color 色
	 */
	static void DrawRect(f32 x, f32 y, f32 w, f32 h, clr::math::Vector^ color)
  {
    gfl2::gfx::DrawUtil::DrawRect(x, y, w, h, (color->GetNativeOldVector()) );
  }

	/*
	 * @brief 矩形塗りつぶし
   * @param x x座標
   * @param y y座標
   * @param w 横幅
   * @param h 縦幅
   * @param color 色
	 */
	static void FillRect(f32 x, f32 y, f32 w, f32 h, clr::math::Vector^ color)
  {
    gfl2::gfx::DrawUtil::FillRect(x, y, w, h, (color->GetNativeOldVector()) );
  }

	/*
	 * @brief スクリーン全体に矩形描画
	 */
	static void DrawScreen()
  {
    gfl2::gfx::DrawUtil::DrawScreen();
  }

	/*
	 * @brief スクリーン全体に矩形描画
   * @param color 色
	 */
	static void DrawScreen(clr::math::Vector^ color)
  {
    gfl2::gfx::DrawUtil::DrawScreen((color->GetNativeOldVector()));
  }

	/*
	 * @brief スクリーン全体に矩形塗りつぶし
   * @param color 色
	 */
	static void FillScreen(clr::math::Vector^ color)
  {
    gfl2::gfx::DrawUtil::FillScreen((color->GetNativeOldVector()));
  }

	/*
	 * @brief AABB(軸平行境界ボックス)描画
   * @param min AABB最小値
   * @param max AABB最大値
   * @param color 色
	 */
	static void DrawAABB(clr::math::Vector^ min, clr::math::Vector^ max, clr::math::Vector^ color)
  {
    gfl2::gfx::DrawUtil::DrawAABB((min->GetNativeOldVector()), (max->GetNativeOldVector()), (color->GetNativeOldVector()));
  }

	/*
	 * @brief AABB(軸平行境界ボックス)描画
   * @param aabb AABB
   * @param color 色
	 */
  static void DrawAABB(clr::math::AABB^ aabb, clr::math::Vector^ color)
  {
    gfl2::gfx::DrawUtil::DrawAABB(*aabb->GetNative(), color->GetNativeOldVector());
  }

	/*
	 * @brief AABB(軸平行境界ボックス)描画
   * @param min AABB最小値
   * @param max AABB最大値
	 */
	static void DrawAABB(clr::math::Vector^ min, clr::math::Vector^ max)
  {
    gfl2::gfx::DrawUtil::DrawAABB((min->GetNativeOldVector()), (max->GetNativeOldVector()));
  }

  /*
	 * @brief AABB(軸平行境界ボックス)描画
   * @param aabb AABB
	 */
  static void DrawAABB(clr::math::AABB^ aabb)
  {
    gfl2::gfx::DrawUtil::DrawAABB(*aabb->GetNative());
  }

  /*
	 * @func DrawCircle
	 * @brief 円柱描画
	 */
  static void DrawCircle(clr::math::Vector^ position, float radius, clr::math::Vector^ color)
  {
    gfl2::gfx::DrawUtil::DrawCircle((position->GetNativeOldVector()), radius, (color->GetNativeOldVector()));
  }

  /*
	 * @func DrawCylinder
	 * @brief 円柱描画
	 */
  static void DrawCylinder(clr::math::Vector^ position, float radius, float height, clr::math::Vector^ color)
  {
    gfl2::gfx::DrawUtil::DrawCylinder((position->GetNativeOldVector()), radius, height, (color->GetNativeOldVector()));
  }
	
	/*
	 * @brief AABB(軸平行境界ボックス)描画
   * @note: AABB_CLRは別途対応
	 */
	//static void DrawAABB(const math::AABB& aabb);

	/*
	 * @brief 球体描画
	 */
	//static void DrawSphere(clr::math::Vector^ center, f32 radius, clr::math::Vector^ color);

	/*
	 * @brief シザリング設定解除
	 */
	static void ClearScissor()
  {
    gfl2::gfx::DrawUtil::ClearScissor();
  }

	/*
	 * @brief シザリング設定
   * @param x x座標
   * @param y y座標
   * @param width 横幅
   * @param height 縦幅
	 */
	static void SetScissor(u32 x, u32 y, u32 width, u32 height)
  {
    gfl2::gfx::DrawUtil::SetScissor(x, y, width, height);
  }

  /*
	 * @brief 深度テスト有効/無効設定
   * @param enable 有効/無効
	 */
  static void SetDepthTestEnable(bool enable)
  {
    gfl2::gfx::DepthStencilStateObject::StateData stateData;
    stateData.m_DepthTestEnable = static_cast<u32>(enable);
    gfl2::gfx::DrawUtil::SetDepthTestState(stateData);
  }

private:
  DrawUtil(){}
  ~DrawUtil(){}
  !DrawUtil(){}
 };

}}}

#endif