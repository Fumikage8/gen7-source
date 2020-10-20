//==============================================================================
/**
 * @file	gfl_Ctex.h
 * 	
 * @author	kimura_shin
 * @data	2012/03/22, 12:50
 */
// =============================================================================

#if !defined( __GFL_CTEX_H__ )
#define	__GFL_CTEX_H__

#include <qt/gfl_QtGui.h>

namespace gfl {
namespace qt {

/**
 * Ctex
 */
class Ctex {
public:
#pragma pack(push,1)

	struct RGB_u8 {
		u8 m_R;
		u8 m_G;
		u8 m_B;
	};
#pragma pack(pop)

	/**
	 * CTEXを並び替えてRGB順にする
	 * @param src　CTEX型データ　書き換えます
	 * @param w　幅
	 * @param h　高さ
	 */
	template <typename T> static void FromCtex( T* src, const s32 w, const s32 h, const bool upsidedown = true ) {
		T* img = DbgNew T [w * h];
		T* imgp = img;
		// image swap
		const s32 i_a_max = h >> 3;
		for( s32 i_a = 0; i_a < i_a_max; ++i_a ){
			const s32 ofs_a = i_a * (w << 3);
			for( s32 i_b = 0; i_b < 2; ++i_b ){
				const s32 ofs_b = i_b << 5;
				for( s32 i_h = 0; i_h < 2; ++i_h ){
					const s32 ofs_h = i_h << 3;
					for( s32 i_v = 0; i_v < 2; ++i_v ){
						const s32 ofs_v = i_v << 1;
						for( s32 i_w = 0; i_w < w >> 3; ++i_w ){
							const s32 ofs_w = i_w << 6;
							for( s32 j = 0; j < 2; ++j ){
								const s32 ofs_j = j << 4;
								for( s32 k = 0; k < 2; ++k ){
									const s32 ofs_k = k << 2;
									for( s32 l = 0; l < 2; ++l ){
										*imgp++ = src[l + ofs_k + ofs_j + ofs_w + ofs_v + ofs_h + ofs_b + ofs_a];
									}
								}
							}
						}
					}
				}
			}
		}

		// upsidedown
		if( upsidedown ){
#if 0 // 
			if( sizeof (T) == 3 ){
				RGB_u8* img_ctex = (RGB_u8*) src;
				for( s32 i = 0; i < h; ++i ){
					RGB_u8* imgpp = (RGB_u8*) & img[w * (h - i - 1)];
					for( s32 j = 0; j < w; ++j, ++img_ctex, ++imgpp ){
						img_ctex->m_R = imgpp->m_B;
						img_ctex->m_G = imgpp->m_G;
						img_ctex->m_B = imgpp->m_R;
					}
				}
			} else {
#endif
				T* img_ctex = src;
				for( s32 i = 0; i < h; ++i ){
					imgp = &img[w * (h - i - 1)];
					for( s32 j = 0; j < w; ++j ){
						*img_ctex++ = *imgp++;
					}
				}
				//			}
			} else {
				memcpy( src, img, w * h * sizeof (T) );
			}
			delete [] img;
		}

		/**
		 * RGBを並び替えてCTEX順にする
		 * @param src　RGB型データ　書き換えます
		 * @param w　幅
		 * @param h　高さ
		 */
		template <typename T> static void ToCtex( T* src, const s32 w, const s32 h, const bool upsidedown = true ) {
			T* img = DbgNew T [w * h];
			T* imgp = img;

			// upsidedown
			if( upsidedown ){
				for( s32 i = 0; i < h; ++i ){
					T* img_ctex = &src[w * (h - i - 1)];
					for( s32 j = 0; j < w; ++j ){
						*imgp++ = *img_ctex++;
					}
				}
			} else {
				memcpy( img, src, w * h * sizeof (T) );
			}
			imgp = img;
			// image swap
			const s32 i_a_max = h >> 3;
			for( s32 i_a = 0; i_a < i_a_max; ++i_a ){
				const s32 ofs_a = i_a * (w << 3);
				for( s32 i_b = 0; i_b < 2; ++i_b ){
					const s32 ofs_b = i_b << 5;
					for( s32 i_h = 0; i_h < 2; ++i_h ){
						const s32 ofs_h = i_h << 3;
						for( s32 i_v = 0; i_v < 2; ++i_v ){
							const s32 ofs_v = i_v << 1;
							for( s32 i_w = 0; i_w < w >> 3; ++i_w ){
								const s32 ofs_w = i_w << 6;
								for( s32 j = 0; j < 2; ++j ){
									const s32 ofs_j = j << 4;
									for( s32 k = 0; k < 2; ++k ){
										const s32 ofs_k = k << 2;
										for( s32 l = 0; l < 2; ++l ){
											src[l + ofs_k + ofs_j + ofs_w + ofs_v + ofs_h + ofs_b + ofs_a] = *imgp++;
										}
									}
								}
							}
						}
					}
				}
			}

			delete [] img;
		}

		/**
		 * ETC1　を解凍
		 * @param src　元データ
		 * @param w　幅
		 * @param h　高さ
		 * @param upsidedown　上下をひっくり返す
		 * @return 解凍されたデータ
		 */
		static u8 * FromEtc1( u64* src, const s32 w, const s32 h, const bool upsidedown = true ) {

			struct Diffbit0 {
				u16 m_LSB;
				u16 m_MSB;
				u32 m_Flip : 1;
				u32 m_Diff : 1;
				u32 m_CW2 : 3;
				u32 m_CW1 : 3;
				u32 m_R2 : 4;
				u32 m_R1 : 4;
				u32 m_G2 : 4;
				u32 m_G1 : 4;
				u32 m_B2 : 4;
				u32 m_B1 : 4;
			};

			struct Diffbit1 {
				u16 m_LSB;
				u16 m_MSB;
				u32 m_Flip : 1;
				u32 m_Diff : 1;
				u32 m_CW2 : 3;
				u32 m_CW1 : 3;
				s32 m_R2 : 3;
				u32 m_R1 : 5;
				s32 m_G2 : 3;
				u32 m_G1 : 5;
				s32 m_B2 : 3;
				u32 m_B1 : 5;
			};

			const s32 size = w * h * 3; // 3 == rgb
			u8* res = DbgNew u8 [size];
			u8* res_org = res;

			static s32 TableCodeWord[8][2] = {
				{ 2, 8},
				{ 5, 17},
				{ 9, 29},
				{ 13, 42},
				{ 18, 60},
				{ 24, 80},
				{ 33, 106},
				{ 47, 183},
			};
			const s32 loop = w * h >> 4;
			for( s32 i = 0; i < loop; ++i, ++src ){
				Diffbit0* d0 = (Diffbit0*) src;

				u32 base_rgb[3][2];

				if( d0->m_Diff ){ // 差分モード
					Diffbit1* d1 = (Diffbit1*) src;

					base_rgb[0][0] = (d1->m_R1 * 0xff) / 0x1f;
					base_rgb[0][1] = ((d1->m_R1 + d1->m_R2) * 0xff) / 0x1f;
					base_rgb[1][0] = (d1->m_G1 * 0xff) / 0x1f;
					base_rgb[1][1] = ((d1->m_G1 + d1->m_G2) * 0xff) / 0x1f;
					base_rgb[2][0] = (d1->m_B1 * 0xff) / 0x1f;
					base_rgb[2][1] = ((d1->m_B1 + d1->m_B2) * 0xff) / 0x1f;
				} else { // 個別モード XXXXXXXXXXXX
					base_rgb[0][0] = d0->m_R1 * 17;
					base_rgb[0][1] = d0->m_R2 * 17;
					base_rgb[1][0] = d0->m_G1 * 17;
					base_rgb[1][1] = d0->m_G2 * 17;
					base_rgb[2][0] = d0->m_B1 * 17;
					base_rgb[2][1] = d0->m_B2 * 17;
				}

				const bool flip = d0->m_Flip;
				u16 lsb = d0->m_LSB;
				u16 msb = d0->m_MSB;
				for( s32 j = 0; j < 16; ++j ){
					u32 subblock;
					u32 cw;
					if( !flip ){
						if( j < 8 ){
							subblock = 0;
							cw = d0->m_CW1;
						} else {
							subblock = 1;
							cw = d0->m_CW2;
						}
					} else {
						if( (j & 3) < 2 ){
							subblock = 0;
							cw = d0->m_CW1;
						} else {
							subblock = 1;
							cw = d0->m_CW2;
						}
					}

					s32 adj = TableCodeWord[cw][(lsb >> j) & 1];
					if( (msb >> j) & 1 ){
						adj = -adj;
					}
					for( s32 k = 0; k < 3; ++k ){ // RGB == 3
						s32 val = base_rgb[k][subblock] + adj;
						gfl::core::Math::Clamp( &val, 0, 0xff );
						*res++ = val;
					}
				}
			}

			c8* img = DbgNew c8 [size];
			c8* imgp = img;
			// image swap

			const s32 e_h = h >> 3;
			const s32 b_w = w >> 3;

			s32 e_ofs = 0;
			for( s32 e = 0; e < e_h; ++e ){
				for( s32 d = 0; d < 2; ++d ){
					const s32 d_ofs = d << 5;
					for( s32 c = 0; c < 4; ++c ){
						for( s32 b = 0; b < b_w; ++b ){
							const s32 b_ofs = b << 6;
							for( s32 a = 0; a < 8; ++a ){
								s32 ofs = (a << 2) + b_ofs + c + d_ofs + e_ofs;
								ofs = ofs + (ofs << 1);
#if 0 // TGA
								*imgp++ = res_org[ofs++];
								*imgp++ = res_org[ofs++];
								*imgp++ = res_org[ofs];
#else // other
								*imgp++ = res_org[ofs + 2];
								*imgp++ = res_org[ofs + 1];
								*imgp++ = res_org[ofs];
#endif
							}
						}
					}
				}
				e_ofs += (w << 3);
			}

			// upsidedown
			if( upsidedown ){
				c8* img_ctex = (c8*) res_org;
				for( s32 i = 0; i < h; ++i ){
					imgp = &img[w * (h - i - 1) * 3];
					for( s32 j = 0; j < w; ++j ){
						*img_ctex++ = *imgp++;
						*img_ctex++ = *imgp++;
						*img_ctex++ = *imgp++;
					}
				}
			} else {
				memcpy( res_org, img, w * h * 3 );
			}
			delete [] img;
			return res_org;
		}
	};

}
}

#endif	/* __GFL_CTEX_H__ */

