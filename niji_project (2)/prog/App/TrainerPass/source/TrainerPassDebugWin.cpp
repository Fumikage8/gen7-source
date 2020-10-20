#if PM_DEBUG
//=============================================================================
/**
 * @file    TrainerPassDebugWin.cpp
 * @brief   トレーナーパス画面デバッグウィンドウ処理
 * @author  Hiroyuki Nakamura
 * @date    2016.03.01
 */
//=============================================================================

// module
#include "TrainerPassDebugWin.h"
// gfl2
#include <debug/include/gfl2_DebugWinSystem.h>
#include <debug/include/gfl2_DebugWinItemTypes.h>
// niji
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "Debug/DebugWin/include/DebugWinSystem.h"
// resource
#include "niji_conv_header/message/debug/msg_debug_menu.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(TrainerPass)


//=============================================================================
//  TrainerPassDebugWinImpl
//=============================================================================
class TrainerPassDebugWinImpl
{
  GFL_FORBID_COPY_AND_ASSIGN(TrainerPassDebugWinImpl);

public:
  TrainerPassDebugWinImpl( gfl2::heap::HeapBase * heap, gfl2::debug::DebugWinGroup * parent_group, gfl2::str::MsgData * msg_data );
  virtual ~TrainerPassDebugWinImpl();

private:
  void Initialize( TrainerPassDebugWinImpl * inst );
  void Terminate(void);

  static void StampFlag_Update( void * userWork, gfl2::debug::DebugWinItem * item );
  static wchar_t * StampFlag_Draw( void * userWork, gfl2::debug::DebugWinItem * item );

private:
  gfl2::heap::HeapBase * m_heap;
  gfl2::debug::DebugWinGroup * m_parentGroup;
  gfl2::str::MsgData * m_pMsgData;

  gfl2::debug::DebugWinGroup * m_groupTop;
  gfl2::debug::DebugWinGroup * m_groupStamp;
};


TrainerPassDebugWinImpl::TrainerPassDebugWinImpl( gfl2::heap::HeapBase * heap, gfl2::debug::DebugWinGroup * parent_group, gfl2::str::MsgData * msg_data )
  : m_heap( heap )
  , m_parentGroup( parent_group )
  , m_pMsgData( msg_data )
{
  Initialize(this);
  
  // 親
  m_groupTop = gfl2::debug::DebugWin_AddGroup( m_heap, m_pMsgData, msg_dmenu_trainerpass_00, m_parentGroup );
  
  { // スタンプ
    m_groupStamp = gfl2::debug::DebugWin_AddGroup( m_heap, m_pMsgData, msg_dmenu_trainerpass_01, m_groupTop );

    static const u32 tbl[] =
    {
      msg_dmenu_trainerpass_02,
      msg_dmenu_trainerpass_03,
      msg_dmenu_trainerpass_04,
      msg_dmenu_trainerpass_05,
      msg_dmenu_trainerpass_06,
      msg_dmenu_trainerpass_07,
      msg_dmenu_trainerpass_08,
      msg_dmenu_trainerpass_09,
      msg_dmenu_trainerpass_10,
      msg_dmenu_trainerpass_11,
      msg_dmenu_trainerpass_12,
      msg_dmenu_trainerpass_13,
      msg_dmenu_trainerpass_14,
      msg_dmenu_trainerpass_15,
    };

    for( u32 i=0; i<GFL_NELEMS(tbl); i++ )
    {
      gfl2::debug::DebugWin_AddItemUpdateFunc( m_groupStamp, m_heap, m_pMsgData, tbl[i], reinterpret_cast<void*>(i+1), StampFlag_Update, StampFlag_Draw );
    }
  }
}

TrainerPassDebugWinImpl::~TrainerPassDebugWinImpl()
{
  gfl2::debug::DebugWin_RemoveGroup( m_groupStamp );
  gfl2::debug::DebugWin_RemoveGroup( m_groupTop );
  
  Terminate();
}

void TrainerPassDebugWinImpl::Initialize( TrainerPassDebugWinImpl * inst )
{
}

void TrainerPassDebugWinImpl::Terminate(void)
{
}

void TrainerPassDebugWinImpl::StampFlag_Update( void * userWork, gfl2::debug::DebugWinItem * /*item*/ )
{
  gfl2::ui::Button * button = gfl2::debug::DebugWin_GetUiButton();
  Savedata::Misc * misc = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetMisc();

  Savedata::TrainerPassStampID id = static_cast<Savedata::TrainerPassStampID>( reinterpret_cast<s32>(userWork) );

  if( button->IsTrigger(gfl2::ui::BUTTON_LEFT|gfl2::ui::BUTTON_RIGHT) )
  {
    if( misc->IsTrainerPassStamp(id) == false )
    {
      misc->SetTrainerPassStamp( id );
    }
    else
    {
      misc->ResetTrainerPassStamp( id );
    }
  }
}

wchar_t * TrainerPassDebugWinImpl::StampFlag_Draw( void * userWork, gfl2::debug::DebugWinItem * item )
{
  wchar_t * work_str = gfl2::debug::DebugWin_GetWorkStr();
  const Savedata::Misc * misc = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetMisc();

  Savedata::TrainerPassStampID id = static_cast<Savedata::TrainerPassStampID>( reinterpret_cast<s32>(userWork) );

  s32 val = 0;
  if( misc->IsTrainerPassStamp(id) != false )
  {
    val = 1;
  }
  swprintf( work_str, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), val );

  return work_str;
}


//==============================================================================
//  TrainerPassDebugWin
//==============================================================================
TrainerPassDebugWin::TrainerPassDebugWin( gfl2::heap::HeapBase * heap, gfl2::debug::DebugWinGroup * parent_group )
  : m_heap( heap )
  , m_parentGroup( parent_group )
  , m_pMsgData( NULL )
  , m_impl( NULL )
{
  ::Debug::DebugWin::DebugWinWork * devWork = GFL_SINGLETON_INSTANCE( ::Debug::DebugWin::DebugWinWork );
  if( devWork )
  {
    m_pMsgData = devWork->GetMessageData();
  }
  m_impl = GFL_NEW(m_heap) TrainerPassDebugWinImpl( m_heap, m_parentGroup, m_pMsgData );
}

TrainerPassDebugWin::~TrainerPassDebugWin()
{
  GFL_SAFE_DELETE(m_impl);
}


GFL_NAMESPACE_END(TrainerPass)
GFL_NAMESPACE_END(App)

#endif  // PM_DEBUG
