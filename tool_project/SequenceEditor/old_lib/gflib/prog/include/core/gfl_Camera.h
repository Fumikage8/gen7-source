/* 
 * File:   gfl_Camera.h
 * Author: s-kimura
 *
 * Created on 2010/06/04, 16:45
 * �J����
 */

#ifndef GFL_CAMERA_H
#define	GFL_CAMERA_H
#pragma once

#define GFL_DEFAULT_NEAR_CLIP METER( 0.01f )
#define GFL_DEFAULT_FAR_CLIP METER( 400.0f )
#define GFL_DEFAULT_PERSPECTIVE 20.0f // degree

namespace gfl {
namespace core {

/**
 *   �J�����N���X
 * @remark �J�������N���X�@�����葁���e�X�g���邽�߂̊ȑf�����ꂽ�J�����N���X
 */
class Camera {
public:

	/**
	 * �J�������[�h
	 */
	enum CameraMode {
		MODE_NORMAL,
		MODE_ORBIT,
	};

	// �����͊ȒP�Ɏg�p�������̂ŃJ�v�Z�������Ă��܂���
	gfl::core::Matrix44 _Matrix;
	gfl::core::Vector _Trans;
	gfl::core::Vector _Interest;
	gfl::core::Vector _Rotation;
	f32 _ControlDistance;
	f32 _ControlRotationY;
	f32 _ControlRotationX;
	f32 _NearClip;
	f32 _FarClip;
	f32 _Perspective;
	u8 _Mode;

	/**
	 *   �R���X�g���N�^
	 * @param  ����
	 * @return ����
	 */
	Camera( void );

	/**
	 *   �����_������W��ݒ肷��
	 * @param  ����
	 * @return ����
	 */
	void SetTransByInterest( void );

	/**
	 *   �����_����p�x��ݒ肷��
	 * @param  ����
	 * @return ����
	 */
	void SetRotationByInterest( void );

	/**
	 *   ���W����}�g���b�N�X��ݒ肷��
	 * @param  ����
	 * @return ����
	 */
	void SetMatrixByPosition( f32 scale = 1.0f ); // 11.11.22 funakoshi ���s���e���ɉ��s����\�����邽��scale���̗p

	/**
	 *   �����_���W��ݒ肷��
	 * @param  pos �����_���W
	 * @return ����
	 * @remark �J�����̍��W�Ɗp�x���ݒ肳���
	 */
	void SetInterest( const gfl::core::Vector& pos );
	void UpdateMatrixByInterest( f32 scale = 1.0f ); // 11.11.22 funakoshi ���s���e���ɉ��s����\�����邽��scale���̗p

	/**
	 *   �J������ X �p�x�ɉ��Z����
	 * @param  ang ���Z���� X �p�x
	 * @return ����
	 * @remark ����܂ł����Ă��Ђ�����Ԃ�Ȃ��悤�ɐ������������Ă���
	 */
	void AddRotationX( const f32 ang );

	/**
	 *   �J������ Y�p�x�ɉ��Z����
	 * @param  ang ���Z���� Y �p�x
	 * @return ����
	 */

	inline void AddRotationY( const f32 ang ) {
		_ControlRotationY = gfl::core::Math::AddRadian( _ControlRotationY, ang );
	}

	/**
	 *   �J�����ƒ����_�̋����ɉ��Z����
	 * @param  dist ���Z���鋗��
	 * @return ����
	 */
	void AddDistance( const f32 dist );
};

}
}

#endif	/* GFL_CAMERA_H */

