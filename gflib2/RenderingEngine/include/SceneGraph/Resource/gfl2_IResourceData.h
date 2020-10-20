#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_IRESOURCEDATA_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_IRESOURCEDATA_H_INCLUDED

#include <types/include/gfl2_Typedef.h>
#include <gfx/include/gfl2_GLMemory.h>

#define DECLARE_ENUM_BEGIN( CLASSNAME )														\
	class CLASSNAME {                                               \
	public:																                          \
		enum Enum {
#define DECLARE_ENUM_END( CLASSNAME )									            \
		};																                            \
	public:																                          \
		CLASSNAME(){}												                          \
		explicit CLASSNAME( int value ) : value_( Enum( value ) ) {}	\
		CLASSNAME( Enum value ) : value_( value ) {}					        \
		operator Enum () const { return value_; }					            \
	private:															                          \
		Enum value_;													                        \
	};

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  GfModelVersion
DECLARE_ENUM_BEGIN( GfModelVersion )
	FirstVersion	= 0x14110100,
	OptimizeAndAddUserData			= 0x15022717,
	AddWrapFilerState						= 0x15030321,
	AddLutHashBumpNo						= 0x15030920,
	SupportVsBoolMap						= 0x15031220,
	SupportConstantColorAnim		= 0x15031221,
	VertexShaderUpdate15031412	= 0x15031412,//! フィールド向けデフォルトシェーダー対応
	VertexShaderUpdate15032914	= 0x15032914,//! デフォルトシェーダーのスフィアマップ対応
	TextureCacheOpt15032918			= 0x15032918,//! テクスチャキャッシュの有効利用
	PokeShaderLutLightingFix		= 0x15041213,//! Pokeシェーダー使用時のLut調整対応
	VertexShaderUpdate15041820	= 0x15041820,//! Pokeシェーダー最適化
	EncountKusaUpdate15052316		= 0x15052316,//! エンカウント草モデル対応
	BillBordModeUpdate15052616	= 0x15052616,//! ビルボードモデル対応
	SegmentScale15061117				= 0x15061117,//! セグメントスケール対応
  BillboardType15072416       = 0x15072415,//! Billboard Data Added (2Bit, 0-3)
	MipMapBias15072420					= 0x15072420,//! MipMapBias対応
	MultiUvAdjust15090112				= 0x15090112,//! ＵＶ番号自動設定マルチＵＶサポート
	SupportShadowTexture15090718= 0x15090718,//! 影テクスチャ付きモデルのカラーシェーダーサポート
  SupportMaterialColor15101716= 0x15101716,//! マテリアルカラー対応（Emission, Ambient, Diffuse）
  MeshOptCTR15103116          = 0x15103116,//! ファイヤーシェーダー用メッシュのストリップ化はCTRのみとする
  ColorU8CTR15111314          = 0x15111314,//! ColorU8によるデータ圧縮
  FireShaderScaleCTR15112017  = 0x15112017,//! 頂点カラーによる1.0以上のファイヤーシェーダースケール対応
  PaddingColorIsWhite         = 0x15122117,//! 頂点カラーを持たないメッシュのパディングを白に変更

  //追加したらBinaryFileAccessor11::SetAttributeParam関数とMaterialResourceNodeFactory::CreateResourceNode関数内の追記を忘れずに

DECLARE_ENUM_END( GfModelVersion )

class IResourceData
{
public:

	union Type{
		Type():m_ID(0){ for(int n=0;n<sizeof(m_Name);n++) m_Name[n]=0; }

		Type( const c8* pName ):m_ID(0)
		{	
			for(int n=0;n<sizeof(m_Name);n++) m_Name[n]=0;
			Set(pName);
		}

		void Set( const c8* pName )
		{			
			

			for( u32 i = 0; i < sizeof(Type); ++i )
			{
				if ( !pName[i] )		break;
				m_Name[i] = pName[i];
			}
		}

		inline Type&  operator =( const Type& rhs )
		{
			m_ID = rhs.m_ID;
      return *this;
		}

		inline b32 operator ==( const Type& rhs ) const
		{
			return ( m_ID == rhs.m_ID );
		}

		inline b32 operator !=( const Type& rhs ) const
		{
			return ( m_ID != rhs.m_ID );
		}

		struct
		{
			c8			m_Name[8];
		};

		struct
		{
			u64			m_ID;
		};
	};

	//! @brief デストラクタ
	virtual ~IResourceData() {}

	//! @brief 全データの数の取得
	virtual s32 GetSize( void ) = 0;

	//! @brief データの取得
	virtual const void* GetData( u32 index ) = 0;

	//! @brief 個々のデータのサイズ取得
	virtual u32 GetDataSize( u32 index ) = 0;

	//! @brief タイプデータの取得
	virtual Type GetType( u32 index ) = 0;

	//! @brief ファイルタイプの取得
	virtual Type GetFileType( void ) = 0;

	//! @brief データを保持しているか？
  virtual bool HasData(void) = 0;

  //! @brief バージョンIDの取得
  virtual u32 GetVersionId( void ) = 0;

protected:
	//! @brief コンストラクタ
	IResourceData(){}
};

}}}}

#undef		DECLARE_ENUM_BEGIN
#undef		DECLARE_ENUM_END

#endif

