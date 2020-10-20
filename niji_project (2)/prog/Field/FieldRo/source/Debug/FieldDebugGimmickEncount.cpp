//======================================================================
/**
 * @file FieldDebugGimmickEncount.cpp
 * @date 2015/09/08 14:55:22
 * @author saita_kazuki
 * @brief デバッグ：GimmickEncount
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#include "Field/FieldRo/include/Debug/FieldDebugGimmickEncount.h"

// gfl2
#include <Debug/include/gfl2_DebugWinItemTypes.h>
// GameSys
#include "GameSys/include/GameManager.h"
// field
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
// System
#include "System/include/MotionBlur.h"

// メッセージのインデックス
#include "niji_conv_header/message/debug/msg_debug_menu.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

bool DebugGimmickEncount::m_isSeEnable = false;
f32 DebugGimmickEncount::m_playSeLength = 15.0f;
f32 DebugGimmickEncount::m_volumeChangeLengthRate = 0.7f;
f32 DebugGimmickEncount::m_changeVolume = 0.7f;
bool DebugGimmickEncount::m_isDebugMotionBlurEnable = false;
u32 DebugGimmickEncount::m_motionBlurBlendType = 0;
u32 DebugGimmickEncount::m_motionBlurAnimeType = 0;
bool DebugGimmickEncount::m_isEncountEffectEnable = false;

bool DebugGimmickEncount::m_isDebugMotionBlurBlendInterpEnable = false;
u32 DebugGimmickEncount::m_motionBlurBlendInterpType = 0;
f32 DebugGimmickEncount::m_motionBlurBlendEnd = 0.0f;
s32 DebugGimmickEncount::m_motionBlurBlendFrame = 0;
bool DebugGimmickEncount::m_isDebugMotionBlurColorInterpEnable = false;
u32 DebugGimmickEncount::m_motionBlurColorInterpType = 0;
gfl2::math::Vector3 DebugGimmickEncount::m_motionBlurColorEnd = gfl2::math::Vector3::GetZero();
s32 DebugGimmickEncount::m_motionBlurColorFrame = 0;

/**
 *  @brief  Group ctor
 */
static void CreateDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugGimmickEncount *pDebug = reinterpret_cast<DebugGimmickEncount*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData = pDebug->GetMessageData();
    gfl2::debug::DebugWin_AddItemValueBool( pGroup, pHeap, pMsgData, msg_dmenu_field_gmcencount_enable, &DebugTypes::m_isJumpMoveEnable);
    gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pHeap, pMsgData, msg_dmenu_field_gmcencount_speed, &DebugTypes::m_jumpMoveSpeed, 0.0f, 30.0f, 0.01f);
    gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pHeap, pMsgData, msg_dmenu_field_gmcencount_scale, &DebugTypes::m_pokeBaseScale, 0.01f, 10.0f, 0.01f);
    gfl2::debug::DebugWin_AddItemValueBool( pGroup, pHeap, pMsgData, msg_dmenu_field_gmcencount_use_ease_func, &DebugTypes::m_isEasingEnable);
    gfl2::debug::DebugWin_AddItemUpdateFunc( 
      pGroup, 
      pHeap, 
      pMsgData,
      msg_dmenu_field_gmcencount_ease_func,
      &DebugTypes::m_easingFunc,
      DebugGimmickEncount::UpdateEaseFunc,
      DebugGimmickEncount::DrawEaseFunc
      );
    gfl2::debug::DebugWin_AddItemValueBool( pGroup, pHeap, pMsgData, msg_dmenu_field_gmcencount_use_se_dbg, &DebugGimmickEncount::m_isSeEnable);
    gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pHeap, pMsgData, msg_dmenu_field_gmcencount_se_length, &DebugGimmickEncount::m_playSeLength, 0.0f, 100.0f, 0.1f);
    gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pHeap, pMsgData, msg_dmenu_field_gmcencount_se_len_rate, &DebugGimmickEncount::m_volumeChangeLengthRate, 0.0f, 1.0f, 0.01f);
    gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pHeap, pMsgData,msg_dmenu_field_gmcencount_se_volume, &DebugGimmickEncount::m_changeVolume, 0.0f, 1.0f, 0.01f);
    gfl2::debug::DebugWin_AddItemValueBool( pGroup, pHeap, pMsgData, msg_dmenu_field_gmcencount_blur_enable, &DebugGimmickEncount::m_isDebugMotionBlurEnable);
    gfl2::debug::DebugWin_AddItemValueU32( pGroup, pHeap, pMsgData,msg_dmenu_field_gmcencount_blur_blend, &DebugGimmickEncount::m_motionBlurBlendType, 0, System::MotionBlur::BLEND_MAX-1, 1);
    gfl2::debug::DebugWin_AddItemValueU32( pGroup, pHeap, pMsgData,msg_dmenu_field_gmcencount_blur_anime, &DebugGimmickEncount::m_motionBlurAnimeType, 0, System::MotionBlur::ANIM_MAX-1, 1);

    gfl2::debug::DebugWin_AddItemValueBool( pGroup, pHeap, pMsgData, msg_dmenu_field_gmcencount_blur_blend_interp_enable, &DebugGimmickEncount::m_isDebugMotionBlurBlendInterpEnable);
    gfl2::debug::DebugWin_AddItemValueU32( pGroup, pHeap, pMsgData,msg_dmenu_field_gmcencount_blur_type, &DebugGimmickEncount::m_motionBlurBlendInterpType, 0, System::MotionBlur::TYPE_END-1, 1);
    gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pHeap, pMsgData,msg_dmenu_field_gmcencount_blur_end, &DebugGimmickEncount::m_motionBlurBlendEnd, -0.5f, 0.5f, 0.01f);
    gfl2::debug::DebugWin_AddItemValueInt( pGroup, pHeap, pMsgData,msg_dmenu_field_gmcencount_blur_frame, &DebugGimmickEncount::m_motionBlurBlendFrame, 0, 30, 1);

    gfl2::debug::DebugWin_AddItemValueBool( pGroup, pHeap, pMsgData, msg_dmenu_field_gmcencount_blur_blend_color_enable, &DebugGimmickEncount::m_isDebugMotionBlurColorInterpEnable);
    gfl2::debug::DebugWin_AddItemValueU32( pGroup, pHeap, pMsgData,msg_dmenu_field_gmcencount_blur_type, &DebugGimmickEncount::m_motionBlurColorInterpType, 0, System::MotionBlur::TYPE_END-1, 1);
    gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pHeap, pMsgData,msg_dmenu_field_gmcencount_blur_end_color, &DebugGimmickEncount::m_motionBlurColorEnd.x, 0.0f, 1.0f, 0.01f);
    gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pHeap, pMsgData,msg_dmenu_field_gmcencount_blur_end_color, &DebugGimmickEncount::m_motionBlurColorEnd.y, 0.0f, 1.0f, 0.01f);
    gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pHeap, pMsgData,msg_dmenu_field_gmcencount_blur_end_color, &DebugGimmickEncount::m_motionBlurColorEnd.z, 0.0f, 1.0f, 0.01f);
    gfl2::debug::DebugWin_AddItemValueInt( pGroup, pHeap, pMsgData,msg_dmenu_field_gmcencount_blur_frame, &DebugGimmickEncount::m_motionBlurColorFrame, 0, 30, 1);
    //gfl2::debug::DebugWin_AddItemValueBool( pGroup, pHeap, L"エンカウントエフェクトON/OFF", &DebugGimmickEncount::m_isEncountEffectEnable);

    // @attention メッセージを使いまわしている
    // オニドリル、ウォーグルのみ専用の値を調整するためのパラメーター
    gfl2::debug::DebugWin_AddItemValueBool( pGroup, pHeap, pMsgData, msg_dmenu_field_gmcencount_blur_blend_color_enable, &DebugTypes::m_isPokeStartPosYEnable);
    gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pHeap, pMsgData,msg_dmenu_field_gmcencount_blur_end_color, &DebugTypes::m_pokeStartPosYOffset, 100.0f, 300.0f, 1.0f);
  }
}
/**
 *  @brief  Group dtor
 */
static void DestroyDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pGroup )
  {
    pGroup->RemoveAllItem();
  }
}

/**
 * @class DebugGimmickEncount
 * @brief クラスの説明
 */
/**
 *  @brief  コンストラクタ
 */
DebugGimmickEncount::DebugGimmickEncount( void )
  : m_pHeap( NULL)
{
}

/**
 *  @brief  デストラクタ
 */
DebugGimmickEncount::~DebugGimmickEncount( void )
{
  Terminate();
}
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  初期化
 */
void DebugGimmickEncount::Initialize( gfl2::heap::HeapBase *pHeap, gfl2::str::MsgData* pMsgData )
{
  m_pHeap = pHeap;
  m_pMsgData = pMsgData;
}

/**
 *  @brief  破棄
 */
void DebugGimmickEncount::Terminate( void )
{
}

/**
 *  @brief  メニューの作成
 */
void DebugGimmickEncount::CreateDebugMenu( gfl2::debug::DebugWinGroup *root )
{
  gfl2::debug::DebugWinGroup *pGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_field_gmcencount_top, root );
  pGroup->SetCreateDestroyGroupFunc( this, CreateDebugGroupFunc, DestroyDebugGroupFunc );
}

void DebugGimmickEncount::UpdateEaseFunc( void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  const gfl2::ui::VectorDevice*  pCross  = pDeviceManager->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );


  s32* pValue = (s32*)userWork;
  if( pCross->IsRepeat( gfl2::ui::DIR_LEFT ) )
  {
    if( *pValue == 0 )
    {
      *pValue = gfl2::math::Easing::INOUT_BOUNCE;
    }
    else
    {
      *pValue -= 1;
    }
  }

  if( pCross->IsRepeat( gfl2::ui::DIR_RIGHT ) )
  {
    if( *pValue >= gfl2::math::Easing::INOUT_BOUNCE )
    {
      *pValue = 0;
    }
    else
    {
      *pValue += 1;
    }
  }
}

wchar_t* DebugGimmickEncount::DrawEaseFunc( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  s32* pValue = (s32*)userWork;
  s32 value = *pValue;

  wchar_t* table[] =
  {
    L"LINEAR",
    
    L"IN_SINE",
    L"OUT_SINE",
    L"INOUT_SINE",
    
    L"IN_QUAD",
    L"OUT_QUAD",
    L"INOUT_QUAD",
    
    L"IN_CUBIC",
    L"OUT_CUBIC",
    L"INOUT_CUBIC",
    
    L"IN_QUART",
    L"OUT_QUART",
    L"INOUT_QUART",

    L"IN_QUINT",
    L"OUT_QUINT",
    L"INOUT_QUINT",

    L"IN_EXPO",
    L"OUT_EXPO",
    L"INOUT_EXPO",

    L"IN_CIRC",
    L"OUT_CIRC",
    L"INOUT_CIRC",

    L"IN_BACK",
    L"OUT_BACK",
    L"INOUT_BACK",

    L"IN_ELASTIC",
    L"OUT_ELASTIC",
    L"INOUT_ELASTIC",

    L"IN_BOUNCE",
    L"OUT_BOUNCE",
    L"INOUT_BOUNCE"
  };

  if( value >= GFL_NELEMS( table ) )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ERROR(%d)]", item->GetName(), value );
  }
  else
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%ls]", item->GetName(), table[ value ] );
  }
  return workStr;

}

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // PM_DEBUG
