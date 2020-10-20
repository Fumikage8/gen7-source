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
	//! @brief ポケモンの大きさカテゴリー
  DECLARE_ENUM_BEGIN( PokemonSize )
    S = 0,
    M,
    L,
    NumberOf
  DECLARE_ENUM_END( PokemonSize )

NAME_SPACE_END

#undef		STRUCT
#undef		CLASS
#undef		ENUM
#undef		LITERAL

#undef		NAME_SPACE_BEGIN
#undef		NAME_SPACE_END

#undef		DECLARE_ENUM_BEGIN
#undef		DECLARE_ENUM_END

