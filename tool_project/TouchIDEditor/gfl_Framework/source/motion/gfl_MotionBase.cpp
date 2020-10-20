// -----------------------------------------------------------------------------

#include "motion/gfl_JointBase.h"
#include "core/gfl_Quaternion.h"
#include "core/gfl_MathBase.h"
#include "motion/gfl_MotionBase.h"

// -----------------------------------------------------------------------------
bool gfl::motion::MotionBase::Initialize( void* data_void, gfl::heap::HeapBase* heap )
{
	m_pHeapBase = heap;
	_pData = ( u32* )data_void;
	u8* data = ( u8* )GetData( MOTION_BASEPOSE );

	if( !data ){
		return false;
	}

	// ジョイント
	s32 jnt_count = *data++;
	_JointCount = jnt_count;
	JointBase* jnt = CreateJoint( jnt_count );
	_pJointBase = jnt;

	// エフェクター
	s32 eff_count = *data++;
	_EffectorCount = eff_count;

	// root
	jnt->_Id = 0;
	jnt->_pParent = 0;
#ifdef USE_FLIP
	jnt->_pFlipJointBase = jnt;
#endif
	jnt->_IkType = 0;
	jnt->_ChildCount = jnt_count;
	jnt->_ScaleExist = false;

	// root をとばして１から、ID、親ジョイント、フリップIDを設定
	jnt = NextJoint( jnt );

	for( s32 i = 1; i < jnt_count; ++i, jnt = NextJoint( jnt ) ){
		//		jnt->_Id = i;
		jnt->_pParent = GetJoint<JointBase*>( *data++ );
#ifdef USE_FLIP
		jnt->_pFlipJointBase = GetJoint<JointBase*>( *data++ );
#endif
		jnt->_IkType = *data++;
		jnt->_ChildCount = *data++;
	}

	// ここでジョイント絶対番号テーブル設定
#if MATCH_JOINT_BY_ID
	_pIdTable = data;
#elif MATCH_JOINT_BY_HASH
	data = gfl::core::Utility::AlignAddress( data, sizeof(u32 ) );
	u32* data_u32 = ( u32* )data;
	_pHashTable = data_u32;
#endif
	jnt = _pJointBase;
	jnt = NextJoint( jnt );
	// ＩＫも設定
	EffectorBase* ep = CreateEffector( eff_count );
	_pEffectorBase = ep;
	for( s32 i = 1; i < jnt_count; ++i, jnt = NextJoint( jnt ) ){
#if MATCH_JOINT_BY_ID
		jnt->_Id = *data++;
#elif MATCH_JOINT_BY_HASH
		jnt->_Id = *data_u32++;
#elif MATCH_JOINT_BY_NAME
		jnt->_Id = i;
		jnt->_pName = ( c8* )data;
		//		gfl::motion::Debug::PrintConsole( "%d %s\n",i,data);
		data = gfl::core::Utility::AddAddress( data, strlen( ( c8* )data ) + 1 ); // +1 for null
#endif

		// IK の設定
		if( JointBase::IK_EFFECTOR <= jnt->_IkType ){
			jnt->_pEffectorBase = ep;

			// それぞれのＪｏｉｎｔを設定
			ep->_pJointBase_Effector = jnt;
			JointBase* chn;
			// Chain と Joint を遡って検索
			s32 j = i;
			for( chn = PreviousJoint( jnt ); j; --j, chn = PreviousJoint( chn ) ){
				if( chn->_IkType == JointBase::IK_JOINT ){
					ep->_pJointBase_Joint = chn;
				} else if( chn->_IkType == JointBase::IK_CHAIN ){
					ep->_pJointBase_Chain = chn;
					chn->_pEffectorBase = ep;
					break;
				}
			}
			//			GFL_ASSERT_MSG( j, "Effector id(%d) does not have chain.", jnt->_Id );

			ep = NextEffector( ep );
		}
	}

	const s32 tmax = *data++;
	_Fcurve.SetTextureCount( tmax );
	m_pTextureTypeTable = data;
	data += tmax;
#if (MATCH_JOINT_BY_ID || MATCH_JOINT_BY_NAME)
	//	data = LoadSetting( data, heap );
	// 次からは座標、Quaternionデータなのでアラインメントを計算
	data = gfl::core::Utility::AlignAddress( data, sizeof(f32 ) );
#elif MATCH_JOINT_BY_HASH
	data = ( u8* )data_u32;
#endif

	// 親のジョイントからのローカル座標、BasePoseQuaternionを設定
	jnt = GetJoint<JointBase*>( );
	f32* f32_val = reinterpret_cast < f32* >( data );

	for( s32 i = 0; i < jnt_count; ++i, jnt = NextJoint( jnt ) ){
		jnt->_pTrans = jnt->_pStaticTrans = ( gfl::core::Vector* )f32_val;
		jnt->_StaticTrans.Set( *jnt->_pStaticTrans );
		f32_val += gfl::core::TXYZ;

#ifdef GFL_QUAT_NEEDS_16BYTE_ALIGN
		jnt->_BasePoseQuat.Set( f32_val[gfl::core::TX], f32_val[gfl::core::TY], f32_val[gfl::core::TZ], f32_val[gfl::core::TW] );
#else // 
		jnt->_pBasePoseQuat = ( gfl::core::Quaternion* )f32_val;
#endif
		f32_val += gfl::core::TXYZW;

		if( i ){
			( jnt->GetBasePoseQuat( ) - jnt->_pParent->GetBasePoseQuat( ) ).QuaternionToRadian( &jnt->_BasePoseLocalRot );
		} else {
			jnt->GetBasePoseQuat( ).QuaternionToRadian( &jnt->_BasePoseLocalRot );
		}
	}

	LoadSetting( ( u8* )f32_val, heap );

#if GFL_DEBUG
	SetBoneLength( );
#endif

	return true;
}

// -----------------------------------------------------------------------------
// Destruct
// -----------------------------------------------------------------------------
gfl::motion::MotionBase::~MotionBase( void )
{
	DestroyJoint( );
	DestroyEffector( );
	GFL_DELETE_ARRAY _pValue;
	_pValue = 0;
#if GFL_CALC_SLOW_MOTION
	GFL_DELETE_ARRAY _pNextValue;
	_pNextValue = 0;
#endif
	//	GFL_DELETE_ARRAY _pData; // 本当はリファレンスカウンタを減らしたい
}

// -------------------------------------------------------------------------
// モーションが変わった
// -------------------------------------------------------------------------
bool gfl::motion::MotionBase::Load( const void* data )
{
	Fcurve* fc = GetFcurve( );
	// *3 trans and rot and scale
	const s32 count = GetJointCount( ) * gfl::core::TXYZ * 3;
	bool ret = fc->Initialize( data, count, m_pHeapBase );
	if( ret ){
		f32* vp = GetValue( );
		if( !vp ){ // モーションデータの計算結果を格納するバッファが確保されていない
			vp = GFL_NEW_ARRAY( m_pHeapBase ) f32 [count + fc->GetTextureCount( )];
			SetValue( vp );
		}
		fc->GetValueBegin( vp );

    // スローモーション用のデータがあればコピーしておく
    if (GetNextValue()) {
      const s32 buffer_size = GetJointCount( ) * gfl::core::TXYZ * 3 + fc->GetTextureCount( );
      gfl::core::Utility::Memcpy( GetNextValue(), GetValue(), buffer_size * sizeof(f32) );
    }

		// IK再計算で必要になる。仕組みを変えたらいらないかも。
		//		SetTransRot( GetJoint( ), vp, GetJointCount( ) );
	}
	return ret;
}

// -------------------------------------------------------------------------
// Change
// -------------------------------------------------------------------------
size_t gfl::motion::MotionBase::IsAvailable( const s32 num )
{
	u32* data = GetData( );
	if( !data ){
		return false; //( "Change() can not be called without calling Initialize()." );
	}
	if( num < MOTION_BEGIN || data[MOTION_SUM] < ( u32 )num ){
		return 0;
	}
	return( size_t )data[num];
}

// -------------------------------------------------------------------------
// Change
// -------------------------------------------------------------------------
bool gfl::motion::MotionBase::Change( const s32 num )
{
	size_t ofs = IsAvailable( num );
	if( ofs ){
		u32* data = GetData( );
		_MotionNumber = num;
		return Load( reinterpret_cast < u32* >( ( size_t )data + ofs ) );
	}
	return false;
}

// -------------------------------------------------------------------------
// Set frame
// -------------------------------------------------------------------------
void gfl::motion::MotionBase::SetFrameAndClear( const f32 frame )
{
	Fcurve* fc = GetFcurve( );
	fc->SetFrame( frame );
	fc->ClearKey( );
	if( frame == 0.0f ){
		fc->GetValueBegin( GetValue( ) );
	} else {
		fc->GetValueByFrame( GetValue( ), frame );
	}

  // スローモーション用のデータがあればコピーしておく
  if (GetNextValue()) {
    const s32 buffer_size = GetJointCount( ) * gfl::core::TXYZ * 3 + fc->GetTextureCount( );
    gfl::core::Utility::Memcpy( GetNextValue(), GetValue(), buffer_size * sizeof(f32) );
  }
}

// -------------------------------------------------------------------------
// SetTransRot
// モーションデータをチェインとエフェクタにセット
// -------------------------------------------------------------------------
#define CHK_SLOW_MOTION( frame_speed ) ( frame_speed != 0.0f && frame_speed < 0.5f + gfl::core::Math::EPSILON )
f32* gfl::motion::MotionBase::SetTransRot( JointBase* jnt, f32* vp, const s32 count )
{
#if GFL_CALC_SLOW_MOTION
	f32* next_vp = GetNextValue( );
#endif
#ifdef USE_LOCATOR_Y
	// ルートデータの始まりをセット
	vp += ROOT_DATA_BEGIN;
#    if GFL_CALC_SLOW_MOTION
	if( next_vp )
		next_vp += ROOT_DATA_BEGIN;
#    endif
#endif

#if 0 // 
	// root は特別処理
	if( vp[gfl::core::TX] == gfl::core::Math::F32_MAX ){
		jnt->_pTrans = jnt->_pStaticTrans;
		++vp;
	} else {
		jnt->_pTrans = ( gfl::core::Vector* )vp;
		vp += gfl::core::TXYZ;
	}

	if( vp[gfl::core::TX] == gfl::core::Math::F32_MAX ){
		jnt->_Rot.Set( jnt->_BasePoseLocalRot );
		++vp;
	} else {
		jnt->_Rot.Set( vp[gfl::core::TX], vp[gfl::core::TY], vp[gfl::core::TZ] );
		vp += gfl::core::TXYZ;
	}
	jnt = NextJoint( jnt ); // root 終わり
#endif
#if GFL_CALC_SLOW_MOTION
	f32 frame_speed = GetFrameSpeed( );
	f32 rate = 0.0f;
	bool interpo = false;
	if( next_vp && CHK_SLOW_MOTION( frame_speed ) ){
		f32 num;
		f32 frame = GetFrame( );
		rate = gfl::core::Math::FModf( frame, &num );
		interpo = !gfl::core::Math::IsAlmostZero( rate );
	}
#endif
	for( s32 j = 0; j < count; ++j, jnt = NextJoint( jnt ) ){
		// Set Trans
		if( vp[gfl::core::TX] == gfl::core::Math::F32_MAX ){
			jnt->_pStaticTrans->Set( jnt->_StaticTrans ); // 書き換え対策
			jnt->_pTrans = jnt->_pStaticTrans;
			++vp;
#if GFL_CALC_SLOW_MOTION
			if( interpo )
				++next_vp;
#endif
		} else if( jnt->_IkType >= JointBase::IK_EFFECTOR ){
			jnt->_pEffectorBase->_pTrans = ( gfl::core::Vector* )vp;
#if GFL_CALC_SLOW_MOTION
			if( interpo ){
				( ( gfl::core::Vector* )vp )->Lerp( *( ( gfl::core::Vector* )next_vp ), frame_speed );
				next_vp += gfl::core::TXYZ;
			}
#endif
			vp += gfl::core::TXYZ;
		} else {
			jnt->_pTrans = ( gfl::core::Vector* )vp;
#if GFL_CALC_SLOW_MOTION
			if( interpo ){
				( ( gfl::core::Vector* )vp )->Lerp( *( ( gfl::core::Vector* )next_vp ), frame_speed );
				next_vp += gfl::core::TXYZ;
			}
#endif
			vp += gfl::core::TXYZ;
		}

		// Set Rotation
		if( vp[gfl::core::TX] == gfl::core::Math::F32_MAX ){
			jnt->_Rot.Set( jnt->_BasePoseLocalRot );
			++vp;
#if GFL_CALC_SLOW_MOTION
			if( interpo )
				++next_vp;
#endif
		} else {
#if GFL_CALC_SLOW_MOTION
			if( !interpo )
#endif
				jnt->_Rot.Set( vp[gfl::core::TX], vp[gfl::core::TY], vp[gfl::core::TZ] );
#if GFL_CALC_SLOW_MOTION
			else {
				gfl::core::Matrix next_mat, mid_mat, bef_mat;

				bef_mat.SetRotateZYX( vp );
				next_mat.SetRotateZYX( next_vp );
				mid_mat.Slerp( bef_mat, next_mat, rate );
				mid_mat.GetRadian( &jnt->_Rot );
				next_vp += gfl::core::TXYZ;
			}
#endif
			vp += gfl::core::TXYZ;
		}

		// Set Scale
		if( vp[gfl::core::TX] == gfl::core::Math::F32_MAX ){
			jnt->_ScaleExist = false;
			++vp;
#if GFL_CALC_SLOW_MOTION
			if( interpo )
				++next_vp;
#endif
		} else {
			const f32 x = vp[gfl::core::TX];
			const f32 y = vp[gfl::core::TY];
			const f32 z = vp[gfl::core::TZ];
#if GFL_CALC_SLOW_MOTION
			if( !interpo )
#endif
				jnt->_Scale.Set( ( x != 0.0f ) ? x : 0.01f, ( y != 0.0f ) ? y : 0.01f, ( z != 0.0f ) ? z : 0.01f );
#if GFL_CALC_SLOW_MOTION
			else {
				gfl::core::Vector scl;
				scl.Set( ( x != 0.0f ) ? x : 0.01f, ( y != 0.0f ) ? y : 0.01f, ( z != 0.0f ) ? z : 0.01f );
				jnt->_Scale.Lerp( scl, frame_speed );
				next_vp += gfl::core::TXYZ;
			}
#endif
			vp += gfl::core::TXYZ;
			jnt->_ScaleExist = true;
		}
	}
	return vp;
}
f32* gfl::motion::MotionBase::SetTransRot( JointBase* jnt, f32* vp, const s32 count, JointBase* jnt_begin, JointBase* jnt_end )
{
#if GFL_CALC_SLOW_MOTION
	f32* next_vp = GetNextValue( );
#endif
#ifdef USE_LOCATOR_Y
	// ルートデータの始まりをセット
	vp += ROOT_DATA_BEGIN;
#    if GFL_CALC_SLOW_MOTION
	if( next_vp )
		next_vp += ROOT_DATA_BEGIN;
#    endif
#endif

#if 0 // 
	// root は特別処理
	if( vp[gfl::core::TX] == gfl::core::Math::F32_MAX ){
		jnt->_pTrans = jnt->_pStaticTrans;
		++vp;
	} else {
		jnt->_pTrans = ( gfl::core::Vector* )vp;
		vp += gfl::core::TXYZ;
	}

	if( vp[gfl::core::TX] == gfl::core::Math::F32_MAX ){
		jnt->_Rot.Set( jnt->_BasePoseLocalRot );
		++vp;
	} else {
		jnt->_Rot.Set( vp[gfl::core::TX], vp[gfl::core::TY], vp[gfl::core::TZ] );
		vp += gfl::core::TXYZ;
	}
	jnt = NextJoint( jnt ); // root 終わり
#endif
#if GFL_CALC_SLOW_MOTION
	f32 frame_speed = GetFrameSpeed( );
	f32 rate = 0.0f;
	bool interpo = false;
	if( next_vp && CHK_SLOW_MOTION( frame_speed ) ){
		f32 num;
		f32 frame = GetFrame( );
		rate = gfl::core::Math::FModf( frame, &num );
		interpo = !gfl::core::Math::IsAlmostZero( rate );
	}
#endif
	for( s32 j = 0; j < count; ++j, jnt = NextJoint( jnt ) ){
		bool set_jnt = ( jnt_begin <= jnt && jnt < jnt_end );
		// Set Trans
		if( vp[gfl::core::TX] == gfl::core::Math::F32_MAX ){
			if( set_jnt ){
				jnt->_pStaticTrans->Set( jnt->_StaticTrans ); // 書き換え対策
				jnt->_pTrans = jnt->_pStaticTrans;
			}
			++vp;
#if GFL_CALC_SLOW_MOTION
			if( interpo )
				++next_vp;
#endif
		} else if( jnt->_IkType >= JointBase::IK_EFFECTOR ){
			if( set_jnt ){
				jnt->_pEffectorBase->_pTrans = ( gfl::core::Vector* )vp;
			}
#if GFL_CALC_SLOW_MOTION
			if( interpo ){
				if( set_jnt ){
					( ( gfl::core::Vector* )vp )->Lerp( *( ( gfl::core::Vector* )next_vp ), frame_speed );
				}
				next_vp += gfl::core::TXYZ;
			}
#endif
			vp += gfl::core::TXYZ;
		} else {
			if( set_jnt ){
				jnt->_pTrans = ( gfl::core::Vector* )vp;
			}
#if GFL_CALC_SLOW_MOTION
			if( interpo ){
				if( set_jnt ){
					( ( gfl::core::Vector* )vp )->Lerp( *( ( gfl::core::Vector* )next_vp ), frame_speed );
				}
				next_vp += gfl::core::TXYZ;
			}
#endif
			vp += gfl::core::TXYZ;
		}

		// Set Rotation
		if( vp[gfl::core::TX] == gfl::core::Math::F32_MAX ){
			if( set_jnt ){
				jnt->_Rot.Set( jnt->_BasePoseLocalRot );
			}
			++vp;
#if GFL_CALC_SLOW_MOTION
			if( interpo )
				++next_vp;
#endif
		} else {
#if GFL_CALC_SLOW_MOTION
			if( !interpo ){
#endif
				if( set_jnt ){
					jnt->_Rot.Set( vp[gfl::core::TX], vp[gfl::core::TY], vp[gfl::core::TZ] );
				}
#if GFL_CALC_SLOW_MOTION
			}
#endif
#if GFL_CALC_SLOW_MOTION
			else {
				if( set_jnt ){
					gfl::core::Matrix next_mat, mid_mat, bef_mat;

					bef_mat.SetRotateZYX( vp );
					next_mat.SetRotateZYX( next_vp );
					mid_mat.Slerp( bef_mat, next_mat, rate );
					mid_mat.GetRadian( &jnt->_Rot );
				}
				next_vp += gfl::core::TXYZ;
			}
#endif
			vp += gfl::core::TXYZ;
		}

		// Set Scale
		if( vp[gfl::core::TX] == gfl::core::Math::F32_MAX ){
			if( set_jnt ){
				jnt->_ScaleExist = false;
			}
			++vp;
#if GFL_CALC_SLOW_MOTION
			if( interpo )
				++next_vp;
#endif
		} else {
			const f32 x = vp[gfl::core::TX];
			const f32 y = vp[gfl::core::TY];
			const f32 z = vp[gfl::core::TZ];
#if GFL_CALC_SLOW_MOTION
			if( !interpo ){
#endif
				if( set_jnt ){
					jnt->_Scale.Set( ( x != 0.0f ) ? x : 0.01f, ( y != 0.0f ) ? y : 0.01f, ( z != 0.0f ) ? z : 0.01f );
				}
#if GFL_CALC_SLOW_MOTION
			}
#endif				
#if GFL_CALC_SLOW_MOTION
			else {
				if( set_jnt ){
					gfl::core::Vector scl;
					scl.Set( ( x != 0.0f ) ? x : 0.01f, ( y != 0.0f ) ? y : 0.01f, ( z != 0.0f ) ? z : 0.01f );
					jnt->_Scale.Lerp( scl, frame_speed );
				}
				next_vp += gfl::core::TXYZ;
			}
#endif
			vp += gfl::core::TXYZ;
			if( set_jnt ){
				jnt->_ScaleExist = true;
			}
		}
	}
	return vp;
}

// -------------------------------------------------------------------------
// CalcIK
// -------------------------------------------------------------------------
void gfl::motion::MotionBase::CalcIK( JointBase* chn, const s32 count, const size_t ofs )
{
	for( s32 j = 1; j < count; ++j, chn = NextJoint( chn ) ){
		gfl::core::Matrix* chn_mp = GFL_GET_POINTER_BY_OFFSET( chn, gfl::core::Matrix, ofs );

		if( chn->_IkType <= JointBase::IK_CHAIN ){
			JointBase* parent = chn->_pParent;
			chn_mp->Set( *GFL_GET_POINTER_BY_OFFSET( parent, gfl::core::Matrix, ofs ) );
			// Maya ではセグメントスケールというのがある

			chn_mp->Trans( *chn->_pTrans );
			// 子に伝播させるのか？
			if( m_ScaleNoInherit && parent->_ScaleExist ){
				chn_mp->ScaleRecip( parent->_Scale );
			}

			// データは親からの差分角度
			chn_mp->RotateZYX( chn->_Rot );
			if( chn->_ScaleExist ){
				chn_mp->Scale( chn->_Scale );
			}
		}
#if 0 // 
		if( 0 == strcmp( "LFeeler1", chn->_pName ) ){
			chn->_pTrans->PrintConsole( "org loc" );
			chn->_Rot.PrintConsole( "org rot" );
			chn_mp->GetTrans( ).PrintConsole( "org glo" );
		}
#endif
		if( chn->_IkType == JointBase::IK_CHAIN ){
			EffectorBase* ep = chn->_pEffectorBase;
			JointBase* jnt = static_cast < JointBase* >( ep->_pJointBase_Joint );
			JointBase* eff = static_cast < JointBase* >( ep->_pJointBase_Effector );
			gfl::core::Matrix* jnt_mp = GFL_GET_POINTER_BY_OFFSET( jnt, gfl::core::Matrix, ofs );

			gfl::core::Vector loc;
			chn_mp->GlobalToLocal( &loc, *( ep->_pTrans ) );

			f32 l_1, l2_1;
			{ // Aim at the effector
				register f32 tmp_0, tmp_1, tmp_2;
				tmp_0 = loc.GetX( ) * loc.GetX( ) + loc.GetY( ) * loc.GetY( );
				l2_1 = tmp_0 + loc.GetZ( ) * loc.GetZ( );
				tmp_0 = gfl::core::Math::FSqrt( tmp_0 );
				l_1 = gfl::core::Math::FSqrt( l2_1 );
				tmp_1 = gfl::core::Math::FRecip( tmp_0 );
				tmp_2 = gfl::core::Math::FRecip( l_1 );
				chn_mp->RotateZ( loc.GetY( ) * tmp_1, loc.GetX( ) * tmp_1 );
				chn_mp->RotateY( -loc.GetZ( ) * tmp_2, tmp_0 * tmp_2 );
			}

			const f32 l_2 = jnt->_pTrans->GetX( );
			const f32 l_0 = eff->_pTrans->GetX( );

			if( l_1 < l_0 + l_2 ){
				f32 sz1, sz2, cz1, cz2;
				{
					register f32 A = l2_1 - ( l_2 * l_2 );
					register f32 B = ( l_0 * l_0 );
					register f32 C = 2.0f * l_0;

					cz2 = ( A + B ) / ( C * l_1 );
					cz1 = ( A - B ) / ( C * l_2 );

					if( cz2 > 1.0f ){
						cz2 = 1.0f;
					} else if( cz2 < -1.0f ){
						cz2 = -1.0f;
					}
					if( cz1 > 1.0f ){
						cz1 = 1.0f;
					} else if( cz1 < -1.0f ){
						cz1 = -1.0f;
					}
				}
				//				if( eff->_IkType == JointBase::IK_EFFECTOR ){
				if( !( eff->_IkType & JointBase::IK_EFFECTOR_REVERSE_BIT ) ){
					sz2 = -gfl::core::Math::FSqrt( 1.0f - cz2 * cz2 );
					sz1 = gfl::core::Math::FSqrt( 1.0f - cz1 * cz1 );
				} else {
					sz2 = gfl::core::Math::FSqrt( 1.0f - cz2 * cz2 );
					sz1 = -gfl::core::Math::FSqrt( 1.0f - cz1 * cz1 );
				}
#if IK_BEND_Z

				chn_mp->RotateZ( sz2, cz2 );
				jnt_mp->Set( *chn_mp );
				chn_mp->RotateZ( sz1, cz1 );

				// For Expression
				jnt->_Rot.SetZ( -gfl::core::Math::FATan2( sz1, cz1 ) );
#else // 
				//				gfl::core::Debug::PrintConsole( "%f\n",gfl::core::Math::RadianToDegree(gfl::core::Math::FATan2(sz2, cz2)));
				//				gfl::core::Debug::PrintConsole( "%f\n",gfl::core::Math::RadianToDegree(gfl::core::Math::FATan2(sz1, cz1)));
				chn_mp->RotateY( sz2, cz2 );
				jnt_mp->Set( *chn_mp );
				chn_mp->RotateY( sz1, cz1 );

				// For Expression
				jnt->_Rot.SetY( -gfl::core::Math::FATan2( sz1, cz1 ) );
#endif
			} else {
				jnt_mp->Set( *chn_mp );
			}
			//			chn_mp->TransX( jnt_mp->GetTrans( ), l_2 ); // AOS
			gfl::core::Vector v;
			chn_mp->TransX( &v, l_2 );
			jnt_mp->SetTrans( v );

			// 先っちょが SKIP_CALC の場合はエフェクターも計算の必要なし
			if( !( eff->_IkType & JointBase::IK_SKIP_CALC ) ){
				gfl::core::Matrix* eff_mp = GFL_GET_POINTER_BY_OFFSET( eff, gfl::core::Matrix, ofs );

				eff_mp->Set( *jnt_mp );
				eff_mp->TransX( l_0 );
				eff_mp->RotateZYX( eff->_Rot );
				if( eff->_ScaleExist ){
					eff_mp->Scale( eff->_Scale );
				}
				//				eff_mp->SetRotateZYX( *eff->_pRot ); // データがグローバル座標の場合
			}

			//			chn_mp->PrintConsole("chn",j);
		}
	}
}

// -------------------------------------------------------------------------
// Set BasePose Matrix
// -------------------------------------------------------------------------
void gfl::motion::MotionBase::SetBasePoseMatrix( JointBase* jnt, const s32 count )
{
	gfl::core::Matrix* mp = &jnt->_MotionMatrix;

	mp->SetUnit( );
	mp->SetTrans( *jnt->_pStaticTrans );
	mp->QuaternionToMatrix( jnt->GetBasePoseQuat( ) );

	jnt = NextJoint( jnt );
	for( s32 j = 1; j < count; ++j, jnt = NextJoint( jnt ) ){
		mp = &jnt->_MotionMatrix;

		mp->Set( jnt->_pParent->_MotionMatrix );
		mp->Trans( *jnt->_pStaticTrans );
		mp->QuaternionToMatrix( jnt->GetBasePoseQuat( ) );
	}
}

// -------------------------------------------------------------------------
// Update
// -------------------------------------------------------------------------
void gfl::motion::MotionBase::UpdateMotionMatrix( void )
{
	f32* vp = GetValue( );
	JointBase* jnt = GetJoint<JointBase*>( );
	const s32 count = GetJointCount( );
	if( vp ){
		// 座標と角度をジョイントにセット
		vp = SetTransRot( jnt, vp, count );

		// ルートの行列を計算

		jnt->_MotionMatrix.SetTrans( *jnt->_pTrans );
		jnt->_MotionMatrix.SetRotateZYX( jnt->_Rot );
		if( jnt->_ScaleExist ){
			jnt->_MotionMatrix.Scale( jnt->_Scale );
		}
		jnt = NextJoint( jnt ); // ルートはスキップ

		// IK 計算
		CalcIK( jnt, count, GFL_OFFSET_OF( JointBase, _MotionMatrix ) );
	} else { // モーションがロードされていない
		SetBasePoseMatrix( jnt, count );
	}
}


// ---------------------------------------------------------------------------
// Set Global Rotation
// ---------------------------------------------------------------------------
void gfl::motion::MotionBase::SetGlobalRotation( gfl::motion::JointBase* jnt, const gfl::core::Matrix& global_mat, const s32 count )
{
	// 角度成分を計算
	for( s32 j = 0; j < count; ++j, jnt = NextJoint( jnt ) ){
		//		if( !jnt->GetExpressionId( ) ){
		jnt->GetMatrix( )->Mul33( global_mat, *jnt->GetMotionMatrix( ) );
		//		jnt->GetMatrix( )->MatrixToQuaternion( jnt->GetQuat( ) );
		//		}
	}
}

// -------------------------------------------------------------------------
// Update Frame
// -------------------------------------------------------------------------
void gfl::motion::MotionBase::UpdateFrame( void )
{
	Fcurve* fc = GetFcurve( );
#if GFL_CALC_SLOW_MOTION
	f32 frame_speed = fc->GetFrameSpeed( );
	f32* vp = GetValue( );
	if( !CHK_SLOW_MOTION( frame_speed ) ){
		fc->GetValueByDeltaFrame( vp, frame_speed );
	} else {
		f32 num, next_num;
		const f32 frame = fc->GetFrame( );
		f32 next_frame = frame + frame_speed;
		const f32 rate = gfl::core::Math::FModf( frame, &num );
		const f32 next_rate = gfl::core::Math::FModf( next_frame, &next_num );
		f32* next_vp = GetNextValue( );
		bool calc = false;
		if( !next_vp ){ // モーションデータの計算結果を格納するバッファが確保されていない
			const s32 buffer_size = GetJointCount( ) * gfl::core::TXYZ * 3 + fc->GetTextureCount( );
			next_vp = GFL_NEW_ARRAY( m_pHeapBase ) f32 [buffer_size];
			SetNextValue( next_vp );
			calc = true;
		}
		if( gfl::core::Math::IsAlmostZero( rate ) || calc ){ // 次のフレームを計算
			fc->GetValueByDeltaFrame( next_vp, 1.0f );
		} else {
			const f32 range = frame_speed * 0.5f;
			bool next_1 = gfl::core::Math::IsSimilar( 1.0f, next_rate, range );
			if( next_1 || gfl::core::Math::IsAlmostZero( next_rate, range ) ){
				//				if( 0.0f != next_num ){
				const s32 buffer_size = GetJointCount( ) * gfl::core::TXYZ * 3 + fc->GetTextureCount( );
				gfl::core::Utility::Memcpy( vp, next_vp, buffer_size * sizeof(f32 ) );
				next_frame = ( next_1 ) ? next_num + 1.0f : next_num; // 浮動少数誤差をリセット
				//				}
			}
		}
		fc->SetFrame( next_frame );
	}
#else // 
	fc->GetValueByDeltaFrame( GetValue( ), fc->GetFrameSpeed( ) );
#endif
}
//==============================================================================
//==============================================================================
u8* gfl::motion::MotionBase::LoadSetting( u8* data, gfl::heap::HeapBase* )
{
	return data;
}

// -----------------------------------------------------------------------------
// 行列再計算
// -----------------------------------------------------------------------------
void gfl::motion::MotionBase::CalculateMatrix( JointBase* jnt, const s32 count )
{
	const s32 inherit = GetScaleNoInherit( );
	for( s32 j = 0; j < count; ++j, jnt = NextJoint( jnt ) ){
		gfl::core::Matrix* mp = jnt->GetMatrix( );
		JointBase* parent = jnt->GetParent( );
		if( parent ){
			mp->Set( *parent->GetMatrix( ) );
		} else {
			mp->SetUnit( );
		}
		mp->Trans( *jnt->GetTrans( ) );

		// 子に伝播させるのか？
		if( inherit && parent && parent->_ScaleExist ){
			mp->ScaleRecip( parent->_Scale );
		}
		// データは親からの差分角度
		mp->RotateZYX( *jnt->GetRot( ) );
		if( jnt->_ScaleExist ){
			mp->Scale( jnt->_Scale );
		}
		//		jnt->GetQuat( )->MatrixToQuaternion( *mp );
#if 0 //
		// 注視点補正角度がある場合
		InterestJoint* ij = jnt->GetInterestJoint( );
		if( ij ){
			gfl::core::Vector* rot = ij->GetRot( );
			mp->RotateY( rot->GetY( ) );
			mp->RotateZ( rot->GetZ( ) );
			mp->RotateX( rot->GetX( ) );
		}
#endif
	}
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
u16 gfl::motion::MotionBase::GetFrameCount( const s32 motnum )
{
	u16* dat = ( u16* )GetData( motnum );
	return dat ? dat[1] : 0;
}

#if GFL_DEBUG || defined(GFL_PLATFORM_PC)
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void gfl::motion::MotionBase::SetBoneLength( void )
{
	JointBase* jnt = GetJoint<JointBase*>( );
	for( s32 i = 0; i < GetJointCount( ); ++i, jnt = NextJoint( jnt ) ){
		f32 len;
		if( 0/*jnt->_ExpressionId*/ ){
			len = CENTI( 10.0f );
		} else {
			JointBase* c_jnt = NextJoint( jnt );
			gfl::core::Vector tra;
			s32 c_cnt = 0;
			tra.SetUnit( );
			for( s32 j = i + 1; j < GetJointCount( ); ++j, c_jnt = NextJoint( c_jnt ) ){
				if( c_jnt->GetParent( ) == jnt ){
					tra += *c_jnt->_pStaticTrans;
					++c_cnt;
				}
			}
			if( c_cnt ){
				tra /= c_cnt;
				len = tra.Magnitude( );
			} else {
				len = ( jnt->GetParent( ) ) ? jnt->GetParent( )->_Length : 0.0f;
				if( CENTI( 15.0f ) < len ){
					len = CENTI( 15.0f );
				}
			}
		}

		jnt->_Length = len;
		//		jnt->_Bone.SetSize( len, len * 0.2f, len * 0.15f );
	}
}
#endif
