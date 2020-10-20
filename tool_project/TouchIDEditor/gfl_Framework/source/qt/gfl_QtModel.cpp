


#include "core/gfl_Matrix.h"
#include "motion/gfl_MotionBase.h"
#include "motion/gfl_JointBase.h"
// =============================================================================
// gfl::qt::Parts
// =============================================================================
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
static const f32 COLOR_TABLE[][4] = {
	{0.75f, 0.75f, 0.75f, 1.0f }, // COL_WHITE

	{0.75f, 0.15f, 0.15f, 1.0f }, // COL_RED
	{0.15f, 0.75f, 0.15f, 1.0f }, // COL_GREEN
	{0.15f, 0.15f, 0.75f, 1.0f }, // COL_BLUE
	{0.75f, 0.75f, 0.15f, 1.0f }, // COL_YELLOW (Gold?)
	{0.45f, 0.45f, 0.45f, 1.0f }, // COL_GRAY

	{0.5f, 0.1f, 0.1f, 1.0f }, // COL_DARKRED
	{0.1f, 0.5f, 0.1f, 1.0f }, // COL_DARKGREEN
	{0.1f, 0.1f, 0.5f, 1.0f }, // COL_DARKBLUE
	{0.5f, 0.5f, 0.1f, 1.0f }, // COL_DARKYELLOW
	{0.25f, 0.25f, 0.25f, 1.0f }, // COL_DARKGRAY

	{0.75f, 0.3f, 0.3f, 1.0f }, // COL_LIGHTRED
	{0.3f, 0.75f, 0.3f, 1.0f }, // COL_LIGHTGREEN
	{0.3f, 0.3f, 0.75f, 1.0f }, // COL_LIGHTBLUE
	{0.75f, 0.75f, 0.3f, 1.0f }, // COL_LIGHTYELLOW
	{0.75f, 0.75f, 0.75f, 1.0f }, // COL_LIGHTGRAY
};
void gfl::qt::Parts::Initialize( void )
{
#ifdef USE_VERTEX_BUFFER
	if( !m_VertexBuffer ){
		//		s32 size = gfl::core::Math::TXYZ * mVertices;
		//		m_pVertexBuffer = DbgNew f32 [size];
		//		size *= sizeof(f32 );
		//		gfl::core::Utility::Memcpy( m_pVertexBuffer, m_pVertices, size );

		//　頂点バッファの作成
		glGenBuffers( 1, &m_VertexBuffer );
		//		glBindBuffer( GL_ARRAY_BUFFER, m_VertexBuffer );
		//		glBufferData( GL_ARRAY_BUFFER, size * gfl::core::Math::TXYZ * mVertices, m_pVertexBuffer, GL_DYNAMIC_DRAW );
		//		glBufferData( GL_ARRAY_BUFFER, size, m_pVertexBuffer, GL_DYNAMIC_DRAW );

		//　インデックスバッファの作成
		glGenBuffers( 1, &m_IndexBuffer );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(s32 ) * m_Triangles * 3, m_pIndexBuffer, GL_STATIC_DRAW );

		// ノーマルバッファ
		//		glGenBuffers( 1, &mNormalBuffer );
		//		m_pNormalBuffer = DbgNew f32 [mTriangles * gfl::core::Math::TXYZ];
		//		glBindBuffer( GL_ARRAY_BUFFER, mNormalBuffer );

		//　バインドしたものをもどす
		//		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	}
#endif
}

// ---------------------------------------------------------------------------
// ワイヤーフレーム描画
// ---------------------------------------------------------------------------
void gfl::qt::Parts::RenderPolygonWireFrame( gfl::core::Vector* vert )
{
	s32* tri = GetTriangles( );
	if( tri ){
		s32 count = GetTrianglesCount( );
		s32 step = GetStep( );

		glDisable( GL_LIGHTING );
		for( s32 j = 0; j < count; ++j ){
			glBegin( GL_LINE_LOOP );
			glVertex3fv( ( f32* ) & vert[*tri] );
			tri += step;
			glVertex3fv( ( f32* ) & vert[*tri] );
			tri += step;
			glVertex3fv( ( f32* ) & vert[*tri] );
			tri += step;
			glEnd( );
		}
		glEnable( GL_LIGHTING );
	}
}

// ---------------------------------------------------------------------------
// ポリゴン描画
// ---------------------------------------------------------------------------
void gfl::qt::Parts::RenderPolygon( gfl::core::Vector* vert )
{
	s32* tri = GetTriangles( );
	if( tri ){
#ifndef USE_VERTEX_BUFFER
		const s32 count = GetTrianglesCount( );
		const s32 step = GetStep( );
		const s32 step2 = step << 1;
		const s32 step3 = step + step2;

		for( s32 j = 0; j < count; ++j ){
			glBegin( GL_TRIANGLES );

			s32 ofs[3];
			ofs[0] = tri[0];
			ofs[1] = tri[step];
			ofs[2] = tri[step2];

			//			Gr::Debug::PrintConsole( "[%d] -> %d\n", j*3, ofs[0] );
			//			Gr::Debug::PrintConsole( "[%d] -> %d\n", j*3+1, ofs[1] );
			//			Gr::Debug::PrintConsole( "[%d] -> %d\n", j*3+2, ofs[2] );

			gfl::qt::Render::SetTriangleNormal( vert[ofs[0]], vert[ofs[1]], vert[ofs[2]] );

			glVertex3fv( ( f32* ) & vert[ofs[0]] );
			glVertex3fv( ( f32* ) & vert[ofs[1]] );
			glVertex3fv( ( f32* ) & vert[ofs[2]] );

			//			Gr::Debug::PrintConsole( "[%d] -> %d %f\n", j * 3, ofs[0], vert[ofs[0]].GetX( ) );
			//			Gr::Debug::PrintConsole( "[%d] -> %d %f\n", j * 3+1, ofs[1], vert[ofs[1]].GetX( ) );
			//			Gr::Debug::PrintConsole( "[%d] -> %d %f\n", j * 3+2, ofs[2], vert[ofs[2]].GetX( ) );

			tri += step3;
			glEnd( );
		}
#else
		Initialize( );

		//		glEnableClientState( GL_VERTEX_ARRAY );
		//		glEnableClientState( GL_NORMAL_ARRAY );

		glBindBuffer( GL_ARRAY_BUFFER, m_VertexBuffer );
		glBufferData( GL_ARRAY_BUFFER, sizeof(f32 ) * gfl::core::TXYZ * m_Vertices * 2, vert, GL_DYNAMIC_DRAW );
		//		glVertexPointer( 3, GL_FLOAT, 0, 0 );

#    if 0
		// 法線をバインド
		Gr::Vector* np = ( Gr::Vector* )m_pNormalBuffer;
		const s32 count = GetTrianglesCount( );
		const s32 step = GetStep( );
		const s32 step2 = step << 1;
		const s32 step3 = step + step2;
		for( s32 i = 0; i < count; ++i, ++np ){
			np->CalcNormal( vert[tri[0]], vert[tri[step]], vert[tri[step2]] );
			np->PrintConsole( "norm", i );
			vert[tri[0]].PrintConsole( "v0", 0 );
			vert[tri[step]].PrintConsole( "v1", step );
			vert[tri[step2]].PrintConsole( "v2", step2 );
			//			np->Set( 0.0f, 1.0f, 0.0f );
			tri += step3;
		}
#    endif
		//		glBindBuffer( GL_ARRAY_BUFFER, mNormalBuffer );
		//		glBufferData( GL_ARRAY_BUFFER, sizeof(f32 ) * Gr::Math::TXYZ * count, m_pNormalBuffer, GL_DYNAMIC_DRAW );
		//		glBufferData( GL_ARRAY_BUFFER, sizeof(f32 ) * Gr::Math::TXYZ * mNormals, m_pNormals, GL_DYNAMIC_DRAW );
		//		glNormalPointer( GL_FLOAT, 0, 0 );

		//　バッファをバインド
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer );
		//　頂点配列のフォーマットの指定
		glInterleavedArrays( GL_N3F_V3F, 0, NULL );

		//　描画
		glDrawElements( GL_TRIANGLES, GetTrianglesCount( ) * 3 /* sizeof(s32 )*/, GL_UNSIGNED_INT, NULL );

		//　バインドしたものをもどす
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

		//		glDisableClientState( GL_NORMAL_ARRAY );
		//		glDisableClientState( GL_VERTEX_ARRAY );
#endif
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void gfl::qt::Model::RenderSkinPolygon( gfl::qt::Parts* parts, gfl::core::Vector* skin_vert, const Render::PolygonColor col, const Render::ShadingType type )
{
	glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, COLOR_TABLE[col] );
	if( type == gfl::qt::Render::SHD_WIRE ){
		parts->RenderPolygonWireFrame( skin_vert );
	} else {
		parts->RenderPolygon( skin_vert );
	}
}
void gfl::qt::Model::RenderSkin( gfl::motion::MotionBase* mot, const Render::PolygonColor col, const Render::ShadingType type, const size_t ofs )
{
	s32 skin_count = GetSkinCount( );
	Parts* parts = GetParts( );
	Skin* skin = GetSkin( );

	for( s32 i = 0; i < skin_count; ++i, ++skin, ++parts ){
		gfl::core::Matrix* inv = skin->GetInvBaseMatrix( );
		//		gfl::core::Vector *skin_vert_org = DbgNew gfl::core::Vector [parts->GetVerticesCount( )];

		// スキン計算
		f32* weights = skin->GetWeights( );
		s32 loop = skin->GetIndexLoop( );

		gfl::core::Vector *skin_vert_org = skin->GetVertex( );
		if( !skin_vert_org ){
			skin_vert_org = DbgNew gfl::core::Vector [loop];
			skin->SetVertex( skin_vert_org );
		}
		s32* idx_count = skin->GetIndexCount( );
		s32* idx = skin->GetIndex( );
		gfl::core::Vector* vert = parts->GetVertices( );
		gfl::core::Vector* skin_vert = skin_vert_org;

		if( !skin->GetBoneIdIndex( ) ){
			const s32 size = skin->GetNameList( )->size( );
			s32* idp = DbgNew s32 [size];
			skin->SetBoneIdIndex( idp );
			for( s32 i = 0; i < size; ++i ){
				gfl::motion::JointBase* jnt = mot->GetJointByName<gfl::motion::JointBase*>( skin->GetNameList( )->at( i ).toAscii( ).data( ) );
				if( jnt ){
					*idp++ = jnt->_Id;
				} else {
					m_ErrorMessage = "Skin Error : " + skin->GetNameList( )->at( i ) + "にスキンが割り当てられています";
					*idp++ = 0;
				}
			}
		}

		for( s32 j = 0; j < loop; ++j, ++idx_count, ++vert, ++skin_vert ){
			skin_vert->SetZero( );
			for( s32 k = 0; k < *idx_count; ++k ){
				s32 id = *idx++;
				gfl::core::Vector v;
				//				gfl::motion::JointBase* col_jnt = mot->GetJoint<gfl::motion::JointBase*>( GetBoneIdToJointId( skin->GetBoneIdIndex( id ) ) );
				//				gfl::motion::JointBase* col_jnt = mot->GetJointByName<gfl::motion::JointBase*>( skin->GetNameList()->at(id).toAscii().data() );
				gfl::motion::JointBase* col_jnt = mot->GetJoint<gfl::motion::JointBase*>( skin->GetBoneIdIndex( id ) );
				if( col_jnt ){
//					inv[id].PrintConsole(col_jnt->_pName);
					//					( col_jnt->_Matrix * inv[id] ).Trans( &v, *vert );
					( ( *GFL_GET_POINTER_BY_OFFSET( col_jnt, gfl::core::Matrix, ofs ) ) * inv[id] ).Trans( &v, *vert );
				}
				*skin_vert += v * weights[*idx++];
				//				*skin_vert += *vert * weights[*idx++];
			}
		}
		RenderSkinPolygon( parts, skin_vert_org, col, type );
	}
}
//==============================================================================
//==============================================================================
void gfl::qt::Model::RenderNoSkin( gfl::motion::MotionBase* mot, const Render::PolygonColor col, const Render::ShadingType type, const size_t ofs )
{
	Parts* parts = GetParts( );
	for( s32 i = 0; i < GetPartsCount( ); ++i, ++parts ){
		gfl::motion::JointBase* jnt = mot->GetJointByName<gfl::motion::JointBase*>( parts->GetMeshName( ).toAscii( ).data( ) );

		if( jnt ){
			glPushMatrix( );
			glMultMatrixf( ( f32* )GFL_GET_POINTER_BY_OFFSET( jnt, gfl::core::Matrix, ofs ) );
			RenderSkinPolygon( parts, parts->GetVertices( ), col, type );
			glPopMatrix( );
		}
	}
}
