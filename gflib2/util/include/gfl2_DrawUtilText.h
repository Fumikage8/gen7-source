#ifndef __GFL2_UTIL_DRAWUTILTEXT_H__
#define __GFL2_UTIL_DRAWUTILTEXT_H__
#pragma once

#if defined(GF_PLATFORM_WIN32)
#include <nw/font.h>
#include <nw/font/font_RectDrawer.h>
#elif defined(GF_PLATFORM_CTR)
#include <nw/font2.h>
#include <nw/font2/font_RectDrawer.h>
#endif

#include <heap/include/gfl2_NwAllocator.h>
#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_math.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(util)

class UtilTextRenderPath;

class DrawUtilText
{
	friend class UtilTextRenderPath;

  GFL_FORBID_COPY_AND_ASSIGN( DrawUtilText );

public:
	// 48文字×32行のメモリを確保
	static const u32			LINE_MAX = 32;
	static const u32			LINE_CHAR_MAX = 48;
	static const u32			CharMax = LINE_MAX * LINE_CHAR_MAX;
  static const u32      StackMax = 2;


private:
	struct UtilTextInfo
	{
		wchar_t* pUtf16;									//!< 文字列
		nw::font::Font* pFont;						//!< フォントリソース
		s32 posX;													//!< 表示位置(Ｘ)
		s32 posY;													//!< 表示位置(Ｙ)
		gfl2::math::Vector2 scale;				//!< スケール
		gfl2::math::Vector4 startColor;		//!< テキストカラー(グラデーション開始色)
		gfl2::math::Vector4 endColor;			//!< グラデーション終端色
		bool gradation;										//!< グラデーションフラグ

		UtilTextInfo()
			: pUtf16(NULL)
			, pFont(NULL)
			, posX(0)
			, posY(0)
			, scale(gfl2::math::Vector2(0, 0))
			, startColor(gfl2::math::Vector4(1,1,1,1))
			, endColor(gfl2::math::Vector4(1,1,1,1))
			, gradation(false)
		{
			;
		}
	};

public:
	/**
	 * @brief	初期化
	 */
	void Initialize( heap::NwAllocator* pMemAllocator, heap::NwAllocator* pDevAllocator, util::UtilTextRenderPath* pRenderPath = NULL );
  /**
   * 終了関数
   */
	void Terminate( void );

public:
	/**
	 * @brief	コンストラクタ
	 * @param	pMemAllocator		メモリアロケータ
	 * @param	pDevAllocator		デバイスアロケータ
	 * @param	stackMax
	 */
	DrawUtilText( heap::NwAllocator* pMemAllocator, heap::NwAllocator* pDevAllocator, s32 stackMax = StackMax, u32 charMax = CharMax, u32 lineMax = LINE_MAX, u32 lineCharaMax = LINE_CHAR_MAX );
	/**
	 * @brief	デストラクタ
	 */
	~DrawUtilText( void );

public:
  

  /**
	 * @brief	RectDrawShaderをわたす
	 */
  void SetRectDrawerShader(void* pBuff,int size);

  /**
	 * @brief	レンダリングパスの設定
	 */
  void SetRenderPath( util::UtilTextRenderPath* pRenderPath );

  /**
   * @brief フォント設定
   * @param pFont		フォント
   */
  void SetFont( nw::font::Font* pFont );

  /**
   * @brief テキストのスケール設定
   * @param scale_x   Ｘスケール
   * @param scale_y   Ｙスケール
   */
  void SetTextScale( f32 scale_x, f32 scale_y );

  /**
   * @brief テクストカラー設定関数
   * @param color   設定したいカラー
   */
  void SetTextColor( const gfl2::math::Vector4 &color );
  
  /**
   * @brief テキスト描画関数
   * @param posX    表示Ｘ座標位置
   * @param posY    表示Ｙ座標位置
   * @param pSjis   表示テキスト(Utf16フォーマット)
   * @param ...     引数
   */
#if defined(GF_PLATFORM_WIN32)
  void DrawTextW( s32 posX, s32 posY, const wchar_t* pUtf16, ... );
#elif defined(GF_PLATFORM_CTR)
  void DrawText( s32 posX, s32 posY, const wchar_t* pUtf16, ... );
#endif

  //この関数はいずれ廃止します。
#if defined(GF_PLATFORM_WIN32)
  void DrawTextW( s32 posX, s32 posY, const wchar_t* pUtf16, const gfl2::math::Vector4 &color, nw::font::Font *pFont = NULL );  
#elif defined(GF_PLATFORM_CTR)
  void DrawText( s32 posX, s32 posY, const wchar_t* pUtf16, const gfl2::math::Vector4 &color, nw::font::Font *pFont = NULL );  
#endif
  /**
   * @brief テキスト描画関数（グラデーションカラー対応版）
   * @param posX        表示Ｘ座標位置
   * @param posY        表示Ｙ座標位置
   * @param pSjis       表示テキスト(Utf16フォーマット)
   * @param startColor  グラデーション開始カラー
   * @param endColor    グラデーション終了カラー
   * @param gradMode    グラデーションモード
   */
#if defined(GF_PLATFORM_WIN32)
  void DrawTextW( s32 posX, s32 posY, const wchar_t* pUtf16, const gfl2::math::Vector4 &startColor, const gfl2::math::Vector4 &endColor, nw::font::Font *pFont = NULL );
#elif defined(GF_PLATFORM_CTR)
  void DrawText( s32 posX, s32 posY, const wchar_t* pUtf16, const gfl2::math::Vector4 &startColor, const gfl2::math::Vector4 &endColor, nw::font::Font *pFont = NULL );
#endif

	/**
	 * @brief		描画テキストの登録をクリア
	 */
  void ClearDrawText( void );

private:
	/**
	 * @brief		フォント描画の初期化
	 */
	void InitFontRenderer( u32 charMax );

	/**
	 * @brief		描画前
	 */
  void BeginDraw( u32 width, u32 height );
	/**
	 * @brief		描画後
	 */
  void EndDraw( void );

	/**
	 * @brief		描画
	 */
  void Draw( UtilTextInfo& info );

private:
  wchar_t**		m_ppUtf16;
	struct Impl;
//	DrawUtilText*	s_Gp;
	Impl*									m_pImpl;

  util::UtilTextRenderPath* m_pRenderPath;

  void* mpDrawShaderBuff;
  int mShaderSize;

  u32 mLineMax;
  u32 mLineCharMax;
};

GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(gfl2)

#endif	// __GFL2_UTIL_DRAWUTILTEXT_H__
