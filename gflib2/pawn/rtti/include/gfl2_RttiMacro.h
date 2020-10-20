//==============================================================================
/**
 * @file  gfl_RttiMacro.h
 *  
 * @author  kimura_shin
 * @date  2011/04/04, 10:51
 */
// =============================================================================

#ifndef GFL_RTTIMACRO_H
#define GFL_RTTIMACRO_H
#pragma once

#ifdef USE_PROPERTY
// -------------------------------------------------------------------------
/** RTTI UIあるなし共通宣言 */
// -------------------------------------------------------------------------
#define GFL_RTTI_COMMON(cls) \
  public: \
    static gfl2::rtti::Rtti<cls> s_Rtti; \
    static gfl2::rtti::Rtti<cls>::Table s_Table[]; \
    typedef cls ThisClass;  \
    static gfl2::rtti::Rtti<cls>* GetRtti(void);
// DLL 対策で関数の実体はソースに移動させる

// -------------------------------------------------------------------------
/** RTTIルート宣言 変数 m_pGui の実体が複数できちゃうけど… */
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
/** RTTI スーパークラス宣言 */
// -------------------------------------------------------------------------
#define GFL_RTTI_SUPER(cls,super_cls)     \
  GFL_RTTI_ROOT(cls) \
  typedef super_cls ThisSuperClass;

// -------------------------------------------------------------------------
/** セーフキャスト */
// -------------------------------------------------------------------------
#define GFL_SAFE_CAST(obj,castto) (( obj ) ? (castto*)( obj )->SafeCast( &castto::s_Rtti) : 0 )

// -------------------------------------------------------------------------
/** RTTI ルート　スタティックインスタンス */
// -------------------------------------------------------------------------
#define GFL_RTTI_ROOT_INSTANCE(Cls) \
  gfl2::rtti::Rtti<Cls> Cls::s_Rtti( #Cls, 0 ); \
  gfl2::rtti::Rtti<Cls>* Cls::GetRtti(void){ return &s_Rtti; }

// -------------------------------------------------------------------------
/** RTTI　スタティックインスタンス */
// -------------------------------------------------------------------------
#define GFL_RTTI_SUPER_INSTANCE(Cls) \
  gfl2::rtti::Rtti<Cls> Cls::s_Rtti( #Cls, &ThisSuperClass::s_Rtti ); \
  gfl2::rtti::Rtti<Cls>* Cls::GetRtti(void){ return &s_Rtti; }

// -------------------------------------------------------------------------
/** クラス変数 */
/* アラインメントしなきゃ３DSだととんじゃう? */
// -------------------------------------------------------------------------
//#define GFL_RTTI_CLASS_VALUE( name ) ((ThisClass*)ThisClass::s_Table)->name
#define GFL_RTTI_CLASS_VALUE( name ) ((ThisClass*)gfl2::core::Utility::AlignAddress(ThisClass::s_Table,0x10))->name

// -------------------------------------------------------------------------
/** プロパティテーブル開始 */
#define GFL_RTTI_TABLE_BEGIN( Cls ) \
  gfl2::rtti::Rtti<Cls>::Table Cls::s_Table[] = {
/** プロパティ */
#define GFL_RTTI( name ) \
    gfl2::rtti::Rtti<ThisClass>::Table( #name, GFL_OFFSET_OF( ThisClass, name ), \
                      &ThisClass::s_Rtti, GFL_RTTI_CLASS_VALUE(name), #name ),
/** プロパティ　タイトル名付き */
#define GFL_RTTI_LABEL( name, text ) \
    gfl2::rtti::Rtti<ThisClass>::Table( #name, GFL_OFFSET_OF( ThisClass, name ), \
                      &ThisClass::s_Rtti, GFL_RTTI_CLASS_VALUE(name), text ),
/** プロパティテーブル終了 */
#define GFL_RTTI_TABLE_END \
  };

/** メインウィンドーで宣言されるクラスボタンたち */
#define GFL_RTTI_CLASS_BUTTON(cls) gfl2::QtPropertyClass( #cls, &cls, cls.GetRtti(), this )
#define GFL_RTTI_CLASS_BUTTON_POINTER(cls) gfl2::QtPropertyClass( #cls, cls, cls->GetRtti(), this )
#define GFL_RTTI_CLASS_BUTTON_LABEL(cls,label) gfl2::QtPropertyClass( label, &cls, cls.GetRtti(), this )
#define GFL_RTTI_CLASS_BUTTON_POINTER_LABEL(cls,label) gfl2::QtPropertyClass( label, cls, cls->GetRtti(), this )

/**  関数登録 heap_debug が必要！*/
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

#endif  /* GFL_RTTIMACRO_H */

