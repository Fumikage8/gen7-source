#pragma once
//===================================================================================================================================================
// 
// 
//===================================================================================================================================================

#if defined(GF_PLATFORM_CTR)
#define USE_AURA_SHADER (1)
#elif defined(GF_PLATFORM_WIN32)
#define USE_AURA_SHADER (0)
#endif  //プラットフォーム分岐

#if USE_AURA_SHADER
#include <nn.h>


#include <math/include/gfl2_MathCommon.h>

//===================================================================================================================================================
// 
//  コンバイナ操作設定用構造体
// 
//===================================================================================================================================================
//---------------------------------------------------------------------------------------------------------------------------------------------------
//  設定用構造体
//---------------------------------------------------------------------------------------------------------------------------------------------------
#define CONV_TEXTURE_MAX      ( 3 )
#define CONV_STAGE_MAX        ( 6 )
#define CONV_CONSTANTCOL_MAX  ( 6 )
#define CONV_LOOKUPTBL_IDX    ( 9 ) // 参照テーブルは9番から

//-----------------------------------------------
// 値構造体
struct u8Value {
  u8 value;
  u8Value(){ value = 0; }

  void setValue( float V )
  {
    // 四捨五入をして格納
    value = static_cast<u32>( V * 255.0f + 0.5f );
  }
};

//-----------------------------------------------
// 色構造体
struct u32Color {
  u8 r; u8 g; u8 b; u8 a;
  u32Color(){ r = g = b = 0; a = 1; }

  void setColor( float R, float G, float B, float A )
  {
    // 四捨五入をして格納
    r = static_cast<u32>( R * 255.0f + 0.5f );
		g = static_cast<u32>( G * 255.0f + 0.5f );
		b = static_cast<u32>( B * 255.0f + 0.5f );
		a = static_cast<u32>( A * 255.0f + 0.5f );
  }
};

//---------------------------------------------------------------------------------------------------------------------------------------------------
//  各種コマンド構造体
//---------------------------------------------------------------------------------------------------------------------------------------------------
// コンバイナ設定【演算式】
struct CONV_MATH_FML {
  enum {
	  REPLACE = 0,  // A
	  MODULATE,     // A * B
	  ADD,          // A + B
	  ADDSIGNED,    // A + B - 0.5
	  INTERPOLATE,  // A * C + B * (1-C)
	  SUBTRACT,     // A - B
	  DOT3RGB,      // RGB ← dot(A, B)
	  DOT3RGBA,     // RGBA ← dot(A, B)
	  ADDMULT,      // (A + B) * C
	  MULTADD       // (A * B) + C
  };

  PicaDataTexEnvCombine getFormula( int idx )
  {
    static const PicaDataTexEnvCombine tbl[] = {
		  PICA_DATA_TEX_ENV_COMBINE_REPLACE,
      PICA_DATA_TEX_ENV_COMBINE_MODULATE,
      PICA_DATA_TEX_ENV_COMBINE_ADD,
      PICA_DATA_TEX_ENV_COMBINE_ADD_SIGNED,
      PICA_DATA_TEX_ENV_COMBINE_INTERPOLATE,
		  PICA_DATA_TEX_ENV_COMBINE_SUBTRACT,
      PICA_DATA_TEX_ENV_COMBINE_DOT3_RGB,
      PICA_DATA_TEX_ENV_COMBINE_DOT3_RGBA,
      PICA_DATA_TEX_ENV_COMBINE_ADD_MULT_DMP,
      PICA_DATA_TEX_ENV_COMBINE_MULT_ADD_DMP,
    };
    if( idx > MULTADD ){ return tbl[0]; }

    return tbl[idx];
  }
};

//-----------------------------------------------
// コンバイナ設定【演算スケール】
struct CONV_MATH_SCALE {
  enum {
	  VAL1 = 0,
	  VAL2,
	  VAL4
  };

  PicaDataTexEnvScale getScale( int idx )
  {
    static const PicaDataTexEnvScale tbl[] = {
			PICA_DATA_TEX_ENV_SCALE_1,
      PICA_DATA_TEX_ENV_SCALE_2,
      PICA_DATA_TEX_ENV_SCALE_4
    };
    if( idx > VAL4 ){ return tbl[0]; }

    return tbl[idx];
  }
};

//-----------------------------------------------
// コンバイナ設定【演算ソース】
struct CONV_MATH_SRC {
  enum {
	  TEX0 = 0,             // テクスチャ0
	  TEX1,                 // テクスチャ1
	  TEX2,                 // テクスチャ2
	  TEX3,                 // テクスチャ3
	  CONSTANTCOL,          // コンスタントカラー
	  PRIMARYCOL,           // 頂点シェーダーの出力結果(頂点カラー)
	  FLAGMENTPRIMARYCOL,   // プライマリカラー(ライトの陰影段階)
	  FLAGMENTSECONDARYCOL, // セカンダリカラー(ライトの陰影段階を基に参照テーブル上の値)
	  PREVIOUS,             // 前段の出力結果
	  PREVIOUSBUFFER        // 前段のバッファ
  };

  PicaDataTexEnvSrc getSource( int idx )
  {
    static const PicaDataTexEnvSrc tbl[] = {
		  PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0,
      PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE1,
      PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE2,
      PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE3,
		  PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT,
      PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR,
      PICA_DATA_TEX_ENV_SRC_RGBA_FRAGMENT_PRIMARY_COLOR_DMP,
		  PICA_DATA_TEX_ENV_SRC_RGBA_FRAGMENT_SECONDARY_COLOR_DMP,
      PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS,
      PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS_BUFFER_DMP
    };
    if( idx > PREVIOUSBUFFER ){ return tbl[0]; }

    return tbl[idx];
  }
};

//-----------------------------------------------
// コンバイナ設定【演算対象(RGB)】
struct CONV_MATH_OPR_RGB {
  enum {
	  RGB = 0,
	  ONEMINUS_RGB,
	  ALPHA,
	  ONEMINUS_ALPHA,
	  RED,
	  ONEMINUS_RED,
	  GREEN,
	  ONEMINUS_GREEN,
	  BLUE,
	  ONEMINUS_BLUE
  };

  PicaDataTexEnvOperand getOperand( int idx )
  {
    static const PicaDataTexEnvOperand tbl[] = {
			PICA_DATA_OPE_RGB_SRC_COLOR,
      PICA_DATA_OPE_RGB_ONE_MINUS_SRC_COLOR,
      PICA_DATA_OPE_RGB_SRC_ALPHA,
      PICA_DATA_OPE_RGB_ONE_MINUS_SRC_ALPHA,
      PICA_DATA_OPE_RGB_SRC_R_DMP,
			PICA_DATA_OPE_RGB_ONE_MINUS_SRC_R_DMP,
      PICA_DATA_OPE_RGB_SRC_G_DMP,
      PICA_DATA_OPE_RGB_ONE_MINUS_SRC_G_DMP,
      PICA_DATA_OPE_RGB_SRC_B_DMP,
      PICA_DATA_OPE_RGB_ONE_MINUS_SRC_B_DMP,
    };
    if( idx > ONEMINUS_BLUE ){ return tbl[0]; }

    return tbl[idx];
  }
};

//-----------------------------------------------
// コンバイナ設定【演算対象(A)】
struct CONV_MATH_OPR_A {
  enum {
	  ALPHA = 0,
	  ONEMINUS_ALPHA,
	  RED,
	  ONEMINUS_RED,
	  GREEN,
	  ONEMINUS_GREEN,
	  BLUE,
	  ONEMINUS_BLUE
  };

  PicaDataTexEnvOperand getOperand( int idx )
  {
    static const PicaDataTexEnvOperand tbl[] = {
      PICA_DATA_OPE_ALPHA_SRC_ALPHA,
      PICA_DATA_OPE_ALPHA_ONE_MINUS_SRC_ALPHA,
      PICA_DATA_OPE_ALPHA_SRC_R_DMP,
      PICA_DATA_OPE_ALPHA_ONE_MINUS_SRC_R_DMP,
			PICA_DATA_OPE_ALPHA_SRC_G_DMP,
      PICA_DATA_OPE_ALPHA_ONE_MINUS_SRC_G_DMP,
      PICA_DATA_OPE_ALPHA_SRC_B_DMP,
      PICA_DATA_OPE_ALPHA_ONE_MINUS_SRC_B_DMP,
    };
    if( idx > ONEMINUS_BLUE ){ return tbl[0]; }

    return tbl[idx];
  }
};

//-----------------------------------------------
// コンバイナ設定【バッファ出力フラグ】
struct CONV_BUFFERINPUT {
  enum {
	  PREVIOUSBUFFER = 0,
	  PREVIOUS
  };

  PicaDataTexEnvBufferInput getInput( int idx )
  {
    static const PicaDataTexEnvBufferInput tbl[] = {
      PICA_DATA_TEX_ENV_BUFFER_INPUT_PREVIOUS_BUFFER_DMP,
      PICA_DATA_TEX_ENV_BUFFER_INPUT_PREVIOUS,
    };
    if( idx > PREVIOUSBUFFER ){ return tbl[0]; }

    return tbl[idx];
  }
};

//-----------------------------------------------
// レンダーステート【ブレンド演算式】
struct RDST_BLENDEQ {
  enum {
	  ADD = 0,          // Src + Dst
	  SUBTRACT,         // Src - Dst
	  REVERSE_SUBTRACT, // Dst - Src
	  MIN,              // min( Src, Dst )
	  MAX               // max( Src, Dst )
  };

  PicaDataBlendEquation getCommand( int idx )
  {
    static const PicaDataBlendEquation tbl[] = {
			PICA_DATA_BLEND_EQUATION_ADD, 
      PICA_DATA_BLEND_EQUATION_SUBTRACT, 
      PICA_DATA_BLEND_EQUATION_REVERSE_SUBTRACT,
      PICA_DATA_BLEND_EQUATION_MIN, 
      PICA_DATA_BLEND_EQUATION_MAX
    };
    if( idx > MAX ){ return tbl[0]; }

    return tbl[idx];
  }
};

//-----------------------------------------------
// レンダーステート【ブレンド要素式】
struct RDST_BLENDFUNC {
  enum {
		ZERO = 0,                 // 0
    ONE,                      // 1
    SRC_COLOR,                // コンバイナ出力RGB
    ONE_MINUS_SRC_COLOR,      // 1 - コンバイナ出力RGB
    DST_COLOR,                // フレームバッファRGB
		ONE_MINUS_DST_COLOR,      // 1 - フレームバッファRGB
    SRC_ALPHA,                // コンバイナ出力A
    ONE_MINUS_SRC_ALPHA,      // 1 - コンバイナ出力A
    DST_ALPHA,                // フレームバッファA
		ONE_MINUS_DST_ALPHA,      // 1 - フレームバッファA
    CONSTANT_COLOR,           // ブレンド・コンスタントカラーRGB
    ONE_MINUS_CONSTANT_COLOR, // 1 - ブレンド・コンスタントカラーRGB
    CONSTANT_ALPHA,           // ブレンド・コンスタントカラーA
		ONE_MINUS_CONSTANT_ALPHA, // 1 - ブレンド・コンスタントカラーA
    SRC_ALPHA_SATURATE        // min( コンバイナ出力A, 1 - フレームバッファA )
  };

  PicaDataBlendFunc getCommand( int idx )
  {
    static const PicaDataBlendFunc tbl[] = {
			PICA_DATA_BLEND_FUNC_ZERO,
      PICA_DATA_BLEND_FUNC_ONE,
      PICA_DATA_BLEND_FUNC_SRC_COLOR,
      PICA_DATA_BLEND_FUNC_ONE_MINUS_SRC_COLOR,
      PICA_DATA_BLEND_FUNC_DST_COLOR,
			PICA_DATA_BLEND_FUNC_ONE_MINUS_DST_COLOR,
      PICA_DATA_BLEND_FUNC_SRC_ALPHA,
      PICA_DATA_BLEND_FUNC_ONE_MINUS_SRC_ALPHA,
      PICA_DATA_BLEND_FUNC_DST_ALPHA,
			PICA_DATA_BLEND_FUNC_ONE_MINUS_DST_ALPHA,
      PICA_DATA_BLEND_FUNC_CONSTANT_COLOR,
      PICA_DATA_BLEND_FUNC_ONE_MINUS_CONSTANT_COLOR,
      PICA_DATA_BLEND_FUNC_CONSTANT_ALPHA,
			PICA_DATA_BLEND_FUNC_ONE_MINUS_CONSTANT_ALPHA,
      PICA_DATA_BLEND_FUNC_SRC_ALPHA_SATURATE
    };
    if( idx > SRC_ALPHA_SATURATE ){ return tbl[0]; }

    return tbl[idx];
  }
};

//-----------------------------------------------
// レンダーステート【アルファテスト式】
struct RDST_ALPHATEST {
  enum {
		ALWAYS = 0, // 常にパスさせる
    LESS,       // リファレンス値より小さい場合にのみパスさせる
		L_EQUAL,    // リファレンス値と同じか小さい場合にパスさせる
		EQUAL,      // リファレンス値と同じ場合にはパスさせる
		G_EQUAL,    // リファレンス値と同じか大きい場合にパスさせる
		GREATER,    // リファレンス値よりも大きい場合にパスさせる
		NOTEQUAL,   // リファレンス値と同じではない場合にパスさせる
		NEVER       // パスさせない
  };

  PicaDataAlphaTest getCommand( int idx )
  {
    static const PicaDataAlphaTest tbl[] = {
			PICA_DATA_ALPHA_TEST_ALWAYS,
			PICA_DATA_ALPHA_TEST_LESS,
			PICA_DATA_ALPHA_TEST_LEQUAL,
			PICA_DATA_ALPHA_TEST_EQUAL,
			PICA_DATA_ALPHA_TEST_GEQUAL,
			PICA_DATA_ALPHA_TEST_GREATER,
			PICA_DATA_ALPHA_TEST_NOTEQUAL,
			PICA_DATA_ALPHA_TEST_NEVER
    };
    if( idx > NEVER ){ return tbl[0]; }

    return tbl[idx];
  }
};

//-----------------------------------------------
// レンダーステート【デプステスト式】
struct RDST_DEPTHTEST {
  enum {
		ALWAYS = 0, // 常にパスさせる
    LESS,       // リファレンス値より小さい場合にのみパスさせる
		L_EQUAL,    // リファレンス値と同じか小さい場合にパスさせる
		EQUAL,      // リファレンス値と同じ場合にはパスさせる
		G_EQUAL,    // リファレンス値と同じか大きい場合にパスさせる
		GREATER,    // リファレンス値よりも大きい場合にパスさせる
		NOTEQUAL,   // リファレンス値と同じではない場合にパスさせる
		NEVER       // パスさせない
  };

  PicaDataDepthTest getCommand( int idx )
  {
    static const PicaDataDepthTest tbl[] = {
			PICA_DATA_DEPTH_TEST_ALWAYS,
			PICA_DATA_DEPTH_TEST_LESS,
			PICA_DATA_DEPTH_TEST_LEQUAL,
			PICA_DATA_DEPTH_TEST_EQUAL,
			PICA_DATA_DEPTH_TEST_GEQUAL,
			PICA_DATA_DEPTH_TEST_GREATER,
			PICA_DATA_DEPTH_TEST_NOTEQUAL,
			PICA_DATA_DEPTH_TEST_NEVER,
    };
    if( idx > NEVER ){ return tbl[0]; }

    return tbl[idx];
  }
};

//-----------------------------------------------
// レンダーステート【ステンシルテスト式】
struct RDST_STENCILTEST {
  enum {
		ALWAYS = 0, // 常にパスさせる
    LESS,       // リファレンス値より小さい場合にのみパスさせる
		L_EQUAL,    // リファレンス値と同じか小さい場合にパスさせる
		EQUAL,      // リファレンス値と同じ場合にはパスさせる
		G_EQUAL,    // リファレンス値と同じか大きい場合にパスさせる
		GREATER,    // リファレンス値よりも大きい場合にパスさせる
		NOTEQUAL,   // リファレンス値と同じではない場合にパスさせる
		NEVER       // パスさせない
  };

  PicaDataStencilTest getCommand( int idx )
  {
    static const PicaDataStencilTest tbl[] = {
			PICA_DATA_STENCIL_TEST_ALWAYS,
			PICA_DATA_STENCIL_TEST_LESS,
			PICA_DATA_STENCIL_TEST_LEQUAL,
			PICA_DATA_STENCIL_TEST_EQUAL,
			PICA_DATA_STENCIL_TEST_GEQUAL,
			PICA_DATA_STENCIL_TEST_GREATER,
			PICA_DATA_STENCIL_TEST_NOTEQUAL,
			PICA_DATA_STENCIL_TEST_NEVER,
    };
    if( idx > NEVER ){ return tbl[0]; }

    return tbl[idx];
  }
};

//-----------------------------------------------
// レンダーステート【ステンシル操作】
struct RDST_STENCILOP {
  enum {
    KEEP = 0,   // 更新しない
		ZERO,       // 0に設定
		REPLACE,    // 参照値に置き換える
		INCR,       // 最大値に達するまで増加
		INCR_WRAP,  // 増加させその値が最大値を超えると0に戻す
		DECR,       // 0になるまで減少
		DECR_WRAP,  // 減少させその値が0より小さくなった場合は最大値に戻す
		INVERT      // ビットを反転
  };

  PicaDataStencilOp getCommand( int idx )
  {
    static const PicaDataStencilOp tbl[] = {
			PICA_DATA_STENCIL_OP_KEEP,
			PICA_DATA_STENCIL_OP_ZERO,
			PICA_DATA_STENCIL_OP_REPLACE,
			PICA_DATA_STENCIL_OP_INCR,
			PICA_DATA_STENCIL_OP_INCR_WRAP,
			PICA_DATA_STENCIL_OP_DECR,
			PICA_DATA_STENCIL_OP_DECR_WRAP,
			PICA_DATA_STENCIL_OP_INVERT
    };
    if( idx > INVERT ){ return tbl[0]; }

    return tbl[idx];
  }
};

//-----------------------------------------------
// レンダーステート【カリング】
struct RDST_CULL {
  enum {
    NONE = 0, // 背面カリングしない
    CCW,      // 背面を反時計回りでカリング
    CW        // 背面を時計回りでカリング
  };

  nn::gr::CTR::RenderState::Culling::CullFace getCommand( int idx )
  {
    static const nn::gr::CTR::RenderState::Culling::CullFace tbl[] = {
			nn::gr::CTR::RenderState::Culling::CULL_FACE_BACK,
			nn::gr::CTR::RenderState::Culling::CULL_FACE_BACK,
			nn::gr::CTR::RenderState::Culling::CULL_FACE_FRONT
    };
    if( idx > CW ){ return tbl[0]; }

    return tbl[idx];
  }
};

//-----------------------------------------------
// テクスチャサンプラー【フィルター】
struct TXST_FILTER {
  enum {
		POINT = 0,
    LINEAR,
    POINT_LINEAR,
    LINEAR_LINEAR
  };

  PicaDataTextureMinFilter getCommand_Min( int idx )
  {
    static const PicaDataTextureMinFilter tbl[] = {
		  PICA_DATA_TEXTURE_MIN_FILTER_NEAREST,
		  PICA_DATA_TEXTURE_MIN_FILTER_LINEAR,
		  PICA_DATA_TEXTURE_MIN_FILTER_NEAREST_MIPMAP_LINEAR,
		  PICA_DATA_TEXTURE_MIN_FILTER_LINEAR_MIPMAP_LINEAR
    };
    if( idx > LINEAR_LINEAR ){ return tbl[0]; }

    return tbl[idx];
  }

  PicaDataTextureMagFilter getCommand_Mag( int idx )
  {
    static const PicaDataTextureMagFilter tbl[] = {
		  PICA_DATA_TEXTURE_MAG_FILTER_NEAREST,
		  PICA_DATA_TEXTURE_MAG_FILTER_LINEAR,
		  PICA_DATA_TEXTURE_MAG_FILTER_NEAREST,
		  PICA_DATA_TEXTURE_MAG_FILTER_LINEAR
    };
    if( idx > LINEAR_LINEAR ){ return tbl[0]; }

    return tbl[idx];
  }
};

//-----------------------------------------------
// テクスチャサンプラー【領域外】
struct TXST_WRAP {
  enum {
    REPEAT = 0, // 繰り返し
    MIRROR,     // 鏡面繰り返し
    BORDER,     // 設定された固有境界値
    CLAMP       // テクスチャの境界値
  };

  PicaDataTextureWrap getCommand( int idx )
  {
    static const PicaDataTextureWrap tbl[] = {
		  PICA_DATA_TEXTURE_WRAP_REPEAT,
		  PICA_DATA_TEXTURE_WRAP_MIRRORED_REPEAT,
		  PICA_DATA_TEXTURE_WRAP_CLAMP_TO_BORDER,
      PICA_DATA_TEXTURE_WRAP_CLAMP_TO_EDGE
    };
    if( idx > CLAMP ){ return tbl[0]; }

    return tbl[idx];
  }
};

//---------------------------------------------------------------------------------------------------------------------------------------------------
// コンバイナ
//---------------------------------------------------------------------------------------------------------------------------------------------------
// 演算手法構造体
struct convinerFormula {
  int idxMathFormula;
  int idxScale;
  int idxSourceA;
  int idxOperandA;
  int idxSourceB;
  int idxOperandB;
  int idxSourceC;
  int idxOperandC;

  int idxInputBufferFlag;

  convinerFormula()
  {
    idxMathFormula = idxScale = idxSourceA = idxOperandA = idxSourceB = idxOperandB = idxSourceC = idxOperandC = idxInputBufferFlag = 0;
  }
};

//-----------------------------------------------
// コンバイナステージ構造体
struct convinerStage {
  u32 constantColIdx;

  convinerFormula color;
  convinerFormula alpha;

  convinerStage()
  {
    constantColIdx = 0;
  }
};

//-----------------------------------------------
// コンバイナ構造体
struct ConvinerData {
  u32Color constantCol[CONV_CONSTANTCOL_MAX];
  u32Color bufferCol;
  u32Color textureBorderCol[CONV_TEXTURE_MAX];
 
  convinerStage stage[CONV_STAGE_MAX];

  ConvinerData()
  {
    // stage1以降はdefaultのソースを「前段の出力結果」に指定
    for(int i=1; i<CONV_STAGE_MAX; ++i){
      stage[i].color.idxSourceA = stage[i].alpha.idxSourceA = CONV_MATH_SRC::PREVIOUS;
    }
  }
};


//---------------------------------------------------------------------------------------------------------------------------------------------------
// レンダーステート
//---------------------------------------------------------------------------------------------------------------------------------------------------
// ブレンド操作構造体
struct BrenderData {
  int       idxEqRGB;
  int       idxFuncSrcRGB;
  int       idxFuncDstRGB;
  int       idxEqA;
  int       idxFuncSrcA;
  int       idxFuncDstA;
  u32Color  constantCol;

  bool      isEnable;
  u8        padding[3];

  BrenderData()
  {
    isEnable = false;
    idxEqRGB = idxFuncSrcRGB = idxFuncDstRGB = idxEqA = idxFuncSrcA = idxFuncDstA = 0;
  }
};

//-----------------------------------------------
// アルファテスト操作構造体
struct AlphaTestData {
  int     idxFunc;

  bool    isEnable;
  u8Value refValue;
  u8      padding[2];

  AlphaTestData()
  {
    isEnable = false;
    idxFunc = 0;
  }
};

//-----------------------------------------------
// デプステスト操作構造体
struct DepthTestData {
  int   idxFunc;

  bool  isEnable;
  bool  isEnableWrite;
  u8    padding[2];

  DepthTestData()
  {
    isEnable = isEnableWrite = true;
    idxFunc = RDST_DEPTHTEST::LESS;
  }
};

//-----------------------------------------------
// ステンシルテスト操作構造体
struct StencilTestData {
  int   idxFunc;
  u32   refValue;
  u32   mask;
  int   idxOpFail;
  int   idxOpZFail;
  int   idxOpZPass;

  bool  isEnable;
  u8    maskOp;
  u8    padding[2];

  StencilTestData()
  {
    isEnable = false;
    maskOp = 0xff;
    refValue = mask = 0;
    idxFunc = idxOpFail = idxOpZFail = idxOpZPass = 0;
  }
};

//-----------------------------------------------
// カリングモード操作構造体
struct CullModeData {
  int   mode;

  bool  isEnable;
  u8    padding[3];

  CullModeData()
  {
    isEnable = false;
    mode = 0;
  }
};

//-----------------------------------------------
// ポリゴンオフセット操作構造体
struct PolyOffsData {
  f32   offset;

  bool  isEnable;
  u8    padding[3];

  PolyOffsData()
  {
    isEnable = false;
    offset = 0;
  }
};

//-----------------------------------------------
// レンダーステート構造体
struct RenderStateData {
  BrenderData     brend;
  AlphaTestData   alphaTest;
  DepthTestData   depthTest;
  StencilTestData stencilTest;
  CullModeData    cullMode;
  PolyOffsData    polyOffs;

};

//---------------------------------------------------------------------------------------------------------------------------------------------------
// テクスチャーステート（サンプラー）
//---------------------------------------------------------------------------------------------------------------------------------------------------
// テクスチャースロット構造体
struct TextureSlotData {
  int       idxMinMagFilter;
  int       idxWrapU;
  int       idxWrapV;
  u32Color  borderCol;

  TextureSlotData()
  {
    idxMinMagFilter = 0;
    idxWrapU = 0;
    idxWrapV = 0;
  }
};

//-----------------------------------------------
// テクスチャーステート構造体
struct TextureStateData {
  TextureSlotData slot[3];  // スロット0～2

};

//===================================================================================================================================================
// 
//  ctr.gfbvsh フォーマット準拠シェーダーコマンドバイナリクラス
//
//  ctr_VertexShaderAssembler32.exe(コンパイラ)
// 	ctr_VertexShaderLinker32.exe(リンカ)
//  によって作成されたシェーダーバイナリを参照し、実行用のコマンドに変換した後
//  ctr.gfbvshファイルフォーマットのデータを作成する
// 
//===================================================================================================================================================
class CtrGfbvshDummy
{
public:
//-----------------------------------------------
// コンストラクタ
	CtrGfbvshDummy()
  {
    m_CtrGfbvsh = NULL;
    m_CtrGfbvshSize = 0;
  }

//-----------------------------------------------
// デストラクタ
  ~CtrGfbvshDummy()
  {
    if( m_CtrGfbvsh ){
      delete[] m_CtrGfbvsh;

      m_CtrGfbvsh = NULL;
      m_CtrGfbvshSize = 0;
    }
  }

//-----------------------------------------------
// 内部変数
  c8*   m_CtrGfbvsh;      // ctr.gfbvshに相当するデータ
  u32   m_CtrGfbvshSize;  // ctr.gfbvshに相当するデータのサイズ

//-----------------------------------------------
// バイナリ読み込み
  void Setup( char* filePath, char* shaderName, bool useGeometryShader );
};

//===================================================================================================================================================
// 
// 
// 
//  ctr.gfbfsh フォーマット準拠シェーダーコマンドバイナリクラス
//
//  コンバイナ操作を、実行用のコマンドに変換した後
//  ctr.gfbfshファイルフォーマットのデータを作成する
// 
// 
// 
//===================================================================================================================================================
class CtrGfbfshDummy
{
public:
//-----------------------------------------------
// コンストラクタ
	CtrGfbfshDummy()
  {
    m_CtrGfbfsh = NULL;
    m_CtrGfbfshSize = 0;
  }

//-----------------------------------------------
// デストラクタ
  ~CtrGfbfshDummy()
  {
    if( m_CtrGfbfsh ){
      delete[] m_CtrGfbfsh;

      m_CtrGfbfsh = NULL;
      m_CtrGfbfshSize = 0;
    }
  }

//-----------------------------------------------
// 内部変数
  c8*   m_CtrGfbfsh;      // ctr.gfbfshに相当するデータ
  u32   m_CtrGfbfshSize;  // ctr.gfbfshに相当するデータのサイズ

//-----------------------------------------------
// バイナリ読み込み
  //void Setup( char* filePath, char* shaderName )
  void Setup( char* shaderName );

  ConvinerData      m_convinerData;
  RenderStateData   m_renderStateData;
  TextureStateData  m_textureStateData;

};

#endif //USE_AURA_SHADER




