#ifndef __GFL_UTIL_DRAWUTIL_H__
#define __GFL_UTIL_DRAWUTIL_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_UtilDrawUtil.h
 *	@brief  �ȈՒ����`��N���X
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

//! @brief �ȈՒ����`��N���X
class DrawUtil
{
  GFL_FORBID_COPY_AND_ASSIGN(DrawUtil);
public:
  static const s32    CharMax = 128;

  //! RenderState�̎��
  DECLARE_ENUM_BEGIN( RenderStateType )
    FONT_2D = 0,    //! �t�H���gor�Q�c�`��
    MODEL_3D,       //! ���f��or�R�c�`��
    MODEL_2D        //! �Q�c�`��
  DECLARE_ENUM_END( RenderStateType )

  //��^���J��
  //! @brief �}�e���A�����
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

    //! �V�F�[�_�[�^�C�v�̎��
    enum MaterialShaderType
    {
      COLOR_SHADER = 0,               //! ���_�J���[�V�F�[�_�[
      TEXTURE_SHADER,                 //! �e�N�X�`���J���[�V�F�[�_�[
      RGBTEXTURE_SHADER,              //! �e�N�X�`���J���[�͂q�f�a�����g�p����V�F�[�_�[
      TEXTURE_LIGHTING_SHADER,        //! �e�N�X�`���J���[ x ���C�e�B���O�V�F�[�_�[

      //@note �O����������ł���`�ɂ���Ƃ悢�B
      FRONT_DOFTEXTURE_SHADER,              //!
      BACK_DOFTEXTURE_SHADER,              //!
      DEBUG_SHADER,         	///!
      SHADER_MAX
    };

		u32						m_ShadingType;			//! �V�F�[�_�[�^�C�v�w��
		b8						m_BlendEnable;			//! �u�����f�B���O�̗L��
		u8						pad[3];
		u32						m_BlendEquation;		//! �u�����f�B���O�̌v�Z���w��
		u32						m_SrcBlendFunc;			//! �u�����f�B���O�̏d�݌W���w��
		u32						m_DstBlendFunc;			//! �u�����f�B���O�̏d�݌W���w��

		gfl::math::VEC3			m_DirLightVector;		//! �f�B���N�V���i�����C�g�x�N�g���w��
		gfl::math::VEC3			m_DirLightColor;		//! �f�B���N�V���i�����C�g�J���[�w��
		gfl::math::VEC3			m_AmbLightColor;		//! �A���r�G���g���C�g�J���[�w��

		gfl::math::VEC4						m_MaterialColor;		//! �}�e���A���J���[�w��i���_�J���[�Ɗ|����j
		gfl::grp::util::Texture		*m_pTexture;        //! �e�N�X�`���I�u�W�F�N�g�w��
		gfl::math::VEC2						m_UvScroll;         //! �t�u�X�N���[���l�w��
	};

  /**
   * @brief �������֐�
   */
  //������͔p�~���܂��B
	static void Initialize( heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator );
  //����������g���Ă�������
  static void Initialize( gfl::grp::GraphicsSystem *pSystem, heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator, s32 stackMax = 2 );

  /**
   * �I���֐�
   */
	static void Terminate( void );
  
  //-------------------------------------------------------------------------
  /**
   * @brief �A���P�[�^�[�̒u������
   * @param heapAllocator
   * @param deviceAllocator
   * @return �v�b�V���ł������HAllocatorStackMax��܂ł����v�b�V���͂ł��܂���B
   */
  //-------------------------------------------------------------------------
  static void PushAllocator( heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator );
  
  //-------------------------------------------------------------------------
  /**
   * @brief �A���P�[�^�[��߂��B
   */
  //-------------------------------------------------------------------------
  static void PopAllocator();
  
  //-------------------------------------------------------------------------
  /**
   * @brief �ς܂�Ă���A���P�[�^�[���̊m�F
   * @return �A���P�[�^��
   */
  //-------------------------------------------------------------------------
  static s32 GetStackAllocatorCount();

  /**
   * @brief �\���t�H���g���\�[�X�ݒ�B
   * @param pBinary   ���\�[�X�f�[�^
   */
  static void SetFontResource( void* pBinary );
  
  /**
   * @brief �f�t�H���g�t�H���g���w�肷��
   * @param pFont     �w�肵�����t�H���g
   */
  static void SetFont( nw::font::Font *pFont );

  /**
   * @brief ������̕����擾
   * @param pUtf16   �擾�Ώە�����
   * @return ������̕\������
   */
  static f32 CalcStringWidth( const wchar_t* pUtf16, nw::font::Font *pFont = NULL );

  /**
   * @brief �`��J�n�֐�
   * @param pFrameBuffer   �`��^�[�Q�b�g
   */
  static void BeginRender( gfl::grp::RenderTarget* pFrameBuffer );

  /**
   * @brief �`��I���֐�
   */
  static void EndRender();

  /**
   * @brief �e�L�X�g�̃X�P�[���ݒ�
   * @param scale_x   �w�X�P�[��
   * @param scale_y   �x�X�P�[��
   */
  static void SetTextScale( f32 scale_x, f32 scale_y );

  /**
   * @brief �e�N�X�g�J���[�ݒ�֐�
   * @param color   �ݒ肵�����J���[
   */
  static void SetTextColor( const gfl::math::VEC4 &color );

  /**
   * @brief �e�L�X�g�`��֐�
   * @param posX    �\���w���W�ʒu
   * @param posY    �\���x���W�ʒu
   * @param pSjis   �\���e�L�X�g(Utf16�t�H�[�}�b�g)
   * @param ...     ����
   */
  static void DrawText( s32 posX, s32 posY, const wchar_t* pUtf16, ... );

  //���̊֐��͂�����p�~���܂��B
  static void DrawText( s32 posX, s32 posY, const wchar_t* pUtf16, const gfl::math::VEC4 &color, nw::font::Font *pFont = NULL );

  /**
   * @brief �e�L�X�g�`��֐��i�O���f�[�V�����J���[�Ή��Łj
   * @param posX        �\���w���W�ʒu
   * @param posY        �\���x���W�ʒu
   * @param pSjis       �\���e�L�X�g(Utf16�t�H�[�}�b�g)
   * @param startColor  �O���f�[�V�����J�n�J���[
   * @param endColor    �O���f�[�V�����I���J���[
   * @param gradMode    �O���f�[�V�������[�h
   */
  static void DrawText( s32 posX, s32 posY, const wchar_t* pUtf16, const gfl::math::VEC4 &startColor, const gfl::math::VEC4 &endColor, nw::font::CharWriter::GradationMode gradMode, nw::font::Font *pFont = NULL );

  /**
   * @brief �e�N�X�`�������֐�
   * @param p_data�@�W��tga�C���[�W�f�[�^�̃|�C���^(24 or 32Bit Targa Format )
   * @return Texture�I�u�W�F�N�g�C���X�^���X
   */
  static Texture* CreateTextureFromMemory( gfl::grp::GraphicsSystem *pSystem, const void* p_data );

  /**
   * @brief �e�N�X�`�������֐�(PICA�l�C�e�B�u�e�N�X�`���𐶐�)
   * @param format  �����_�[�J���[�t�H�[�}�b�g
   * @param width   ����
   * @param height  ����
   * @param p_data  PICA�l�C�e�B�u�f�[�^
   * @return Texture�I�u�W�F�N�g�C���X�^���X
   */
  static Texture* CreateTexture( gfl::grp::GraphicsSystem *pSystem, gfl::grp::RenderColorFormat format, u32 width, u32 height, const void *p_data );

  /**
   * @brief �L���[�u�e�N�X�`�������֐�
   * @param p_data�@�W��tga�C���[�W�f�[�^�̃|�C���^�z��U����(24 or 32Bit Targa Format )
   * @return Texture�I�u�W�F�N�g�C���X�^���X
   */
  static Texture* CreateCubeTextureFromMemory( gfl::grp::GraphicsSystem *pSystem, const void** p_data );

  /**
   * @brief ���_�z��̐����֐�
   * @param vertex_num    �m�ۂ��������_��
   * @return  �m�ۂ����o�b�t�@�̃|�C���^
   */
  static Vertex* CreateVertexArray( u32 vertex_num );
  
  /**
   * @brief ���_�z��̉���֐�
   * @param pBuff
   */
  static void DeleteVertexArray( Vertex* pBuff );

  /**
   * @brief �}�e���A�����ݒ�֐�
   * @param rInfo�@�}�e���A�����
   */
	static void SetMaterial( const gfl::grp::util::DrawUtil::MaterialInfo &rInfo );

  /**
   * @brief �e�N�X�`�����ڐݒ�֐�
   * @param textureAddr       �C���[�W�f�[�^�̃A�h���X
   * @param textureWidth      �C���[�W��
   * @param textureHeight     �C���[�W����
   * @param textureFormat     �C���[�W�t�H�[�}�b�g
   */
  static inline void SetTexture( u32 textureNo, u32 textureAddr, u32 textureWidth, u32 textureHeight, u32 textureFormat, f32 offset_u = 0.0f, f32 offset_v = 0.0f );

  /**
   * @brief �v���W�F�N�V�����}�g���b�N�X�ݒ�
   * @param mat�@�v���W�F�N�V�����}�g���b�N�X
   */
	static void SetProjectionMatrix( const gfl::math::MTX44 &mat );

  /**
   * @brief �r���[�}�g���b�N�X�ݒ�֐�
   * @param mat�@�r���[�}�g���b�N�X
   */
	static void SetViewMatrix( const gfl::math::MTX44 &mat );

  /**
   * @brief ���f���}�g���b�N�X�ݒ�֐�
   * @param mat�@���f���}�g���b�N�X
   */
	static void SetModelMatrix( const gfl::math::MTX44 &mat );

  /**
   * @brfief �����_�[�X�e�C�g�����I�ݒ�
   * @param state�@�X�e�[�g�^�C�v
   */
  static inline void SetRenderState( RenderStateType::Enum state );

  /**
   * @brief ���[�U�[���_���X�g�`��֐�
   * @param pBuffer�@���[�U�[���_�z��
   * @param vertex_num�@���_��
   */
	static void DrawVertexList( const gfl::grp::util::Vertex *pBuffer, u32 vertex_num, b32 fSetState = true );

  /**
   * @brief ���[�U�[���_�X�g���b�v�`��֐�
   * @param pBuffer�@���[�U�[���_�z��
   * @param vertex_num�@���_��
   */
	static void DrawVertexStrip( const gfl::grp::util::Vertex *pBuffer, u32 vertex_num, b32 fSetState = true );

  /**
   * @brief AABB�`��
   * @param aabb    �`��Ώ�AABB
   * @param color   �F
   */
  static void DrawBBoxAA( const gfl::math::AABB& aabb,  gfl::math::VEC4 color = gfl::math::VEC4(1.0f,1.0f,1.0f,1.0f) );

  /**
   * @brief �Q�c��`�`��
   * @param srcX    �`��J�n�X�N���[���w���W
   * @param srcY    �`��J�n�X�N���[���w���W
   * @param trgX    �`�敝
   * @param trgY    �`�捂��
   * @param color   �F
   */
  static void DrawRect( s32 x, s32 y, s32 w, s32 h, gfl::math::VEC4 color = gfl::math::VEC4(1.0f,1.0f,1.0f,1.0f) );

  /**
   * @brief �Q�c��`�`��
   * @param srcX    �`��J�n�X�N���[���w���W
   * @param srcY    �`��J�n�X�N���[���w���W
   * @param trgX    �`�敝
   * @param trgY    �`�捂��
   * @param pUvSet  �S���_���̂t�u�z��|�C���^
   * @param color   �F
   */static void DrawRect( s32 x, s32 y, s32 w, s32 h, const gfl::math::VEC2 *pUvSet, gfl::math::VEC4 color = gfl::math::VEC4(1.0f,1.0f,1.0f,1.0f) );


  /**
   * @brief �X�N���[���S�̂�`�悷��
   * @param color   �F
   */
  static void DrawFillScreen( gfl::math::VEC4 color = gfl::math::VEC4(1.0f,1.0f,1.0f,1.0f) );

  /**
   * @brief �X�N���[���S�̂�`�悷��
   * @param pUvSet  �S���_���̂t�u�z��|�C���^
   * @param color   �F
   */
  static void DrawFillScreen( const gfl::math::VEC2 *pUvSet, gfl::math::VEC4 color = gfl::math::VEC4(1.0f,1.0f,1.0f,1.0f) );

  /**
   * @brief �t�u�X�N���[���l�ݒ�֐�
   * @param scroll_u
   * @param scroll_u
   */
  static void SetUvScroll( u32 textureNo, f32 scroll_u, f32 scroll_v );

  /**
   * @brief �V�U�����O�ݒ������
   */
  static void ClearScissor( void );

  /**
   * @brief �V�U�����O�̈��ݒ�
   * @param x ����w���W
   * @param y ����x���W
   * @param w ����
   * @param h �c��
   */
  static void SetScissor( s32 x, s32 y, s32 w, s32 h );
  
  class Debug{
  public:
    
    static bool G3DH3DMODEL_DEBUG_PRINT;
    /**
    * @brief �f�o�b�O�p�`��R�}���h�_���v�֐�
    */
    static void CommandLogBegin();

    /**
    * @brief �f�o�b�O�p�`��R�}���h�_���v�֐�
    */
    static void CommandLogEnd();

    /**
    * @brief �f�o�b�O�p�R�}���h�_���v�o�͊֐�
    */
    static void CommandDump();
        
    static void AddCommand( nn::gr::CTR::CommandBufferJumpHelper *dcs, const bit32* pCommand, s32 size );
    
    /**
    * @brief �f�o�b�O�p�R�}���h�L�^�N���X
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
  
  //�f�o�b�O����Ȃ���
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
  //���J�O�쐬��

	static void DrawVertexList( const gfl::grp::util::VertexObject &rObject );
	static void DrawVertexStrip( const gfl::grp::util::VertexObject &rObject );

  /**
   * @brief ���C���`��
   * @param src   ���C���n�_
   * @param trg   ���C���I�_
   * @param color ���C���̐F
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
