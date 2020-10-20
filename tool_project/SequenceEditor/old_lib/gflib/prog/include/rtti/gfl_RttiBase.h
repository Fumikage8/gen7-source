//==============================================================================
/**
 * @file	gfl_RttiBase.h
 * 	
 * @author	kimura_shin
 * @data	2011/03/31, 14:45
 */
// =============================================================================

#ifndef GFL_RTTIBASE_H
#define	GFL_RTTIBASE_H
#pragma once

#ifdef USE_PROPERTY

namespace gfl {
namespace rtti {

/**
 * RTTI ���N���X
 */

class RttiBase {
protected:
	const c8 * const m_pRttiName;
	RttiBase* m_pRttiParent;

	friend class PropertyList;
	PropertyList m_PropertyList;

#ifdef USE_PROPERTY_GUI
	QStringList m_NameList;
#endif

	/**
	 * �R���X�g���N�^
	 * @param name �N���X��
	 * @param parent �eRTTI
	 */
	RttiBase( const c8 * const name, RttiBase* parent ) : m_pRttiName( name ), m_pRttiParent( parent ) {
	}

	virtual ~RttiBase( void ) {
		GetPropertyList( )->clear_and_delete<PropertyBase > ();
	}

	inline RttiBase* GetRttiParent( void ) {
		return m_pRttiParent;
	}

	inline const RttiBase * GetRttiParent( void ) const {
		return m_pRttiParent;
	}

	inline PropertyList* GetPropertyList( void ) {
		return &m_PropertyList;
	}

	inline const PropertyList * GetPropertyList( void ) const {
		return &m_PropertyList;
	}
	
#ifdef USE_PROPERTY_GUI

	inline QStringList * GetNameList( void ) {
		return &m_NameList;
	}
#endif

	/**
	 * �����@RTTI �������Ă��邩�H
	 * @param rtti_type ��r�� RTTI
	 * @return �^�F���݂���@�U�F���݂��Ȃ�
	 */
	bool IsA( RttiBase* dti_type ) {
		RttiBase* strat_type = this;
		while(strat_type) {
			if( strat_type == dti_type ){
				return true;
			} else {
				strat_type = strat_type->GetRttiParent( );
			}
		}
		return false;
	}

	/**
	 * �v���p�e�B���擾
	 * @param name�@�v���p�e�B��
	 * @return �v���p�e�B
	 */
	const void* GetProperty( const c8 * const name ) const {
		// �������O�̊֐��ɑΉ����邽�� List.Retrieve �̒���W�J
		u32 ofs = gfl::core::Utility::CalcHash( name );
		u32 add = (ofs << 1) | 1;
		u32 mask = GetPropertyList( )->GetCapacity( ) - 1;
		const void** tbl_list = GetPropertyList( )->GetTable( );
		for( u32 i = 0; i <= mask; i++ ){
			ofs = (ofs + add) & mask;
			const void* tbl = tbl_list[ofs];
			// �n�b�V���l����v�����疼�O����v���邩�m�F
			if( tbl && 0 == gfl::core::Utility::Strcmp( name, PropertyBase::GetPropertyName( tbl ) ) ){
				return tbl;
			}
		}
		return 0;
	}

public:

	inline const c8 * GetRttiName( void ) const {
		return m_pRttiName;
	}

	void SetPropertyListHeap( gfl::heap::HeapBase* heap ){
		GetPropertyList()->SetHeap(heap);
	}

	//	static const c8 * GetRttiName( const void* cls ) {
	//		return static_cast<const RttiBase*> (cls)->m_pRttiName;
	//	}

#ifdef USE_PROPERTY_GUI
	virtual QWidget* CreateInternalGui( void* cls, QWidget* parent ) = 0;
#endif
};

}
}

#endif

#endif	/* GFL_RTTIBASE_H */

