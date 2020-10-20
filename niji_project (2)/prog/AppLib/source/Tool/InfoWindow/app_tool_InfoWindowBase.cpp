//======================================================================
/**
 * @file app_tool_InfoWindow.cpp
 * @date 2015/07/02 15:49:24
 * @author kawada_koji
 * @brief インフォウィンドウ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================


// gfl2のインクルード
#include <fs/include/gfl2_Fs.h>
#include <str/include/gfl2_StrSys.h>
#include <str/include/gfl2_StrBuf.h>
#include <thread/include/gfl2_ThreadStatic.h>

// nijiのデータのヘッダーファイルのインクルード
#include <arc_def_index/arc_def.h>
#include <arc_index/app_information.gaix>
#include <niji_conv_header/app/information/res2d/information_sml.h>
#include <niji_conv_header/app/information/res2d/information_sml_anim_list.h>
#include <niji_conv_header/app/information/res2d/information_sml_pane.h>
#include <niji_conv_header/app/information/res2d/information_lrg.h>
#include <niji_conv_header/app/information/res2d/information_lrg_anim_list.h>
#include <niji_conv_header/app/information/res2d/information_lrg_pane.h>

// nijiのインクルード
#include "AppLib/include/ui/UIInputListener.h"

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Util/app_util_FileAccessor.h>

#include <Sound/include/Sound.h>

#include "app_tool_InfoWindowDefine.h"
#include "app_tool_InfoWindowBase.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)


//==============================================================================
//
//  InfoWindowBase
//
InfoWindowBase::InfoWindowBase(app::util::Heap* appHeap)
  : app::ui::UIView( appHeap, appHeap->GetDeviceAllocator(), appHeap->GetDeviceAllocator() ),
    m_appHeap(NULL),
    m_appRenderingManager(NULL),
    m_dataManager(NULL),
    m_isEndFuncCalled(false),
    m_createState(CREATE_STATE_NONE),
    m_isDataManagerSelfCreated(false),
    m_openStartPosPane(NULL),
    m_openEndPosPane(NULL),
    m_bgPane(NULL),
    m_bgVisible(false),
    m_SEIsOn(true)
{}
InfoWindowBase::~InfoWindowBase()
{}

void InfoWindowBase::StartAsyncCreate(
    app::util::Heap* appHeap,
    app::util::AppRenderingManager* appRenderingManager,
    InfoWindowDataManager* dataManager
)
{
  if(m_isEndFuncCalled == false)  // CreateはEndFuncが呼び出された後は禁止。
  {
    if(m_createState == CREATE_STATE_NONE)
    {
      m_appHeap = appHeap;
      m_appRenderingManager = appRenderingManager;
    
      // データマネージャ生成
      m_createState = CREATE_STATE_DATA_MANAGER_CREATING;
      this->startAsyncCreateDataManager(dataManager);

      if( m_dataManager->IsCreated() )
      {
        this->createLayoutFromCreatingToCreated();
      }
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。  // 既に生成中か生成済み。
    }
  }
  else
  {
    GFL_ASSERT(0);  // EndFuncが呼び出された後。  // 開発中に気付かせるためのASSERT
  }
}

void InfoWindowBase::SyncCreate(
    app::util::Heap* appHeap,
    app::util::AppRenderingManager* appRenderingManager,
    InfoWindowDataManager* dataManager
)
{
  if(m_isEndFuncCalled == false)  // CreateはEndFuncが呼び出された後は禁止。
  {
    if(m_createState == CREATE_STATE_NONE)
    {
      m_appHeap = appHeap;
      m_appRenderingManager = appRenderingManager;
      
      // データマネージャ生成
      m_createState = CREATE_STATE_DATA_MANAGER_CREATING;
      this->syncCreateDataManager(dataManager);

      // 同期生成なので、この時点でデータマネージャの生成は完了している。
      {
        this->createLayoutFromCreatingToCreated();
      }
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。  // 既に生成中か生成済み。
    }
  }
  else
  {
    GFL_ASSERT(0);  // EndFuncが呼び出された後。  // 開発中に気付かせるためのASSERT
  }
}

bool InfoWindowBase::IsCreated(void) const
{
  return (m_createState == CREATE_STATE_CREATED);
}

bool InfoWindowBase::EndFunc(void)
{
  m_isEndFuncCalled = true;

  this->localUpdate();
  
  bool canDestroy = ( m_createState == CREATE_STATE_NONE || m_createState == CREATE_STATE_CREATED );

  if(canDestroy)
  {
    // 破棄
    this->destroyLayout();
    this->destroyDataManager();
  }

  m_createState = CREATE_STATE_NONE;

  return canDestroy;
}

void InfoWindowBase::Update(void)
{
  app::ui::UIView::Update();

  this->localUpdate();
}

void InfoWindowBase::SetBgVisible(bool visible)
{
  if(m_isEndFuncCalled == false)  // EndFuncが呼び出された後は禁止。
  {
    m_bgVisible = visible;
    if(m_bgPane)
    {
      m_bgPane->SetVisible(m_bgVisible);
    }
  }
  else
  {
    GFL_ASSERT(0);  // EndFuncが呼び出された後。  // 開発中に気付かせるためのASSERT
  }
}
bool InfoWindowBase::IsBgVisible(void) const
{
  return m_bgVisible;
}

void InfoWindowBase::SetSEOn(bool isOn)
{
  m_SEIsOn = isOn;
}
bool InfoWindowBase::IsSEOn(void) const
{
  return m_SEIsOn;
}

void InfoWindowBase::Draw(gfl2::gfx::CtrDisplayNo /*displayNo*/)
{}
void InfoWindowBase::Draw(gfl2::gfx::CtrDisplayNo /*displayNo*/, u8 /*order*/, u32 /*layerNo*/)
{}

void InfoWindowBase::destructa(void)
{
  bool endFlag = this->EndFunc();
  GFL_ASSERT(endFlag);  // 開発中に気付かせるためのASSERT。ケア済み。  // EndFuncがtrueを返すようになってから破棄して下さい。

  // 終了処理が終わるのを待つ
  u32 loopCount = 0; 
  while( this->EndFunc() == false )
  {
    ++loopCount;
    if(loopCount >= InfoWindowDefine::LOOP_COUNT_MAX) break;  // 無限ループ対策
    gfl2::thread::ThreadStatic::CurrentSleep(1);
  }

  // 終了処理が終わる前にここに来てしまったら、メモリ確保したものが残っている。
  // 破棄
  this->destroyLayout();
  this->destroyDataManager();
}

void InfoWindowBase::startAsyncCreateDataManager(InfoWindowDataManager* dataManager)
{
  if(dataManager == NULL)
  {
    m_isDataManagerSelfCreated = true;
    m_dataManager = GFL_NEW(m_appHeap->GetDeviceHeap()) InfoWindowDataManager();
    m_dataManager->StartAsyncCreate(m_appHeap, this->GetDataManagerSelfCreateType());
  }
  else
  {
    m_isDataManagerSelfCreated = false;
    m_dataManager = dataManager;
    if( m_dataManager->IsCreated() == false )
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。使い方が間違っているので開発中に起こらないようにすること。  // データマネージャを生成してから渡して下さい。
    }
  }
}

void InfoWindowBase::syncCreateDataManager(InfoWindowDataManager* dataManager)
{
  if(dataManager == NULL)
  {
    m_isDataManagerSelfCreated = true;
    m_dataManager = GFL_NEW(m_appHeap->GetDeviceHeap()) InfoWindowDataManager();
    m_dataManager->SyncCreate(m_appHeap, this->GetDataManagerSelfCreateType());
  }
  else
  {
    m_isDataManagerSelfCreated = false;
    m_dataManager = dataManager;
    if( m_dataManager->IsCreated() == false )
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。使い方が間違っているので開発中に起こらないようにすること。  // データマネージャを生成してから渡して下さい。
    }
  }
}

void InfoWindowBase::destroyDataManager(void)
{
  if(m_isDataManagerSelfCreated)
  {
    // CREATE_STATE_CREATEDになるまで処理を続けてからここに来るので、
    // m_dataManager->EndFunc
    // は呼ばなくてよい。

    GFL_SAFE_DELETE(m_dataManager);
  }
}

void InfoWindowBase::localUpdate(void)
{
  if(m_createState == CREATE_STATE_DATA_MANAGER_CREATING)
  {
    m_dataManager->UpdateFunc();
    
    if( m_dataManager->IsCreated() )
    {
      this->createLayoutFromCreatingToCreated();
    }
  }
}

void InfoWindowBase::createLayoutFromCreatingToCreated(void)
{
  // レイアウト生成
  m_createState = CREATE_STATE_LAYOUT_CREATING;
  this->createLayout();
  m_bgPane->SetVisible(m_bgVisible);
  m_appHeap = NULL;
  m_createState = CREATE_STATE_CREATED;
}


GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

