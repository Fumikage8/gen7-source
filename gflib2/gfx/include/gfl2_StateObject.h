#ifndef GFLIB2_GRX_STATEOBJECT_H_INCLUDED
#define GFLIB2_GRX_STATEOBJECT_H_INCLUDED

#include <stdio.h>
#include <types/include/gfl2_Typedef.h>
#include <gfx/include/gfl2_types.h>
#include <gfx/include/gfl2_GLMemory.h>
#include <gfx/include/gfl2_Color.h>

namespace gfl2 { namespace gfx {

template <class Implement_>
class GFGLBase;

//-----------------------------------------------------------
//! @brief ブレンドステートオブジェクト
//-----------------------------------------------------------
class BlendStateObject : public GLMemory
{
public:

  //!< @brief カラーの書き込みの設定
	enum COLOR_WRITE_ENABLE
  {
    //!< @brief 赤成分のみ
    COLOR_WRITE_ENABLE_RED    = 1,
    //!< @brief 青成分のみ
    COLOR_WRITE_ENABLE_GREEN  = 2,
    //!< @brief 緑成分のみ
    COLOR_WRITE_ENABLE_BLUE   = 4,
    //!< @brief アルファのみ
    COLOR_WRITE_ENABLE_ALPHA  = 8,
    //!< @brief RGBのみ(アルファは無視)
    COLOR_WRITE_ENABLE_RGB = ( ( ( COLOR_WRITE_ENABLE_RED | COLOR_WRITE_ENABLE_GREEN )  | COLOR_WRITE_ENABLE_BLUE ) ),
    //!< @brief RGBA全て
    COLOR_WRITE_ENABLE_ALL = ( ( ( COLOR_WRITE_ENABLE_RED | COLOR_WRITE_ENABLE_GREEN )  | COLOR_WRITE_ENABLE_BLUE )  | COLOR_WRITE_ENABLE_ALPHA ) 
  };

	template <class Implement_>
	friend class GFGLBase;

	virtual ~BlendStateObject(){}

	//! @brief ステートを初期状態にリセットする
	inline virtual void ResetRenderStates()
	{
		m_StateData.m_AlphaToCoverageEnable = false;
    m_StateData.m_AlphaCompareFunc = CompareFunc::Less;
		m_StateData.m_AlphaRef = 0.0f;
		m_StateData.m_AlphaTestEnable = false;
		m_StateData.m_BlendEnable = false;
		m_StateData.m_ColorBlendOp = BlendOp::Add;
		m_StateData.m_AlphaBlendOp = BlendOp::Add;
		m_StateData.m_SrcColorBlendFunc = BlendFunc::One;
		m_StateData.m_SrcAlphaBlendFunc = BlendFunc::One;
		m_StateData.m_DestColorBlendFunc = BlendFunc::Zero;
		m_StateData.m_DestAlphaBlendFunc = BlendFunc::Zero;
		for( u32 i = 0; i < RenderTargetMax; ++i )
		{
			m_StateData.m_RenderTargetWriteMask[i] = COLOR_WRITE_ENABLE_ALL;
		}
	}

	//! @brief レンダーターゲットマスクの設定
	inline virtual void SetRenderTargetWriteMask( u32 no, const u8 mask )
	{
		m_StateData.m_RenderTargetWriteMask[no] = mask;
	}
		
	//! @brief アルファテスト参照値の設定
	inline virtual void SetAlphaRef(
		const float       ref    //!< 入力アルファ値と比較する参照値( 0〜1 )
	)
	{
		m_StateData.m_AlphaRef = ref;
	}

  //! @brief アルファテスト参照値の取得
	inline float GetAlphaRef() const
	{
		return m_StateData.m_AlphaRef;
	}

  //! @brief アルファテスト比較関数の設定
  inline virtual void SetAlphaCompareFunc(CompareFunc func)
  {
    m_StateData.m_AlphaCompareFunc = func;
  }

  //!@ brief アルファテスト比較関数の取得
  inline CompareFunc GetAlphaCompareFunc() const
  {
    return m_StateData.m_AlphaCompareFunc;
  }
		
	//! @brief アルファテストの有効、無効の設定
	inline virtual void SetAlphaTestEnable(const b32 enable)
	{
		m_StateData.m_AlphaTestEnable = enable;
	}

	//! @brief 現在のアルファテストの取得
	inline b32 GetAlphaTestEnable() const
	{
		return m_StateData.m_AlphaTestEnable;
	}

	//! @brief ピクセルブレンドに使用する定数値の設定
	inline virtual void SetBlendEnable(const b32 enable)
	{
		m_StateData.m_BlendEnable = enable;
	}

	//! @brief 現在のピクセルブレンドに使用する定数値の取得
	inline b32 GetBlendEnable() const
	{
		return m_StateData.m_BlendEnable;
	}

	//! @brief ピクセルブレンド機能の演算式の指定
	inline virtual void SetBlendOp(
		const BlendOp color, //!< ＲＧＢのブレンド演算式
		const BlendOp alpha  //!< αのブレンド演算式
	)
	{
		m_StateData.m_ColorBlendOp = color;
		m_StateData.m_AlphaBlendOp = alpha;
	}

	//! @copydoc setBlendOp
	inline virtual void SetBlendOp( BlendOp op )
	{
		SetBlendOp( op, op );
	}

	//! @brief ブレンド機能の演算式の取得：ＲＧＢ
	inline BlendOp GetBlendOpColor() const
	{
    return m_StateData.m_ColorBlendOp;
	}

	//! @brief ブレンド機能の演算式の取得：α
	inline BlendOp GetBlendOpAlpha() const
	{
    return m_StateData.m_AlphaBlendOp;
	}

	//! @brief ブレンド係数値の計算式の指定
	inline virtual void SetBlendFunc(
		const BlendFunc sfc, //!< ソースＲＧＢ係数の計算式
		const BlendFunc dfc, //!< デストＲＧＢ係数の計算式
		const BlendFunc sfa, //!< ソースα係数の計算式
		const BlendFunc dfa  //!< デストα係数の計算式
	)
	{
		m_StateData.m_SrcColorBlendFunc = sfc;
		m_StateData.m_SrcAlphaBlendFunc = sfa;
		m_StateData.m_DestColorBlendFunc = dfc;
		m_StateData.m_DestAlphaBlendFunc = dfa;
	}

	//! @copydoc setBlendFunc
	inline virtual void SetBlendFunc(const BlendFunc src, const BlendFunc dest)
	{
		SetBlendFunc(src, dest, src, dest);
	}

	//! @brief ブレンドの計算式取得：ソースＲＧＢ
  inline BlendFunc GetBlendFuncSrcColor() const
  {
    return m_StateData.m_SrcColorBlendFunc;
  }

	//! @brief ブレンドの計算式取得：デストＲＧＢ
  inline BlendFunc GetBlendFuncDestColor() const
  {
    return m_StateData.m_DestColorBlendFunc;
  }

	//! @brief ブレンドの計算式取得：ソースα
  inline BlendFunc GetBlendFuncSrcAlpha() const
  {
    return m_StateData.m_SrcAlphaBlendFunc;
  }

	//! @brief ブレンドの計算式取得：デストα
  inline BlendFunc GetBlendFuncDestAlpha() const
  {
    return m_StateData.m_DestAlphaBlendFunc;
  }

	//! @brief Alpha To Coverageの設定
	inline virtual void SetAlphaToCoverageEnable(const bool enable)
	{
		m_StateData.m_AlphaToCoverageEnable = enable;
	}

	virtual void UpdateState() = 0;

protected:
	BlendStateObject() : GLMemory()
	{
		ResetRenderStates();
	}

	struct StateData{
		b32					m_AlphaToCoverageEnable;
    CompareFunc m_AlphaCompareFunc;
		f32					m_AlphaRef;
		b32					m_AlphaTestEnable;
		b32					m_BlendEnable;
		BlendOp			m_ColorBlendOp;
		BlendOp			m_AlphaBlendOp;
		BlendFunc		m_SrcColorBlendFunc;
		BlendFunc		m_SrcAlphaBlendFunc;
		BlendFunc		m_DestColorBlendFunc;
		BlendFunc		m_DestAlphaBlendFunc;
		u8					m_RenderTargetWriteMask[RenderTargetMax];
	};

	StateData				m_StateData;
};

//-----------------------------------------------------------
//! @brief ラスタライザステートオブジェクト
//-----------------------------------------------------------
class RasterizerStateObject : public GLMemory
{
public:

	template <class Implement_>
	friend class GFGLBase;

	virtual ~RasterizerStateObject(){}

	//! @brief ステートを初期状態にリセットする
	inline void ResetRenderStates()
	{
		m_StateData.m_CullMode = CullMode::CW;
		m_StateData.m_FillMode = FillMode::Fill;
		m_StateData.m_DepthBias = 0;
		m_StateData.m_MultisampleEnable = false;
	}

	//! @brief カリングモードを指定する
	//! @param func デフォルトは CullMode::CW
	inline void SetCullMode(const CullMode mode)
	{
		m_StateData.m_CullMode = mode;
	}

	//! @brief 現在のカリングモードを取得する
	inline CullMode GetCullMode()
	{
		return m_StateData.m_CullMode;
	}

	//! @brief フィルモードの指定( ポリゴン系にしか影響しません )
	inline void SetFillMode(const FillMode mode)
	{
		m_StateData.m_FillMode = mode;
	}

	//! @brief 深度バイアス係数を設定する\n
	//! ※ポリゴン描画のみ影響します。ラインやポイント描画には影響しません。
	inline void SetDepthBias(const u32 bias)
	{
		m_StateData.m_DepthBias = bias;
	}

	//! @brief ＭＳＡＡの有効、無効の設定
	//! ※レンダーターゲットをMultiSampleモードで作成しておく必要があります。
	inline void SetMultisampleEnable(const bool enable)
	{
		m_StateData.m_MultisampleEnable = enable;
	}

	virtual void UpdateState() = 0;

protected:
	RasterizerStateObject() : GLMemory()
	{
		ResetRenderStates();
	}

	struct StateData{
		CullMode					m_CullMode;
		FillMode					m_FillMode;
		u32								m_DepthBias;
		b32								m_MultisampleEnable;
	};

	StateData						m_StateData;
};

//-----------------------------------------------------------
//! @brief 深度ステンシルステートオブジェクト
//-----------------------------------------------------------
class DepthStencilStateObject : public GLMemory
{
public:

  struct StateData
  {
    CompareFunc   m_DepthFunc;
    b32           m_DepthWriteEnable;
    b32           m_DepthTestEnable;
    b32           m_StencilTestEnable;
    u8            m_SetStencilWriteMask;
    b32           m_TwoSideStencilTestEnable;
    u8            m_StencilRef;
    u8            m_StencilMask;
    CompareFunc   m_StencilFunc[PolygonFace::NumberOf];
    StencilOp     m_StencilFailOp[PolygonFace::NumberOf];
    StencilOp     m_StencilDepthFailOp[PolygonFace::NumberOf];
    StencilOp     m_StencilPassOp[PolygonFace::NumberOf];

    /**
     * @brief コンストラクタ
     * @note リセットの際の設定情報が設定される
     */
    StateData()
    {
      m_DepthFunc = CompareFunc::Less;
      m_DepthWriteEnable = true;
      m_DepthTestEnable = true;
      m_StencilTestEnable = false;
      m_SetStencilWriteMask = 0xFF;
      m_TwoSideStencilTestEnable = false;
      m_StencilRef = 0;
      m_StencilMask = 0xFF;
      for( int face = 0; face < PolygonFace::NumberOf; ++face )
      {
        m_StencilFunc[face] = CompareFunc::Always;
        m_StencilFailOp[face] = StencilOp::Keep;
        m_StencilDepthFailOp[face] = StencilOp::Keep;
        m_StencilPassOp[face] = StencilOp::Keep;
      }
    }
  };

	template <class Implement_>
	friend class GFGLBase;

	virtual ~DepthStencilStateObject(){}

	//! @brief ステートを初期状態にリセットする
	inline virtual void ResetRenderStates()
	{
		m_StateData.m_DepthFunc = CompareFunc::Less;
		m_StateData.m_DepthWriteEnable = true;
		m_StateData.m_DepthTestEnable = true;
		m_StateData.m_StencilTestEnable = false;
		m_StateData.m_SetStencilWriteMask = 0xFF;
		m_StateData.m_TwoSideStencilTestEnable = false;
		m_StateData.m_StencilRef = 0;
		m_StateData.m_StencilMask = 0xFF;

		for( int face = 0; face < PolygonFace::NumberOf; ++face )
		{
			m_StateData.m_StencilFunc[face] = CompareFunc::Always;
			m_StateData.m_StencilFailOp[face] = StencilOp::Keep;
			m_StateData.m_StencilDepthFailOp[face] = StencilOp::Keep;
			m_StateData.m_StencilPassOp[face] = StencilOp::Keep;
		}
	}

	//! @brief 深度テスト関数の指定
	inline virtual void SetDepthFunc(const CompareFunc func)
	{
		m_StateData.m_DepthFunc = func;
	}

	//! @brief 深度テスト関数の取得
	inline virtual CompareFunc GetDepthFunc() const
	{
		return m_StateData.m_DepthFunc;
	}

	//! @brief 深度テストの有効、無効の設定
	inline virtual void SetDepthWriteEnable(const b32 enable)
	{
		m_StateData.m_DepthWriteEnable = enable;
	}

	//! @brief 現在の深度書き込みフラグの取得
	inline b32 GetDepthWriteEnable() const
	{
		return m_StateData.m_DepthWriteEnable;
	}

	//! @brief 深度テストの有効、無効の設定
	inline virtual void SetDepthTestEnable(const b32 enable)
	{
		m_StateData.m_DepthTestEnable = enable;
	}

	//! @brief 現在の深度テストの取得
	inline virtual b32 GetDepthTestEnable() const
	{
		return m_StateData.m_DepthTestEnable;
	}
		
	//! @brief ステンシルテストの有効、無効の設定
	inline virtual void SetStencilTestEnable(const b32 enable)
	{
		m_StateData.m_StencilTestEnable = enable;
	}

	//! @brief ステンシルテストの有効、無効の設定
	inline b32 GetStencilTestEnable() const
	{
		return m_StateData.m_StencilTestEnable;
	}
		
	//! @brief 両面ステンシルテスト機能の有効、無効の設定
	inline virtual void SetTwoSideStencilTestEnable(const bool enable)
	{
		m_StateData.m_TwoSideStencilTestEnable = enable;
	}

	//! @brief ステンシルテスト関数の指定
	inline virtual void SetStencilFunc(
		const PolygonFace face, //!< 裏表のどちらの面か指定する
		const CompareFunc func, //!< アルファ比較関数
		const u8 ref,        //!< 入力アルファ値と比較する参照値
		const u8 mask        //!< テスト実行時に、マスク参照値と格納されているステンシル値の両方とAND演算するマスク
	)
	{
		SetStencilFunc( face, func );
		SetStencilReference( ref );
		SetStencilMask( mask );
	}

  //! @brief ステンシルテスト関数の設定
  inline void SetStencilFunc(const PolygonFace face, const CompareFunc func )
  {
    m_StateData.m_StencilFunc[face] = func;
  }

  //! @brief ステンシルテスト関数の取得
  inline CompareFunc GetStencilFunc(const PolygonFace face) const
  {
    return m_StateData.m_StencilFunc[face];
  }

  //! @brief ステンシルテスト参照値の設定
  inline void SetStencilReference( u8 value )
  {
    m_StateData.m_StencilRef = value;
  }

  //! @brief ステンシルテスト参照値の取得
  inline u8 GetStencilReference() const
  {
    return m_StateData.m_StencilRef;
  }

  //! @brief ステンシルテストマスク値の設定
  inline void SetStencilMask( u8 value )
  {
    m_StateData.m_StencilMask = value;
  }

  //! @brief ステンシルテストマスク値の取得
  inline u8 GetStencilMask() const
  {
    return m_StateData.m_StencilMask;
  }

	//! @brief 裏面描画時のステンシルプレーンの書き込み制御
	inline virtual void SetStencilWriteMask(
		const u8 bits        //!< 書き込み許可マスク
	)
	{
		m_StateData.m_SetStencilWriteMask = bits;
	}

	//! @brief 裏面描画時のステンシル処理の設定
	inline virtual void SetStencilOp(
		const PolygonFace face,    //!< 裏表のどちらの面か指定する
		const StencilOp fail,      //!< ステンシルテストにパスしなかった場合の動作
		const StencilOp depthFail, //!< ステンシルテストはパスしたが、デプステストにパスしなかった場合の処理
		const StencilOp depthPass  //!< ステンシルテストとデプステストの両方にパスした場合、\n
			                          //!< またはステンシルテストにパスし、デプスバッファまたはデプステストが有効でない場合のステンシルの処理
	)
	{
		m_StateData.m_StencilFailOp[face] = fail;
		m_StateData.m_StencilDepthFailOp[face] = depthFail;
		m_StateData.m_StencilPassOp[face] = depthPass;
	}

	inline StateData GetStateData() const { return m_StateData; }
	
  //! @brief ステンシルテスト失敗時処理の取得
  inline StencilOp GetStencilFailOp(const gfl2::gfx::PolygonFace face) const
  {
    return m_StateData.m_StencilFailOp[face];
  }

  //! @brief ステンシルテスト、デスプテスト失敗時処理の取得
  inline StencilOp GetStencilDepthFailOp(const gfl2::gfx::PolygonFace face) const
  {
    return m_StateData.m_StencilDepthFailOp[face];
  }

  //! @brief ステンシルテスト、成功時処理の取得
  inline StencilOp GetStencilPassOp(const gfl2::gfx::PolygonFace face) const
  {
    return m_StateData.m_StencilPassOp[face];
  }

	virtual void UpdateState() = 0;

protected:
	DepthStencilStateObject() : GLMemory()
	{
		ResetRenderStates();
	}

	StateData						m_StateData;
};

//-----------------------------------------------------------
//! @brief サンプラーステートオブジェクト
//-----------------------------------------------------------
class SamplerStateObject : public GLMemory
{
public:

	template <class Implement_>
	friend class GFGLBase;

	virtual ~SamplerStateObject(){}

	//! @brief ステートを初期状態にリセットする
	inline void ResetRenderStates()
	{
		m_StateData.m_WrapS					= TextureAddress::Wrap;
		m_StateData.m_WrapT					= TextureAddress::Wrap;
		m_StateData.m_WrapR					= TextureAddress::Wrap;
		m_StateData.m_SampleCmp			= CompareFunc::Always;
		m_StateData.m_BorderColor   = Color::GetWhite();
		m_StateData.m_MinLod				= 0;
		m_StateData.m_MaxLod				= 12;
		m_StateData.m_MaxAniso			= 1;
		m_StateData.m_MinFilter			= TextureFilter::Point;
		m_StateData.m_MagFilter			= TextureFilter::Point;
		m_StateData.m_MipFilter			= TextureFilter::None;
		m_StateData.m_MipmapLodBias	= 0.0f;
	}

	//! @brief テクスチャサンプラのアドレッシングモード
	inline void SetSamplerAddress(
		const TextureAddress  wrapS,
		const TextureAddress  wrapT,
		const TextureAddress  wrapR = TextureAddress::Wrap,
		const CompareFunc     sampleCmp = CompareFunc::Always
	)
	{
		m_StateData.m_WrapS					= wrapS;
		m_StateData.m_WrapT					= wrapT;
		m_StateData.m_WrapR					= wrapR;
		m_StateData.m_SampleCmp			= sampleCmp;
	}

	//! @brief テクスチャサンプラのアドレッシングモード取得：Ｓ
  inline TextureAddress GetSamplerAddressS() const
  {
    return m_StateData.m_WrapS;
  }

	//! @brief テクスチャサンプラのアドレッシングモード取得：Ｔ
  inline TextureAddress GetSamplerAddressT() const
  {
    return m_StateData.m_WrapT;
  }

	//! @brief テクスチャサンプラのボーダーカラー
	inline void SetSamplerBorderColor(
		const Color color
	)
	{
		m_StateData.m_BorderColor = color;
	}

	//! @brief テクスチャサンプラのコントローラ
	inline void SetSamplerControl(
		const u16 minLod = 0, //!< テクスチャの縮小フィルタのLODの最少値(0 - 11)
		const u16 maxLod = 12, //!< テクスチャの縮小フィルタのLODの最大値(1 - 12)
		const u8  maxAniso = 1 //!< 異方性フィルタのサンプルレベルの最大値(1 - 16)
	)
	{
		GFL_ASSERT( minLod <= 11 );
		GFL_ASSERT( maxLod >= 1 && maxLod <= 12 );
		GFL_ASSERT( maxAniso >= 1 && maxAniso <= 16 );
		
		m_StateData.m_MinLod = minLod;
		m_StateData.m_MaxLod = maxLod;
		m_StateData.m_MaxAniso = maxAniso;
	}

	//! @brief テクスチャサンプラのフィルタ
	inline void SetSamplerFilter(
		const TextureFilter magFilter = TextureFilter::Point, //!< 拡大フィルタの種類
		const TextureFilter minFilter = TextureFilter::Point, //!< 縮小フィルタの種類
		const TextureFilter mipFilter = TextureFilter::None,  //!< 縮小時に使うミップマップフィルタの種類
		const float mipmapLodBias = 0.0f                      //!< 縮小フィルタのLODバイアス値
	)
	{
		m_StateData.m_MinFilter = minFilter;
		m_StateData.m_MagFilter = magFilter;
		m_StateData.m_MipFilter = mipFilter;
		m_StateData.m_MipmapLodBias = mipmapLodBias;
	}

	virtual void UpdateState() = 0;

protected:
	SamplerStateObject() : GLMemory()
	{
		ResetRenderStates();
	}

	struct StateData{
		TextureAddress	m_WrapS;
		TextureAddress	m_WrapT;
		TextureAddress	m_WrapR;
		CompareFunc			m_SampleCmp;
		Color						m_BorderColor;
		u16							m_MinLod;
		u16							m_MaxLod;
		u8							m_MaxAniso;
		TextureFilter		m_MinFilter;
		TextureFilter		m_MagFilter;
		TextureFilter		m_MipFilter;
		f32							m_MipmapLodBias;
	};

	StateData					m_StateData;
};

}}

#endif
