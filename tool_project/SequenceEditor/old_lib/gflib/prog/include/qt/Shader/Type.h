#ifndef GFL_MAYA_GLINTERFACE_TYPE_H_
#define GFL_MAYA_GLINTERFACE_TYPE_H_

#include <GL/glew.h> // glew

#define PLATFORM_WIN

#define DECLARE_ENUM_BEGIN( CLASSNAME )									\
	class CLASSNAME {													\
	public:																\
		enum Enum {
#define DECLARE_ENUM_END( CLASSNAME )									\
		};																\
	};


typedef	unsigned char		UChar;
typedef	unsigned short		UShort;
typedef	unsigned int		UInt;
typedef	unsigned long		ULong;
typedef	char				Int8, int8;
typedef	unsigned char		UInt8, uint8;
typedef	short				Int16, int16;
typedef	unsigned short		UInt16, uint16;
typedef	int					Int32, int32;
typedef	unsigned int		UInt32, uint32;
typedef	float				Float32;
typedef unsigned long long int ulong128;

typedef unsigned long long int ulong128;
typedef	long long			Int64, int64;
typedef	unsigned long long	UInt64, uint64;

#define SAFE_DELETE( p )       { if( p ) { delete ( p );		(p)=0; } }
#define SAFE_DELETE_ARRAY( p ) { if( p ) { delete[] ( p );		(p)=0; } }

namespace gfl { namespace maya { namespace GLInterface {

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! ストリームのUsage
	DECLARE_ENUM_BEGIN( StreamUsage )

		Source0 = 0,
		Source1,
		Source2,
		Source3,
		Source4,
		Source5,
		Source6,
		Source7,
		Source8,
    Source9,
    Source10,
		Source11,
		Source12,
		NumberOf,

		//  alias
		Position			  = Source0,
		Color					  = Source1,
		TexCoord0			  = Source2,
    TexCoord1			  = Source3,
    TexCoord2			  = Source4,
		Normal			    = Source5,
		Tangent0			  = Source6,
		Tangent1			  = Source7,
		Tangent2			  = Source8,
		JointIndex			= Source9,
		JointWeight			= Source10,
		OrigNormal      = Source11,
		OrigTangent     = Source12,

	DECLARE_ENUM_END( StreamUsage )

	//-------------------------------------------------------------------------------------------------------------------------------------------------
    //!  座標のデータ型
	DECLARE_ENUM_BEGIN( VertexType )
		Float	= GL_FLOAT,			//! 32bit浮動少数値
		Short	= GL_SHORT,			//! 16bit符号付整数
		UByte	= GL_UNSIGNED_BYTE,	//! 8bit符号なし整数値
		Byte	= GL_BYTE,			//! 8bit符号付整数値
		NumberOf = 4

	DECLARE_ENUM_END( VertexType )

	//-----------------------------------------------------------------------------
	//! テキストオブジェクト引数タイプ
	DECLARE_ENUM_BEGIN( ShaderType )
		Vertex = 0,
		Pixel,
		Geometry,
		NumberOf
	DECLARE_ENUM_END( ShaderType )

	//-----------------------------------------------------------------------------
	//! メモリ配置フラグ
    DECLARE_ENUM_BEGIN( Pool )
        Default = 0,    //! 最適なメモリ
        System,         //! メインメモリ
        Managed,        //! 必要に応じてメモリ配置を変更します
        NumberOf
    DECLARE_ENUM_END( Pool )
	
	//-----------------------------------------------------------------------------
    //! インデクスのデータ型
    DECLARE_ENUM_BEGIN( IndexType )
		Index8	= GL_UNSIGNED_BYTE,		//! 8bit符号なし整数値
		Index16	= GL_UNSIGNED_SHORT,	//! 16bit符号なし整数値
        NumberOf = 2
    DECLARE_ENUM_END( IndexType )

	//-----------------------------------------------------------------------------
    //! 使用用途フラグ
    DECLARE_ENUM_BEGIN( Usage )

        None = 0,

        //  サーフェイスとテクスチャに有効
        RenderTarget    = (1 << 0),     //! レンダリング対象に使う
        DepthStencil    = (1 << 1),     //! 深度ステンシルバッファに使う
        Swizzled        = (1 << 2),     //! @brief テクスチャがスウィズルフォーマットとして作成される\n
                                        //! テクスチャフェッチはやや高速だが、レンダリング対象としてはやや低速\n
                                        //! このフォーマットを指定する場合、解像度の幅と高さがそれぞれ２のべき乗である必要がある。
        Tiled           = (1 << 3),     //! @brief テクスチャがタイルドフォーマットとして作成される\n
                                        //! テクスチャフェッチは中速で、レンダリング対象としては最高速\n
                                        //! このフォーマットを指定する場合、キューブテクスチャはミップマップ禁止でかつ、解像度の幅と高さがそれぞれ１６の倍数である必要がある。
        ZCull           = (1 << 4),     //! @brief ZCull領域を設定する(PS3専用)\n
                                        //! このフラグを指定する場合、サーフェイスは必ずローカルメモリでなくてはいけない。

        //  テクスチャのみに有効
        AutoGenMipmap   = (1 << 8),     //! ミップマップを自動生成する

        //  テクスチャとバッファ両方に有効
        Dynamic         = (1 << 16),     //! @brief 頂点バッファのメモリ配置を動的に行うことを示します\n
                                        //! このフラグを指定しないとメモリ配置は静的に行われます\n
                                        //! Usage::Dynamic と Pool::Managed は同時に設定できません。\n
                                        //! 毎フレーム(もしくはそれ以上の頻度で)書き換わるような場合に適しています。
        WriteOnly       = (1 << 17)     //! @brief 頂点バッファへの書き込み操作しか実行しないことをシステムに知らせます\n
                                        //! つまりロック時にバッファの読み取りはできません

    DECLARE_ENUM_END( Usage )

	//-----------------------------------------------------------------------------
    //! ロックフラグ
    DECLARE_ENUM_BEGIN( Lock )

        Discard       = (1 << 0),   //! @brief 以前のバッファの内容を破棄してロックします。このためメモリ転送が停止することはありません。\n
                                    //! メモリを新たに用意するのでバッファの部分更新はできません\n
                                    //! Usage::Dynamic を指定されたバッファにしか適用できません。
        DoNotWait     = (1 << 1),   //! メモリ転送中やレンダリング中などメモリにアクセスできない場合、ロック可能になるまで待つのではなく、即座にエラーコードを返します
        NoOverWrite   = (1 << 2),   //! アプリケーションがメモリ転送やレンダリングに使用中のバッファを上書きしないことを明示します。\n
                                    //! Usage::Dynamic を指定されたバッファにしか適用できません。
        ReadOnly      = (1 << 3)    //! @brief アプリケーションがバッファに書き込まないことを明示します。\n
                                    //! ロック後の処理(ピクセルフォーマット変換など)を省略できます。

    DECLARE_ENUM_END( Lock )

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief プリミティブの種類
	DECLARE_ENUM_BEGIN( PrimitiveType )

		Triangles		= GL_TRIANGLES,			//! トライアングル
		TriangleStrip	= GL_TRIANGLE_STRIP,	//! トライアングルストリップ
		TriangleFan		= GL_TRIANGLE_FAN,		//! トライアングルファン

		NumberOf = 3

	DECLARE_ENUM_END( PrimitiveType )

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief テクスチャタイプ
	DECLARE_ENUM_BEGIN( TextureType )

		Image = GL_TEXTURE_2D,
		Cube = GL_TEXTURE_CUBE_MAP,

		NumberOf

	DECLARE_ENUM_END( TextureType )

//-------------------------------------------------------------------------------------------------------------------------------------------------

	//! @brief サーフェイスフォーマット
	DECLARE_ENUM_BEGIN( Format )

		A8R8G8B8	= GL_RGBA,
		R8G8B8		= GL_RGB,
		NumberOf

	DECLARE_ENUM_END( Format )

	//-----------------------------------------------------------------------------
	//! シェーダーハンドル
	typedef GLuint					ShaderObjectHandle;
	typedef GLuint					ShaderProgramHandle;
	typedef unsigned int			ShaderParameterHandle;
	typedef GLuint					BufferObjectHandle;
	typedef GLuint					TextureHandle;
	//typedef CGparameter			ShaderParameterHandle;
	//typedef CGcontext				ShaderObjectHandle;
	//typedef CGprogram				ShaderProgramHandle;

	//-----------------------------------------------------------------------------
	/**
	 * @struct	VertexBufferDesc
	 * @brief	頂点バッファの詳細情報
	 */
    struct VertexBufferDesc
    {
        UInt32		usage;		//! 使用用途フラグ
        UInt32		size;		//! バッファサイズ
		Pool::Enum	pool;		//! メモリ配置
		UInt8		pad[3];
    };

	//-----------------------------------------------------------------------------
	/**
	 * @struct	IndexBufferDesc
	 * @brief	インデクスバッファの詳細情報
	 */
    struct IndexBufferDesc
    {
        UInt32				usage;	//! 使用用途フラグ
        UInt32				size;	//! バッファサイズ
		IndexType::Enum		type;	//! タイプ
		Pool::Enum			pool;	//! メモリ配置
		UInt8				pad;
    };

	//-----------------------------------------------------------------------------
	/**
	 * @struct	VertexElement
	 * @brief	パイプラインに送られる入力頂点の定義型
	 */
	struct VertexElement {
		static const UInt8			StreamEnd = 0xFF;

		UInt8				Stream;		// ストリーム番号
		UInt8				Offset;		// オフセット
		UInt16				VertexType;	// データ型
		UInt8				Count;		// 1頂点の要素数
		UInt8				Size;		// 1頂点のサイズ
		UInt8				Stride;		// 1頂点のストライドサイズ
		UInt8				StreamUsage;// Usage
	};

	//-----------------------------------------------------------------------------
	/**
	 * @struct	TextureDesc
	 * @brief	テクスチャの詳細情報
	 */
	struct TextureDesc
	{
		UInt32		usage;          //! 使用用途フラグ
		UInt16		width;          //! 幅
		UInt16		height;         //! 高さ
		UInt16		format;         //! ピクセルフォーマット
		UInt16		type;           //! テクスチャの種類
		UInt8		depth;          //! 深度
		UInt8		mipLevels;      //! ミップマップ数
		UInt8		pool;           //! メモリ配置
		Int8		pad;
	};

	//-----------------------------------------------------------------------------
	/**
	 * @struct	TgaFormat
	 * @brief	TgaFormat情報
	 */
# pragma pack (1)//ちょっと反則
	struct TgaFormat
	{
		unsigned char	No_ID;			/* 0=イメージＩＤ無し*/
		unsigned char	CMap_Type;		/* 0=カラーマップ無し */
		unsigned char	Image_Type;		/* 2=無圧縮RGBタイプ 10=圧縮RGBタイプ */
		unsigned char	CMap_Spec[5];	/* スキップされる*/

		unsigned short	Ox;				/* 画面 左上 X 0固定 */
		unsigned short	Oy;				/* 画面 左上 Y 0固定 */

		unsigned short	Width;			/* 幅 (dot)*/
		unsigned short	Hight;			/* 高さ (dot)*/

		unsigned char	Pixel;			/* Pixel Size. 32=32bit */
		unsigned char	IDesc_Type;		/* 8=TGA32 アトリビュート */
	};
# pragma pack ()

	#define			RGBA32(r, g, b, a)	(a << 24) | (b << 16) | (g << 8) | (r << 0)



}}}

#undef		DECLARE_ENUM_BEGIN
#undef		DECLARE_ENUM_END

#endif
