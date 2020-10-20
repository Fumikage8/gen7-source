//======================================================================
/**
 * @file  DebugWinSystem.cpp
 * @brief 15/02/19
 * @author  ariizumi
 * @data  niji�p�f�o�b�O�E�B���h�E����
 */
//======================================================================
#include <ui/include/gfl2_UI.h>
#include <Debug/include/gfl2_DebugWinItemTypes.h>
#include "System/include/GflvUse.h"

#include "DebugWin/include/DebugWinSystem.h"
#include "../../System/include/HeapDefine.h"
#include "GameSystem/include/GameManager.h"
#include "../../Print/include/SystemFont.h"
///#include "FieldScript/include/EventScriptCall.h"@saito�s�v

#include <system/include/Timer/gfl2_PerformanceCounter.h> //max2�d��`�E�E�E

#if USE_DEBUG_WIN
/** ---------------------------------------------------------------------
//! @brief  SINGLETON�̑O���Q�Ɛ錾
// ---------------------------------------------------------------------*/
SINGLETON_INSTANCE_DEFINITION( Debug::DebugWin::DebugWinWork );
template class gfl2::base::SingletonAccessor<Debug::DebugWin::DebugWinWork>;

#endif


GFL_NAMESPACE_BEGIN(Debug)
GFL_NAMESPACE_BEGIN(DebugWin)

class DebugWinWork;

extern void DebugWin_UpdateTickBar( void* userWork  , gfl2::debug::DebugWinItem *item );
extern wchar_t* DebugWin_DrawTickBar( void* userWork  , gfl2::debug::DebugWinItem *item );
extern void DebugWin_UpdateTick( void* userWork  , gfl2::debug::DebugWinItem *item );
extern wchar_t* DebugWin_DrawTick( void* userWork  , gfl2::debug::DebugWinItem *item );
extern void DebugWin_ResetTick( void* userWork  , gfl2::debug::DebugWinItem *item );
///extern void DebugWin_ScriptTest( void* userWork  , gfl2::debug::DebugWinItem *item );@saito
extern void DebugWin_UpdateFs( void* userWork  , gfl2::debug::DebugWinItem *item );
extern wchar_t* DebugWin_DrawFs( void* userWork  , gfl2::debug::DebugWinItem *item );

extern void DebugWin_UpdateGpuProfile( void* userWork  , gfl2::debug::DebugWinItem *item );
extern wchar_t* DebugWin_DrawGpuProfile( void* userWork  , gfl2::debug::DebugWinItem *item );
static void DebugWin_AddGpuProfileWin( gfl2::heap::HeapBase* heap );


//�f�o�b�O�V�X�e���̏�����
void InitializeSystem( gfl2::heap::HeapBase *parentHeap , gfl2::heap::HeapBase *parentDevHeap )
{
#if USE_DEBUG_WIN
  GameSystem::GameManager *gMan = GFL_SINGLETON_INSTANCE(GameSystem::GameManager);

  gfl2::heap::HeapBase *heap = GFL_CREATE_HEAP( parentHeap , HEAPID_DEBUGWIN , 0x60000 , gfl2::heap::HEAP_TYPE_EXP , true );
  gfl2::heap::HeapBase *devHeap = GFL_CREATE_HEAP( parentDevHeap , HEAPID_DEBUGWIN_DEVICE , 0x80000 , gfl2::heap::HEAP_TYPE_EXP , true );

  gfl2::debug::DebugWin_Initialize(heap,devHeap,GFL_SINGLETON_INSTANCE(view_print::SystemFont)->GetFont(), 128);
  //�f�o�C�X�}�l�[�W���[�̐ݒ�(�f�o�C�X�}�l�[�W���[���ς��Ȃ����1���OK
  gfl2::debug::DebugWin_SetDeviceManager( gMan->GetUiDeviceManager() );

  //�f�o�b�O�p�̃��[�N
  DebugWinWork *debWork = GFL_NEW(heap) DebugWinWork();
  gfl2::debug::DebugWin_SetCallbackClass(debWork);

  //----------------------------------------------------
  //�e���j���[�̒ǉ�

  //�������� @todo ���b�Z�[�W�t�@�C���Ή�
  //gfl2::debug::DebugWinGroup *tickGroup = gfl2::debug::DebugWin_AddGroup( heap , msgData , DebMenu_TopItem2 , topGroup );
  //gfl2::debug::DebugWin_AddItemUpdateFunc( tickGroup , msgData , DebMenu_TickItem1 , NULL , DebugWin_UpdateTickBar , DebugWin_DrawTickBar );
  //gfl2::debug::DebugWin_AddItemUpdateFunc( tickGroup , msgData , DebMenu_TickItem2 , reinterpret_cast<void*>((int)DEBUGWIN_TICK_UPDATE) , DebugWin_UpdateTick , DebugWin_DrawTick );
  //gfl2::debug::DebugWin_AddItemUpdateFunc( tickGroup , msgData , DebMenu_TickItem3 , reinterpret_cast<void*>((int)DEBUGWIN_TICK_UPNEXT) , DebugWin_UpdateTick , DebugWin_DrawTick );
  //gfl2::debug::DebugWin_AddItemUpdateFunc( tickGroup , msgData , DebMenu_TickItem4 , reinterpret_cast<void*>((int)DEBUGWIN_TICK_DRAW)   , DebugWin_UpdateTick , DebugWin_DrawTick );
  //gfl2::debug::DebugWin_AddItemUpdateFunc( tickGroup , msgData , DebMenu_TickItem5 , reinterpret_cast<void*>((int)DEBUGWIN_TICK_GPU)    , DebugWin_UpdateTick , DebugWin_DrawTick );
  //gfl2::debug::DebugWin_AddItemUpdateFunc( tickGroup , msgData , DebMenu_TickItem6 , NULL , DebugWin_ResetTick , NULL );

  gfl2::debug::DebugWinGroup *tickGroup = gfl2::debug::DebugWin_AddGroup( heap , L"�����ӂ�" );
  gfl2::debug::DebugWin_AddItemUpdateFunc( tickGroup , heap , L"" , NULL , DebugWin_UpdateTickBar , DebugWin_DrawTickBar );
  gfl2::debug::DebugWin_AddItemUpdateFunc( tickGroup , heap , L"Update" , reinterpret_cast<void*>((int)DEBUGWIN_TICK_UPDATE) , DebugWin_UpdateTick , DebugWin_DrawTick );
  gfl2::debug::DebugWin_AddItemUpdateFunc( tickGroup , heap , L"UpNext" , reinterpret_cast<void*>((int)DEBUGWIN_TICK_UPNEXT) , DebugWin_UpdateTick , DebugWin_DrawTick );
  gfl2::debug::DebugWin_AddItemUpdateFunc( tickGroup , heap , L"Draw" , reinterpret_cast<void*>((int)DEBUGWIN_TICK_DRAW)   , DebugWin_UpdateTick , DebugWin_DrawTick );
  gfl2::debug::DebugWin_AddItemUpdateFunc( tickGroup , heap , L"GPU" , reinterpret_cast<void*>((int)DEBUGWIN_TICK_GPU)    , DebugWin_UpdateTick , DebugWin_DrawTick );
  gfl2::debug::DebugWin_AddItemUpdateFunc( tickGroup , heap , L"Reset" , NULL , DebugWin_ResetTick , NULL );
  /*
  gfl2::debug::DebugWinGroup *scriptGroup = gfl2::debug::DebugWin_AddGroup( heap , L"�X�N���v�g" );
  gfl2::debug::DebugWin_AddItemUpdateFunc( scriptGroup , heap , L"�e�X�g" , gMan , DebugWin_ScriptTest , DebugWin_DrawTickBar );

  gfl2::debug::DebugWinGroup *fsGroup = gfl2::debug::DebugWin_AddGroup( heap , L"�t�@�C��" );
  gfl2::debug::DebugWin_AddItemUpdateFunc( fsGroup , heap , L"PC�t�@�C����ǂ�"   , reinterpret_cast<void*>(0) , DebugWin_UpdateFs , DebugWin_DrawFs );
  gfl2::debug::DebugWin_AddItemUpdateFunc( fsGroup , heap , L"garc���ǂ݃V�X�e��" , reinterpret_cast<void*>(1) , DebugWin_UpdateFs , DebugWin_DrawFs );
  gfl2::debug::DebugWin_AddItemUpdateFunc( fsGroup , heap , L"garc�̌���ǂ�"     , reinterpret_cast<void*>(2) , DebugWin_UpdateFs , DebugWin_DrawFs );
  *///@saito
  // GPUProfile�ǉ�
  DebugWin_AddGpuProfileWin( heap);

#endif //USE_DEBUG_WIN
}

//�f�o�b�O�V�X�e���̏I��
void FinalizeSystem()
{
#if USE_DEBUG_WIN

  gfl2::debug::DebugWin_Finalize();
  DebugWinWork *debWork = GFL_SINGLETON_INSTANCE(DebugWinWork);
  GFL_SAFE_DELETE( debWork );
  GFL_DELETE_HEAP( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUGWIN_DEVICE) );
  GFL_DELETE_HEAP( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUGWIN) );
  

#endif //USE_DEBUG_WIN
}

//Tick�v���p
void TickStart( int tickType )
{
#if USE_DEBUG_WIN
  GFL_SINGLETON_INSTANCE(DebugWinWork)->TickStart(tickType);
#endif //USE_DEBUG_WIN
}

void TickEnd( int tickType )
{
#if USE_DEBUG_WIN
  GFL_SINGLETON_INSTANCE(DebugWinWork)->TickEnd(tickType);
#endif //USE_DEBUG_WIN
}

void TickCalc( int tickType , s64 microSec )
{
#if USE_DEBUG_WIN
  GFL_SINGLETON_INSTANCE(DebugWinWork)->TickCalc(tickType,microSec);
#endif //USE_DEBUG_WIN
}


#if USE_DEBUG_WIN

//---------------------------------------------------------------------------
//�������׃f�o�b�O���j���[�p�֐�
void DebugWin_UpdateTickBar( void* userWork  , gfl2::debug::DebugWinItem *item )
{
#if GFL_DEBUGWIN_USE
  GFL_UNUSED(item);
  GFL_UNUSED(userWork);

  DebugWinWork *work = GFL_SINGLETON_INSTANCE(DebugWinWork);

  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();

  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    item->PlaySystemSound(gfl2::debug::SE_CHANGE_VALUE);
    work->SetMode60( !work->IsMode60() );
  }
#endif
}

wchar_t* DebugWin_DrawTickBar( void* userWork  , gfl2::debug::DebugWinItem *item )
{
#if GFL_DEBUGWIN_USE
  //�O����ĂԂ�������͎g��Ȃ����ƁI
  GFL_UNUSED(item);
  GFL_UNUSED(userWork);

  DebugWinWork *work = GFL_SINGLETON_INSTANCE(DebugWinWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr , gfl2::debug::DEBUGWIN_NAME_LEN
            ,L"Mode %2dFrame ���̌��Ƀo�[�������\��",(work->IsMode60()?60:30));

  //�o�[�̕`��
  static const s32 BarL = 64;
  static const s32 BarR = 288;
  static const s32 BarW = BarR-BarL;
  static const s32 BarT = *gfl2::debug::DebugWin_GetLineHeight();
  static const s32 BarB = *gfl2::debug::DebugWin_GetLineHeight()*2;
  static const s32 BarH = BarB-BarT;

  /*
  gfl2::grp::util::DrawUtil::MaterialInfo info;
  gfl2::grp::util::DrawUtil::SetMaterial( info );

  //�F�o�[
  if( work->IsMode60() )
  {
    static const int maxSec = 16666;

    gfl2::math::VEC4 UpdateCol(1.0f,0.0f,0.0f,1.0f);
    gfl2::math::VEC4 DrawCol(0.0f,1.0f,0.0f,1.0f);
    gfl2::math::VEC4 GpuCol(1.0f,1.0f,0.0f,1.0f);

    s32 UpdateLen = work->GetTickNow( DEBUGWIN_TICK_UPDATE ) * BarW / maxSec;
    s32 DrawLen   = work->GetTickNow( DEBUGWIN_TICK_DRAW   ) * BarW / maxSec;
    s32 GpuLen    = work->GetTickNow( DEBUGWIN_TICK_GPU    ) * BarW / maxSec;

    gfl2::grp::util::DrawUtil::DrawRect( BarL , BarT , UpdateLen , BarH/2 , UpdateCol );
    gfl2::grp::util::DrawUtil::DrawRect( BarL + UpdateLen, BarT , DrawLen , BarH/2 , DrawCol );
    gfl2::grp::util::DrawUtil::DrawRect( BarL , BarT+BarH/2 , GpuLen , BarH/2 , GpuCol );

  }
  else
  {
    static const int maxSec = 16666;

    gfl2::math::VEC4 UpdateCol(1.0f,0.0f,0.0f,1.0f);
    gfl2::math::VEC4 UpNextCol(0.0f,0.0f,1.0f,1.0f);
    gfl2::math::VEC4 DrawCol(0.0f,1.0f,0.0f,1.0f);
    gfl2::math::VEC4 GpuCol(1.0f,1.0f,0.0f,1.0f);
    s32 UpdateLen = work->GetTickNow( DEBUGWIN_TICK_UPDATE ) * BarW / maxSec;
    s32 UpNextLen = work->GetTickNow( DEBUGWIN_TICK_UPNEXT ) * BarW / maxSec;
    s32 DrawLen   = work->GetTickNow( DEBUGWIN_TICK_DRAW   ) * BarW / maxSec;
    s32 GpuLen    = work->GetTickNow( DEBUGWIN_TICK_GPU    ) * BarW / 33333;

    gfl2::grp::util::DrawUtil::DrawRect( BarL , BarT , UpdateLen , BarH/3 , UpdateCol );
    gfl2::grp::util::DrawUtil::DrawRect( BarL , BarT+BarH/3 , UpNextLen , BarH/3 , UpNextCol );

    gfl2::grp::util::DrawUtil::DrawRect( BarL + UpNextLen, BarT+BarH/3 , DrawLen , BarH/3 , DrawCol );
    gfl2::grp::util::DrawUtil::DrawRect( BarL , BarT+(BarH/3*2) , GpuLen , BarH/3 , GpuCol );
  }
  //���胉�C��
  gfl2::grp::util::DrawUtil::DrawRect( BarL,BarT,2,BarH );
  gfl2::grp::util::DrawUtil::DrawRect( BarL+BarW/2,BarT,2,BarH );
  gfl2::grp::util::DrawUtil::DrawRect( BarR,BarT,2,BarH );
  */

  return workStr;
#else
  return NULL;
#endif
}


void DebugWin_UpdateTick( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  GFL_UNUSED(item);
  GFL_UNUSED(userWork);
  //int type = reinterpret_cast<int>(userWork);
  //DebugWinWork *work = GFL_SINGLETON_INSTANCE(DebugWinWork);

}

wchar_t* DebugWin_DrawTick( void* userWork  , gfl2::debug::DebugWinItem *item )
{
#if GFL_DEBUGWIN_USE
  int type = reinterpret_cast<int>(userWork);
  DebugWinWork *work = GFL_SINGLETON_INSTANCE(DebugWinWork);

  s64 ave = work->GetTickCalcAve(type);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr , gfl2::debug::DEBUGWIN_NAME_LEN
            ,L"%ls Now[%6d]Ave[%6d]Max[%6d]" ,item->GetName()
            ,(int)work->GetTickNow(type)
            ,(int)ave
            ,(int)work->GetTickMax(type) );
  return workStr;
#else
  return NULL;
#endif
}

void DebugWin_ResetTick( void* userWork  , gfl2::debug::DebugWinItem *item )
{
#if GFL_DEBUGWIN_USE
  GFL_UNUSED(item);
  GFL_UNUSED(userWork);

  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();
  DebugWinWork *work = GFL_SINGLETON_INSTANCE(DebugWinWork);

  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    item->PlaySystemSound(gfl2::debug::SE_ACCEPT);
    work->TickReset();
  }
#endif
}
/**
void DebugWin_ScriptTest( void* userWork  , gfl2::debug::DebugWinItem *item )
{
#if GFL_DEBUGWIN_USE
  GFL_UNUSED(item);

  DebugWinWork *work = GFL_SINGLETON_INSTANCE(DebugWinWork);
  GameSys::GameManager *p_gman = static_cast<GameSys::GameManager*>(userWork);
  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();

  if( uiBtn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    item->PlaySystemSound(gfl2::debug::SE_CHANGE_VALUE);
    Field::EventScriptCall::CallScript( p_gman, 0 );
  }
#endif
}
*///@saito
void DebugWin_UpdateFs( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  GFL_UNUSED(item);

#if GFL_DEBUGWIN_USE
  gfl2::ui::Button *uiBtn = gfl2::debug::DebugWin_GetUiButton();

  enum
  {
    ITEM_INDEX_HIO_USE_FLAG        = 0,
    ITEM_INDEX_ARCSRC_CREATE_FLAG  = 1,
    ITEM_INDEX_ARCSRC_USE_FLAG     = 2,
  };
  int itemIndex = reinterpret_cast<int>(userWork);

  GameSystem::GameManager *gMan = GFL_SINGLETON_INSTANCE(GameSystem::GameManager);
  gfl2::fs::AsyncFileManager* romAsyncFileManager = gMan->GetAsyncFileManager();

  switch(itemIndex)
  {
  case ITEM_INDEX_HIO_USE_FLAG:
    {
#if defined(GF_PLATFORM_CTR)
      bool useFlag = romAsyncFileManager->GetHioUseFlag();
      if( uiBtn->IsTrigger( gfl2::ui::BUTTON_LEFT ) || uiBtn->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
      {
        useFlag = !useFlag;
      }
      romAsyncFileManager->SetHioUseFlag(useFlag);
#endif
    }
    break;
  case ITEM_INDEX_ARCSRC_CREATE_FLAG:
    {
      bool createFlag = romAsyncFileManager->GetArcSrcCreateFlag();
      if( uiBtn->IsTrigger( gfl2::ui::BUTTON_LEFT ) || uiBtn->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
      {
        createFlag = !createFlag;
      }
      romAsyncFileManager->SetArcSrcCreateFlag(createFlag);
    }
    break;
  case ITEM_INDEX_ARCSRC_USE_FLAG:
    {
      bool useFlag = romAsyncFileManager->GetArcSrcUseFlag();
      if( uiBtn->IsTrigger( gfl2::ui::BUTTON_LEFT ) || uiBtn->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
      {
        useFlag = !useFlag;
      }
      romAsyncFileManager->SetArcSrcUseFlag(useFlag);
    }
    break;
  }
#endif
}

wchar_t* DebugWin_DrawFs( void* userWork  , gfl2::debug::DebugWinItem *item )
{
#if GFL_DEBUGWIN_USE
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"" );

  enum
  {
    ITEM_INDEX_HIO_USE_FLAG        = 0,
    ITEM_INDEX_ARCSRC_CREATE_FLAG  = 1,
    ITEM_INDEX_ARCSRC_USE_FLAG     = 2,
  };
  int itemIndex = reinterpret_cast<int>(userWork);

  GameSystem::GameManager *gMan = GFL_SINGLETON_INSTANCE(GameSystem::GameManager);
  gfl2::fs::AsyncFileManager* romAsyncFileManager = gMan->GetAsyncFileManager();

  switch(itemIndex)
  {
  case ITEM_INDEX_HIO_USE_FLAG:
    {
      bool useFlag = false;
#if defined(GF_PLATFORM_CTR)
      useFlag = romAsyncFileManager->GetHioUseFlag();
#elif defined(GF_PLATFORM_WIN32)
      useFlag = true;  // �uPC�t�@�C����ǂށv�Ƃ������ڂȂ̂ŁAWin32�ł͏��true�ƂȂ�B
#endif
      swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN,
        L"%ls [%d]",
        item->GetName(), useFlag?(1):(0) );
    }
    break;
  case ITEM_INDEX_ARCSRC_CREATE_FLAG:
    {
      bool createFlag = romAsyncFileManager->GetArcSrcCreateFlag();
      swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN,
        L"%ls [%d]",
        item->GetName(), createFlag?(1):(0) );
    }
    break;
  case ITEM_INDEX_ARCSRC_USE_FLAG:
    {
      bool useFlag = romAsyncFileManager->GetArcSrcUseFlag();
      swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN,
        L"%ls [%d]",
        item->GetName(), useFlag?(1):(0) );
    }
    break;
  }

  return workStr;
#else
  return NULL;
#endif
}

void DebugWin_UpdateGpuProfile( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  GFL_UNUSED(item);
  GFL_UNUSED(userWork);
  //int type = reinterpret_cast<int>(userWork);
  //DebugWinWork *work = GFL_SINGLETON_INSTANCE(DebugWinWork);
}

wchar_t* DebugWin_DrawGpuProfile( void* userWork  , gfl2::debug::DebugWinItem *item )
{
#if GFL_DEBUGWIN_USE

#if defined(GFX_CTR_PROFILING_ENABLED)
  gfl2::gfx::ctr::CTRGPUProfiler::ProfilingResult result = gfl2::gfx::ctr::CTRGL::GetGPUProfilingResult();
  int index = reinterpret_cast<int>(userWork);
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr , gfl2::debug::DEBUGWIN_NAME_LEN,
    L"%ls [%d]" ,item->GetName(), result.values[index] );

  return workStr;
#else
  return NULL;
#endif // GFX_CTR_PROFILING_ENABLED

#else // GFL_DEBUGWIN_USE

  return NULL;

#endif // GFL_DEBUGWIN_USE
}

void DebugWin_AddGpuProfileWin( gfl2::heap::HeapBase* heap )
{
#if defined(GFX_CTR_PROFILING_ENABLED)

  gfl2::debug::DebugWinGroup *gpuProfileGroup                     = gfl2::debug::DebugWin_AddGroup( heap , L"GPUProfile" ); // GPUProfile���[�g�O���[�v
  gfl2::debug::DebugWinGroup *gpuProfileVertexShader01Group       = gfl2::debug::DebugWin_AddGroup( heap , L"VertexShader0-1"       , gpuProfileGroup );
  gfl2::debug::DebugWinGroup *gpuProfileVertexShader23Group       = gfl2::debug::DebugWin_AddGroup( heap , L"VertexShader2-3"       , gpuProfileGroup );
  gfl2::debug::DebugWinGroup *gpuProfileBusyPolygonFragmentGroup  = gfl2::debug::DebugWin_AddGroup( heap , L"Busy Polygon Fragment" , gpuProfileGroup );
  gfl2::debug::DebugWinGroup *gpuProfileMemoryAccessGroup         = gfl2::debug::DebugWin_AddGroup( heap , L"MemoryAccess"          , gpuProfileGroup );

  // VertexShader0-1�O���[�v
  {
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader01Group , heap , L"0[��۸��Ѷ����̑J�ڐ�]"          , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX0_PROGCOUNTER)      , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader01Group , heap , L"0[���߂̈ˑ��֌W �İ� �ۯ���]"   , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX0_CMD_STOLE)        , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader01Group , heap , L"0[���ڽڼ޽��̍X�V �İ� �ۯ���]" , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX0_MOVA_CMD_STOLE)   , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader01Group , heap , L"0[�ð��ڼ޽��̍X�V �İ� �ۯ���]" , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX0_CMP_CMD_STOLE)    , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader01Group , heap , L"0[���̪����н �İ� �ۯ���]"      , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX0_FETCH_MISS_STOLE) , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );

    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader01Group , heap , L"1[��۸��Ѷ����̑J�ڐ�]"          , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX1_PROGCOUNTER)      , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader01Group , heap , L"1[���߂̈ˑ��֌W �İ� �ۯ���]"   , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX1_CMD_STOLE)        , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader01Group , heap , L"1[���ڽڼ޽��̍X�V �İ� �ۯ���]" , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX1_MOVA_CMD_STOLE)   , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader01Group , heap , L"1[�ð��ڼ޽��̍X�V �İ� �ۯ���]" , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX1_CMP_CMD_STOLE)    , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader01Group , heap , L"1[���̪����н �İ� �ۯ���]"      , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX1_FETCH_MISS_STOLE) , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
  }

  // VertexShader2-3�O���[�v
  {
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader23Group , heap , L"2[��۸��Ѷ����̑J�ڐ�]"          , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX2_PROGCOUNTER)      , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader23Group , heap , L"2[���߂̈ˑ��֌W �İ� �ۯ���]"   , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX2_CMD_STOLE)        , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader23Group , heap , L"2[���ڽڼ޽��̍X�V �İ� �ۯ���]" , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX2_MOVA_CMD_STOLE)   , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader23Group , heap , L"2[�ð��ڼ޽��̍X�V �İ� �ۯ���]" , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX2_CMP_CMD_STOLE)    , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader23Group , heap , L"2[���̪����н �İ� �ۯ���]"      , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX2_FETCH_MISS_STOLE) , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );

    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader23Group , heap , L"3[��۸��Ѷ����̑J�ڐ�]"          , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX3_PROGCOUNTER)      , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader23Group , heap , L"3[���߂̈ˑ��֌W �İ� �ۯ���]"   , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX3_CMD_STOLE)        , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader23Group , heap , L"3[���ڽڼ޽��̍X�V �İ� �ۯ���]" , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX3_MOVA_CMD_STOLE)   , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader23Group , heap , L"3[�ð��ڼ޽��̍X�V �İ� �ۯ���]" , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX3_CMP_CMD_STOLE)    , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileVertexShader23Group , heap , L"3[���̪����н �İ� �ۯ���]"      , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::VERTEX3_FETCH_MISS_STOLE) , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
  }

  // BusyPolygonFragment�O���[�v
  {
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileBusyPolygonFragmentGroup , heap , L"BusyUs[����ް��۾��0]"      , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::BUSY_SHADER0)             , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileBusyPolygonFragmentGroup , heap , L"BusyUs[cmd vertex ذ��]"    , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::BUSY_COMMAND_VERTEX_LOAD) , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileBusyPolygonFragmentGroup , heap , L"BusyUs[׽�ײ��]"            , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::BUSY_RASTERIZE)           , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileBusyPolygonFragmentGroup , heap , L"BusyUs[�ײ�ݸ�پ�ı���]"    , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::BUSY_TRIANGLE_SETUP)      , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileBusyPolygonFragmentGroup , heap , L"BusyUs[�׸����ײèݸ�]"     , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::BUSY_FRAGMENT)            , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileBusyPolygonFragmentGroup , heap , L"BusyUs[ø����Ư�]"          , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::BUSY_TEXTURE_UNIT)        , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileBusyPolygonFragmentGroup , heap , L"BusyUs[�߰�׸���ĵ��ڰ���]" , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::BUSY_PERFRAGMENT)         , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileBusyPolygonFragmentGroup , heap , L"BusyUs[ø������޲�]"        , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::BUSY_TEXTURE_COMB)        , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );

    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileBusyPolygonFragmentGroup , heap , L"Pol[�ײ�ݸ�پ�ı��� ���͒��_��]"     , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::POLYGON_INPUT_VERTEX)   , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileBusyPolygonFragmentGroup , heap , L"Pol[�ײ�ݸ�پ�ı��� ���̓|���S����]" , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::POLYGON_INPUT_POLYGON)  , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileBusyPolygonFragmentGroup , heap , L"Pol[�ײ�ݸ�پ�ı��� �o�̓|���S����]" , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::POLYGON_OUTPUT_POLYGON) , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );

    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileBusyPolygonFragmentGroup , heap , L"Fragment[�����׸���Đ�����]" , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::FRAGMENT_INPUT_FRAGMENT) , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
  }

  // MemoryAccess�O���[�v
  {
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileMemoryAccessGroup , heap , L"[VRAMA ذ�� Byte]"                   , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::ACCESS_VRAMA_READ)         , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileMemoryAccessGroup , heap , L"[VRAMA ײ� Byte]"                    , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::ACCESS_VRAMA_WRITE)        , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileMemoryAccessGroup , heap , L"[VRAMB ذ�� Byte]"                   , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::ACCESS_VRAMB_READ)         , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileMemoryAccessGroup , heap , L"[VRAMB ײ� Byte]"                    , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::ACCESS_VRAMB_WRITE)        , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileMemoryAccessGroup , heap , L"[CMD Ver Index ذ�� Byte]"           , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::ACCESS_CMD_VERTEX)         , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileMemoryAccessGroup , heap , L"[ø����Ư� ø������ ذ�� Byte]"      , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::ACCESS_TEX_UNIT)           , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileMemoryAccessGroup , heap , L"[FragmentOpe Depth ذ�� Byte]"       , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::ACCESS_DEPTH_READ)         , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileMemoryAccessGroup , heap , L"[FragmentOpe Depth ײ� Byte]"        , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::ACCESS_DEPTH_WRITE)        , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileMemoryAccessGroup , heap , L"[FragmentOpe Color ذ�� Byte]"       , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::ACCESS_COLOR_READ)         , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileMemoryAccessGroup , heap , L"[FragmentOpe COlor ײ� Byte]"        , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::ACCESS_COLOR_WRITE)        , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileMemoryAccessGroup , heap , L"[�ި���ڲ�ޯ̧ up   ذ�� Byte]"      , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::ACCESS_LCD_UP_DISP_READ)   , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileMemoryAccessGroup , heap , L"[�ި���ڲ�ޯ̧ down ذ�� Byte]"      , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::ACCESS_LCD_DOWN_DISP_READ) , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileMemoryAccessGroup , heap , L"[�߽ē]��Ӽޭ�� ذ�� Byte]"          , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::ACCESS_POST_READ)          , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileMemoryAccessGroup , heap , L"[�߽ē]��Ӽޭ�� ײ� Byte]"           , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::ACCESS_POST_WRITE)         , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileMemoryAccessGroup , heap , L"[���̨�Ӽޭ�� ����0 �ޯ̧ ײ� Byte]" , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::ACCESS_MEMFILL0_WRITE)     , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileMemoryAccessGroup , heap , L"[���̨�Ӽޭ�� ����1 �ޯ̧ ײ� Byte]" , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::ACCESS_MEMFILL1_WRITE)     , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileMemoryAccessGroup , heap , L"[glReadPixels VRAM ذ�� Byte]"       , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::ACCESS_READPIXCEL)         , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
    gfl2::debug::DebugWin_AddItemUpdateFunc( gpuProfileMemoryAccessGroup , heap , L"[DMA�]�� VRAM ײ� Byte]"             , reinterpret_cast<void*>(gfl2::gfx::ctr::CTRGPUProfiler::TYPE::ACCESS_DMA_VRAM_WRITE)     , DebugWin_UpdateGpuProfile , DebugWin_DrawGpuProfile );
  }

#endif
}

//---------------------------------------------------------------------------
//�f�o�b�O�E�B���h�E���[�N
DebugWinWork::DebugWinWork()
  : mResetRequest(DEBUGWIN_TICK_MAX)
{
  mIs60Mode = false;
  TickReset();
}

void DebugWinWork::TickReset(void)
{
  for( int i=0;i<DEBUGWIN_TICK_MAX;i++ )
  {
    this->TickReset(i);
  }
}
void DebugWinWork::TickReset( int tickType )
{
  mTickStart[tickType] = 0;
  mTickNow[tickType] = 0;
  mTickMax[tickType] = 0;
  for( int j=0;j<TICK_AVE_NUM;j++ )
  {
    mTickAve[tickType][j] = 0;
  }
  mTickAveCnt[tickType] = 0;
}

void DebugWinWork::TickResetRequest( int tickType )
{
  mResetRequest = tickType;
}

void DebugWinWork::TickStart( int tickType )
{
  if( mResetRequest == tickType )
  {
    this->TickReset( tickType );
    mResetRequest = DEBUGWIN_TICK_MAX;
  }

  mTickStart[tickType] = gfl2::system::Timer::PerformanceCounter::GetMicroSecond();
}

void DebugWinWork::TickEnd( int tickType )
{
  s64 end = gfl2::system::Timer::PerformanceCounter::GetMicroSecond();
  TickCalc(tickType,end - mTickStart[tickType]);
}
void DebugWinWork::TickCalc( int tickType , s64 microSec )
{
  mTickNow[tickType] = microSec;

  if( mTickNow[tickType] > mTickMax[tickType] )
  {
    mTickMax[tickType] = mTickNow[tickType];
  }

  mTickAve[tickType][mTickAveCnt[tickType]] = mTickNow[tickType];
  mTickAveCnt[tickType]++;

  if( mTickAveCnt[tickType] >= TICK_AVE_NUM )
  {
    mTickAveCnt[tickType] = 0;
  }
}

s64 DebugWinWork::GetTickCalcAve( int tickType )
{
  s64 ave = 0;
  for( int i=0;i<DebugWinWork::TICK_AVE_NUM;i++ )
  {
    ave += GetTickAve(tickType,i);
  }
  ave /= DebugWinWork::TICK_AVE_NUM;
  return ave;
}

//�f�o�b�O�E�B���R�[���o�b�N
//----------------------------------------------------------------------------
/**
 *  @brief  �f�o�b�O�E�B���h�E�p�T�E���h�Đ�
 *          gflib�ɃT�E���h����������킯�ɂ������Ȃ��̂ŁA�A�v���ɂ��C���B
 *
 *  @param  const DEBUGWIN_SOUND_TYPE �T�E���h���
 */
//-----------------------------------------------------------------------------
void DebugWinWork::SoundCallBack( const gfl2::debug::DEBUGWIN_SOUND_TYPE type )
{
  /*
  switch(type)
  {
  case gfl2::debug::SE_CURSOR_MOVE:
    xy_snd::PlaySE( SEQ_SE_SELECT1 );
    break;
  case gfl2::debug::SE_CHANGE_VALUE:
    xy_snd::PlaySE( SEQ_SE_SLIDE );
    break;
  case gfl2::debug::SE_GROUP_IN:
    xy_snd::PlaySE( SEQ_SE_DECIDE1 );
    break;
  case gfl2::debug::SE_GROUP_OUT:
    xy_snd::PlaySE( SEQ_SE_CANCEL1 );
    break;
  case gfl2::debug::SE_ACCEPT:
    xy_snd::PlaySE( SEQ_SE_DECIDE1 );
    break;
  case gfl2::debug::SE_CANCEL:
    xy_snd::PlaySE( SEQ_SE_CANCEL1 );
    break;
  case gfl2::debug::SE_ERROR:
    xy_snd::PlaySE( SEQ_SE_BEEP );
    break;
  case gfl2::debug::SE_WINDOW_OPEN:
    xy_snd::PlaySE( SEQ_SE_OPEN1 );
    break;
  case gfl2::debug::SE_WINDOW_CLOSE:
    xy_snd::PlaySE( SEQ_SE_CLOSE1 );
    break;
  }
  */
}

#endif //USE_DEBUG_WIN

GFL_NAMESPACE_END(DebugWin)
GFL_NAMESPACE_END(Debug)