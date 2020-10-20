
bool gfl::physic::Collision::SphereToSphere(gfl::core::Vector3* new_p0, const gfl::core::Vector3& p0, const gfl::core::Vector3& old_p0, const f32 p0_r,
											 gfl::core::Vector3* new_p1, const gfl::core::Vector3& p1, const gfl::core::Vector3& old_p1, const f32 p1_r )
{
	const gfl::core::Vector3 old_p01( old_p1 - old_p0 ); // 前の位置の差
	const gfl::core::Vector3 p01( p1 - p0 ); // 今の位置の差
	const gfl::core::Vector3 diff_diff( p01 - old_p01 ); // 差の差
	const f32 len2 = diff_diff.Magnitude2( ); // 大きさの二乗
	const f32 r = p0_r + p1_r; // 半径の和
	const f32 r2 = r*r;
	if( len2 == 0.0f ){ // 同方向へ移動している
		const f32 l2 = p01.Magnitude2();
		if(l2 < r2 && l2 != 0.0f){ // お前は既に衝突している
			const f32 l = gfl::core::Math::FSqrt(l2);
			gfl::core::Vector3 p10;
			p10.Neg(p01);
			p10.Mul( r/l );
			new_p0->Add(p1,p10);
			return true;
		}
		return false;
	}

	const f32 old_dot = old_p01.Dot( diff_diff ); // 内積
	const f32 old_len2 = old_p01.Magnitude2( ); // 大きさの二乗
	const f32 collide2 = old_dot * old_dot - len2 * ( old_len2 - r2 ); // 衝突するか？
	if( collide2 < 0.0f ){ // 衝突しなかった
		return false;
	}

	// 衝突時間を求める
	const f32 recip_len2 = gfl::core::Math::FRecip( len2 );
	const f32 collide = gfl::core::Math::FSqrt( collide2 );
	const f32 t_plus = ( -old_dot + collide ) * recip_len2;
	const f32 t_minus = ( -old_dot - collide ) * recip_len2;
	// 衝突時間チェック　0-1 じゃないと駄目
	f32 t;
	if( 0.0f <= t_minus && t_minus <= 1.0f ){
		t = t_minus;
	}else if( 0.0f <= t_plus && t_plus <= 1.0f ){
		t = t_plus;
	}else{	
		return false;
	}

	// 衝突位置
	if( new_p0 ){
		new_p0->Sub( p0, old_p0 );
		new_p0->Mul( t );
		new_p0->Add( old_p0 );
	}
	if( new_p1 ){
		new_p1->Sub( p1, old_p1 );
		new_p1->Mul( t );
		new_p1->Add( old_p1 );
	}
	return true; // 衝突した
}
