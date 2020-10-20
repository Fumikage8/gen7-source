//=============================================================================================
/**
 * @file  btl_ui.cpp
 * @brief ポケモンXY バトルシステム UI管理
 * @author  taya
 *
 * @date  2011.03.02  作成
 */
//=============================================================================================
#include <GameSys/include/GameManager.h>
#include "btl_ui.h"

GFL_NAMESPACE_BEGIN(btl)
namespace ui {


gfl2::ui::Button*             s_button  = NULL;
gfl2::ui::VectorDevice*       s_key     = NULL;
gfl2::ui::TouchPanel*         s_tp      = NULL;



//=============================================================================================
/**
 * システム初期化：namespace 'ui' 内部に各種デバイス管理モジュールを構築する
 *
 * @param   pHeap   モジュール構築用ヒープハンドル
 */
//=============================================================================================
void InitSystem( HeapHandle pHeap )
{
  gfl2::ui::DeviceManager* manager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
  if( manager )
  {
    s_button = manager->GetButton( 0 );
    s_key    = manager->GetCrossKey( 0 );
    s_tp     = manager->GetTouchPanel( 0 );
  }
}
//=============================================================================================
/**
 * システム終了：namespace 'ui' 内部に構築した各種デバイス管理モジュールを破棄する
 */
//=============================================================================================
void QuitSystem( void )
{
  s_button = NULL;
  s_key    = NULL;
  s_tp     = NULL;
}


bool CheckNextTrg( void )
{
  if( s_tp->IsTouchTrigger() ){
    return true;
  }
  if( s_button->IsTrigger(BUTTON_A | BUTTON_B) ){
    return true;
  }
  return false;

}

bool CheckKeyTrg( u32 keyMask )
{
  return s_button->IsTrigger( keyMask );
}

bool CheckKeyCont( u32 keyMask )
{
  bool ret = s_button->IsHold( keyMask );
  return ret;
}

bool CheckKeyRepeat( u32 keyMask )
{
  return s_button->IsRepeat( keyMask );
}

} // end of namespace 'ui'
GFL_NAMESPACE_END(btl)


