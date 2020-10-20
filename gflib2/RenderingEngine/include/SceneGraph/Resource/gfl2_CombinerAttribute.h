#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_COMBINERATTRIBUTE_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_COMBINERATTRIBUTE_H_INCLUDED

#include <stdio.h>
#include <types/include/gfl2_Typedef.h>

#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>

#define DECLARE_ENUM_BEGIN( CLASSNAME )                           \
	class CLASSNAME {                                               \
	public:																                          \
		enum Enum {
#define DECLARE_ENUM_END( CLASSNAME, DEFAULTVALUE )               \
		};																                            \
	public:																                          \
		CLASSNAME(): value_(DEFAULTVALUE) {}                          \
		explicit CLASSNAME( int value ) : value_( Enum( value ) ) {}	\
		CLASSNAME( Enum value ) : value_( value ) {}					        \
		operator Enum () const { return value_; }					            \
	private:															                          \
		Enum value_;													                        \
	};

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  コンバイナタイプ
DECLARE_ENUM_BEGIN( CombinerColorType )
	RGB	= 0,
	A,
	NumberOf
DECLARE_ENUM_END( CombinerColorType, RGB )

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  参照テーブルタイプ
DECLARE_ENUM_BEGIN( LookUpTableType )
	Distribution0 = 0,
	Distribution1,
	ReflectionR,
	ReflectionG,
	ReflectionB,
	Fresnel,
	NumberOf
DECLARE_ENUM_END( LookUpTableType, Distribution0 )

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  入力ソースタイプ
DECLARE_ENUM_BEGIN( InputSourceType )
	A	= 0,
	B,
	C,
	NumberOf
DECLARE_ENUM_END( InputSourceType, A )


//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  ブレンディングタイプ
DECLARE_ENUM_BEGIN( BlendModeType )
	Blend	= 0,
	Separate,
	LogicOp,
	None,
	NumberOf
DECLARE_ENUM_END( BlendModeType, Blend )

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  
DECLARE_ENUM_BEGIN( BlendEquationType )
	Add = 0,
  Subtract,
  ReverseSubtract,
  Min,
  Max,
	NumberOf
DECLARE_ENUM_END( BlendEquationType, Add )

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  ブレンド関数タイプ
DECLARE_ENUM_BEGIN( BlendFuncType )
	Zero = 0,
  One,
  SourceColor,
  OneMinusSourceColor,
  DestinationColor,
  OneMinusDestinationColor,
  SourceAlpha,
  OneMinusSourceAlpha,
  DestinationAlpha,
  OneMinusDestinationAlpha,
  ConstantColor,
  OneMinusConstantColor,
  ConstantAlpha,
  OneMinusConstantAlpha,
  SourceAlphaSaturate,
	NumberOf
DECLARE_ENUM_END( BlendFuncType, Zero )

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  演算タイプ
DECLARE_ENUM_BEGIN( MathFormulaType )
	Replace,
	Modulate,
	Add,
	AddSigned,
	Interpolate,
	Subtract,
	DotProduct3Rgb,
	DotProduct3Rgba,
	AddMult,
	MultAdd,
	NumberOf
DECLARE_ENUM_END( MathFormulaType, Replace )

	static const int s_MathFormulaSourceNum[MathFormulaType::NumberOf] = {
	1, //Replace,
	2, //Modulate,
	2, //Add,
	2, //AddSigned,
	3, //Interpolate,
	2, //Subtract,
	2, //DotProduct3Rgb,
	2, //DotProduct3Rgba,
	3, //AddMult,
	3  //MultAdd,
};

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  入力タイプ
DECLARE_ENUM_BEGIN( CombInputSourceType )
	Texture0 = 0,
	Texture1,
	Texture2,
	Texture3,
	Constant,
	PrimaryColor,
	FragmentPrimaryColor,
	FragmentSecondaryColor,
	Previous,
	PreviousBuffer,
	NumberOf
DECLARE_ENUM_END( CombInputSourceType, Texture0 )

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  入力要素タイプ
DECLARE_ENUM_BEGIN( OperandType )
	Color = 0,
	OneMinusColor,
	Alpha,
	OneMinusAlpha,
	Red,
	OneMinusRed,
	Green,
	OneMinusGreen,
	Blue,
	OneMinusBlue,
	NumberOf
DECLARE_ENUM_END( OperandType, Color )

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  入力角タイプ
DECLARE_ENUM_BEGIN( LutInputAngleType )
	NH = 0,//法線とハーフベクトル （デフォルト）
	VH,//ビューベクトルとハーフベクトル
	NV,//法線とビューベクトル
	LN,//ライトベクトルと法線
	SP,//ライトベクトルの逆ベクトルとスポットライトの方向ベクトル（RR, RG, RB, FR
	CP,//ハーフベクトルの接平面への投影と接線（RR, RG, RB, FR は不可）
	NumberOf
DECLARE_ENUM_END( LutInputAngleType, NH )

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  参照テーブルライティング結果スケールタイプ
DECLARE_ENUM_BEGIN( LutScaleType )
	x0_25,
	x0_5,
	x1_0,
	x2_0,
	x4_0,
	x8_0,
	NumberOf
DECLARE_ENUM_END( LutScaleType, x0_25 )

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  深度テストタイプ
DECLARE_ENUM_BEGIN( DepthTestType )
	LESS = 0,
	LEQUAL,
	EQUAL,
	GEQUAL,
	GREATER,
	NOTEQUAL,
	ALWAYS,
	NEVER,
	NumberOf
DECLARE_ENUM_END( DepthTestType, LESS )

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  ステンシルテストタイプ
DECLARE_ENUM_BEGIN( StencilTestType )
	LESS = 0,
	LEQUAL,
	EQUAL,
	GEQUAL,
	GREATER,
	NOTEQUAL,
	ALWAYS,
	NEVER,
	NumberOf
DECLARE_ENUM_END( StencilTestType, LESS )

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  ステンシルオペレーションタイプ
DECLARE_ENUM_BEGIN( StencilOpType )
	KEEP = 0,		//	ステンシル値は現在の値を保持します。
	ZERO,				//	ステンシル値は0で置き換えられます。
	REPLACE,		//	ステンシル値はglStencilFuncのrefで指定した値で置き換えられます。
	INCR,				//	ステンシル値は1加算されます。
	INCR_WRAP,	//	ステンシル値は1加算されます。
	DECR,				//	ステンシル値は1減算されます。
	DECR_WRAP,	//	ステンシル値は1減算されます。
	INVERT,			//	ステンシル値はビット反転します。
	NumberOf
DECLARE_ENUM_END( StencilOpType, KEEP )

//"Keep" "Zero" "Replace" "Increase" "IncreaseWrap" "Decrease" "DecreaseWrap" "Invert"

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  ステンシル値タイプ
DECLARE_ENUM_BEGIN( StencilValue )
	PressShadow = 247,
	NumberOf
DECLARE_ENUM_END( StencilValue, PressShadow )

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  アルファテストタイプ
DECLARE_ENUM_BEGIN( AlphaTestType )
	LESS = 0,
	LEQUAL,
	EQUAL,
	GEQUAL,
	GREATER,
	NOTEQUAL,
	ALWAYS,
	NEVER,
	NumberOf
DECLARE_ENUM_END( AlphaTestType, LESS )

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  ブレンドタイプ
DECLARE_ENUM_BEGIN( BlendType )
	None = 0,
	Nuki,			//! 抜き設定
	Liner,		//! 線形補完
	Add,			//! 加算
	Sub,			//! 減算
	Mul,			//! 乗算
	Inverse,	//! ネガポジ反転
	NumberOf
DECLARE_ENUM_END( BlendType, None )

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  ブレンド関数タイプ
DECLARE_ENUM_BEGIN( BlendFunc )
	SrcAddDst = 0,
	SrcSubDst,
	DstSubSrc,
	Min,
	Max,
	NumberOf
DECLARE_ENUM_END( BlendFunc, SrcAddDst )

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  ブレンド関数タイプ
DECLARE_ENUM_BEGIN( BlendElement )
	Zero = 0,
	One,
	CombinerRGB,
	OneSubCombinerRGB,
	FrameBufferRGB,
	OneSubFrameBufferRGB,
	CombinerA,
	OneSubCombinerA,
	FrameBufferA,
	OneSubFrameBufferA,
	BlendRGB,
	OneSubBlendRGB,
	BlendA,
	OneSubBlendA,
	Min,
	NumberOf
DECLARE_ENUM_END( BlendElement, Zero )

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  ポケモン法線タイプ
DECLARE_ENUM_BEGIN( PokeNormalType )
	None = 0,
	Dummy,
	ObjectSpace,
	NumberOf
DECLARE_ENUM_END( PokeNormalType, None )

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  ポケモン法線タイプ
DECLARE_ENUM_BEGIN( LutInputAngle )
	NH = 0,//法線とハーフベクトル （デフォルト）
	VH,//ビューベクトルとハーフベクトル
	NV,//法線とビューベクトル
	LN,//ライトベクトルと法線
	NumberOf
DECLARE_ENUM_END( LutInputAngle, NH )

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  エッジタイプ(ユーザーアトリビュート)
DECLARE_ENUM_BEGIN( EdgeType )
	Default = 0,
	VColor,
	//Dot,
	None,
	Erase,
	OwnColor,
	Deco,
	NumberOf
DECLARE_ENUM_END( EdgeType, Default )

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  表示レイタータイプ
DECLARE_ENUM_BEGIN( DrawLayer )
	//----------- デフォルトレイヤ ------------
	Layer0	= 0,    // 不透明
	Layer1,         // 半透明
	Layer2,
	Layer3,

	//----------- 屈折表現等で用いるレイヤ ------------
	Layer4,
	Layer5,
	Layer6,
	Layer7,
	NumberOf
DECLARE_ENUM_END( DrawLayer, Layer0 )

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  表示レイヤータイプ
DECLARE_ENUM_BEGIN( MappingType )
	UvCoordinateMap	= 0,
	CameraCubeEnvMap,
	CameraSphereEnvMap,
	ProjectionMap,
	NumberOf
DECLARE_ENUM_END( MappingType, UvCoordinateMap )

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  プロジェクションタイプ
DECLARE_ENUM_BEGIN( ProjectionType )
	None	= 0,
	DropShadow,
	DropLight,
	NumberOf
DECLARE_ENUM_END( ProjectionType, None )

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  テクスチャのベイクタイプ
DECLARE_ENUM_BEGIN( TextureBakeType )
	None	= 0,
	AtEffectCol,
	AtAll,
	AtLight,
	NumberOf
DECLARE_ENUM_END( TextureBakeType, None )

class VertexShaderType
{
public:
	enum Enum {
    Default = 0,                //0
    Poke = (1 << 0),            //1
    PokeNormal = (1 << 1),      //2
    PokeFire = (1 << 2),        //4
    MegaGangar = (1 << 3),      //8
    Contrast = (1 << 4),        //16
    Default1 = (1 << 5),        //32
    D01 = (1 << 6),             //64
    D01ProjTex = (1 << 7),      //128
    PressShadow = (1 << 8),     //256
    Obon = (1 << 9),            //512
    Kusa = (1 << 10),           //1024
    FieldChar = (1 << 11),      //2048
    FillScrean = (1 << 12),     //4096
    Reflect = (1 << 13),        //8192
    BattleChar = (1 << 14),     //16384
    Evolution = 16385,          //16385
    Particle,                   //16386
    RotateCubeMap,              //16387
    NwShader,                   //16388
    BattleFieldShader,          //16389
    PokeFireShadow,             //16390
    PressShadowTextureAlpha,    //16391
    PressShadowYAlpha,          //16392
    RotateSphereMap,            //16393
    MegaYamirami,               //16394
    VsShading,                  //16395
    VsFog,                      //16396
    NormalToUv,                 //16397
    BattleFieldShader20,        //16398
	};

	VertexShaderType( void ) : m_Value(Default){}
	explicit VertexShaderType( Enum value ) : m_Value( value ) {}
	operator Enum ( void ) const { return (Enum)m_Value; }

	static Enum GetValueByIndex( u32 index )
	{
     const Enum vertexShaderTypes[] =
    {
      Default,                    //0
      Poke,                       //1
      PokeNormal,                 //2
      PokeFire,                   //4
      MegaGangar,                 //8
      Contrast,                   //16
      Default1,                   //32
      D01,                        //64
      D01ProjTex,                 //128
      PressShadow,                //256
      Obon,                       //512
      Kusa,                       //1024
      FieldChar,                  //2048
      FillScrean,                 //4096
      Reflect,                    //8192
      BattleChar,                 //16384
      Evolution,                  //16385
      Particle,                   //16386
      RotateCubeMap,              //16387
      NwShader,                   //16388
      BattleFieldShader,          //16389
      PokeFireShadow,             //16390
      PressShadowTextureAlpha,    //16391
      PressShadowYAlpha,          //16392
      RotateSphereMap,            //16393
      MegaYamirami,               //16394
      VsShading,                  //16395
      VsFog,                      //16396
      NormalToUv,                 //16397
      BattleFieldShader20,        //16398
    };

		 return vertexShaderTypes[index];
	}

	bool IsPokemonShader() const
	{
		switch( this->m_Value ){
		case Poke:
		case PokeNormal:
		case MegaGangar:
		case PokeFire:
		case MegaYamirami:
			return true;
		default:
			return false;
		}
	}

  bool operator ==(VertexShaderType::Enum types) const
  {
    return (static_cast<VertexShaderType::Enum>(this->m_Value) == types);
  }

  bool operator !=(VertexShaderType::Enum types) const
  {
    return (static_cast<VertexShaderType::Enum>(this->m_Value) != types);
  }

  VertexShaderType::Enum operator =(VertexShaderType::Enum types)
  {
    this->m_Value = types;
    return types;
  }

private:
	u32 m_Value;
};

class CombinerAttribute
{
public:	
	static const s32    TextureMax = 3;
	static const s32    CombinerMax = 6;
  static const s32    ConstantMax = 6;
	static const s32    LookUpTableIndex = 9;		// 参照テーブルは9番から

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//!  カラーテクスチャとして使うテクスチャ
	DECLARE_ENUM_BEGIN( UvTextures )
		None = -1,			// テクスチャ未使用の際などに使う
		Texture0 = 0,
		Texture1,
		Texture2,
		NumberOf
	DECLARE_ENUM_END( UvTextures, Texture0 )

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//!  エッジマップ抜きとして使うテクスチャ
	DECLARE_ENUM_BEGIN( EdgeMapAlpha )
		None = -1,			// テクスチャ未使用の際などに使う
		Texture0A = 0,
		Texture1A,
		Texture2A,
		Texture0R,
		Texture1R,
		Texture2R,
		NumberOf
	DECLARE_ENUM_END( EdgeMapAlpha, Texture0A )

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//!  
	DECLARE_ENUM_BEGIN( TextureNode )
		Texture0 = 0,
		Texture1,
		Texture2,
		LutTextureD0,
		LutTextureD1,
		LutTextureRR,
		LutTextureRG,
		LutTextureRB,
		LutTextureFR,
		HighLightTexture,
		OutLineColorTexture,
		BlurScaleTexture,
		NumberOf
	DECLARE_ENUM_END( TextureNode, Texture0 )

	static const int GetTextureStateNo(TextureNode textureNode)
	{
		// 9番目から参照テーブルのインデックスなので, それに合わせた配列をここで用意
		const int textureStateNo[TextureNode::NumberOf] = {	0, 1, 2, 
																												9, 10, 11, 12, 13, 14,
																												8, 15, 14 };
		return textureStateNo[textureNode];
	}

	struct TextureImage{
		float							layerdAlpha;//レイヤテクスチャ時の補間値
		short							layerdBlendMode;

    TextureImage()
    {
			layerdAlpha = 0.0f;
			layerdBlendMode = -1;
    }
    
  };

	struct LutTableImage{
    unsigned char     lut[256];

    LutTableImage()
    {
      ::std::memset( lut, 0xFF, sizeof(lut) );
    }
    
  };

	struct MaterialColor{
		math::Vector   m_Emission;
		math::Vector   m_Ambient;
		math::Vector   m_Diffuse;
		math::Vector   m_Specular0;
		math::Vector   m_Specular1;
	};

	struct EtcColor{
		math::Vector   m_BlendColor;
		math::Vector   m_BufferColor;
		math::Vector   m_TextureBorderColor[TextureMax];
	};

	struct TextureCombinerAttribute{
		short   m_MathFormula;
		int		  m_Scale;
		short   m_InputSource[InputSourceType::NumberOf];
		short   m_Operand[InputSourceType::NumberOf];
		bool		m_OutPutBuffer;
	};

	struct LookUpTable{
		short   m_LutInputAngle;
		short   m_LutScale;

		LookUpTable() : m_LutInputAngle(0),m_LutScale(0){}
	};

	struct TextureAttribute{
		short   m_MappingType;
		short   m_UvSetNo;
		float   m_ScaleU;
		float   m_ScaleV;
		float   m_Rotate;
		float   m_TranslateU;
		float   m_TranslateV;
		short   m_RepeatU;
		short   m_RepeatV;
		short   m_MagFilter;
		short   m_MinFilter;
	};

	struct BlendFunc{
		short											m_BlendFunc;
		short											m_SrcElement;
		short											m_DstElement;
	};

	struct AttributeParam{
		int													m_CombinerStageMax;

		bool												m_PsLightingEnable;
		bool												m_VsLightingEnable;
		short												m_LightSetNo;
		bool												m_FogEnable;
		short												m_FogNo;
		short												m_CullMode;
		
		MaterialColor								m_MaterialColor;													//カラー/マテリアルカラー
		math::Vector								m_ConsntantColor[ConstantMax];						//カラー/コンスタントカラー
		EtcColor										m_EtcColor;																//カラー/その他のカラー

		short												m_BumpMapNo;															//テクスチャ/バンプマッピング/バンプ使用テクスチャ
		short												m_BumpMapType;														//テクスチャ/バンプマッピング/使用方法
		bool												m_ZRecastEnable;													//テクスチャ/バンプマッピング/Ｚ成分の再計算

		TextureAttribute						m_TextureAttribute[TextureMax];						//テクスチャ一覧/テクスチャ
	  bool												m_TextureAttributeEnable[TextureMax];

		bool												m_LookUpTableEnable[LookUpTableType::NumberOf];
		LookUpTable									m_LookUpTable[LookUpTableType::NumberOf];	//フラグメント/フラグメントライティング
		bool												m_PrimaryFresnelEnable;										//フラグメント/フラグメントライティング/フレネルをプライマリに適応
		bool												m_SecondaryFresnelEnable;									//フラグメント/フラグメントライティング/フレネルをセカンダリに適応
		bool												m_SpecularColorClamp;											//フラグメント/フラグメントライティング/スペキュラをクランプ
		bool												m_CookTorrance0Enable;										//フラグメント/フラグメントライティング/ジオメトリファクタ０
		bool												m_CookTorrance1Enable;										//フラグメント/フラグメントライティング/ジオメトリファクタ０

		TextureCombinerAttribute		m_TextureCombinerAttribute[CombinerColorType::NumberOf][CombinerMax];	//フラグメント/テクスチャコンバイナ
		short												m_TextureCombinerConstant[ConstantMax];																//フラグメント/テクスチャコンバイナ/各コンスタントカラー

		short												m_DrawLayer;															//ブレンド/テスト/描画レイヤー
		short												m_AlphaTestType;													//ブレンド/テスト/アルファテスト/満たせば通過
		float												m_AlphaTestRefValue;											//ブレンド/テスト/アルファテスト/参照値
		short												m_DepthTestType;													//ブレンド/テスト/デプステスト/満たせば通過
		bool												m_UpDateDepth;														//ブレンド/テスト/デプステスト/デプスバッファを更新する
		int													m_PolygonOffSet;													//ブレンド/テスト/デプステスト/ポリゴンオフセット

		short												m_StencilTestEnable;											//ブレンド/テスト/ステンシルテスト/
		short												m_StencilTestType;												//ブレンド/テスト/ステンシルテスト/
		short												m_StencilValue;														//ブレンド/テスト/ステンシルテスト/
		short												m_StencilMask;														//ブレンド/テスト/ステンシルテスト/
		short												m_StencilFailOp;													//ブレンド/テスト/ステンシルテスト/
		short												m_StencilZFailOp;													//ブレンド/テスト/ステンシルテスト/
		short												m_StencilZpassOp;													//ブレンド/テスト/ステンシルテスト/


		short												m_BlendMode;															//ブレンド/テスト/ブレンダー/ブレンドモード
		BlendFunc										m_BlendFunc[CombinerColorType::NumberOf];	//ブレンド/テスト/ブレンダー/フレンド式各種
		short												m_LogicalOperation;												//ブレンド/テスト/ブレンダー/論理演算方法

		VertexShaderType						m_VertexShaderType;

		bool												m_UseObjectSpaceNormalMap;

		bool												m_UseShadowTexture;
		bool												m_HighLightTextureEnable;
		bool												m_IsPokemonShader;//CR様の対応が完了したら削除


		math::Vector								m_VsRimParam;
		short												m_PokeNormalType;
		short												m_BlendType;
		bool												m_UpdateShaderParam;
		math::Vector								m_VsShaderParam0123;
	};

private:
	CombinerAttribute(){}
	~CombinerAttribute(){}
};

#undef		DECLARE_ENUM_BEGIN
#undef		DECLARE_ENUM_END

}}}}

#endif
