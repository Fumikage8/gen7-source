//==============================================================================

#include "../include/gfl_CtexXml.h"
#include "../include/gfl_CmdlXml.h"

/**
 * @file	gfl_CtexXml.cpp
 * @brief	
 * @author	kimura_shin
 * @data	2012/03/22, 14:32
 */

// =============================================================================

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
s32 gfl::xml::Ctex::GetWrap( const c8* str )
{
	if( 0 == strcmp( "Repeat", str ) ){
		return GL_REPEAT;
	} else if( 0 == strcmp( "MirroredRepeat", str ) ){
		return GL_MIRRORED_REPEAT;
	} else if( 0 == strcmp( "Clamp", str ) ){
		return GL_CLAMP;
	}
	return 0;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
template <typename T>
void gfl::xml::Ctex::SetCtexTexture(gfl::cmdl::Model* model, gfl::xml::Node* i_node, s32 gl_fmt, s32 gl_type, gfl::cmdl::Material::TextureMapper* tm)
{
	s32 w, h;
	T* img_ctex = GetCtexImage<T > ( i_node, &w, &h );
	// いらないかもしれないけど取っておく
	model->SetImageData(w,h,(c8*)img_ctex,gl_fmt,gl_type,tm);
	glTexImage2D( GL_TEXTURE_2D, 0, gl_fmt, w, h, 0, gl_fmt, gl_type, (c8*)img_ctex );
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void gfl::xml::Ctex::SetCtexTextureL8(gfl::cmdl::Model* model, gfl::xml::Node* i_node, s32 gl_fmt, s32 gl_type, gfl::cmdl::Material::TextureMapper* tm)
{
	s32 w, h;
	c8* img_ctex = GetCtexImage<c8> ( i_node, &w, &h );

	const s32 size = w*h;
	c8* img_rgb = DbgNew c8 [size*3]; // rgb
	for( s32 i = 0, j = 0; i < size; ++i ){
		const u8 c = (const u8)img_ctex[i];
		img_rgb[j++] = c;
		img_rgb[j++] = c;
		img_rgb[j++] = c;
	}
	// いらないかもしれないけど取っておく
	model->SetImageData(w,h,img_rgb,gl_fmt,gl_type,tm);
	glTexImage2D( GL_TEXTURE_2D, 0, gl_fmt, w, h, 0, gl_fmt, gl_type, img_rgb );
	GFL_DELETE [] img_ctex;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void gfl::xml::Ctex::SetCtexTextureLa8(gfl::cmdl::Model* model, gfl::xml::Node* i_node, s32 gl_fmt, s32 gl_type, gfl::cmdl::Material::TextureMapper* tm)
{
	s32 w, h;
	c8* img_ctex = (c8*)GetCtexImage<u16> ( i_node, &w, &h );

	const s32 size = w*h;
	c8* img_rgb = DbgNew c8 [size<<2]; // rgba
	for( s32 i = 0, j = 0, k = 0; i < size; ++i ){
		const u8 c = (const u8)img_ctex[k++];
		img_rgb[j++] = c;
		img_rgb[j++] = c;
		img_rgb[j++] = c;
		img_rgb[j++] = img_ctex[k++];
	}
	// いらないかもしれないけど取っておく
	model->SetImageData(w,h,img_rgb,gl_fmt,gl_type,tm);
	glTexImage2D( GL_TEXTURE_2D, 0, gl_fmt, w, h, 0, gl_fmt, gl_type, img_rgb );
	GFL_DELETE [] img_ctex;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
u8* gfl::xml::Ctex::GetCtexEtc1Image( gfl::xml::Node* i_node, s32* w, s32* h, const bool upsidedown )
{
	u8* img_ctex = ( u8* )gfl::core::Utility::DecodeBase64( i_node->FirstNode( "Images" )
														 ->FirstNode( "PixelBasedImageCtr" )->value( ) );
	*w = i_node->AttributeInteger( "Width" );
	*h = i_node->AttributeInteger( "Height" );
	u8* ret = gfl::qt::Ctex::FromEtc1( (u64*)img_ctex, *w, *h, upsidedown );
	delete [] img_ctex;
	return ret;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
void gfl::xml::Ctex::SetCtexEtc1Texture(gfl::cmdl::Model* model, gfl::xml::Node* i_node, gfl::cmdl::Material::TextureMapper* tm)
{
	s32 w, h;
	u8* img_ctex = GetCtexEtc1Image( i_node, &w, &h );
	// いらないかもしれないけど取っておく
	model->SetImageData(w,h,(c8*)img_ctex,GL_RGB,GL_UNSIGNED_BYTE,tm);
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, (c8*)img_ctex );
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
c8* gfl::xml::Ctex::GetDummyImage(gfl::xml::Node* i_node, s32* wp, s32* hp)
{
	s32 w = i_node->AttributeInteger("Width");
	if(wp){
		*wp = w;
	}
	s32 h = i_node->AttributeInteger("Height");
	if(hp){
		*hp = h;
	}
	const s32 size = w * h;
	u8* img_ctex = DbgNew u8 [size*3];
	for( s32 i = 0; i < size*3; ++i ){
		img_ctex[i] = 0x40;
	}
	return (c8*)img_ctex;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
bool gfl::xml::Ctex::LoadToTexture(gfl::cmdl::Model* model, gfl::cmdl::Material::TextureMapper* tm, const QString& fname, gfl::xml::Node* s_node)
{
	gfl::xml::Document doc;
	if( doc.Load( fname ) ){
		gfl::xml::Node* i_node = doc.RootNode( )->FirstNode( "GraphicsContentCtr" )
			->FirstNode( "Textures" )->FirstNode( "ImageTextureCtr" );

		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
		glGenTextures( 1, tm->GetTextureId() );
		glBindTexture( GL_TEXTURE_2D, *tm->GetTextureId() );
		u32 type = ( 0 == strcmp( "Nearest", s_node->AttributeString( "MagFilter" ) ) ) ? GL_NEAREST : GL_LINEAR;
		tm->SetMagFilter(type);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, type);
		type = ( 0 == strcmp( "Nearest", s_node->AttributeString( "MinFilter" ) ) ) ? GL_NEAREST : GL_LINEAR ;
		tm->SetMinFilter(type);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, type);
		type = GetWrap( s_node->AttributeString( "WrapS" ));
		tm->SetWrapS(type);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, type );
		type = GetWrap( s_node->AttributeString( "WrapT" ));
		tm->SetWrapT(type);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, type );
		tm->m_MinLod = s_node->AttributeInteger("MinLod");
		tm->m_LodBias = s_node->AttributeInteger("LodBias");
		tm->m_BorderColor.Set( gfl::xml::Cmdl::GetRGBA( s_node->FirstNode("BorderColor") ) );

		c8* fmt = i_node->AttributeString( "Format" );
		if( 0 == strcmp( "Rgba8", fmt ) ){
			SetCtexTexture<u32 > ( model, i_node, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, tm );
		} else if( 0 == strcmp( "Rgb565", fmt ) ){
			SetCtexTexture<u16 > ( model, i_node, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, tm );
		} else if( 0 == strcmp( "Rgb8", fmt ) ){
			SetCtexTexture<gfl::qt::Ctex::RGB_u8> ( model, i_node, GL_RGB, GL_UNSIGNED_BYTE, tm );
		} else if( 0 == strcmp( "L8", fmt ) ){
			SetCtexTextureL8( model, i_node, GL_RGB, GL_UNSIGNED_BYTE, tm );
		} else if( 0 == strcmp( "La8", fmt ) ){
			SetCtexTextureLa8( model, i_node, GL_RGBA, GL_UNSIGNED_BYTE, tm );
		} else {
			SetCtexEtc1Texture( model, i_node, tm );
		}
		return true;
	}
	QMessageBox::critical( 0, "Error", QString( "テクスチャーファイルのロードに失敗しました。\n" ) + fname );
	return false;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
template <u32>
QImage gfl::xml::Ctex::GetCtexQImage(gfl::cmdl::Model* model, gfl::xml::Node* i_node, QImage::Format fmt)
{
	s32 w, h;
	T* img_ctex = GetCtexImage<T > ( i_node, &w, &h, false );

	// RGBA -> ARGB
	const s32 size = w*h;
	T* imgp = img_ctex;
	for( s32 i = 0; i < size; ++i ){
		T img = *imgp;
		*imgp++ = ( ( img & 0x000000ff ) << 24 ) | ( ( img & 0xffffff00 ) >> 8 );
	}
	model->SetImageData(w,h,(c8*)img_ctex, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8);
	QImage img( ( u8* )img_ctex, w, h, fmt );
	return img;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
template <typename T>
QImage gfl::xml::Ctex::GetCtexQImage(gfl::cmdl::Model* model, gfl::xml::Node* i_node, QImage::Format fmt)
{
	s32 w, h;
	T* img_ctex = GetCtexImage<T > ( i_node, &w, &h, false );
	model->SetImageData(w,h,(c8*)img_ctex, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8);
	QImage img( ( u8* )img_ctex, w, h, fmt );
	return img;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
QImage gfl::xml::Ctex::GetCtexQImageL8(gfl::cmdl::Model* model, gfl::xml::Node* i_node, QImage::Format fmt)
{
	s32 w, h;
	c8* img_ctex = GetCtexImage<c8> ( i_node, &w, &h, false );
	const s32 size = w*h;
	c8* img_rgb = DbgNew c8 [size*3]; // rgb
	for( s32 i = 0, j = 0; i < size; ++i ){
		const u8 c = (const u8)img_ctex[i];
		img_rgb[j++] = c;
		img_rgb[j++] = c;
		img_rgb[j++] = c;
	}
	model->SetImageData(w,h,img_rgb, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8);
	QImage img( ( u8* )img_rgb, w, h, fmt );
	GFL_DELETE [] img_ctex;
	return img;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
QImage gfl::xml::Ctex::GetCtexQImageLa8(gfl::cmdl::Model* model, gfl::xml::Node* i_node, QImage::Format fmt)
{
	s32 w, h;
	c8* img_ctex = (c8*)GetCtexImage<u16> ( i_node, &w, &h, false );
	const s32 size = w*h;
	c8* img_rgb = DbgNew c8 [size<<2]; // rgba
	for( s32 i = 0, j = 0, k = 0; i < size; ++i ){
		const u8 c = (const u8)img_ctex[k++];
		img_rgb[j++] = c;
		img_rgb[j++] = c;
		img_rgb[j++] = c;
		img_rgb[j++] = img_ctex[k++];
	}
	model->SetImageData(w,h,img_rgb, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8);
	QImage img( ( u8* )img_rgb, w, h, fmt );
	GFL_DELETE [] img_ctex;
	return img;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
QImage gfl::xml::Ctex::GetCtexEtc1QImage(gfl::cmdl::Model* model, gfl::xml::Node* i_node, QImage::Format fmt)
{
	s32 w, h;
	u8* img_ctex = GetCtexEtc1Image( i_node, &w, &h, false );

	model->SetImageData(w,h,(c8*)img_ctex, GL_RGB, GL_UNSIGNED_BYTE);
	QImage img( img_ctex, w, h, fmt );
	return img;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
u8* gfl::xml::Ctex::LoadToQImage( gfl::cmdl::Model* model, QPixmap* pm, const QString& fname )
{
	gfl::xml::Document doc;
	if( doc.Load( fname ) ){
		gfl::xml::Node* i_node = doc.RootNode( )->FirstNode( "GraphicsContentCtr" )
			->FirstNode( "Textures" )->FirstNode( "ImageTextureCtr" );

		c8* fmt = i_node->AttributeString( "Format" );
		QImage img;
		if( 0 == strcmp( "Rgba8", fmt ) ){
			img = GetCtexQImage<u32 > ( model, i_node, QImage::Format_ARGB32 );
		} else if( 0 == strcmp( "Rgb565", fmt ) ){
			img = GetCtexQImage<u16 > ( model, i_node, QImage::Format_RGB16 );
		} else if( 0 == strcmp( "Rgb8", fmt ) ){
			img = GetCtexQImage<gfl::qt::Ctex::RGB_u8> ( model, i_node, QImage::Format_RGB888 );
		} else if( 0 == strcmp( "L8", fmt ) ){
			img = GetCtexQImageL8( model, i_node, QImage::Format_RGB888 );
		} else if( 0 == strcmp( "La8", fmt ) ){
			img = GetCtexQImageLa8( model, i_node, QImage::Format_ARGB32 );
		} else {
			img = GetCtexEtc1QImage( model, i_node, QImage::Format_RGB888 );
		}

		*pm = QPixmap::fromImage( img );
		return img.bits();
	}
//	QMessageBox::critical( 0, "Error", QString( "テクスチャーファイルのロードに失敗しました。\n" ) + fname );
	return 0;
}
