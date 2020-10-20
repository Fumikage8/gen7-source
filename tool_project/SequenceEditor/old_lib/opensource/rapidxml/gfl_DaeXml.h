//==============================================================================
/**
 * @file	gfl_DaeXml.h
 * @brief	
 * @author	kimura_shin
 * @data	2011/11/04, 14:35
 */
// =============================================================================

#ifndef GFL_DAEXML_H
#define	GFL_DAEXML_H

#include "gfl_Xml.h"

namespace gfl { namespace xml {

class Dae {
public:

	static inline s32 SetIntegerBySpaceSeparatedData( s32* data, const c8* text ){
		return gfl::core::Utility::SscanfInteger(data,text);
	}
	static inline s32 SetFloatBySpaceSeparatedData( f32* data, const c8* text, const f32 mul ){
		return gfl::core::Utility::SscanfFloat(data,text,mul);
	}

	// parts
	static gfl::core::Vector3* SetArray( gfl::xml::Node* c_node, s32* data_count, const f32 coe );
	enum {
		MODE_TRIANGLE,
		MODE_POLYGON
	};
	static void SetIndecies( gfl::qt::Parts* parts, gfl::xml::Node* c_node, const s32 mode );

	// skin
	static bool CheckName( gfl::xml::Node* elem, const c8 * const name );
	static void SetWeight( gfl::qt::Skin* skin, gfl::xml::Node* p_node );
	static void SetVertexWeight( gfl::qt::Skin* skin, gfl::xml::Node* p_node );

	// model
	static bool LoadModel(gfl::qt::Model* model, const QString& fname );
	static s32 GetCount( gfl::xml::Node* p_node, const c8 * const name );
	static void SetParts( gfl::qt::Model* model, gfl::xml::Node* p_node );
	static void SetSkin( gfl::qt::Model* model, gfl::xml::Node* p_node );
	static void SetSkinId( gfl::qt::Model* model, gfl::xml::Node * p_node );
	static void SetSkinIdRecursively(gfl::qt::Model* model, gfl::xml::Node * p_node );
};

} }

#endif	/* GFL_DAEXML_H */

