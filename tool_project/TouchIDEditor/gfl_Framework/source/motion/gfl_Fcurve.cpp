
const f32 gfl::motion::Fcurve::ONE_FRAME = ( f32 )GFL_FPS;
const f32 gfl::motion::Fcurve::ONE_FRAME_TIME = 1.0f / ONE_FRAME;

// =======================================================================================
// Init Function Curve
// =======================================================================================
bool gfl::motion::Fcurve::Initialize( const void* adr, const s32 max_size, gfl::heap::HeapBase* heap )
{
	s32 i, j, lp_count, fc_cnt;
	u16 *u16_ptr;
	u16_ptr = ( u16* )adr;

	// 1st datum is FunctionCurve count
	s32 fc_count = *u16_ptr++; // set fc count
#if 0 //
	// ファンクションカーブが（関節数上限＋ルート座標）＊ＸＹＺ * (trans rot scale)を超えてる場合は壊れていると判断
	if( ( gfl::motion::Utility::U8_MAX + 1 ) * gfl::core::Math::TXYZ * 3 <= fc_count ){
		GR_WARNING( "Too many bones, this must be broken\n" );
		return false;
	}
#endif
	// 2nd datum is frame count.
	f32 frame_count;
	_FrameCount = frame_count = ( f32 ) * u16_ptr++;
	frame_count -= 1.0f;

	// 3rd key frame
#ifndef KEYSUM_WITHOUT_BEGIN_END
	u16_ptr++;
#endif

	size_t keynum_size = 1 + ( gfl::core::Math::U8_MAX < _FrameCount );

	_FcCount = fc_count;

	// load fcurve pointer
	FcurveKey* fck = _pFck;
	if( !fck ){
		_pFck = fck = GFL_NEW_ARRAY( heap ) FcurveKey [max_size];
	}

#ifndef USE_SPLINE_SLOPE_0
	// set loop counter for fcurve kind
	lp_count = ( fc_count << 1 ) / ( sizeof(u16 ) << 3 );
	if( fc_count & 7 ){
		lp_count++;
	}

	// each fcurve kind is only 2 bits
	for( i = fc_cnt = 0; i < lp_count; i++ ){
		u16 u16_val = *u16_ptr++;
		for( j = 0; j < ( s32 )( sizeof(u16 ) << 3 ); j += 2, fck++ ){
			fck->_Kind = ( u16_val >> j ) & FC_DATA_MASK;
			if( ++fc_cnt >= fc_count ) break;
		}
	}
#else
	// set loop counter for fcurve kind
	lp_count = ( fc_count * 3 ) / 24;
	if( fc_count & 7 ){
		lp_count++;
	}

	// each fcurve kind is only 3 bits
	u8* u8_ptr = ( u8* )u16_ptr;
	for( i = fc_cnt = 0; i < lp_count; i++ ){
#    ifdef GFL_LITTLE_ENDIAN
		u32 u32_val = u8_ptr[0] | ( u8_ptr[1] << 8 ) | ( u8_ptr[2] << 16 );
#    else
		u32 u32_val = u8_ptr[2] | ( u8_ptr[1] << 8 ) | ( u8_ptr[0] << 16 );
#    endif
		u8_ptr += 3;
		for( j = 0; j < 24; j += 3, fck++ ){
			fck->_Kind = ( u32_val >> j ) & FC_DATA_MASK;
			//			gfl::motion::Debug::PrintConsole( "[%d] %d\n", fc_cnt,fck->_Kind );
			if( ++fc_cnt >= fc_count ){
				//				u8_ptr -= 2 - ( j >> 3 ); // 余分な分を戻す
				break;
			}
		}
	}
#endif

#ifdef USE_TEXTURE_ANIMATION
	// TextureData
	//	const s32 tex_max = *u8_ptr++;
	if( !m_pTxk && m_TextureCount){
		m_pTxk = GFL_NEW_ARRAY( heap ) TextureKey [m_TextureCount];
	}
#endif

	f32 *f32_ptr;
	if( keynum_size == sizeof(u8 ) ){ // frame number is u8
#ifndef USE_SPLINE_SLOPE_0
		u8 *u8_ptr = ( u8* )u16_ptr;
#endif
#ifdef USE_TEXTURE_ANIMATION
		TextureKey* tk = m_pTxk;
		for( i = 0; i < m_TextureCount; ++i, ++tk ){
			tk->m_Sum = *u8_ptr++;
			tk->m_KeyNum = 0;
			if( tk->m_Sum ){
				tk->m_pKeyFrame = u8_ptr;
				u8_ptr += tk->m_Sum - 1;
				tk->m_pValue = u8_ptr;
				u8_ptr += tk->m_Sum;
			}
		}
#endif
		// set each fcurve key parameter pointer
		for( i = 0, fck = _pFck; i < fc_count; i++, fck++ ){
			if( FC_SPLINE_EXIST <= fck->_Kind ){
				s32 sum;
				fck->_Sum = sum = *u8_ptr++;

				fck->_pKeyFrame = u8_ptr;
#ifdef KEYSUM_WITHOUT_BEGIN_END
				u8_ptr += sum;
#else
				u8_ptr += sum - 2;
#endif
				// reset key number
				fck->_KeyNum = 0;
			}
		}

		f32_ptr = ( f32* )gfl::core::Utility::AlignAddress( u8_ptr, sizeof(f32 ) );
	} else { // frame number is u16
#ifdef USE_SPLINE_SLOPE_0
		u8_ptr = gfl::core::Utility::AlignAddress( u8_ptr, sizeof(u16 ) );
		u16_ptr = ( u16* )u8_ptr;
#endif
#ifdef USE_TEXTURE_ANIMATION
		TextureKey* tk = m_pTxk;
		for( i = 0; i < m_TextureCount; ++i, ++tk ){
			u8_ptr = gfl::core::Utility::AlignAddress( u8_ptr, sizeof(u16 ) );
			u16_ptr = ( u16* )u8_ptr;
			tk->m_Sum = *u16_ptr++;
			tk->m_KeyNum = 0;
			if( tk->m_Sum ){
				tk->m_pKeyFrame = u16_ptr;
				u16_ptr += tk->m_Sum - 1;
				u8_ptr = ( u8* )u16_ptr;
				tk->m_pValue = u8_ptr;
				u8_ptr += tk->m_Sum;
			}
		}
#endif
		//		mpGetValue = &GetValueByFrame_Type<u16>;
		// set each fcurve key parameter pointer
		for( i = 0, fck = _pFck; i < fc_count; i++, fck++ ){
			if( FC_SPLINE_EXIST <= fck->_Kind ){
				s32 sum;
				fck->_Sum = sum = *u16_ptr++;

				fck->_pKeyFrame = u16_ptr;
#ifdef KEYSUM_WITHOUT_BEGIN_END
				u16_ptr += sum;
#else
				u16_ptr += sum - 2;
#endif

				// reset key number
				fck->_KeyNum = 0;
			}
		}

		f32_ptr = ( f32* )gfl::core::Utility::AlignAddress( u16_ptr, sizeof(f32 ) );
	}

	// clear frame
	_Frame = 0.0f;

	// set key value pointer
	for( i = 0, fck = _pFck; i < fc_count; i++, fck++ ){
		if( FC_SPLINE_EXIST <= fck->_Kind ){
			fck->_pValue = f32_ptr;

			// if spline data has slope, it must be doubled.
#ifdef KEYSUM_WITHOUT_BEGIN_END
			f32_ptr += ( fck->_Sum + 2 ) << ( s32 )( fck->_Kind == FC_SPLINE_DATA );
#else
			f32_ptr += fck->_Sum << ( s32 )( fck->_Kind == FC_SPLINE_DATA );
#endif
		} else if( FC_STATIC_DATA == fck->_Kind ){
			fck->_pValue = f32_ptr++;
		} else {
#ifdef USE_SPLINE_SLOPE_0
			static const f32 * STATIC_VALUE_TABLE[FC_STATIC_DATA] = {
				&gfl::core::Math::F32_ZERO,
				&gfl::core::Math::F32_MAX,
				&gfl::core::Math::HALF_PI,
				&gfl::core::Math::ONE_PI,
				&gfl::core::Math::ONE_HALF_PI
			};
#else
			static const f32 * STATIC_VALUE_TABLE[FC_SPLINE_EXIST] = {
				&gfl::core::Math::F32_ZERO,
				&gfl::core::Math::F32_ZERO, // dummy
				&gfl::core::Math::F32_MAX,
			};
#endif
			fck->_pValue = ( f32* )STATIC_VALUE_TABLE[fck->_Kind];
		}
	}
	return true;
}

// =======================================================================================
// Clear Keynum
// =======================================================================================
void gfl::motion::Fcurve::ClearKey( void )
{
	s32 i;
	FcurveKey *fck;

	for( i = 0, fck = _pFck; i < _FcCount; i++, fck++ ){
		fck->_KeyNum = 0;
	}
	_Pivot._KeyNum = 0;
}

// =======================================================================================
// Get Function Curve Value End
// =======================================================================================
void gfl::motion::Fcurve::GetValueBegin( f32 *value )
{
	s32 i;
	FcurveKey *fck;
	const s32 end = _FcCount;

	for( i = 0, fck = _pFck; i < end; i++, fck++, value++ ){
		*value = *fck->_pValue;
	}
#ifdef USE_TEXTURE_ANIMATION
	TextureKey* tk = m_pTxk;
	if( tk ){
		for( i = 0; i < m_TextureCount; ++i, ++tk ){
			tk->m_Value = ( tk->m_Sum ) ? tk->m_pValue[0] : -1.0f;
		}
	}
#endif
}

// =======================================================================================
// Get Function Curve Value End
// =======================================================================================
void gfl::motion::Fcurve::GetValueEnd( f32 *value, s32 end )
{
	s32 i;
	FcurveKey* fck;

	if( !end ){
		end = _FcCount;
	}

	for( i = 0, fck = _pFck; i < end; i++, fck++, value++ ){
		switch( fck->_Kind ){
#ifdef USE_SPLINE_SLOPE_0
		case FC_SPLINE_SLOPE_0:
#    ifdef KEYSUM_WITHOUT_BEGIN_END
			*value = fck->_pValue[fck->_Sum + 1];
#    else
			*value = fck->_pValue[fck->_Sum - 1];
#    endif
			break;
#endif
		case FC_SPLINE_DATA:
#ifdef KEYSUM_WITHOUT_BEGIN_END
			*value = fck->_pValue[( fck->_Sum + 1 ) << 1];
#else
			*value = fck->_pValue[( fck->_Sum - 1 ) << 1];
#endif
			break;

		default:
			*value = *fck->_pValue;
		}
	}

#ifdef USE_TEXTURE_ANIMATION
	TextureKey* tk = m_pTxk;
	if( tk ){
		for( i = 0; i < m_TextureCount; ++i, ++tk ){
			tk->m_Value = ( tk->m_Sum ) ? tk->m_pValue[tk->m_Sum - 1] : -1.0f;
		}
	}
#endif
}
#ifndef FC_KEY_FLOAT
template<typename T>
bool gfl::motion::Fcurve::GetValueByFrame_Type( f32 *value, const f32 frame, s32 end )
{
	s32 i, j;
	register f32 num_f, pre_num_f;
	FcurveKey *fck;

	if( !value ){
		return false;
	}

	_OldFrame = _Frame;
	f32 frame_count = _FrameCount;
	if( frame_count <= frame ){
		_Frame = frame;
		GetValueEnd( value, end );
		return true;
	} else if( frame <= _Frame ){
		ClearKey( );
	}
	_Frame = frame;
	if( !end ){
		end = _FcCount;
	}
	//	frame_count -= 1.0f; // this is bug!!
	for( i = 0, fck = _pFck; i < end; i++, fck++, value++ ){
		switch( fck->_Kind ){
#    ifdef USE_SPLINE_SLOPE_0
		case FC_SPLINE_SLOPE_0:
		{
			s32 calc = 1;
#        ifdef KEYSUM_WITHOUT_BEGIN_END
			s32 sum = fck->_Sum;
#        else
			s32 sum = fck->_Sum - 2;
#        endif
			if( sum ){
				j = fck->_KeyNum;
				//				pre_num_f = j ? (f32) ( ( (T*) fck->_pKeyFrame )[j - 1] ) : 0.0f;
				T* kp = &( ( T* )fck->_pKeyFrame )[j];
				pre_num_f = j ? ( f32 ) * ( kp - 1 ) : 0.0f;
				for(;; ){
					num_f = ( f32 ) * kp++;
					if( num_f == frame ){
						*value = fck->_pValue[ j + 1 ]; // +1 to skip 0
						calc = 0;
						break;
					} else if( frame < num_f ){
						fck->_KeyNum = j;
						++j; // +1 to skip 0
						break;
					}

					pre_num_f = num_f;
					if( sum <= ++j ){
						num_f = frame_count;
						++j; // +1 to skip 0
						break;
					}
				}
			} else {
				j = 1;
				pre_num_f = 0.0f;
				num_f = frame_count;
			}

			if( calc ){
				register f32 t, t2, p0X, pX, p3X, v0, v1;

				pX = frame*ONE_FRAME_TIME;
				p0X = pre_num_f*ONE_FRAME_TIME;
				p3X = num_f*ONE_FRAME_TIME;

				t2 = pX - p0X;
				v0 = fck->_pValue[j - 1];
				t = t2 / ( p3X - p0X );
				v1 = fck->_pValue[j];

				*value = t * t * ( t + t - 3.0f )*( v0 - v1 ) + v0;
				break;
			}
			break;
		}
#    endif
		case FC_SPLINE_DATA:
		{
			s32 calc = 1;
#    ifdef KEYSUM_WITHOUT_BEGIN_END
			s32 sum = fck->_Sum;
#    else
			s32 sum = fck->_Sum - 2;
#    endif
			if( sum ){
				j = fck->_KeyNum;
				//				pre_num_f = j ? (f32) ( ( (T*) fck->_pKeyFrame )[j - 1] ) : 0.0f;
				T* kp = &( ( T* )fck->_pKeyFrame )[j];
				pre_num_f = j ? ( f32 ) *( kp - 1 ) : 0.0f;
				for(;; ){
					num_f = ( f32 ) * kp++;
					if( num_f == frame ){
						*value = fck->_pValue[( j + 1 ) << 1]; // +1 to skip 0
						calc = 0;
						break;
					} else if( frame < num_f ){
						fck->_KeyNum = j;
						++j; // +1 to skip 0
						break;
					}

					pre_num_f = num_f;
					if( sum <= ++j ){
						num_f = frame_count;
						++j; // +1 to skip 0
						break;
					}
				}
			} else {
				j = 1;
				pre_num_f = 0.0f;
				num_f = frame_count;
			}

			if( calc ){
				register f32 t, t2, t3, p0X, pX, p3X, v0;

				pX = frame*ONE_FRAME_TIME;
				p0X = pre_num_f*ONE_FRAME_TIME;
				p3X = num_f*ONE_FRAME_TIME;

				t2 = pX - p0X;
				j <<= 1; // value and slope
				t = t2 / ( p3X - p0X );
				v0 = fck->_pValue[j - 2];
				t3 = t - 1.0f;
				*value = ( t * fck->_pValue[j + 1] + t3 * fck->_pValue[j - 1] ) * t3 * t2 + t * t * ( ( t3 + t3 ) - 1.0f )*( v0 - fck->_pValue[j] ) + v0;
			}
			break;
		}
		default:
			*value = *fck->_pValue;
			break;
		}
	}

#    ifdef USE_TEXTURE_ANIMATION
	TextureKey* tk = m_pTxk;
	if( tk ){
		for( i = 0; i < m_TextureCount; ++i, ++tk ){
			u16 sum = tk->m_Sum;
			if( sum ){
				if(!--sum){ // キーフレームが一つだとスタティックな値
					tk->m_Value = ( f32 )tk->m_pValue[0];
					continue;
				}
				j = tk->m_KeyNum;

				T* kp = &( ( T* )tk->m_pKeyFrame )[j];
				pre_num_f = j ? ( f32 ) *( kp - 1 ) : 0.0f;
				for(;; ){
					num_f = ( f32 ) * kp++;
					if( num_f == frame ){
						tk->m_Value = tk->m_pValue[j + 1]; // +1 to skip 0
						break;
					} else if( frame < num_f ){
						tk->m_Value = ( f32 )tk->m_pValue[j];
						tk->m_KeyNum = j;
						++j; // +1 to skip 0
						break;
					}

					pre_num_f = num_f;
					if( sum <= ++j ){
						tk->m_Value = ( f32 )tk->m_pValue[sum];
						num_f = frame_count;
						++j; // +1 to skip 0
						break;
					}
				}
			} else {
				tk->m_Value = -1.0f;
			}
			//		if(i==0)
			//		gfl::core::Debug::PrintConsole( "%d %d (%d)\n",(s32)frame,(s32)value[i],j);
		}
	}
#    endif

	return false;
}

template bool gfl::motion::Fcurve::GetValueByFrame_Type<u8>( f32 *value, const f32 frame, s32 end );
template bool gfl::motion::Fcurve::GetValueByFrame_Type<u16>( f32 *value, const f32 frame, s32 end );

#endif
