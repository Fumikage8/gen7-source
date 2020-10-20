//==============================================================================
/**
 * @file	gfl_Pawn.h
 * 	
 * @author	kimura_shin
 * @data	2011/02/03, 15:44
 */
// =============================================================================

#ifndef GFL_PAWN_H
#define	GFL_PAWN_H

#include "gfl_PawnBase.h"
#if GFL_PAWN_DEBUG
#include "gfl_PawnDebug.h"
#endif

namespace gfl { namespace pawn {

#if GFL_PAWN_DEBUG
class Pawn : public PawnDebug {
#else
class Pawn : public PawnBase {
#endif
public:

#if GFL_PAWN_DEBUG

	Pawn( gfl::heap::HeapBase* heap = 0, gfl::heap::HeapBase* heap_debug = 0 ) : PawnDebug( heap, heap_debug ) {
	}
#else

	Pawn( gfl::heap::HeapBase* heap = 0, gfl::heap::HeapBase* heap_debug = 0 ) : PawnBase( heap ) {
	}
#endif

	virtual ~Pawn( void ) {
	}

	virtual void Load( const void* buffer, const size_t size, const c8* const amxname="" ) {
#if GFL_PAWN_DEBUG
		PawnDebug::Load( buffer, size, amxname );
#else
		PawnBase::Load( buffer, size, amxname );
#endif
		GetAmx( )->userdata[GFL_AMX_THIS] = this; // �����œ���Ȃ���Pawn�̃|�C���^�ɂȂ�Ȃ�
	}
	
	/**
	 * �R�[�h���V�F�A���ă��[�h
     * @param org�@�R�s�[���ƂȂ�Pawn�N���X�C���X�^���X
     */
	virtual void LoadShare(const PawnBase* org)
	{
#if GFL_PAWN_DEBUG
		PawnDebug::LoadShare( org );
#else
		PawnBase::LoadShare( org );
#endif
		GetAmx( )->userdata[GFL_AMX_THIS] = this; // �����œ���Ȃ���Pawn�̃|�C���^�ɂȂ�Ȃ�
	}

	void LoadByteArrayPointer( gfl::core::ByteArrayPointer dat, const c8* const amxname ) {
		Load( dat->GetData( ), dat->GetSize( ), amxname );
	}

	void LoadFile( const c8 * const filename ) {
		LoadByteArrayPointer( gfl::core::File::LoadSmart( filename, m_pHeap ), filename );
	}
};

} }

#endif	/* GFL_PAWN_H */

