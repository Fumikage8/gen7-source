#pragma once
#if !defined( FIELD_SUB_SCREEN_H_INCLUDED )
#define FIELD_SUB_SCREEN_H_INCLUDED

//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		 FieldSubScreen.h
 *	@brief   下画面 常駐してつなぐ為の役割 ここには機能はない
 *	@author  k.ohno
 *	@date		 2015.09.08
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <macro/include/gfl2_Macros.h>


GFL_NAMESPACE_BEGIN( Field );
class EventXMenu;
GFL_NAMESPACE_END( Field );


GFL_NAMESPACE_BEGIN( App );
GFL_NAMESPACE_BEGIN( FieldMenu );
class FieldMenuSubWindowProc;
GFL_NAMESPACE_END( FieldMenu );
GFL_NAMESPACE_END( App );



GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( SubScreen );



class FieldSubScreen
{

  GFL_FORBID_COPY_AND_ASSIGN(FieldSubScreen); //コピーコンストラクタ＋代入禁止


public:

  /**
   *	@brief  コンストラクタ
   */
  FieldSubScreen( void );
  /**
   *	@brief  デストラクタ
   */
  ~FieldSubScreen( void );

  void SetEventXMenu(Field::EventXMenu* pEve);

  void SetFieldMenuProc(App::FieldMenu::FieldMenuSubWindowProc* pProc);

  Field::EventXMenu* GetXMenu(void){ return m_pEventXMenu; }
  
private:
	Field::EventXMenu* m_pEventXMenu;

};


GFL_NAMESPACE_END( SubScreen );
GFL_NAMESPACE_END( Field );


#endif  // FIELD_SUB_SCREEN_H_INCLUDED
