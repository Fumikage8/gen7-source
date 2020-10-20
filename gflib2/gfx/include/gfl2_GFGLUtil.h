#ifndef GFLIB2_GRX_GFGLUTIL_H_INCLUDED
#define GFLIB2_GRX_GFGLUTIL_H_INCLUDED

#include <stdio.h>
#include <types/include/gfl2_Typedef.h>
#include <gfx/include/gfl2_types.h>

#include <Debug/include/gfl2_Assert.h>

namespace gfl2 { namespace gfx {

class GFGLUtil
{
public:

	//  各ディスプレイモードの情報
	struct DisplayModeInfo
	{
			u32 width;
			u32 height;
	};

	//  各フォーマットの情報
	struct SurfaceFormatInfo
	{
			const char* name;   //  フォーマット名

			int color;          //  カラーのビット幅
			int red;            //  赤輝度成分のビット幅
			int green;          //  緑輝度成分のビット幅
			int blue;           //  青輝度成分のビット幅
			int alpha;          //  α成分のビット幅

			int depthStencil;   //  深度ステンシルのビット幅
			int depth;          //  深度成分のビット幅
			int stencil;        //  ステンシルのビット幅

			b32 isFloat;       //  ＦＰバッファ
			b32 isCompressed;  //  圧縮フォーマット
	};

	//! @name フォーマット処理
	//@{
		//! @brief ディスプレイモードがフルスクリーンかどうか
		//! @note プラットフォームがWindowsでなければ必ずtrueを返します
		static b32 IsFullScreenDisplayMode( DisplayMode displayMode );
		//! @brief 各ディスプレイモードの解像度の幅を取得する
		static u32 GetDisplayWidthByDisplayMode( DisplayMode displayMode );
		//! @brief 各ディスプレイモードの解像度の高さを取得する
		static u32 GetDisplayHeightByDisplayMode( DisplayMode displayMode );

		//! @brief ユーザー設定用ディスプレイモードの解像度の幅を設定する
		static void SetDisplayWidthForUserSetting( u32 size );
		//! @brief ユーザー設定用ディスプレイモードの解像度の高さを取得する
		static void SetDisplayHeightForUserSetting( u32 size );
		
        //! @brief ピクセルフォーマットを文字列に変換する
		static const char* GetFormatString( Format format );
		//! @brief ピクセルフォーマットのビットサイズを取得する
		//! @retval 0 引数がピクセルフォーマットではない
		static int GetPixelBitSizeByFormat( Format format );
		//! @brief カラーフォーマットのビットサイズを取得する
		//! @retval 0 引数がカラーフォーマットではない
		static int GetColorBitSizeByFormat( Format format );
		//! @brief フォーマットのＲ成分のビット数
		static int GetRedBitSizeByFormat( Format format );
		//! @brief フォーマットのＧ成分のビット数
		static int GetGreenBitSizeByFormat( Format format );
		//! @brief フォーマットのＢ成分のビット数
		static int GetBlueBitSizeByFormat( Format format );
		//! @brief フォーマットのα成分のビット数
		static int GetAlphaBitSizeByFormat( Format format );
		
		//! @brief フォーマットの深度ステンシル成分のビット数
		//! @retval 0 引数のフォーマットにに深度ステンシル成分が存在しない
		static int GetDepthStencilBitSizeByFormat( Format format );
		//! @brief フォーマットの深度成分のビット数
		//! @retval 0 引数のフォーマットにに深度成分が存在しない
		static int GetDepthBitSizeByFormat( Format format );
		//! @brief フォーマットのステンシル成分のビット数
		//! @retval 0 引数のフォーマットにはステンシル成分が存在しない
		static int GetStencilBitSizeByFormat( Format format );
		
		//! @brief 圧縮テクスチャかどうか
		static b32 IsFormatCompressed( Format format );
		//! @brief ＦＰテクスチャかどうか
		static b32 IsFormatFloatingPointer( Format format );
		//! @brief ビット幅を指定して、最適なサーフェイスフォーマットを取得する
		//! @retval Format::Unknown 指定されたフォーマットには対応していない
		static Format GetSuitedSurfaceFormat( u32 red, u32 green, u32 blue, u32 alpha, b32 fp );
		//! @brief ビット幅を指定して、最適なテクスチャフォーマットを取得する
		//! @retval Format::Unknown 指定されたフォーマットには対応していない
		static Format GetSuitedTextureFormat( u32 red, u32 green, u32 blue, u32 alpha, b32 fp );
		//! @brief ビット幅を指定して、最適な深度ステンシルフォーマットを取得する
		//! @retval Format::Unknown 指定されたフォーマットには対応していない
		static Format GetSuitedDepthStencilFormat( u32 depth, u32 stencil );
		//! @brief 指定フォーマットでの画像のバッファサイズ(バイト数)を求める
		static u32 GetImageSizeByFormat( u32 width, u32 height, Format format );
	//@}

	//! @name 頂点演算処理
	//@{
		//! @brief 型と要素数から１座標のバイトサイズを計算する
		//! @retval 0 引数が不正です
		inline static u32 GetCoordStride(const VertexType type, const u32 nbElements){
			static const u32 s_table[ VertexType::NumberOf ] = {
				2,	  //ShortNormalized = 0, //!< 16bit符号付整数を[-1...1]に正規化した値
				4,	  //Float,               //!< 32bit浮動少数値
				2,	  //Half,                //!< 16bit浮動少数値
				1,	  //UByteNormalized,     //!< 8bit符号なし整数を[0...1]に正規化した値
				2,	  //Short,               //!< 16bit符号付整数
				0,	  //CompressNormalized,  //!< 32bit(11:11:10)に圧縮した整数を[-1...1]に正規化した値(PS3のみ)
				1,	  //UByte,               //!< 8bit符号なし整数値
				1,	  //ByteNormalized6,     //!< 8bit固定小数値(小数部6bit)
				2,	  //ShortNormalized12,   //!< 16bit固定小数値(小数部12bit)
			};
			if(type == VertexType::CompressNormalized)
				return (nbElements == 3) ? 4 : 0;
			return s_table[type] * nbElements;
		}

		//! @brief インデクスタイプから１インデクスのバイト幅を取得する
		inline static u32 GetIndexStride(const IndexType type){
			switch(type){
				case IndexType::Index16: return 2;
				case IndexType::Index32: return 4;
				case IndexType::Index8:  return 1;
			}

      GFL_ASSERT(0);
      return 1;
		}
	//@}

private:

	static const DisplayModeInfo s_displayModeInfoTable[ DisplayMode::NumberOf ];
	static const SurfaceFormatInfo s_formatInfoTable[ Format::NumberOf ];
  static DisplayModeInfo s_displayModeInfoUserSetting;

	GFGLUtil(){}
	~GFGLUtil(){}
};

}}

#endif
