//==============================================================================

#include "gfl_CsklaXml.h"
#include "gfl_Xml.h"

/**
 * @file	gfl_CsklaXml.cpp
 * @brief	
 * @author	kimura_shin
 * @data	2012/03/27, 15:37
 */
// =============================================================================

//------------------------------------------------------------------------------
// load
//------------------------------------------------------------------------------
bool gfl::xml::Cskla::Load( const QString& fname )
{
	if( !gfl::xml::Document::Load( fname ) ){
		return false;
	}

	gfl::xml::Node* node = RootNode( );
	if( !node ){
		return false;
	}
	node = node->FirstNode( "GraphicsContentCtr" );
	if( !node ){
		return false;
	}
	node = node->FirstNode( "Animations" );
	if( !node ){
		return false;
	}
	node = node->FirstNode( "SkeletalAnimationData" );
	if( !node ){
		return false;
	}

	m_FrameCount = node->AttributeInteger( "FrameSize" )+1; // +1 for afo

	gfl::xml::Node* a_node = node->FirstNode( "MemberAnimationDataSet" );
	if( !a_node ){
		return false;
	}

	for( gfl::xml::Node* c_node = a_node->FirstNode( "MemberAnimationData" ); c_node; c_node = c_node->NextSibling( "MemberAnimationData" ) ){
		JointAnim* ja = DbgNew JointAnim;
		m_JointAnim.append( ja );

		ja->m_Name = QString( c_node->FirstNode( "Path" )->value( ) ).section( '"', 1, 1 );
		ja->m_pJoint = 0;
		gfl::xml::Node* t_node = c_node->FirstNode( "TransformAnimation" );
		if( !t_node ){
			continue;
		}
		for( t_node = t_node->FirstNode( ); t_node; t_node = t_node->NextSibling( ) ){
			//			QString type( QString( t_node->name( ) ).section( "Vector", 0, 0 ) ); // これだと飛んじゃう!なんで？
			//			gfl::core::Debug::PrintConsole( "\t%s\n", type.toAscii( ).data( ) );
			for( gfl::xml::Node* v_node = t_node->FirstNode( ); v_node; v_node = v_node->NextSibling( ) ){
				c8 typestr[16];
				for( s32 i = 0; i < 16; ++i ){
					c8 c = t_node->name( )[i];
					if( c == 'V' ){
						typestr[i] = 0x00;
						break;
					} else {
						typestr[i] = c;
					}
				}

				s32 xyz = v_node->name( )[0] - 'X';

				f32* val = DbgNew f32 [m_FrameCount];
				if( 0 == strcmp( typestr, "Translate" ) ){
					ja->m_pTranslate[xyz] = val;
				} else if( 0 == strcmp( typestr, "Rotate" ) ){
					ja->m_pRotate[xyz] = val;
				} else if( 0 == strcmp( typestr, "Scale" ) ){
					ja->m_pScale[xyz] = val;
				}

				gfl::xml::Node* s_node = v_node->FirstNode( "Segments" );
				gfl::xml::Node* h_node = s_node->FirstNode( "HermiteFloatSegment" );
				if( h_node ){
					gfl::xml::Node* pre_node = h_node->FirstNode( "Keys" );
					pre_node = pre_node->FirstNode( );
					f64 val_f = 0.0;
					for( gfl::xml::Node* n_node = pre_node->NextSibling( ); n_node;
						pre_node = n_node, n_node = n_node->NextSibling( ) ){
						const f64 pre_num_f = pre_node->AttributeFloat( "Frame" );
						const f64 pre_val_f = pre_node->AttributeFloat( "Value" );
						const f64 pre_r_f = pre_node->AttributeFloat( "OutSlope" ) * gfl::motion::Fcurve::ONE_FRAME;
						const f64 num_f = n_node->AttributeFloat( "Frame" );
						val_f = n_node->AttributeFloat( "Value" );
						const f64 l_f = n_node->AttributeFloat( "InSlope" ) * gfl::motion::Fcurve::ONE_FRAME;

						for( f64 frame = pre_num_f; frame < num_f; frame += 1.0 ){
							register f64 t, t2, t3, p0X, pX, p3X;

							pX = frame * gfl::motion::Fcurve::ONE_FRAME_TIME;
							p0X = pre_num_f * gfl::motion::Fcurve::ONE_FRAME_TIME;
							p3X = num_f * gfl::motion::Fcurve::ONE_FRAME_TIME;

							t2 = pX - p0X;
							t = t2 / ( p3X - p0X );
							t3 = t - 1.0;
							*val++ = ( t * l_f + t3 * pre_r_f ) * t3 * t2 + t * t * ( ( t3 + t3 ) - 1.0 )*( pre_val_f - val_f ) + pre_val_f;
						}
					}
					*val = val_f;
				} else {
					h_node = s_node->FirstNode( "LinearFloatSegment" );
					if( h_node ){
						const f32 v = h_node->FirstNode( "Keys" )->FirstNode( "LinearFloatKey" )->AttributeFloat( "Value" );
						for( s32 i = 0; i < m_FrameCount; ++i ){
							*val++ = v;
						}
					}
				}
			}
		}
	}

	node = node->FirstNode( "Skeleton" );
	if( !node ){
		return false;
	}
	node = node->FirstNode( "Bones" );
	if( !node ){
		return false;
	}

	for( gfl::xml::Node* c_node = node->FirstNode( "Bone" ); c_node; c_node = c_node->NextSibling( "Bone" ) ){
		Joint* jnt = DbgNew Joint;
		m_Joint.append( jnt );
		jnt->m_Name = c_node->AttributeString( "Name" );
		jnt->m_ScaleCompensate = c_node->AttributeBool( "IsSegmentScaleCompensate" );

		// アニメーションジョイントを設定
		for( s32 i = 0; i < m_JointAnim.size( ); ++i ){
			JointAnim* ja = m_JointAnim.at( i );
			if( ja->m_Name == jnt->m_Name ){
				jnt->m_pJointAnim = ja;
				ja->m_pJoint = jnt;
				break;
			}
		}

		// 親ジョイントを設定
		QString pname( c_node->AttributeString( "ParentBoneName" ) );
		for( s32 i = 0; i < m_Joint.size( ); ++i ){
			if( m_Joint.at( i )->m_Name == pname ){
				jnt->m_pParentJoint = m_Joint.at( i );
				break;
			}
		}

		for( gfl::xml::Node* t_node = c_node->FirstNode( "Transform" )->FirstNode( ); t_node; t_node = t_node->NextSibling( ) ){
			QString type( t_node->name( ) );
			gfl::core::Vector v;
			v.SetX( t_node->AttributeFloat( "X" ) );
			v.SetY( t_node->AttributeFloat( "Y" ) );
			v.SetZ( t_node->AttributeFloat( "Z" ) );
			if( type == "Translate" ){
				jnt->m_Translate = v;
			} else if( type == "Rotate" ){
				jnt->m_Rotate = v;
			} else if( type == "Scale" ){
				jnt->m_Scale = v;
			}
		}
	}

	return true;
}
//------------------------------------------------------------------------------
// to model
//------------------------------------------------------------------------------
s32* gfl::xml::Cskla::CreateModelIdTable( gfl::cmdl::Model* model )
{
	gfl::cmdl::Bone* bp = model->GetBone( );
	const s32 cnt = model->GetBoneCount( );
	s32* id = DbgNew s32 [cnt];
	for( s32 i = 0; i < cnt; ++i ){
		QString bname( bp[i].GetName( ) );
		for( s32 j = 0; j < m_Joint.size( ); ++j ){
			if( bname == m_Joint.at( j )->m_Name ){
				id[i] = j;
				break;
			}
		}
	}
	return id;
}
#if 0 // 
//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void gfl::xml::Cskla::CalcMatrix(gfl::core::Matrix* matp, const f32 frame)
{
	for( s32 i = 0; i < GetJoint( )->size(); ++i, ++matp ){
		gfl::xml::Cskla::Joint* jnt = GetJoint( )->at( m_pId[i] );
		gfl::core::Matrix* mat = jnt->GetMatrix( );
		gfl::xml::Cskla::Joint* pjnt = jnt->GetParentJoint( );
		if( pjnt ){
			mat->Set( *pjnt->GetMatrix( ) );
		} else {
			mat->SetUnit( );
		}
		gfl::xml::Cskla::JointAnim* ja = jnt->GetJointAnim( );
		if( ja ){
			gfl::core::Vector v;
			for( s32 i = 0; i < gfl::core::TXYZ; ++i ){
				f32* vp = ja->GetTranslate( i );
				v.SetElem( i, vp ? vp[frame] : jnt->GetTranslate( ).Get( i ) );
			}
			mat->Trans( v );

			for( s32 i = 0; i < gfl::core::TXYZ; ++i ){
				f32* vp = ja->GetRotate( i );
				v.SetElem( i, vp ? vp[frame] : jnt->GetRotate( ).Get( i ) );
			}
			mat->RotateZYX( v );

			for( s32 i = 0; i < gfl::core::TXYZ; ++i ){
				f32* vp = ja->GetScale( i );
				v.SetElem( i, vp ? vp[frame] : jnt->GetScale( ).Get( i ) );
			}
			mat->Scale( v );
		} else {
			mat->Trans( jnt->GetTranslate( ) );
			mat->RotateZYX( jnt->GetRotate( ) );
			mat->Scale( jnt->GetScale( ) );
		}
		matp->Set(*mat);
//		mat->PrintConsole(jnt->GetName().toAscii( ).data( ),i);
	}
}
#endif
