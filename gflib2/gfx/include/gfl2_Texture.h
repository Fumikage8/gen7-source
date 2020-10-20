#ifndef GFLIB2_GRX_TEXTURE_H_INCLUDED
#define GFLIB2_GRX_TEXTURE_H_INCLUDED

#include <stdio.h>
#include <types/include/gfl2_Typedef.h>
#include <gfx/include/gfl2_types.h>
#include <gfx/include/gfl2_GLMemory.h>
#include <gfx/include/gfl2_Surface.h>

namespace gfl2 { namespace gfx {

template <class Implement_>
class GFGLBase;

class Texture : public GLMemory
{
public:

	static const u32							SurfaceMax = 8;//!< ８枚までサポート

	template <class Implement_>
	friend class GFGLBase;

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief テクスチャの詳細情報
	struct TextureDesc
	{
		TextureType type;		   //!< テクスチャの種類
		u32					usage;		  //!< 使用用途フラグ
		Pool				pool;		   //!< メモリ配置
		u32					width;		  //!< 幅
		u32					height;		 //!< 高さ
		u32					depth;		  //!< 深度
		s32					mipLevels;	  //!< ミップマップ数
		Format			format;		 //!< ピクセルフォーマット
		MultiSample multiSample;	//!< マルチサンプリング
	};

	virtual ~Texture();

	//! @brief テクスチャを更新する
	virtual void Update( 
		const void* pData,								//! 画像ポインタ
		u32 depth = 0								//! MipLevel or VolumeLevel or CubeFaceNoとして使用
		)
	{
    (void)pData;
    (void)depth;
	}

	void SetTextureDesc( const TextureDesc& rDesc ){ m_TextureDesc = rDesc; }
	const TextureDesc& GetTextureDesc() const { return m_TextureDesc; }
	const Surface* GetSurface( u32 surfaceLv = 0 ) const { return m_pSurface[surfaceLv]; }

  // 必ずLock(), UnLock()は対にして呼んでください
	virtual void* Lock() = 0;
	virtual void UnLock() = 0;

protected:
	Texture( const TextureDesc &rDesc );
	TextureDesc					m_TextureDesc;
	Surface							*m_pSurface[SurfaceMax];
};

}}

#endif
