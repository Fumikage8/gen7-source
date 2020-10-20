//==============================================================================
/**
 * @file	gfl_CmdlModel.h
 * 	
 * @author	kimura_shin
 * @data	2012/03/13, 14:30
 */
// =============================================================================

#if !defined( __GFL_CMDLMODEL_H__ )
#define	__GFL_CMDLMODEL_H__

#ifdef GFL_PLATFORM_PC

//#define USE_VERTEX_BUFFER1
#include <qt/gfl_QtGui.h>
#include <qt/gfl_Tga.h>

#define MAYA_INPORT		//いつでも削除できるように TouchIdEditor ではオンにすべし！

#ifdef MAYA_INPORT

#define DECLARE_ENUM_BEGIN( CLASSNAME )			\
	class CLASSNAME {													\
	public:																		\
		enum Enum {
#define DECLARE_ENUM_END( CLASSNAME )				\
		};																			\
	};

namespace gfl {
namespace maya {
namespace GLInterface {
class IShader;
}
}
}

#endif

namespace gfl {

namespace motion {
class MotionBase;
class JointBase;

}

GFL_NAMESPACE_BEGIN( cmdl )

class Model;
class Image;

// =============================================================================
// =============================================================================

class Material;
class Bone;

class Shape {
public:

	class Primitive {
	public:

		Primitive( ) : m_pBoneIndexTable( 0 ), m_pTriangles( 0 ) {
		}

		~Primitive( ) {
			delete [] m_pBoneIndexTable;
			delete [] m_pTriangles;
		}

		void SetTriangleCount( s32 TriangleCount ) {
			this->m_TriangleCount = TriangleCount;
		}

		s32 GetTriangleCount( ) const {
			return m_TriangleCount;
		}

		void SetTriangles( s32* Triangles ) {
			this->m_pTriangles = Triangles;
		}

		s32* GetTriangles( ) const {
			return m_pTriangles;
		}

		void SetBoneIndexTable( s32* BoneIndexTable ) {
			this->m_pBoneIndexTable = BoneIndexTable;
		}

		s32* GetBoneIndexTable( ) const {
			return m_pBoneIndexTable;
		}

		void SetBoneIndexTableCount( s32 BoneIndexTableCount ) {
			this->m_BoneIndexTableCount = BoneIndexTableCount;
		}

		s32 GetBoneIndexTableCount( ) const {
			return m_BoneIndexTableCount;
		}

	private:
		s32* m_pBoneIndexTable;
		s32* m_pTriangles;
		s32 m_BoneIndexTableCount;
		s32 m_TriangleCount;
	};

	struct BoundingBox {
		void Render( );
		gfl::core::Matrix44 m_Matrix;
		gfl::core::Vector m_Size;
	};

	enum {
		MULTI_TEX_MAX = 3
	};

	Shape( void ) : m_pPrimitives( 0 ), m_pVertices( 0 ), m_pNormals( 0 ), m_pTangents( 0 )
	, m_pWorkVertices( 0 ), m_pWorkNormals( 0 ), m_pWorkColor( 0 ), m_pWeights( 0 ), /* m_pIndex( 0 ),*/ m_pBoneIdIndex( 0 ), m_pColor( 0 )
#if 0 // def USE_VERTEX_BUFFER
	, m_VertexBuffer( 0 ), m_IndexBuffer( 0 )
#endif
	{
		for( s32 i = 0; i < MULTI_TEX_MAX; ++i ){
			m_pTextures[i] = m_pWorkTextures[i] = 0;
		}
	}

	~Shape( void ) {
		delete [] m_pPrimitives;
		m_pPrimitives = 0;
		delete [] m_pVertices;
		m_pVertices = 0;
		delete [] m_pNormals;
		m_pNormals = 0;
		delete [] m_pWorkVertices;
		m_pWorkVertices = 0;
		delete [] m_pWorkNormals;
		m_pWorkNormals = 0;
		delete [] m_pWorkColor;
		m_pWorkColor = 0;
		delete [] m_pTangents;
		m_pTangents = 0;
		for( s32 i = 0; i < MULTI_TEX_MAX; ++i ){
			delete [] m_pTextures[i];
			m_pTextures[i] = 0;
			delete [] m_pWorkTextures[i];
			m_pWorkTextures[i] = 0;
		}
		delete [] m_pWeights;
		m_pWeights = 0;
		//		delete [] m_pIndex;
		delete [] m_pBoneIdIndex;
		m_pBoneIdIndex = 0;
		delete [] m_pColor;
		m_pColor = 0;
	}

	void SetPrimitiveCount( s32 PrimitiveCount ) {
		this->m_PrimitiveCount = PrimitiveCount;
	}

	s32 GetPrimitiveCount( ) const {
		return m_PrimitiveCount;
	}

	s32 GetTextureCount( const s32 id = 0 ) const {
		return m_TextureCount[id];
	}

	s32 GetTangentCount( ) const {
		return m_TangentCount;
	}

	s32 GetNormalCount( ) const {
		return m_NormalCount;
	}

	s32 GetVertexCount( ) const {
		return m_VertexCount;
	}

	s32* GetTextureCountPointer( const s32 id ) {
		return &m_TextureCount[id];
	}

	s32* GetTangentCountPointer( ) {
		return &m_TangentCount;
	}

	s32* GetNormalCountPointer( ) {
		return &m_NormalCount;
	}

	s32* GetVertexCountPointer( ) {
		return &m_VertexCount;
	}

	void SetIndexCount( s32 IndexCount ) {
		this->m_IndexCount = IndexCount;
	}

	s32 GetIndexCount( ) const {
		return m_IndexCount;
	}

	void SetTriangleCount( s32* TriangleCount ) {
		this->m_pTriangleCount = TriangleCount;
	}

	s32* GetTriangleCount( ) const {
		return m_pTriangleCount;
	}

	void SetBoneIdIndex( s32* BoneIdIndex ) {
		this->m_pBoneIdIndex = BoneIdIndex;
	}

	s32* GetBoneIdIndex( ) const {
		return m_pBoneIdIndex;
	}
#if 0 // 

	void SetIndex( s32* Index ) {
		this->m_pIndex = Index;
	}

	s32* GetIndex( ) const {
		return m_pIndex;
	}
#endif

	void SetWeights( f32* Weights ) {
		this->m_pWeights = Weights;
	}

	f32* GetWeights( ) const {
		return m_pWeights;
	}

	void SetWorkVertices( gfl::core::Vector* WorkVertices ) {
		this->m_pWorkVertices = WorkVertices;
	}

	gfl::core::Vector* GetWorkVertices( ) const {
		return m_pWorkVertices;
	}

	void SetTextures( const s32 id, gfl::core::Vector2* Textures ) {
		this->m_pTextures[id] = Textures;
	}

	gfl::core::Vector2* GetTextures( const s32 id = 0 ) const {
		return m_pTextures[id];
	}

	void SetTangents( gfl::core::Vector* Tangents ) {
		this->m_pTangents = Tangents;
	}

	gfl::core::Vector* GetTangents( ) const {
		return m_pTangents;
	}

	void SetNormals( gfl::core::Vector* Normals ) {
		this->m_pNormals = Normals;
	}

	gfl::core::Vector* GetNormals( ) const {
		return m_pNormals;
	}

	void SetVertices( gfl::core::Vector* Vertices ) {
		this->m_pVertices = Vertices;
	}

	gfl::core::Vector* GetVertices( ) const {
		return m_pVertices;
	}

	void SetPrimitives( Primitive* Primitives ) {
		this->m_pPrimitives = Primitives;
	}

	Primitive* GetPrimitives( ) const {
		return m_pPrimitives;
	}

	void SetIndexStep( s32 IndexStep ) {
		this->m_IndexStep = IndexStep;
	}

	s32 GetIndexStep( ) const {
		return m_IndexStep;
	}

	void SetBoneIndexCount( s32 BoneIndexCount ) {
		this->m_BoneIndexCount = BoneIndexCount;
	}

	s32 GetBoneIndexCount( ) const {
		return m_BoneIndexCount;
	}

	void SetWorkNormals( gfl::core::Vector* WorkNormals ) {
		this->m_pWorkNormals = WorkNormals;
	}

	gfl::core::Vector* GetWorkNormals( ) const {
		return m_pWorkNormals;
	}

	void SetWorkTextures( const s32 id, gfl::core::Vector2* WorkTextures ) {
		this->m_pWorkTextures[id] = WorkTextures;
	}

	gfl::core::Vector2* GetWorkTextures( const s32 id = 0 ) const {
		return m_pWorkTextures[id];
	}

	void SetColorStep( s32 ColorStep ) {
		this->m_ColorStep = ColorStep;
	}

	s32 GetColorStep( ) const {
		return m_ColorStep;
	}

	void SetColorScale( f32 ColorScale ) {
		this->m_ColorScale = ColorScale;
	}

	f32 GetColorScale( ) const {
		return m_ColorScale;
	}

	void SetColor( u8* Color ) {
		this->m_pColor = Color;
	}

	u8* GetColor( ) const {
		return m_pColor;
	}

	void Skining( gfl::core::Matrix* mat, Bone* pBone, Material* mate );

	void SetWorkColor( u32* WorkColor ) {
		this->m_pWorkColor = WorkColor;
	}

	u32* GetWorkColor( ) const {
		return m_pWorkColor;
	}

	BoundingBox m_BoundingBox;

private:

	void RenderPolygonWireFrame( gfl::core::Vector* vp );
	void RenderPolygon( gfl::core::Vector* vp );
	void RenderSkinPolygon( gfl::core::Vector* skin_vert, const gfl::qt::Render::ShadingType type );

	Primitive* m_pPrimitives;
	gfl::core::Vector* m_pVertices;
	gfl::core::Vector* m_pNormals;
	gfl::core::Vector* m_pTangents;
	gfl::core::Vector2* m_pTextures[MULTI_TEX_MAX];
	gfl::core::Vector* m_pWorkVertices;
	gfl::core::Vector* m_pWorkNormals;
	u32* m_pWorkColor;
	gfl::core::Vector2* m_pWorkTextures[MULTI_TEX_MAX];

	f32* m_pWeights;
	//	s32* m_pIndex;
	s32* m_pBoneIdIndex;
	u8* m_pColor;
	s32* m_pTriangleCount;

	f32 m_ColorScale;

	s32 m_IndexCount;
	s32 m_BoneIndexCount;
	s32 m_VertexCount;
	s32 m_NormalCount;
	s32 m_TangentCount;
	s32 m_TextureCount[MULTI_TEX_MAX];
	s32 m_PrimitiveCount;
	s32 m_IndexStep;
	s32 m_ColorStep;

	friend class Model;
};

struct LookupTable {

	LookupTable( ) : m_pValue( 0 ) {
	}

	~LookupTable( ) {
		delete [] m_pValue;
		glDeleteTextures( 1, &m_hLutTexture );
	}

	QString m_Name;
	bool m_IsGeneratedAsAbs;
	bool m_IsSizeDoubled;
	bool m_IsMipMap;

	QString m_PreRepeatMethod;
	QString m_PostRepeatMethod;
	f32* m_pValue;
	GLuint m_hLutTexture;

};

/**
 * Material
 */
class Material {
public:

	class TextureCoordinator {
	public:

		TextureCoordinator( ) {
		}

		~TextureCoordinator( ) {
		}

		void SetTranslateT( f32 TranslateT ) {
			this->m_TranslateT = TranslateT;
		}

		f32 GetTranslateT( ) const {
			return m_TranslateT;
		}

		void SetTranslateS( f32 TranslateS ) {
			this->m_TranslateS = TranslateS;
		}

		f32 GetTranslateS( ) const {
			return m_TranslateS;
		}

		void SetRotate( f32 Rotate ) {
			this->m_Rotate = Rotate;
		}

		f32 GetRotate( ) const {
			return m_Rotate;
		}

		void SetScaleT( f32 ScaleT ) {
			this->m_ScaleT = ScaleT;
		}

		f32 GetScaleT( ) const {
			return m_ScaleT;
		}

		void SetScaleS( f32 ScaleS ) {
			this->m_ScaleS = ScaleS;
		}

		f32 GetScaleS( ) const {
			return m_ScaleS;
		}

		void SetTextureEnv( const u32 tex_id );

		void SetReferenceCamera( s32 ReferenceCamera ) {
			this->m_ReferenceCamera = ReferenceCamera;
		}

		s32 GetReferenceCamera( ) const {
			return m_ReferenceCamera;
		}

		void SetSourceCoordinate( s32 SourceCoordinate ) {
			this->m_SourceCoordinate = SourceCoordinate;
		}

		s32 GetSourceCoordinate( ) const {
			return m_SourceCoordinate;
		}

		void SetMatrixMode( QString MatrixMode ) {
			this->m_MatrixMode = MatrixMode;
		}

		QString GetMatrixMode( ) const {
			return m_MatrixMode;
		}

		void SetMappingMethod( QString MappingMethod ) {
			this->m_MappingMethod = MappingMethod;
		}

		QString GetMappingMethod( ) const {
			return m_MappingMethod;
		}

	private:
		QString m_MappingMethod;
		QString m_MatrixMode;
		s32 m_SourceCoordinate;
		s32 m_ReferenceCamera;
		f32 m_ScaleS;
		f32 m_ScaleT;
		f32 m_Rotate;
		f32 m_TranslateS;
		f32 m_TranslateT;
	};

	class TextureMapper {
	public:

		TextureMapper( ) {
		}

		~TextureMapper( ) {
		}

		void SetName( const QString & Name ) {
			this->m_Name = Name;
		}

		const QString & GetName( ) const {
			return m_Name;
		}

		void SetFileName( const QString & Name ) {
			this->m_FileName = Name;
		}

		const QString & GetFileName( ) const {
			return m_FileName;
		}

		u32* GetTextureId( void ) {
			return &m_TextureId;
		}

		void SetWrapT( u32 WrapT ) {
			this->m_WrapT = WrapT;
		}

		u32 GetWrapT( ) const {
			return m_WrapT;
		}

		void SetWrapS( u32 WrapS ) {
			this->m_WrapS = WrapS;
		}

		u32 GetWrapS( ) const {
			return m_WrapS;
		}

		void SetMinFilter( u32 MinFilter ) {
			this->m_MinFilter = MinFilter;
		}

		u32 GetMinFilter( ) const {
			return m_MinFilter;
		}

		void SetMagFilter( u32 MagFilter ) {
			this->m_MagFilter = MagFilter;
		}

		u32 GetMagFilter( ) const {
			return m_MagFilter;
		}

		QList<Image*>* GetImageList( ) {
			return &m_ImageList;
		}

		void SetTextureEnv( );

		gfl::core::Vector4 m_BorderColor;
		s32 m_MinLod;
		s32 m_LodBias;

	private:
		QList<Image*> m_ImageList;
		QString m_Name;
		QString m_FileName;
		u32 m_TextureId;
		u32 m_MagFilter;
		u32 m_MinFilter;
		u32 m_WrapS;
		u32 m_WrapT;
	};

	class MaterialColor {
	public:

		enum {
			SPECULAR_MAX = 2,
			CONSTANT_MAX = 6
		};

		void SetSpecular( const s32 id, const gfl::core::Vector4& Specular ) {
			this->m_Specular[id] = Specular;
		}

		const gfl::core::Vector4& GetSpecular( const s32 id ) const {
			return m_Specular[id];
		}

		void SetConstant( const s32 id, const gfl::core::Vector4& Constant ) {
			this->m_Constant[id] = Constant;
		}

		const gfl::core::Vector4& GetConstant( const s32 id ) const {
			return m_Constant[id];
		}

		void SetDiffuse( const gfl::core::Vector4& Diffuse ) {
			this->m_Diffuse = Diffuse;
		}

		const gfl::core::Vector4& GetDiffuse( ) const {
			return m_Diffuse;
		}

		void SetAmbient( const gfl::core::Vector4& Ambient ) {
			this->m_Ambient = Ambient;
		}

		const gfl::core::Vector4& GetAmbient( ) const {
			return m_Ambient;
		}

		void SetEmission( const gfl::core::Vector4& Emission ) {
			this->m_Emission = Emission;
		}

		const gfl::core::Vector4& GetEmission( ) const {
			return m_Emission;
		}

		void SetLight( );

		s32 m_VertexColorScale;

	private:

		// MaterialColor
		gfl::core::Vector4 m_Emission;
		gfl::core::Vector4 m_Ambient;
		gfl::core::Vector4 m_Diffuse;
		gfl::core::Vector4 m_Specular[SPECULAR_MAX];
		gfl::core::Vector4 m_Constant[CONSTANT_MAX];
	};

	struct Shader {

		struct FragmentShader {

			struct FragmentBump {
				QString m_BumpTextureIndex;
				QString m_BumpMode;
				bool m_IsBumpRenormalize;
			};

			struct FragmentLighting {
				QString m_FresnelConfig;
				bool m_IsClampHighLight;
				bool m_IsDistribution0Enabled;
				bool m_IsDistribution1Enabled;
				bool m_IsGeometricFactor0Enabled;
				bool m_IsGeometricFactor1Enabled;
				bool m_IsReflectionEnabled;
			};

			struct FragmentLightingTable {

				struct ReferenceLookupTable {

					ReferenceLookupTable( ) : m_TableName( 0 ), m_LookupTableSetContent( 0 ) {
					}

					~ReferenceLookupTable( ) {
						delete m_TableName;
						m_TableName = 0;
						delete m_LookupTableSetContent;
						m_LookupTableSetContent = 0;
					}
					QString* m_TableName;
					QString* m_LookupTableSetContent;
				};

				struct ReflectanceSampler {

					ReflectanceSampler( ) : m_pReferenceLookupTable( 0 ) {
					}

					~ReflectanceSampler( ) {
						delete m_pReferenceLookupTable;
						m_pReferenceLookupTable = 0;
					}
					bool m_IsAbs;
					QString m_Input;
					QString m_Scale;
					ReferenceLookupTable* m_pReferenceLookupTable;
				};

				ReflectanceSampler m_ReflectanceRSampler;
				ReflectanceSampler m_ReflectanceGSampler;
				ReflectanceSampler m_ReflectanceBSampler;
				ReflectanceSampler m_Distribution0Sampler;
				ReflectanceSampler m_Distribution1Sampler;
				ReflectanceSampler m_FresnelSampler;
			};

			struct TextureCombiner {

				struct Source {
					QString m_Source0;
					QString m_Source1;
					QString m_Source2;
				};

				struct Operand {
					QString m_Operand0;
					QString m_Operand1;
					QString m_Operand2;
				};

				QString m_CombineRgb;
				QString m_CombineAlpha;
				QString m_ScaleRgb;
				QString m_ScaleAlpha;
				QString m_Constant;
				QString m_BufferInputRgb;
				QString m_BufferInputAlpha;

				Source m_SourceRgb;
				Operand m_OperandRgb;
				Source m_SourceAlpha;
				Operand m_OperandAlpha;
			};

			struct AlphaTest {
				bool m_IsTestEnabled;
				QString m_TestFunction;
				f32 m_TestReference;
			};

			~FragmentShader( ) {
				while(!m_TextureConbiner.isEmpty( )) {
					delete m_TextureConbiner.takeFirst( );
				}
			}

			QString m_LayerConfig;
			gfl::core::Vector4 m_BufferColor;
			FragmentBump m_FragmentBump;
			FragmentLighting m_FragmentLighting;
			FragmentLightingTable m_FragmentLightingTable;
			QList<TextureCombiner*> m_TextureConbiner;
			AlphaTest m_AlphaTest;
		};

		struct FragmentOperation {

			struct DepthOperation {
				QString m_TestFunction;
				bool m_IsTestEnabled;
				bool m_IsMaskEnabled;
			};

			struct BlendOperation {

				struct BlendParameter {
					QString m_BlendFunctionSource;
					QString m_BlendFunctionDestination;
					QString m_BlendEquation;
				};

				QString m_Mode;
				QString m_LogicOperation;
				BlendParameter m_RgbParameter;
				BlendParameter m_AlphaParameter;
				gfl::core::Vector4 m_BlendColor;
			};

			struct StencilOperation {
				bool m_IsTestEnabled;
				QString m_TestFunction;
				s32 m_TestReference;
				s32 m_TestMask;
				QString m_FailOperation;
				QString m_ZFailOperation;
				QString m_PassOperation;
			};

			DepthOperation m_DepthOperation;
			BlendOperation m_BlendOperation;
			StencilOperation m_StencilOperation;
		};

		struct UserData {

			UserData( QString type, QString val ) : m_Type( type ), m_Value( val ) {
			}

			QString m_Type;
			QString m_Value;
		};

		~Shader( ) {
			QList<UserData*> lst = m_UserData.values( );
			while(!lst.isEmpty( )) {
				delete lst.takeFirst( );
			}
		}

		QString m_Name;
		QString m_FileName;

		FragmentShader m_FragmentShader;
		FragmentOperation m_FragmentOperation;
		QMap<QString, UserData*> m_UserData;
	};

#ifdef MAYA_INPORT

	//-----------------------------------------------------------------------------
	// シェーダータイプ
	DECLARE_ENUM_BEGIN( ShaderType )
	ModelRender = 0,
	UvRender,
	SimpleIDRender,
	Ignore,
	NumberOf
	DECLARE_ENUM_END( ShaderType )

	class impl;
	impl *m_pImpl[ShaderType::NumberOf];
	ShaderType::Enum m_ShaderType;

	void glCreateShader( );
	void glBindShader( ShaderType::Enum shaderType );
	void glDeleteShader( );
	void SetShaderType( ShaderType::Enum type );
#endif

	Material( ) : m_pTextureCoordinator( 0 ), m_pTextureMapper( 0 ), m_pMaterialColor( 0 ), m_pShader( 0 ) {
#ifdef MAYA_INPORT
		for( int i = 0; i < ShaderType::NumberOf; ++i )
			m_pImpl[i] = NULL;
#endif
	}

	~Material( ) {
		delete [] m_pTextureCoordinator;
		delete [] m_pTextureMapper;
		delete m_pMaterialColor;
		delete m_pShader;

#ifdef MAYA_INPORT
		glDeleteShader( );
#endif
	}

	void SetName( const QString& Name ) {
		this->m_Name = Name;
	}

	const QString& GetName( ) const {
		return m_Name;
	}

	void SetTextureMapperCount( s32 TextureMapperCount ) {
		this->m_TextureMapperCount = TextureMapperCount;
	}

	s32 GetTextureMapperCount( ) const {
		return m_TextureMapperCount;
	}

	void SetTextureMapper( TextureMapper* TextureMapper ) {
		this->m_pTextureMapper = TextureMapper;
	}

	TextureMapper* GetTextureMapper( ) const {
		return m_pTextureMapper;
	}

	void SetMaterialColor( MaterialColor* MaterialColor ) {
		this->m_pMaterialColor = MaterialColor;
	}

	MaterialColor* GetMaterialColor( ) const {
		return m_pMaterialColor;
	}

	void SetTextureCoordinatorCount( s32 TextureCoordinatorCount ) {
		this->m_TextureCoordinatorCount = TextureCoordinatorCount;
	}

	s32 GetTextureCoordinatorCount( ) const {
		return m_TextureCoordinatorCount;
	}

	s32 GetTextureMapperCoordCount( ) const {
		return min( GetTextureMapperCount( ), GetTextureCoordinatorCount( ) );
	}

	void SetTextureCoordinator( TextureCoordinator* TextureCoordinator ) {
		this->m_pTextureCoordinator = TextureCoordinator;
	}

	TextureCoordinator* GetTextureCoordinator( ) const {
		return m_pTextureCoordinator;
	}

	void SetShader( Shader* Shader ) {
		this->m_pShader = Shader;
	}

	Shader* GetShader( ) const {
		return m_pShader;
	}

	void DraswUvSpaceNo( s32 no ) {
		m_DraswUvSpaceNo = no;
	}

	void DrawUvDepthTest( bool f ) {
		m_DrawUvDepthTest = f;
	}

#ifdef MAYA_INPORT
	void glBind( ShaderType::Enum shaderType, QList<LookupTable*> &rLookupTables );
	void SetCamera( gfl::core::Camera* );
	void SetLightEnable( bool f );
#endif

	struct Rasterization {
		QString m_CullingMode;
		bool m_IsPolygonOffsetEnabled;
		s32 m_PolygonOffsetUnit;
	};

	Rasterization m_Rasterization;

	bool m_IsCompressible;
	s32 m_LightSetIndex;
	s32 m_FogIndex;
	bool m_IsFragmentLightEnabled;
	bool m_IsVertexLightEnabled;
	bool m_IsHemiSphereLightEnabled;
	bool m_IsHemiSphereOcclusionEnabled;
	bool m_IsFogEnabled;
	QString m_TextureCoordinateConfig;
	QString m_TranslucencyKind;
	s32 m_ShaderProgramDescriptionIndex;
	QString m_ShaderBinaryKind;
	s32 m_DraswUvSpaceNo;
	s32 m_DrawUvDepthTest;

private:
	QString m_Name;

	TextureCoordinator* m_pTextureCoordinator;
	TextureMapper* m_pTextureMapper;
	MaterialColor* m_pMaterialColor;
	Shader* m_pShader;

	s32 m_TextureCoordinatorCount;
	s32 m_TextureMapperCount;
};

/**
 * Mesh
 */
class Mesh {
public:

	void SetMaterial( Material* Material ) {
		this->m_pMaterial = Material;
	}

	Material* GetMaterial( ) const {
		return m_pMaterial;
	}

	void SetShape( Shape* Shape ) {
		this->m_pShape = Shape;
	}

	Shape* GetShape( ) const {
		return m_pShape;
	}

	Mesh( ) : m_pMaterial( 0 ) {
	}

private:
	Shape* m_pShape;
	Material* m_pMaterial;
};

/**
 * Bone
 */
class Bone {
public:

	void SetScale( gfl::core::Vector Scale ) {
		this->m_Scale = Scale;
	}

	const gfl::core::Vector& GetScale( ) const {
		return m_Scale;
	}

	void SetRotate( const gfl::core::Vector& Rotate ) {
		this->m_Rotate = Rotate;
	}

	const gfl::core::Vector& GetRotate( ) const {
		return m_Rotate;
	}

	void SetTranslate( const gfl::core::Vector& Translate ) {
		this->m_Translate = Translate;
	}

	const gfl::core::Vector& GetTranslate( ) const {
		return m_Translate;
	}

	void SetParentName( const QString& ParentName ) {
		this->m_ParentName = ParentName;
	}

	const QString& GetParentName( ) const {
		return m_ParentName;
	}

	void SetName( const QString& Name ) {
		this->m_Name = Name;
	}

	const QString& GetName( ) const {
		return m_Name;
	}

	gfl::core::Matrix* GetInvMatrix( ) {
		return &m_InvMatrix;
	}

private:
	gfl::core::Matrix m_InvMatrix;
	QString m_Name;
	QString m_ParentName;
	gfl::core::Vector m_Translate;
	gfl::core::Vector m_Rotate;
	gfl::core::Vector m_Scale;
};

class Image : public gfl::qt::Tga {
public:

	Image( ) : m_pTextureMapper( 0 ), m_RGB_Type( 0 ), m_OrderType( 0 ) {
	}

	void SetOrderType( s32 OrderType ) {
		this->m_OrderType = OrderType;
	}

	/**
	 * GL_ 色配列を返す
	 * @return GL_UNSIGNED_BYTE　など
	 */
	s32 GetOrderType( ) const {
		return m_OrderType;
	}

	void SetRGB_Type( s32 RGB_Type ) {
		this->m_RGB_Type = RGB_Type;
	}

	/**
	 * GL_ のRGB型を返す
	 * @return GL_RGB など
	 */
	s32 GetRGB_Type( ) const {
		return m_RGB_Type;
	}

	void SetTextureMapper( Material::TextureMapper* TextureMapper ) {
		this->m_pTextureMapper = TextureMapper;
	}

	Material::TextureMapper* GetTextureMapper( ) const {
		return m_pTextureMapper;
	}

	const QString& GetName( void ) const {
		return m_pTextureMapper->GetName( );
	}

	const QString& GetFileName( void ) const {
		return m_pTextureMapper->GetFileName( );
	}

private:
	Material::TextureMapper* m_pTextureMapper;
	s32 m_RGB_Type;
	s32 m_OrderType;
};

/**
 * Model
 */
class Model {
public:

	class TextureTabInfo {
	public:

		TextureTabInfo( const QString& fname, Material* mate ) : m_FileName( fname ), m_pMaterial( mate ) {
		}

		Material* GetMaterial( ) const {
			return m_pMaterial;
		}

		const QString& GetFileName( ) const {
			return m_FileName;
		}

	private:
		QString m_FileName;
		Material* m_pMaterial;
	};

	Model( ) : m_pShape( 0 ), m_pMaterial( 0 ), m_pMesh( 0 ), m_pBone( 0 ), m_pJointMatrix( 0 )
	, m_pCamera( 0 ), m_fLightEnable( true ), m_NoSkeletonMode(false) {
	}

	~Model( ) {
		Clear( );
	}

	void Clear( ) {
		// 重複して削除されるのを回避する
		QMap<Image*, s32> removed_list;
		while(!m_ImageData.isEmpty( )) {
			Image* img = m_ImageData.takeFirst( );
			//			gfl::core::Debug::PrintConsole( "img %p\n",img );
			if( !removed_list.value( img ) ){
				removed_list.insert( img, 1 );
				delete img;
			}
		}
		while(!m_LookupTable.isEmpty( )) {
			delete m_LookupTable.takeFirst( );
		}
		delete [] m_pShape;
		m_pShape = 0;
		delete [] m_pMaterial;
		m_pMaterial = 0;
		delete [] m_pMesh;
		m_pMesh = 0;
		delete [] m_pBone;
		m_pBone = 0;
		delete [] m_pJointMatrix;
		m_pJointMatrix = 0;
	}

	void RenderPrimitiveWireFrame( void );
	void RenderPrimitive( void );
	void RenderSkin( gfl::motion::MotionBase* mot, const gfl::qt::Render::ShadingType type
					 , const size_t ofs );
	void RenderSkin( gfl::core::Matrix* mat = 0, const gfl::qt::Render::ShadingType type = gfl::qt::Render::SHD_NORMAL );
	void RenderNoSkin( gfl::motion::MotionBase* mot, const gfl::qt::Render::ShadingType type
					 , const size_t ofs );

	void RenderBoundingBox( void );

	void SetShapeCount( s32 ShapeCount ) {
		this->m_ShapeCount = ShapeCount;
	}

	s32 GetShapeCount( ) const {
		return m_ShapeCount;
	}

	void SetShape( Shape* Shape ) {
		this->m_pShape = Shape;
	}

	Shape* GetShape( ) const {
		return m_pShape;
	}

	void SetBoneCount( s32 BoneCount ) {
		this->m_BoneCount = BoneCount;
	}

	s32 GetBoneCount( ) const {
		return m_BoneCount;
	}

	void SetMeshCount( s32 MeshCount ) {
		this->m_MeshCount = MeshCount;
	}

	s32 GetMeshCount( ) const {
		return m_MeshCount;
	}

	void SetMaterialCount( s32 MaterialCount ) {
		this->m_MaterialCount = MaterialCount;
	}

	s32 GetMaterialCount( ) const {
		return m_MaterialCount;
	}

	void SetMesh( Mesh* Mesh ) {
		this->m_pMesh = Mesh;
	}

	Mesh* GetMesh( ) const {
		return m_pMesh;
	}

	void SetMaterial( Material* Material ) {
		this->m_pMaterial = Material;
	}

	Material* GetMaterial( ) const {
		return m_pMaterial;
	}

	void SetBone( Bone* Bone ) {
		this->m_pBone = Bone;
	}

	Bone* GetBone( ) const {
		return m_pBone;
	}

	QList<TextureTabInfo>* GetTextureTabInfo( void ) {
		return &m_TextureTabInfo;
	}

	QList<Image*>* GetImageData( void ) {
		return &m_ImageData;
	}

	QList<Image*>* GetTextureFileData( void ) {
		return &m_TextureFileData;
	}
	void SetImageData( s32 w, s32 h, c8* dat, s32 rgb_type = 0, s32 order_type = 0, Material::TextureMapper* tm = 0 );

	QList<LookupTable*>* GetLookupTable( ) {
		return &m_LookupTable;
	}

	void SetCamera( gfl::core::Camera* pCamera );
	void SetLightEnable( bool f );

	void SetJointMatrix( gfl::core::Matrix* JointMatrix ) {
		this->m_pJointMatrix = JointMatrix;
	}

	gfl::core::Matrix* GetJointMatrix( ) const {
		return m_pJointMatrix;
	}

    void SetNoSkeletonMode( s32 NoSkeletonMode ) {
    	this->m_NoSkeletonMode = NoSkeletonMode;
    }

    s32 GetNoSkeletonMode( ) const {
    	return m_NoSkeletonMode;
    }

private:
	QList<TextureTabInfo> m_TextureTabInfo;
	QList<Image*> m_ImageData; // 取っておいてデストラクタで削除
	QList<Image*> m_TextureFileData; // ポインタを取っておくだけ
	Shape* m_pShape;
	Material* m_pMaterial;
	Mesh* m_pMesh;
	Bone* m_pBone;
	QList<LookupTable*> m_LookupTable;
	gfl::core::Matrix* m_pJointMatrix;

	s32 m_ShapeCount;
	s32 m_MaterialCount;
	s32 m_MeshCount;
	s32 m_BoneCount;
	s32 m_NoSkeletonMode;

	gfl::core::Camera *m_pCamera;
	bool m_fLightEnable;
};

GFL_NAMESPACE_END( cmdl )

#ifndef GFL_DAE_MODEL
GFL_NAMESPACE_BEGIN( grp )
GFL_NAMESPACE_BEGIN( g3d )
class Model : public gfl::cmdl::Model {
};
GFL_NAMESPACE_END( g3d )
GFL_NAMESPACE_END( grp )
#endif

}

#undef		DECLARE_ENUM_BEGIN
#undef		DECLARE_ENUM_END

#endif

#endif	/* __GFL_CMDLMODEL_H__ */

