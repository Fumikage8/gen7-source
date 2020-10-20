//==============================================================================
/**
 * @file	gfl_CmdlXml.cpp
 * @brief	
 * @author	kimura_shin
 * @data	2011/11/04, 14:35
 */
// =============================================================================

#include "../include/gfl_CmdlXml.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void gfl::xml::Cmdl::SetIndecies( gfl::cmdl::Shape* shape, gfl::xml::Node* c_node )
{
	s32 count = c_node->AttributeInteger( "VertexSize" );
	s32 attr_loop, alloc_size, data_count;
	const s32 step = GetVectorSize( c_node );
	if( !count ){ // ストリームデータでない場合
		attr_loop = count = *shape->GetVertexCountPointer( );
		data_count = step;
		alloc_size = data_count * attr_loop;
	} else {
		attr_loop = 0;
		data_count = count * step;
		alloc_size = data_count;
	}

	s32* data = DbgNew s32 [alloc_size];

	shape->SetIndexStep( step );
	shape->SetBoneIndexCount( count );
	shape->SetBoneIdIndex( data );

	s32 ret = SetIntegerBySpaceSeparatedData( data, c_node->value( ) );
	GFL_ASSERT_MSG( data_count == ret, "データサイズ(%d %d)が一致しません。", data_count, ret );

	// ストリームデータでない場合　うめなのです
	if( attr_loop ){
		s32* ume_data = &data[step];
		for( s32 i = 1; i < attr_loop; ++i ){
			for( s32 j = 0; j < step; ++j ){
				*ume_data++ = data[j];
			}
		}
	}
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void gfl::xml::Cmdl::SetColors( gfl::cmdl::Shape* shape, gfl::xml::Node* c_node )
{
	s32 count = c_node->AttributeInteger( "VertexSize" );
	s32 attr_loop, alloc_size, data_count;
	const s32 step = GetVectorSize( c_node );
	if( !count ){ // ストリームデータでない場合
#if 1 // 
		attr_loop = count = *shape->GetVertexCountPointer( );
		data_count = step;
		alloc_size = data_count * attr_loop;
#else // 
		return;
#endif
	} else {
		attr_loop = 0;
		data_count = count * step;
		alloc_size = data_count;
	}

	u8* data = DbgNew u8 [alloc_size];

	shape->SetColorStep( step );
//	shape->SetColorCount( count );
	shape->SetColor( data );
	shape->SetColorScale( c_node->AttributeFloat("Scale") );

	s32 ret = SetIntegerBySpaceSeparatedData( data, c_node->value( ) );
	GFL_ASSERT_MSG( data_count == ret, "データサイズ(%d %d)が一致しません。", data_count, ret );
#if 1 // 
	// ストリームデータでない場合　うめなのです
	if( attr_loop ){
		u8* ume_data = &data[step];
		for( s32 i = 1; i < attr_loop; ++i ){
			for( s32 j = 0; j < step; ++j ){
				*ume_data++ = data[j];
			}
		}
	}
#endif
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void gfl::xml::Cmdl::SetWeight( gfl::cmdl::Shape* skin, gfl::xml::Node* p_node )
{
	s32 count = p_node->AttributeInteger( "VertexSize" );
	s32 attr_loop, alloc_size, data_count;
	const s32 step = GetVectorSize( p_node );
	if( !count ){ // ストリームデータでない場合
		attr_loop = *skin->GetVertexCountPointer( );
		data_count = step;
		alloc_size = data_count * attr_loop;
	} else {
		attr_loop = 0;
		data_count = count * step;
		alloc_size = data_count;
	}

	const f32 scl = p_node->AttributeFloat( "Scale", 1.0f );
	f32* data = DbgNew f32 [alloc_size];
	skin->SetWeights( data );
	s32 ret = SetFloatBySpaceSeparatedData( data, p_node->value( ), scl );
	GFL_ASSERT_MSG( data_count == ret, "データサイズ(%d %d)が一致しません。", data_count, ret );

	// ストリームデータでない場合　うめなのです
	// ストリームデータでない場合　うめなのです
	if( attr_loop ){
		f32* ume_data = &data[step];
		for( s32 i = 1; i < attr_loop; ++i ){
			for( s32 j = 0; j < step; ++j ){
				*ume_data++ = data[j];
			}
		}
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
gfl::core::Vector* gfl::xml::Cmdl::SetArray( gfl::xml::Node* c_node, s32* data_count, const f32 coe )
{
	s32 count = c_node->AttributeInteger( "VertexSize",1 );

	*data_count = count;
	gfl::core::Vector* data = DbgNew gfl::core::Vector [*data_count];
	// cmdl は改行(しかもcrlf!)からデータが始まるので2から。　アフォ？
	s32 ret = SetFloatBySpaceSeparatedData( ( f32* )data, c_node->FirstNode( )->value( ), coe );
	ret /= ( sizeof(gfl::core::Vector3 ) / sizeof(f32 ) );
	GFL_ASSERT_MSG( count == ret, "Shape error %d != %d\n", count, ret );
	return data;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
gfl::core::Vector2* gfl::xml::Cmdl::SetArray2( gfl::xml::Node* c_node, s32* data_count, const f32 coe )
{
	s32 count = c_node->AttributeInteger( "VertexSize" );

	*data_count = count;
	gfl::core::Vector2* data = DbgNew gfl::core::Vector2 [*data_count];
	// cmdl は改行(しかもcrlf!)からデータが始まるので2から。　アフォ？
	s32 ret = SetFloatBySpaceSeparatedData( ( f32* )data, c_node->FirstNode( )->value( ), coe );
	ret /= ( sizeof(gfl::core::Vector2 ) / sizeof(f32 ) );
	GFL_ASSERT_MSG( count == ret, "Shape error %d != %d\n", count, ret );
	return data;
}

// =============================================================================
// gfl::cmdl::Model
// =============================================================================

//==============================================================================
// Shape
//==============================================================================
void gfl::xml::Cmdl::SetShape( gfl::cmdl::Model* model, const gfl::xml::Node* p_node )
{
	s32 cnt = 0;
	for( gfl::xml::Node* node = p_node->FirstNode( "SeparateDataShapeCtr" ); node; node = node->NextSibling( "SeparateDataShapeCtr" ) ){
		++cnt;
	}
	model->SetShapeCount( cnt );
	gfl::cmdl::Shape* shape = DbgNew gfl::cmdl::Shape [cnt];
	model->SetShape( shape );
	const size_t texlen = strlen("TextureCoordinate");
	for( gfl::xml::Node* node = p_node->FirstNode( "SeparateDataShapeCtr" ); node; node = node->NextSibling( "SeparateDataShapeCtr" ), ++shape ){
		//		model->SetIdMap( c_node->AttributeString("id"), shape );
		
		// Bounding Box
		gfl::xml::Node* bb_node = node->FirstNode( "OrientedBoundingBox" );
		if(bb_node){
			gfl::xml::Node* pos_node = bb_node->FirstNode( "CenterPosition" );
			gfl::xml::Node* rot_node = bb_node->FirstNode( "OrientationMatrix" );
			shape->m_BoundingBox.m_Matrix.Set( rot_node->AttributeFloat("M00"),rot_node->AttributeFloat("M01"),rot_node->AttributeFloat("M02"),0.0f,
											 rot_node->AttributeFloat("M10"),rot_node->AttributeFloat("M11"),rot_node->AttributeFloat("M12"),0.0f,
											 rot_node->AttributeFloat("M20"),rot_node->AttributeFloat("M21"),rot_node->AttributeFloat("M22"),0.0f,
											 pos_node->AttributeFloat("X"),pos_node->AttributeFloat("Y"),pos_node->AttributeFloat("Z"),1.0f
			);
			pos_node = bb_node->FirstNode("Size");
			shape->m_BoundingBox.m_Size.Set( pos_node->AttributeFloat("X"),pos_node->AttributeFloat("Y"),pos_node->AttributeFloat("Z") );
		}

		// Primive Sets
		gfl::xml::Node* ps_node = node->FirstNode( "PrimitiveSets" );
		s32 p_cnt = 0;
		for( gfl::xml::Node* c_node = ps_node->FirstNode( "PrimitiveSetCtr" ); c_node; c_node = c_node->NextSibling( "PrimitiveSetCtr" ) ){
			++p_cnt;
		}
		shape->SetPrimitiveCount( p_cnt );
		gfl::cmdl::Shape::Primitive* pri = DbgNew gfl::cmdl::Shape::Primitive [p_cnt];
		shape->SetPrimitives( pri );
		for( gfl::xml::Node* c_node = ps_node->FirstNode( "PrimitiveSetCtr" ); c_node; c_node = c_node->NextSibling( "PrimitiveSetCtr" ), ++pri ){
			// Bone Index
			gfl::xml::Node* i_node = c_node->FirstNode( "BoneIndexTable" );
			if(i_node){
				s32* vp = DbgNew s32 [i_node->value_size( )]; // 大きめ
				pri->SetBoneIndexTable( vp );
				pri->SetBoneIndexTableCount( SetIntegerBySpaceSeparatedData( vp, i_node->value( ) ) );
			}
			// Primitive Index
			i_node = c_node->FirstNode( "Primitives" )->FirstNode( "PrimitiveCtr" )->FirstNode( "IndexStreams" )->FirstNode( );
			if(i_node){
				const s32 data_count = i_node->AttributeInteger( "Size" );
				pri->SetTriangleCount( data_count / 3 ); // triangle
				s32* vp = DbgNew s32 [data_count];
				pri->SetTriangles( vp );
				s32 ret = SetIntegerBySpaceSeparatedData( vp, i_node->value( ) );
				GFL_ASSERT_MSG( data_count == ret, "データサイズ(%d %d)が一致しません。", data_count, ret );
			}
		}

		// VertexAttributes
		for( gfl::xml::Node* c_node = node->FirstNode( "VertexAttributes" )->FirstNode( ); c_node; c_node = c_node->NextSibling( ) ){
			const c8 * const attr = c_node->AttributeString( "Usage" );
			if( 0 == strcmp( "Position", attr ) ){
				shape->SetVertices( SetArray( c_node, shape->GetVertexCountPointer( ), 1.0f ) );
			} else if( 0 == strcmp( "Normal", attr ) ){
				shape->SetNormals( SetArray( c_node, shape->GetNormalCountPointer( ), 1.0f ) );
			} else if( 0 == strcmp( "Tangent", attr ) ){
				shape->SetTangents( SetArray( c_node, shape->GetTangentCountPointer( ), 1.0f ) );
			} else if( 0 == strcmp( "BoneIndex", attr ) ){
				SetIndecies( shape, c_node );
			} else if( 0 == strcmp( "Color", attr ) ){
				SetColors( shape, c_node );
			} else if( 0 == strcmp( "BoneWeight", attr ) ){
				SetWeight( shape, c_node );
			} else if( 0 == strncmp( "TextureCoordinate", attr, texlen ) ){
				s32 id = attr[texlen]-'0';
				shape->SetTextures( id, SetArray2( c_node, shape->GetTextureCountPointer( id ), 1.0f ) );
			}
		}
		
#if 0//def USE_VERTEX_BUFFER
//		glGenBuffers( 1, &shape->m_VertexBuffer );
//		glBindBuffer( GL_ARRAY_BUFFER, shape->m_VertexBuffer );
//		glBufferData( GL_ARRAY_BUFFER, shape->GetVertexCount( ) * sizeof(gfl::core::Vector ), shape->GetVertices( ), GL_STATIC_DRAW );
//		glVertexPointer(3,GL_FLOAT,0,shape->GetVertices( ));

#    if 0 // InCase when need to swap data
		// * 3 means triangle edge count
		s32* i_data;
		shape->m_pIndexBuffer = i_data = DbgNew s32 [alloc_size];
		s32 i, j;
#        if 0
		for( i = j = 0; j < data_count; ++i, j += step ){
			i_data[i] = data[j];
			//				gfl::core::Debug::PrintConsole( "[%d] -> %d %f\n", i,data[j], mpVertices[data[j]].GetY( ) );
		}
#        else
		for( i = j = 0; j < data_count; i += 3, j += step * 3 ){
			i_data[i] = data[j + ( step * 2 )];
			i_data[i + 1] = data[j + step];
			i_data[i + 2] = data[j];
			//				gfl::core::Debug::PrintConsole( "[%d] -> %d %f\n", i,data[j], mpVertices[data[j]].GetY( ) );
		}
#        endif

		glGenBuffers( 1, &shape->m_IndexBuffer );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, shape->m_IndexBuffer );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(s32 ) * count * 3, m_pIndexBuffer, GL_STATIC_DRAW );
#    else // 
//		glGenBuffers( 1, &shape->m_IndexBuffer );
//		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, shape->m_IndexBuffer );
//		glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(s32 ) * shape->GetIndexCount() * shape->GetIndexStep(), shape->GetIndex( ), GL_STATIC_DRAW );
		
#    endif
#endif
	}
}

//------------------------------------------------------------------------------
// RGBA を取得
//------------------------------------------------------------------------------
gfl::core::Vector4 gfl::xml::Cmdl::GetRGBA( gfl::xml::Node* c_node )
{
	return gfl::core::Vector4(
							 c_node->AttributeFloat( "R" ),
							 c_node->AttributeFloat( "G" ),
							 c_node->AttributeFloat( "B" ),
							 c_node->AttributeFloat( "A" )
							 );
}

//------------------------------------------------------------------------------
// XYZ を取得
//------------------------------------------------------------------------------
gfl::core::Vector gfl::xml::Cmdl::GetXYZ( gfl::xml::Node* c_node )
{
	return gfl::core::Vector(
							 c_node->AttributeFloat( "X" ),
							 c_node->AttributeFloat( "Y" ),
							 c_node->AttributeFloat( "Z" )
							 );
}

//------------------------------------------------------------------------------
// sampler
//------------------------------------------------------------------------------
void gfl::xml::Cmdl::SetSampler( gfl::cmdl::Material::Shader::FragmentShader::FragmentLightingTable::ReflectanceSampler* smp, gfl::xml::Node* c_node )
{
	smp->m_IsAbs = c_node->AttributeBool( "IsAbs" );
	smp->m_Input = c_node->AttributeString( "Input" );
	smp->m_Scale = c_node->AttributeString( "Scale" );

	gfl::xml::Node* cc_node = c_node->FirstNode( "ReferenceLookupTableCtr" );
	if( cc_node ){
		gfl::cmdl::Material::Shader::FragmentShader::FragmentLightingTable::ReferenceLookupTable* rt
			= DbgNew gfl::cmdl::Material::Shader::FragmentShader::FragmentLightingTable::ReferenceLookupTable;
		smp->m_pReferenceLookupTable = rt;

		rt->m_TableName = new QString;
		*rt->m_TableName = cc_node->AttributeString( "TableName" );
		QString cont( cc_node->FirstNode( "LookupTableSetContentReferenceCtr" )->value( ) );
		rt->m_LookupTableSetContent = new QString;
		*rt->m_LookupTableSetContent = cont.section( '"', 1, 1 );
	}
}

//------------------------------------------------------------------------------
// texture combiner
//------------------------------------------------------------------------------
void gfl::xml::Cmdl::SetSource( gfl::cmdl::Material::Shader::FragmentShader::TextureCombiner::Source* tc, gfl::xml::Node* c_node )
{
	tc->m_Source0 = c_node->AttributeString( "Source0" );
	tc->m_Source1 = c_node->AttributeString( "Source1" );
	tc->m_Source2 = c_node->AttributeString( "Source2" );
}

//------------------------------------------------------------------------------
// texture combiner
//------------------------------------------------------------------------------
void gfl::xml::Cmdl::SetOperand( gfl::cmdl::Material::Shader::FragmentShader::TextureCombiner::Operand* tc, gfl::xml::Node* c_node )
{
	tc->m_Operand0 = c_node->AttributeString( "Operand0" );
	tc->m_Operand1 = c_node->AttributeString( "Operand1" );
	tc->m_Operand2 = c_node->AttributeString( "Operand2" );
}

//------------------------------------------------------------------------------
// texture combiner
//------------------------------------------------------------------------------
void gfl::xml::Cmdl::SetBlend( gfl::cmdl::Material::Shader::FragmentOperation::BlendOperation::BlendParameter* tc, gfl::xml::Node* c_node )
{
	tc->m_BlendFunctionSource = c_node->AttributeString( "BlendFunctionSource" );
	tc->m_BlendFunctionDestination = c_node->AttributeString( "BlendFunctionDestination" );
	tc->m_BlendEquation = c_node->AttributeString( "BlendEquation" );
}

//==============================================================================
// Material
//==============================================================================
bool gfl::xml::Cmdl::SetMaterial( gfl::cmdl::Model* model, const gfl::xml::Node* p_node )
{
	s32 cnt = 0;
	for( gfl::xml::Node* node = p_node->FirstNode( "MaterialCtr" ); node; node = node->NextSibling( "MaterialCtr" ) ){
		++cnt;
	}
	model->SetMaterialCount( cnt );
	gfl::cmdl::Material* mate = DbgNew gfl::cmdl::Material [cnt];
	model->SetMaterial( mate );
	for( gfl::xml::Node* node = p_node->FirstNode( "MaterialCtr" ); node; node = node->NextSibling( "MaterialCtr" ), ++mate ){
		if( 0 == strcmp( node->AttributeString("Name"), "ParticleShape" )){
			continue;
		}
		// Shader
		gfl::xml::Node* sh_node = node->FirstNode( "ShaderReference" );
		if( sh_node ){
			gfl::cmdl::Material::Shader* shader = DbgNew gfl::cmdl::Material::Shader;
			mate->SetShader( shader );

			QString name( sh_node->value( ) );
			shader->m_Name = name.section( '"', 1, 1 );
			shader->m_FileName = name.section( ':', 1 );

			sh_node = node->FirstNode( "FragmentShader" );
			gfl::cmdl::Material::Shader::FragmentShader* fs = &shader->m_FragmentShader;
			gfl::xml::Node* fs_node = sh_node->FirstNode( "BufferColor" );
			if(fs_node){
				fs->m_BufferColor.Set( fs_node->AttributeFloat( "R" ), fs_node->AttributeFloat( "G" )
				, fs_node->AttributeFloat( "B" ), fs_node->AttributeFloat( "A" ) );
			}
			fs_node = sh_node->FirstNode( "FragmentBump" );
			if(fs_node){
				fs->m_FragmentBump.m_BumpTextureIndex = fs_node->AttributeString( "BumpTextureIndex" );
				fs->m_FragmentBump.m_BumpMode = fs_node->AttributeString( "BumpMode" );
				fs->m_FragmentBump.m_IsBumpRenormalize = fs_node->AttributeBool( "IsBumpRenormalize" );
			}
			fs_node = sh_node->FirstNode( "FragmentLighting" );
			if(fs_node){
				gfl::cmdl::Material::Shader::FragmentShader::FragmentLighting* fl = &fs->m_FragmentLighting;
				fl->m_FresnelConfig = fs_node->AttributeString( "FresnelConfig" );
				fl->m_IsClampHighLight = fs_node->AttributeBool( "IsClampHighLight" );
				fl->m_IsDistribution0Enabled = fs_node->AttributeBool( "IsDistribution0Enabled" );
				fl->m_IsDistribution1Enabled = fs_node->AttributeBool( "IsDistribution1Enabled" );
				fl->m_IsGeometricFactor0Enabled = fs_node->AttributeBool( "IsGeometricFactor0Enabled" );
				fl->m_IsGeometricFactor1Enabled = fs_node->AttributeBool( "IsGeometricFactor1Enabled" );
				fl->m_IsReflectionEnabled = fs_node->AttributeBool( "IsReflectionEnabled" );
			}
			fs_node = sh_node->FirstNode( "FragmentLightingTable" );
			if(fs_node){
				gfl::cmdl::Material::Shader::FragmentShader::FragmentLightingTable* flt = &fs->m_FragmentLightingTable;
				SetSampler( &flt->m_ReflectanceRSampler, fs_node->FirstNode( "ReflectanceRSampler" ) );
				SetSampler( &flt->m_ReflectanceGSampler, fs_node->FirstNode( "ReflectanceGSampler" ) );
				SetSampler( &flt->m_ReflectanceBSampler, fs_node->FirstNode( "ReflectanceBSampler" ) );
				SetSampler( &flt->m_Distribution0Sampler, fs_node->FirstNode( "Distribution0Sampler" ) );
				SetSampler( &flt->m_Distribution1Sampler, fs_node->FirstNode( "Distribution1Sampler" ) );
				SetSampler( &flt->m_FresnelSampler, fs_node->FirstNode( "FresnelSampler" ) );
			}
			for( fs_node = sh_node->FirstNode( "TextureCombiners" )->FirstNode( "TextureCombinerCtr" ); fs_node; fs_node = fs_node->NextSibling( "TextureCombinerCtr" ) ){
				gfl::cmdl::Material::Shader::FragmentShader::TextureCombiner* tc = DbgNew gfl::cmdl::Material::Shader::FragmentShader::TextureCombiner;
				fs->m_TextureConbiner.append( tc );

				tc->m_CombineRgb = fs_node->AttributeString( "CombineRgb" );
				tc->m_CombineAlpha = fs_node->AttributeString( "CombineAlpha" );
				tc->m_ScaleRgb = fs_node->AttributeString( "ScaleRgb" );
				tc->m_ScaleAlpha = fs_node->AttributeString( "ScaleAlpha" );
				tc->m_Constant = fs_node->AttributeString( "Constant" );
				tc->m_BufferInputRgb = fs_node->AttributeString( "BufferInputRgb" );
				tc->m_BufferInputAlpha = fs_node->AttributeString( "BufferInputAlpha" );

				SetSource( &tc->m_SourceRgb, fs_node->FirstNode( "SourceRgb" ) );
				SetOperand( &tc->m_OperandRgb, fs_node->FirstNode( "OperandRgb" ) );
				SetSource( &tc->m_SourceAlpha, fs_node->FirstNode( "SourceAlpha" ) );
				SetOperand( &tc->m_OperandAlpha, fs_node->FirstNode( "OperandAlpha" ) );
			}

			sh_node = node->FirstNode( "FragmentOperation" );
			gfl::cmdl::Material::Shader::FragmentOperation* fo = &shader->m_FragmentOperation;

			fs_node = sh_node->FirstNode( "DepthOperation" );
			if(fs_node){
				gfl::cmdl::Material::Shader::FragmentOperation::DepthOperation* deo = &fo->m_DepthOperation;
				deo->m_IsTestEnabled = fs_node->AttributeBool( "IsTestEnabled" );
				deo->m_IsMaskEnabled = fs_node->AttributeBool( "IsMaskEnabled" );
				deo->m_TestFunction = fs_node->AttributeString( "TestFunction" );
			}
			fs_node = sh_node->FirstNode( "BlendOperation" );
			if(fs_node){
				gfl::cmdl::Material::Shader::FragmentOperation::BlendOperation* blo = &fo->m_BlendOperation;
				blo->m_Mode = fs_node->AttributeString( "Mode" );
				blo->m_LogicOperation = fs_node->AttributeString( "LogicOperation" );
				SetBlend( &blo->m_RgbParameter, fs_node->FirstNode( "RgbParameter" ) );
				SetBlend( &blo->m_AlphaParameter, fs_node->FirstNode( "AlphaParameter" ) );
				fs_node = fs_node->FirstNode( "BlendColor" );
				blo->m_BlendColor.Set( fs_node->AttributeFloat( "R" ), fs_node->AttributeFloat( "G" ), fs_node->AttributeFloat( "B" ), fs_node->AttributeFloat( "A" ) );
			}
			fs_node = sh_node->FirstNode( "StencilOperation" );
			if(fs_node){
				gfl::cmdl::Material::Shader::FragmentOperation::StencilOperation* sto = &fo->m_StencilOperation;
				sto->m_IsTestEnabled = fs_node->AttributeBool( "IsTestEnabled" );
				sto->m_TestFunction = fs_node->AttributeString( "TestFunction" );
				sto->m_TestReference = fs_node->AttributeInteger( "TestReference" );
				sto->m_TestMask = fs_node->AttributeInteger( "TestMask" );
				sto->m_FailOperation = fs_node->AttributeString( "FailOperation" );
				sto->m_ZFailOperation = fs_node->AttributeString( "ZFailOperation" );
				sto->m_PassOperation = fs_node->AttributeString( "PassOperation" );
			}
			sh_node = node->FirstNode( "UserData" );
			if( sh_node ){
				for( sh_node = sh_node->FirstNode( ); sh_node; sh_node = sh_node->NextSibling( ) ){
					shader->m_UserData.insert( sh_node->FirstNode( "Key" )->value( )
											 , DbgNew gfl::cmdl::Material::Shader::UserData( sh_node->FirstNode( "Values" )->FirstNode( )->name( )
																						  , sh_node->FirstNode( "Values" )->FirstNode( )->value( ) ) );
				}
			}
		}

		mate->SetName( node->AttributeString( "Name" ) );
		mate->m_IsCompressible = node->AttributeBool( "IsCompressible" );
		mate->m_LightSetIndex = node->AttributeInteger( "LightSetIndex" );
		mate->m_FogIndex = node->AttributeInteger( "FogIndex" );
		mate->m_IsFragmentLightEnabled = node->AttributeBool( "IsFragmentLightEnabled" );
		mate->m_IsVertexLightEnabled = node->AttributeBool( "IsVertexLightEnabled" );
		mate->m_IsHemiSphereLightEnabled = node->AttributeBool( "IsHemiSphereLightEnabled" );
		mate->m_IsHemiSphereOcclusionEnabled = node->AttributeBool( "IsHemiSphereOcclusionEnabled" );
		mate->m_IsFogEnabled = node->AttributeBool( "IsFogEnabled" );
		mate->m_TextureCoordinateConfig = node->AttributeString( "TextureCoordinateConfig" );
		mate->m_TranslucencyKind = node->AttributeString( "TranslucencyKind" );
		mate->m_ShaderProgramDescriptionIndex = node->AttributeInteger( "ShaderProgramDescriptionIndex" );
		mate->m_ShaderBinaryKind = node->AttributeString( "ShaderBinaryKind" );

		gfl::xml::Node* rs_node = node->FirstNode( "Rasterization" );
		if(rs_node){
			mate->m_Rasterization.m_CullingMode = rs_node->AttributeString( "CullingMode" );
			mate->m_Rasterization.m_IsPolygonOffsetEnabled = rs_node->AttributeBool( "IsPolygonOffsetEnabled" );
			mate->m_Rasterization.m_PolygonOffsetUnit = rs_node->AttributeInteger( "PolygonOffsetUnit" );
		}
		gfl::cmdl::Material::MaterialColor* mc = DbgNew gfl::cmdl::Material::MaterialColor;
		mate->SetMaterialColor( mc );
		gfl::xml::Node* mc_node = node->FirstNode( "MaterialColor" );
		if(mc_node){
			mc->m_VertexColorScale = mc_node->AttributeInteger( "VertexColorScale" );
			mc->SetEmission( GetRGBA( mc_node->FirstNode( "Emission" ) ) );
			mc->SetAmbient( GetRGBA( mc_node->FirstNode( "Ambient" ) ) );
			mc->SetDiffuse( GetRGBA( mc_node->FirstNode( "Diffuse" ) ) );
			for( s32 i = 0; i < gfl::cmdl::Material::MaterialColor::SPECULAR_MAX; ++i ){
				mc->SetSpecular( i, GetRGBA( mc_node->FirstNode( "Specular" + QString::number( i ) ) ) );
			}
			for( s32 i = 0; i < gfl::cmdl::Material::MaterialColor::CONSTANT_MAX; ++i ){
				mc->SetConstant( i, GetRGBA( mc_node->FirstNode( "Constant" + QString::number( i ) ) ) );
			}
		}

		gfl::xml::Node* tc_node = node->FirstNode( "TextureCoordinators" );
		s32 p_cnt = 0;
		for( gfl::xml::Node* c_node = tc_node->FirstNode( "TextureCoordinatorCtr" ); c_node; c_node = c_node->NextSibling( "TextureCoordinatorCtr" ) ){
			++p_cnt;
		}
		mate->SetTextureCoordinatorCount( p_cnt );
		gfl::cmdl::Material::TextureCoordinator* tc = DbgNew gfl::cmdl::Material::TextureCoordinator [p_cnt];
		mate->SetTextureCoordinator( tc );
		for( tc_node = tc_node->FirstNode( "TextureCoordinatorCtr" ); tc_node; tc_node = tc_node->NextSibling( "TextureCoordinatorCtr" ), ++tc ){
			tc->SetSourceCoordinate( tc_node->AttributeInteger( "SourceCoordinate" ) );
			tc->SetMappingMethod( tc_node->AttributeString( "MappingMethod" ) );
			tc->SetReferenceCamera( tc_node->AttributeInteger( "ReferenceCamera" ) );
			tc->SetMatrixMode( tc_node->AttributeString( "MatrixMode" ) );
			tc->SetScaleS( tc_node->AttributeFloat( "ScaleS" ) );
			tc->SetScaleT( tc_node->AttributeFloat( "ScaleT" ) );
			tc->SetRotate( tc_node->AttributeFloat( "Rotate" ) );
			tc->SetTranslateS( tc_node->AttributeFloat( "TranslateS" ) );
			tc->SetTranslateT( tc_node->AttributeFloat( "TranslateT" ) );
		}

		gfl::xml::Node* tm_node = node->FirstNode( "TextureMappers" );
		p_cnt = 0;
		for( gfl::xml::Node* c_node = tm_node->FirstNode( "PixelBasedTextureMapperCtr" ); c_node; c_node = c_node->NextSibling( "PixelBasedTextureMapperCtr" ) ){
			++p_cnt;
		}

		mate->SetTextureMapperCount( p_cnt );
		gfl::cmdl::Material::TextureMapper* tm = DbgNew gfl::cmdl::Material::TextureMapper [p_cnt];
		mate->SetTextureMapper( tm );
		for( gfl::xml::Node* c_node = tm_node->FirstNode( "PixelBasedTextureMapperCtr" ); c_node; c_node = c_node->NextSibling( "PixelBasedTextureMapperCtr" ), ++tm ){
			QString str( c_node->FirstNode( "TextureReference" )->value( ) );
			tm->SetName( str.section( '"', 1, 1 ) );

			// load texture file
			QString texfname( str.section( ':', 1 ) );
			QList<gfl::cmdl::Model::TextureTabInfo>* tt = model->GetTextureTabInfo( );
#if 0 // remove duplicate
			bool exist = false;
			for( s32 i = 0; i < tt->size( ); ++i ){
				if( tt->at( i ).GetFileName( ) == texfname ){
					exist = true;
					break;
				}
			}
			if( !exist ){
				tt->append( gfl::cmdl::Model::TextureTabInfo( texfname, mate ) );
			}
#else // 
			tt->append( gfl::cmdl::Model::TextureTabInfo( texfname, mate ) );
#endif
			tm->SetFileName( texfname );
			if( !gfl::xml::Ctex::LoadToTexture( model, tm, texfname
											 , c_node->FirstNode( "StandardTextureSamplerCtr" ) ) ){
				return false;
			}
		}

		//解析情報を元に頂点シェーダー＆ピクセルシェーダー作成
#ifdef MAYA_INPORT
		mate->glCreateShader( );
#endif
	}
	return true;
}

//==============================================================================
// Mesh
//==============================================================================
bool gfl::xml::Cmdl::SetMesh( gfl::cmdl::Model* model, const gfl::xml::Node* p_node )
{
	s32 cnt = 0;
	for( gfl::xml::Node* node = p_node->FirstNode( "Mesh" ); node; node = node->NextSibling( "Mesh" ) ){
		++cnt;
	}
	gfl::cmdl::Mesh* mesh = DbgNew gfl::cmdl::Mesh [cnt];
	model->SetMeshCount( cnt );
	gfl::cmdl::Shape* shape = model->GetShape( );
	model->SetMesh( mesh );
	for( gfl::xml::Node* node = p_node->FirstNode( "Mesh" ); node; node = node->NextSibling( "Mesh" ), ++mesh ){
		// Shape
		QString str( node->FirstNode( "SeparateShapeReference" )->value( ) );
		mesh->SetShape( &shape[str.section( '[', 1 ).section( ']', 0, 0 ).toInt( )] );

		// Material
		str = QString( node->FirstNode( "MaterialReference" )->value( ) ).section( '"', 1, 1 );
		gfl::cmdl::Material* mate = model->GetMaterial( );
		for( s32 i = 0; i < model->GetMaterialCount( ); ++i, ++mate ){
			if( str == mate->GetName( ) ){
				mesh->SetMaterial( mate );
				break;
			}
		}
	}
	return true;
}

//------------------------------------------------------------------------------
// Bone
//------------------------------------------------------------------------------
bool gfl::xml::Cmdl::SetBone( gfl::cmdl::Model* model, const gfl::xml::Node* p_node )
{
	s32 bone_cnt = 0;
	for( gfl::xml::Node* node = p_node->FirstNode( "Bone" ); node; node = node->NextSibling( "Bone" ) ){
		++bone_cnt;
	}
	model->SetBoneCount( bone_cnt );
	gfl::cmdl::Bone* bone = DbgNew gfl::cmdl::Bone [bone_cnt];
	gfl::cmdl::Bone* root_bone = bone;
	model->SetBone( bone );
	for( gfl::xml::Node* node = p_node->FirstNode( "Bone" ); node; node = node->NextSibling( "Bone" ), ++bone ){
		bone->SetName( node->AttributeString( "Name" ) );
		bone->SetParentName( node->AttributeString( "ParentBoneName" ) );

		gfl::xml::Node* c_node = node->FirstNode( "Transform" );
		bone->SetScale( GetXYZ( c_node->FirstNode( "Scale" ) ) );
		bone->SetRotate( GetXYZ( c_node->FirstNode( "Rotate" ) ) );
		bone->SetTranslate( GetXYZ( c_node->FirstNode( "Translate" ) ) );

		gfl::core::Matrix* mat = bone->GetInvMatrix( );
		if( !bone->GetParentName( ).isEmpty( ) ){
			gfl::cmdl::Bone* p_bone = bone - 1;
			for(; p_bone != root_bone; --p_bone ){
				if( p_bone->GetName( ) == bone->GetParentName( ) ){
					break;
				}
			}
			//			bone->GetScale().PrintConsole(bone->GetName().toAscii().data());
			mat->Set( *p_bone->GetInvMatrix( ) );
#if 1 // 
			mat->Trans( bone->GetTranslate( ) );
			mat->RotateZYX( bone->GetRotate( ) );
			mat->Scale( bone->GetScale( ) );
#else // 
			mat->Scale( bone->GetScale( ) );
#    if 0 // 
			mat->RotateX( bone->GetRotate( ).GetX( ) );
			mat->RotateY( bone->GetRotate( ).GetY( ) );
			mat->RotateZ( bone->GetRotate( ).GetZ( ) );
#    else // 
			mat->RotateZYX( bone->GetRotate( ) );
#    endif
			mat->Trans( bone->GetTranslate( ) );
#endif
		} else { // root
			mat->SetUnit( );
#if 1 // 
			mat->SetTrans( bone->GetTranslate( ) );
			mat->SetRotateZYX( bone->GetRotate( ) );
			mat->Scale( bone->GetScale( ) );
#else // 
			mat->SetScale( bone->GetScale( ) );
#    if 0 // 
			mat->RotateX( bone->GetRotate( ).GetX( ) );
			mat->RotateY( bone->GetRotate( ).GetY( ) );
			mat->RotateZ( bone->GetRotate( ).GetZ( ) );
#    else // 
			mat->RotateZYX( bone->GetRotate( ) );
#    endif
			mat->SetTrans( bone->GetTranslate( ) );
#endif
		}
		//		mat->PrintConsole(bone->GetName().toAscii().data());
	}

	bone = root_bone;
	for( s32 i = 0; i < bone_cnt; ++i, ++bone ){
		bone->GetInvMatrix( )->Inverse( );
		//		bone->GetInvMatrix()->PrintConsole(bone->GetName().toAscii().data());
	}

	return true;
}

#if 0 // あてにならん
//==============================================================================
//==============================================================================
bool gfl::xml::Cmdl::SetMetaData( gfl::cmdl::Model* model, gfl::xml::Node* p_node )
{
	p_node = p_node->FirstNode( "ContentSummaryMetaData" )->FirstNode( "Values" )->FirstNode( "ContentSummary" )
		->FirstNode( "ObjectSummaries" )->FirstNode( "ObjectSummary" )->FirstNode( "Notes" )->FirstNode( );
	for(; p_node; p_node = p_node->NextSibling( ) ){
		const c8 * const name = p_node->AttributeString( "Name" );
		if( 0 == strcmp( name, "ShapeCount" ) ){
			model->SetShapeCount( p_node->AttributeInteger( "Value" ) );
		} else if( 0 == strcmp( name, "MaterialCount" ) ){
			model->SetMaterialCount( p_node->AttributeInteger( "Value" ) );
		} else if( 0 == strcmp( name, "MeshCount" ) ){
			model->SetMeshCount( p_node->AttributeInteger( "Value" ) );
		} else if( 0 == strcmp( name, "BoneCount" ) ){
			model->SetBoneCount( p_node->AttributeInteger( "Value" ) );
		}
	}
	return true;
}
#endif

//==============================================================================
//==============================================================================
gfl::xml::Node* gfl::xml::Cmdl::LoadFile( gfl::xml::Document* xml, const QString& fname )
{
	if( fname.at( 1 ) == ':' || fname.at( 0 ) == '/' ){
		QString rootdir( fname.section( '/', 0, -2 ) );
		QDir::setCurrent( rootdir );
		gfl::core::File::s_RootDir = rootdir + '/';
	}
	if( !xml->Load( fname ) ){
		QMessageBox::critical( 0, "Error", "ファイルがありません。\n" + fname );
		return 0;
	}

	gfl::xml::Node* p_node = xml->RootNode( );
	//	GFL_ASSERT( p_node, "モデル %s のロードに失敗しました。", fname.toAscii().data() );
	if( !p_node ){
		QMessageBox::critical( 0, "Error", "モデルファイルのロードに失敗しました。\n" + fname );
		return 0;
	}
	p_node = p_node->FirstNode( "GraphicsContentCtr" );
	if( !p_node ){
		QMessageBox::critical( 0, "Error", "モデルファイルが壊れています。\n" + fname );
		return 0;
	}

	gfl::xml::Node* bgn_node;
	bgn_node = p_node->FirstNode( "Models" )->FirstNode( "SkeletalModel" );
	// スケルトンがなくても良し
	if( !bgn_node ){
		bgn_node = p_node->FirstNode( "Models" )->FirstNode( "Model" );
//		QMessageBox::critical( 0, "Error", "モデルファイルにスケルトンモデルがありません。\n" + fname );
//		return 0;
	}
	return bgn_node;
}

//------------------------------------------------------------------------------
// lookup table
//------------------------------------------------------------------------------
bool gfl::xml::Cmdl::SetLookupTable( gfl::cmdl::Model* model, const gfl::xml::Node* p_node )
{
	for( gfl::xml::Node* node = p_node->FirstNode( "ImageLookupTableCtr" ); node; node = node->NextSibling( "ImageLookupTableCtr" ) ){
		gfl::cmdl::LookupTable* lt = DbgNew gfl::cmdl::LookupTable;
		model->GetLookupTable( )->append( lt );

		lt->m_Name = node->AttributeString( "Name" );
		lt->m_IsGeneratedAsAbs = node->AttributeBool( "IsGeneratedAsAbs" );
		lt->m_IsSizeDoubled = node->AttributeBool( "IsSizeDoubled" );
		lt->m_IsMipMap = node->AttributeBool( "IsMipMap" );

		gfl::xml::Node* c_node = node->FirstNode( "CurveLookupTableCtr" )->FirstNode( "SegmentsFloatCurve" );
		lt->m_PreRepeatMethod = c_node->AttributeString( "PreRepeatMethod" );
		lt->m_PostRepeatMethod = c_node->AttributeString( "PostRepeatMethod" );
		//		s32 cnt = c_node->AttributeInteger("EndFrame") - c_node->AttributeInteger("StartFrame") + 1;

		c_node = c_node->FirstNode( "Segments" );

		gfl::xml::Node* s_node = c_node->FirstNode( "LinearFloatSegment" );
		if( s_node ){
			f32* val;
			const s32 cnt = 256;
			lt->m_pValue = val = DbgNew f32 [cnt];
			for( s_node = s_node->FirstNode( "Keys" )->FirstNode( "LinearFloatKey" ); s_node; s_node = s_node->NextSibling( "LinearFloatKey" ) ){
				*val++ = s_node->AttributeFloat( "Value" );
			}

			{//参照テーブルテクスチャ作成
				struct RGBA8{
					void Set( float value )
					{
						r = static_cast<unsigned char>(255.0f * value);
						g = b = a = r;
					}

					unsigned char  r, g, b, a;
				};

				static RGBA8			imageBuff[cnt];
				char*							p = (char*)imageBuff;
				for( int i = 0; i < cnt; ++i )
				{
					imageBuff[i].Set( lt->m_pValue[i] );
				}

				glGenTextures( 1, &lt->m_hLutTexture );
				glBindTexture( GL_TEXTURE_2D, lt->m_hLutTexture );
				glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, 256, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageBuff );
				glBindTexture( GL_TEXTURE_2D, 0 );//リセット
				glFinish();
			}
		} else {
			QMessageBox::critical( 0, "Error", "変なカーブだよ" );
			return false;
		}
	}
	return true;
}

//==============================================================================
//==============================================================================
bool gfl::xml::Cmdl::LoadModel( gfl::cmdl::Model* model, const QString& fname )
{
	gfl::xml::Document xml;
	gfl::xml::Node* bgn_node = LoadFile( &xml, fname );
	if( !bgn_node ){
		model->SetNoSkeletonMode(true);
//		return false;
	}

	SetShape( model, bgn_node->FirstNode( "Shapes" ) );
	if( !SetMaterial( model, bgn_node->FirstNode( "Materials" ) ) ){
		return false;
	}
	SetMesh( model, bgn_node->FirstNode( "Meshes" ) );
	SetBone( model, bgn_node->FirstNode( "Skeleton" )->FirstNode( "Bones" ) );
	gfl::xml::Node* p_node = xml.RootNode( )->FirstNode( "GraphicsContentCtr" )->FirstNode( "LookupTableSetContents" );
	if( p_node ){
		SetLookupTable( model, p_node->FirstNode( "LookupTableSetContentCtr" )->FirstNode( "LookupTableSet" )->FirstNode( "Samplers" ) );
	}
	return true;
}

//==============================================================================
//==============================================================================
bool gfl::xml::Cmdl::LoadBone( gfl::cmdl::Model* model, const QString& fname )
{
	gfl::xml::Document xml;
	gfl::xml::Node* bgn_node = LoadFile( &xml, fname );
	if( bgn_node ){
		SetBone( model, bgn_node->FirstNode( "Skeleton" )->FirstNode( "Bones" ) );
		return true;
	}
	return false;
}
