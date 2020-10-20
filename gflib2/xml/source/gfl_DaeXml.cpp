//==============================================================================
/**
 * @file	gfl_DaeXml.cpp
 * @brief	
 * @author	kimura_shin
 * @data	2011/11/04, 14:35
 */
// =============================================================================

#include "gfl_DaeXml.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void gfl::xml::Dae::SetIndecies(gfl::qt::Parts* parts, gfl::xml::Node* c_node, const s32 mode)
{
	s32 step = 0;
	s32 count = c_node->AttributeInteger( "count" );

	for(gfl::xml::Node* g_node = c_node->FirstNode( ); g_node; g_node = g_node->NextSibling( )){
		if( 0 == strcmp("input",g_node->name( )) ){
			++step;
		} else if( 0 == strcmp( "p", g_node->name( ) ) ){
			delete [] parts->GetTriangles();
			// * 3 means triangle edge count
			s32 seg = step * 3; // p の一データのサイズ　以前はわかれていなかった…
			s32 data_count = count * seg;
			s32* data = DbgNew s32 [data_count];

			parts->SetStep( step );
			parts->SetTriangles( count );
			parts->SetTriangles( data );

			if(mode == MODE_TRIANGLE){
				s32 ret = SetIntegerBySpaceSeparatedData( data, g_node->value() );
				GFL_ASSERT_MSG( data_count == ret, "データサイズ(%d %d)が一致しません。", data_count, ret );
			}else{ // Polygon
				for( s32 i = 0; i < count; ++i, g_node = g_node->NextSibling() ){
					s32 ret = SetIntegerBySpaceSeparatedData( data, g_node->value() );
					GFL_ASSERT_MSG( seg == ret, "データサイズ(%d %d)が一致しません。", data_count, ret );
					data += seg;
				}
			}
#ifdef USE_VERTEX_BUFFER
			// * 3 means triangle edge count
			s32* i_data;
			//mpIndexBuffer = i_data = DbgNew s32 [count * 3];
			i_data = DbgNew s32 [count * 3];
			s32 i, j;
#if 0
			for( i = j = 0; j < data_count; ++i, j += step ){
				i_data[i] = data[j];
//				gfl::core::Debug::PrintConsole( "[%d] -> %d %f\n", i,data[j], mpVertices[data[j]].GetY( ) );
			}
#else
			for( i = j = 0; j < data_count; i += 3, j += step * 3 ){
				i_data[i] = data[j + ( step * 2 )];
				i_data[i + 1] = data[j + step];
				i_data[i + 2] = data[j];
//				gfl::core::Debug::PrintConsole( "[%d] -> %d %f\n", i,data[j], mpVertices[data[j]].GetY( ) );
			}
#endif
#endif
			return;
		}
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

gfl::core::Vector3* gfl::xml::Dae::SetArray( gfl::xml::Node* c_node, s32* data_count, const f32 coe )
{
	gfl::xml::Node* g_node;
	g_node = c_node->FirstNode( "float_array" );
	if(!g_node){
		return 0;
	}
	s32 count = g_node->AttributeInteger( "count" );

	*data_count = count / (sizeof (gfl::core::Vector3) / sizeof (f32));
	gfl::core::Vector3* data = DbgNew gfl::core::Vector3 [*data_count];

	s32 ret = SetFloatBySpaceSeparatedData( (f32*)data, g_node->FirstNode( )->value(), coe );
	GFL_ASSERT_MSG( count == ret, "Parts error %d != %d\n", count, ret );
	return data;
}

// =============================================================================
// gfl::qt::Model
// =============================================================================
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

s32 gfl::xml::Dae::GetCount( gfl::xml::Node* p_node, const c8 * const name )
{
	s32 parts_count = 0;
	for(gfl::xml::Node* node = p_node->FirstNode( ); node; node = node->NextSibling( )){
		if( 0 == strcmp( name, node->name( ) ) ){
			++parts_count;
		}
	}

	return parts_count;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void gfl::xml::Dae::SetParts( gfl::qt::Model* model, gfl::xml::Node* p_node )
{
	static const c8 * const label_name = "geometry";
	s32 parts_count = GetCount( p_node, label_name );

	delete [] model->GetParts();
	model->SetParts( parts_count );
	gfl::qt::Parts* parts = DbgNew gfl::qt::Parts [parts_count];
	model->SetParts( parts );
//	s32 parts_count_test = 0;

	for(p_node = p_node->FirstNode( ); p_node; p_node = p_node->NextSibling( )){
		// Rigは取り除く 例外処理！
		if( 0 == strcmp( label_name, p_node->name( ) ) && 0 != strncmp("Rig", p_node->AttributeString("id"), 3) ){
			model->SetIdMap( p_node->AttributeString("id"), parts );
			for(gfl::xml::Node* node = p_node->FirstNode( ); node; node = node->NextSibling( )){
				if( 0 == strcmp( "mesh", node->name( ) ) ){
					gfl::xml::Node* c_node;
					for(c_node = node->FirstNode( ); c_node; c_node = c_node->NextSibling( )){
//						if( 0 == strcmp( "position", c_node->AttributeString ( "name" ) ) ){
						QString idstr(QString( c_node->AttributeString("id") ));
						if( idstr.endsWith("POSITION",Qt::CaseInsensitive) ){
							delete [] parts->GetVertices();
							s32 cnt;
							parts->SetVertices( SetArray( c_node, &cnt, 1.0f ) );
							parts->SetVertices( cnt );
//							gfl::core::Debug::PrintConsole( "vert %d\n", parts->mVertices );
//						} else if( 0 == strcmp( "normal", p_node->AttributeString ( "name" ) ) ){
						} else if( idstr.endsWith( "Normal0" ) ){
							delete [] parts->GetNormals();
							s32 cnt;
							parts->SetNormals( SetArray( c_node, &cnt, 1.0f ) );
							parts->SetNormals( cnt );
//							gfl::core::Debug::PrintConsole( "norm %d\n", parts->mNormals );
						} else if( 0 == strcmp( "triangles", c_node->name( ) )){
							SetIndecies( parts, c_node, MODE_TRIANGLE );
						} else if( 0 == strcmp( "polygons", c_node->name( ) )){
							SetIndecies( parts, c_node, MODE_POLYGON );
						}
					}
				}
			}
			++parts;
		}
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

bool gfl::xml::Dae::CheckName( gfl::xml::Node* p_node, const c8 * const name )
{
	const size_t name_len = strlen( name );

	const c8 * const id = p_node->AttributeString ( "id" );
	if( id && 0 == strncmp( name, &id[strlen( id ) - name_len], name_len ) ){
		return true;
	}
	return false;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void gfl::xml::Dae::SetWeight(gfl::qt::Skin* skin, gfl::xml::Node* p_node )
{
//	if( CheckName( p_node, "skin-joints" ) ){
	if( CheckName( p_node, "Joints" ) ){
		for(p_node = p_node->FirstNode( ); p_node; p_node = p_node->NextSibling( )){
			if( 0 == strcmp( "Name_array", p_node->name( ) ) ){
				s32 count = p_node->AttributeInteger( "count" );

//				delete [] skin->GetBoneIdIndex();
//				s32* data = DbgNew s32 [count];
//				skin->SetBoneIdIndex( data );
				skin->GetNameList()->clear();

				c8* ptr = const_cast<c8*> ( p_node->FirstNode( )->value() );
				c8 buf[0x20]; // IDはこれで収まるでしょ
				c8 c;
				c8* buf_ptr = buf;
//				s32 cnt = 0;
				for(;; ){
					c = *ptr++;
					if( c == ' ' || c == '\n' ){
						if(buf != buf_ptr){
							*buf_ptr = 0x00;
//							*data++ = atoi( &buf[4] ); // 4 size of "bone"
//							*data++ = s_JointIdTable.value(buf,0);
							QString name(buf);
							name.remove("Ref_");
							skin->GetNameList()->append(name);
//							cnt++;
							buf_ptr = buf;
						}
					} else {
						*buf_ptr++ = c;
					}
					if( !c ){
						if( buf != buf_ptr ){
							*buf_ptr = 0x00;
//							*data = atoi( &buf[4] );
//							*data++ = s_JointIdTable.value(buf,0);
							QString name(buf);
							name.remove("Ref_");
							skin->GetNameList()->append(name);
//							cnt++;
						}
						break;
					}
				}
//				GFL_ASSERT_MSG(cnt==count,"skin-joints unmatch %d != %d\n",cnt,count);
				break;
			}
		}
//	} else if( CheckName( p_node, "skin-bind_poses" ) ){
	} else if( CheckName( p_node, "Matrices" ) ){
		for(p_node = p_node->FirstNode( ); p_node; p_node = p_node->NextSibling( )){
			if( 0 == strcmp( "float_array", p_node->name( ) ) ){
				s32 data_count = p_node->AttributeInteger( "count" );
				s32 count = data_count / (sizeof (gfl::core::Matrix) / sizeof (f32));

				delete [] skin->GetInvBaseMatrix();
				gfl::core::Matrix* data = DbgNew gfl::core::Matrix [count];
				skin->SetInvBaseMatrix( data );

				s32 ret = SetFloatBySpaceSeparatedData( data->Get( ), p_node->FirstNode( )->value(), 1.0f );
				GFL_ASSERT_MSG( data_count == ret, "データサイズ(%d %d)が一致しません。", data_count, ret );

				for(s32 i = 0; i < count; ++i, ++data){
					data->Transpose( );
//					data->GetTransRefX( ) *= 0.01f;
//					data->GetTransRefY( ) *= 0.01f;
//					data->GetTransRefZ( ) *= 0.01f;
				}
				break;
			}
		}
//	} else if( CheckName( p_node, "skin-weights" ) ){
	} else if( CheckName( p_node, "Weights" ) ){
		for(p_node = p_node->FirstNode( ); p_node; p_node = p_node->NextSibling( )){
			if( 0 == strcmp( "float_array", p_node->name( ) ) ){
				s32 count = p_node->AttributeInteger( "count" );

				delete [] skin->GetWeights();
				f32* data = DbgNew f32 [count];
				skin->SetWeights( data );

				s32 ret = SetFloatBySpaceSeparatedData( data, p_node->FirstNode( )->value(), 1.0f );
				GFL_ASSERT_MSG( count == ret, "データサイズ(%d %d)が一致しません。", count, ret );
				break;
			}
		}
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void gfl::xml::Dae::SetVertexWeight(gfl::qt::Skin* skin, gfl::xml::Node* p_node)
{
	s32 step = 0;
	s32 count = p_node->AttributeInteger( "count" );
	s32 v_count = 0;

	for(p_node = p_node->FirstNode( ); p_node; p_node = p_node->NextSibling( )){
		if( 0 == strcmp( "input", p_node->name( ) ) ){
			++step;
		} else if( 0 == strcmp( "vcount", p_node->name( ) ) ){
			delete [] skin->GetIndexCount();
			s32* data = DbgNew s32 [count];
			skin->SetIndexCount( data );

			s32 ret = SetIntegerBySpaceSeparatedData( data, p_node->FirstNode( )->value() );
			GFL_ASSERT_MSG( count == ret, "データサイズ(%d %d)が一致しません。", count, ret );

			for(s32 i = 0; i < count; ++i){
				v_count += *data++;
			}

		} else if( 0 == strcmp( "v", p_node->name( ) ) ){
			delete [] skin->GetIndex();
			// * 3 means triangle edge count
			s32* data = DbgNew s32 [v_count * step];
			skin->SetIndex( data );

			skin->SetStep( step );
			skin->SetIndex( count );
			skin->SetIndex( data );

			s32 ret = SetIntegerBySpaceSeparatedData( data, p_node->FirstNode( )->value() );
			GFL_ASSERT_MSG( v_count * step == ret, "データサイズ(%d(%d*%d) %d)が一致しません。", v_count*step, v_count, step, ret );
		}
	}
}
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void gfl::xml::Dae::SetSkin(gfl::qt::Model* model, gfl::xml::Node* p_node)
{
	static const c8 * const label_name = "controller";
	s32 parts_count = GetCount( p_node, label_name );

	delete [] model->GetSkin();
	model->SetSkin( parts_count );
	gfl::qt::Skin* parts = DbgNew gfl::qt::Skin [parts_count];
	model->SetSkin(parts);

	for(p_node = p_node->FirstNode( ); p_node; p_node = p_node->NextSibling( )){
		if( 0 == strcmp( label_name, p_node->name( ) ) ){
			for(gfl::xml::Node* node = p_node->FirstNode( ); node; node = node->NextSibling( )){
				gfl::xml::Node* c_node;
//				if( 0 == strcmp( "skin", p_node->name( ) ) ){
				if( 0 == strcmp( "skin", node->name( ) ) ){
					for(c_node = node->FirstNode( ); c_node; c_node = c_node->NextSibling( )){
//						if( 0 == strcmp( "source", p_node->name( ) ) ){
						if( 0 == strcmp( "source", c_node->name( ) ) ){
							SetWeight( parts, c_node );
//						} else if( 0 == strcmp( "vertex_weights", p_node->name( ) ) ){
						} else if( 0 == strcmp( "vertex_weights", c_node->name( ) ) ){
							SetVertexWeight( parts, c_node );
						}
					}
				}
			}
			++parts;
		}
	}
}

void gfl::xml::Dae::SetSkinIdRecursively(gfl::qt::Model* model,gfl::xml::Node* p_node)
{
	for( p_node = p_node->FirstNode(); p_node; p_node = p_node->NextSibling( ) ){
		if( 0 == strcmp( "node", p_node->name( ) ) ){
#if 0 // 
			if( 0 == strcmp( "JOINT", p_node->AttributeString("type") )){
				model->GetNameList()->append(p_node->AttributeString("name"));
//				s_JointIdTable.insert(p_node->AttributeString("name"),++s_Id);
			}
#else //
			gfl::xml::Node* c_node = p_node->FirstNode("instance_geometry");
			if(c_node){
				QString id(c_node->AttributeString("url"));
				id.remove('#');
				gfl::qt::Parts* parts = model->GetIdMap(id);
				if(parts){
					parts->SetMeshName( p_node->AttributeString("id") );
				}
//				gfl::core::Debug::PrintConsole( "c_node %s %s\n",p_node->AttributeString("name"),c_node->AttributeString("url"));
			}
#endif
			SetSkinIdRecursively(model,p_node);
		}
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void gfl::xml::Dae::SetSkinId(gfl::qt::Model* model, gfl::xml::Node * p_node )
{
	// skip visual_scene
	gfl::xml::Node* bgn_node = p_node->FirstNode( )->FirstNode( );

#if 0 // 
	for( p_node = bgn_node; p_node; p_node = p_node->NextSibling( ) ){
		if( 0 == strcmp( "node", p_node->name( ) ) ){
			if( 0 == strcmp( "model", p_node->AttributeString ( "id" ) ) ){
				for( p_node = p_node->FirstNode( ); p_node; p_node = p_node->NextSibling( ) ){
					const c8* id = p_node->AttributeString ( "sid" );
					if( id && 0 == strncmp( id, "bone", 4 ) ){
						++count;
					}
				}
				break;
			}
		}
	}
#else // 
	for( p_node = bgn_node; p_node; p_node = p_node->NextSibling( ) ){
		SetSkinIdRecursively(model,p_node);
	}
	
#endif

//	GFL_ASSERT_MSG( s_Id ,"skin が bone に割り当てられていません" );
#if 0 // 
	// モデル番号からジョイント番号を参照するためのテーブル作成
	model->SetIdCount( s_Id );
	delete [] model->GetBoneIdToJointId();
	s32* data = DbgNew s32 [s_Id];
	model->SetBoneIdToJointId( data );

	for(p_node = bgn_node; p_node; p_node = p_node->NextSibling( )){
		if( 0 == strcmp( "node", p_node->name( ) ) ){
			if( 0 == strcmp( "model", p_node->AttributeString ( "id" ) ) ){
				for( p_node = p_node->FirstNode( ); p_node; p_node = p_node->NextSibling( ) ){
					const c8* id = p_node->AttributeString ( "sid" );
					if( id && 0 == strncmp( id, "bone", 4 ) ){
						s32 b_id = atoi( &id[4] ); // 4 = length of "bone"
						QString j_name( p_node->AttributeString ( "id" ) );
						s32 j_id = atoi( j_name.section( '_', -1 ).toAscii().data( ) );
						//							gfl::core::Debug::PrintConsole( "%s %d -> %d\n", j_name, b_id, j_id );
						data[b_id] = j_id;
					}
				}
				break;
			}
		}
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

bool gfl::xml::Dae::LoadModel(gfl::qt::Model* model, const QString& fname)
{
	gfl::xml::Document xml;
//	if( 0 == strcmp( ".gz", &fname[strlen( fname ) - 3] ) ){
//		gfl::core::Zlib zlib;
//		zlib.DecodeFile( fname );
//		xml.Parse( zlib.GetData( ) );
//	} else {
		xml.Load( fname );
//	}
//	gfl::core::Debug::PrintConsole( "%s\n", fname );
	gfl::xml::Node* p_node = xml.RootNode( );
	GFL_ASSERT( p_node, "モデル %s のロードに失敗しました。", fname.toAscii().data() );

	gfl::xml::Node* bgn_node;
	// Skip Colldada root
//	bgn_node = p_node->FirstNode("library_visual_scenes" );
//	GFL_ASSERT( bgn_node, "モデル %s のロードに失敗しました。\n階層構造がありません。", fname.toAscii().data() );
//	SetSkinId( model, bgn_node );
	
	bgn_node = p_node->FirstNode("library_geometries" );
	if(!bgn_node){
		//	GFL_ASSERT( bgn_node, "モデル %s のロードに失敗しました。\nジオメトリがありません。", fname.toAscii().data() );
		return false;
	}
	SetParts( model, bgn_node );
	
	bgn_node = p_node->FirstNode("library_controllers" );
	if(!bgn_node){
		bgn_node = p_node->FirstNode("library_visual_scenes" );
		SetSkinId( model, bgn_node );
		return true;
	}

	SetSkin( model, bgn_node );
	return true;
}
