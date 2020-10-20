//======================================================================
/**
 * @file	gfl_DebugwinItemTypes.cpp
 * @brief デバッグウィンドウ用アイテム(項目)
 * @author	ariizumi
 * @data	11/03/29
 */
//======================================================================
#include <gflib.h>
#include <debug/gfl_DebugWinItemTypes.h>


namespace gfl
{
namespace debug
{
#if GFL_DEBUGWIN_USE

//---------------------------------------------------------
//数値操作用クラスVec3型(独自処理追加)
const wchar_t* DebugWinItemValueVec3::GetDispName(DebugWin *debSys)
{
  wchar_t* workStr = this->GetSysWorkStr();
  wchar_t mark1 = (&mVecTarget->x == DebugWinItemValue<f32>::mTarget ? L'>' : L' ' );
  wchar_t mark2 = (&mVecTarget->y == DebugWinItemValue<f32>::mTarget ? L'>' : L' ' );
  wchar_t mark3 = (&mVecTarget->z == DebugWinItemValue<f32>::mTarget ? L'>' : L' ' );
  
  
  nw::ut::snwprintf( workStr ,DEBUGWIN_NAME_LEN,DEBUGWIN_NAME_LEN-1, L"%ls%c[%.1f]%c[%.1f]%c[%.1f]",DebugWinItemValue<f32>::GetName() 
                                                                      ,mark1, mVecTarget->x
                                                                      ,mark2, mVecTarget->y
                                                                      ,mark3, mVecTarget->z );
  return workStr;
}

void DebugWinItemValueVec3::UpdateFunc(DebugWin *debSys)
{
  gfl::ui::Button *uiButton = this->GetSysUiButton();
  
  //操作対象の変更
  if( uiButton->IsTrigger( gfl::ui::BUTTON_Y ) )
  {
    if( DebugWinItemValue<f32>::mTarget == &mVecTarget->x )
    {
      DebugWinItemValue<f32>::mTarget = &mVecTarget->y;
    }
    else
    if( DebugWinItemValue<f32>::mTarget == &mVecTarget->y )
    {
      DebugWinItemValue<f32>::mTarget = &mVecTarget->z;
    }
    else
    if( DebugWinItemValue<f32>::mTarget == &mVecTarget->z )
    {
      DebugWinItemValue<f32>::mTarget = &mVecTarget->x;
    }
    debSys->SoundCallBack(SE_CURSOR_MOVE);
  }
  
  UpdateValueFunc<f32>( debSys,mTarget , mMin , mMax , mStep);
}

//---------------------------------------------------------
//数値操作用クラスVec4型(独自処理追加)
const wchar_t* DebugWinItemValueVec4::GetDispName(DebugWin *debSys)
{
  wchar_t* workStr = this->GetSysWorkStr();
  wchar_t mark1 = (&mVecTarget->x == DebugWinItemValue<f32>::mTarget ? L'>' : L' ' );
  wchar_t mark2 = (&mVecTarget->y == DebugWinItemValue<f32>::mTarget ? L'>' : L' ' );
  wchar_t mark3 = (&mVecTarget->z == DebugWinItemValue<f32>::mTarget ? L'>' : L' ' );
  wchar_t mark4 = (&mVecTarget->w == DebugWinItemValue<f32>::mTarget ? L'>' : L' ' );
  
  
  nw::ut::snwprintf( workStr ,DEBUGWIN_NAME_LEN,DEBUGWIN_NAME_LEN-1, L"%ls%c[%.1f]%c[%.1f]%c[%.1f]%c[%.1f]",DebugWinItemValue<f32>::GetName() 
                                                                      ,mark1, mVecTarget->x
                                                                      ,mark2, mVecTarget->y
                                                                      ,mark3, mVecTarget->z
                                                                      ,mark4, mVecTarget->w );
  return workStr;
}

void DebugWinItemValueVec4::UpdateFunc(DebugWin *debSys)
{
  gfl::ui::Button *uiButton = this->GetSysUiButton();
  
  //操作対象の変更
  if( uiButton->IsTrigger( gfl::ui::BUTTON_Y ) )
  {
    if( DebugWinItemValue<f32>::mTarget == &mVecTarget->x )
    {
      DebugWinItemValue<f32>::mTarget = &mVecTarget->y;
    }
    else
    if( DebugWinItemValue<f32>::mTarget == &mVecTarget->y )
    {
      DebugWinItemValue<f32>::mTarget = &mVecTarget->z;
    }
    else
    if( DebugWinItemValue<f32>::mTarget == &mVecTarget->z )
    {
      DebugWinItemValue<f32>::mTarget = &mVecTarget->w;
    }
    else
    if( DebugWinItemValue<f32>::mTarget == &mVecTarget->w )
    {
      DebugWinItemValue<f32>::mTarget = &mVecTarget->x;
    }
    debSys->SoundCallBack(SE_CURSOR_MOVE);
  }
  
  UpdateValueFunc<f32>( debSys,mTarget , mMin , mMax , mStep);
}
//---------------------------------------------------------
//数値操作用クラスColorF32型(独自処理追加)
const wchar_t* DebugWinItemValueColorF32::GetDispName(DebugWin *debSys)
{
  wchar_t* workStr = this->GetSysWorkStr();
  wchar_t mark1 = (&mVecTarget->r == DebugWinItemValue<f32>::mTarget ? L'>' : L' ' );
  wchar_t mark2 = (&mVecTarget->g == DebugWinItemValue<f32>::mTarget ? L'>' : L' ' );
  wchar_t mark3 = (&mVecTarget->b == DebugWinItemValue<f32>::mTarget ? L'>' : L' ' );
  wchar_t mark4 = (&mVecTarget->a == DebugWinItemValue<f32>::mTarget ? L'>' : L' ' );
  
  nw::ut::snwprintf( workStr ,DEBUGWIN_NAME_LEN,DEBUGWIN_NAME_LEN-1, L"%ls%c[%.1f]%c[%.1f]%c[%.1f]%c[%.1f]",DebugWinItemValue<f32>::GetName() 
                                                                      ,mark1, mVecTarget->r
                                                                      ,mark2, mVecTarget->g
                                                                      ,mark3, mVecTarget->b
                                                                      ,mark4, mVecTarget->a );
  return workStr;
}

void DebugWinItemValueColorF32::UpdateFunc(DebugWin *debSys)
{
  gfl::ui::Button *uiButton = this->GetSysUiButton();
  
  //操作対象の変更
  if( uiButton->IsTrigger( gfl::ui::BUTTON_Y ) )
  {
    if( DebugWinItemValue<f32>::mTarget == &mVecTarget->r )
    {
      DebugWinItemValue<f32>::mTarget = &mVecTarget->g;
    }
    else
    if( DebugWinItemValue<f32>::mTarget == &mVecTarget->g )
    {
      DebugWinItemValue<f32>::mTarget = &mVecTarget->b;
    }
    else
    if( DebugWinItemValue<f32>::mTarget == &mVecTarget->b )
    {
      DebugWinItemValue<f32>::mTarget = &mVecTarget->a;
    }
    else
    if( DebugWinItemValue<f32>::mTarget == &mVecTarget->a )
    {
      DebugWinItemValue<f32>::mTarget = &mVecTarget->r;
    }
    debSys->SoundCallBack(SE_CURSOR_MOVE);
  }
  
  UpdateValueFunc<f32>( debSys,mTarget , mMin , mMax , mStep);
}

//---------------------------------------------------------
//数値操作用クラス整数型MsgData版(独自処理追加)
DebugWinItemValueIntMsg::DebugWinItemValueIntMsg( DebugWinGroup *parent , const wchar_t *name , int *target , fs::ArcFile::ARCID arcID, u32 datID , int min, int max , int msgOfs , heap::HeapBase *heap )
  :DebugWinItemValueInt<int>( parent , name , target , min , max , 1 )
  ,mMsgOffset(msgOfs)
  ,mDispStrNo(-1)
{
  mMsgData = GFL_NEW( heap ) gfl::str::MsgData( arcID, datID, heap );
  mWorkStr = GFL_NEW( heap ) gfl::str::StrBuf(DEBUGWIN_NAME_LEN,heap);
  
  if( max == -1 )
  {
    mMax = mMsgData->GetStrCount()-1;
  }
  
}

DebugWinItemValueIntMsg::~DebugWinItemValueIntMsg()
{
  GFL_DELETE mMsgData;
  GFL_DELETE mWorkStr;
}

const wchar_t* DebugWinItemValueIntMsg::GetDispName(DebugWin *debSys)
{
  wchar_t* workStr = this->GetSysWorkStr();
  
  if( mDispStrNo != *DebugWinItemValue<int>::mTarget )
  {
    mDispStrNo = *DebugWinItemValue<int>::mTarget;
    mMsgData->GetString( mDispStrNo + mMsgOffset,*mWorkStr );
  }

  swprintf( workStr ,DEBUGWIN_NAME_LEN, L"%ls[%d:%ls]",DebugWinItemValue<int>::GetName() , *DebugWinItemValue<int>::mTarget , mWorkStr->GetPtr() );

  return workStr;
}

//---------------------------------------------------------
//Util
const wchar_t* DebugWinUtil_GetMsgString( str::MsgData *msgData , u32 strId )
{
  DebugWin *debWin = GFL_SINGLETON_INSTANCE(gfl::debug::DebugWin);
  str::StrBuf *workStr = debWin->GetWorkStrBuf();
  msgData->GetString( strId,*workStr );
  return workStr->GetPtr();
}

//---------------------------------------------------------
//数値系
DebugWinItemValueU64<u64>* DebugWin_AddItemValueU64( DebugWinGroup *parent , const wchar_t *name , u64 *target , u64 min , u64 max , u64 step , heap::HeapBase *heap )
{
  DebugWin *debWin = GFL_SINGLETON_INSTANCE(gfl::debug::DebugWin);
  heap::HeapBase *useHeap = (heap?heap:debWin->GetHeap());

  DebugWinItemValueU64<u64> *item = GFL_NEW(useHeap)DebugWinItemValueU64<u64>( parent , name , target , min , max , step );
  
  debWin->AddItem( item , parent );
  return item;
}
DebugWinItemValueU64<u64>* DebugWin_AddItemValueU64( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , u64 *target , u64 min , u64 max , u64 step , heap::HeapBase *heap )
{
  return DebugWin_AddItemValueU64( parent , DebugWinUtil_GetMsgString(msgData,strId) , target , min , max , step , heap );
}


DebugWinItemValueU8<u8>* DebugWin_AddItemValueU8( DebugWinGroup *parent , const wchar_t *name , u8 *target , u8 min , u8 max , u8 step , heap::HeapBase *heap )
{
  DebugWin *debWin = GFL_SINGLETON_INSTANCE(gfl::debug::DebugWin);
  heap::HeapBase *useHeap = (heap?heap:debWin->GetHeap());

  DebugWinItemValueU8<u8> *item = GFL_NEW(useHeap)DebugWinItemValueU8<u8>( parent , name , target , min , max , step );

  debWin->AddItem( item , parent );
  return item;
}
DebugWinItemValueU8<u8>* DebugWin_AddItemValueU8( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , u8 *target , u8 min , u8 max , u8 step , heap::HeapBase *heap )
{
  return DebugWin_AddItemValueU8( parent , DebugWinUtil_GetMsgString(msgData,strId) , target , min , max , step , heap );
}


DebugWinItemValueU32<u32>* DebugWin_AddItemValueU32( DebugWinGroup *parent , const wchar_t *name , u32 *target , u32 min , u32 max , u32 step , heap::HeapBase *heap )
{
  DebugWin *debWin = GFL_SINGLETON_INSTANCE(gfl::debug::DebugWin);
  heap::HeapBase *useHeap = (heap?heap:debWin->GetHeap());

  DebugWinItemValueU32<u32> *item = GFL_NEW(useHeap)DebugWinItemValueU32<u32>( parent , name , target , min , max , step );

  debWin->AddItem( item , parent );
  return item;
}
DebugWinItemValueU32<u32>* DebugWin_AddItemValueU32( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , u32 *target , u32 min , u32 max , u32 step , heap::HeapBase *heap )
{
  return DebugWin_AddItemValueU32( parent , DebugWinUtil_GetMsgString(msgData,strId) , target , min , max , step , heap );
}

DebugWinItemValueInt<int>* DebugWin_AddItemValueInt( DebugWinGroup *parent , const wchar_t *name , int *target , int min , int max , int step , heap::HeapBase *heap )
{
  DebugWin *debWin = GFL_SINGLETON_INSTANCE(gfl::debug::DebugWin);
  heap::HeapBase *useHeap = (heap?heap:debWin->GetHeap());

  DebugWinItemValueInt<int> *item = GFL_NEW(useHeap)DebugWinItemValueInt<int>( parent , name , target , min , max , step );

  debWin->AddItem( item , parent );
  return item;
}
DebugWinItemValueInt<int>* DebugWin_AddItemValueInt( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , int *target , int min , int max , int step , heap::HeapBase *heap )
{
  return DebugWin_AddItemValueInt( parent , DebugWinUtil_GetMsgString(msgData,strId) , target , min , max , step , heap );
}

DebugWinItemValueFloat<f32>* DebugWin_AddItemValueFloat( DebugWinGroup *parent , const wchar_t *name , f32 *target , f32 min , f32 max , f32 step , heap::HeapBase *heap )
{
  DebugWin *debWin = GFL_SINGLETON_INSTANCE(gfl::debug::DebugWin);
  heap::HeapBase *useHeap = (heap?heap:debWin->GetHeap());

  DebugWinItemValueFloat<f32> *item = GFL_NEW(useHeap)DebugWinItemValueFloat<f32>( parent , name , target , min , max , step );
  
  debWin->AddItem( item , parent );
  return item;
}
DebugWinItemValueFloat<f32>* DebugWin_AddItemValueFloat( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , f32 *target , f32 min , f32 max , f32 step , heap::HeapBase *heap )
{
  return DebugWin_AddItemValueFloat( parent , DebugWinUtil_GetMsgString(msgData,strId) , target , min , max , step , heap );
}

DebugWinItemValueInt<bool>* DebugWin_AddItemValueBool( DebugWinGroup *parent , const wchar_t *name , bool *target , heap::HeapBase *heap )
{
  DebugWin *debWin = GFL_SINGLETON_INSTANCE(gfl::debug::DebugWin);
  heap::HeapBase *useHeap = (heap?heap:debWin->GetHeap());

  DebugWinItemValueInt<bool> *item = GFL_NEW(useHeap)DebugWinItemValueInt<bool>( parent , name , target , 0,1,1 );
  
  debWin->AddItem( item , parent );
  return item;
}
DebugWinItemValueInt<bool>* DebugWin_AddItemValueBool( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , bool *target , heap::HeapBase *heap )
{
  return DebugWin_AddItemValueBool( parent , DebugWinUtil_GetMsgString(msgData,strId) , target , heap );
}

DebugWinItemValueVec3* DebugWin_AddItemValueVec3( DebugWinGroup *parent , const wchar_t *name , gfl::math::VEC3 *target , f32 min , f32 max , f32 step , heap::HeapBase *heap )
{
  DebugWin *debWin = GFL_SINGLETON_INSTANCE(DebugWin);
  heap::HeapBase *useHeap = (heap?heap:debWin->GetHeap());

  DebugWinItemValueVec3 *item = GFL_NEW(useHeap)DebugWinItemValueVec3( parent , name , target , min , max , step );
  
  debWin->AddItem( item , parent );
  return item;
}
DebugWinItemValueVec3* DebugWin_AddItemValueVec3( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , gfl::math::VEC3 *target , f32 min , f32 max , f32 step , heap::HeapBase *heap )
{
  return DebugWin_AddItemValueVec3( parent , DebugWinUtil_GetMsgString(msgData,strId) , target , min , max , step , heap );
}

DebugWinItemValueVec4* DebugWin_AddItemValueVec4( DebugWinGroup *parent , const wchar_t *name , gfl::math::VEC4 *target , f32 min , f32 max , f32 step , heap::HeapBase *heap )
{
  DebugWin *debWin = GFL_SINGLETON_INSTANCE(DebugWin);
  heap::HeapBase *useHeap = (heap?heap:debWin->GetHeap());

  DebugWinItemValueVec4 *item = GFL_NEW(useHeap)DebugWinItemValueVec4( parent , name , target , min , max , step );
  
  debWin->AddItem( item , parent );
  return item;
}
DebugWinItemValueVec4* DebugWin_AddItemValueVec4( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , gfl::math::VEC4 *target , f32 min , f32 max , f32 step , heap::HeapBase *heap )
{
  return DebugWin_AddItemValueVec4( parent , DebugWinUtil_GetMsgString(msgData,strId) , target , min , max , step , heap );
}

DebugWinItemValueColorF32* DebugWin_AddItemValueColorF32( DebugWinGroup *parent , const wchar_t *name , gfl::grp::ColorF32 *target , f32 min , f32 max , f32 step , heap::HeapBase *heap )
{
  DebugWin *debWin = GFL_SINGLETON_INSTANCE(DebugWin);
  heap::HeapBase *useHeap = (heap?heap:debWin->GetHeap());

  DebugWinItemValueColorF32 *item = GFL_NEW(useHeap)DebugWinItemValueColorF32( parent , name , target , min , max , step );
  
  debWin->AddItem( item , parent );
  return item;
}
DebugWinItemValueColorF32* DebugWin_AddItemValueColorF32( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , gfl::grp::ColorF32 *target , f32 min , f32 max , f32 step , heap::HeapBase *heap )
{
  return DebugWin_AddItemValueColorF32( parent , DebugWinUtil_GetMsgString(msgData,strId) , target , min , max , step , heap );
}

DebugWinItemValueIntMsg* DebugWin_AddItemValueIntMsg( DebugWinGroup *parent , const wchar_t *name , int *target , fs::ArcFile::ARCID arcID, u32 datID , int min, int max , int msgOfs , heap::HeapBase *heap )
{
  DebugWin *debWin = GFL_SINGLETON_INSTANCE(gfl::debug::DebugWin);
  heap::HeapBase *useHeap = (heap?heap:debWin->GetHeap());

  DebugWinItemValueIntMsg *item = GFL_NEW(useHeap)DebugWinItemValueIntMsg( parent , name , target , arcID , datID , min , max , msgOfs , useHeap );
  
  debWin->AddItem( item , parent );
  return item;
}
DebugWinItemValueIntMsg* DebugWin_AddItemValueIntMsg( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , int *target , fs::ArcFile::ARCID arcID, u32 datID , int min, int max , int msgOfs , heap::HeapBase *heap )
{
  return DebugWin_AddItemValueIntMsg( parent , DebugWinUtil_GetMsgString(msgData,strId) , target , arcID , datID , min , max , msgOfs , heap );
}

//---------------------------------------------------------
//カメラ操作

//管理ワーク
typedef struct
{
  gfl::math::VEC3 pos;
  gfl::math::VEC3 trg;
  f32 near;
  f32 far;
  int fov;
  
  gfl::grp::g3d::Camera *camera;
}DEBWIN_ITEM_CAMERA;

//数値更新時処理
void DebugWin_CameraFuncSet( void *userWork )
{
  DEBWIN_ITEM_CAMERA *work = static_cast<DEBWIN_ITEM_CAMERA*>(userWork);
  
  work->camera->SetCameraPosition( work->pos );
  work->camera->SetTargetPosition( work->trg );
  work->camera->SetNearClip( work->near );
  work->camera->SetFarClip( work->far );
  work->camera->SetFovy( GFL_MATH_DEG_TO_RAD(work->fov) );
}

void DebugWin_CameraRefreshValue( void *userWork )
{
  DEBWIN_ITEM_CAMERA *work = static_cast<DEBWIN_ITEM_CAMERA*>(userWork);

  work->camera->GetCameraAndTargetPosition( work->pos , work->trg );
  work->near = work->camera->GetNearClip();
  work->far = work->camera->GetFarClip();
  work->fov = GFL_MATH_RAD_TO_DEG( work->camera->GetFovy() );
}

//値取得処理
void DebugWin_CameraFuncUpdate( void* userWork  , DebugWinItem *item )
{
  GFL_UNUSED(item);

  DEBWIN_ITEM_CAMERA *work = static_cast<DEBWIN_ITEM_CAMERA*>(userWork);
  DebugWin *debWin = GFL_SINGLETON_INSTANCE(DebugWin);
  gfl::ui::DeviceManager *uiMng = debWin->GetDeviceManager();

  gfl::ui::Button *uiButton = uiMng->GetButton(0);

  if( uiButton->IsHold( gfl::ui::BUTTON_A ) )
  {
    DebugWin_CameraRefreshValue(userWork);
  }
}

//グループ作成処理
void DebugWin_AddItemCamera( DebugWinGroup *parent , const wchar_t *name , gfl::grp::g3d::Camera *target , heap::HeapBase *heap )
{
  DebugWin *debWin = GFL_SINGLETON_INSTANCE(DebugWin);
  gfl::debug::DebugWinGroup *grp = debWin->AddGroup(name , parent);
  
  DEBWIN_ITEM_CAMERA *camWork = static_cast<DEBWIN_ITEM_CAMERA*>(grp->CreateGroupWork(sizeof(DEBWIN_ITEM_CAMERA),heap));
  camWork->camera = target;
  grp->SetOpenGroupFunc( DebugWin_CameraRefreshValue );
  
  DebugWin_AddItemUpdateFunc( grp , L"GetValue" , camWork , DebugWin_CameraFuncUpdate );
  DebugWinItemValueVec3 *itemPos = DebugWin_AddItemValueVec3( grp , L"position" , &camWork->pos , -10000.0f , 10000.0f , 0.1f , heap );
  DebugWinItemValueVec3 *itemTrg = DebugWin_AddItemValueVec3( grp , L"target  " , &camWork->trg , -10000.0f , 10000.0f , 0.1f , heap );
  DebugWinItemValueFloat<f32> *itemNear = DebugWin_AddItemValueFloat( grp , L"near" , &camWork->near , 0 , 10000.0f , 0.1f , heap );
  DebugWinItemValueFloat<f32> *itemFar = DebugWin_AddItemValueFloat( grp , L"far " , &camWork->far  , 0 , 10000.0f , 0.1f , heap );
  DebugWinItemValueInt<int> *itemFov = DebugWin_AddItemValueInt( grp , L"fov " , &camWork->fov  , 0 , 360 , 1 , heap );

  itemPos->SetChangedValueFunc( DebugWin_CameraFuncSet , camWork );
  itemTrg->SetChangedValueFunc( DebugWin_CameraFuncSet , camWork );
  itemNear->SetChangedValueFunc( DebugWin_CameraFuncSet , camWork );
  itemFar->SetChangedValueFunc( DebugWin_CameraFuncSet , camWork );
  itemFov->SetChangedValueFunc( DebugWin_CameraFuncSet , camWork );
}

void DebugWin_AddItemCamera( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , gfl::grp::g3d::Camera *target , heap::HeapBase *heap )
{
  return DebugWin_AddItemCamera( parent , DebugWinUtil_GetMsgString(msgData,strId) , target , heap );
}

//---------------------------------------------------------
//モデル操作

//管理ワーク
typedef struct
{
  gfl::math::VEC3 pos;
  gfl::math::VEC3 rot;
  gfl::math::VEC3 scale;
  
  f32 anmFrame;
  f32 anmSpeed;
  
  gfl::grp::g3d::Model *model;
}DEBWIN_ITEM_MODEL;

//数値更新時処理
void DebugWin_ModelFuncSet( void *userWork )
{
  DEBWIN_ITEM_MODEL *work = static_cast<DEBWIN_ITEM_MODEL*>(userWork);
  
  work->model->SetTranslate( work->pos );
  work->model->SetRotateXYZ( work->rot );
  work->model->SetScale( work->scale );
  work->model->SetAnimationFrame( work->anmFrame );
  work->model->SetAnimationStepFrame( work->anmSpeed );
}

void DebugWin_ModelRefreshValue( void *userWork )
{
  DEBWIN_ITEM_MODEL *work = static_cast<DEBWIN_ITEM_MODEL*>(userWork);
  
  work->pos  = work->model->GetTranslate();
  //work->rot = work->model->GetRotateXYZ();
  work->scale = work->model->GetScale();
  work->anmFrame = work->model->GetAnimationFrame();
  work->anmSpeed = work->model->GetAnimationStepFrame();
}

//値取得処理
void DebugWin_ModelFuncUpdate( void* userWork  , DebugWinItem *item )
{
  GFL_UNUSED(item);

  DEBWIN_ITEM_MODEL *work = static_cast<DEBWIN_ITEM_MODEL*>(userWork);
  DebugWin *debWin = GFL_SINGLETON_INSTANCE(DebugWin);
  gfl::ui::DeviceManager *uiMng = debWin->GetDeviceManager();

  gfl::ui::Button *uiButton = uiMng->GetButton(0);

  if( uiButton->IsHold( gfl::ui::BUTTON_A ) )
  {
    DebugWin_ModelRefreshValue( userWork );
  }
}

//グループ作成処理
void DebugWin_AddItemModel( DebugWinGroup *parent , const wchar_t *name , gfl::grp::g3d::Model *target , heap::HeapBase *heap )
{
  DebugWin *debWin = GFL_SINGLETON_INSTANCE(DebugWin);
  gfl::debug::DebugWinGroup *grp = debWin->AddGroup(name , parent);
  
  DEBWIN_ITEM_MODEL *mdlWork = static_cast<DEBWIN_ITEM_MODEL*>(grp->CreateGroupWork(sizeof(DEBWIN_ITEM_MODEL),heap));
  mdlWork->model = target;
  grp->SetOpenGroupFunc( DebugWin_ModelRefreshValue );
  
  DebugWin_AddItemUpdateFunc( grp , L"GetValue" , mdlWork , DebugWin_ModelFuncUpdate );
  DebugWinItemValueVec3 *itemVec;
  DebugWinItemValueFloat<f32> *itemF32;
  itemVec = DebugWin_AddItemValueVec3( grp , L"position" , &mdlWork->pos , -10000.0f , 10000.0f , 0.1f , heap );
  itemVec->SetChangedValueFunc( DebugWin_ModelFuncSet , mdlWork );
  itemVec = DebugWin_AddItemValueVec3( grp , L"rotation" , &mdlWork->rot , -10000.0f , 10000.0f , 0.1f , heap );
  itemVec->SetChangedValueFunc( DebugWin_ModelFuncSet , mdlWork );
  itemVec = DebugWin_AddItemValueVec3( grp , L"scale   " , &mdlWork->scale , -10000.0f , 10000.0f , 0.1f , heap );
  itemVec->SetChangedValueFunc( DebugWin_ModelFuncSet , mdlWork );
  
  itemF32 = DebugWin_AddItemValueFloat( grp , L"anime frame" , &mdlWork->anmFrame , 0 , 10000.0f , 0.1f , heap );
  itemF32->SetChangedValueFunc( DebugWin_ModelFuncSet , mdlWork );
  itemF32 = DebugWin_AddItemValueFloat( grp , L"anime speed" , &mdlWork->anmSpeed , 0 , 10.0f , 0.1f , heap );
  itemF32->SetChangedValueFunc( DebugWin_ModelFuncSet , mdlWork );
  //DebugWinItemValueFloat<f32> *itemFar = DebugWin_AddItemValueFloat( grp , L"far " , &camWork->far  , 0 , 10000.0f , 0.1f , heap );

  //itemNear->SetChangedValueFunc( DebugWin_ModelFuncSet , mdlWork );
  //itemFar->SetChangedValueFunc( DebugWin_ModelFuncSet , mdlWork );
}

void DebugWin_AddItemModel( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , gfl::grp::g3d::Model *target , heap::HeapBase *heap )
{
  return DebugWin_AddItemModel( parent , DebugWinUtil_GetMsgString(msgData,strId) , target , heap );
}

//---------------------------------------------------------
//ライト操作
//管理ワーク
typedef struct
{
  gfl::math::VEC3 pos;
  gfl::math::VEC3 dir;
  
  gfl::grp::ColorF32 ambient;
  gfl::grp::ColorF32 diffuse;
  gfl::grp::ColorF32 specular0;
  gfl::grp::ColorF32 specular1;
  gfl::grp::ColorF32 sky;
  gfl::grp::ColorF32 ground;
  
  gfl::grp::g3d::Light *light;
}DEBWIN_ITEM_LIGHT;

//数値更新時処理
void DebugWin_LightFuncSet( void *userWork )
{
  DEBWIN_ITEM_LIGHT *work = static_cast<DEBWIN_ITEM_LIGHT*>(userWork);

  const gfl::grp::g3d::Light::Type type = work->light->GetType();

  switch( type )
  {
  case gfl::grp::g3d::Light::TYPE_AMBIENT:
    work->light->SetTranslate( work->pos );
    work->light->SetAmbient( work->ambient );
    break;
  case gfl::grp::g3d::Light::TYPE_FRAGMENT:
    work->light->SetTranslate( work->pos );
    work->light->SetDirection( work->dir );
    work->light->SetAmbient( work->ambient );
    work->light->SetDiffuse( work->diffuse );
    work->light->SetSpecular0( work->specular0 );
    work->light->SetSpecular1( work->specular1 );
    break;
  case gfl::grp::g3d::Light::TYPE_HEMI_SPHERE:
    work->light->SetDirection( work->dir );
    work->light->SetSkyColor( work->sky );
    work->light->SetGroundColor( work->ground );
    break;
  case gfl::grp::g3d::Light::TYPE_VERTEX:
    work->light->SetTranslate( work->pos );
    work->light->SetDirection( work->dir );
    work->light->SetAmbient( work->ambient );
    work->light->SetDiffuse( work->diffuse );
    break;
  }
}

void DebugWin_LightRefreshValue( void *userWork )
{
  DEBWIN_ITEM_LIGHT *work = static_cast<DEBWIN_ITEM_LIGHT*>(userWork);
  
  const gfl::grp::g3d::Light::Type type = work->light->GetType();

  switch( type )
  {
  case gfl::grp::g3d::Light::TYPE_AMBIENT:
    work->light->GetTranslate( &work->pos );
    work->light->GetAmbient( &work->ambient );
    break;
  case gfl::grp::g3d::Light::TYPE_FRAGMENT:
    work->light->GetTranslate( &work->pos );
    work->light->GetDirection( &work->dir );
    work->light->GetAmbient( &work->ambient );
    work->light->GetDiffuse( &work->diffuse );
    work->light->GetSpecular0( &work->specular0 );
    work->light->GetSpecular1( &work->specular1 );
    break;
  case gfl::grp::g3d::Light::TYPE_HEMI_SPHERE:
    work->light->GetDirection( &work->dir );
    work->light->GetSkyColor( &work->sky );
    work->light->GetGroundColor( &work->ground );
    break;
  case gfl::grp::g3d::Light::TYPE_VERTEX:
    work->light->GetTranslate( &work->pos );
    work->light->GetDirection( &work->dir );
    work->light->GetAmbient( &work->ambient );
    work->light->GetDiffuse( &work->diffuse );
    break;
  }
}
//値取得処理
void DebugWin_LightFuncUpdate( void* userWork  , DebugWinItem *item )
{
  GFL_UNUSED(item);
  DebugWin *debWin = GFL_SINGLETON_INSTANCE(DebugWin);
  gfl::ui::DeviceManager *uiMng = debWin->GetDeviceManager();
  gfl::ui::Button *uiButton = uiMng->GetButton(0);

  if( uiButton->IsHold( gfl::ui::BUTTON_A ) )
  {
    DebugWin_LightRefreshValue( userWork );
  }
}

void DebugWin_AddItemLight( DebugWinGroup *parent , const wchar_t *name , gfl::grp::g3d::Light *target , heap::HeapBase *heap )
{
  DebugWin *debWin = GFL_SINGLETON_INSTANCE(DebugWin);
  gfl::debug::DebugWinGroup *grp = debWin->AddGroup(name , parent);
  
  DEBWIN_ITEM_LIGHT *work = static_cast<DEBWIN_ITEM_LIGHT*>(grp->CreateGroupWork(sizeof(DEBWIN_ITEM_LIGHT),heap));
  work->light = target;
  
  const gfl::grp::g3d::Light::Type type = target->GetType();
  DebugWinItemValueVec3 *itemVec;
  DebugWinItemValueColorF32 *itemCol;

  grp->SetOpenGroupFunc( DebugWin_LightRefreshValue );
  
  DebugWin_AddItemUpdateFunc( grp , L"GetValue" , work , DebugWin_LightFuncUpdate );

  switch( type )
  {
  case gfl::grp::g3d::Light::TYPE_AMBIENT:
    itemVec = DebugWin_AddItemValueVec3( grp , L"position" , &work->pos , -10000.0f , 10000.0f , 1.0f , heap );
    itemVec->SetChangedValueFunc( DebugWin_LightFuncSet , work );
    itemCol = DebugWin_AddItemValueColorF32( grp  , L"ambient" , &work->ambient , 0 , 1 , 0.1 , heap );
    itemCol->SetChangedValueFunc( DebugWin_LightFuncSet , work );
    break;
  case gfl::grp::g3d::Light::TYPE_FRAGMENT:
    itemVec = DebugWin_AddItemValueVec3( grp , L"position " , &work->pos , -10000.0f , 10000.0f , 1.0f , heap );
    itemVec->SetChangedValueFunc( DebugWin_LightFuncSet , work );
    itemVec = DebugWin_AddItemValueVec3( grp , L"direction" , &work->dir , -10000.0f , 10000.0f , 1.0f , heap );
    itemVec->SetChangedValueFunc( DebugWin_LightFuncSet , work );
    itemCol = DebugWin_AddItemValueColorF32( grp  , L"ambient" , &work->ambient , 0 , 1 , 0.1 ,heap );
    itemCol->SetChangedValueFunc( DebugWin_LightFuncSet , work );
    itemCol = DebugWin_AddItemValueColorF32( grp  , L"diffuse" , &work->diffuse , 0 , 1 , 0.1 ,heap );
    itemCol->SetChangedValueFunc( DebugWin_LightFuncSet , work );
    itemCol = DebugWin_AddItemValueColorF32( grp  , L"specular0" , &work->specular0 , 0 , 1 , 0.1 ,heap );
    itemCol->SetChangedValueFunc( DebugWin_LightFuncSet , work );
    itemCol = DebugWin_AddItemValueColorF32( grp  , L"specular1" , &work->specular1 , 0 , 1 , 0.1 ,heap );
    itemCol->SetChangedValueFunc( DebugWin_LightFuncSet , work );
    break;
  case gfl::grp::g3d::Light::TYPE_HEMI_SPHERE:
    itemVec = DebugWin_AddItemValueVec3( grp , L"direction" , &work->dir , -10000.0f , 10000.0f , 1.0f , heap );
    itemVec->SetChangedValueFunc( DebugWin_LightFuncSet , work );
    itemCol = DebugWin_AddItemValueColorF32( grp  , L"sky" , &work->sky , 0 , 1 , 0.1 ,heap );
    itemCol->SetChangedValueFunc( DebugWin_LightFuncSet , work );
    itemCol = DebugWin_AddItemValueColorF32( grp  , L"ground" , &work->ground , 0 , 1 , 0.1 ,heap );
    itemCol->SetChangedValueFunc( DebugWin_LightFuncSet , work );
    break;
  case gfl::grp::g3d::Light::TYPE_VERTEX:
    itemVec = DebugWin_AddItemValueVec3( grp , L"position " , &work->pos , -10000.0f , 10000.0f , 1.0f , heap );
    itemVec->SetChangedValueFunc( DebugWin_LightFuncSet , work );
    itemVec = DebugWin_AddItemValueVec3( grp , L"direction" , &work->dir , -10000.0f , 10000.0f , 1.0f , heap );
    itemVec->SetChangedValueFunc( DebugWin_LightFuncSet , work );
    itemCol = DebugWin_AddItemValueColorF32( grp  , L"ambient" , &work->ambient , 0 , 1 , 0.1 ,heap );
    itemCol->SetChangedValueFunc( DebugWin_LightFuncSet , work );
    itemCol = DebugWin_AddItemValueColorF32( grp  , L"diffuse" , &work->diffuse , 0 , 1 , 0.1 ,heap );
    itemCol->SetChangedValueFunc( DebugWin_LightFuncSet , work );
    break;
  }
}

void DebugWin_AddItemLight( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , gfl::grp::g3d::Light *target , heap::HeapBase *heap )
{
  return DebugWin_AddItemLight( parent , DebugWinUtil_GetMsgString(msgData,strId) , target , heap );
}


//---------------------------------------------------------
//Heap操作
//管理ワーク
typedef struct
{
  gfl::heap::HeapBase *heap;
  
  
}DEBWIN_ITEM_HEAP;

wchar_t* DebugWin_HeapDrawTotalSize( void* userWork  , DebugWinItem *item )
{
  DEBWIN_ITEM_HEAP *work = static_cast<DEBWIN_ITEM_HEAP*>(userWork);

  gfl::debug::DebugWin *debWin = GFL_SINGLETON_INSTANCE(gfl::debug::DebugWin);
  wchar_t* workStr = debWin->GetWorkStr();

  swprintf( workStr , gfl::debug::DEBUGWIN_NAME_LEN 
            ,L"TotalSize [0x%08x]" ,work->heap->GetTotalSize() );

  return workStr;
  
}
wchar_t* DebugWin_HeapDrawTotalFree( void* userWork  , DebugWinItem *item )
{
  DEBWIN_ITEM_HEAP *work = static_cast<DEBWIN_ITEM_HEAP*>(userWork);

  gfl::debug::DebugWin *debWin = GFL_SINGLETON_INSTANCE(gfl::debug::DebugWin);
  wchar_t* workStr = debWin->GetWorkStr();

  swprintf( workStr , gfl::debug::DEBUGWIN_NAME_LEN 
            ,L"TotalFree [0x%08x]" ,work->heap->GetTotalFreeSize() );
  
  return workStr;
}
wchar_t* DebugWin_HeapDrawNowAlloctable( void* userWork  , DebugWinItem *item )
{
  DEBWIN_ITEM_HEAP *work = static_cast<DEBWIN_ITEM_HEAP*>(userWork);

  gfl::debug::DebugWin *debWin = GFL_SINGLETON_INSTANCE(gfl::debug::DebugWin);
  wchar_t* workStr = debWin->GetWorkStr();

  swprintf( workStr , gfl::debug::DEBUGWIN_NAME_LEN 
            ,L"Allocatable [0x%08x]" ,work->heap->GetTotalAllocatableSize() );
  
  return workStr;
}
wchar_t* DebugWin_HeapDrawMaxLeast( void* userWork  , DebugWinItem *item )
{
  DEBWIN_ITEM_HEAP *work = static_cast<DEBWIN_ITEM_HEAP*>(userWork);

  gfl::debug::DebugWin *debWin = GFL_SINGLETON_INSTANCE(gfl::debug::DebugWin);
  wchar_t* workStr = debWin->GetWorkStr();

  swprintf( workStr , gfl::debug::DEBUGWIN_NAME_LEN 
            ,L"MinLeastSize[0x%08x]" ,work->heap->GetMaxLeastSize() );
  
  return workStr;
  
}

void DebugWin_AddItemHeap( DebugWinGroup *parent , const wchar_t *name , gfl::heap::HeapBase *target , heap::HeapBase *heap )
{
  DebugWin *debWin = GFL_SINGLETON_INSTANCE(DebugWin);
  gfl::debug::DebugWinGroup *grp = debWin->AddGroup(name , parent);

  DEBWIN_ITEM_HEAP *heapWork = static_cast<DEBWIN_ITEM_HEAP*>(grp->CreateGroupWork(sizeof(DEBWIN_ITEM_HEAP),heap));
  heapWork->heap = target;
  
  DebugWin_AddItemUpdateFunc( grp , L"Size" , heapWork , NULL , DebugWin_HeapDrawTotalSize ,  heap );
  DebugWin_AddItemUpdateFunc( grp , L"Free" , heapWork , NULL , DebugWin_HeapDrawTotalFree ,  heap );
  DebugWin_AddItemUpdateFunc( grp , L"Alloc" , heapWork , NULL , DebugWin_HeapDrawNowAlloctable ,  heap );
  DebugWin_AddItemUpdateFunc( grp , L"Least" , heapWork , NULL , DebugWin_HeapDrawMaxLeast ,  heap );
}

void DebugWin_AddItemHeap( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , gfl::heap::HeapBase *target , heap::HeapBase *heap )
{
  return DebugWin_AddItemHeap( parent , DebugWinUtil_GetMsgString(msgData,strId) , target , heap );
}

//---------------------------------------------------------
//独自更新型項目追加
DebugWinFunc* DebugWin_AddItemUpdateFunc( DebugWinGroup *parent , const wchar_t *name , void *work , DebugWinUpdateFunc updateFunc , DebugWinDrawFunc drawFunc , heap::HeapBase *heap )
{
  DebugWin *debWin = GFL_SINGLETON_INSTANCE(gfl::debug::DebugWin);
  heap::HeapBase *useHeap = (heap?heap:debWin->GetHeap());

  DebugWinFunc *item = GFL_NEW(useHeap)DebugWinFunc( parent , name , work , updateFunc , drawFunc );
  
  debWin->AddItem( item , parent );
  return item;
}

DebugWinFunc* DebugWin_AddItemUpdateFunc( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , void *work , DebugWinUpdateFunc updateFunc , DebugWinDrawFunc drawFunc , heap::HeapBase *heap )
{
  return DebugWin_AddItemUpdateFunc( parent , DebugWinUtil_GetMsgString(msgData,strId) , work , updateFunc , drawFunc , heap );
}

#else //GFL_DEBUGWIN_USE

DebugWinItemValueInt<int>* DebugWin_AddItemValueInt(...){return NULL;}
DebugWinItemValueFloat<f32>* DebugWin_AddItemValueFloat(...){return NULL;}
DebugWinItemValueInt<bool>* DebugWin_AddItemValueBool(...){return NULL;}
DebugWinItemValueVec3* DebugWin_AddItemValueVec3(...){return NULL;}
DebugWinItemValueVec4* DebugWin_AddItemValueVec4(...){return NULL;}
DebugWinItemValueColorF32* DebugWin_AddItemValueColorF32(...){return NULL;}

void DebugWin_AddItemCamera(...){}
void DebugWin_AddItemModel(...){}
void DebugWin_AddItemLight(...){}
void DebugWin_AddItemHeap(...){}
void DebugWin_AddItemUpdateFunc(...){}


#endif //GFL_DEBUGWIN_USE


}//namespace debug
}//namespace gfl
