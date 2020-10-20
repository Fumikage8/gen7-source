#ifndef __GFL_UTIL_DRAWUTIL_H__
#define __GFL_UTIL_DRAWUTIL_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_UtilDrawUtil.h
 *	@brief  簡易逐次描画クラス
 *	@author	Masateru Ishiguro
 *	@date		2011.2.21
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
// gflib
#include <gfl_Macros.h>
#include <grp/util/gfl_UtilVertexObject.h>
#include <grp/util/gfl_UtilTexture.h>
#include <grp/gfl_GraphicsType.h>
#include <math/gfl_MathAABB.h>


#define DECLARE_ENUM_BEGIN( CLASSNAME )                           \
	class CLASSNAME {                                               \
	public:                                                         \
		enum Enum {
#define DECLARE_ENUM_END( CLASSNAME )                             \
		};                                                            \
	public:                                                         \
		CLASSNAME(){}                                                 \
		explicit CLASSNAME( int value ) : value_( Enum( value ) ) {}	\
		CLASSNAME( Enum value ) : value_( value ) {}                  \
		operator Enum () const { return value_; }                     \
	private:                                                        \
		Enum value_;                                                  \
	};

namespace gfl {
namespace grp {
namespace util {

//! @brief 簡易逐次描画クラス
class DrawUtil
{
  GFL_FORBID_COPY_AND_ASSIGN(DrawUtil);
public:
  static const s32    CharMax = 128;

  //! RenderStateの種類
  DECLARE_ENUM_BEGIN( RenderStateType )
    FONT_2D = 0,    //! フォントor２Ｄ描画
    MODEL_3D,       //! モデルor３Ｄ描画
    MODEL_2D        //! ２Ｄ描画
  DECLARE_ENUM_END( RenderStateType )

  //絶賛公開中
  //! @brief マテリアル情報
	struct MaterialInfo
	{
		MaterialInfo():
			m_ShadingType( COLOR_SHADER ),
			m_BlendEnable( false ),
			m_BlendEquation( GL_FUNC_ADD ),
			m_SrcBlendFunc( GL_SRC_ALPHA ),
			m_DstBlendFunc( GL_ONE_MINUS_SRC_ALPHA ),
			m_DirLightVector( 1.0f, 0.0f, 0.0f ),
			m_DirLightColor( 0.7f, 0.7f, 0.7f ),
			m_AmbLightColor( 0.3f, 0.3f, 0.3f ),
			m_MaterialColor( 1.0f, 1.0f, 1.0f, 1.0f ),
			m_pTexture( NULL ),
			m_UvScroll( 0.0f, 0.0f )
		{

		}

    //! シェーダータイプの種類
    enum MaterialShaderType
    {
      COLOR_SHADER = 0,               //! 頂点カラーシェーダー
      TEXTURE_SHADER,                 //! テクスチャカラーシェーダー
      RGBTEXTURE_SHADER,              //! テクスチャカラーはＲＧＢだけ使用するシェーダー
      TEXTURE_LIGHTING_SHADER,        //! テクスチャカラー x ライティングシェーダー

      //@note 外部から実装できる形にするとよい。
      FRONT_DOFTEXTURE_SHADER,              //!
      BACK_DOFTEXTURE_SHADER,              //!
      DEBUG_SHADER,         	///!
      SHADER_MAX
    };

		u32						m_ShadingType;			//! シェーダータイプ指定
		b8						m_BlendEnable;			//! ブレンディングの有無
		u8						pad[3];
		u32						m_BlendEquation;		//! ブレンディングの計算式指定
		u32						m_SrcBlendFunc;			//! ブレンディングの重み係数指定
		u32						m_DstBlendFunc;			//! ブレンディングの重み係数指定

		gfl::math::VEC3			m_DirLightVector;		//! ディレクショナルライトベクトル指定
		gfl::math::VEC3			m_DirLightColor;		//! ディレクショナルライトカラー指定
		gfl::math::VEC3			m_AmbLightColor;		//! アンビエントライトカラー指定

		gfl::math::VEC4						m_MaterialColor;		//! マテリアルカラー指定（頂点カラーと掛ける）
		gfl::grp::util::Texture		*m_pTexture;        //! テクスチャオブジェクト指定
		gfl::math::VEC2						m_UvScroll;         //! ＵＶスクロール値指定
	};

  /**
   * @brief 初期化関数
   */
  //↓これは廃止します。
	static void Initialize( heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator );
  //↓こちらを使ってください
  static void Initialize( gfl::grp::GraphicsSystem *pSystem, heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator, s32 stackMax = 2 );

  /**
   * 終了関数
   */
	static void Terminate( void );
  
  //-------------------------------------------------------------------------
  /**
   * @brief アロケーターの置き換え
   * @param heapAllocator
   * @param deviceAllocator
   * @return プッシュできたか？AllocatorStackMax回までしかプッシュはできません。
   */
  //-------------------------------------------------------------------------
  static void PushAllocator( heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator );
  
  //-------------------------------------------------------------------------
  /**
   * @brief アロケーターを戻す。
   */
  //-------------------------------------------------------------------------
  static void PopAllocator();
  
  //-------------------------------------------------------------------------
  /**
   * @brief 積まれているアロケーター数の確認
   * @return アロケータ数
   */
  //-------------------------------------------------------------------------
  static s32 GetStackAllocatorCount();

  /**
   * @brief 表示フォントリソース設定。
   * @param pBinary   リソースデータ
   */
  static void SetFontResource( void* pBinary );
  
  /**
   * @brief デフォルトフォントを指定する
   * @param pFont     指定したいフォント
   */
  static void SetFont( nw::font::Font *pFont );

  /**
   * @brief 文字列の幅を取得
   * @param pUtf16   取得対象文字列
   * @return 文字列の表示横幅
   */
  static f32 CalcStringWidth( const wchar_t* pUtf16, nw::font::Font *pFont = NULL );

  /**
   * @brief 描画開始関数
   * @param pFrameBuffer   描画ターゲット
   */
  static void BeginRender( gfl::grp::RenderTarget* pFrameBuffer );

  /**
   * @brief 描画終了関数
   */
  static void EndRender();

  /**
   * @brief テキストのスケール設定
   * @param scale_x   Ｘスケール
   * @param scale_y   Ｙスケール
   */
  static void SetTextScale( f32 scale_x, f32 scale_y );

  /**
   * @brief テクストカラー設定関数
   * @param color   設定したいカラー
   */
  static void SetTextColor( const gfl::math::VEC4 &color );

  /**
   * @brief テキスト描画関数
   * @param posX    表示Ｘ座標位置
   * @param posY    表示Ｙ座標位置
   * @param pSjis   表示テキスト(Utf16フォーマット)
   * @param ...     引数
   */
  static void DrawText( s32 posX, s32 posY, const wchar_t* pUtf16, ... );

  //この関数はいずれ廃止します。
  static void DrawText( s32 posX, s32 posY, const wchar_t* pUtf16, const gfl::math::VEC4 &color, nw::font::Font *pFont = NULL );

  /**
   * @brief テキスト描画関数（グラデーションカラー対応版）
   * @param posX        表示Ｘ座標位置
   * @param posY        表示Ｙ座標位置
   * @param pSjis       表示テキスト(Utf16フォーマット)
   * @param startColor  グラデーション開始カラー
   * @param endColor    グラデーション終了カラー
   * @param gradMode    グラデーションモード
   */
  static void DrawText( s32 posX, s32 posY, const wchar_t* pUtf16, const gfl::math::VEC4 &startColor, const gfl::math::VEC4 &endColor, nw::font::CharWriter::GradationMode gradMode, nw::font::Font *pFont = NULL );

  /**
   * @brief テクスチャ生成関数
   * @param p_data　標準tgaイメージデータのポインタ(24 or 32Bit Targa Format )
   * @return Textureオブジェクトインスタンス
   */
  static Texture* CreateTextureFromMemory( gfl::grp::GraphicsSystem *pSystem, const void* p_data );

  /**
   * @brief テクスチャ生成関数(PICAネイティブテクスチャを生成)
   * @param format  レンダーカラーフォーマット
   * @param width   横幅
   * @param height  高さ
   * @param p_data  PICAネイティブデータ
   * @return Textureオブジェクトインスタンス
   */
  static Texture* CreateTexture( gfl::grp::GraphicsSystem *pSystem, gfl::grp::RenderColorFormat format, u32 width, u32 height, const void *p_data );

  /**
   * @brief キューブテクスチャ生成関数
   * @param p_data　標準tgaイメージデータのポインタ配列６枚分(24 or 32Bit Targa Format )
   * @return Textureオブジェクトインスタンス
   */
  static Texture* CreateCubeTextureFromMemory( gfl::grp::GraphicsSystem *pSystem, const void** p_data );

  /**
   * @brief 頂点配列の生成関数
   * @param vertex_num    確保したい頂点数
   * @return  確保したバッファのポインタ
   */
  static Vertex* CreateVertexArray( u32 vertex_num );
  
  /**
   * @brief 頂点配列の解放関数
   * @param pBuff
   */
  static void DeleteVertexArray( Vertex* pBuff );

  /**
   * @brief マテリアル情報設定関数
   * @param rInfo　マテリアル情報
   */
	static void SetMaterial( const gfl::grp::util::DrawUtil::MaterialInfo &rInfo );

  /**
   * @brief テクスチャ直接設定関数
   * @param textureAddr       イメージデータのアドレス
   * @param textureWidth      イメージ幅
   * @param textureHeight     イメージ高さ
   * @param textureFormat     イメージフォーマット
   */
  static inline void SetTexture( u32 textureNo, u32 textureAddr, u32 textureWidth, u32 textureHeight, u32 textureFormat, f32 offset_u = 0.0f, f32 offset_v = 0.0f );

  /**
   * @brief プロジェクションマトリックス設定
   * @param mat　プロジェクションマトリックス
   */
	static void SetProjectionMatrix( const gfl::math::MTX44 &mat );

  /**
   * @brief ビューマトリックス設定関数
   * @param mat　ビューマトリックス
   */
	static void SetViewMatrix( const gfl::math::MTX44 &mat );

  /**
   * @brief モデルマトリックス設定関数
   * @param mat　モデルマトリックス
   */
	static void SetModelMatrix( const gfl::math::MTX44 &mat );

  /**
   * @brfief レンダーステイト明示的設定
   * @param state　ステートタイプ
   */
  static inline void SetRenderState( RenderStateType::Enum state );

  /**
   * @brief ユーザー頂点リスト描画関数
   * @param pBuffer　ユーザー頂点配列
   * @param vertex_num　頂点数
   */
	static void DrawVertexList( const gfl::grp::util::Vertex *pBuffer, u32 vertex_num, b32 fSetState = true );

  /**
   * @brief ユーザー頂点ストリップ描画関数
   * @param pBuffer　ユーザー頂点配列
   * @param vertex_num　頂点数
   */
	static void DrawVertexStrip( const gfl::grp::util::Vertex *pBuffer, u32 vertex_num, b32 fSetState = true );

  /**
   * @brief AABB描画
   * @param aabb    描画対象AABB
   * @param color   色
   */
  static void DrawBBoxAA( const gfl::math::AABB& aabb,  gfl::math::VEC4 color = gfl::math::VEC4(1.0f,1.0f,1.0f,1.0f) );

  /**
   * @brief ２Ｄ矩形描画
   * @param srcX    描画開始スクリーンＸ座標
   * @param srcY    描画開始スクリーンＸ座標
   * @param trgX    描画幅
   * @param trgY    描画高さ
   * @param color   色
   */
  static void DrawRect( s32 x, s32 y, s32 w, s32 h, gfl::math::VEC4 color = gfl::math::VEC4(1.0f,1.0f,1.0f,1.0f) );

  /**
   * @brief ２Ｄ矩形描画
   * @param srcX    描画開始スクリーンＸ座標
   * @param srcY    描画開始スクリーンＸ座標
   * @param trgX    描画幅
   * @param trgY    描画高さ
   * @param pUvSet  ４頂点分のＵＶ配列ポインタ
   * @param color   色
   */static void DrawRect( s32 x, s32 y, s32 w, s32 h, const gfl::math::VEC2 *pUvSet, gfl::math::VEC4 color = gfl::math::VEC4(1.0f,1.0f,1.0f,1.0f) );


  /**
   * @brief スクリーン全体を描画する
   * @param color   色
   */
  static void DrawFillScreen( gfl::math::VEC4 color = gfl::math::VEC4(1.0f,1.0f,1.0f,1.0f) );

  /**
   * @brief スクリーン全体を描画する
   * @param pUvSet  ４頂点分のＵＶ配列ポインタ
   * @param color   色
   */
  static void DrawFillScreen( const gfl::math::VEC2 *pUvSet, gfl::math::VEC4 color = gfl::math::VEC4(1.0f,1.0f,1.0f,1.0f) );

  /**
   * @brief ＵＶスクロール値設定関数
   * @param scroll_u
   * @param scroll_u
   */
  static void SetUvScroll( u32 textureNo, f32 scroll_u, f32 scroll_v );

  /**
   * @brief シザリング設定を解除
   */
  static void ClearScissor( void );

  /**
   * @brief シザリング領域を設定
   * @param x 左上Ｘ座標
   * @param y 左上Ｙ座標
   * @param w 横幅
   * @param h 縦幅
   */
  static void SetScissor( s32 x, s32 y, s32 w, s32 h );
  
  class Debug{
  public:
    
    static bool G3DH3DMODEL_DEBUG_PRINT;
    /**
    * @brief デバッグ用描画コマンドダンプ関数
    */
    static void CommandLogBegin();

    /**
    * @brief デバッグ用描画コマンドダンプ関数
    */
    static void CommandLogEnd();

    /**
    * @brief デバッグ用コマンドダンプ出力関数
    */
    static void CommandDump();
        
    static void AddCommand( nn::gr::CTR::CommandBufferJumpHelper *dcs, const bit32* pCommand, s32 size );
    
    /**
    * @brief デバッグ用コマンド記録クラス
    */
    class RegisterStatus{
    public:
      
      static void Clear();  
      static void Record();
      static void SetCommand();
      
      static void Print();
      static void PrintCommand();
      
      ~RegisterStatus(){}
      
    private:
      
      static const u32            RegisterMax = 0x300;
      
      struct RegisterState{
        u32                         m_PicaRegister[RegisterMax];
        u32                         m_AssignedByteFlag[RegisterMax];
        b32                         m_fEnable[RegisterMax];
      };
      
      RegisterStatus();
      
      static void ReadCommand( const u32* pCommand, u32 size );
      void clear_();
      
      static RegisterStatus       s_RegisterStatus;
      
      RegisterState             m_RegisterState;
      
      
    };
    
  private:
    Debug(){}
    ~Debug(){}
  };
  
  //デバッグじゃない版
  static void AddCommand( nn::gr::CTR::CommandBufferJumpHelper *dcs, const bit32* pCommand, s32 size );
  
  struct CacheCommand{
    CacheCommand();
    ~CacheCommand();
    
    void Init( heap::NwAllocator* deviceAllocator, s32 allocSize = 1024 );
    bool IsEnable()
    {      
      return m_IsInit;
    }
    
    s32 WriteSize(){ return m_pShaderCacheDcs->GetSize(); }
    s32 WriteSizeWithoutKickCommand(){ return m_BufferSizeWithoutKickCommand; }
    
    nn::gr::CTR::CommandBufferJumpHelper* GetDrawCommandStream(){ return m_pShaderCacheDcs; }
    void SetCH1KickCommand();
    void Reset();
    
    nn::gr::CTR::CommandBufferJumpHelper      *m_pShaderCacheDcs;
    bit32*                          m_pCacheDcsBuff;
    b32                             m_IsInit;
    s32                             m_BufferSize;
    s32                             m_BufferSizeWithoutKickCommand;
  };

private:
  //公開前作成中

	static void DrawVertexList( const gfl::grp::util::VertexObject &rObject );
	static void DrawVertexStrip( const gfl::grp::util::VertexObject &rObject );

  /**
   * @brief ライン描画
   * @param src   ライン始点
   * @param trg   ライン終点
   * @param color ラインの色
   */
  static void DrawLine( const gfl::math::VEC4& src, const gfl::math::VEC4& trg, gfl::math::VEC4 color = gfl::math::VEC4(1.0f,1.0f,1.0f,1.0f) );

  static const s32								CubePageNum = 6;

  DrawUtil(){}
	DrawUtil( heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator, s32 stackMax = 2 );
	~DrawUtil();

  struct cRegMatrixData
  {
    cRegMatrixData()  :
      reg_command( 0x00ff02c1 ),
      pad( 0.0f )
    {

    }

    void Set( const gfl::math::MTX44 &mat )
    {
      col00 = mat[0]; col01 = mat[1]; col02 = mat[2]; col03 = mat[3];
      col10 = mat[4]; col11 = mat[5]; col12 = mat[6]; col13 = mat[7];
      col20 = mat[8]; col21 = mat[9]; col22 = mat[10]; col23 = mat[11];
      col30 = mat[12]; col31 = mat[13]; col32 = mat[14]; col33 = mat[15];
    }

    f32   col03; u32   reg_command;
    f32   col02, col01;
    f32   col00, col13;
    f32   col12, col11;
    f32   col10, col23;
    f32   col22, col21;
    f32   col20, col33;
    f32   col32, col31;
    f32   col30, pad;
  };

  struct cRegVectorData
  {
    cRegVectorData()  :
      reg_command( 0x803F02C1 ),
      pad( 0.0f )
    {

    }

    void Set( const gfl::math::VEC4 &vec )
    {
      col00 = vec.x; col01 = vec.y; col02 = vec.z; col03 = vec.w;
    }

    f32   col03; u32   reg_command;
    f32   col02, col01;
    f32   col00, pad;
  };

	struct Impl;
	static DrawUtil			*s_Gp;
	Impl								*m_pImpl;

  static void convertPicaFormat( u8* p_dst, const u8* p_src, GLenum format, s32 w, s32 h );

  void initDefaultRenderer();
  void initFontRenderer();

	void setShaderParam( s32 type, const gfl::math::MTX44& proj_mat, const gfl::math::MTX44& view_mat, const gfl::grp::util::DrawUtil::MaterialInfo *pInfo );
	void setTexture( const gfl::grp::util::Texture &Texture, GLuint samplingtype = GL_LINEAR );
  void setTexture( u32 textureNo, u32 textureAddr, u32 textureWidth, u32 textureHeight, u32 textureFormat, f32 offset_u = 0.0f, f32 offset_v = 0.0f );
  void resetRenderState( RenderStateType type );
  void setRenderScissorState( nw::font::ColorBufferInfo &colBufInfo );
  void setRenderScissorState( void );
  void writeVertexAttributeCommand( const gfl::grp::util::Vertex *pBuffer, u32 vertex_num );
  void setupTextCamera();
  void getColorBuffer( nw::font::ColorBufferInfo &colBufInfo );
};


inline void DrawUtil::SetTexture( u32 textureNo, u32 textureAddr, u32 textureWidth, u32 textureHeight, u32 textureFormat, f32 offset_u, f32 offset_v )
{
  s_Gp->setTexture( textureNo, textureAddr, textureWidth, textureHeight, textureFormat, offset_u, offset_v );
}

inline void DrawUtil::SetRenderState( RenderStateType::Enum state )
{
  s_Gp->resetRenderState( state );
}

#undef DECLARE_ENUM_BEGIN
#undef DECLARE_ENUM_END

}  // namespace util
}  // namespace grp
}  // namespace gfl

#endif // __GFL_UTIL_DRAWUTIL_H__
