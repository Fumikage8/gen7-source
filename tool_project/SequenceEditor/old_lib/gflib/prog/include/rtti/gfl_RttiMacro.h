//==============================================================================
/**
 * @file	gfl_RttiMacro.h
 * 	
 * @author	kimura_shin
 * @data	2011/04/04, 10:51
 */
// =============================================================================

#ifndef GFL_RTTIMACRO_H
#define	GFL_RTTIMACRO_H
#pragma once

#ifdef USE_PROPERTY
// -------------------------------------------------------------------------
/** RTTI UI����Ȃ����ʐ錾 */
// -------------------------------------------------------------------------
#define GFL_RTTI_COMMON(cls) \
	public: \
		static gfl::rtti::Rtti<cls> s_Rtti; \
		static gfl::rtti::Rtti<cls>::Table s_Table[]; \
		typedef cls ThisClass;	\
		static gfl::rtti::Rtti<cls>* GetRtti(void);
// DLL �΍�Ŋ֐��̎��̂̓\�[�X�Ɉړ�������

// -------------------------------------------------------------------------
/** RTTI���[�g�錾 �ϐ� m_pGui �̎��̂������ł����Ⴄ���ǁc */
// -------------------------------------------------------------------------
#ifdef USE_PROPERTY_GUI
#define GFL_RTTI_ROOT(cls) \
	GFL_RTTI_COMMON(cls) \
    QWidget* m_pGui; \
	virtual QWidget* CreateGui( QWidget* parent=0 ){ m_pGui = GetRtti()->CreateGui( this, parent ); return m_pGui; } \
	virtual void DestroyGui(void){ delete m_pGui; m_pGui = 0; }

#else
#define GFL_RTTI_ROOT(cls) \
	GFL_RTTI_COMMON(cls) \
	void* CreateGui( void* ){ return 0; } \
	void DestroyGui( void ){ }
#endif

// -------------------------------------------------------------------------
/** RTTI �X�[�p�[�N���X�錾 */
// -------------------------------------------------------------------------
#define GFL_RTTI_SUPER(cls,super_cls)			\
	GFL_RTTI_ROOT(cls) \
	typedef super_cls ThisSuperClass;

// -------------------------------------------------------------------------
/** �Z�[�t�L���X�g */
// -------------------------------------------------------------------------
#define GFL_SAFE_CAST(obj,castto) (( obj ) ? (castto*)( obj )->SafeCast( &castto::s_Rtti) : 0 )

// -------------------------------------------------------------------------
/** RTTI ���[�g�@�X�^�e�B�b�N�C���X�^���X */
// -------------------------------------------------------------------------
#define GFL_RTTI_ROOT_INSTANCE(Cls) \
	gfl::rtti::Rtti<Cls> Cls::s_Rtti( #Cls, 0 ); \
	gfl::rtti::Rtti<Cls>* Cls::GetRtti(void){ return &s_Rtti; }

// -------------------------------------------------------------------------
/** RTTI�@�X�^�e�B�b�N�C���X�^���X */
// -------------------------------------------------------------------------
#define GFL_RTTI_SUPER_INSTANCE(Cls) \
	gfl::rtti::Rtti<Cls> Cls::s_Rtti( #Cls, &ThisSuperClass::s_Rtti ); \
	gfl::rtti::Rtti<Cls>* Cls::GetRtti(void){ return &s_Rtti; }

// -------------------------------------------------------------------------
/** �N���X�ϐ� */
/* �A���C�������g���Ȃ���RDS���ƂƂ񂶂Ⴄ? */
// -------------------------------------------------------------------------
//#define GFL_RTTI_CLASS_VALUE( name ) ((ThisClass*)ThisClass::s_Table)->name
#define GFL_RTTI_CLASS_VALUE( name ) ((ThisClass*)gfl::core::Utility::AlignAddress(ThisClass::s_Table,0x10))->name

// -------------------------------------------------------------------------
/** �v���p�e�B�e�[�u���J�n */
#define GFL_RTTI_TABLE_BEGIN( Cls ) \
	gfl::rtti::Rtti<Cls>::Table Cls::s_Table[] = {
/** �v���p�e�B */
#define GFL_RTTI( name ) \
		gfl::rtti::Rtti<ThisClass>::Table( #name, GFL_OFFSET_OF( ThisClass, name ), \
											&ThisClass::s_Rtti, GFL_RTTI_CLASS_VALUE(name), #name ),
/** �v���p�e�B�@�^�C�g�����t�� */
#define GFL_RTTI_LABEL( name, text ) \
		gfl::rtti::Rtti<ThisClass>::Table( #name, GFL_OFFSET_OF( ThisClass, name ), \
											&ThisClass::s_Rtti, GFL_RTTI_CLASS_VALUE(name), text ),
/** �v���p�e�B�e�[�u���I�� */
#define GFL_RTTI_TABLE_END \
	};

/** ���C���E�B���h�[�Ő錾�����N���X�{�^������ */
#define GFL_RTTI_CLASS_BUTTON(cls) gfl::QtPropertyClass( #cls, &cls, cls.GetRtti(), this )
#define GFL_RTTI_CLASS_BUTTON_POINTER(cls) gfl::QtPropertyClass( #cls, cls, cls->GetRtti(), this )
#define GFL_RTTI_CLASS_BUTTON_LABEL(cls,label) gfl::QtPropertyClass( label, &cls, cls.GetRtti(), this )
#define GFL_RTTI_CLASS_BUTTON_POINTER_LABEL(cls,label) gfl::QtPropertyClass( label, cls, cls->GetRtti(), this )

/**  �֐��o�^ heap_debug ���K�v�I*/
#define GFL_RTTI_REGISTER_FUNCTION(func) ThisClass::s_Rtti.RegisterProperty(#func,&ThisClass::func,heap_debug)
#define GFL_RTTI_CALL_FUNCTION(cls,func,val) cls->s_Rtti.CallProperty(cls,func,val);

// -------------------------------------------------------------------------
#else // USE_PROPERTY
#define GFL_RTTI_ROOT(cls)
#define GFL_RTTI_SUPER(cls,super_cls)
#define GFL_SAFE_CAST(obj,castto) ((castto*)( obj ))
#define GFL_RTTI_ROOT_INSTANCE(Cls)
#define GFL_RTTI_SUPER_INSTANCE(Cls)
#define GFL_RTTI_CLASS_VALUE( name )
#define GFL_RTTI_TABLE_BEGIN( Cls )
#define GFL_RTTI( name )
#define GFL_RTTI_LABEL( name, text )
#define GFL_RTTI_CLASS( class_type, name)
#define GFL_RTTI_TABLE_END
#define GFL_RTTI_CLASS_BUTTON(cls)
#define GFL_RTTI_CLASS_BUTTON_POINTER(cls)
#define GFL_RTTI_CLASS_BUTTON_LABEL(cls,label)
#define GFL_RTTI_CLASS_BUTTON_POINTER_LABEL(cls,label)
#define GFL_RTTI_REGISTER_FUNCTION(func)
#define GFL_RTTI_CALL_FUNCTION(cls,func,val)
#endif // USE_PROPERTY

#endif	/* GFL_RTTIMACRO_H */

