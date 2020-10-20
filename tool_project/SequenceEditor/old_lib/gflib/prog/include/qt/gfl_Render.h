/* 
 * File:   gfl_Render.h
 * Author: s-kimura
 *
 * Created on 2010/06/08, 14:07
 */

#ifndef GFL_RENDER_H
#define	GFL_RENDER_H

#include <qt/gfl_QtGui.h>

namespace gfl {
namespace qt {

/**
 *   �����_�[�N���X
 * @remark �ȒP�Ƀv���~�e�B�u���f���������_�����O����X�^�e�B�b�N�֐��Q
 */
class Render {
public:

	/**
	 * �|���S���F
	 */
	enum PolygonColor {
		PCOL_WHITE, /**< �� */
		PCOL_RED, /**< �� */
		PCOL_GREEN, /**< �� */
		PCOL_BLUE, /**< �� */
		PCOL_YELLOW, /**< ���F */
		PCOL_GRAY, /**< �D�F */

		PCOL_DARKRED, /**< �Ð� */
		PCOL_DARKGREEN, /**< �×� */
		PCOL_DARKBLUE, /**< �Ð� */
		PCOL_DARKYELLOW, /**< �É��F */
		PCOL_DARKGRAY, /**< �ÊD�F */

		PCOL_SKYBLUE, /**< ��F */
		PCOL_MAX, /**< �F�e�[�u���ő�l */
	};

	/**
	 * �V�F�[�f�B���O�̃A�T�C���@SHD_
	 */
	enum ShadingType {
		SHD_NORMAL, /**< �t���b�g�V�F�[�f�B���O */
		SHD_WIRE, /**< ���C���[�t���[�� */
		SHD_GOURAUD, /**< �O�[���[ */
		SHD_TEXTURE, /**< �e�N�X�`���[���� */
		SHD_MAX, /**< �V�F�[�f�B���O��ލő�l */
	};

	/**
	 * �O���E���h�^�C�v
	 */
	enum GroundType {
		GROUND_NORMAL, /**< ���_�ɕ\�� */
		GROUND_CURRENT, /**< ��_�𒆐S�ɕ\�� */
		DEFAULT_GROUND_SIZE = 5,
	};

	enum Status {
		ST_NONE,
		ST_LOADING,
		ST_READY,
		ST_RENDERING
	};

	static const f32 _scColorTable[][4];
	static const u8 _scBoneEdge[][4];

	/**
	 * �}�e���A���J���[��ݒ�
	 * @param col �F�ԍ�
	 */
	static void SetMaterialColor( const PolygonColor col );
	static void SetColor( const PolygonColor col );

	/**
	 *   �O�p�`��\��
	 * @param  pos �\��������W
	 * @param  trans_y pos �� Y ���W�ɉ��Z�����l
	 * @param  rot_y �O�p�`�̌����@Y���p�x
	 * @param  type ShadingType �\�����@
	 * @return ����
	 */
	static void RotY( const gfl::core::Vector& pos, const f32 trans_y, const f32 rot_y, const ShadingType type );

	/**
	 *   �l�p�`��\��
	 * @param  x X ���W
	 * @param  z Z ���W
	 * @param  type ShadingType �\�����@
	 * @return ����
	 */
	static void Pivot( const f32 x, const f32 z, const ShadingType type );

	/**
	 *   �O�_����@�����v�Z���Đݒ肷��
	 * @param  v0 ���W
	 * @param  v1 ���W
	 * @param  v2 ���W
	 * @return ����
	 */
	static void SetTriangleNormal( const gfl::core::Vector3& v0, const gfl::core::Vector3& v1, const gfl::core::Vector3 & v2 );

	/**
	 *   XYZ����\��
	 * @param  size ���̒���
	 * @param  line_size ���̑���
	 * @return ����
	 */
	static void Axis( const f32 size, const f32 line_size = 1.0f );

	/**
	 *   �����̂�\��
	 * @param  size ���̒���
	 * @param  line_size ���̑���
	 * @return ����
	 */
	static void Cube( const f32 size, const f32 line_size = 1.0f );

	/**
	 *   �����f����\��
	 * @param  sx X ������
	 * @param  sy Y ������
	 * @param  sz Z ������
	 * @param  type �\�����@
	 * @param  cotex_no �|���S���F
	 * @return ����
	 */
	static void Bone( const f32 sx, const f32 sy, const f32 sz, const ShadingType type = SHD_NORMAL, const PolygonColor cotex_no = PCOL_WHITE );

	/**
	 *   �n�ʂ��O���b�h�ŕ\��
	 * @param  size �n�ʃT�C�Y
	 * @param  kind ���_�\�����ǐ��\����
	 * @param  ofs_x X ���W�I�t�Z�b�g
	 * @param  ofs_z Z ���W�I�t�Z�b�g
	 * @return ����
	 */
	static void Ground( const s32 size = DEFAULT_GROUND_SIZE, const GroundType kind = GROUND_NORMAL, const f32 ofs_x = 0.0f, const f32 ofs_z = 0.0f );
};

}
}

#endif	/* GFL_RENDER_H */

