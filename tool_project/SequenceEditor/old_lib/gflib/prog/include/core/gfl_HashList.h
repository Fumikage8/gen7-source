/**
 * @file	gfl_HashList.h
 * @brief	
 * @author	kimura_shin
 * @data	2011/03/29, 11:43
 */

#ifndef GFL_HASHLIST_H
#define	GFL_HASHLIST_H
#pragma once

namespace gfl {
namespace core {

/**
 *  Void�^�n�b�V�����X�g�N���X
 * @remark �e���v���[�g�̔�剻��h������
 */

class VoidHashList {
	GFL_FORBID_COPY_AND_ASSIGN( VoidHashList );
public:
	/**
	 *   �L�[���[�h���擾����֐��^
	 */
	typedef const c8* (*GET_KEY_FUNC)(const void* cls);

protected:
	gfl::heap::HeapBase* m_pHeap;
	const void** m_ppTable;
	GET_KEY_FUNC m_pGetKeyFunc;
	s32 m_Capacity;
	s32 m_Counter;

	/**
	 *   �A�C�e�����e�[�u���ɐݒ�
	 * @param  cls  �ݒ肵�����A�C�e���|�C���^
	 * @param  mask �n�b�V���e�[�u���̐��[�P�@�i�Q�ׂ̂����|�P�j
	 * @param  tbl  �n�b�V���e�[�u��
	 * @return ����
	 */
	void SetItem( const void* cls, const u32 mask, const void** tbl );

	/**
	 *   �e�[�u���̃T�C�Y�ύX
	 * @param  max  �m�ۂ������A�C�e���̐�
	 * @return ����
	 */
	void ResizeTable( const s32 size );

public:

	/**
	 *   �R���X�g���N�^
	 * @param  func  �N���X T ���̃L�[���[�h���擾����֐�
	 * @param  size  �e�[�u���T�C�Y�@�Q�ׂ̂���
	 * @return ����
	 */

	VoidHashList( GET_KEY_FUNC func = 0, const s32 size = 0, gfl::heap::HeapBase* heap = 0 ) : m_pHeap(heap), m_ppTable( 0 ), m_pGetKeyFunc( func ), m_Capacity( 0 ), m_Counter( 0 ) {
		if( size ){
			ResizeTable( size );
		}
	}

	/**
	 *   �f�X�g���N�^
	 * @param  ����
	 * @return ����
	 * @remark �e�[�u�����������J�����Ȃ�
	 */

	~VoidHashList( void ) {
		clear( );
	}

	/**
	 *   �L�[���[�h���擾����֐���ݒ�
	 * @param  func  �N���X T ���̃L�[���[�h���擾����֐�
	 * @return ����
	 */

	void SetKeyFunction( GET_KEY_FUNC func ) {
		m_pGetKeyFunc = func;
	}

	/**
	 *   �e�[�u���ɃN���X��ǉ�����
	 * @param  cls �ǉ�������void�^�|�C���^
	 * @return ����
	 */
	void Append( const void* cls );

	/**
	 *   �e�[�u������N���X���폜
	 * @param  cls �폜������void�^�|�C���^
	 * @return ����
	 * @remark �}�b�`����N���X���Ȃ��ꍇ�̓A�T�[�g�Œ�~����
	 */
	void Remove( const void* cls );

	/**
	 *   �����񂩂�N���X�|�C���^���擾
	 * @param  name �N���X��
	 * @return void�^�|�C���^
	 * @remark �}�b�`����N���X���Ȃ��ꍇ�͂O��Ԃ�
	 */
	void* Retrieve( const c8 * const name ) const;

	/**
	 *   �e�[�u�����N���A
	 * @param  ����
	 * @return ����
	 */

	void clear( void ) {
		delete [] m_ppTable;
		m_ppTable = 0;
		m_Capacity = m_Counter = 0;
	}

	/**
	 *   �e�[�u���̊e�N���X���N���A
	 * @param  ����
	 * @return ����
	 */

	template<typename T>
	void clear_and_delete( void ) {
		const void** tbl = m_ppTable;
		for( s32 i = 0; i < m_Capacity; i++ ){
			GFL_DELETE static_cast<const T*> (*tbl++);
		}
		clear( );
	}

	/**
	 *   �e�[�u�����擾
	 * @param  ����
	 * @return �e�[�u���|�C���^
	 */

	const void** GetTable( void ) {
		return m_ppTable;
	}

	const void** GetTable( void ) const {
		return m_ppTable;
	}

	/**
	 *   �e�[�u���̗e�ʂ��擾
	 * @param  ����
	 * @return �e�[�u���̗e�ʁ@�����Ă��鐔�ł͂Ȃ����e��
	 */

	s32 GetCapacity( void ) const {
		return m_Capacity;
	}

	/**
	 *   �e�[�u���̃T�C�Y���擾
	 * @param  ����
	 * @return �e�[�u���̃T�C�Y �����Ă��鐔
	 */

	s32 GetCounter( void ) const {
		return m_Counter;
	}

	/**
	 *   �N���X�e�[�u���̐擪�A�h���X���擾
	 * @param  ����
	 * @return �N���X�e�[�u���̐擪�A�h���X
	 */

	inline const void** begin( void ) const {
		return m_ppTable;
	}

	/**
	 *   �N���X�e�[�u���̏I�[�A�h���X���擾
	 * @param  ����
	 * @return �N���X�e�[�u���̏I�[�A�h���X
	 */

	inline const void** end( void ) const {
		return &m_ppTable[m_Capacity];
	}

    void SetHeap( gfl::heap::HeapBase* Heap ) {
    	this->m_pHeap = Heap;
    }

}; // class VoidHashList

/**
 *   �n�b�V�����X�g�N���X
 */

template<typename T>
class HashList : public VoidHashList {
	GFL_FORBID_COPY_AND_ASSIGN( HashList );
public:

	/**
	 *   �f�t�H���g�R���X�g���N�^
	 * @param  ����
	 * @return ����
	 */

	HashList( void ) {
	}

	/**
	 *   �R���X�g���N�^
	 * @param  func  �N���X T ���̃L�[���[�h���擾����֐�
	 * @return ����
	 * @remark �C���X�^���X�̑����h������ static �Ȍ^�̊֐���n���K�v������B
	 */

	HashList( GET_KEY_FUNC func, const s32 size = 0, gfl::heap::HeapBase* heap=0 ) : VoidHashList( func, size, heap ) {
	}

	/**
	 *   �f�X�g���N�^
	 * @param  ����
	 * @return ����
	 * @remark �e�[�u�����������J��
	 */

	~HashList( void ) {
	}

	/**
	 *   �e�[�u���ɃN���X��ǉ�����
	 * @param  cls �ǉ��������N���X�̎Q��
	 * @return ����
	 */

	inline void Append( const T& cls ) {
		VoidHashList::Append( static_cast<const void*> (&cls) );
	}

	/**
	 *   �e�[�u���ɃN���X��ǉ�����
	 * @param  cls �ǉ��������N���X�̃|�C���^
	 * @return ����
	 */

	inline void Append( T* cls ) {
		VoidHashList::Append( static_cast<const void*> (cls) );
	}

	/**
	 *   �e�[�u������N���X���폜
	 * @param  cls �폜�������N���X�̎Q��
	 * @return ����
	 * @remark �}�b�`����N���X���Ȃ��ꍇ�̓A�T�[�g�Œ�~����
	 */

	void Remove( const T& cls ) {
		VoidHashList::Remove( static_cast<const void*> (&cls) );
	}

	/**
	 *   �����񂩂�N���X�|�C���^���擾
	 * @param  name �N���X��
	 * @return �N���X�|�C���^
	 * @remark �}�b�`����N���X���Ȃ��ꍇ�͂O��Ԃ�
	 */

	T* Retrieve( const c8 * const name ) const {
		return static_cast<T*> (VoidHashList::Retrieve( name ));
	}
}; // class HashList

}
}

#endif	/* GFL_HASHLIST_H */

