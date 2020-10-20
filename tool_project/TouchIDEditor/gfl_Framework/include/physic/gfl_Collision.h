/* 
 * File:   gfl_Collision.h
 * Author: skimura
 *
 * Created on 2011/12/17, 23:25
 */

#ifndef GFL_COLLISION_H
#define	GFL_COLLISION_H
#pragma once

namespace gfl {
namespace physic {

// 円柱コリジョン　コリジョン基底クラスをつくるべきかも

class Collision {
public:
	/**
	 * 球と球のコリジョン　すり抜けチェックをする
	 * @param new_p0　修正された座標０
	 * @param new_p1　修正された座標１
	 * @param p0　現在の座標０
	 * @param p1　現在の座標１
	 * @param old_p0　前の座標０
	 * @param old_p1　前の座標１
	 * @param p0_r　半径０
	 * @param p1_r　半径１
	 * @return 真　当たった　疑　当たらなかった
	 */

	static bool SphereToSphere( gfl::core::Vector3* new_p0, const gfl::core::Vector3& p0, const gfl::core::Vector3& old_p0, const f32 p0_r,
							 gfl::core::Vector3* new_p1, const gfl::core::Vector3& p1, const gfl::core::Vector3& old_p1, const f32 p1_r );

};

}
}

#endif	/* GFL_COLLISION_H */

