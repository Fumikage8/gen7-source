//==============================================================================
/**
 * @file	gfl_CmdlXml.h
 * @brief	
 * @author	kimura_shin
 * @data	2011/11/04, 14:35
 */
// =============================================================================

#ifndef GFL_CmdlXML_H
#define	GFL_CmdlXML_H

#include "gfl_Xml.h"
#include "gfl_CtexXml.h"

namespace gfl { namespace xml {

class Cmdl {
private:

	template<typename T>
	static inline s32 SetIntegerBySpaceSeparatedData( T* data, c8* text ) {
		while(*text == '\r' || *text == '\n') {
			++text;
		}
		return gfl::core::Utility::SscanfInteger( data, text );
	}

	static inline s32 SetFloatBySpaceSeparatedData( f32* data, c8* text, const f32 mul ) {
		while(*text == '\r' || *text == '\n') {
			++text;
		}
		return gfl::core::Utility::SscanfFloat( data, text, mul );
	}

	static void SetSampler( gfl::cmdl::Material::Shader::FragmentShader::FragmentLightingTable::ReflectanceSampler* smp
						 , gfl::xml::Node* c_node );
	static void SetSource( gfl::cmdl::Material::Shader::FragmentShader::TextureCombiner::Source* tc
								 , gfl::xml::Node* c_node );
	static void SetOperand( gfl::cmdl::Material::Shader::FragmentShader::TextureCombiner::Operand* tc
								 , gfl::xml::Node* c_node );
	static void SetBlend( gfl::cmdl::Material::Shader::FragmentOperation::BlendOperation::BlendParameter* tc
								 , gfl::xml::Node* c_node );

	// parts
	static gfl::core::Vector* SetArray( gfl::xml::Node* c_node, s32* data_count, const f32 coe );
	static gfl::core::Vector2* SetArray2( gfl::xml::Node* c_node, s32* data_count, const f32 coe );
	static void SetIndecies( gfl::cmdl::Shape* parts, gfl::xml::Node* c_node );
	static void SetColors( gfl::cmdl::Shape* parts, gfl::xml::Node* c_node );

	static s32 GetVectorSize( gfl::xml::Node* c_node ) {
		return c_node->name( )[6] - '0'; // Vertex? ÇÃêîílÇéÊìæ
	}

	static gfl::core::Vector GetXYZ( gfl::xml::Node* c_node );

	// skin
	static void SetWeight( gfl::cmdl::Shape* skin, gfl::xml::Node* p_node );

	// model
	static void SetShape( gfl::cmdl::Model* model, const gfl::xml::Node* p_node );
	static bool SetMaterial( gfl::cmdl::Model* model, const gfl::xml::Node* p_node );
	static bool SetMesh( gfl::cmdl::Model* model, const gfl::xml::Node* p_node );
	static bool SetBone( gfl::cmdl::Model* model, const gfl::xml::Node* p_node );
	static bool SetLookupTable( gfl::cmdl::Model* model, const gfl::xml::Node* p_node );
	//	static bool SetMetaData( gfl::cmdl::Model* model, gfl::xml::Node* p_node );

	static gfl::xml::Node* LoadFile( gfl::xml::Document* xml, const QString& fname );
public:
	static gfl::core::Vector4 GetRGBA( gfl::xml::Node* c_node );
	static bool LoadModel( gfl::cmdl::Model* model, const QString& fname );
	static bool LoadBone( gfl::cmdl::Model* model, const QString& fname );
};

} }

#endif	/* GFL_CmdlXML_H */

