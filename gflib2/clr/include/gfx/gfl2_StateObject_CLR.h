#ifndef GFLIB2_GRX_CLR_STATEOBJECT_CLR_H_INCLUDED
#define GFLIB2_GRX_CLR_STATEOBJECT_CLR_H_INCLUDED

#using <System.Drawing.dll>

#include <gfx/include/gfl2_GFGL.h>
#include <clr/include/gfx/gfl2_Color_CLR.h>
#include <clr/include/gfx/gfl2_types_CLR.h>

namespace gfl2 { namespace clr { namespace gfx {

//-----------------------------------------------------------
//! @brief ブレンドステートオブジェクト
//-----------------------------------------------------------
public ref class BlendStateObject{
public:

	BlendStateObject( gfl2::gfx::BlendStateObject *native ){ m_pNative = native; }
	~BlendStateObject(){this->!BlendStateObject();}
	!BlendStateObject();

	//! @brief ステートを初期状態にリセットする
	void ResetRenderStates(){ m_pNative->ResetRenderStates(); }

	//! @brief アルファテスト関数の指定
	void SetAlphaRef(
		const float       ref    //!< 入力アルファ値と比較する参照値( 0〜1 )
	)
	{
		m_pNative->SetAlphaRef(ref);
	}

	float GetAlphaRef(){ return m_pNative->GetAlphaRef(); }

	//! @brief アルファテストの有効、無効の設定
	void SetAlphaTestEnable(b32 enable){ m_pNative->SetAlphaTestEnable(enable); }

	//! @brief 現在のアルファテストの取得
	b32 GetAlphaTestEnable(){ return m_pNative->GetAlphaTestEnable(); }

	//! @brief ピクセルブレンドに使用する定数値の設定
	void SetBlendEnable(b32 enable){ m_pNative->SetBlendEnable(enable); }

	//! @brief 現在のピクセルブレンドに使用する定数値の取得
	b32 GetBlendEnable(){ return m_pNative->GetBlendEnable(); }

	//! @brief ピクセルブレンド機能の演算式の指定
	void SetBlendOp(
		BlendOp::Enum color, //!< ＲＧＢのブレンド演算式
		BlendOp::Enum alpha  //!< αのブレンド演算式
	);

	//! @copydoc setBlendOp
	void SetBlendOp( BlendOp::Enum op );

	//! @brief ブレンド係数値の計算式の指定
	void SetBlendFunc(
		BlendFunc::Enum sfc, //!< ソースＲＧＢ係数の計算式
		BlendFunc::Enum dfc, //!< デストＲＧＢ係数の計算式
		BlendFunc::Enum sfa, //!< ソースα係数の計算式
		BlendFunc::Enum dfa  //!< デストα係数の計算式
	);

	//! @copydoc setBlendFunc
	void SetBlendFunc( BlendFunc::Enum src, BlendFunc::Enum dest);

	//! @brief Alpha To Coverageの設定
	void SetAlphaToCoverageEnable(bool enable){ m_pNative->SetAlphaToCoverageEnable(enable); }

	void UpdateState(){ m_pNative->UpdateState(); }

	gfl2::gfx::BlendStateObject* GetNative(){ return m_pNative; }

private:

	gfl2::gfx::BlendStateObject			*m_pNative;
};

//-----------------------------------------------------------
//! @brief ラスタライザステートオブジェクト
//-----------------------------------------------------------
public ref class RasterizerStateObject{
public:

	RasterizerStateObject( gfl2::gfx::RasterizerStateObject *native ){ m_pNative = native; }
	~RasterizerStateObject(){this->!RasterizerStateObject();}
	!RasterizerStateObject();

	//! @brief ステートを初期状態にリセットする
	void ResetRenderStates(){ m_pNative->ResetRenderStates(); }

	//! @brief カリングモードを指定する
	//! @param func デフォルトは CullMode::CW
	void SetCullMode(CullMode::Enum mode);

	//! @brief 現在のカリングモードを取得する
	CullMode::Enum GetCullMode();

	//! @brief フィルモードの指定( ポリゴン系にしか影響しません )
	void SetFillMode(const FillMode::Enum mode);

	//! @brief 深度バイアス係数を設定する\n
	//! ※ポリゴン描画のみ影響します。ラインやポイント描画には影響しません。
	void SetDepthBias(const u32 bias){ m_pNative->SetDepthBias(bias); }

	//! @brief ＭＳＡＡの有効、無効の設定
	//! ※レンダーターゲットをMultiSampleモードで作成しておく必要があります。
	void SetMultisampleEnable(const bool enable){ m_pNative->SetMultisampleEnable(enable); }

	void UpdateState(){ m_pNative->UpdateState(); }

	gfl2::gfx::RasterizerStateObject* GetNative(){ return m_pNative; }

private:

	gfl2::gfx::RasterizerStateObject			*m_pNative;
};

//-----------------------------------------------------------
//! @brief 深度ステンシルステートオブジェクト
//-----------------------------------------------------------
public ref class DepthStencilStateObject{
public:

	DepthStencilStateObject( gfl2::gfx::DepthStencilStateObject *native ){ m_pNative = native; }
	~DepthStencilStateObject(){this->!DepthStencilStateObject();}
	!DepthStencilStateObject();

	//! @brief ステートを初期状態にリセットする
	void ResetRenderStates(){ m_pNative->ResetRenderStates(); }

	//! @brief 深度テスト関数の指定
	void SetDepthFunc(const CompareFunc::Enum func);

	//! @brief 深度テスト関数の取得
	CompareFunc::Enum GetDepthFunc();

	//! @brief 深度テストの有効、無効の設定
	void SetDepthWriteEnable(const b32 enable){ m_pNative->SetDepthWriteEnable(enable); }

	//! @brief 現在の深度書き込みフラグの取得
	b32 GetDepthWriteEnable(){ return m_pNative->GetDepthWriteEnable(); }

	//! @brief 深度テスト関数の指定
	void SetDepthTestEnable(bool enable);

	//! @brief 現在の深度テストの取得
	b32 GetDepthTestEnable(){ return m_pNative->GetDepthTestEnable(); }
		
	//! @brief ステンシルテストの有効、無効の設定
	void SetStencilTestEnable(const b32 enable){ m_pNative->SetStencilTestEnable(enable); }

	//! @brief ステンシルテストの有効、無効の設定
	b32 GetStencilTestEnable(){ return m_pNative->GetStencilTestEnable(); }
		
	//! @brief 両面ステンシルテスト機能の有効、無効の設定
	void SetTwoSideStencilTestEnable(const bool enable){ m_pNative->SetTwoSideStencilTestEnable(enable); }

	//! @brief ステンシルテスト関数の指定
	void SetStencilFunc(
		const PolygonFace::Enum face, //!< 裏表のどちらの面か指定する
		const CompareFunc::Enum func, //!< アルファ比較関数
		const u8 ref,        //!< 入力アルファ値と比較する参照値
		const u8 mask        //!< テスト実行時に、マスク参照値と格納されているステンシル値の両方とAND演算するマスク
	);

	//! @brief 裏面描画時のステンシルプレーンの書き込み制御
	void SetStencilWriteMask(
		const u8 bits        //!< 書き込み許可マスク
	);

	//! @brief 裏面描画時のステンシル処理の設定
	void SetStencilOp(
		const PolygonFace::Enum face,    //!< 裏表のどちらの面か指定する
		const StencilOp::Enum fail,      //!< ステンシルテストにパスしなかった場合の動作
		const StencilOp::Enum depthFail, //!< ステンシルテストはパスしたが、デプステストにパスしなかった場合の処理
		const StencilOp::Enum depthPass  //!< ステンシルテストとデプステストの両方にパスした場合、\n
			                          //!< またはステンシルテストにパスし、デプスバッファまたはデプステストが有効でない場合のステンシルの処理
	);

	void UpdateState(){ m_pNative->UpdateState(); }

	gfl2::gfx::DepthStencilStateObject* GetNative(){ return m_pNative; }

private:

	gfl2::gfx::DepthStencilStateObject			*m_pNative;
};

//-----------------------------------------------------------
//! @brief サンプラーステートオブジェクト
//-----------------------------------------------------------
public ref class SamplerStateObject{
public:

	SamplerStateObject( gfl2::gfx::SamplerStateObject *native ){ m_pNative = native; }
	~SamplerStateObject(){this->!SamplerStateObject();}
	!SamplerStateObject();

	//! @brief ステートを初期状態にリセットする
	void ResetRenderStates(){ m_pNative->ResetRenderStates(); }

	//! @brief テクスチャサンプラのアドレッシングモード
	void SetSamplerAddress(
		const TextureAddress::Enum  wrapS,
		const TextureAddress::Enum  wrapT,
		const TextureAddress::Enum  wrapR,
		const CompareFunc::Enum     sampleCmp
	);

	//! @brief テクスチャサンプラのボーダーカラー
	void SetSamplerBorderColor(
		Color^ color
	);

	//! @brief テクスチャサンプラのコントローラ
	void SetSamplerControl(
		const u16 minLod, //!< テクスチャの縮小フィルタのLODの最少値(0 - 11)
		const u16 maxLod, //!< テクスチャの縮小フィルタのLODの最大値(1 - 12)
		const u8  maxAniso //!< 異方性フィルタのサンプルレベルの最大値(1 - 16)
	);

	//! @brief テクスチャサンプラのフィルタ
	void SetSamplerFilter(
		const TextureFilter::Enum magFilter, //!< 拡大フィルタの種類
		const TextureFilter::Enum minFilter, //!< 縮小フィルタの種類
		const TextureFilter::Enum mipFilter,  //!< 縮小時に使うミップマップフィルタの種類
		const float mipmapLodBias                      //!< 縮小フィルタのLODバイアス値
	);

	void UpdateState(){ m_pNative->UpdateState(); }

	gfl2::gfx::SamplerStateObject* GetNative(){ return m_pNative; }

private:

	gfl2::gfx::SamplerStateObject			*m_pNative;
};


}}}

#endif