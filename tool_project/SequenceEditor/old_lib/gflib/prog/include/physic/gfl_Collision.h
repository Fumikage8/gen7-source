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

// �~���R���W�����@�R���W�������N���X������ׂ�����

class Collision {
public:
	/**
	 * ���Ƌ��̃R���W�����@���蔲���`�F�b�N������
	 * @param new_p0�@�C�����ꂽ���W�O
	 * @param new_p1�@�C�����ꂽ���W�P
	 * @param p0�@���݂̍��W�O
	 * @param p1�@���݂̍��W�P
	 * @param old_p0�@�O�̍��W�O
	 * @param old_p1�@�O�̍��W�P
	 * @param p0_r�@���a�O
	 * @param p1_r�@���a�P
	 * @return �^�@���������@�^�@������Ȃ�����
	 */

	static bool SphereToSphere( gfl::core::Vector3* new_p0, const gfl::core::Vector3& p0, const gfl::core::Vector3& old_p0, const f32 p0_r,
							 gfl::core::Vector3* new_p1, const gfl::core::Vector3& p1, const gfl::core::Vector3& old_p1, const f32 p1_r );

};

}
}

#endif	/* GFL_COLLISION_H */

