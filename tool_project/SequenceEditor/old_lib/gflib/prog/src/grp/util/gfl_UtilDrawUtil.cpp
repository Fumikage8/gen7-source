//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_UtilDrawUtil.cpp
 *	@brief  簡易逐次描画クラス
 *	@author	Masateru Ishiguro
 *	@date		2011.2.21
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include <stdarg.h>

#include <nn/gx/CTR/gx_MacroReg.h>
#include <nn/gx/CTR/gx_MacroCommon.h>
#include <nn/gx/CTR/gx_MacroTexture.h>

#include <nn/gr/CTR/gr_Utility.h>



// NW4C
#include <nw/gfx.h>
#include <nw/font/font_WideTextWriter.h>
#include <nw/font/font_ResFont.h>
#include <nw/font/font_RectDrawer.h>

// gflib
#include <gfl_Heap.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dSystem.h>
#include <grp/util/gfl_UtilDrawUtil.h>
#include <debug/gfl_Assert.h>
#include <grp/gfl_GraphicsType.h>
#include <grp/g3d/gfl_G3dH3dCommandUtil.h>


#include "gfl_UtilShader.h"
#include "gfl_UtilPixelShader.h"

namespace gfl { namespace grp { namespace util {

DrawUtil			*DrawUtil::s_Gp = NULL;

# pragma pack (1)
struct TgaFormat
{
  u8	No_ID;			/* 0=イメージＩＤ無し*/
  u8	CMap_Type;		/* 0=カラーマップ無し */
  u8	Image_Type;		/* 2=無圧縮RGBタイプ 10=圧縮RGBタイプ */
  u8	CMap_Spec[5];	/* スキップされる*/

  u16	Ox;				/* 画面 左上 X 0固定 */
  u16	Oy;				/* 画面 左上 Y 0固定 */

  u16	Width;			/* 幅 (dot)*/
  u16	Hight;			/* 高さ (dot)*/

  u8	Pixel;			/* Pixel Size. 32=32bit */
  u8	IDesc_Type;		/* 8=TGA32 アトリビュート */
};
# pragma pack ()

//24 ビット浮動小数点数への変換マクロ
#define UTL_F2F_16M7E(_inarg, _outarg) \
{ \
  u32 uval_, m_; \
  s32 e_; \
  f32 f_; \
  static const s32 bias_ = 128 - (1 << (7 - 1)); \
  f_ = (_inarg); \
  uval_ = *(u32*)&f_; \
  e_ = (uval_ & 0x7fffffff) ? (((uval_ >> 23) & 0xff) - bias_) : 0; \
  m_ = (uval_ & 0x7fffff) >> (23 - 16); \
  if (e_ >= 0) \
    _outarg = m_ | (e_ << 16) | ((uval_ >> 31) << (16 + 7)); \
  else \
    _outarg = ((uval_ >> 31) << (16 + 7)); \
}


/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
■
■	DrawUtil::Impl構造体(メンバが増えていくたびに余計なヘッダーをインクルードさせたくないのでここで定義)
■
■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
struct DrawUtil::Impl
{
	enum VertexShaderType
	{
		STANDERD_VS = 0,
		TWOD_VS,
		VERTEX_SHADER_MAX
	};

	enum ShaderType
	{
		VERTEX_COLOR_SHADER = 0,			///!
		TEXTURE_MAP_SHADER,         	///!
    RGBTEXTURE_MAP_SHADER,         	///!
    TEXTURE_LIGHTING_SHADER,			///!
    FRONT_DOFTEXTURE_MAP_SHADER,         	///!
    BACK_DOFTEXTURE_MAP_SHADER,         	///!
    DEBUG_SHADER,         	///!
		//FILL_SHADER,                ///! フィルスクリーンシェーダー
		SHADER_MAX,

    ENUM_FORCE_DWORD(FragmentOperationEnum)
	};

  Impl() :
    m_pHeapAllocator(NULL),
    m_pDeviceAllocator(NULL),
    m_pTargetFrameBuffer(NULL),
    m_pDefaultFont(NULL),
    m_pRectDrawerCmdBuf(NULL),
    m_pDrawStringBuf(NULL),
    m_TextColor(1.0f, 1.0f, 1.0f, 1.0f),
    m_TextScale(1.0f, 1.0f),
    m_ScissorEnable(false),
    m_UseTextureFlag(false)
  { 
  }

  s32                                       m_StackSize;
  heap::NwAllocator                         **m_pHeapAllocator;
  heap::NwAllocator                         **m_pDeviceAllocator;
  gfl::grp::RenderTarget                    *m_pTargetFrameBuffer;

	GLuint																		m_ShaderId[VERTEX_SHADER_MAX];
  gfl::grp::util::VertexColorPixelShader		m_VertexColorPixelShader;
	gfl::grp::util::TextureMapPixelShader			m_TextureMapPixelShader;
  gfl::grp::util::RgbTextureMapPixelShader	m_RgbTextureMapPixelShader;
  gfl::grp::util::TextureMapPixelShader 		m_TexDirLightPixelShader;
  gfl::grp::util::FrontDofTextureMapPixelShader 	m_FrontDofTextureMapPixelShader;
  gfl::grp::util::BackDofTextureMapPixelShader    m_BackDofTextureMapPixelShader;
  gfl::grp::util::DebugPixelShader         	m_DebugPixelShader;

	gfl::grp::util::ShaderObject							m_ShaderObject[SHADER_MAX];
	gfl::grp::util::ShaderParameterHandle			m_hVsProjection[SHADER_MAX];
	gfl::grp::util::ShaderParameterHandle			m_hVsPivotRot[SHADER_MAX];
	gfl::grp::util::ShaderParameterHandle			m_hVsModelView[SHADER_MAX];
	gfl::grp::util::ShaderParameterHandle			m_hVsModelMatrix[SHADER_MAX];
	gfl::grp::util::ShaderParameterHandle			m_hVsFogLength[SHADER_MAX];
	gfl::grp::util::ShaderParameterHandle			m_hVsFogColor[SHADER_MAX];
	gfl::grp::util::ShaderParameterHandle			m_hVsCubeMatrix[SHADER_MAX];
	gfl::grp::util::ShaderParameterHandle			m_hVsRimDirPow[SHADER_MAX];
	gfl::grp::util::ShaderParameterHandle			m_hVsLightPos[SHADER_MAX];
	gfl::grp::util::ShaderParameterHandle			m_hVsBRDFColor[SHADER_MAX];
	gfl::grp::util::ShaderParameterHandle			m_hVsAnisoRot[SHADER_MAX];
	gfl::grp::util::ShaderParameterHandle			m_hVsAnisoPow[SHADER_MAX];
	gfl::grp::util::ShaderParameterHandle			m_hVsUvScale[SHADER_MAX];
	gfl::grp::util::ShaderParameterHandle			m_hVsUvOffSet[SHADER_MAX];
	gfl::grp::util::ShaderParameterHandle			m_hPsAmbientColor[SHADER_MAX];
	gfl::grp::util::ShaderParameterHandle			m_hPsDiffuseColor[SHADER_MAX];
	gfl::grp::util::ShaderParameterHandle			m_hPsLightPos[SHADER_MAX];
	gfl::grp::util::ShaderParameterHandle			m_hPsLight1Pos[SHADER_MAX];

	gfl::math::MTX44													m_ProjMat;
	gfl::math::MTX44													m_ViewMat;
	gfl::math::MTX44													m_ModelMat;
  nn::math::MTX44                           m_TextProj;
  nn::math::MTX34                           m_TextMv;

  nw::font::Font                            *m_pDefaultFont;
  nw::font::ResFont                         m_ResFont;
  nw::font::RectDrawer                      m_RectDrawer;
  void                                      *m_pRectDrawerCmdBuf;
  nw::font::DispStringBuffer                *m_pDrawStringBuf;
  gfl::math::VEC4                           m_TextColor;
  gfl::math::VEC2                           m_TextScale;

  gfl::grp::ScissorRect                     m_ScissorRect;
  bool                                      m_ScissorEnable;

  GLuint                                    m_StartAddr;
  GLuint                                    m_EndAddr;
  s32																				m_ShaderType;
  
  bool                                      m_UseTextureFlag;  // テクスチャを使うか否かフラグ。  sango_2014/06/19_kawada_koji
};

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
■
■　　	DrawUtil::CacheCommandクラス
■
■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
DrawUtil::CacheCommand::CacheCommand()
{
  m_pCacheDcsBuff = NULL;
  m_pShaderCacheDcs = NULL;
  m_IsInit = false;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
DrawUtil::CacheCommand::~CacheCommand()
{
  if ( m_pCacheDcsBuff )
  {
    if(gfl::debug::AUTHOR_ISHIGURO ==DEFINED_AUTHOR_ID)
    {//１フレ遅れ解放になっていなけでばＧＰＵハングする。
      u8*     pBufferTop = reinterpret_cast<u8*>( m_pCacheDcsBuff);
      //memset( pBufferTop, 0xFF, 8 );//書き込み量を抑えるため先頭１コマンドのみ上書き
      //memset( &pBufferTop[this->WriteSize()-8], 0xFF, 8 );//書き込み量を抑えるため末尾１コマンドのみ上書き
    }
  }

  GFL_SAFE_DELETE( m_pCacheDcsBuff );
  GFL_SAFE_DELETE( m_pShaderCacheDcs );
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void DrawUtil::CacheCommand::Init( heap::NwAllocator* deviceAllocator, s32 allocSize )
{
  m_IsInit = true;
  m_BufferSize = allocSize;
  if ( m_BufferSize % 16 )
  {
    m_BufferSize += (16 - (m_BufferSize % 16));
  }
  
  m_pCacheDcsBuff = reinterpret_cast<bit32*>( deviceAllocator->Alloc( m_BufferSize, 16 ) );
  m_pShaderCacheDcs = GFL_NEW( deviceAllocator->GetHeapBase() ) nn::gr::CTR::CommandBufferJumpHelper( m_pCacheDcsBuff );
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void DrawUtil::CacheCommand::SetCH1KickCommand()
{
  bit32     *p_command = m_pShaderCacheDcs->GetCurrentPtr();
  u32       cmdSize = m_pShaderCacheDcs->GetSize() + 8;
  
  m_BufferSizeWithoutKickCommand = this->WriteSize();
  
  p_command = nw::h3d::fnd::Util::AddDummyDataForCommandBuffer(p_command, cmdSize);
  p_command = nn::gr::CTR::MakeChannelKickCommand(p_command, nn::gr::CMD_BUF_CH1);
  
  m_pShaderCacheDcs->Put( p_command );
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void DrawUtil::CacheCommand::Reset()
{
  m_pShaderCacheDcs->FinalizeJump( m_pShaderCacheDcs->GetStartPtr() );//seek headみたいなもの
}
    
/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
■
■	DrawUtilクラス
■
■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
//-----------------------------------------------------------------------------
/// @brief	コンストラクタ
//-----------------------------------------------------------------------------
DrawUtil::DrawUtil( heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator, s32 stackMax )
{
	m_pImpl = GFL_NEW( heapAllocator->GetHeapBase() ) Impl();
  
  m_pImpl->m_StackSize = stackMax;
  m_pImpl->m_pHeapAllocator = GFL_NEW_ARRAY( heapAllocator->GetHeapBase() ) heap::NwAllocator*[m_pImpl->m_StackSize];
  m_pImpl->m_pDeviceAllocator = GFL_NEW_ARRAY( heapAllocator->GetHeapBase() ) heap::NwAllocator*[m_pImpl->m_StackSize];

  for( s32 i = 0; i < m_pImpl->m_StackSize; ++i )
  {
    m_pImpl->m_pHeapAllocator[i] = NULL;
    m_pImpl->m_pDeviceAllocator[i] = NULL;
  }
  
  m_pImpl->m_pHeapAllocator[0] = heapAllocator;
  m_pImpl->m_pDeviceAllocator[0] = deviceAllocator;

  initDefaultRenderer();
  initFontRenderer();
}

//-----------------------------------------------------------------------------
/// @brief	デストラクタ
//-----------------------------------------------------------------------------
DrawUtil::~DrawUtil()
{
  if ( m_pImpl->m_StackSize >= 2 )
  {
    GFL_ASSERT_STOP_MSG( m_pImpl->m_pHeapAllocator[1] == NULL, "アロケーターがプッシュされたままです。\n" );
    GFL_ASSERT_STOP_MSG( m_pImpl->m_pDeviceAllocator[1] == NULL, "アロケーターがプッシュされたままです。\n" );
  }
  
  // 描画用バッファの無効化
  // 描画用バッファがセットされているなら 構築時に SetDrawBuffer に渡したバッファへの
  // ポインタが返ってきます。
  void *const drawBuffer = m_pImpl->m_ResFont.SetDrawBuffer(NULL);
  if (drawBuffer != NULL)
  {
    m_pImpl->m_pDeviceAllocator[0]->Free(drawBuffer);
    m_pImpl->m_ResFont.RemoveResource();
  }

  if ( m_pImpl->m_pDrawStringBuf )
  {
    m_pImpl->m_pDeviceAllocator[0]->Free(m_pImpl->m_pDrawStringBuf);
  }

  if ( m_pImpl->m_pRectDrawerCmdBuf )
  {
    m_pImpl->m_pDeviceAllocator[0]->Free(m_pImpl->m_pRectDrawerCmdBuf);
  }

  GFL_SAFE_DELETE_ARRAY(m_pImpl->m_pHeapAllocator);
  GFL_SAFE_DELETE_ARRAY(m_pImpl->m_pDeviceAllocator);
  
	GFL_DELETE( m_pImpl );
  m_pImpl = NULL;
}

//-----------------------------------------------------------------------------
/// @brief	初期化
//-----------------------------------------------------------------------------
void DrawUtil::Initialize( heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator )
{
	s_Gp = GFL_NEW(heapAllocator->GetHeapBase()) DrawUtil( heapAllocator, deviceAllocator );
}

//-----------------------------------------------------------------------------
/// @brief	初期化
//-----------------------------------------------------------------------------
void DrawUtil::Initialize( gfl::grp::GraphicsSystem *pSystem, heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator, s32 stackMax )
{
  gfl::grp::CommandListManager* pManager = NULL;
  pManager = pSystem->GetCommandListManager();
  pSystem->BindCurrentCommandList();

  s_Gp = GFL_NEW(heapAllocator->GetHeapBase()) DrawUtil( heapAllocator, deviceAllocator, stackMax );

  //マテリアル初期化
  MaterialInfo    info;
  SetMaterial( info );
}

//-----------------------------------------------------------------------------
/// @brief	破棄
//-----------------------------------------------------------------------------
void DrawUtil::Terminate( void )
{
	if ( s_Gp )
	{
		GFL_DELETE( s_Gp );
		s_Gp = NULL;
	}
}

//-------------------------------------------------------------------------
/**
  * @brief アロケーターの置き換え
  * @param heapAllocator
  * @param deviceAllocator
  */
//-------------------------------------------------------------------------
void DrawUtil::PushAllocator( heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator )
{
  if( s_Gp )
  {
    if( s_Gp->m_pImpl->m_StackSize >= 2 )
    {
      if( s_Gp->m_pImpl->m_pHeapAllocator[s_Gp->m_pImpl->m_StackSize-1] == NULL )
      {
        for( s32 i = s_Gp->m_pImpl->m_StackSize-1; i >= 1; --i )
        {
          s_Gp->m_pImpl->m_pHeapAllocator[i] = s_Gp->m_pImpl->m_pHeapAllocator[i-1];
          s_Gp->m_pImpl->m_pDeviceAllocator[i] = s_Gp->m_pImpl->m_pDeviceAllocator[i-1];
        }

        s_Gp->m_pImpl->m_pHeapAllocator[0] = heapAllocator;
        s_Gp->m_pImpl->m_pDeviceAllocator[0] = deviceAllocator;
      }
      else
      {
        GFL_ASSERT_MSG( 0, "これ以上プッシュできません。\n" );
      }
    }
    else
    {
      GFL_ASSERT_MSG( 0, "スタックが確保されていません。\n" );
    }
  }
  else
  {
    GFL_ASSERT_MSG( 0, "初期化されていません。\n" );
  }
}

//-------------------------------------------------------------------------
/**
  * @brief アロケーターを戻す。
  */
//-------------------------------------------------------------------------
void DrawUtil::PopAllocator()
{
  if( s_Gp )
  {
    if( s_Gp->m_pImpl->m_pHeapAllocator[1] )
    {
      for( s32 i = 0; i < (s_Gp->m_pImpl->m_StackSize-1); ++i )
      {
        s_Gp->m_pImpl->m_pHeapAllocator[i] = s_Gp->m_pImpl->m_pHeapAllocator[i+1];
        s_Gp->m_pImpl->m_pDeviceAllocator[i] = s_Gp->m_pImpl->m_pDeviceAllocator[i+1];
      }

      s_Gp->m_pImpl->m_pHeapAllocator[s_Gp->m_pImpl->m_StackSize-1] = NULL;
      s_Gp->m_pImpl->m_pDeviceAllocator[s_Gp->m_pImpl->m_StackSize-1] = NULL;
    }
    else
    {
      GFL_ASSERT_MSG( 0, "これ以上ポップできません。\n" );
    }
  }
  else
  {
    GFL_ASSERT_MSG( 0, "初期化されていません。\n" );
  }
}

//-------------------------------------------------------------------------
/**
  * @brief 積まれているアロケーター数の確認
  * @return アロケータ数
  */
//-------------------------------------------------------------------------
s32 DrawUtil::GetStackAllocatorCount()
{
  s32     cnt = 0;
 
  if( s_Gp )
  {
    for( s32 i = 0; i < s_Gp->m_pImpl->m_StackSize; ++i )
    {
      if ( s_Gp->m_pImpl->m_pHeapAllocator[i] == NULL )
        break;

      ++cnt;
    }
  }
  else
  {
    GFL_ASSERT_MSG( 0, "初期化されていません。\n" );
  }
  
  return cnt;
}
  
//-----------------------------------------------------------------------------
/// @brief 文字列の幅を取得
//-----------------------------------------------------------------------------
f32 DrawUtil::CalcStringWidth( const wchar_t* pUtf16, nw::font::Font *pFont )
{
  nw::font::WideTextWriter        writer;
  gfl::grp::RenderTarget          *pTargetFrameBuffer = s_Gp->m_pImpl->m_pTargetFrameBuffer;
  nw::font::DispStringBuffer      *pDrawStringBuf = s_Gp->m_pImpl->m_pDrawStringBuf;
  
  if ( pFont == NULL )
    pFont = s_Gp->m_pImpl->m_pDefaultFont;
  
  if ( pFont == NULL )    return 0.0f;

  writer.SetDispStringBuffer( pDrawStringBuf );
  writer.SetFont( pFont );
  writer.SetScale( s_Gp->m_pImpl->m_TextScale.x, s_Gp->m_pImpl->m_TextScale.y );

  return  writer.CalcStringWidth( pUtf16 );
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtil::SetFontResource( void* pBinary )
{
  if ( pBinary )
  {
    nw::font::ResFont               &rResFont = s_Gp->m_pImpl->m_ResFont;

    void *drawBuffer = rResFont.SetDrawBuffer(NULL);
    if (drawBuffer != NULL)
    {//すでにリソースを持っていたら破棄
      s_Gp->m_pImpl->m_pDeviceAllocator[0]->Free(drawBuffer);
      rResFont.RemoveResource();
    }

    // フォントリソースをセットします
    bool bSuccess = rResFont.SetResource(pBinary);
    NN_ASSERT(bSuccess);

    // 描画用バッファを設定します。
    const u32 drawBufferSize = nw::font::ResFont::GetDrawBufferSize( pBinary );
    drawBuffer = s_Gp->m_pImpl->m_pDeviceAllocator[0]->Alloc( drawBufferSize, 128);
    rResFont.SetDrawBuffer(drawBuffer);

    SetFont( &(s_Gp->m_pImpl->m_ResFont) );
  }
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtil::SetFont( nw::font::Font *pFont )
{
  if ( pFont )
  {
    s_Gp->m_pImpl->m_pDefaultFont = pFont;
  }
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtil::BeginRender( gfl::grp::RenderTarget* pFrameBuffer )
{
//  nngxSplitDrawCmdlist();

  pFrameBuffer->ActivateBuffer();

  // 射影行列を正射影に設定
  {
    f32 left, top, right, bottom;
    pFrameBuffer->GetViewport( &left, &top, &right, &bottom );

    f32     width = static_cast<f32>(pFrameBuffer->GetHeight());
    f32     height = static_cast<f32>(pFrameBuffer->GetWidth());
    
    if ( pFrameBuffer->GetDisplayType() == gfl::grp::DISPLAY_LOWER )
    {
      width = bottom - top;
      height = right - left;
    }
  
    // 左上原点とし、Y軸とZ軸の向きが逆になるように設定します。
    nn::math::MTX44         proj;
    nw::font::RectDrawer    &rRectDrawer = s_Gp->m_pImpl->m_RectDrawer;
    f32 znear   =  0.0f;
    f32 zfar    = -1.0f;
    f32 t       =  0;
    f32 b       =  width;
    f32 l       =  0;
    f32 r       =  height;
    nn::math::MTX44OrthoPivot(&s_Gp->m_pImpl->m_TextProj, l, r, b, t, znear, zfar, nn::math::PIVOT_UPSIDE_TO_TOP);


    // モデルビュー行列を単位行列に設定
    nn::math::MTX34 mv;
    nn::math::MTX34Identity(&s_Gp->m_pImpl->m_TextMv);
  }

  {
    // ジオメトリシェーダを使用するかどうか
    const bool useGeometryShader = false;

    // 予約ジオメトリシェーダのサブディビジョンを使用するかどうか
    const bool useGeometryShaderSubdivision = false;

    static const u32 commands[] =
    {
      PICA_CMD_SET_DRAW_MODE0_DUMMY_BEGIN(),

      // 0x229 [1:0] ジオメトリシェーダを使用しない
      PICA_CMD_DATA_DRAW_MODE0(
      useGeometryShader,
      0                   /* drawMode */,
      false),             /* useGeometryShaderSubdivision */
      PICA_CMD_HEADER_SINGLE_BE( PICA_REG_DRAW_MODE0, 0x1 ),

      // 0x200
      PICA_CMD_SET_DRAW_MODE0_DUMMY_END(),

      // 0x244 [0:0] 共用プロセッサへのコピー
      PICA_CMD_SET_VS_COM_MODE( useGeometryShader ),
    };

    nngxAdd3DCommand(commands, sizeof(commands), true);
  }

  s_Gp->m_pImpl->m_pTargetFrameBuffer = pFrameBuffer;
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtil::EndRender()
{

}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtil::SetTextScale( f32 scale_x, f32 scale_y )
{
  s_Gp->m_pImpl->m_TextScale.Set( scale_x, scale_y );
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtil::SetTextColor( const gfl::math::VEC4 &color )
{
  s_Gp->m_pImpl->m_TextColor = color;
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtil::SetScissor( s32 x, s32 y, s32 w, s32 h )
{
  // 実際のディスプレイ座標に合わせて、x-y, w-h を入れ替える
  s_Gp->m_pImpl->m_ScissorRect.x      = y;
  s_Gp->m_pImpl->m_ScissorRect.y      = x;
  s_Gp->m_pImpl->m_ScissorRect.width  = h;
  s_Gp->m_pImpl->m_ScissorRect.height = w;
  s_Gp->m_pImpl->m_ScissorEnable = true;

  s_Gp->setRenderScissorState();
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtil::ClearScissor( void )
{
  s_Gp->m_pImpl->m_ScissorEnable = false;

  s_Gp->setRenderScissorState();
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtil::DrawText( s32 posX, s32 posY, const wchar_t* pUtf16, ... )
{
	wchar_t tmp[CharMax];

  va_list VA;
	va_start( VA, pUtf16 );
	vswprintf( tmp, CharMax, pUtf16, VA );
	va_end( VA );

  DrawText( posX, posY, tmp, s_Gp->m_pImpl->m_TextColor );
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtil::DrawText( s32 posX, s32 posY, const wchar_t* pUtf16, const gfl::math::VEC4 &color, nw::font::Font *pFont )
{
  nw::font::WideTextWriter        writer;
  gfl::grp::RenderTarget          *pTargetFrameBuffer = s_Gp->m_pImpl->m_pTargetFrameBuffer;
  nw::font::RectDrawer            &rRectDrawer = s_Gp->m_pImpl->m_RectDrawer;
  nw::font::DispStringBuffer      *pDrawStringBuf = s_Gp->m_pImpl->m_pDrawStringBuf;
  
  if ( pFont == NULL )
    pFont = s_Gp->m_pImpl->m_pDefaultFont;
  
  if ( pFont == NULL )
    return;

  s_Gp->m_pImpl->m_pTargetFrameBuffer->ActivateBuffer();
  s_Gp->resetRenderState( RenderStateType::FONT_2D );

  rRectDrawer.DrawBegin();
  rRectDrawer.SetProjectionMtx(s_Gp->m_pImpl->m_TextProj);
  rRectDrawer.SetViewMtxForText(s_Gp->m_pImpl->m_TextMv);

  writer.SetDispStringBuffer( pDrawStringBuf );
  writer.SetFont( pFont );
  writer.SetCursor(static_cast<f32>(posX), static_cast<f32>(posY));
  writer.SetScale( s_Gp->m_pImpl->m_TextScale.x, s_Gp->m_pImpl->m_TextScale.y );

  writer.StartPrint();
  writer.Print( pUtf16 );
  writer.EndPrint();

  rRectDrawer.BuildTextCommand(&writer);

  // 文字の色は、文字列の描画コマンドを再作成しなくても変更できます。
  writer.SetTextColor(nw::ut::Color8(static_cast<u8>(color.x * 255.0f), static_cast<u8>(color.y * 255.0f), static_cast<u8>(color.z * 255.0f), static_cast<u8>(color.w * 255.0f)));

  writer.UseCommandBuffer();

  rRectDrawer.DrawEnd();

  s_Gp->resetRenderState( RenderStateType::MODEL_3D );
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtil::DrawText( s32 posX, s32 posY, const wchar_t* pUtf16, const gfl::math::VEC4 &startColor, const gfl::math::VEC4 &endColor, nw::font::CharWriter::GradationMode gradMode, nw::font::Font *pFont )
{
  nw::font::WideTextWriter        writer;
  gfl::grp::RenderTarget          *pTargetFrameBuffer = s_Gp->m_pImpl->m_pTargetFrameBuffer;
  nw::font::RectDrawer            &rRectDrawer = s_Gp->m_pImpl->m_RectDrawer;
  nw::font::DispStringBuffer      *pDrawStringBuf = s_Gp->m_pImpl->m_pDrawStringBuf;
  
  if ( pFont == NULL )
    pFont = s_Gp->m_pImpl->m_pDefaultFont;
  
  if ( pFont == NULL )
    return;

  s_Gp->m_pImpl->m_pTargetFrameBuffer->ActivateBuffer();
  s_Gp->resetRenderState( RenderStateType::FONT_2D );

  rRectDrawer.DrawBegin();
  rRectDrawer.SetProjectionMtx(s_Gp->m_pImpl->m_TextProj);
  rRectDrawer.SetViewMtxForText(s_Gp->m_pImpl->m_TextMv);

  writer.SetDispStringBuffer( pDrawStringBuf );
  writer.SetFont( pFont );
  writer.SetCursor(static_cast<f32>(posX), static_cast<f32>(posY));
  writer.SetScale( s_Gp->m_pImpl->m_TextScale.x, s_Gp->m_pImpl->m_TextScale.y );

  writer.StartPrint();
  writer.Print( pUtf16 );
  writer.EndPrint();

  rRectDrawer.BuildTextCommand(&writer);

  // 文字の色は、文字列の描画コマンドを再作成しなくても変更できます。
  nw::ut::Color8 col1(static_cast<u8>(startColor.x * 255.0f), static_cast<u8>(startColor.y * 255.0f), static_cast<u8>(startColor.z * 255.0f), static_cast<u8>(startColor.w * 255.0f));
  nw::ut::Color8 col2(static_cast<u8>(endColor.x * 255.0f),   static_cast<u8>(endColor.y * 255.0f),   static_cast<u8>(endColor.z * 255.0f),   static_cast<u8>(endColor.w * 255.0f));
  writer.SetGradationMode( gradMode );

  writer.SetTextColor( col1, col2 );

  writer.UseCommandBuffer();

  rRectDrawer.DrawEnd();

  s_Gp->resetRenderState( RenderStateType::MODEL_3D );
}



//-----------------------------------------------------------------------------
/// @brief	ファイルからのテクスチャ生成関数。とりあえず32Bitフォーマットのtgaのみ対応
//-----------------------------------------------------------------------------
Texture* DrawUtil::CreateTextureFromMemory( gfl::grp::GraphicsSystem *pSystem, const void* p_data )
{
  gfl::grp::CommandListManager* pManager = NULL;
  pManager = pSystem->GetCommandListManager();
  
  pSystem->BindCurrentCommandList();

	TgaFormat		*pImage = (TgaFormat*)p_data;
	Texture			*pInstance = GFL_NEW(s_Gp->m_pImpl->m_pHeapAllocator[0]->GetHeapBase()) Texture();

	pInstance->m_Format = pImage->Pixel;
	pInstance->m_Width = pImage->Width;
	pInstance->m_Height = pImage->Hight;

	switch( pImage->Pixel ){
	case 24:
		pInstance->m_Format = GL_RGB;
		break;
	default:
		pInstance->m_Format = GL_RGBA;
		break;
	}

	//PICAネイティブフォーマットに変換
	const u8*		pImageTop = reinterpret_cast<const u8*>( p_data );
  pImageTop = &pImageTop[ sizeof(TgaFormat) ];

  u8          *pNativeImageData = reinterpret_cast<u8*>( s_Gp->m_pImpl->m_pHeapAllocator[0]->Alloc( pInstance->m_Width * pInstance->m_Height * (pImage->Pixel / 8) ) );
	DrawUtil::convertPicaFormat( pNativeImageData, pImageTop, pInstance->m_Format, pInstance->m_Width, pInstance->m_Height );

	glGenTextures( 1, &pInstance->m_hTexture );
	glBindTexture( GL_TEXTURE_2D, pInstance->m_hTexture );
	glTexImage2D( GL_TEXTURE_2D, 0, pInstance->m_Format, pInstance->m_Width, pInstance->m_Height, 0, pInstance->m_Format, GL_UNSIGNED_BYTE, pNativeImageData );

  glGetTexParameteriv( GL_TEXTURE_2D, GL_TEXTURE_DATA_ADDR_DMP, &pInstance->m_TextureVirtualAddr );
  pInstance->m_TextureAddr = nngxGetPhysicalAddr ( pInstance->GetTextureVirtualAddr() );

	glBindTexture( GL_TEXTURE_2D, 0 );//リセット

	pInstance->m_Type = GL_TEXTURE_2D;

  s_Gp->m_pImpl->m_pHeapAllocator[0]->Free( pNativeImageData );

  pInstance->m_pContext = pManager;

	return pInstance;
}

//-----------------------------------------------------------------------------
/// @brief	テクスチャサイズを指定してネイティブテクスチャを生成
//-----------------------------------------------------------------------------
Texture* DrawUtil::CreateTexture( gfl::grp::GraphicsSystem *pSystem, gfl::grp::RenderColorFormat format, u32 width, u32 height, const void *p_data )
{
  ISHIGURO_PRINT("DrawUtil::CreateTexture\n");
  
  gfl::grp::CommandListManager* pManager = NULL;
  pManager = pSystem->GetCommandListManager();
  pSystem->BindCurrentCommandList();

	Texture			*pInstance = GFL_NEW(s_Gp->m_pImpl->m_pDeviceAllocator[0]->GetHeapBase()) Texture();

	pInstance->m_Width = width;
	pInstance->m_Height = height;

	switch( format ){
  case RENDER_COLOR_FORMAT_RGBA8 :
		pInstance->m_Format = GL_RGBA_NATIVE_DMP;
    break;
#if 0
  case RENDER_COLOR_FORMAT_RGB8 :
		pInstance->m_Format = GL_RGB_NATIVE_DMP;
		break;
  case RENDER_COLOR_FORMAT_RGBA4 :
		pInstance->m_Format = GL_RGBA4;
    break;
  case RENDER_COLOR_FORMAT_RGB5_A1 :
		pInstance->m_Format = GL_RGB5_A1;
    break;
  case RENDER_COLOR_FORMAT_RGB565 :
		pInstance->m_Format = GL_RGB565;
    break;
#endif
	case RENDER_COLOR_FORMAT_NONE:
	default:
    GFL_ASSERT(0); //@check
    pInstance->m_Format = GL_RGBA;
		break;
	}

	glGenTextures( 1, &pInstance->m_hTexture );
  GFL_GL_ASSERT();
	glBindTexture( GL_TEXTURE_2D, pInstance->m_hTexture );
  GFL_GL_ASSERT();
	glTexImage2D( GL_TEXTURE_2D, 0, pInstance->m_Format, pInstance->m_Width, pInstance->m_Height, 0, pInstance->m_Format, GL_UNSIGNED_BYTE, p_data );
  GFL_GL_ASSERT();

  glGetTexParameteriv( GL_TEXTURE_2D, GL_TEXTURE_DATA_ADDR_DMP, &pInstance->m_TextureVirtualAddr );

  pInstance->m_TextureAddr = nngxGetPhysicalAddr ( pInstance->GetTextureVirtualAddr() );

	glBindTexture( GL_TEXTURE_2D, 0 );//リセット

	pInstance->m_Type = GL_TEXTURE_2D;

  pInstance->m_pContext = pManager;

	return pInstance;

}

//-----------------------------------------------------------------------------
/// @brief	ファイルからのCubeテクスチャ生成関数。とりあえず32Bitフォーマットのtgaのみ対応
//-----------------------------------------------------------------------------
Texture* DrawUtil::CreateCubeTextureFromMemory( gfl::grp::GraphicsSystem *pSystem, const void** p_data_array )
{
  ISHIGURO_PRINT("DrawUtil::CreateCubeTextureFromMemory\n");
  
	struct RGBA{
		u8		m_R;
		u8		m_G;
		u8		m_B;
		u8		m_A;
	};

  gfl::grp::CommandListManager* pManager = NULL;
  pManager = pSystem->GetCommandListManager();
  pSystem->BindCurrentCommandList();

	const u8*		p_data = reinterpret_cast<const u8*>( p_data_array[0] );
	TgaFormat		*pImage = (TgaFormat*)p_data;
	Texture			*pInstance = GFL_NEW(s_Gp->m_pImpl->m_pHeapAllocator[0]->GetHeapBase()) Texture();

	pInstance->m_Format = pImage->Pixel;
	pInstance->m_Width = pImage->Width;
	pInstance->m_Height = pImage->Hight;

	switch( pImage->Pixel ){
	case 24:
		pInstance->m_Format = GL_RGB;
		break;
	default:
		pInstance->m_Format = GL_RGBA;
		break;
	}

	glGenTextures( 1, &pInstance->m_hTexture );

	glBindTexture( GL_TEXTURE_CUBE_MAP, pInstance->m_hTexture );
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_LOD, 0);

  glGetTexParameteriv( GL_TEXTURE_2D, GL_TEXTURE_DATA_ADDR_DMP, &pInstance->m_TextureVirtualAddr );
  pInstance->m_TextureAddr = nngxGetPhysicalAddr ( pInstance->GetTextureVirtualAddr() );

  u8      *pNativeImageData = reinterpret_cast<u8*>( s_Gp->m_pImpl->m_pHeapAllocator[0]->Alloc( pInstance->m_Width * pInstance->m_Height * (pImage->Pixel / 8) ) );
  RGBA		*pRgbaBuff = NULL;

  if ( pInstance->m_Format != GL_RGBA )
		pRgbaBuff = reinterpret_cast<RGBA*>( s_Gp->m_pImpl->m_pHeapAllocator[0]->Alloc( pInstance->m_Width * pInstance->m_Height * sizeof(RGBA) ) );

	//PICAネイティブフォーマットに変換
	for( s32 i = 0; i < CubePageNum; ++i )
	{
		p_data = reinterpret_cast<const u8*>( p_data_array[i] );

    u8 *pImageTop = const_cast<u8*>( p_data );
    pImageTop = &pImageTop[ sizeof(TgaFormat) ];

		DrawUtil::convertPicaFormat( pNativeImageData, pImageTop, pInstance->m_Format, pInstance->m_Width, pInstance->m_Height );
    pImageTop = pNativeImageData;

		if ( pRgbaBuff )
		{
			for( s32 i2 = 0; i2 < (pInstance->m_Width * pInstance->m_Height); ++i2)
			{
				pRgbaBuff[i2].m_R = pImageTop[0];
				pRgbaBuff[i2].m_G = pImageTop[1];
				pRgbaBuff[i2].m_B = pImageTop[2];
				pRgbaBuff[i2].m_A = 255;
				pImageTop += 3;
			}
			pImageTop = (u8*)pRgbaBuff;
		}

		glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, pInstance->m_Width, pInstance->m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pImageTop );

		if ( pRgbaBuff )
			s_Gp->m_pImpl->m_pHeapAllocator[0]->Free( pRgbaBuff );
	}

	pInstance->m_Type = GL_TEXTURE_CUBE_MAP;
	pInstance->m_Format = GL_RGBA;
	glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );//リセット

  s_Gp->m_pImpl->m_pHeapAllocator[0]->Free( pNativeImageData );

  if ( pRgbaBuff )
			s_Gp->m_pImpl->m_pHeapAllocator[0]->Free( pRgbaBuff );

	return pInstance;
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
Vertex* DrawUtil::CreateVertexArray( u32 vertex_num )
{
  ISHIGURO_PRINT("DrawUtil::CreateVertexArray\n");
  
  return reinterpret_cast<gfl::grp::util::Vertex*>( s_Gp->m_pImpl->m_pDeviceAllocator[0]->Alloc(sizeof( gfl::grp::util::Vertex ) * vertex_num, 128) );
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtil::DeleteVertexArray( Vertex* pBuff )
{
  ISHIGURO_PRINT("DrawUtil::DeleteVertexArray\n");
  
  s_Gp->m_pImpl->m_pDeviceAllocator[0]->Free( pBuff );
}

//-----------------------------------------------------------------------------
/// @brief	マテリアル情報設定関数
//-----------------------------------------------------------------------------
void DrawUtil::SetMaterial( const gfl::grp::util::DrawUtil::MaterialInfo &rInfo )
{
	gfl::grp::util::ShaderObject							*pShaderObject;
	s_Gp->m_pImpl->m_ShaderType = gfl::grp::util::DrawUtil::Impl::VERTEX_COLOR_SHADER;

	switch( rInfo.m_ShadingType ){
	case gfl::grp::util::DrawUtil::MaterialInfo::COLOR_SHADER:
		s_Gp->m_pImpl->m_ShaderType = gfl::grp::util::DrawUtil::Impl::VERTEX_COLOR_SHADER;
		break;
	case gfl::grp::util::DrawUtil::MaterialInfo::TEXTURE_SHADER:
		s_Gp->m_pImpl->m_ShaderType = gfl::grp::util::DrawUtil::Impl::TEXTURE_MAP_SHADER;
		break;
  case gfl::grp::util::DrawUtil::MaterialInfo::RGBTEXTURE_SHADER:
		s_Gp->m_pImpl->m_ShaderType = gfl::grp::util::DrawUtil::Impl::RGBTEXTURE_MAP_SHADER;
		break;
	case gfl::grp::util::DrawUtil::MaterialInfo::TEXTURE_LIGHTING_SHADER:
		s_Gp->m_pImpl->m_ShaderType = gfl::grp::util::DrawUtil::Impl::TEXTURE_MAP_SHADER;
		break;
  case gfl::grp::util::DrawUtil::MaterialInfo::FRONT_DOFTEXTURE_SHADER:
		s_Gp->m_pImpl->m_ShaderType = gfl::grp::util::DrawUtil::Impl::FRONT_DOFTEXTURE_MAP_SHADER;
		break;
  case gfl::grp::util::DrawUtil::MaterialInfo::BACK_DOFTEXTURE_SHADER:
		s_Gp->m_pImpl->m_ShaderType = gfl::grp::util::DrawUtil::Impl::BACK_DOFTEXTURE_MAP_SHADER;
		break;
  case gfl::grp::util::DrawUtil::MaterialInfo::DEBUG_SHADER:
		s_Gp->m_pImpl->m_ShaderType = gfl::grp::util::DrawUtil::Impl::DEBUG_SHADER;
		break;
	}

	pShaderObject = &s_Gp->m_pImpl->m_ShaderObject[s_Gp->m_pImpl->m_ShaderType];

  pShaderObject->SetStaticShader();
  GFL_GRPGL_ERROR_CHECK();

  const IShader		*pShader = pShaderObject->GetPixelShader();
  if ( pShader )
  {
    pShader->vSetUpShader();
  }

  gfl::math::MTX44	viewModelMatrix;
	gfl::math::MTX44Mult(&viewModelMatrix, &s_Gp->m_pImpl->m_ViewMat, &s_Gp->m_pImpl->m_ModelMat);

	s_Gp->setShaderParam( s_Gp->m_pImpl->m_ShaderType, s_Gp->m_pImpl->m_ProjMat, viewModelMatrix, &rInfo );

  if ( rInfo.m_pTexture )
  {
    s_Gp->setTexture( *rInfo.m_pTexture );
    GFL_GRPGL_ERROR_CHECK();

    s_Gp->m_pImpl->m_UseTextureFlag = true;
  }
  else
  {
    s_Gp->m_pImpl->m_UseTextureFlag = false;
  }
}

//-----------------------------------------------------------------------------
/// @brief	投影マトリックス設定
//-----------------------------------------------------------------------------
void DrawUtil::SetProjectionMatrix( const gfl::math::MTX44 &mat )
{
	s_Gp->m_pImpl->m_ProjMat = mat;//コピー
}

//-----------------------------------------------------------------------------
/// @brief	ビューマトリックス設定
//-----------------------------------------------------------------------------
void DrawUtil::SetViewMatrix( const gfl::math::MTX44 &mat )
{
	s_Gp->m_pImpl->m_ViewMat = mat;//コピー
}

//-----------------------------------------------------------------------------
/// @brief	モデルマトリックス設定
//-----------------------------------------------------------------------------
void DrawUtil::SetModelMatrix( const gfl::math::MTX44 &mat )
{
	s_Gp->m_pImpl->m_ModelMat = mat;//コピー
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtil::DrawVertexList( const gfl::grp::util::Vertex *pBuffer, u32 vertex_num, b32 fSetState )
{
  if ( fSetState )
  {
    s_Gp->resetRenderState( RenderStateType::MODEL_3D );
  }

  {
    //@note マクロ化しておくときれい。
    static const u32 commandBuffer[] = {
      //入力レジスタのマッピング設定レジスタ（0x02BB, 0x02BC）
      0x00003210, PICA_CMD_HEADER(PICA_REG_VS_ATTR_IN_REG_MAP0, 0, 0x0f, 0 ),
      0x00000000, PICA_CMD_HEADER(PICA_REG_VS_ATTR_IN_REG_MAP1, 0, 0x0f, 0 )
    };
    nngxAdd3DCommand(commandBuffer, sizeof(commandBuffer), GL_TRUE);
  }

  {
    //@note マクロ化しておくときれい。
    static const u32 commandBuffer[] = {
      0x00000000, PICA_CMD_HEADER(PICA_REG_DRAW_MODE2, 0, 0x02, 0 ),//0x025e : DRAW_MODE=0->GL_TRIANGLES
      0x00000001, 0x000f025f,//RESET_VTX_INFO=1->インデックス情報リセット
      0x00000007, 0x00010080,//テクスチャ設定レジスタ samplerType0=1->!false
      0x00000001, PICA_CMD_HEADER(PICA_REG_DRAW_MODE1, 0, 0x01, 0 ),//0x0253 : DRAW_TRIANGLES2=0=>GL_TRIANGLES, CALL_DRAWARRAYS=1
      0x00000000, 0x00010245,//DRAW_START 0
      0x0000000f, 0x000f0232,//頂点属性データの設定
    };
    nngxAdd3DCommand(commandBuffer, sizeof(commandBuffer), GL_TRUE);
  }

  s_Gp->writeVertexAttributeCommand( pBuffer, vertex_num );

  {
    //@note マクロ化しておくときれい。
    static const u32 commandBuffer[] = {
      0x00000001, 0x00010245,//DRAW_START 1
      0x00000001, 0x000f0231,//CLEAR_POST_VTX=1 必ず設定する
      0x00000000, 0x00010253,//DRAW_TRIANGLES2=0=>GL_TRIANGLES, CALL_DRAWARRAYS=0
      0x00000001, 0x000f0111,//フレームバッファキャッシュクリア
      0x00000000, 0x00010080,//テクスチャ設定レジスタ samplerType0=0->false
      0x7fff0000, 0x000c02ba,//上の7fffと組み合わせ消費電力軽減のためのコマンド
    };
    nngxAdd3DCommand(commandBuffer, sizeof(commandBuffer), GL_TRUE);
  }
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtil::DrawVertexStrip(const gfl::grp::util::Vertex *pBuffer, u32 vertex_num, b32 fSetState )
{
  if ( fSetState )
  {
    s_Gp->resetRenderState( RenderStateType::MODEL_3D );
  }

  {
    //@note マクロ化しておくときれい。
    static const u32 commandBuffer[] = {
      //頂点属性アレイ設定レジスタ（0x0200 ? 0x0227）頂点バッファを使わないので設定不要
      //入力レジスタのマッピング設定レジスタ（0x02BB, 0x02BC）
      0x00003210, PICA_CMD_HEADER(PICA_REG_VS_ATTR_IN_REG_MAP0, 0, 0x0f, 0 ),
      0x00000000, PICA_CMD_HEADER(PICA_REG_VS_ATTR_IN_REG_MAP1, 0, 0x0f, 0 )
    };
    nngxAdd3DCommand(commandBuffer, sizeof(commandBuffer), GL_TRUE);
  }

  {
    //@note マクロ化しておくときれい。
    static const u32 commandBuffer[] = {
      0x00000100, PICA_CMD_HEADER(PICA_REG_DRAW_MODE2, 0, 0x02, 0 ),//0x025e : DRAW_MODE=1->GL_TRIANGLESTRIP
      0x00000001, 0x000f025f,//RESET_VTX_INFO=1->インデックス情報リセット
      0x00000007, 0x00010080,//テクスチャ設定レジスタ samplerType0=1->!false
      0x00000001, PICA_CMD_HEADER(PICA_REG_DRAW_MODE1, 0, 0x01, 0 ),//0x0253 : DRAW_TRIANGLES2=0=>GL_TRIANGLES, CALL_DRAWARRAYS=1
      0x00000000, 0x00010245,//DRAW_START 0
      0x0000000f, 0x000f0232,//頂点属性データの設定
    };
    nngxAdd3DCommand(commandBuffer, sizeof(commandBuffer), GL_TRUE);
  }

  s_Gp->writeVertexAttributeCommand( pBuffer, vertex_num );

  {
    //@note マクロ化しておくときれい。
    static const u32 commandBuffer[] = {
      0x00000001, 0x00010245,//DRAW_START 1
      0x00000001, 0x000f0231,//CLEAR_POST_VTX=1 必ず設定する
      0x00000000, 0x00010253,//DRAW_TRIANGLES2=0=>GL_TRIANGLES, CALL_DRAWARRAYS=0
      0x00000001, 0x000f0111,//フレームバッファキャッシュクリア
      0x00000000, 0x00010080,//テクスチャ設定レジスタ samplerType0=0->false
      0x7fff0000, 0x000c02ba,//上の7fffと組み合わせ消費電力軽減のためのコマンド
    };
    nngxAdd3DCommand(commandBuffer, sizeof(commandBuffer), GL_TRUE);
  }
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtil::DrawVertexList( const gfl::grp::util::VertexObject &rObject )
{
	s32			offset = 0;

	glBindBuffer( GL_ARRAY_BUFFER, rObject.GetVertexBufferId() );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, rObject.GetIndexBufferId() );

	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(gfl::grp::util::Vertex), (GLvoid*)offset );
	offset += sizeof(f32) * 3;

	glEnableVertexAttribArray( 1 );
	glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, sizeof(gfl::grp::util::Vertex), (GLvoid*)offset );
	offset += sizeof(f32) * 4;

	glEnableVertexAttribArray( 2 );
	glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof(gfl::grp::util::Vertex), (GLvoid*)offset );
	offset += sizeof(f32) * 2;

	glEnableVertexAttribArray( 3 );
	glVertexAttribPointer( 3, 3, GL_FLOAT, GL_FALSE, sizeof(gfl::grp::util::Vertex), (GLvoid*)offset );
	offset += sizeof(f32) * 3;

	glDisableVertexAttribArray(4);
	glDisableVertexAttribArray(5);
	glDisableVertexAttribArray(6);
	glDisableVertexAttribArray(7);

	glDrawElements( GL_TRIANGLES, rObject.GetIndexNum(), GL_UNSIGNED_SHORT, 0 );
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtil::DrawVertexStrip( const gfl::grp::util::VertexObject &rObject )
{
	s32			offset = 0;

	glBindBuffer( GL_ARRAY_BUFFER, rObject.GetVertexBufferId() );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, rObject.GetIndexBufferId() );

	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(gfl::grp::util::Vertex), (GLvoid*)offset );
	offset += sizeof(f32) * 3;

	glEnableVertexAttribArray( 1 );
	glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, sizeof(gfl::grp::util::Vertex), (GLvoid*)offset );
	offset += sizeof(f32) * 4;

	glEnableVertexAttribArray( 2 );
	glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof(gfl::grp::util::Vertex), (GLvoid*)offset );
	offset += sizeof(f32) * 2;

	glEnableVertexAttribArray( 3 );
	glVertexAttribPointer( 3, 3, GL_FLOAT, GL_FALSE, sizeof(gfl::grp::util::Vertex), (GLvoid*)offset );
	offset += sizeof(f32) * 3;

	glDisableVertexAttribArray(4);
	glDisableVertexAttribArray(5);
	glDisableVertexAttribArray(6);
	glDisableVertexAttribArray(7);

	glDrawElements( GL_TRIANGLE_STRIP, rObject.GetIndexNum(), GL_UNSIGNED_SHORT, 0 );
}

//-----------------------------------------------------------------------------
/// @brief	PICAネイティブフォーマットに変換する関数
//-----------------------------------------------------------------------------
void DrawUtil::convertPicaFormat( u8* p_dst, const u8* p_src, GLenum format, s32 w, s32 h )
{
	switch( format ){
	case GL_RGB:
		for( s32 y = 0; y < h; ++y )
		{
			for( s32 x = 0; x < w; ++x )
			{
        p_dst[0] = p_src[2];
        p_dst[1] = p_src[1];
        p_dst[2] = p_src[0];
				p_src += 3;
        p_dst += 3;
			}
		}
		break;
	default:
		for( s32 y = 0; y < h; ++y )
		{
			for( s32 x = 0; x < w; ++x )
			{
        p_dst[0] = p_src[2];
        p_dst[1] = p_src[1];
        p_dst[2] = p_src[0];
        p_dst[3] = p_src[3];
				p_src += 4;
        p_dst += 4;
			}
		}
		break;
	}
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtil::initDefaultRenderer()
{
#if 0
  static const u8 uVSStanderdData[] = {
    #include "uVSStanderd.cdat"
  };

  m_pImpl->m_ShaderId[DrawUtil::Impl::STANDERD_VS] = gfl::grp::util::ShaderObject::CreateVertexShaderFromMemory( uVSStanderdData, sizeof(uVSStanderdData) );

	for( u32 i = 0; i < DrawUtil::Impl::SHADER_MAX; ++i )
	{
		m_pImpl->m_ShaderObject[i].CreateProgram();
	}
#endif

#if 0
  //@note マクロ化しておくときれい。
  static u32 commandBuffer[] = {
    //ジオメトリシェーダを使用しない
    //(レジスタ 0x0244 のビット [ 0 : 0 ] が 0、かつレジスタ 0x0229 のビット [ 1 : 0 ] が 0 )場合
    0x00000000, 0x00010244,//(0x0244)

    //プログラムコード設定レジスタ（0x02BF, 0x02CB ? 0x02D3, 0x02D5 ? 0x02DD）
    0x00000000, 0x000f02cb,//(0x02cb)頂点シェーダのプログラムコードをロードするアドレス指定

    //0x02CC ? 0x02D3 には、ロードするプログラムコードのデータを書き込みます。
    0x0be24000, 0x01af02cc,//(0x02cc)
    0x0be25001, 0x0be26002,
    0x0be27003, 0x4c81f004,
    0x08020f80, 0x08021f81,
    0x08022f82, 0x08023f83,
    0x07c24180, 0x07c25181,
    0x07c26182, 0xb807df05,
    0x0287df06, 0x2287ea07,
    0x4c27d008, 0x3e814009,
    0x22a7ef07, 0xa1805401,
    0x3821400a, 0x20215a0b,
    0x4c27d00c, 0x84000000,
    0x4c40100d, 0x4c60200d,
    0x88000000, 0x84000000,

    0x00000001, 0x000f02bf,//(0x02bf)プログラムの更新完了通知

    //Swizzle パターンのロードレジスタ（0x02D5 ? 0x02DD）
    0x00000000, 0x000f02d5,//(0x02d5)Swizzle パターンをロードするアドレス
    0x8006c368, 0x00df02d6,//(0x02d6)ロードする Swizzle パターンのデータを書き込みます。d=13->4
    0x8006c364, 0x8006c362,
    0x8006c361, 0x8000037f,
    0x802a8ab0, 0x802a8aaf,
    0x8006d54f, 0x80000001,
    0x8000000f, 0x80000002,
    0x8006c36c, 0x8000080e,
    0x8000036f, 0x00005544,

    //頂点属性入力数設定レジスタ（0x0242, 0x02B9）
    0xa0000003, 0x000b02b9,

    0x7fff0000, 0x000f02ba,//(0x02BA)開始アドレス設定レジスタ
    0x0000001f, 0x000f02bd,//(0x02BD)出力レジスタのマスク設定レジスタ

    //出力レジスタ使用数設定レジスタ（0x004F, 0x024A, 0x0251, 0x025E）
    0x00000004, 0x000f0251,//(0x0251)使用する出力レジスタの個数 - 1

    //頂点属性入力数設定レジスタ（0x0242, 0x02B9）
    0x00000003, 0x000f0242,

    //出力レジスタ使用数設定レジスタ（0x004F, 0x024A, 0x0251, 0x025E）
    0x00000004, 0x000f024a,//使用する出力レジスタの個数 - 1
    0x00000004, 0x0001025e,//使用する出力レジスタの個数 - 1
    0x00000005, 0x000f004f,//出力レジスタの個数そのまま

    //出力レジスタの属性設定レジスタ（0x0050 ? 0x0056, 0x0064）
    0x03020100, 0x000f0050,
    0x07060504, 0x000f0051,
    0x0b0a0908, 0x000f0052,
    0x1f1f0d0c, 0x000f0053,
    0x1f141312, 0x000f0054,
    0x00000001, 0x000f0064,

    //出力属性のクロック制御レジスタ（0x006F）
    0x01000103, 0x000f006f,

    //フレームバッファキャッシュクリアの設定レジスタ（0x0110, 0x0111）
    0x00000001, 0x000f0111,
    0x00000001, 0x000f0110,

    //フレームバッファアクセス制御設定レジスタ（0x0112 ? 0x0115）
    0x00000000, 0x000f0112,
    0x0000000f, 0x000f0113,
    0x00000000, 0x000f0114,
    0x00000000, 0x000f0115,
  };
#else

  //@note マクロ化しておくときれい。
  static u32 commandBuffer[] = {
    //ジオメトリシェーダを使用しない
    //(レジスタ 0x0244 のビット [ 0 : 0 ] が 0、かつレジスタ 0x0229 のビット [ 1 : 0 ] が 0 )場合
    0x00000000, 0x00010244,//(0x0244)

    //プログラムコード設定レジスタ（0x02BF, 0x02CB ? 0x02D3, 0x02D5 ? 0x02DD）
    0x00000000, 0x000f02cb,//(0x02cb)頂点シェーダのプログラムコードをロードするアドレス指定

    //0x02CC ? 0x02D3 には、ロードするプログラムコードのデータを書き込みます。
    0x0be24000, 0x01ff02cc,//(0x02cc)
    0x0be25001, 0x0be26002,
    0x0be27003, 0x4c81f004,
    0x08020f80, 0x08021f81,
    0x08022f82, 0x08023f83,
    0x07c24180, 0x07c25181,
    0x07c26182, 0xb807df05,
    0x0287df06, 0x2287ea07,
    0x4c27d008, 0x3e814009,
    0x22a7ef07, 0xa1805401,
    0x3821400a, 0x20215a0b,
    0x4c27d00c, 0x84000000,
    0x4c40100d, 0x02c2810d,
    0x4c61600d, 0x02c2910d,
    0x4ca1600d, 0x02c2a10d,
    0x4cc1600d, 0x88000000,
    0x84000000, 0x00005544,

    0x00000001, 0x000f02bf,//(0x02bf)プログラムの更新完了通知

    //Swizzle パターンのロードレジスタ（0x02D5 ? 0x02DD）
    0x00000000, 0x000f02d5,//(0x02d5)Swizzle パターンをロードするアドレス
    0x8006c368, 0x00df02d6,//(0x02d6)ロードする Swizzle パターンのデータを書き込みます。d=13->4
    0x8006c364, 0x8006c362,
    0x8006c361, 0x8000037f,
    0x802a8ab0, 0x802a8aaf,
    0x8006d54f, 0x80000001,
    0x8000000f, 0x80000002,
    0x8006c36c, 0x8000080e,
    0x8006c36f, 0x00005544,

    //頂点属性入力数設定レジスタ（0x0242, 0x02B9）
    0xa0000003, 0x000b02b9,

    0x7fff0000, 0x000f02ba,//(0x02BA)開始アドレス設定レジスタ
    0x0000007f, 0x000f02bd,//(0x02BD)出力レジスタのマスク設定レジスタ

    //出力レジスタ使用数設定レジスタ（0x004F, 0x024A, 0x0251, 0x025E）
    0x00000006, 0x000f0251,//(0x0251)使用する出力レジスタの個数 - 1

    //頂点属性入力数設定レジスタ（0x0242, 0x02B9）
    0x00000003, 0x000f0242,

    //出力レジスタ使用数設定レジスタ（0x004F, 0x024A, 0x0251, 0x025E）
    0x00000006, 0x000f024a,//使用する出力レジスタの個数 - 1
    0x00000006, 0x0001025e,//使用する出力レジスタの個数 - 1
    0x00000007, 0x000f004f,//出力レジスタの個数そのまま

    //出力レジスタの属性設定レジスタ（0x0050 ? 0x0056, 0x0064）
    0x03020100, 0x000f0050,
    0x07060504, 0x000f0051,
    0x0b0a0908, 0x000f0052,
    0x1f1f0d0c, 0x000f0053,
    0x1f141312, 0x000f0054,
    0x1f1f0f0e, 0x000f0055,
    0x1f1f1716, 0x000f0056,
    0x00000001, 0x000f0064,

    //出力属性のクロック制御レジスタ（0x006F）
    0x01000703, 0x000f006f,

    //フレームバッファキャッシュクリアの設定レジスタ（0x0110, 0x0111）
    0x00000001, 0x000f0111,
    0x00000001, 0x000f0110,

    //フレームバッファアクセス制御設定レジスタ（0x0112 ? 0x0115）
    0x00000000, 0x000f0112,
    0x0000000f, 0x000f0113,
    0x00000000, 0x000f0114,
    0x00000000, 0x000f0115,
  };

#endif

	//m_pImpl->m_ShaderObject[DrawUtil::Impl::VERTEX_COLOR_SHADER].AttachVertexShader( m_pImpl->m_ShaderId[DrawUtil::Impl::STANDERD_VS] );
  m_pImpl->m_ShaderObject[DrawUtil::Impl::VERTEX_COLOR_SHADER].AttachStaticVertexShader( commandBuffer, sizeof(commandBuffer) );
	m_pImpl->m_ShaderObject[DrawUtil::Impl::VERTEX_COLOR_SHADER].AttachPixelShader( &m_pImpl->m_VertexColorPixelShader );
	GFL_GRPGL_ERROR_CHECK();

	//m_pImpl->m_ShaderObject[DrawUtil::Impl::TEXTURE_MAP_SHADER].AttachVertexShader( m_pImpl->m_ShaderId[DrawUtil::Impl::STANDERD_VS] );
  m_pImpl->m_ShaderObject[DrawUtil::Impl::TEXTURE_MAP_SHADER].AttachStaticVertexShader( commandBuffer, sizeof(commandBuffer) );
	m_pImpl->m_ShaderObject[DrawUtil::Impl::TEXTURE_MAP_SHADER].AttachPixelShader( &m_pImpl->m_TextureMapPixelShader );
	GFL_GRPGL_ERROR_CHECK();

  m_pImpl->m_ShaderObject[DrawUtil::Impl::RGBTEXTURE_MAP_SHADER].AttachStaticVertexShader( commandBuffer, sizeof(commandBuffer) );
	m_pImpl->m_ShaderObject[DrawUtil::Impl::RGBTEXTURE_MAP_SHADER].AttachPixelShader( &m_pImpl->m_RgbTextureMapPixelShader );
	GFL_GRPGL_ERROR_CHECK();

  //m_pImpl->m_ShaderObject[DrawUtil::Impl::TEXTURE_LIGHTING_SHADER].AttachVertexShader( m_pImpl->m_ShaderId[DrawUtil::Impl::STANDERD_VS] );
  m_pImpl->m_ShaderObject[DrawUtil::Impl::TEXTURE_LIGHTING_SHADER].AttachStaticVertexShader( commandBuffer, sizeof(commandBuffer) );
	m_pImpl->m_ShaderObject[DrawUtil::Impl::TEXTURE_LIGHTING_SHADER].AttachPixelShader( &m_pImpl->m_TexDirLightPixelShader );
	GFL_GRPGL_ERROR_CHECK();

  m_pImpl->m_ShaderObject[DrawUtil::Impl::FRONT_DOFTEXTURE_MAP_SHADER].AttachStaticVertexShader( commandBuffer, sizeof(commandBuffer) );
	m_pImpl->m_ShaderObject[DrawUtil::Impl::FRONT_DOFTEXTURE_MAP_SHADER].AttachPixelShader( &m_pImpl->m_FrontDofTextureMapPixelShader );
	GFL_GRPGL_ERROR_CHECK();

  m_pImpl->m_ShaderObject[DrawUtil::Impl::BACK_DOFTEXTURE_MAP_SHADER].AttachStaticVertexShader( commandBuffer, sizeof(commandBuffer) );
	m_pImpl->m_ShaderObject[DrawUtil::Impl::BACK_DOFTEXTURE_MAP_SHADER].AttachPixelShader( &m_pImpl->m_BackDofTextureMapPixelShader );
	GFL_GRPGL_ERROR_CHECK();

  m_pImpl->m_ShaderObject[DrawUtil::Impl::DEBUG_SHADER].AttachStaticVertexShader( commandBuffer, sizeof(commandBuffer) );
	m_pImpl->m_ShaderObject[DrawUtil::Impl::DEBUG_SHADER].AttachPixelShader( &m_pImpl->m_DebugPixelShader );
	GFL_GRPGL_ERROR_CHECK();
}
//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtil::initFontRenderer()
{
  static const u8   nwfont_RectDrawerShaderData[] = {
    #include "nwfont_RectDrawerShader.cdat"
  };

  const u32 vtxBufCmdBufSize = nw::font::RectDrawer::GetVertexBufferCommandBufferSize( nwfont_RectDrawerShaderData, sizeof(nwfont_RectDrawerShaderData) );
  m_pImpl->m_pRectDrawerCmdBuf = m_pImpl->m_pDeviceAllocator[0]->Alloc(vtxBufCmdBufSize, 128);
  m_pImpl->m_RectDrawer.Initialize( m_pImpl->m_pRectDrawerCmdBuf, nwfont_RectDrawerShaderData, sizeof(nwfont_RectDrawerShaderData) );

  //文字列格納バッファ確保
  const u32 DrawBufferSize = nw::font::CharWriter::GetDispStringBufferSize(CharMax);
  void *const bufMem = m_pImpl->m_pDeviceAllocator[0]->Alloc(DrawBufferSize, 128);
  m_pImpl->m_pDrawStringBuf = nw::font::CharWriter::InitDispStringBuffer(bufMem, CharMax);
}

//-----------------------------------------------------------------------------
/// @brief	持っているパラメーターをシェーダーに渡す
//-----------------------------------------------------------------------------
void DrawUtil::setShaderParam( s32 type, const gfl::math::MTX44& proj_mat, const gfl::math::MTX44& view_mat, const gfl::grp::util::DrawUtil::MaterialInfo *pInfo )
{
	//シェーダーが持っているハンドルに対してパラメーターを設定
  {
    static const u32 commandBuffer[] = {
      //浮動小数点定数レジスタインデックス指定（0x02C0）
      0x80000000, 0x000f02c0//0x00="c0", Mode=1->f32
    };
    nngxAdd3DCommand(commandBuffer, sizeof(commandBuffer), GL_TRUE);

    cRegMatrixData      matrixData;
    matrixData.Set( proj_mat );
    nngxAdd3DCommand( &matrixData, sizeof(matrixData), GL_TRUE );
  }
	GFL_GRPGL_ERROR_CHECK();

	{
    static const u32 commandBuffer[] = {
      //浮動小数点定数レジスタインデックス指定（0x02C0）
      0x80000004, 0x000f02c0//0x04="c4", Mode=1->f32
    };
    nngxAdd3DCommand(commandBuffer, sizeof(commandBuffer), GL_TRUE);

    cRegMatrixData      matrixData;
    matrixData.Set( view_mat );
    nngxAdd3DCommand( &matrixData, sizeof(matrixData), GL_TRUE );
	}
	GFL_GRPGL_ERROR_CHECK();

  if ( pInfo )
  {
    static const u32 commandBuffer[] = {
      //浮動小数点定数レジスタインデックス指定（0x02C0）
      0x80000008, 0x000f02c0//0x08="c8", Mode=1->f32
    };
    nngxAdd3DCommand(commandBuffer, sizeof(commandBuffer), GL_TRUE);

    cRegVectorData      vecData;
    vecData.Set( pInfo->m_UvScroll );
    nngxAdd3DCommand( &vecData, sizeof(vecData), GL_TRUE );
	}
	GFL_GRPGL_ERROR_CHECK();
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtil::setTexture( const gfl::grp::util::Texture &Texture, GLuint /*samplingtype*/ )
{
  setTexture( 0, Texture.GetTextureAddr(), Texture.GetWidth(), Texture.GetHeight(), Texture.GetTextureFormat() );
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtil::setTexture( u32 textureNo, u32 textureAddr, u32 textureWidth, u32 textureHeight, u32 textureFormat, f32 offset_u, f32 offset_v )
{
  u32     textureFormatData = ( textureFormat == GL_RGB ) ? PICA_DATA_TEXTURE_FORMAT_RGB_UNSIGNED_BYTE : PICA_DATA_TEXTURE_FORMAT_RGBA_UNSIGNED_BYTE;

  switch( textureNo ){
  case 0:
   {
      //@note マクロ化しておくときれい。
      u32 commandBuffer[] = {
        //テクスチャパラメータ設定レジスタ（0x0081?0x008Aまで）
        0x00000000, 0x809f0081,//(0x0081)ボーダーカラー
        PICA_CMD_DATA_TEXTURE_SIZE( textureWidth, textureHeight ),//(0x0082)テクスチャ解像度
        0x00002206,//(0x0083)ラップモード(GL_LINEAR)
        0x00000000,//(0x0084)フィルタ
        PICA_CMD_DATA_TEXTURE_ADDR( textureAddr ),//(0x0085)テクスチャ8Byteアドレス（0x0085 ? 0x008A, 0x0095, 0x009D）
        0x00000000,//(0x0086)Cubeテクスチャ8Byteアドレス
        0x00000000,//(0x0087)Cubeテクスチャ8Byteアドレス
        0x00000000,//(0x0088)Cubeテクスチャ8Byteアドレス
        0x00000000,//(0x0089)Cubeテクスチャ8Byteアドレス
        0x00000000,//(0x008A)Cubeテクスチャ8Byteアドレス
        0x00000000,//padding
        textureFormatData, 0x000f008e,//(0x008E)テクスチャフォーマット設定レジスタ
        0x00011007, 0x00010080,//(0x0080)テクスチャSamplerType
        0x00010000, 0x00050080,//(0x0080)テクスチャキャッシュクリア
      };
      nngxAdd3DCommand(commandBuffer, sizeof(commandBuffer), GL_TRUE);
    }
    break;
  case 1:
    {
      //@note マクロ化しておくときれい。
      u32 commandBuffer[] = {
        //テクスチャパラメータ設定レジスタ
        0x00000000, 0x804f0091,//(0x0091)ボーダーカラー
        PICA_CMD_DATA_TEXTURE_SIZE( textureWidth, textureHeight ),//(0x0092)テクスチャ解像度
        0x00002206,//(0x0093)ラップモード(GL_LINEAR)
        0x00000000,//(0x0094)フィルタ
        PICA_CMD_DATA_TEXTURE_ADDR( textureAddr ),//(0x0095)アドレス
        textureFormatData, 0x000f0096,//(0x0096)テクスチャフォーマット設定レジスタ
        0x00011007, 0x00010080,//(0x0080)テクスチャSamplerType
        0x00010000, 0x00050080,//(0x0080)テクスチャキャッシュクリア
      };
      nngxAdd3DCommand(commandBuffer, sizeof(commandBuffer), GL_TRUE);
    }
    break;
  case 2:
    {
      //@note マクロ化しておくときれい。
      u32 commandBuffer[] = {
        //テクスチャパラメータ設定レジスタ
        0x00000000, 0x804f0099,//(0x0099)ボーダーカラー
        PICA_CMD_DATA_TEXTURE_SIZE( textureWidth, textureHeight ),//(0x009A)テクスチャ解像度
        0x00002206,//(0x009B)ラップモード(GL_LINEAR)
        0x00000000,//(0x009C)フィルタ
        PICA_CMD_DATA_TEXTURE_ADDR( textureAddr ),//(0x009D)アドレス
        textureFormatData, 0x000f009e,//(0x009E)テクスチャフォーマット設定レジスタ
        0x00011007, 0x00010080,//(0x0080)テクスチャSamplerType
        0x00010000, 0x00050080,//(0x0080)テクスチャキャッシュクリア
      };
      nngxAdd3DCommand(commandBuffer, sizeof(commandBuffer), GL_TRUE);
    }
    break;
  }
  
  s_Gp->m_pImpl->m_UseTextureFlag = true;
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtil::SetUvScroll( u32 textureNo, f32 scroll_u, f32 scroll_v )
{
  {
    u32 commandBuffer[] = {
      //浮動小数点定数レジスタインデックス指定（0x02C0）
      0x80000008 + textureNo, 0x000f02c0//0x08="c8", Mode=1->f32
    };
    nngxAdd3DCommand(commandBuffer, sizeof(commandBuffer), GL_TRUE);

    cRegVectorData      vecData;
    vecData.Set( gfl::math::VEC4(scroll_u, scroll_v, 0.0f, 0.0f) );
    nngxAdd3DCommand( &vecData, sizeof(vecData), GL_TRUE );
	}
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtil::resetRenderState( RenderStateType type )
{
  nw::font::ColorBufferInfo colBufInfo;

  DrawUtil::getColorBuffer( colBufInfo );

  f32 left, top, right, bottom;
  m_pImpl->m_pTargetFrameBuffer->GetViewport( &left, &top, &right, &bottom );

  colBufInfo.width = bottom - top;
  colBufInfo.height = right - left;
  colBufInfo.depth = PICA_DATA_DEPTH24_STENCIL8_EXT;

  {
    const u32 screenSettingCommands[] =
    {
        PICA_CMD_SET_DISABLE_ALPHA_TEST(),
        PICA_CMD_SET_BLEND_DEFAULT(),
        // ビューポートの設定
        NW_FONT_CMD_SET_VIEWPORT( 0, 0, colBufInfo.width, colBufInfo.height ),

        // wバッファの無効化
        // デプスレンジの設定
        // ポリゴンオフセットの無効化
        NW_FONT_CMD_SET_WBUFFER_DEPTHRANGE_POLYGONOFFSET(
            0.0f,           // wScale : 0.0 でWバッファが無効
            0.0f,           // depth range near
            1.0f,           // depth range far
            0,              // polygon offset units : 0.0 で ポリゴンオフセットが無効
            colBufInfo),

        PICA_CMD_SET_GAS_FOG_MODE(0, 0, 0),
    };

    nngxAdd3DCommand(screenSettingCommands, sizeof(screenSettingCommands), true);
  }



  static const u32 s_Init3DCommands[] =
  {
      // カリングを無効
      //NW_FONT_CMD_SET_CULL_FACE( NW_FONT_CMD_CULL_FACE_FRONT ),
      NW_FONT_CMD_SET_CULL_FACE( NW_FONT_CMD_CULL_FACE_DISABLE ),

      // ステンシルテストを無効
      NW_FONT_CMD_SET_DISABLE_STENCIL_TEST(),

      // デプステストを有効
      // カラーバッファの全ての成分を書き込み可
      NW_FONT_CMD_SET_DEPTH_FUNC_COLOR_MASK(
          true,   // isDepthTestEnabled
          PICA_DATA_DEPTH_TEST_LEQUAL,      // depthFunc
          true,   // depthMask
          true,   // red
          true,   // green
          true,   // blue
          true),  // alpha

      // アーリーデプステストを無効
      NW_FONT_CMD_SET_ENABLE_EARLY_DEPTH_TEST( false ),

      // フレームバッファアクセス制御
      NW_FONT_CMD_SET_FBACCESS(
          true,   // colorRead
          true,   // colorWrite
          true,   // depthRead
          true,   // depthWrite
          false,  // stencilRead
          false), // stencilWrite
  };

  static const u32 s_InitModel2DCommands[] =
  {
      // カリングを無効
      NW_FONT_CMD_SET_CULL_FACE( NW_FONT_CMD_CULL_FACE_FRONT ),
      //NW_FONT_CMD_SET_CULL_FACE( NW_FONT_CMD_CULL_FACE_DISABLE ),

      // ステンシルテストを無効
      NW_FONT_CMD_SET_DISABLE_STENCIL_TEST(),

      // デプステストを有効
      // カラーバッファの全ての成分を書き込み可
      NW_FONT_CMD_SET_DEPTH_FUNC_COLOR_MASK(
          false,  // isDepthTestEnabled
          PICA_DATA_DEPTH_TEST_LEQUAL,      // depthFunc
          true,   // depthMask
          true,   // red
          true,   // green
          true,   // blue
          true),  // alpha

      // アーリーデプステストを無効
      NW_FONT_CMD_SET_ENABLE_EARLY_DEPTH_TEST( false ),

      // フレームバッファアクセス制御
      NW_FONT_CMD_SET_FBACCESS(
          true,   // colorRead
          true,   // colorWrite
          false,  // depthRead
          false,  // depthWrite
          false,  // stencilRead
          false), // stencilWrite
  };

  static const u32 s_Init2DCommands[] =
  {
     // カリングを無効
     NW_FONT_CMD_SET_CULL_FACE( NW_FONT_CMD_CULL_FACE_DISABLE ),

     // ステンシルテストを無効
     NW_FONT_CMD_SET_DISABLE_STENCIL_TEST(),

     // デプステストを無効
     // カラーバッファの全ての成分を書き込み可
     NW_FONT_CMD_SET_DEPTH_FUNC_COLOR_MASK(
         false,  // isDepthTestEnabled
         PICA_DATA_DEPTH_TEST_ALWAYS,      // depthFunc
         true,   // depthMask
         true,   // red
         true,   // green
         true,   // blue
         true),  // alpha

     // アーリーデプステストを無効
     NW_FONT_CMD_SET_ENABLE_EARLY_DEPTH_TEST( false ),

     // フレームバッファアクセス制御
     NW_FONT_CMD_SET_FBACCESS(
         true,   // colorRead
         true,   // colorWrite
         false,  // depthRead
         false,  // depthWrite
         false,  // stencilRead
         false), // stencilWrite
   };

  switch( type ){
  case RenderStateType::FONT_2D:
    nngxAdd3DCommand(s_Init2DCommands, sizeof(s_Init2DCommands), true);
    break;
    case RenderStateType::MODEL_3D:
    nngxAdd3DCommand(s_Init3DCommands, sizeof(s_Init3DCommands), true);
    break;
  default:
    nngxAdd3DCommand(s_Init3DCommands, sizeof(s_Init3DCommands), true);
    break;
  }
  setRenderScissorState( colBufInfo );
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtil::getColorBuffer( nw::font::ColorBufferInfo &colBufInfo )
{
  f32 left, top, right, bottom;
  m_pImpl->m_pTargetFrameBuffer->GetViewport( &left, &top, &right, &bottom );

  colBufInfo.width = bottom - top;
  colBufInfo.height = right - left;
  colBufInfo.depth = PICA_DATA_DEPTH24_STENCIL8_EXT;
}


//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtil::setRenderScissorState( void )
{
  s_Gp->m_pImpl->m_pTargetFrameBuffer->ActivateBuffer();
  nw::font::ColorBufferInfo colBufInfo;
  DrawUtil::getColorBuffer( colBufInfo );
  setRenderScissorState( colBufInfo );
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtil::setRenderScissorState( nw::font::ColorBufferInfo &colBufInfo )
{
  if( m_pImpl->m_ScissorEnable )
  {
    u16 x = colBufInfo.width  - m_pImpl->m_ScissorRect.x - m_pImpl->m_ScissorRect.width;
    u16 y = colBufInfo.height - m_pImpl->m_ScissorRect.y - m_pImpl->m_ScissorRect.height;

     u32 scissorEnableCommand[] =
     {
         // シザー処理有効
         PICA_CMD_DATA_SCISSOR( true ),
         PICA_CMD_HEADER_BURSTSEQ( PICA_REG_SCISSOR, 3 ),
         (x) | (y<<16),
         (x+m_pImpl->m_ScissorRect.width-1) | ((y+m_pImpl->m_ScissorRect.height-1)<<16),
     };
     nngxAdd3DCommand(scissorEnableCommand, sizeof(scissorEnableCommand), true);
  }
  else
  {
    u32 scissorDisableCommand[] =
    {
        // シザー処理を無効
        NW_FONT_CMD_SET_DISABLE_SCISSOR( colBufInfo ),
    };
    nngxAdd3DCommand(scissorDisableCommand, sizeof(scissorDisableCommand), true);
  }
}

bool DrawUtil::Debug::G3DH3DMODEL_DEBUG_PRINT = true;

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtil::Debug::CommandLogBegin()
{
  s_Gp->m_pImpl->m_StartAddr = 0;
  s_Gp->m_pImpl->m_EndAddr = 0;

  nngxValidateState(NN_GX_STATE_ALL, false);//ここまでのコマンドを生成し積む
  nngxGetCmdlistParameteri(NN_GX_CMDLIST_CURRENT_BUFADDR, (GLint*)&s_Gp->m_pImpl->m_StartAddr);
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtil::Debug::CommandLogEnd()
{
  nngxValidateState(NN_GX_STATE_ALL, false);//ここまでのコマンドを生成し積む
  nngxGetCmdlistParameteri(NN_GX_CMDLIST_CURRENT_BUFADDR, (GLint*)&s_Gp->m_pImpl->m_EndAddr);

  NN_LOG("%08x -> %08x\n", s_Gp->m_pImpl->m_StartAddr, s_Gp->m_pImpl->m_EndAddr );

  u32     *pCommand = (u32*)s_Gp->m_pImpl->m_StartAddr;

  for( int i = 0; i < (s_Gp->m_pImpl->m_EndAddr - s_Gp->m_pImpl->m_StartAddr); i += 8 )
  {
    NN_LOG( "0x%08x, 0x%08x,\n", pCommand[0], pCommand[1] );
    pCommand += 2;
  }
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtil::Debug::CommandDump()
{
  struct LocalFunc{
    static void *getLogicalAddr(u32 addr)
    {
      NN_ASSERTMSG(addr - (0x0c000000+0x14000000) < 0x08000000, "not on device memory");
      return reinterpret_cast<void*>(addr - 0x0c000000);
    }
  };

  struct CommandRequest
  {
    u32 type;
    u32 *buf;
    u32 size;
    u32 padding[4];
  };
  
  u32 splitData[4] = {0xffffffff, 0xffffffff /* addr */ /* size */ };
  u32* jumpBufAddr[2] = { NULL, NULL };
  u32 jumpBufSize[2] = { 0, 0 };
  const int ReqType3D = 1;

  const CommandRequest* reqs;
  nngxGetCmdlistParameteri(NN_GX_CMDLIST_TOP_REQADDR, reinterpret_cast<int*>(&reqs));
  int usedReqCount;
  nngxGetCmdlistParameteri(NN_GX_CMDLIST_USED_REQCOUNT, &usedReqCount);

  nn::fs::FileOutputStream s(L"pc:/CommandDunmp.bin", true);

  for (int reqIndex = 0; reqIndex < usedReqCount; reqIndex ++)
  {
    const CommandRequest* req = &reqs[reqIndex];
    if (req->type == ReqType3D)
    {
      splitData[2] = reinterpret_cast<u32>(req->buf);
      splitData[3] = reinterpret_cast<u32>(req->size);
      s.Write(splitData, sizeof(splitData), false);
      s.Write(req->buf, req->size, false);
      NN_LOG("[%d] %p, %x\n", reqIndex, req->buf, req->size);

      const u32* pCommand = req->buf;
      u32 command_size = req->size / 4;
      u32 addr = 0;
      u32 be   = 0;
      u32 size = 0;
      u32 seq  = 0;

      u32 i = 0;
      bool burst = false;
      while( i < command_size )
      {
        u32 data;
        if (burst)
        {
          if (seq)
            addr ++;
          data = pCommand[i];
          size --;
          i ++;
          if (size == 0)
          {
            if (i & 1)
              i ++;
            burst = false;
          }
        }
        else
        {
          u32 header = pCommand[i+1];
          data = pCommand[i];
          addr = ( header ) & 0xFFFF;
          be   = ( header >> 16 ) & 0xF;
          size = ( header >> 20 ) & 0xFF;
          seq  = ( header >> 31 );
          i += 2;

          if (size > 0)
            burst = true;
        }

        switch (addr)
        {
        case 0x238://BUFFER_SIZE_CHANNEL0
        case 0x239://BUFFER_SIZE_CHANNEL1
          jumpBufSize[addr-0x238] = data;
          //NN_LOG("%04x %08x\n", addr, data);
          break;
        case 0x23a://BUFFER_ADDR_CHANNEL0
        case 0x23b://BUFFER_ADDR_CHANNEL1
          jumpBufAddr[addr-0x23a] = static_cast<u32*>(LocalFunc::getLogicalAddr(data << 3));
          //NN_LOG("%04x %08x %p\n", addr, data, getLogicalAddr(data << 3));
          break;
        case 0x23c://KICK_CHANNEL0
        case 0x23d://KICK_CHANNEL1
          //NN_LOG("%04x %08x\n", addr, data);
          command_size = jumpBufSize[addr-0x23c]*2;
          pCommand = jumpBufAddr[addr-0x23c];
          i = 0;
          splitData[2] = reinterpret_cast<u32>(pCommand);
          splitData[3] = command_size*4;
          s.Write(splitData, sizeof(splitData), false);
          s.Write(pCommand, command_size*4, false);
          break;
        default:
          //NN_LOG("%04x %08x\n", addr, data);
          break;
        }
      }
    }
  }

  s.Flush();
  s.Finalize();
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
DrawUtil::Debug::RegisterStatus      DrawUtil::Debug::RegisterStatus::s_RegisterStatus;

DrawUtil::Debug::RegisterStatus::RegisterStatus()
{
  clear_();
}

void DrawUtil::Debug::RegisterStatus::clear_()
{
  memset( &m_RegisterState, 0, sizeof(m_RegisterState) );
  
  for( s32 RegNo = 0; RegNo < RegisterMax; ++RegNo )
  {
    switch(RegNo){
    //case PICA_REG_INTERRUPT://! 0x010 です。
    case PICA_REG_CULL_FACE://! 0x040 です。
    case PICA_REG_VIEWPORT_WIDTH1://! 0x041 です。
    case PICA_REG_VIEWPORT_WIDTH2://! 0x042 です。
    case PICA_REG_VIEWPORT_HEIGHT1://! 0x043 です。
    case PICA_REG_VIEWPORT_HEIGHT2://! 0x044 です。
    case PICA_REG_FRAGOP_CLIP://! 0x047 です。
    case PICA_REG_FRAGOP_CLIP_DATA1://! 0x048 です。
    case PICA_REG_FRAGOP_CLIP_DATA2://! 0x049 です。
    case PICA_REG_FRAGOP_CLIP_DATA3://! 0x04a です。
    case PICA_REG_FRAGOP_CLIP_DATA4://! 0x04b です。
    case PICA_REG_FRAGOP_WSCALE_DATA1://! 0x04d です。
    case PICA_REG_FRAGOP_WSCALE_DATA2://! 0x04e です。
    case PICA_REG_VS_OUT_REG_NUM0://! 0x04f です。
    //case PICA_REG_GS_OUT_REG_NUM0://! 0x04f です。
    case PICA_REG_VS_OUT_ATTR0://! 0x050 です。
    //case PICA_REG_GS_OUT_ATTR0://! 0x050 です。
    case PICA_REG_VS_OUT_ATTR1://! 0x051 です。
    //case PICA_REG_GS_OUT_ATTR1://! 0x051 です。
    case PICA_REG_VS_OUT_ATTR2://! 0x052 です。
    //case PICA_REG_GS_OUT_ATTR2://! 0x052 です。
    //case PICA_REG_VS_OUT_ATTR3://! 0x053 です。
    case PICA_REG_GS_OUT_ATTR3://! 0x053 です。
    case PICA_REG_VS_OUT_ATTR4://! 0x054 です。
    //case PICA_REG_GS_OUT_ATTR4://! 0x054 です。
    case PICA_REG_VS_OUT_ATTR5://! 0x055 です。
    //case PICA_REG_GS_OUT_ATTR5://! 0x055 です。
    case PICA_REG_VS_OUT_ATTR6://! 0x056 です。
    //case PICA_REG_GS_OUT_ATTR6://! 0x056 です。
    case PICA_REG_EARLY_DEPTH_FUNC://! 0x061 です。
    case PICA_REG_EARLY_DEPTH_TEST1://! 0x062 です。
    case PICA_REG_EARLY_DEPTH_CLEAR://! 0x063 です。
    case PICA_REG_VS_OUT_ATTR_MODE://! 0x064 です。
    //case PICA_REG_GS_OUT_ATTR_MODE://! 0x064 です。
    case PICA_REG_SCISSOR://! 0x065 です。
    case PICA_REG_SCISSOR_XY://! 0x066 です。
    case PICA_REG_SCISSOR_SIZE://! 0x067 です。
    case PICA_REG_VIEWPORT_XY://! 0x068 です。
    case PICA_REG_EARLY_DEPTH_DATA://! 0x06a です。
    case PICA_REG_FRAGOP_WSCALE://! 0x06d です。
    case PICA_REG_RENDER_BUFFER_RESOLUTION1://! 0x06e です。
    case PICA_REG_VS_OUT_ATTR_CLK://! 0x06f です。
    //case PICA_REG_GS_OUT_ATTR_CLK://! 0x06f です。
    case PICA_REG_TEXTURE_FUNC://! 0x080 です。
    case PICA_REG_TEXTURE0_BORDER_COLOR://! 0x081 です。
    case PICA_REG_TEXTURE0_SIZE://! 0x082 です。
    case PICA_REG_TEXTURE0_WRAP_FILTER://! 0x083 です。
    case PICA_REG_TEXTURE0_LOD://! 0x084 です。    
    case PICA_REG_TEXTURE0_ADDR1://! 0x085 です。
    case PICA_REG_TEXTURE0_ADDR2://! 0x086 です。
    case PICA_REG_TEXTURE0_ADDR3://! 0x087 です。
    case PICA_REG_TEXTURE0_ADDR4://! 0x088 です。    
    case PICA_REG_TEXTURE0_ADDR5://! 0x089 です。
    case PICA_REG_TEXTURE0_ADDR6://! 0x08a です。
    case PICA_REG_TEXTURE0_SHADOW://! 0x08b です。
    case PICA_REG_TEXTURE0_FORMAT://! 0x08e です。
    case PICA_REG_FRAG_LIGHT_EN0://! 0x08f です。
    case PICA_REG_TEXTURE1_BORDER_COLOR://! 0x091 です。
    case PICA_REG_TEXTURE1_SIZE://! 0x092 です。
    case PICA_REG_TEXTURE1_WRAP_FILTER://! 0x093 です。
    case PICA_REG_TEXTURE1_LOD://! 0x094 です。
    case PICA_REG_TEXTURE1_ADDR://! 0x095 です。
    case PICA_REG_TEXTURE1_FORMAT://! 0x096 です。
    case PICA_REG_TEXTURE2_BORDER_COLOR://! 0x099 です。
    case PICA_REG_TEXTURE2_SIZE://! 0x09a です。
    case PICA_REG_TEXTURE2_WRAP_FILTER://! 0x09b です。
    case PICA_REG_TEXTURE2_LOD://! 0x09c です。
    case PICA_REG_TEXTURE2_ADDR://! 0x09d です。
    case PICA_REG_TEXTURE2_FORMAT://! 0x09e です。
    case PICA_REG_TEXTURE3_PROCTEX0://! 0x0a8 です。
    case PICA_REG_TEXTURE3_PROCTEX1://! 0x0a9 です。
    case PICA_REG_TEXTURE3_PROCTEX2://! 0x0aa です。
    case PICA_REG_TEXTURE3_PROCTEX3://! 0x0ab です。
    case PICA_REG_TEXTURE3_PROCTEX4://! 0x0ac です。
    case PICA_REG_TEXTURE3_PROCTEX5://! 0x0ad です。
    case PICA_REG_PROCTEX_LUT://! 0x0af です。
    case PICA_REG_PROCTEX_LUT_DATA0://! 0x0b0 です。
    case PICA_REG_PROCTEX_LUT_DATA1://! 0x0b1 です。
    case PICA_REG_PROCTEX_LUT_DATA2://! 0x0b2 です。
    case PICA_REG_PROCTEX_LUT_DATA3://! 0x0b3 です。
    case PICA_REG_PROCTEX_LUT_DATA4://! 0x0b4 です。
    case PICA_REG_PROCTEX_LUT_DATA5://! 0x0b5 です。
    case PICA_REG_PROCTEX_LUT_DATA6://! 0x0b6 です。
    case PICA_REG_PROCTEX_LUT_DATA7://! 0x0b7 です。
    case PICA_REG_TEX_ENV0://! 0x0c0 です。
    case PICA_REG_TEX_ENV0_OPERAND://! 0x0c1 です。
    case PICA_REG_TEX_ENV0_COMBINE://! 0x0c2 です。
    case PICA_REG_TEX_ENV0_COLOR://! 0x0c3 です。
    case PICA_REG_TEX_ENV0_SCALE://! 0x0c4 です。
    case PICA_REG_TEX_ENV1://! 0x0c8 です。
    case PICA_REG_TEX_ENV1_OPERAND://! 0x0c9 です。
    case PICA_REG_TEX_ENV1_COMBINE://! 0x0ca です。
    case PICA_REG_TEX_ENV1_COLOR://! 0x0cb です。
    case PICA_REG_TEX_ENV1_SCALE://! 0x0cc です。
    case PICA_REG_TEX_ENV2://! 0x0d0 です。
    case PICA_REG_TEX_ENV2_OPERAND://! 0x0d1 です。
    case PICA_REG_TEX_ENV2_COMBINE://! 0x0d2 です。
    case PICA_REG_TEX_ENV2_COLOR://! 0x0d3 です。
    case PICA_REG_TEX_ENV2_SCALE://! 0x0d4 です。
    case PICA_REG_TEX_ENV3://! 0x0d8 です。
    case PICA_REG_TEX_ENV3_OPERAND://! 0x0d9 です。
    case PICA_REG_TEX_ENV3_COMBINE://! 0x0da です。
    case PICA_REG_TEX_ENV3_COLOR://! 0x0db です。
    case PICA_REG_TEX_ENV3_SCALE://! 0x0dc です。
    case PICA_REG_GAS_FOG_MODE://! 0x0e0 です。
    //case PICA_REG_TEX_ENV_BUFFER_INPUT://! 0x0e0 です。
    case PICA_REG_FOG_COLOR://! 0x0e1 です。
    case PICA_REG_GAS_ATTENUATION://! 0x0e4 です。
    case PICA_REG_GAS_ACCMAX://! 0x0e5 です。
    case PICA_REG_FOG_LUT_INDEX://! 0x0e6 です。
    case PICA_REG_FOG_LUT_DATA0://! 0x0e8 です。
    case PICA_REG_FOG_LUT_DATA1://! 0x0e9 です。
    case PICA_REG_FOG_LUT_DATA2://! 0x0ea です。
    case PICA_REG_FOG_LUT_DATA3://! 0x0eb です。
    case PICA_REG_FOG_LUT_DATA4://! 0x0ec です。    
    case PICA_REG_FOG_LUT_DATA5://! 0x0ed です。
    case PICA_REG_FOG_LUT_DATA6://! 0x0ee です。
    case PICA_REG_FOG_LUT_DATA7://! 0x0ef です。
    case PICA_REG_TEX_ENV4://! 0x0f0 です。
    case PICA_REG_TEX_ENV4_OPERAND://! 0x0f1 です。
    case PICA_REG_TEX_ENV4_COMBINE://! 0x0f2 です。
    case PICA_REG_TEX_ENV4_COLOR://! 0x0f3 です。
    case PICA_REG_TEX_ENV4_SCALE://! 0x0f4 です。
    case PICA_REG_TEX_ENV5://! 0x0f8 です。
    case PICA_REG_TEX_ENV5_OPERAND://! 0x0f9 です。
    case PICA_REG_TEX_ENV5_COMBINE://! 0x0fa です。
    case PICA_REG_TEX_ENV5_COLOR://! 0x0fb です。
    case PICA_REG_TEX_ENV5_SCALE://! 0x0fc です。
    case PICA_REG_TEX_ENV_BUFFER_COLOR://! 0x0fd です。
    case PICA_REG_COLOR_OPERATION://! 0x100 です。
    case PICA_REG_BLEND_FUNC://! 0x101 です。
    case PICA_REG_LOGIC_OP://! 0x102 です。
    case PICA_REG_BLEND_COLOR://! 0x103 です。
    case PICA_REG_FRAGOP_ALPHA_TEST://! 0x104 です。
    case PICA_REG_STENCIL_TEST://! 0x105 です。
    case PICA_REG_STENCIL_OP://! 0x106 です。
    case PICA_REG_DEPTH_COLOR_MASK://! 0x107 です。
    case PICA_REG_COLOR_DEPTH_BUFFER_CLEAR0://! 0x110 です。
    case PICA_REG_COLOR_DEPTH_BUFFER_CLEAR1://! 0x111 です。
    case PICA_REG_COLOR_BUFFER_READ://! 0x112 です。
    case PICA_REG_COLOR_BUFFER_WRITE://! 0x113 です。
    case PICA_REG_DEPTH_STENCIL_BUFFER_READ://! 0x114 です。
    case PICA_REG_DEPTH_STENCIL_BUFFER_WRITE://! 0x115 です。
    case PICA_REG_RENDER_BUFFER_DEPTH_MODE://! 0x116 です。
    case PICA_REG_RENDER_BUFFER_COLOR_MODE://! 0x117 です。
    case PICA_REG_EARLY_DEPTH_TEST2://! 0x118 です。
    case PICA_REG_RENDER_BLOCK_FORMAT://! 0x11b です。
    case PICA_REG_RENDER_BUFFER_DEPTH_ADDR://! 0x11c です。
    case PICA_REG_RENDER_BUFFER_COLOR_ADDR://! 0x11d です。
    case PICA_REG_RENDER_BUFFER_RESOLUTION0://! 0x11e です。
    case PICA_REG_GAS_LIGHT_XY://! 0x120 です。
    case PICA_REG_GAS_LIGHT_Z://! 0x121 です。
    case PICA_REG_GAS_LIGHT_Z_COLOR://! 0x122 です。
    case PICA_REG_GAS_LUT_INDEX://! 0x123 です。
    case PICA_REG_GAS_LUT_DATA://! 0x124 です。
    case PICA_REG_GAS_DELTAZ_DEPTH://! 0x126 です。    
    case PICA_REG_FRAGOP_SHADOW://! 0x130 です。
    //case PICA_FRAG_LIGHT_STRIDE://! 0x0010 です。1 個のフラグメントライトのレジスタのストライドです。
    case PICA_REG_FRAG_LIGHT_START://! 0x140 です。
    //case PICA_REG_FRAG_LIGHT0_SPECULAR0://! 0x140 です。
    case PICA_REG_FRAG_LIGHT0_SPECULAR1://! 0x141 です。
    case PICA_REG_FRAG_LIGHT0_DIFFUSE://! 0x142 です。
    case PICA_REG_FRAG_LIGHT0_AMBIENT://! 0x143 です。
    case PICA_REG_FRAG_LIGHT0_POSITION_XY://! 0x144 です。
    case PICA_REG_FRAG_LIGHT0_POSITION_Z://! 0x145 です。
    case PICA_REG_FRAG_LIGHT0_SPOT_XY://! 0x146 です。
    case PICA_REG_FRAG_LIGHT0_SPOT_Z://! 0x147 です。
    case PICA_REG_FRAG_LIGHT0_TYPE://! 0x149 です。
    case PICA_REG_FRAG_LIGHT0_DIST_ATTN_BIAS://! 0x14a です。
    case PICA_REG_FRAG_LIGHT0_DIST_ATTN_SCALE://! 0x14b です。
    case PICA_REG_FRAG_LIGHT1_SPECULAR0://! 0x150 です。
    case PICA_REG_FRAG_LIGHT1_SPECULAR1://! 0x151 です。
    case PICA_REG_FRAG_LIGHT1_DIFFUSE://! 0x152 です。
    case PICA_REG_FRAG_LIGHT1_AMBIENT://! 0x153 です。
    case PICA_REG_FRAG_LIGHT1_POSITION_XY://! 0x154 です。
    case PICA_REG_FRAG_LIGHT1_POSITION_Z://! 0x155 です。
    case PICA_REG_FRAG_LIGHT1_SPOT_XY://! 0x156 です。    
    case PICA_REG_FRAG_LIGHT1_SPOT_Z://! 0x157 です。
    case PICA_REG_FRAG_LIGHT1_TYPE://! 0x159 です。
    case PICA_REG_FRAG_LIGHT1_DIST_ATTN_BIAS://! 0x15a です。
    case PICA_REG_FRAG_LIGHT1_DIST_ATTN_SCALE://! 0x15b です。
    case PICA_REG_FRAG_LIGHT2_SPECULAR0://! 0x160 です。
    case PICA_REG_FRAG_LIGHT2_SPECULAR1://! 0x161 です。    
    case PICA_REG_FRAG_LIGHT2_DIFFUSE://! 0x162 です。
    case PICA_REG_FRAG_LIGHT2_AMBIENT://! 0x163 です。
    case PICA_REG_FRAG_LIGHT2_POSITION_XY://! 0x164 です。
    case PICA_REG_FRAG_LIGHT2_POSITION_Z://! 0x165 です。
    case PICA_REG_FRAG_LIGHT2_SPOT_XY://! 0x166 です。
    case PICA_REG_FRAG_LIGHT2_SPOT_Z://! 0x167 です。
    case PICA_REG_FRAG_LIGHT2_TYPE://! 0x168 です。
    case PICA_REG_FRAG_LIGHT2_DIST_ATTN_BIAS://! 0x16a です。
    case PICA_REG_FRAG_LIGHT2_DIST_ATTN_SCALE://! 0x16b です。
    case PICA_REG_FRAG_LIGHT3_SPECULAR0://! 0x170 です。
    case PICA_REG_FRAG_LIGHT3_SPECULAR1://! 0x171 です。
    case PICA_REG_FRAG_LIGHT3_DIFFUSE://! 0x172 です。
    case PICA_REG_FRAG_LIGHT3_AMBIENT://! 0x173 です。
    case PICA_REG_FRAG_LIGHT3_POSITION_XY://! 0x174 です。
    case PICA_REG_FRAG_LIGHT3_POSITION_Z://! 0x175 です。
    case PICA_REG_FRAG_LIGHT3_SPOT_XY://! 0x176 です。
    case PICA_REG_FRAG_LIGHT3_SPOT_Z://! 0x177 です。
    case PICA_REG_FRAG_LIGHT3_TYPE://! 0x179 です。
    case PICA_REG_FRAG_LIGHT3_DIST_ATTN_BIAS://! 0x17a です。
    case PICA_REG_FRAG_LIGHT3_DIST_ATTN_SCALE://! 0x17b です。
    case PICA_REG_FRAG_LIGHT4_SPECULAR0://! 0x180 です。
    case PICA_REG_FRAG_LIGHT4_SPECULAR1://! 0x181 です。
    case PICA_REG_FRAG_LIGHT4_DIFFUSE://! 0x182 です。
    case PICA_REG_FRAG_LIGHT4_AMBIENT://! 0x183 です。
    case PICA_REG_FRAG_LIGHT4_POSITION_XY://! 0x184 です。
    case PICA_REG_FRAG_LIGHT4_POSITION_Z://! 0x185 です。
    case PICA_REG_FRAG_LIGHT4_SPOT_XY://! 0x186 です。
    case PICA_REG_FRAG_LIGHT4_SPOT_Z://! 0x187 です。
    case PICA_REG_FRAG_LIGHT4_TYPE://! 0x189 です。
    case PICA_REG_FRAG_LIGHT4_DIST_ATTN_BIAS://! 0x18a です。
    case PICA_REG_FRAG_LIGHT4_DIST_ATTN_SCALE://! 0x18b です。
    case PICA_REG_FRAG_LIGHT5_SPECULAR0://! 0x190 です。
    case PICA_REG_FRAG_LIGHT5_SPECULAR1://! 0x191 です。
    case PICA_REG_FRAG_LIGHT5_DIFFUSE://! 0x192 です。
    case PICA_REG_FRAG_LIGHT5_AMBIENT://! 0x193 です。
    case PICA_REG_FRAG_LIGHT5_POSITION_XY://! 0x194 です。
    case PICA_REG_FRAG_LIGHT5_POSITION_Z://! 0x195 です。
    case PICA_REG_FRAG_LIGHT5_SPOT_XY://! 0x196 です。
    case PICA_REG_FRAG_LIGHT5_SPOT_Z://! 0x197 です。
    case PICA_REG_FRAG_LIGHT5_TYPE://! 0x199 です。
    case PICA_REG_FRAG_LIGHT5_DIST_ATTN_BIAS://! 0x19a です。
    case PICA_REG_FRAG_LIGHT5_DIST_ATTN_SCALE://! 0x19b です。
    case PICA_REG_FRAG_LIGHT6_SPECULAR0://! 0x1a0 です。
    case PICA_REG_FRAG_LIGHT6_SPECULAR1://! 0x1a1 です。
    case PICA_REG_FRAG_LIGHT6_DIFFUSE://! 0x1a2 です。
    case PICA_REG_FRAG_LIGHT6_AMBIENT://! 0x1a3 です。
    case PICA_REG_FRAG_LIGHT6_POSITION_XY://! 0x1a4 です。
    case PICA_REG_FRAG_LIGHT6_POSITION_Z://! 0x1a5 です。
    case PICA_REG_FRAG_LIGHT6_SPOT_XY://! 0x1a6 です。
    case PICA_REG_FRAG_LIGHT6_SPOT_Z://! 0x1a7 です。
    case PICA_REG_FRAG_LIGHT6_TYPE://! 0x1a9 です。
    case PICA_REG_FRAG_LIGHT6_DIST_ATTN_BIAS://! 0x1aa です。
    case PICA_REG_FRAG_LIGHT6_DIST_ATTN_SCALE://! 0x1ab です。
    case PICA_REG_FRAG_LIGHT7_SPECULAR0://! 0x1b0 です。
    case PICA_REG_FRAG_LIGHT7_SPECULAR1://! 0x1b1 です。
    case PICA_REG_FRAG_LIGHT7_DIFFUSE://! 0x1b2 です。
    case PICA_REG_FRAG_LIGHT7_AMBIENT://! 0x1b3 です。
    case PICA_REG_FRAG_LIGHT7_POSITION_XY://! 0x1b4 です。
    case PICA_REG_FRAG_LIGHT7_POSITION_Z://! 0x1b5 です。
    case PICA_REG_FRAG_LIGHT7_SPOT_XY://! 0x1b6 です。
    case PICA_REG_FRAG_LIGHT7_SPOT_Z://! 0x1b7 です。
    case PICA_REG_FRAG_LIGHT7_TYPE://! 0x1b9 です。
    case PICA_REG_FRAG_LIGHT7_DIST_ATTN_BIAS://! 0x1ba です。
    case PICA_REG_FRAG_LIGHT7_DIST_ATTN_SCALE://! 0x1bb です。
    case PICA_REG_FRAG_LIGHT_AMBIENT://! 0x1c0 です。
    case PICA_REG_FRAG_LIGHT_SRC_NUM://! 0x1c2 です。
    case PICA_REG_FRAG_LIGHT_FUNC_MODE0://! 0x1c3 です。
    case PICA_REG_FRAG_LIGHT_FUNC_MODE1://! 0x1c4 です。
    case PICA_REG_FRAG_LIGHT_LUT://! 0x1c5 です。
    case PICA_REG_FRAG_LIGHT_EN1://! 0x1c6 です。
    case PICA_REG_FRAG_LIGHT_LUT_DATA0://! 0x1c8 です。    
    case PICA_REG_FRAG_LIGHT_LUT_DATA1://! 0x1c9 です。
    case PICA_REG_FRAG_LIGHT_LUT_DATA2://! 0x1ca です。
    case PICA_REG_FRAG_LIGHT_LUT_DATA3://! 0x1cb です。
    case PICA_REG_FRAG_LIGHT_LUT_DATA4://! 0x1cc です。
    case PICA_REG_FRAG_LIGHT_LUT_DATA5://! 0x1cd です。
    case PICA_REG_FRAG_LIGHT_LUT_DATA6://! 0x1ce です。
    case PICA_REG_FRAG_LIGHT_LUT_DATA7://! 0x1cf です。
    case PICA_REG_FRAG_LIGHT_ABSLUTINPUT://! 0x1d0 です。
    case PICA_REG_FRAG_LIGHT_LUTINPUT://! 0x1d1 です。
    case PICA_REG_FRAG_LIGHT_LUTSCALE://! 0x1d2 です。
    case PICA_REG_FRAG_LIGHT_SRC_EN_ID://! 0x1d9 です。
    case PICA_REG_VERTEX_ATTR_ARRAYS_BASE_ADDR://! 0x200 です。
    case PICA_REG_VERTEX_ATTR_ARRAYS0://! 0x201 です。
    case PICA_REG_VERTEX_ATTR_ARRAYS1://! 0x202 です。
    //case PICA_LOAD_ARRAY_STRIDE://! ロードアレイのストライドです。
    case PICA_REG_LOAD_ARRAY0_ATTR_OFFSET://! 0x203 です。
    case PICA_REG_LOAD_ARRAY0_ELEMENT0://! 0x204 です。
    case PICA_REG_LOAD_ARRAY0_ELEMENT1://! 0x205 です。
    case PICA_REG_INDEX_ARRAY_ADDR_OFFSET://! 0x227 です。
    case PICA_REG_DRAW_VERTEX_NUM://! 0x228 です。
    case PICA_REG_DRAW_MODE0://! 0x229 です。
    case PICA_REG_DRAW_VERTEX_OFFSET://! 0x22a です。    
    case PICA_REG_POST_VERTEX_CACHE_NUM://! 0x22d です。
    //case PICA_REG_START_DRAW_ARRAY://! 0x22e です。
    //case PICA_REG_START_DRAW_ELEMENT://! 0x22f です。
    case PICA_REG_VERTEX_FUNC://! 0x231 です。
    case PICA_REG_VS_FIXED_ATTR://! 0x232 です。
    case PICA_REG_VS_FIXED_ATTR_DATA0://! 0x233 です。
    case PICA_REG_VS_FIXED_ATTR_DATA1://! 0x234 です。
    case PICA_REG_VS_FIXED_ATTR_DATA2://! 0x235 です。
    case PICA_REG_COMMAND_BUF_SIZE_CH0://! 0x238 です。
    case PICA_REG_COMMAND_BUF_SIZE_CH1://! 0x239 です。
    case PICA_REG_COMMAND_BUF_ADDR_CH0://! 0x23a です。
    case PICA_REG_COMMAND_BUF_ADDR_CH1://! 0x23b です。
    //case PICA_REG_COMMAND_BUF_KICK_CH0://! 0x23c です。
    //case PICA_REG_COMMAND_BUF_KICK_CH1://! 0x23d です。
    case PICA_REG_VS_ATTR_NUM1://! 0x242 です。
    case PICA_REG_VS_COM_MODE://! 0x244 です。
    //case PICA_REG_START_DRAW_FUNC0://! 0x245 です。
    case PICA_REG_VS_OUT_REG_NUM1://! 0x24a です。
    case PICA_REG_VS_OUT_REG_NUM2://! 0x251 です。
    case PICA_REG_GS_MISC_REG0://! 0x252 です。
    case PICA_REG_DRAW_MODE1://! 0x253 です。
    case PICA_REG_GS_MISC_REG1://! 0x254 です。
    //case PICA_REG_VS_OUT_REG_NUM3://! 0x25e です。
    case PICA_REG_DRAW_MODE2://! 0x25e です。
    //case PICA_REG_GS_OUT_REG_NUM3://! 0x25e です。
    //case PICA_REG_START_DRAW_FUNC1://! 0x25f です。
    case PICA_REG_GS_BOOL://! 0x280 です。
    case PICA_REG_GS_INT0://! 0x281 です。
    case PICA_REG_GS_INT1://! 0x282 です。
    case PICA_REG_GS_INT2://! 0x283 です。
    case PICA_REG_GS_INT3://! 0x284 です。
    case PICA_REG_GS_ATTR_NUM://! 0x289 です。
    case PICA_REG_GS_START_ADDR://! 0x28a です。
    case PICA_REG_GS_ATTR_IN_REG_MAP0://! 0x28b です。
    case PICA_REG_GS_ATTR_IN_REG_MAP1://! 0x28c です。
    case PICA_REG_GS_OUT_REG_MASK://! 0x28d です。
    case PICA_REG_GS_PROG_UPDATE_END://! 0x28f です。
    case PICA_REG_GS_FLOAT_ADDR://! 0x290 です。
    //case PICA_REG_GS_FLOAT0://! 0x291 です。
    //case PICA_REG_GS_FLOAT1://! 0x292 です。
    //case PICA_REG_GS_FLOAT2://! 0x293 です。
    //case PICA_REG_GS_FLOAT3://! 0x294 です。
    //case PICA_REG_GS_FLOAT4://! 0x295 です。
    //case PICA_REG_GS_FLOAT5://! 0x296 です。
    //case PICA_REG_GS_FLOAT6://! 0x297 です。
    //case PICA_REG_GS_FLOAT7://! 0x298 です。
    case PICA_REG_GS_PROG_ADDR://! 0x29b です。
    case PICA_REG_GS_PROG_DATA0://! 0x29c です。
    case PICA_REG_GS_PROG_DATA1://! 0x29d です。
    case PICA_REG_GS_PROG_DATA2://! 0x29e です。
    case PICA_REG_GS_PROG_DATA3://! 0x29f です。
    case PICA_REG_GS_PROG_DATA4://! 0x2a0 です。
    case PICA_REG_GS_PROG_DATA5://! 0x2a1 です。
    case PICA_REG_GS_PROG_DATA6://! 0x2a2 です。
    case PICA_REG_GS_PROG_DATA7://! 0x2a3 です。
    case PICA_REG_GS_PROG_SWIZZLE_ADDR://! 0x2a5 です。
    case PICA_REG_GS_PROG_SWIZZLE_DATA0://! 0x2a6 です。    
    case PICA_REG_GS_PROG_SWIZZLE_DATA1://! 0x2a7 です。
    case PICA_REG_GS_PROG_SWIZZLE_DATA2://! 0x2a8 です。    
    case PICA_REG_GS_PROG_SWIZZLE_DATA3://! 0x2a9 です。
    case PICA_REG_GS_PROG_SWIZZLE_DATA4://! 0x2aa です。
    case PICA_REG_GS_PROG_SWIZZLE_DATA5://! 0x2ab です。
    case PICA_REG_GS_PROG_SWIZZLE_DATA6://! 0x2ac です。
    case PICA_REG_GS_PROG_SWIZZLE_DATA7://! 0x2ad です。
    case PICA_REG_VS_BOOL://! 0x2b0 です。
    case PICA_REG_VS_INT0://! 0x2b1 です。
    case PICA_REG_VS_INT1://! 0x2b2 です。
    case PICA_REG_VS_INT2://! 0x2b3 です。
    case PICA_REG_VS_INT3://! 0x2b4 です。
    case PICA_REG_VS_ATTR_NUM0://! 0x2b9 です。
    case PICA_REG_VS_START_ADDR://! 0x2ba です。
    case PICA_REG_VS_ATTR_IN_REG_MAP0://! 0x2bb です。
    case PICA_REG_VS_ATTR_IN_REG_MAP1://! 0x2bc です。
    case PICA_REG_VS_OUT_REG_MASK://! 0x2bd です。
    case PICA_REG_VS_PROG_UPDATE_END://! 0x2bf です。
    case PICA_REG_VS_FLOAT_ADDR://! 0x2c0 です。
    case PICA_REG_VS_FLOAT0://! 0x2c1 です。
    case PICA_REG_VS_FLOAT1://! 0x2c2 です。
    case PICA_REG_VS_FLOAT2://! 0x2c3 です。
    case PICA_REG_VS_FLOAT3://! 0x2c4 です。
    case PICA_REG_VS_FLOAT4://! 0x2c5 です。
    case PICA_REG_VS_FLOAT5://! 0x2c6 です。
    case PICA_REG_VS_FLOAT6://! 0x2c7 です。
    case PICA_REG_VS_FLOAT7://! 0x2c8 です。
    case PICA_REG_VS_PROG_ADDR://! 0x2cb です。
    case PICA_REG_VS_PROG_DATA0://! 0x2cc です。
    case PICA_REG_VS_PROG_DATA1://! 0x2cd です。
    case PICA_REG_VS_PROG_DATA2://! 0x2ce です。
    case PICA_REG_VS_PROG_DATA3://! 0x2cf です。
    case PICA_REG_VS_PROG_DATA4://! 0x2d0 です。
    case PICA_REG_VS_PROG_DATA5://! 0x2d1 です。
    case PICA_REG_VS_PROG_DATA6://! 0x2d2 です。
    case PICA_REG_VS_PROG_DATA7://! 0x2d3 です。
    case PICA_REG_VS_PROG_SWIZZLE_ADDR://! 0x2d5 です。
    case PICA_REG_VS_PROG_SWIZZLE_DATA0://! 0x2d6 です。
    case PICA_REG_VS_PROG_SWIZZLE_DATA1://! 0x2d7 です。
    case PICA_REG_VS_PROG_SWIZZLE_DATA2://! 0x2d8 です。
    case PICA_REG_VS_PROG_SWIZZLE_DATA3://! 0x2d9 です。
    case PICA_REG_VS_PROG_SWIZZLE_DATA4://! 0x2da です。
    case PICA_REG_VS_PROG_SWIZZLE_DATA5://! 0x2db です。
    case PICA_REG_VS_PROG_SWIZZLE_DATA6://! 0x2dc です。
    case PICA_REG_VS_PROG_SWIZZLE_DATA7://! 0x2ddです。
      break;
    default:
      continue;
    }
    m_RegisterState.m_fEnable[RegNo] = true;
  }
}

void DrawUtil::Debug::RegisterStatus::Clear()
{
  s_RegisterStatus.clear_();
}

void DrawUtil::Debug::RegisterStatus::Record()
{
  struct LocalFunc{
    static void *getLogicalAddr(u32 addr)
    {
      NN_ASSERTMSG(addr - (0x0c000000+0x14000000) < 0x08000000, "not on device memory");
      return reinterpret_cast<void*>(addr - 0x0c000000);
    }
  };

  struct CommandRequest
  {
    u32 type;
    u32 *buf;
    u32 size;
    u32 padding[4];
  };
  
  u32 splitData[4] = {0xffffffff, 0xffffffff /* addr */ /* size */ };
  u32* jumpBufAddr[2] = { NULL, NULL };
  u32 jumpBufSize[2] = { 0, 0 };
  const int ReqType3D = 1;

  const CommandRequest* reqs;
  nngxGetCmdlistParameteri(NN_GX_CMDLIST_TOP_REQADDR, reinterpret_cast<int*>(&reqs));
  int usedReqCount;
  nngxGetCmdlistParameteri(NN_GX_CMDLIST_USED_REQCOUNT, &usedReqCount);

  for (int reqIndex = 0; reqIndex < usedReqCount; reqIndex ++)
  {
    const CommandRequest* req = &reqs[reqIndex];
    if (req->type == ReqType3D)
    {
      splitData[2] = reinterpret_cast<u32>(req->buf);
      splitData[3] = reinterpret_cast<u32>(req->size);
      
      //s.Write(splitData, sizeof(splitData), false);
      //s.Write(req->buf, req->size, false);
      RegisterStatus::ReadCommand( req->buf, req->size / 4 );
      //NN_LOG("[%d] %p, %x\n", reqIndex, req->buf, req->size);

      const u32* pCommand = req->buf;
      u32 command_size = req->size / 4;
      u32 addr = 0;
      u32 be   = 0;
      u32 size = 0;
      u32 seq  = 0;

      u32 i = 0;
      bool burst = false;
      while( i < command_size )
      {
        u32 data;
        if (burst)
        {
          if (seq)
            addr ++;
          data = pCommand[i];
          size --;
          i ++;
          if (size == 0)
          {
            if (i & 1)
              i ++;
            burst = false;
          }
        }
        else
        {
          u32 header = pCommand[i+1];
          data = pCommand[i];
          addr = ( header ) & 0xFFFF;
          be   = ( header >> 16 ) & 0xF;
          size = ( header >> 20 ) & 0xFF;
          seq  = ( header >> 31 );
          i += 2;

          if (size > 0)
            burst = true;
        }

        switch (addr)
        {
        case 0x238://BUFFER_SIZE_CHANNEL0
        case 0x239://BUFFER_SIZE_CHANNEL1
          jumpBufSize[addr-0x238] = data;
          //NN_LOG("%04x %08x\n", addr, data);
          break;
        case 0x23a://BUFFER_ADDR_CHANNEL0
        case 0x23b://BUFFER_ADDR_CHANNEL1
          jumpBufAddr[addr-0x23a] = static_cast<u32*>(LocalFunc::getLogicalAddr(data << 3));
          //NN_LOG("%04x %08x %p\n", addr, data, getLogicalAddr(data << 3));
          break;
        case 0x23c://KICK_CHANNEL0
        case 0x23d://KICK_CHANNEL1
          //NN_LOG("%04x %08x\n", addr, data);
          command_size = jumpBufSize[addr-0x23c]*2;
          pCommand = jumpBufAddr[addr-0x23c];
          i = 0;
          splitData[2] = reinterpret_cast<u32>(pCommand);
          splitData[3] = command_size*4;         
          RegisterStatus::ReadCommand( pCommand, command_size );
          
          break;
        default:
          //NN_LOG("%04x %08x\n", addr, data);
          break;
        }
      }
    }
  }
}

void DrawUtil::Debug::RegisterStatus::SetCommand()
{  
  ISHIGURO_PRINT("DrawUtil::Debug::RegisterStatus::SetCommand\n");

  {
    static const u32 s_TagCommands[] ={ 0x1496000A, PICA_CMD_HEADER( PICA_REG_VS_ATTR_IN_REG_MAP0, 0, 0, 0 ) };
    nngxAdd3DCommand( s_TagCommands, sizeof(s_TagCommands), GL_TRUE);
  }
  
  s32     writeSize = 0;
  
  for( s32 RegNo = 0; RegNo < RegisterMax; ++RegNo )
  {
    u32     mask = s_RegisterStatus.m_RegisterState.m_AssignedByteFlag[RegNo];
    b32     skip = s_RegisterStatus.m_RegisterState.m_fEnable[RegNo] ? false : true;
    u32     byteEnable = 0;
    
    if ( mask & 0xFF000000 )      byteEnable |= 8;
    if ( mask & 0x00FF0000 )      byteEnable |= 4;
    if ( mask & 0x0000FF00 )      byteEnable |= 2;
    if ( mask & 0x000000FF )      byteEnable |= 1;
    
    //書き込みがない場合はスキップ。固定値などもあるため、不用意に書き込まない。
    if ( mask == 0 )      continue;
    if ( skip )           continue;
      
    u32     command[2] = {
      s_RegisterStatus.m_RegisterState.m_PicaRegister[RegNo],
      PICA_CMD_HEADER( RegNo, 0, byteEnable, 0 )
    };
    
    nngxAdd3DCommand( command, sizeof(command), GL_TRUE);
    writeSize += sizeof(command);
    
    //ISHIGURO_PRINT( "[%04x] %08x, %08x, %08x : %d\n", RegNo, s_RegisterStatus.m_RegisterCommand.m_PicaRegister[RegNo], mask, byteEnable, skip );
  }
  
  if ( writeSize % 16 )
  {//パディング
    static const u32 s_TagCommands[] ={ 0x1496000B, PICA_CMD_HEADER( PICA_REG_VS_ATTR_IN_REG_MAP0, 0, 0, 0 ) };
    nngxAdd3DCommand( s_TagCommands, sizeof(s_TagCommands), GL_TRUE);
  }
  
  {
    static const u32 s_TagCommands[] ={ 0x1496FFFA, PICA_CMD_HEADER( PICA_REG_VS_ATTR_IN_REG_MAP0, 0, 0, 0 ) };
    nngxAdd3DCommand( s_TagCommands, sizeof(s_TagCommands), GL_TRUE);
  }
}

void DrawUtil::Debug::RegisterStatus::Print()
{
  for( s32 RegNo = 0; RegNo < RegisterMax; )
  {
    ISHIGURO_PRINT("%04X ", RegNo );
    
    for( s32 i = 0; i < 8; i++ )
    {
      ISHIGURO_PRINT("%08X", s_RegisterStatus.m_RegisterState.m_PicaRegister[RegNo] );
      if ( i != 7 )
        ISHIGURO_PRINT(" ");
      ++RegNo;
    }
    
    ISHIGURO_PRINT("\n");
  }
}

void DrawUtil::Debug::RegisterStatus::PrintCommand()
{
  ISHIGURO_PRINT("DrawUtil::Debug::RegisterStatus::PrintCommand\n");
  
  s32     writeSize = 0;
  
  for( s32 RegNo = 0; RegNo < RegisterMax; ++RegNo )
  {
    u32     mask = s_RegisterStatus.m_RegisterState.m_AssignedByteFlag[RegNo];
    b32     skip = s_RegisterStatus.m_RegisterState.m_fEnable[RegNo] ? false : true;
    u32     byteEnable = 0;
    
    if ( mask & 0xFF000000 )      byteEnable |= 8;
    if ( mask & 0x00FF0000 )      byteEnable |= 4;
    if ( mask & 0x0000FF00 )      byteEnable |= 2;
    if ( mask & 0x000000FF )      byteEnable |= 1;
    
    //書き込みがない場合はスキップ。固定値などもあるため、不用意に書き込まない。
    if ( mask == 0 )      continue;
    if ( skip )
    {
      ISHIGURO_PRINT( "//" );
    }
      
    u32     command[2] = {
      s_RegisterStatus.m_RegisterState.m_PicaRegister[RegNo],
      PICA_CMD_HEADER( RegNo, 0, byteEnable, 0 )
    };
    
    ISHIGURO_PRINT( "0x%08X, 0x%08X,\n", command[0], command[1] );
    writeSize += sizeof(command);
  }
  
  if ( writeSize % 16 )
  {//パディング
    static const u32 s_TagCommands[] ={ 0x1496000B, PICA_CMD_HEADER( PICA_REG_VS_ATTR_IN_REG_MAP0, 0, 0, 0 ) };
    ISHIGURO_PRINT( "0x%08X, 0x%08X,\n", s_TagCommands[0], s_TagCommands[1] );
  }
}

void DrawUtil::Debug::RegisterStatus::ReadCommand( const u32* pCommand, u32 size )
{
  class MyH3dCommandListener : public g3d::H3dCommandListener
  {
    GFL_FORBID_COPY_AND_ASSIGN(MyH3dCommandListener);
  public:
    MyH3dCommandListener( RegisterState *pRegisterState )
    {
      m_pRegisterState = pRegisterState;
    }
    virtual ~MyH3dCommandListener() {}
  
    virtual bool OnReadData(u32 index, bit32 command, u32 addr, u32 mask, u32 count, u32 count_max, Access access)
    {
      if ( command == 0x1496BBBB )
      {
        DrawUtil::Debug::RegisterStatus::PrintCommand();
        return true;
      }
      
      //command = (command & mask);
      //m_pPicaRegister[addr] |= command;
      
      u32 oldValue = m_pRegisterState->m_PicaRegister[addr];
      m_pRegisterState->m_PicaRegister[addr] = (oldValue & ~mask) | (command & mask);
      m_pRegisterState->m_AssignedByteFlag[addr] |= mask;
      
      if ( command == 0x1496AAAA )
        DrawUtil::Debug::RegisterStatus::Clear();
      
      return false;
    }
    
  private:
    RegisterState     *m_pRegisterState;
  };
  
  MyH3dCommandListener    listener( &s_RegisterStatus.m_RegisterState);

  g3d::H3dCommandUtil::ScanCommand(reinterpret_cast<const bit32*>(pCommand), size, &listener);
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtil::Debug::AddCommand( nn::gr::CTR::CommandBufferJumpHelper *dcs, const bit32* pCommand, s32 size )
{
#if GFL_DEBUG  
  bit32     *p_command = dcs->GetCurrentPtr();
    
  for( s32 i = 0; i < size / sizeof(bit32); ++i )
  {
    *p_command ++ = pCommand[i];
  }
  
  dcs->Put( p_command );
#endif  
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtil::AddCommand( nn::gr::CTR::CommandBufferJumpHelper *dcs, const bit32* pCommand, s32 size )
{
  bit32     *p_command = dcs->GetCurrentPtr();
    
  for( s32 i = 0; i < size / sizeof(bit32); ++i )
  {
    *p_command ++ = pCommand[i];
  }
  
  dcs->Put( p_command );
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtil::writeVertexAttributeCommand( const gfl::grp::util::Vertex *pBuffer, u32 vertex_num )
{
  struct VertexAttributeRegData
  {
    VertexAttributeRegData()  :
      reg_command( 0x802f0233 )
    {

    }

    // 16 Byte
    union{
      u64				command[2];

      struct
      {
        u64             z23_16        :  8;
        u64             w23_0         : 24;
        u64             reg_command   : 32;
        u64             y23_8         : 16;
        u64             z15_0         : 16;
        u64             x23_0         : 24;
        u64             y7_0          :  8;
      };
    };

    void Set( f32 x, f32 y, f32 z, f32 w )
    {
      u32     out;
      UTL_F2F_16M7E( x, out );
      x23_0 = out;
      UTL_F2F_16M7E( y, out );
      y23_8 = (out>>8) & 0xFFFF;
      y7_0 = (out & 0xFF);
      UTL_F2F_16M7E( z, out );
      z23_16 = (out>>16) & 0xFF;
      z15_0 = (out & 0xFFFF);
      UTL_F2F_16M7E( w, out );
      w23_0 = out;
    }
  };

  VertexAttributeRegData      vertexData[4];

  for( s32 i = 0; i < vertex_num; ++i )
  {
    vertexData[0].Set( pBuffer[i].m_Position.x, pBuffer[i].m_Position.y, pBuffer[i].m_Position.z, 1.0f );
    vertexData[1].Set( pBuffer[i].m_Color.x, pBuffer[i].m_Color.y, pBuffer[i].m_Color.z, pBuffer[i].m_Color.w );
    vertexData[2].Set( pBuffer[i].m_Uv.x, pBuffer[i].m_Uv.y, 0.0f, 0.0f );
    vertexData[3].Set( pBuffer[i].m_Normal.x, pBuffer[i].m_Normal.y, pBuffer[i].m_Normal.z, 1.0f );

    nngxAdd3DCommand(vertexData, sizeof(vertexData), GL_TRUE);
  }
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtil::DrawBBoxAA( const gfl::math::AABB& aabb,  gfl::math::VEC4 color )
{
  gfl::grp::util::Vertex      box[4];

  box[0].m_Color = color;
  box[1].m_Color = color;
  box[2].m_Color = color;
  box[3].m_Color = color;

  box[0].m_Uv.Set(  0.0f, 1.0f );//左上ＧＬ座標
  box[1].m_Uv.Set(  1.0f, 1.0f );//右上ＧＬ座標
  box[2].m_Uv.Set(  0.0f, 0.0f );//左下ＧＬ座標
  box[3].m_Uv.Set(  1.0f, 0.0f );//左下ＧＬ座標

  //正面
  box[1].m_Position.Set( aabb.GetMin().x, aabb.GetMax().y, aabb.GetMin().z );
  box[0].m_Position.Set( aabb.GetMax().x, aabb.GetMax().y, aabb.GetMin().z );
  box[3].m_Position.Set( aabb.GetMin().x, aabb.GetMin().y, aabb.GetMin().z );
  box[2].m_Position.Set( aabb.GetMax().x, aabb.GetMin().y, aabb.GetMin().z );
  DrawUtil::DrawVertexStrip( box, 4 );

  //背面
  box[0].m_Position.Set( aabb.GetMax().x, aabb.GetMax().y, aabb.GetMax().z );
  box[1].m_Position.Set( aabb.GetMin().x, aabb.GetMax().y, aabb.GetMax().z );
  box[2].m_Position.Set( aabb.GetMax().x, aabb.GetMin().y, aabb.GetMax().z );
  box[3].m_Position.Set( aabb.GetMin().x, aabb.GetMin().y, aabb.GetMax().z );
  DrawUtil::DrawVertexStrip( box, 4 );

  //右側面
  box[0].m_Position.Set( aabb.GetMax().x, aabb.GetMax().y, aabb.GetMin().z );
  box[1].m_Position.Set( aabb.GetMax().x, aabb.GetMax().y, aabb.GetMax().z );
  box[2].m_Position.Set( aabb.GetMax().x, aabb.GetMin().y, aabb.GetMin().z );
  box[3].m_Position.Set( aabb.GetMax().x, aabb.GetMin().y, aabb.GetMax().z );
  DrawUtil::DrawVertexStrip( box, 4 );

  //左側面
  box[0].m_Position.Set( aabb.GetMin().x, aabb.GetMax().y, aabb.GetMax().z );
  box[1].m_Position.Set( aabb.GetMin().x, aabb.GetMax().y, aabb.GetMin().z );
  box[2].m_Position.Set( aabb.GetMin().x, aabb.GetMin().y, aabb.GetMax().z );
  box[3].m_Position.Set( aabb.GetMin().x, aabb.GetMin().y, aabb.GetMin().z );
  DrawUtil::DrawVertexStrip( box, 4 );

  //上面
  box[0].m_Position.Set( aabb.GetMin().x, aabb.GetMax().y, aabb.GetMax().z );
  box[1].m_Position.Set( aabb.GetMax().x, aabb.GetMax().y, aabb.GetMax().z );
  box[2].m_Position.Set( aabb.GetMin().x, aabb.GetMax().y, aabb.GetMin().z );
  box[3].m_Position.Set( aabb.GetMax().x, aabb.GetMax().y, aabb.GetMin().z );
  DrawUtil::DrawVertexStrip( box, 4 );

  //下面
  box[0].m_Position.Set( aabb.GetMax().x, aabb.GetMin().y, aabb.GetMax().z );
  box[1].m_Position.Set( aabb.GetMin().x, aabb.GetMin().y, aabb.GetMax().z );
  box[2].m_Position.Set( aabb.GetMax().x, aabb.GetMin().y, aabb.GetMin().z );
  box[3].m_Position.Set( aabb.GetMin().x, aabb.GetMin().y, aabb.GetMin().z );
  DrawUtil::DrawVertexStrip( box, 4 );
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtil::DrawLine( const gfl::math::VEC4& src, const gfl::math::VEC4& trg, gfl::math::VEC4 color )
{
  s_Gp->resetRenderState( RenderStateType::MODEL_3D );

  {
    //@note マクロ化しておくときれい。
    static const u32 commandBuffer[] = {
      //入力レジスタのマッピング設定レジスタ（0x02BB, 0x02BC）
      0x00003210, PICA_CMD_HEADER(PICA_REG_VS_ATTR_IN_REG_MAP0, 0, 0x0f, 0 ),
      0x00000000, PICA_CMD_HEADER(PICA_REG_VS_ATTR_IN_REG_MAP1, 0, 0x0f, 0 )
    };
    nngxAdd3DCommand(commandBuffer, sizeof(commandBuffer), GL_TRUE);
  }

  {
    //@note マクロ化しておくときれい。
    static const u32 commandBuffer[] = {
      0x00000000, PICA_CMD_HEADER(PICA_REG_DRAW_MODE2, 0, 0x02, 0 ),//0x025e : DRAW_MODE=0->GL_TRIANGLES
      0x00000001, 0x000f025f,//RESET_VTX_INFO=1->インデックス情報リセット
      0x00000007, 0x00010080,//テクスチャ設定レジスタ samplerType0=1->!false
      0x00000001, PICA_CMD_HEADER(PICA_REG_DRAW_MODE1, 0, 0x01, 0 ),//0x0253 : DRAW_TRIANGLES2=0=>GL_TRIANGLES, CALL_DRAWARRAYS=1
      0x00000000, 0x00010245,//DRAW_START 0
      0x0000000f, 0x000f0232,//頂点属性データの設定
    };
    nngxAdd3DCommand(commandBuffer, sizeof(commandBuffer), GL_TRUE);
  }

  gfl::grp::util::Vertex      line[3];

  line[0].m_Position = src;
  line[1].m_Position = trg;
  line[2].m_Position = trg;
  line[2].m_Position.z += 0.1f;

  line[0].m_Color = color;
  line[1].m_Color = color;
  line[2].m_Color = color;

  line[0].m_Uv.Set(  0.0f, 1.0f );//左上ＧＬ座標
  line[1].m_Uv.Set(  1.0f, 1.0f );//右上ＧＬ座標
  line[2].m_Uv.Set(  0.0f, 0.0f );//左下ＧＬ座標

  s_Gp->writeVertexAttributeCommand( line, 3 );

  {
    //@note マクロ化しておくときれい。
    static const u32 commandBuffer[] = {
      0x00000001, 0x00010245,//DRAW_START 1
      0x00000001, 0x000f0231,//CLEAR_POST_VTX=1 必ず設定する
      0x00000000, 0x00010253,//DRAW_TRIANGLES2=0=>GL_TRIANGLES, CALL_DRAWARRAYS=0
      0x00000001, 0x000f0111,//フレームバッファキャッシュクリア
      0x00000000, 0x00010080,//テクスチャ設定レジスタ samplerType0=0->false
      0x7fff0000, 0x000c02ba,//上の7fffと組み合わせ消費電力軽減のためのコマンド
    };
    nngxAdd3DCommand(commandBuffer, sizeof(commandBuffer), GL_TRUE);
  }
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtil::DrawRect( s32 x, s32 y, s32 w, s32 h, gfl::math::VEC4 color )
{
  static const gfl::math::VEC2   uvSet[4] =
  {
    gfl::math::VEC2(  0.0f, 1.0f ),//左上ＧＬ座標
    gfl::math::VEC2(  1.0f, 1.0f ),//右上ＧＬ座標
    gfl::math::VEC2(  0.0f, 0.0f ),//左下ＧＬ座標
    gfl::math::VEC2(  1.0f, 0.0f )//左下ＧＬ座標
  };

  DrawRect( x, y, w, h, uvSet, color );
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtil::DrawRect( s32 x, s32 y, s32 w, s32 h, const gfl::math::VEC2 *pUvSet, gfl::math::VEC4 color )
{
  gfl::math::MTX44    unit_mat;
  unit_mat.SetupIdentity();

  s_Gp->setShaderParam( s_Gp->m_pImpl->m_ShaderType, unit_mat, unit_mat, NULL );
  s_Gp->resetRenderState( RenderStateType::MODEL_2D );

  {
    //@note マクロ化しておくときれい。
    static const u32 commandBuffer[] = {
      //頂点属性アレイ設定レジスタ（0x0200 ? 0x0227）頂点バッファを使わないので設定不要
      //入力レジスタのマッピング設定レジスタ（0x02BB, 0x02BC）
      0x00003210, PICA_CMD_HEADER(PICA_REG_VS_ATTR_IN_REG_MAP0, 0, 0x0f, 0 ),
      0x00000000, PICA_CMD_HEADER(PICA_REG_VS_ATTR_IN_REG_MAP1, 0, 0x0f, 0 )
    };
    nngxAdd3DCommand(commandBuffer, sizeof(commandBuffer), GL_TRUE);
  }

  {
    //@note マクロ化しておくときれい。
    static const u32 commandBuffer[] = {
      0x00000100, PICA_CMD_HEADER(PICA_REG_DRAW_MODE2, 0, 0x02, 0 ),//0x025e : DRAW_MODE=1->GL_TRIANGLESTRIP
      0x00000001, 0x000f025f,//RESET_VTX_INFO=1->インデックス情報リセット
      0x00000007, 0x00010080,//テクスチャ設定レジスタ samplerType0=1->!false
      0x00000001, PICA_CMD_HEADER(PICA_REG_DRAW_MODE1, 0, 0x01, 0 ),//0x0253 : DRAW_TRIANGLES2=0=>GL_TRIANGLES, CALL_DRAWARRAYS=1
      0x00000000, 0x00010245,//DRAW_START 0
      0x0000000f, 0x000f0232,//頂点属性データの設定
    };
    nngxAdd3DCommand(commandBuffer, sizeof(commandBuffer), GL_TRUE);
  }

  if(!(s_Gp->m_pImpl->m_UseTextureFlag))
  {
    // テクスチャなしのとき
    static const u32 commandBuffer[] = {
      0x00000000, 0x00010080,  // テクスチャユニットを無効にする。  sango_2014/06/19_kawada_koji
      0x00000000, 0x00000000,  // コマンド数調整
    };
    nngxAdd3DCommand(commandBuffer, sizeof(commandBuffer), GL_TRUE);
  }

  gfl::grp::util::Vertex      rect[4];

  w += x;
  h += y;
  
  gfl::grp::RenderTarget  *m_pTargetFrameBuffer = s_Gp->m_pImpl->m_pTargetFrameBuffer;
  f32 left, top, right, bottom;
  s_Gp->m_pImpl->m_pTargetFrameBuffer->GetViewport( &left, &top, &right, &bottom );
  f32   fwidth = right - left;//400
  f32   fheight = bottom - top;//240

  f32 fx = static_cast<f32>(x) / fwidth;
  f32 fy = static_cast<f32>(y) / fheight;
  f32 fw = static_cast<f32>(w) / fwidth;
  f32 fh = static_cast<f32>(h) / fheight;

  fx = -((fx * 2.0f) - 1.0f);
  fy = -((fy * 2.0f) - 1.0f);
  fw = -((fw * 2.0f) - 1.0f);
  fh = -((fh * 2.0f) - 1.0f);

  rect[0].m_Position.Set( fy, fx, 0.0f );
  rect[1].m_Position.Set( fy, fw, 0.0f );
  rect[2].m_Position.Set( fh, fx, 0.0f );
  rect[3].m_Position.Set( fh, fw, 0.0f );

  for( s32 i = 0; i < 4; ++i )
  {
    rect[i].m_Color = color;
    rect[i].m_Uv = pUvSet[i];
  }

  s_Gp->writeVertexAttributeCommand( rect, 4 );

  {
    //@note マクロ化しておくときれい。
    static const u32 commandBuffer[] = {
      0x00000001, 0x00010245,//DRAW_START 1
      0x00000001, 0x000f0231,//CLEAR_POST_VTX=1 必ず設定する
      0x00000000, 0x00010253,//DRAW_TRIANGLES2=0=>GL_TRIANGLES, CALL_DRAWARRAYS=0
      0x00000001, 0x000f0111,//フレームバッファキャッシュクリア
      0x00000000, 0x00010080,//テクスチャ設定レジスタ samplerType0=0->false
      0x7fff0000, 0x000c02ba,//上の7fffと組み合わせ消費電力軽減のためのコマンド
    };
    nngxAdd3DCommand(commandBuffer, sizeof(commandBuffer), GL_TRUE);
  }
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtil::DrawFillScreen( gfl::math::VEC4 color )
{
  gfl::grp::RenderTarget  *m_pTargetFrameBuffer = s_Gp->m_pImpl->m_pTargetFrameBuffer;

  f32 left, top, right, bottom;
  m_pTargetFrameBuffer->GetViewport( &left, &top, &right, &bottom );

  DrawRect( 0, 0, right - left, bottom - top, color );

}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void DrawUtil::DrawFillScreen( const gfl::math::VEC2 *pUvSet, gfl::math::VEC4 color )
{
  gfl::grp::RenderTarget  *m_pTargetFrameBuffer = s_Gp->m_pImpl->m_pTargetFrameBuffer;
  f32 left, top, right, bottom;
  m_pTargetFrameBuffer->GetViewport( &left, &top, &right, &bottom );

  DrawRect( 0, 0, right - left, bottom - top, pUvSet, color );
}


}}}
