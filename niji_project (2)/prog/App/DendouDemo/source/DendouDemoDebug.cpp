//======================================================================
/**
 * @file DendouDemoDebug.cpp
 * @date 2016/03/08 17:31:36
 * @author nagihashi_toru
 * @brief 殿堂デモのデバッグ機能
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if PM_DEBUG


#include <System/include/HeapDefine.h>
#include <heap/include/gfl2_Heap.h>
#include <ui/include/gfl2_UI.h>
#include <GameSys/include/GameManager.h>
#include "DendouDemoDebug.h"

SINGLETON_INSTANCE_DEFINITION(app::dendou_demo::Debug)
template class gfl2::base::SingletonAccessor<app::dendou_demo::Debug>;


namespace app {
namespace dendou_demo {

/**
  *  @brief  ctor
  */
Debug::Debug() :
  mIsReset(false),
  mIsLoop(false),
  mUseDebugPokemon(false),
  mIsAging(false),
  mLightPos(),
  mLightDir(),
  mLightPokeColor(),
  mLightDirColor(),
  mLightAmvColor(),
  mPokeDirLight(NULL),
  mTrainerDirLight(NULL),
  mTrainerAmbLight(NULL)
{
}
/**
  *  @brief  デストラクタ
  */
Debug::~Debug()
{

}

/**
*  @brief  ライト調整用にポインタを貰う
*/
void Debug::SetupPokemonDirectionalLight( gfl2::renderingengine::scenegraph::instance::LightNode* pokeDirLight )
{
  mPokeDirLight = pokeDirLight;
  mLightPokeColor = mPokeDirLight->GetColor();
  mLightDir = mPokeDirLight->GetDirection();
}
void Debug::SetupTrainerDirectionalLight( gfl2::renderingengine::scenegraph::instance::LightNode* trainerDirLight )
{
  mTrainerDirLight  = trainerDirLight;
  mLightDirColor = mTrainerDirLight->GetColor();
}
void Debug::SetupTrainerAmbiaentLight( gfl2::renderingengine::scenegraph::instance::LightNode* trainerAmbLight )
{
  mTrainerAmbLight  = trainerAmbLight;
  mLightAmvColor = mTrainerAmbLight->GetColor();
}


/**
*  @brief  カメラにポインタを貰う
*/
void Debug::SetupCamera( poke_3d::model::BaseCamera* camera )
{
  mCamera = camera;
}

/**
  *  @brief  Aging用アップデート処理(終了時呼ぶこと)
  */
void Debug::UpdatePokemonToAging()
{
  if( mIsAging )
  {
    ++mAgingCounter;

    for (int i = 0; i < 6; ++i)
    {
      mSimpleParam[i].monsNo = (MonsNo)((mAgingCounter - MONSNO_MAX *i) % MONSNO_MAX);
      if( mSimpleParam[i].monsNo < 0 ) 
      {
        mSimpleParam[i].monsNo = MONSNO_NULL;
      }
    }
  }
}

/**
  *  @brief  シングルトン生成
  */
void Debug::InitializeSingleton()
{
  gfl2::heap::HeapBase *heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
  Debug* instance = GFL_NEW(heap) Debug();
  instance->Initialize();

}
/**
  *  @brief  シングルトン破棄
  */
void Debug::FinalizeSingleton()
{
  Debug* instance = GFL_SINGLETON_INSTANCE(Debug);
  instance->Finalize();
  GFL_DELETE( instance );
}

/**
  *  @brief  初期化
  */
void Debug::Initialize()
{
  AddDebugWin();
}
/**
  *  @brief  破棄
  */
void Debug::Finalize()
{
  RemoveDebugWin();
}

/**
  *  @brief  DebugWin破棄
  */
void Debug::AddDebugWin()
{
  gfl2::heap::HeapBase *heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
  m_group     = gfl2::debug::DebugWin_AddGroup(heap, L"Dendou", gfl2::debug::DebugWin_GetTopGroup() );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_group, heap, L"StopCamera", this, DebugWinUpdateFunc_StopCamera, DebugWinDrawFunc_StopCamera );
  gfl2::debug::DebugWin_AddItemValueBool(m_group, heap, L"Reset", &mIsReset);
  gfl2::debug::DebugWin_AddItemValueBool(m_group, heap, L"Loop", &mIsLoop);
  gfl2::debug::DebugWin_AddItemValueBool(m_group, heap, L"Aging", &mIsAging);
  gfl2::debug::DebugWin_AddItemValueBool(m_group, heap, L"UseDebugPoke", &mUseDebugPokemon);

  m_groupPoke = gfl2::debug::DebugWin_AddGroup(heap, L"DebugPoke", m_group );
  gfl2::debug::DebugWin_AddItemValueInt( m_groupPoke, heap, L"操作ポケ", &mPokeIndex, 0, 6);
  gfl2::debug::DebugWin_AddItemValueInt( m_groupPoke, heap, L"MonsNo", (int*)&this->mSimpleParam[mPokeIndex].monsNo, 0, MONSNO_MAX);
  gfl2::debug::DebugWin_AddItemValueU8( m_groupPoke, heap, L"FormNo", &this->mSimpleParam[mPokeIndex].formNo, 0, 30);
  gfl2::debug::DebugWin_AddItemValueBool(m_groupPoke, heap, L"isRare", &this->mSimpleParam[mPokeIndex].isRare);
  gfl2::debug::DebugWin_AddItemValueBool(m_groupPoke, heap, L"isEgg", &this->mSimpleParam[mPokeIndex].isEgg);
  gfl2::debug::DebugWin_AddItemValueU32( m_groupPoke, heap, L"perRand", &this->mSimpleParam[mPokeIndex].perRand, 0, 0xFFFFFFFF);

  m_groupLight = gfl2::debug::DebugWin_AddGroup(heap, L"Light", m_group );
  gfl2::debug::DebugWin_AddItemValueVector3( m_groupLight, heap , L"向き", &mLightDir, -9999.0f, 9999.0f, 0.1f );
  gfl2::debug::DebugWin_AddItemValueVector4( m_groupLight, heap , L"色[poke]", &mLightPokeColor, -9999.0f, 9999.0f, 0.1f );
  gfl2::debug::DebugWin_AddItemValueVector4( m_groupLight, heap , L"色[dir]", &mLightDirColor, -9999.0f, 9999.0f, 0.1f );
  gfl2::debug::DebugWin_AddItemValueVector4( m_groupLight, heap , L"色[amb]", &mLightAmvColor, -9999.0f, 9999.0f, 0.1f );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_groupLight, heap, L"反映", this, DebugWinUpdateFunc_SetLight );
}
/**
  *  @brief  DebugWin作成
  */
void Debug::RemoveDebugWin()
{
  gfl2::debug::DebugWin_RemoveGroup(m_groupLight);
  gfl2::debug::DebugWin_RemoveGroup(m_groupPoke);
  gfl2::debug::DebugWin_RemoveGroup(m_group);
}

void Debug::Update_StopCamera()
{
  if( !mCamera ) { return; }

  gfl2::ui::Button* button = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if(button->IsTrigger(gfl2::ui::BUTTON_A))
  {
    if( mCamera->GetAnimationStepFrame() == 0 )
    {
      mCamera->SetAnimationStepFrame( 1.0f );
    }
    else
    {
      mCamera->SetAnimationStepFrame( 0.0f );
    }
  }
}
wchar_t* Debug::Draw_StopCamera()
{
  if( !mCamera ) { return L""; }

  if( mCamera->GetAnimationStepFrame() == 0 )
  {
    return L"カメラ開始";
  }
  else
  {
    return L"カメラ停止";
  }
}
void Debug::Update_SetLight()
{
  if( !mPokeDirLight ) { return; }

  gfl2::ui::Button* button = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if(button->IsTrigger(gfl2::ui::BUTTON_A))
  {
    mPokeDirLight->SetDirection(mLightDir);
    mPokeDirLight->SetColor(mLightPokeColor);
    mTrainerDirLight->SetDirection(mLightDir);
    mTrainerDirLight->SetColor(mLightDirColor);
    mTrainerAmbLight->SetDirection(mLightDir);
    mTrainerAmbLight->SetColor(mLightAmvColor);
  }
}

void Debug::DebugWinUpdateFunc_StopCamera( void* userWork, gfl2::debug::DebugWinItem *item )
{
  Debug * self = reinterpret_cast<Debug*>(userWork);
  self->Update_StopCamera();
}

wchar_t* Debug::DebugWinDrawFunc_StopCamera( void* userWork, gfl2::debug::DebugWinItem *item )
{
  Debug * self = reinterpret_cast<Debug*>(userWork);
  return self->Draw_StopCamera();
}
void Debug::DebugWinUpdateFunc_SetLight( void* userWork, gfl2::debug::DebugWinItem *item )
{
  Debug * self = reinterpret_cast<Debug*>(userWork);
  self->Update_SetLight();
}

}		// dendou_demo
}		// app
#endif //PM_DEBUG
