/* 
 * File:   gfl_QtModel.h
 * Author: s-kimura
 *
 * Created on 2009/01/09, 15:17
 */

#ifndef GFL_QTMODEL_H
#define	GFL_QTMODEL_H

//#define USE_VERTEX_BUFFER1
#include <qt/gfl_QtGui.h>

namespace gfl {

namespace motion {
class MotionBase;
}

namespace qt {

class Model;

// =============================================================================
// =============================================================================

class Parts {
private:
	QString m_MeshName;
	gfl::core::Vector* m_pVertices;
	gfl::core::Vector* m_pNormals;
	s32* m_pTriangles;

	s32 m_Vertices;
	s32 m_Normals;
	s32 m_Triangles;
	s32 m_Step;

#ifdef USE_VERTEX_BUFFER
	GLuint m_VertexBuffer;
	GLuint m_IndexBuffer;
	s32* m_pIndexBuffer;
#endif
public:

	Parts( void ) : m_pVertices( 0 ), m_pNormals( 0 ), m_pTriangles( 0 )
#ifdef USE_VERTEX_BUFFER
	, m_VertexBuffer( 0 ), m_pIndexBuffer( 0 )
#endif
	{
	}

	~Parts( void ) {
		delete [] m_pVertices;
		delete [] m_pNormals;
		delete [] m_pTriangles;
#ifdef USE_VERTEX_BUFFER
		delete [] m_pIndexBuffer;
#endif
	}

	gfl::core::Vector* GetVertices( void ) {
		return m_pVertices;
	}

	gfl::core::Vector* GetNormals( void ) {
		return m_pNormals;
	}

	s32* GetTriangles( void ) {
		return m_pTriangles;
	}

	s32 GetVerticesCount( void ) {
		return m_Vertices;
	}

	s32 GetTrianglesCount( void ) {
		return m_Triangles;
	}

	s32 GetStep( void ) {
		return m_Step;
	}

	void Initialize( void );

	void RenderPolygonWireFrame( gfl::core::Vector* vert );
	void RenderPolygon( gfl::core::Vector* vert );

	void SetStep( s32 Step ) {
		this->m_Step = Step;
	}

	void SetTriangles( s32 Triangles ) {
		this->m_Triangles = Triangles;
	}

	void SetNormals( s32 Normals ) {
		this->m_Normals = Normals;
	}

	void SetVertices( s32 Vertices ) {
		this->m_Vertices = Vertices;
	}

	void SetTriangles( s32* Triangles ) {
		this->m_pTriangles = Triangles;
	}

	void SetNormals( gfl::core::Vector* m_pNormals ) {
		this->m_pNormals = m_pNormals;
	}

	void SetVertices( gfl::core::Vector* m_pVertices ) {
		this->m_pVertices = m_pVertices;
	}

	void SetMeshName( const QString& MeshName ) {
		this->m_MeshName = MeshName;
	}

	const QString& GetMeshName( ) const {
		return m_MeshName;
	}

	friend class Model;
};

// =============================================================================
// =============================================================================

class Skin {
private:
	QStringList m_NameList;
	gfl::core::Matrix* m_pInvBaseMatrix;
	gfl::core::Vector* m_pVertex;
	f32* m_pWeights;
	s32* m_pIndexCount;
	s32* m_pIndex;
	s32* m_pBoneIdIndex;
	s32 m_Index;
	s32 m_Step;

public:

	Skin( void ) : m_pInvBaseMatrix( 0 ), m_pVertex( 0 ), m_pWeights( 0 ), m_pIndexCount( 0 ), m_pIndex( 0 ), m_pBoneIdIndex( 0 ) {
	}

	~Skin( void ) {
		delete [] m_pInvBaseMatrix;
		delete [] m_pWeights;
		delete [] m_pVertex;
		delete [] m_pIndexCount;
		delete [] m_pIndex;
		delete [] m_pBoneIdIndex;
	}

	f32* GetWeights( void ) {
		return m_pWeights;
	}

	gfl::core::Matrix* GetInvBaseMatrix( void ) {
		return m_pInvBaseMatrix;
	}

	s32* GetIndexCount( void ) {
		return m_pIndexCount;
	}

	s32* GetIndex( void ) {
		return m_pIndex;
	}

	s32 GetBoneIdIndex( const s32 id ) {
		return m_pBoneIdIndex[id];
	}

	s32* GetBoneIdIndex( void ) {
		return m_pBoneIdIndex;
	}

	s32 GetIndexLoop( void ) {
		return m_Index;
	}

	void SetStep( s32 Step ) {
		this->m_Step = Step;
	}

	void SetIndex( s32 Index ) {
		this->m_Index = Index;
	}

	void SetBoneIdIndex( s32* BoneIdIndex ) {
		this->m_pBoneIdIndex = BoneIdIndex;
	}

	void SetIndex( s32* Index ) {
		this->m_pIndex = Index;
	}

	void SetIndexCount( s32* IndexCount ) {
		this->m_pIndexCount = IndexCount;
	}

	void SetWeights( f32* Weights ) {
		this->m_pWeights = Weights;
	}

	void SetInvBaseMatrix( gfl::core::Matrix* InvBaseMatrix ) {
		this->m_pInvBaseMatrix = InvBaseMatrix;
	}

	QStringList* GetNameList( ) {
		return &m_NameList;
	}

	void SetVertex( gfl::core::Vector* Vertex ) {
		this->m_pVertex = Vertex;
	}

	gfl::core::Vector* GetVertex( ) const {
		return m_pVertex;
	}
	friend class Model;
};

// =============================================================================
// =============================================================================

class Model {
private:
	QMap<QString, Parts*> m_IdMap;

	Parts* m_pParts;
	s32 m_Parts;

	Skin* m_pSkin;
	s32 m_Skin;

	QString m_ErrorMessage;

	//	s32* m_pBoneIdToJointId;
	//	s32 m_IdCount;

public:

	Model( void ) : m_pParts( 0 ), m_pSkin( 0 ), m_Skin( 0 )/*, m_pBoneIdToJointId( 0 )*/ {
	}

	~Model( void ) {
		delete [] m_pParts;
		delete [] m_pSkin;
		//		delete [] m_pBoneIdToJointId;
	}

	Parts* GetParts( void ) {
		return m_pParts;
	}

	s32 GetPartsCount( void ) {
		return m_Parts;
	}

	Skin* GetSkin( void ) {
		return m_pSkin;
	}

	s32 GetSkinCount( void ) {
		return m_Skin;
	}
#if 0 // 

	s32 GetBoneIdToJointId( const s32 id ) {
		return m_pBoneIdToJointId[id];
	}

	s32* GetBoneIdToJointId( void ) {
		return m_pBoneIdToJointId;
	}

	s32 GetIdCount( void ) const {
		return mIdCount;
	}
#endif

	enum {
		MAT_MOTION,
		MAT_GLOBAL,
	};
	void RenderSkinPolygon( gfl::qt::Parts* parts, gfl::core::Vector* skin_vert, const Render::PolygonColor col, const Render::ShadingType type );
#if 0 // 
	void RenderSkin( gfl::motion::MotionBase* mot, const Render::PolygonColor col = Render::PCOL_GRAY, const Render::ShadingType type = Render::SHD_NORMAL,
					 const size_t ofs = GFL_OFFSET_OF( gfl::motion::JointBase, _MotionMatrix ) );
	void RenderNoSkin( gfl::motion::MotionBase* mot, const Render::PolygonColor col = Render::PCOL_GRAY, const Render::ShadingType type = Render::SHD_NORMAL,
					 const size_t ofs = GFL_OFFSET_OF( gfl::motion::JointBase, _MotionMatrix ) );
#else
	void RenderSkin( gfl::motion::MotionBase* mot, const Render::PolygonColor col, const Render::ShadingType type, const size_t ofs );
	void RenderNoSkin( gfl::motion::MotionBase* mot, const Render::PolygonColor col, const Render::ShadingType type, const size_t ofs );
#endif
#if 0 // 

	void SetIdCount( s32 IdCount ) {
		this->mIdCount = IdCount;
	}

	void SetBoneIdToJointId( s32* BoneIdToJointId ) {
		this->m_pBoneIdToJointId = BoneIdToJointId;
	}
#endif

	void SetSkin( s32 Skin ) {
		this->m_Skin = Skin;
	}

	void SetSkin( Skin* Skin ) {
		this->m_pSkin = Skin;
	}

	void SetParts( s32 Parts ) {
		this->m_Parts = Parts;
	}

	void SetParts( Parts* Parts ) {
		this->m_pParts = Parts;
	}

	void SetIdMap( const QString& id, Parts* parts ) {
		m_IdMap.insert( id, parts );
	}

	Parts* GetIdMap( const QString& id ) {
		return m_IdMap.value( id );
	}

	const QString& GetErrorMessage( ) const {
		return m_ErrorMessage;
	}

	void SetErrorMessage( const QString& ErrorMessage ) {
		this->m_ErrorMessage = ErrorMessage;
	}
};

}

//#define GFL_DAE_MODEL
#ifdef GFL_DAE_MODEL

GFL_NAMESPACE_BEGIN( grp )
GFL_NAMESPACE_BEGIN( g3d )
class Model : public qt::Model {
};
GFL_NAMESPACE_END( g3d )
GFL_NAMESPACE_END( grp )
#endif

}

#endif	/* GFL_MODEL_H */
