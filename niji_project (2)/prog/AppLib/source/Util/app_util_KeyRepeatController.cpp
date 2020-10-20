//=============================================================================
/**
 * @brief キーリピートの挙動を制御するクラス
 * @file app_util_KeyRepeatController.cpp
 * @author 
 * @date 
 */
//=============================================================================
#include <AppLib/include/Util/app_util_KeyRepeatController.h>

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( util )


//-------------------------------------------------------------------------
/**
  * @brief コンストラクタ
  */
//-------------------------------------------------------------------------
KeyRepeatController::KeyRepeatController( u32 cursorPosMaxY , u32 cursorPosMaxX )
: m_pKey( NULL ) 
, m_pButton( NULL )
, m_cursorPosX( 0 )
, m_cursorPosY( 0 )
, m_cursorPosMaxX( cursorPosMaxX )
, m_cursorPosMaxY( cursorPosMaxY )
{
     
}

//-------------------------------------------------------------------------
/**
  * @brief デストラクタ
  */
//-------------------------------------------------------------------------
KeyRepeatController::~KeyRepeatController()
{
}

//-------------------------------------------------------------------------
/**
  * @brief マックスを再設定
  */
//-------------------------------------------------------------------------
void KeyRepeatController::SetMaxPos( u32 cursorPosMaxY ,  u32 cursorPosMaxX )
{
  m_cursorPosMaxX = cursorPosMaxX;
  m_cursorPosMaxY = cursorPosMaxY;
}

//-------------------------------------------------------------------------
/**
  * @brief 判定情報更新
  */
//-------------------------------------------------------------------------
void KeyRepeatController::UpdateInfo( gfl2::ui::VectorDevice * key , u32 cursorPosY ,  u32 cursorPosX , gfl2::ui::Button * button )
{
  m_pKey = key;
  m_cursorPosY = cursorPosY;
  m_cursorPosX = cursorPosX;
  m_pButton = button;//ここにボタンを設定するとボタンのLRも左右で取るようになる
}

//-------------------------------------------------------------------------
/**
  * @brief リピート判定取得
  */
//-------------------------------------------------------------------------
bool KeyRepeatController::IsRepeat( u32 dir )
{
  GFL_ASSERT( dir == gfl2::ui::DIR_UP || //@fix
              dir == gfl2::ui::DIR_DOWN || 
              dir == gfl2::ui::DIR_LEFT || 
              dir == gfl2::ui::DIR_RIGHT );//@fix

  if( m_pKey == NULL )
  {
    return false;
  }

  gfl2::ui::Button *pButton = NULL;
  u32 buttonID = 0;
  if( dir == gfl2::ui::DIR_LEFT || dir == gfl2::ui::DIR_RIGHT )
  {//LRと左右を同期させる
    pButton = m_pButton;
    buttonID = ( dir == gfl2::ui::DIR_LEFT ) ? gfl2::ui::BUTTON_L : gfl2::ui::BUTTON_R;
  }

  bool isLimit = false;

  switch( dir )
  {
  case gfl2::ui::DIR_UP:
    if( m_cursorPosY == 0 )
    {
      isLimit = true;
    }
  break;
  case gfl2::ui::DIR_DOWN:
    if( m_cursorPosY == m_cursorPosMaxY )
    {
      isLimit = true;
    }
  break;
  case gfl2::ui::DIR_LEFT:
    if( m_cursorPosX == 0 )
    {
      isLimit = true;
    }
  break;
  case gfl2::ui::DIR_RIGHT:
    if( m_cursorPosX == m_cursorPosMaxX )
    {
      isLimit = true;
    }
  break;
  }

  if( isLimit )
  {//端
    return m_pKey->IsTrigger( dir ) || ( pButton && pButton->IsTrigger( buttonID ) );
  }
  else
  {//通常リピート
    return m_pKey->IsRepeat( dir ) || ( pButton && pButton->IsRepeat( buttonID ) );
  }  
}




GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )
