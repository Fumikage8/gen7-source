//=============================================================================
/**
 * @file    gfl2_CTRStateObject.h
 * @brief   各種描画ステートオブジェクト設定のＣＴＲ実装が記載されたファイルです
 * @author  takebe_makoto
 * @date    2014
 *
 *  2016.4.8    ishiguro  ステンシルコマンドを積むためキャッシュサイズを32に拡張しました。
 *
 */
//=============================================================================
#ifndef GFLIB2_GRX_CTR_CTRSTATEOBJECT_H_INCLUDED
#define GFLIB2_GRX_CTR_CTRSTATEOBJECT_H_INCLUDED

#include <gfx/include/gfl2_StateObject.h>
#include <gfx/include/CTR/gfl2_CTRTypes.h>
#include <gfx/include/CTR/gfl2_CTRSurface.h>
#include <gfx/include/CTR/gfl2_CTRTexture.h>
#include <gfx/include/CTR/gfl2_CTRCommandBufferManager.h>

namespace gfl2 { namespace gfx { namespace ctr {

class CTRGL;

class CTRBlendStateObject : public BlendStateObject
{
	friend class CTRGL;
public:
	virtual void UpdateState();
	virtual ~CTRBlendStateObject();
	void Bind(nn::gr::RenderState::AlphaTest* pGrAlphaTest, nn::gr::RenderState::Blend* pGrBlend, nn::gr::RenderState::ColorMask* pGrColorMask);

  //kawa20150527
  inline CTRCommandBufferManager<16>* GetCommandCache(void)
  {
    return m_CommandCache;
  }

public:
	//! @brief ステートを初期状態にリセットする
	inline virtual void ResetRenderStates()
	{
    BlendStateObject::ResetRenderStates();
    m_CommandCacheResetFlag = true;  // CTR版では「初期状態 = StateObjectを使っていないとき」とするので、キャッシュを未使用にする。  //kawa20150527
	}

	//! @brief レンダーターゲットマスクの設定
	inline virtual void SetRenderTargetWriteMask( u32 no, const u8 mask )
	{
    BlendStateObject::SetRenderTargetWriteMask( no, mask );
    m_CommandCacheResetFlag = false;  //kawa20150527
	}
		
	//! @brief アルファテスト参照値の設定
	inline virtual void SetAlphaRef(
		const float       ref    //!< 入力アルファ値と比較する参照値( 0〜1 )
	)
	{
    BlendStateObject::SetAlphaRef(
        ref
    );
    m_CommandCacheResetFlag = false;  //kawa20150527
	}

  //! @brief アルファテスト比較関数の設定
  inline virtual void SetAlphaCompareFunc(CompareFunc func)
  {
    BlendStateObject::SetAlphaCompareFunc(func);
    m_CommandCacheResetFlag = false;  //kawa20150527
  }

	//! @brief アルファテストの有効、無効の設定
	inline virtual void SetAlphaTestEnable(const b32 enable)
	{
    BlendStateObject::SetAlphaTestEnable(enable);
    m_CommandCacheResetFlag = false;  //kawa20150527
	}

	//! @brief ピクセルブレンドに使用する定数値の設定
	inline virtual void SetBlendEnable(const b32 enable)
	{
    BlendStateObject::SetBlendEnable(enable);
    m_CommandCacheResetFlag = false;  //kawa20150527
	}

	//! @brief ピクセルブレンド機能の演算式の指定
	inline virtual void SetBlendOp(
		const BlendOp color, //!< ＲＧＢのブレンド演算式
		const BlendOp alpha  //!< αのブレンド演算式
	)
	{
    BlendStateObject::SetBlendOp(
        color,
        alpha
    );
    m_CommandCacheResetFlag = false;  //kawa20150527
	}

	//! @copydoc setBlendOp
	inline virtual void SetBlendOp( BlendOp op )
	{
    BlendStateObject::SetBlendOp( op );
    m_CommandCacheResetFlag = false;  //kawa20150527
	}

	//! @brief ブレンド係数値の計算式の指定
	inline virtual void SetBlendFunc(
		const BlendFunc sfc, //!< ソースＲＧＢ係数の計算式
		const BlendFunc dfc, //!< デストＲＧＢ係数の計算式
		const BlendFunc sfa, //!< ソースα係数の計算式
		const BlendFunc dfa  //!< デストα係数の計算式
	)
	{
    BlendStateObject::SetBlendFunc(
        sfc,
        dfc,
        sfa,
        dfa
    );
    m_CommandCacheResetFlag = false;  //kawa20150527
	}

	//! @copydoc setBlendFunc
	inline virtual void SetBlendFunc(const BlendFunc src, const BlendFunc dest)
	{
    BlendStateObject::SetBlendFunc(src, dest);
    m_CommandCacheResetFlag = false;  //kawa20150527
	}

	//! @brief Alpha To Coverageの設定
	inline virtual void SetAlphaToCoverageEnable(const bool enable)
	{
    BlendStateObject::SetAlphaToCoverageEnable(enable);
    m_CommandCacheResetFlag = false;  //kawa20150527
	}

protected:
	
	CTRBlendStateObject();

	StateData			m_FixData;

  //kawa20150727
  CTRCommandBufferManager<16>* m_CommandCache;
  b32                      m_CommandCacheResetFlag;

};

class CTRRasterizerStateObject : public RasterizerStateObject
{
	friend class CTRGL;
public:
	virtual void UpdateState();
	virtual ~CTRRasterizerStateObject();
	void Bind(nn::gr::RenderState::WBuffer* pGrWBuffer, nn::gr::RenderState::Culling* pGrCulling);

protected:
	
	CTRRasterizerStateObject();

	StateData						m_FixData;

};

class CTRDepthStencilStateObject : public DepthStencilStateObject
{
	friend class CTRGL;
public:
	virtual void UpdateState();
	virtual ~CTRDepthStencilStateObject();

	void Bind(nn::gr::RenderState::DepthTest* pGrDepthTest, nn::gr::RenderState::StencilTest* pGrStencilTest);

  //kawa20150428
  //@attention 2016/04/08 ステンシルコマンドを積むため32に拡張しました。
  inline CTRCommandBufferManager<32>* GetCommandCache(void)
  {
    return m_CommandCache;
  }

public:
	//! @brief ステートを初期状態にリセットする
	inline virtual void ResetRenderStates()
  {
    DepthStencilStateObject::ResetRenderStates();
    m_CommandCacheResetFlag = true;  // CTR版では「初期状態 = StateObjectを使っていないとき」とするので、キャッシュを未使用にする。  //kawa20150428
  }

	//! @brief 深度テスト関数の指定
	inline virtual void SetDepthFunc(const CompareFunc func)
  {
    DepthStencilStateObject::SetDepthFunc(func);
    m_CommandCacheResetFlag = false;  //kawa20150428
  }

	//! @brief 深度テストの有効、無効の設定
	inline virtual void SetDepthWriteEnable(const b32 enable)
  {
    DepthStencilStateObject::SetDepthWriteEnable(enable);
    m_CommandCacheResetFlag = false;  //kawa20150428
  }

	//! @brief 深度テストの有効、無効の設定
	inline virtual void SetDepthTestEnable(const b32 enable)
  {
    DepthStencilStateObject::SetDepthTestEnable(enable);
    m_CommandCacheResetFlag = false;  //kawa20150428
  }

	//! @brief ステンシルテストの有効、無効の設定
	inline virtual void SetStencilTestEnable(const b32 enable)
  {
    DepthStencilStateObject::SetStencilTestEnable(enable);
    m_CommandCacheResetFlag = false;  //kawa20150428
  }

	//! @brief 両面ステンシルテスト機能の有効、無効の設定
	inline virtual void SetTwoSideStencilTestEnable(const bool enable)
  {
    DepthStencilStateObject::SetTwoSideStencilTestEnable(enable);
    m_CommandCacheResetFlag = false;  //kawa20150428
  }

	//! @brief ステンシルテスト関数の指定
	inline virtual void SetStencilFunc(
		const PolygonFace face, //!< 裏表のどちらの面か指定する
		const CompareFunc func, //!< アルファ比較関数
		const u8 ref,        //!< 入力アルファ値と比較する参照値
		const u8 mask        //!< テスト実行時に、マスク参照値と格納されているステンシル値の両方とAND演算するマスク
	)
  {
    DepthStencilStateObject::SetStencilFunc(
        face,
        func,
        ref,
        mask
    );
    m_CommandCacheResetFlag = false;  //kawa20150428
  }

	//! @brief 裏面描画時のステンシルプレーンの書き込み制御
	inline virtual void SetStencilWriteMask(
		const u8 bits        //!< 書き込み許可マスク
	)
  {
    DepthStencilStateObject::SetStencilWriteMask(
        bits
    );
    m_CommandCacheResetFlag = false;  //kawa20150428
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
    DepthStencilStateObject::SetStencilOp(
        face,
        fail,
        depthFail,
        depthPass
    );
    m_CommandCacheResetFlag = false;  //kawa20150428
  }

protected:
	
	CTRDepthStencilStateObject();

	StateData						m_FixData;

  //kawa20150428
  CTRCommandBufferManager<32>* m_CommandCache;//@attention 2016/04/08 ステンシルコマンドを積むため32に拡張しました。
  b32                      m_CommandCacheResetFlag;

};

class CTRSamplerStateObject : public SamplerStateObject
{
	friend class CTRGL;
public:
	virtual void UpdateState();
	virtual ~CTRSamplerStateObject();

	void Bind(nn::gr::Texture* pGrTexture, u32 texNo, const CTRTexture *pTexture ) const;

protected:
	
	CTRSamplerStateObject();

	StateData					m_FixData;


};


}}}

#endif
