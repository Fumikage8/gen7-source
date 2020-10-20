//==============================================================================
/**
 * @file	gfl_Utility.cpp
 * @brief	
 * @author	kimura_shin
 * @data	2012/03/12, 16:34
 */
// =============================================================================

#ifdef GFL_PLATFORM_PC

// -----------------------------------------------------------------
// Scanf の不定形対応版
// -----------------------------------------------------------------
s32 gfl::core::Utility::SscanfFloat( f32* data, const c8* text, const f32 mul )
{
	c8* ptr = const_cast < c8* >( text );
	c8 buf[0x10]; // 浮動小数はこれで収まるでしょ
	c8 c;
	c8* buf_ptr = buf;
	s32 count = 0;
	for(;; ){
		c = *ptr++;
		if( c == ' ' || c == '\n' ){
			if(buf_ptr != buf){
				*buf_ptr = 0x00;
				// 文字列から浮動小数に変換して係数を掛ける
				*data++ = static_cast < f32 >( atof( buf ) ) * mul;
				count++;
				buf_ptr = buf;
			}
		}  else if( c != '\r') {
			*buf_ptr++ = c;
		}
		if( !c ){
			if( buf != buf_ptr && ( isdigit( buf[0] ) || buf[0] == '-' ) ){
				*buf_ptr = 0x00;
				// 文字列から浮動小数に変換して係数を掛ける
				*data = static_cast < f32 >( atof( buf ) ) * mul;
				count++;
			}
			break;
		}
	}
	return count;
}

const c8* const gfl::core::Utility::s_pBase64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
c8* gfl::core::Utility::DecodeBase64(const c8* src, u32* size)
{
	char b64[128];
	char *p, c[4], *buff;
	int i = 0, j;
	
	buff = DbgNew c8 [((strlen(src)*3)>>2) + 4];

	/* 変換テーブルの作成 */
	for(j = 0; j < 65; j++){
		b64[s_pBase64[j]] = j & 63;
	}
	p = const_cast<c8*>(src);
	while(*p){
		/* 4文字ずつ変換 */
		for(j = 0; j < 4; j++){
			c[j] = b64[*(p++)];
		}
		for(j = 0; j < 3; j++){
			buff[i++] = c[j] << (j * 2 + 2) | c[j + 1] >> ((2 - j) * 2);
		}
	}
	if(size){
		*size = i;
	}

	return buff;
}
//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
c8* gfl::core::Utility::EncodeBase64(const c8* src, u32 src_size, u32* size)
{
	c8 *p, *buff;
	buff = DbgNew c8 [((src_size * 4 / 3 + 3) & ~0x03 + 1)+1];
	s32 i = 0, x = 0, l = 0;

	p = const_cast<c8*>(src);
	for (u32 j = 0; j < src_size; ++j, ++p) {
		x = x << 8 | *p;
		for (l += 8; l >= 6; l -= 6) {
			buff[i++] = s_pBase64[(x >> (l - 6)) & 0x3f];
		}
	}
	if (l > 0) {
		x <<= 6 - l;
		buff[i++] = s_pBase64[x & 0x3f];
	}
	for (; i % 4;) {
		buff[i++] = '=';
	}
	buff[i++] = 0x00;
	if(size){
		*size = i;
	}	
	
	return buff;
}
#endif
