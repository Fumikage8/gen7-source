#ifdef PLATFORM_CLR

	#define		STRUCT		public ref struct
	#define		CLASS		public ref class
	#define		ENUM		enum class
	#define		LITERAL		

	#define		NAME_SPACE_BEGIN		namespace gfl2 { namespace clr { namespace renderingengine {
	#define		NAME_SPACE_END			}}}

#else

	#define		STRUCT		struct
	#define		CLASS		class
	#define		ENUM		enum
	#define		LITERAL		const

	#define		NAME_SPACE_BEGIN		namespace gfl2 { namespace renderingengine {
	#define		NAME_SPACE_END			}}

#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define DECLARE_ENUM_BEGIN( CLASSNAME )														\
	CLASS CLASSNAME {																								\
	public:																													\
		ENUM Enum {
#define DECLARE_ENUM_END( CLASSNAME )															\
		};																														\
	public:																													\
		CLASSNAME(){}																									\
		explicit CLASSNAME( int value ) : value_( Enum( value ) ) {}	\
		CLASSNAME( Enum value ) : value_( value ) {}									\
		operator Enum () LITERAL { return value_; }										\
		void Set( Enum value ){value_ = value;}												\
		Enum Get(){return value_;}																		\
	private:																												\
		Enum value_;																									\
	};

NAME_SPACE_BEGIN

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief ライトの種類
	DECLARE_ENUM_BEGIN( LightType )
		Ambient = 0,
		Directional,
		Point,
		Spot,
		NumberOf
	DECLARE_ENUM_END( LightType )

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief 注視の種類
	DECLARE_ENUM_BEGIN( InterestType )
    None = 0,
    X,
    YZ,
    XZ,
    XY,
    ZY,
    Y,
    Z,
    X_M90,
    Z_180,
    Y_90,

		NumberOf
	DECLARE_ENUM_END( InterestType )
	
	//-------------------------------------------------------------------------------------------------------------------------------------------------	
	//! @brief ビルボードの種類
	DECLARE_ENUM_BEGIN( BillboardType )
		None = 0,			//!< ビルボードなし。
		ScreenY,			//!< スクリーンに対してY軸のみ回転
		ScreenXYZ,		//!< スクリーンに並行になるようにXYZ変換
		NumberOf,
	DECLARE_ENUM_END( BillboardType )

  //-------------------------------------------------------------------------------------------------------------------------------------------------	
  //!< @brief ポケデコの種類(ロケータノードの判別でGFModelViewer, nijiのいずれでも使用するのでここに定義
  DECLARE_ENUM_BEGIN( PokedecoType )
    BB_HAT = 0,  //!< @brief 帽子型チャーム(頭部)
    BB_RIBBON,   //!< @brief リボンチャーム(胸部)
    BB_GLASSES,  //!< @brief メガネ(顔)
    BB_BADGE,    //!< @brief ステータス系(バッジ, リボン)(胸部)
    NumberOf     //!< @brief 総数
  DECLARE_ENUM_END( PokedecoType )

  //-------------------------------------------------------------------------------------------------------------------------------------------------	
  //!< @brief 法線カラー出力用コンスタントID
  DECLARE_ENUM_BEGIN( FillScreenConstantType )
    RGB = 0,  //!< @brief RGB
    R,        //!< @brief R
    G,        //!< @brief G
    B,        //!< @brief B
    A,        //!< @brief A
    NumberOf  //!< @brief 総数
  DECLARE_ENUM_END( FillScreenConstantType )

  //-------------------------------------------------------------------------------------------------------------------------------------------------	
  //!< @brief 被写界深度の前面/背面設定ID
  DECLARE_ENUM_BEGIN( DofType )
    Front = 0,  //!< @brief 前面
    Back,       //!< @brief 背面
    NumberOf    //!< @brief 総数
  DECLARE_ENUM_END( DofType )

  //-------------------------------------------------------------------------------------------------------------------------------------------------	
  //!< @brief 定数
  DECLARE_ENUM_BEGIN( Constant )
    VERTEX_BUFFER_MAX = 8,  //!< @brief 頂点バッファ最大数
    BLEND_MESH_MAX = 2,     //!< @brief ブレンドシェイプメッシュ最大数
    MESH_NAME_MAX = 64,     //!< @brief メッシュ名最大文字数
    JOINT_NAME_MAX = 64,    //!< @brief ジョイント名最大文字数
    RESOURCE_FILE_NAME_MAX = 64,    //!< @brief リソースファイル名最大文字数
    MODEL_ELEMENT_NAME_MAX = 64,    //!< @brief モデルデータ内各要素名最大文字数
    LOCATOR_NAME_MAX = 64,          //!< @brief ロケーター名最大文字数
    MATERIAL_ATTRIBUTE_NAME_DATA_MAX = 64,    //!< @brief マテリアル内項目名最大文字数
    TEXTURE_MAX = 16,                         //!< @brief テクスチャ最大数
    FLAT_RESOURCE_NAME_MAX = 80,    //!< @brief 旧環境リソースファイル名最大文字数
  DECLARE_ENUM_END( Constant )

NAME_SPACE_END

#undef		STRUCT
#undef		CLASS
#undef		ENUM
#undef		LITERAL

#undef		NAME_SPACE_BEGIN
#undef		NAME_SPACE_END

#undef		DECLARE_ENUM_BEGIN
#undef		DECLARE_ENUM_END

