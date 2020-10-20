//==============================================================================
/**
 * @file	ExHashList.cpp
 * @brief	
 * @author	kimura_shin
 * @data	2011/03/29, 11:50
 */
// =============================================================================

//==============================================================================
// VoidHashList �֐�����
//==============================================================================
//------------------------------------------------------------------------------
//  SetItem
//------------------------------------------------------------------------------
void gfl::core::VoidHashList::SetItem( const void* cls, const u32 mask, const void** tbl )
{
	// �����񂩂�n�b�V�����擾
	u32 ofs = Utility::CalcHash( ( *m_pGetKeyFunc )( cls ) );
	// �I�t�Z�b�g���Z����ݒ�@�b�P���Ă�̂͂P�ȏ�ɂ��邽��
	u32 add = ( ofs << 1 ) | 1;
	// for(;;) �ł����v�����A���S�������߂邽��
	for( u32 i = 0; i <= mask; ++i ){
		ofs = ( ofs + add ) & mask;
		// �e�[�u��[�I�t�Z�b�g] ���󂩁H
		if( tbl[ofs] == 0 ){
			// ��Ȃ�A�C�e�����Z�b�g
			tbl[ofs] = cls;
			return;
		}
	}
	gfl::core::Debug::PrintConsole( "This hashtable is broken. Could not add (%s)\n", ( *m_pGetKeyFunc )( cls ) );
}

//------------------------------------------------------------------------------
//  ResizeTable
//------------------------------------------------------------------------------
void gfl::core::VoidHashList::ResizeTable( const s32 size )
{
	// �f�t�H���g�A���P�[�^�[���g���ă������m��
//	const void **new_tbl = new const void* [size];
	const void **new_tbl = m_pHeap?GFL_NEW_ARRAY(m_pHeap) const void* [size]:new const void* [size];
	// �O�N���A
	Utility::Memset( new_tbl, 0x00, size * sizeof(void* ) );
	// size �͂Q�ׂ̂����̂͂��Ȃ̂Ń}�X�N�́[�P����΂悢
	u32 mask = size - 1;
	// ���̃e�[�u���A�C�e����V�����e�[�u���ɍĔz�u
	for( s32 i = 0; i < m_Capacity; ++i ){
		const void* cls = m_ppTable[i];
		// ���̃e�[�u���A�C�e������łȂ����H
		if( cls ){
			// ��łȂ��̂ōĔz�u
			SetItem( cls, mask, new_tbl );
		}
	}
	// �Ĕz�u���ꂽ�̂Ō��̃e�[�u���͊J��
	//	gfl::core::LibAllocator::Free(m_ppTable);
	delete [] m_ppTable;
	// �V�����e�[�u���|�C���^��ݒ�
	m_ppTable = new_tbl;
	// �V�����e�[�u���T�C�Y��ݒ�
	m_Capacity = size;
}

// -----------------------------------------------------------------
//  Append
// -----------------------------------------------------------------
void gfl::core::VoidHashList::Append( const void* cls )
{
	s32 tmax = m_Capacity;
	// �e�[�u���A�C�e���̐��������Ă��āA�p�ӂ��ꂽ���̔����𒴂�����L����
	if( tmax < ( ++m_Counter << 1 ) ){
		// ���̃T�C�Y�̂Q�{�ɍL����
		tmax = ( tmax ) ? ( tmax << 1 ) : 1;
		// �L�����e�[�u���ɍĔz�u����
		ResizeTable( tmax );
	}
	// �V�����A�C�e����ǉ�����
	SetItem( cls, tmax - 1, m_ppTable );
}

// -----------------------------------------------------------------
//  Remove
// -----------------------------------------------------------------
void gfl::core::VoidHashList::Remove( const void* cls )
{
	const void **tbl = m_ppTable;
	s32 tmax = m_Capacity;
	u32 ofs = Utility::CalcHash( ( *m_pGetKeyFunc )( cls ) );
	u32 add = ( ofs << 1 ) | 1;
	u32 mask = tmax - 1;
#if GFL_HIO_ENABLE
	bool flag = false;
#endif
	for( u32 i = 0; i <= mask; i++ ){
		ofs = ( ofs + add ) & mask;
		if( tbl[ofs] == cls ){
			tbl[ofs] = 0;
#if GFL_HIO_ENABLE
			flag = true;
#endif
			break;
		}
	}
#if GFL_HIO_ENABLE
	if(!flag){
		gfl::core::Debug::PrintConsole( "No such class(%s) in this hashtable.\n", ( *m_pGetKeyFunc )( cls ) );
		return;
	}
#endif
	// �A�C�e���������Ă����̂Ńe�[�u�����k������
	if( ( --m_Counter << 1 ) <= tmax ){
		ResizeTable( tmax >> 1 );
	}
}

// -----------------------------------------------------------------
//  Retrieve
// -----------------------------------------------------------------
void* gfl::core::VoidHashList::Retrieve( const c8 * const name ) const
{
	if( m_ppTable ){
		u32 ofs = Utility::CalcHash( name );
		u32 add = ( ofs << 1 ) | 1;
		u32 mask = m_Capacity - 1;
		for( u32 i = 0; i <= mask; i++ ){
			ofs = ( ofs + add ) & mask;
			const void* tbl = m_ppTable[ofs];
			// �n�b�V������v�Z���ꂽ�A�C�e���ɖ��O����v���邩�H
			// ��v���Ȃ��ꍇ�͈Ⴄ�I�t�Z�b�g�l�̏ꏊ�Ɋi�[����Ă���̂Ń��[�v������
			if( tbl && 0 == Utility::Strcmp( name, ( *m_pGetKeyFunc )( tbl ) ) ){
				return const_cast < void* >( tbl );
			}
		}
	}
	return 0;
}
