#ifndef GFL_MAYA_GLINTERFACE_TYPE_H_
#define GFL_MAYA_GLINTERFACE_TYPE_H_

#include <GL/glew.h> // glew

#define PLATFORM_WIN

#define DECLARE_ENUM_BEGIN( CLASSNAME )									\
	class CLASSNAME {													\
	public:																\
		enum Enum {
#define DECLARE_ENUM_END( CLASSNAME )									\
		};																\
	};


typedef	unsigned char		UChar;
typedef	unsigned short		UShort;
typedef	unsigned int		UInt;
typedef	unsigned long		ULong;
typedef	char				Int8, int8;
typedef	unsigned char		UInt8, uint8;
typedef	short				Int16, int16;
typedef	unsigned short		UInt16, uint16;
typedef	int					Int32, int32;
typedef	unsigned int		UInt32, uint32;
typedef	float				Float32;
typedef unsigned long long int ulong128;

typedef unsigned long long int ulong128;
typedef	long long			Int64, int64;
typedef	unsigned long long	UInt64, uint64;

#define SAFE_DELETE( p )       { if( p ) { delete ( p );		(p)=0; } }
#define SAFE_DELETE_ARRAY( p ) { if( p ) { delete[] ( p );		(p)=0; } }

namespace gfl { namespace maya { namespace GLInterface {

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! �X�g���[����Usage
	DECLARE_ENUM_BEGIN( StreamUsage )

		Source0 = 0,
		Source1,
		Source2,
		Source3,
		Source4,
		Source5,
		Source6,
		Source7,
		Source8,
    Source9,
    Source10,
		Source11,
		Source12,
		NumberOf,

		//  alias
		Position			  = Source0,
		Color					  = Source1,
		TexCoord0			  = Source2,
    TexCoord1			  = Source3,
    TexCoord2			  = Source4,
		Normal			    = Source5,
		Tangent0			  = Source6,
		Tangent1			  = Source7,
		Tangent2			  = Source8,
		JointIndex			= Source9,
		JointWeight			= Source10,
		OrigNormal      = Source11,
		OrigTangent     = Source12,

	DECLARE_ENUM_END( StreamUsage )

	//-------------------------------------------------------------------------------------------------------------------------------------------------
    //!  ���W�̃f�[�^�^
	DECLARE_ENUM_BEGIN( VertexType )
		Float	= GL_FLOAT,			//! 32bit���������l
		Short	= GL_SHORT,			//! 16bit�����t����
		UByte	= GL_UNSIGNED_BYTE,	//! 8bit�����Ȃ������l
		Byte	= GL_BYTE,			//! 8bit�����t�����l
		NumberOf = 4

	DECLARE_ENUM_END( VertexType )

	//-----------------------------------------------------------------------------
	//! �e�L�X�g�I�u�W�F�N�g�����^�C�v
	DECLARE_ENUM_BEGIN( ShaderType )
		Vertex = 0,
		Pixel,
		Geometry,
		NumberOf
	DECLARE_ENUM_END( ShaderType )

	//-----------------------------------------------------------------------------
	//! �������z�u�t���O
    DECLARE_ENUM_BEGIN( Pool )
        Default = 0,    //! �œK�ȃ�����
        System,         //! ���C��������
        Managed,        //! �K�v�ɉ����ă������z�u��ύX���܂�
        NumberOf
    DECLARE_ENUM_END( Pool )
	
	//-----------------------------------------------------------------------------
    //! �C���f�N�X�̃f�[�^�^
    DECLARE_ENUM_BEGIN( IndexType )
		Index8	= GL_UNSIGNED_BYTE,		//! 8bit�����Ȃ������l
		Index16	= GL_UNSIGNED_SHORT,	//! 16bit�����Ȃ������l
        NumberOf = 2
    DECLARE_ENUM_END( IndexType )

	//-----------------------------------------------------------------------------
    //! �g�p�p�r�t���O
    DECLARE_ENUM_BEGIN( Usage )

        None = 0,

        //  �T�[�t�F�C�X�ƃe�N�X�`���ɗL��
        RenderTarget    = (1 << 0),     //! �����_�����O�ΏۂɎg��
        DepthStencil    = (1 << 1),     //! �[�x�X�e���V���o�b�t�@�Ɏg��
        Swizzled        = (1 << 2),     //! @brief �e�N�X�`�����X�E�B�Y���t�H�[�}�b�g�Ƃ��č쐬�����\n
                                        //! �e�N�X�`���t�F�b�`�͂�⍂�������A�����_�����O�ΏۂƂ��Ă͂��ᑬ\n
                                        //! ���̃t�H�[�}�b�g���w�肷��ꍇ�A�𑜓x�̕��ƍ��������ꂼ��Q�ׂ̂���ł���K�v������B
        Tiled           = (1 << 3),     //! @brief �e�N�X�`�����^�C���h�t�H�[�}�b�g�Ƃ��č쐬�����\n
                                        //! �e�N�X�`���t�F�b�`�͒����ŁA�����_�����O�ΏۂƂ��Ă͍ō���\n
                                        //! ���̃t�H�[�}�b�g���w�肷��ꍇ�A�L���[�u�e�N�X�`���̓~�b�v�}�b�v�֎~�ł��A�𑜓x�̕��ƍ��������ꂼ��P�U�̔{���ł���K�v������B
        ZCull           = (1 << 4),     //! @brief ZCull�̈��ݒ肷��(PS3��p)\n
                                        //! ���̃t���O���w�肷��ꍇ�A�T�[�t�F�C�X�͕K�����[�J���������łȂ��Ă͂����Ȃ��B

        //  �e�N�X�`���݂̂ɗL��
        AutoGenMipmap   = (1 << 8),     //! �~�b�v�}�b�v��������������

        //  �e�N�X�`���ƃo�b�t�@�����ɗL��
        Dynamic         = (1 << 16),     //! @brief ���_�o�b�t�@�̃������z�u�𓮓I�ɍs�����Ƃ������܂�\n
                                        //! ���̃t���O���w�肵�Ȃ��ƃ������z�u�͐ÓI�ɍs���܂�\n
                                        //! Usage::Dynamic �� Pool::Managed �͓����ɐݒ�ł��܂���B\n
                                        //! ���t���[��(�������͂���ȏ�̕p�x��)���������悤�ȏꍇ�ɓK���Ă��܂��B
        WriteOnly       = (1 << 17)     //! @brief ���_�o�b�t�@�ւ̏������ݑ��삵�����s���Ȃ����Ƃ��V�X�e���ɒm�点�܂�\n
                                        //! �܂胍�b�N���Ƀo�b�t�@�̓ǂݎ��͂ł��܂���

    DECLARE_ENUM_END( Usage )

	//-----------------------------------------------------------------------------
    //! ���b�N�t���O
    DECLARE_ENUM_BEGIN( Lock )

        Discard       = (1 << 0),   //! @brief �ȑO�̃o�b�t�@�̓��e��j�����ă��b�N���܂��B���̂��߃������]������~���邱�Ƃ͂���܂���B\n
                                    //! ��������V���ɗp�ӂ���̂Ńo�b�t�@�̕����X�V�͂ł��܂���\n
                                    //! Usage::Dynamic ���w�肳�ꂽ�o�b�t�@�ɂ����K�p�ł��܂���B
        DoNotWait     = (1 << 1),   //! �������]�����⃌���_�����O���Ȃǃ������ɃA�N�Z�X�ł��Ȃ��ꍇ�A���b�N�\�ɂȂ�܂ő҂̂ł͂Ȃ��A�����ɃG���[�R�[�h��Ԃ��܂�
        NoOverWrite   = (1 << 2),   //! �A�v���P�[�V�������������]���⃌���_�����O�Ɏg�p���̃o�b�t�@���㏑�����Ȃ����Ƃ𖾎����܂��B\n
                                    //! Usage::Dynamic ���w�肳�ꂽ�o�b�t�@�ɂ����K�p�ł��܂���B
        ReadOnly      = (1 << 3)    //! @brief �A�v���P�[�V�������o�b�t�@�ɏ������܂Ȃ����Ƃ𖾎����܂��B\n
                                    //! ���b�N��̏���(�s�N�Z���t�H�[�}�b�g�ϊ��Ȃ�)���ȗ��ł��܂��B

    DECLARE_ENUM_END( Lock )

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief �v���~�e�B�u�̎��
	DECLARE_ENUM_BEGIN( PrimitiveType )

		Triangles		= GL_TRIANGLES,			//! �g���C�A���O��
		TriangleStrip	= GL_TRIANGLE_STRIP,	//! �g���C�A���O���X�g���b�v
		TriangleFan		= GL_TRIANGLE_FAN,		//! �g���C�A���O���t�@��

		NumberOf = 3

	DECLARE_ENUM_END( PrimitiveType )

	//-------------------------------------------------------------------------------------------------------------------------------------------------
	//! @brief �e�N�X�`���^�C�v
	DECLARE_ENUM_BEGIN( TextureType )

		Image = GL_TEXTURE_2D,
		Cube = GL_TEXTURE_CUBE_MAP,

		NumberOf

	DECLARE_ENUM_END( TextureType )

//-------------------------------------------------------------------------------------------------------------------------------------------------

	//! @brief �T�[�t�F�C�X�t�H�[�}�b�g
	DECLARE_ENUM_BEGIN( Format )

		A8R8G8B8	= GL_RGBA,
		R8G8B8		= GL_RGB,
		NumberOf

	DECLARE_ENUM_END( Format )

	//-----------------------------------------------------------------------------
	//! �V�F�[�_�[�n���h��
	typedef GLuint					ShaderObjectHandle;
	typedef GLuint					ShaderProgramHandle;
	typedef unsigned int			ShaderParameterHandle;
	typedef GLuint					BufferObjectHandle;
	typedef GLuint					TextureHandle;
	//typedef CGparameter			ShaderParameterHandle;
	//typedef CGcontext				ShaderObjectHandle;
	//typedef CGprogram				ShaderProgramHandle;

	//-----------------------------------------------------------------------------
	/**
	 * @struct	VertexBufferDesc
	 * @brief	���_�o�b�t�@�̏ڍ׏��
	 */
    struct VertexBufferDesc
    {
        UInt32		usage;		//! �g�p�p�r�t���O
        UInt32		size;		//! �o�b�t�@�T�C�Y
		Pool::Enum	pool;		//! �������z�u
		UInt8		pad[3];
    };

	//-----------------------------------------------------------------------------
	/**
	 * @struct	IndexBufferDesc
	 * @brief	�C���f�N�X�o�b�t�@�̏ڍ׏��
	 */
    struct IndexBufferDesc
    {
        UInt32				usage;	//! �g�p�p�r�t���O
        UInt32				size;	//! �o�b�t�@�T�C�Y
		IndexType::Enum		type;	//! �^�C�v
		Pool::Enum			pool;	//! �������z�u
		UInt8				pad;
    };

	//-----------------------------------------------------------------------------
	/**
	 * @struct	VertexElement
	 * @brief	�p�C�v���C���ɑ�������͒��_�̒�`�^
	 */
	struct VertexElement {
		static const UInt8			StreamEnd = 0xFF;

		UInt8				Stream;		// �X�g���[���ԍ�
		UInt8				Offset;		// �I�t�Z�b�g
		UInt16				VertexType;	// �f�[�^�^
		UInt8				Count;		// 1���_�̗v�f��
		UInt8				Size;		// 1���_�̃T�C�Y
		UInt8				Stride;		// 1���_�̃X�g���C�h�T�C�Y
		UInt8				StreamUsage;// Usage
	};

	//-----------------------------------------------------------------------------
	/**
	 * @struct	TextureDesc
	 * @brief	�e�N�X�`���̏ڍ׏��
	 */
	struct TextureDesc
	{
		UInt32		usage;          //! �g�p�p�r�t���O
		UInt16		width;          //! ��
		UInt16		height;         //! ����
		UInt16		format;         //! �s�N�Z���t�H�[�}�b�g
		UInt16		type;           //! �e�N�X�`���̎��
		UInt8		depth;          //! �[�x
		UInt8		mipLevels;      //! �~�b�v�}�b�v��
		UInt8		pool;           //! �������z�u
		Int8		pad;
	};

	//-----------------------------------------------------------------------------
	/**
	 * @struct	TgaFormat
	 * @brief	TgaFormat���
	 */
# pragma pack (1)//������Ɣ���
	struct TgaFormat
	{
		unsigned char	No_ID;			/* 0=�C���[�W�h�c����*/
		unsigned char	CMap_Type;		/* 0=�J���[�}�b�v���� */
		unsigned char	Image_Type;		/* 2=�����kRGB�^�C�v 10=���kRGB�^�C�v */
		unsigned char	CMap_Spec[5];	/* �X�L�b�v�����*/

		unsigned short	Ox;				/* ��� ���� X 0�Œ� */
		unsigned short	Oy;				/* ��� ���� Y 0�Œ� */

		unsigned short	Width;			/* �� (dot)*/
		unsigned short	Hight;			/* ���� (dot)*/

		unsigned char	Pixel;			/* Pixel Size. 32=32bit */
		unsigned char	IDesc_Type;		/* 8=TGA32 �A�g���r���[�g */
	};
# pragma pack ()

	#define			RGBA32(r, g, b, a)	(a << 24) | (b << 16) | (g << 8) | (r << 0)



}}}

#undef		DECLARE_ENUM_BEGIN
#undef		DECLARE_ENUM_END

#endif
