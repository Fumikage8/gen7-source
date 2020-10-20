//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		 FieldSubScreen.cpp
 *	@brief   下画面 常駐してつなぐ為の役割
 *	@author  k.ohno
 *	@date		 2015.09.08
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#include "Field/FieldStatic/include/Event/EventXMenu.h"
#include "Field/FieldStatic/include/SubScreen/FieldSubScreen.h"



GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( SubScreen );


//-----------------------------------------------------------------------------
/**
 *					定数宣言
 */
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
 */
//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
/**
 *					実装
 */
//-----------------------------------------------------------------------------

/**
 *	@brief  コンストラクタ
 */
FieldSubScreen::FieldSubScreen( void ) :
m_pEventXMenu(NULL)
{
}

/**
 *	@brief  デストラクタ
 */
FieldSubScreen::~FieldSubScreen( void )
{
}


void FieldSubScreen::SetEventXMenu(Field::EventXMenu* pEve)
{
  m_pEventXMenu = pEve;
}

void FieldSubScreen::SetFieldMenuProc(App::FieldMenu::FieldMenuSubWindowProc* pProc)
{

  if(m_pEventXMenu){
    m_pEventXMenu->SetFieldMenuProc(pProc);
  }

}



GFL_NAMESPACE_END( SubScreen );
GFL_NAMESPACE_END( Field );


