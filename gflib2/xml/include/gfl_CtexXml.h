//==============================================================================
/**
 * @file	gfl_CtexXml.h
 * @brief	
 * @author	kimura_shin
 * @data	2012/03/22, 14:30
 */
// =============================================================================

#if !defined( __GFL_CTEXXML_H__ )
#define	__GFL_CTEXXML_H__

#define _WINDOWS

#include <gflib.h>
#include "gfl_Xml.h"

namespace gfl { namespace xml {

/**
 * Ctex
 */
class Ctex {
private:
	static u8* GetCtexEtc1Image(gfl::xml::Node* i_node, s32* w, s32* h, const bool upsidedown=true);

	template <typename T> static void SetCtexTexture(gfl::cmdl::Model* model, gfl::xml::Node* i_node, s32 gl_fmt, s32 gl_type, gfl::cmdl::Material::TextureMapper* tm);
	static void SetCtexEtc1Texture(gfl::cmdl::Model* model, gfl::xml::Node* i_node, gfl::cmdl::Material::TextureMapper* tm);
	template <u32> static QImage GetCtexQImage(gfl::cmdl::Model* model, gfl::xml::Node* i_node, QImage::Format fmt);
	template <typename T> static QImage GetCtexQImage(gfl::cmdl::Model* model, gfl::xml::Node* i_node, QImage::Format fmt);
	static QImage GetCtexEtc1QImage(gfl::cmdl::Model* model, gfl::xml::Node* i_node, QImage::Format fmt);

	static s32 GetWrap( const c8* const str );
	static c8* GetDummyImage(gfl::xml::Node* i_node, s32* w=0, s32* h=0);

public:
	template <typename T>
	static T* GetCtexImage( gfl::xml::Node* i_node, s32* w, s32* h, const bool upsidedown=true )
	{
		T* img_ctex = ( T* )gfl::core::Utility::DecodeBase64( i_node->FirstNode( "Images" )
														 ->FirstNode( "PixelBasedImageCtr" )->value( ) );
		*w = i_node->AttributeInteger( "Width" );
		*h = i_node->AttributeInteger( "Height" );
		gfl::qt::Ctex::FromCtex( img_ctex, *w, *h, upsidedown );
		return img_ctex;
	}

	static bool LoadToTexture(gfl::cmdl::Model* model, gfl::cmdl::Material::TextureMapper* tm, const QString& fname,gfl::xml::Node* s_node);
	static u8* LoadToQImage( gfl::cmdl::Model* model, QPixmap* pm, const QString& fname );
};

} }

#endif	/* __GFL_CTEXXML_H__ */

