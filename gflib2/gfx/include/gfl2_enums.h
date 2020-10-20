#ifdef PLATFORM_CLR

	#define		STRUCT		public ref struct
	#define		CLASS		public ref class
	#define		ENUM		enum class
	#define		LITERAL		

	#define		NAME_SPACE_BEGIN		namespace gfl2 { namespace clr { namespace gfx {
	#define		NAME_SPACE_END			}}}

#define DECLARE_ENUM_BEGIN( CLASSNAME )                           \
	CLASS CLASSNAME {																								\
	public:																													\
		ENUM Enum {
#define DECLARE_ENUM_END( CLASSNAME, DEFAULTVALUE )								\
		};																														\
	public:																													\
		CLASSNAME(){}																									\
		explicit CLASSNAME( int value ) : value_( Enum( value ) ) {}	\
		CLASSNAME( Enum value ) : value_( value ) {}									\
		operator Enum () LITERAL { return value_; }										\
		void Set( Enum value ){value_ = value;}												\
		Enum Get() LITERAL {return value_;}														\
	private:																												\
		Enum value_;																									\
	};

#else

	#define		STRUCT		struct
	#define		CLASS		class
	#define		ENUM		enum
	#define		LITERAL		const

	#define		NAME_SPACE_BEGIN		namespace gfl2 { namespace gfx {
	#define		NAME_SPACE_END			}}

#define DECLARE_ENUM_BEGIN( CLASSNAME )                           \
	CLASS CLASSNAME {																								\
	public:																													\
		ENUM Enum {
#define DECLARE_ENUM_END( CLASSNAME, DEFAULTVALUE )								\
		};																														\
	public:																													\
		CLASSNAME() : value_(DEFAULTVALUE) {}													\
		explicit CLASSNAME( int value ) : value_( Enum( value ) ) {}	\
		CLASSNAME( Enum value ) : value_( value ) {}									\
		operator Enum () LITERAL { return value_; }										\
		void Set( Enum value ){value_ = value;}												\
		Enum Get() LITERAL {return value_;}														\
	private:																												\
		Enum value_;																									\
	};

#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



NAME_SPACE_BEGIN

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief ディスプレイモード (現在は主にレンダリング解像度として使用されている。要整理)
	DECLARE_ENUM_BEGIN( DisplayMode )

		//! @brief 指定なし\n
		//! @par PLAYSTATION3
		//! Defaultを指定してディスプレイを初期化した場合、\n
		//! 本体に設定されているプライマリのディスプレイフォーマットを使用する。
		Default = 0,

		//  TV出力( VGA出力の場合はウィンドウで代用 )
		SDTV,				   //!< 720x480(NTSC) or 720x576(PAL) or 480p(HDMI)
		HD720,				  //!< 720p  1280x720  D4
		HD1080,				 //!< 1080p 1920x1080 D5

		//  VGA出力
		QVGA,				   //!< VGA   320x240
		FullScreenQVGA,		 //!< VGA   320x240(FullScreen)
		VGA,					//!< VGA   640x480
		FullScreenVGA,		  //!< VGA   640x480(FullScreen)
		SVGA,				   //!< SVGA  800x600
		FullScreenSVGA,		 //!< SVGA  800x600(FullScreen)
		XGA,					//!< XGA   1024x768
		FullScreenXGA,		  //!< XGA   1024x768(FullScreen)
		SXGA,				   //!< SXGA  1280x1024
		FullScreenSXGA,		 //!< SXGA  1280x1024(FullScreen)
		UXGA,				   //!< UXGA  1600x1200
		FullScreenUXGA,		 //!< UXGA  1600x1200(FullScreen)
		WUXGA,				  //!< WUXGA 1920x1200
		FullScreenWUXGA,		//!< WUXGA 1920x1200(FullScreen)
		QXGA,				   //!< QXGA  2048x1536
		FullScreenQXGA,		 //!< QXGA  2048x1536(FullScreen)
		WQXGA,				  //!< WQXGA 2560x1600
		FullScreenWQXGA,		//!< WQXGA 2560x1600(FullScreen)
    UserSetting,        //! < UserSettingSize

    // CTR用に追加
    CTR_LEFT,           //!< 3DSの左画面 400x240
    CTR_RIGHT,          //!< 3DSの右画面 400x240
    CTR_DOWN,           //!< 3DSの下画面 320x240
    
		NumberOf,

		HDTVBegin = HD720,
		VGABegin  = QVGA,

		//legacy
		HD480I = SDTV,			//!< 720x480(NTSC) or 720x576(PAL) or 480p(HDMI)
		HD480P = SDTV,			//!< 720x480(NTSC) or 720x576(PAL) or 480p(HDMI)
		HD576I = SDTV,			//!< 720x480(NTSC) or 720x576(PAL) or 480p(HDMI)
		HD576P = SDTV,			//!< 720x480(NTSC) or 720x576(PAL) or 480p(HDMI)
		HD720P = HD720,			//!< 720p  1280x720  D4
		HD1080I = HD1080,		//!< 1080p 1920x1080 D5
		HD1080P = HD1080,		//!< 1080p 1920x1080 D5

	DECLARE_ENUM_END( DisplayMode, Default )

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief CTRのディスプレイ番号
	DECLARE_ENUM_BEGIN( CtrDisplayNo )

    LEFT = 0,       //!< 3DSの左画面 400x240
    RIGHT,          //!< 3DSの右画面 400x240
    DOWN,           //!< 3DSの下画面 320x240
    
		NumberOf,

	DECLARE_ENUM_END( CtrDisplayNo, LEFT )

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief サーフェイスフォーマット
	DECLARE_ENUM_BEGIN( Format )

		Unknown = -1,
		None = 0,

		//  surface		 [surface]	   [texture]
		//! 各色に 5 ビットを使用する 16 ビットのピクセル形式。
		X1R5G5B5,	   //  X1R5G5B5		X1R5G5B5
		//! R 5ビット、G 6ビット、B 5ビットをのピクセル形式。
		R5G6B5,		 //  R5G6B5		  R5G6B5
		//! 各色に 8 ビットを使用する 32 ビットの RGB ピクセル形式。
		X8R8G8B8,	   //  X8R8G8B8		X8R8G8B8
		//! アルファ付きの、チャネルごとに 8 ビットを使用する 32 ビットの ARGB ピクセル形式。 
		A8R8G8B8,	   //  A8R8G8B8		A8R8G8B8		(PS3:displayable)
		//! ８ビットのBのみ。
		B8,			 //  B8			  B8
		//! ８ビットのGとBのみ。
		G8B8,		   //  G8B8			G8B8
		//! アルファ付きの、チャネルごとに 16 ビットを使用する 64　ビットのHalfFloatのARGBピクセル形式
		A16B16G16R16F,  //  A16B16G16R16F   A16B16G16R16F   (PS3:displayable)
		//! アルファ付きの、チャネルごとに 32 ビットを使用する 128　ビットのFloatのARGBピクセル形式
		A32B32G32R32F,  //  A32B32G32R32F   A32B32G32R32F
		//! 32ビットのFloat
		R32F,		   //  R32F			R32F
		//! 各色に 8 ビットを使用する 32 ビットの BGR ピクセル形式。
		X8B8G8R8,	   //  X8B8G8R8		(X8R8G8B8 + PS3:remap)
		//! アルファ付きの各色 8 ビットを使用する 32 ビットの ABGR ピクセル形式。
		A8B8G8R8,	   //  A8B8G8R8		(A8R8G8B8 + PS3:remap)

		//  depth stencil
		//! 16 ビットの Z バッファ
		D16,			//  D16			 D16
		//! 16 ビットFloatの Z バッファ
		D16F,		   //  D16F			D16F
		//! 深度チャネルに 24 ビットを使用する 32 ビットの z バッファ。
		D24X8,		  //  D24S8		   D24S8		   (PS3:Ｚステンシルバッファが３２ビットの場合ステンシルバッファを切れない)
		//! 深度チャネルに 24 ビット、ステンシル チャネルに 8 ビットを使用する 32 ビットの Z バッファ。 
		D24S8,		  //  D24S8		   D24S8
		//! 深度チャネルに 24 ビットFloat、ステンシル チャネルに 8 ビットを使用する 32 ビットの Z バッファ。 
		D24S8F,		 //  D24S8F		  D24S8F

		//  texture
		//! R 6ビット、G 5ビット、B 5ビットをのピクセル形式。
		R6G5B5,		 //  (none)		  R6G5B5
		//! G, Rに16ビットFloatを使用する32ビットのピクセル形式。
		G16R16F,		//  (none)		  G16R16F
		//! G, Rに16ビットを使用する32ビットのピクセル形式。
		G16R16,		 //  (none)		  G16R16
		//! Rに16ビットを使用するピクセル形式。
		R16,			//  (none)		  R16
		//! Aに1ビット、RGBに５ビットを使用する32ビットのピクセル形式。
		A1R5G5B5,	   //  (none)		  A1R5G5B5
		//! アルファ付きの各色 4 ビットを使用する 16 ビットの ARGB ピクセル形式。
		A4R4G4B4,	   //  (none)		  A4R4G4B4
		//! Aに1ビット、RGBに５ビットを使用する16ビットのRGBAピクセル形式。
		R5G5B5A1,	   //  (none)		  R5G5B5A1
		//! DXT1圧縮フォーマット
		DXT1,		   //  (none)		  DXT1
		//! DXT3圧縮フォーマット
		DXT3,		   //  (none)		  DXT3
		//! DXT5圧縮フォーマット
		DXT5,		   //  (none)		  DXT5

		//! V8U8フォーマット(バンプマップに使用)
		V8U8,		   //  (none)		  V8U8

		X2R10G10B10,
		A2R10G10B10,
		X8R8G8B8_LE,
		A8R8G8B8_LE,
		X2R10G10B10_LE,
		A2R10G10B10_LE,

    // CTR用に追加
    R8G8B8,
    L8A8,
    H8L8,
    L8,
    A8,
    L4A4,
    L4,
    A4,
    ETC1,
    ETC1_A4,
    SHADOW,
    GAS,
    REF,

		NumberOf,

	DECLARE_ENUM_END( Format, None )

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief マルチサンプル
	DECLARE_ENUM_BEGIN( MultiSample )

		None = 0,	   //!< マルチサンプルなし
		MSAA2X,		 //!< ２×２サンプル
		MSAA4X,		 //!< ４×４サンプル
		MSAA8X,		 //!< ８×８サンプル
		MSAA16X,		//!< １６×１６サンプル

		NumberOf,

	DECLARE_ENUM_END( MultiSample, None )

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief メモリ配置フラグ(次期ハードの内容によって見直します。)
	DECLARE_ENUM_BEGIN( Pool )

		Default = 0,	//!< 最適なメモリ
		System,		 //!< メインメモリ
		Managed,		//!< 必要に応じてメモリ配置を変更します
		LocalA,			//!< ３ＤＳ用
		LocalB,			//!< ３ＤＳ用

		NumberOf,

	DECLARE_ENUM_END( Pool, Default )

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief フィルモード
	DECLARE_ENUM_BEGIN( FillMode )

		Line = 0,		   //!< ライン
		Fill,		   //!< フィル

		NumberOf,

	DECLARE_ENUM_END( FillMode, Line )

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief テクスチャタイプ
	DECLARE_ENUM_BEGIN( TextureType )

		Image = 0,		//!< ２Ｄイメージ
		Volume,			//!< ボリュームテクスチャ
		Cube,			//!< キューブテクスチャ

		NumberOf,

	DECLARE_ENUM_END( TextureType, Image )

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief 使用用途フラグ(次期ハードの内容によって見直します。)
	DECLARE_ENUM_BEGIN( Usage )

		None = 0,

		//  サーフェイスとテクスチャに有効
		RenderTarget	= (1 << 0),	 //!< レンダリング対象に使う
		DepthStencil	= (1 << 1),	 //!< 深度ステンシルバッファに使う
		Swizzled		= (1 << 2),	 //!< @brief テクスチャがスウィズルフォーマットとして作成される\n
										//!< テクスチャフェッチはやや高速だが、レンダリング対象としてはやや低速\n
										//!< このフォーマットを指定する場合、解像度の幅と高さがそれぞれ２のべき乗である必要がある。
		Tiled		   = (1 << 3),	 //!< @brief テクスチャがタイルドフォーマットとして作成される\n
										//!< テクスチャフェッチは中速で、レンダリング対象としては最高速\n
										//!< このフォーマットを指定する場合、キューブテクスチャはミップマップ禁止でかつ、解像度の幅と高さがそれぞれ１６の倍数である必要がある。
		ZCull		   = (1 << 4),	 //!< @brief ZCull領域を設定する(PS3専用)\n
										//!< このフラグを指定する場合、サーフェイスは必ずローカルメモリでなくてはいけない。
		TiledJoint	  = (1 << 5),	 //!< @brief Tiled 共有を設定する(PS3専用)\n
										//!< Tiled フラグも有効になっている必要がある。

		//  テクスチャのみに有効
		AutoGenMipmap   = (1 << 8),	 //!< ミップマップを自動生成する

		//  テクスチャとバッファ両方に有効
		Dynamic		 = (1 << 16),	 //!< @brief 頂点バッファのメモリ配置を動的に行うことを示します\n
										//!< このフラグを指定しないとメモリ配置は静的に行われます\n
										//!< Usage::Dynamic と Pool::Managed は同時に設定できません。\n
										//!< 毎フレーム(もしくはそれ以上の頻度で)書き換わるような場合に適しています。
		WriteOnly	   = (1 << 17),	//!< @brief 頂点バッファへの書き込み操作しか実行しないことをシステムに知らせます\n
										//!< つまりロック時にバッファの読み取りはできません

	DECLARE_ENUM_END( Usage, None )

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief ファイルの種類
	DECLARE_ENUM_BEGIN( FileFormat )

		Unknown = 0,		//!< 不明

		ShaderCode,			//!< シェーダコード
		ShaderBinary,	  //!< シェーダバイナリ

		NumberOf,

	DECLARE_ENUM_END( FileFormat, Unknown )

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief 座標のデータ型
	DECLARE_ENUM_BEGIN( VertexType )

		ShortNormalized = 0,	//!< 16bit符号付整数を[-1...1]に正規化した値
		Float,								//!< 32bit浮動少数値
		Half,									//!< 16bit浮動少数値
		UByteNormalized,			//!< 8bit符号なし整数を[0...1]に正規化した値
		Short,								//!< 16bit符号付整数
		CompressNormalized,		//!< 32bit(11:11:10)に圧縮した整数を[-1...1]に正規化した値(PS3のみ)
		UByte,								//!< 8bit符号なし整数値
		ByteNormalized6,			//!< 8bit固定小数値(小数部6bit)
		ShortNormalized12,		//!< 16bit固定小数値(小数部12bit)
    SByte,                //!< 8bit符号付整数値

		NumberOf,

	DECLARE_ENUM_END( VertexType, ShortNormalized )

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief インデクスのデータ型
	DECLARE_ENUM_BEGIN( IndexType )

		Index16 = 0,	//!< 16bit符号なし整数値
		Index32,			//!< 32bit符号なし整数値
    Index8,       //!< 8bit符号なし整数値

		NumberOf,

	DECLARE_ENUM_END( IndexType, Index16 )

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief 定数のデータ型
	DECLARE_ENUM_BEGIN( ConstantType )

		Vector = 0,
		Matrix,
		Bool,
		Int,
		Float,
		BoolArray,
		IntArray,
		FloatArray,

		NumberOf,

	DECLARE_ENUM_END( ConstantType, Vector )

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief ストリームの用途
	DECLARE_ENUM_BEGIN( StreamUsage )

		Position = 0,
		Color,
		TexCoord,
		Normal,
		Tangent,
		BiNormal,
		BlendWeight,
		BlendIndices,

		NumberOf,

	DECLARE_ENUM_END( StreamUsage, Position )

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief プリミティブの種類
	DECLARE_ENUM_BEGIN( PrimitiveType )

		Points = 0,			//!< ポイント
		LineStrip,			//!< ラインストリップ
		Lines,					//!< ライン
		Triangles,			//!< トライアングル
		TriangleStrip,	//!< トライアングルストリップ

		NumberOf,

	DECLARE_ENUM_END( PrimitiveType, Points )

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief 比較関数
	DECLARE_ENUM_BEGIN( CompareFunc )

		Less = 0,			//!< リファレンス値より小さい場合にのみパスさせる
		LessEqual,		//!< リファレンス値と同じか小さい場合にパスさせる
		Equal,				//!< リファレンス値と同じ場合にはパスさせる
		GreaterEqual,	//!< リファレンス値と同じか大きい場合にパスさせる
		Greater,			//!< リファレンス値よりも大きい場合にパスさせる
		NotEqual,			//!< リファレンス値と同じではない場合にパスさせる
		Always,				//!< 常にパスさせる
		Never,				//!< パスさせない

		NumberOf,

	DECLARE_ENUM_END( CompareFunc, Less )

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief ブレンド係数
	//! ( Rs, Gs, Bs, As ) ソース値\n
	//! ( Rd, Gd, Bd, Ad ) デスティネーション値\n
	//! ( R0, G0, B0, A0 ) setBlendColor() で設定された値
	DECLARE_ENUM_BEGIN( BlendFunc )

		Zero = 0,							//!< ( R, G, B, A ) = ( 0, 0, 0, 0 )
		One,									//!< ( R, G, B, A ) = ( 1, 1, 1, 1 )
		SrcColor,							//!< ( R, G, B, A ) = ( Rs, Gs, Bs, As )
		InverseSrcColor,			//!< ( R, G, B, A ) = ( 1 - Rs, 1 - Gs, 1 - Bs, 1 - As )
		DestColor,						//!< ( R, G, B, A ) = ( Rd, Gd, Bd, Ad )
		InverseDestColor,			//!< ( R, G, B, A ) = ( 1 - Rd, 1 - Gd, 1 - Bd, 1 - Ad )
		SrcAlpha,							//!< ( R, G, B, A ) = ( As, As, As, As )
		InverseSrcAlpha,			//!< ( R, G, B, A ) = ( 1 - As, 1 - As, 1 - As, 1 - As )
		DestAlpha,						//!< ( R, G, B, A ) = ( Ad, Ad, Ad, Ad )
		InverseDestAlpha,			//!< ( R, G, B, A ) = ( 1 - Ad, 1 - Ad, 1 - Ad, 1 - Ad )
		ConstantColor,
		InverseConstantColor,
		ConstantAlpha,
		InverseConstantAlpha,
		SrcAlphaSaturate,			//!< ( R, G, B, A ) = ( min( As, 1 - Ad ), min( As, 1 - Ad ), min( As, 1 - Ad ), 1 )

		NumberOf,

	DECLARE_ENUM_END( BlendFunc, Zero )

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief ブレンド演算式
	//! ( Rs, Gs, Bs, As ) ソース値\n
	//! ( Rd, Gd, Bd, Ad ) デスティネーション値\n
	//! ( Rx, Gx, Bx, Ax ) ソース係数値\n
	//! ( Ry, Gy, By, Ay ) デスティネーション係数値
	DECLARE_ENUM_BEGIN( BlendOp )

		Add = 0,					//!< ( R, G, B, A ) = ( Rs * Rx + Rd * Ry, Gs * Gx + Gd * Gy, Bs * Bx + Bd * By, As * Ax + Ad * Ay )
		Subtract,					//!< ( R, G, B, A ) = ( Rs * Rx - Rd * Ry, Gs * Gx - Gd * Gy, Bs * Bx - Bd * By, As * Ax - Ad * Ay )
		ReverseSubtract,	//!< ( R, G, B, A ) = ( Rd * Ry - Rs * Rx, Gd * Gy - Gs * Gx, Bd * By - Bs * Bx, Ad * Ay - As * Ax )
		Min,							//!< ( R, G, B, A ) = ( min( Rs * Rd ), min( Gs * Gd ), min( Bs * Bd ), min( As * Ad ) )
		Max,							//!< ( R, G, B, A ) = ( MAX( Rs * Rd ), MAX( Gs * Gd ), MAX( Bs * Bd ), MAX( As * Ad ) )

		NumberOf,

	DECLARE_ENUM_END( BlendOp, Add )

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief 背面カリングモード
	DECLARE_ENUM_BEGIN( CullMode )

		None = 0,		   //!< 背面カリングしない
		CW,				 //!< 背面を時計回りでカリング
		CCW,				//!< 背面を反時計回りでカリング

		NumberOf,

	DECLARE_ENUM_END( CullMode, None )

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief ポリゴン面の裏表フラグ
	DECLARE_ENUM_BEGIN( PolygonFace )

		CCW = 0,	//!< 反時計回り(デフォルトでは表)
		CW,				//!< 時計回り(デフォルトでは裏)

		NumberOf,

	DECLARE_ENUM_END( PolygonFace, CCW )

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief ステンシル処理
	DECLARE_ENUM_BEGIN( StencilOp )

		Keep = 0,		  //!< 更新しない
		Zero,					//!< ０に設定
		Replace,			//!< 参照値で置き換える
		Increment,		//!< 最大値に達するまで増加
		IncrementWrap,//!< 増加させその値が最大値を超えると０に戻します。
		Decrement,		//!< ０になるまで減少
		DecrementWrap,//!< 減少させその値が０より小さくなった場合は最大値に戻します。
		Invert,				//!< ビットを反転

		NumberOf,

	DECLARE_ENUM_END( StencilOp, Keep )

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief テクスチャアドレス
	DECLARE_ENUM_BEGIN( TextureAddress )

		Wrap = 0,	   //!< テクスチャサイズで割った余りを座標値としてとる。\nテクスチャを繰り返しパターンとして使用することになる
		Mirror,		 //!< テクスチャサイズごとに折り返した座標値をとる。\nテクスチャを反転しながら繰り返すパターンとして使用することになる
		Border,		 //!< テクスチャ境界のテクセル値を読み出す。
		Clamp,		  //!< [0, 1]の範囲にクランプした座標値をとる。

		NumberOf,

	DECLARE_ENUM_END( TextureAddress, Wrap )

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief テクスチャフィルタ
	DECLARE_ENUM_BEGIN( TextureFilter )

		None = 0,
		Point = None,	//!< 近傍ピクセルサンプリング
		Linear,				//!< バイリニアフィルタリング

		NumberOf,

	DECLARE_ENUM_END( TextureFilter, None )

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//!  ShaderType
	DECLARE_ENUM_BEGIN( ShaderType )
		Vertex = 0,
		Geometry,
		Pixel,
		NumberOf,
	DECLARE_ENUM_END( ShaderType, Vertex )


  //-------------------------------------------------------------------------------------------------------------------------------------------------
	//  その他
  static const unsigned int										RenderTargetMax = 8;

NAME_SPACE_END

#undef		STRUCT
#undef		CLASS
#undef		ENUM
#undef		LITERAL

#undef		NAME_SPACE_BEGIN
#undef		NAME_SPACE_END

#undef		DECLARE_ENUM_BEGIN
#undef		DECLARE_ENUM_END

