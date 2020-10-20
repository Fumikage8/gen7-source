#ifndef __GFL_UTIL_VERTEXOBJECT_H__
#define __GFL_UTIL_VERTEXOBJECT_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_UtilVertexObject.h
 *	@brief  ���_�I�u�W�F�N�g�N���X
 *	@author	Masateru Ishiguro
 *	@date		2011.2.23
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
// gflib
#include <gfl_Macros.h>
#include <math/gfl_MathType.h>
#include <grp/gfl_GraphicsSystem.h>
#include <grp/util/gfl_UtilVertex.h>

namespace gfl {
namespace grp {
namespace util {

//! @brief ���_�I�u�W�F�N�g�N���X
class VertexObject
{
  GFL_FORBID_COPY_AND_ASSIGN(VertexObject);	
public:

	VertexObject();
	~VertexObject();

  /**
   * @brief �C���f�b�N�X�o�b�t�@�����֐�
   * @param size    �T�C�Y
   * @param pData   �C���f�b�N�X�f�[�^(�������ɏ������݂����ꍇ)
   */
	void CreateIndexBuffer( s32 size, u16* pData = NULL );

  /**
   * @brief ���_�o�b�t�@�����֐�
   * @param size    �T�C�Y
   * @param pData   ���_�f�[�^(�������ɏ������݂����ꍇ)
   */
	void CreateVertexBuffer( s32 size, Vertex* pData = NULL );

  /**
   * @brief �C���f�b�N�X�o�b�t�@�������݊֐�
   * @param pData   �������݂����f�[�^
   * @param size    �T�C�Y
   * @param offset  �������݊J�n�I�t�Z�b�g
   */
	void WriteIndexBuffer( u16* pData, s32 size, s32 offset = 0 );

  /**
   * @brief ���_�o�b�t�@�������݊֐�
   * @param pData   �������݂������_�f�[�^
   * @param size    �T�C�Y
   * @param offset  �������݊J�n�I�t�Z�b�g
   */
	void WriteVertexBuffer( Vertex* pData, s32 size, s32 offset = 0 );

  /**
   * @brief   �C���f�b�N�X���擾�֐�
   * @return  �C���f�b�N�X��
   */
	inline GLuint GetIndexNum() const { return m_IndexNum; }

  /**
   * @brief   �C���f�b�N�X�o�b�t�@�h�c�擾�֐�
   * @return  �C���f�b�N�X�o�b�t�@�h�c
   */
	inline GLuint GetIndexBufferId() const { return m_IndexBufferId; }

  /**
   * @brief   ���_�o�b�t�@�h�c�擾�֐�
   * @return  ���_�o�b�t�@�h�c
   */
	inline GLuint GetVertexBufferId() const { return m_VertexBufferId; }

private:

	s32							m_IndexNum;           //! �C���f�b�N�X��
	GLuint					m_IndexBufferId;      //! �C���f�b�N�X�o�b�t�@�h�c
	GLuint					m_VertexBufferId;     //! ���_�o�b�t�@�h�c

};

}  // namespace util
}  // namespace grp
}  // namespace gfl

#endif // __GFL_UTIL_VERTEXOBJECT_H__
