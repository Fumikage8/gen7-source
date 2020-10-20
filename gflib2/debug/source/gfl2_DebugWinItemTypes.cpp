//======================================================================
/**
 * @file  gfl2_DebugwinItemTypes.cpp
 * @brief デバッグウィンドウ用アイテム(項目)
 * @author  ariizumi
 * @data  11/03/29
 */
//======================================================================
#include <nw/ut.h>
#include <ui/include/gfl2_UI.h>

#include <debug/include/gfl2_DebugWinItemTypes.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(debug)

#if GFL_DEBUGWIN_USE

#if defined(GF_PLATFORM_CTR)
#define WCHAR_PRINT_FUNC nw::ut::snwprintf
#elif defined(GF_PLATFORM_WIN32)
#define WCHAR_PRINT_FUNC   nw::ut::snw16printf
#endif  //プラットフォーム分岐

//---------------------------------------------------------
//数値操作用クラスbool型(独自処理追加)
const wchar_t* DebugWinItemValueBool::GetDispName(DebugWin *debSys)
{
  GFL_UNUSED(debSys);
  wchar_t* workStr = this->GetSysWorkStr();
  swprintf( workStr ,DEBUGWIN_NAME_LEN, L"%ls[%ls]",GetName() , (*mBoolTarget?L"ON":L"OFF"));
    
  return workStr;
}

void DebugWinItemValueBool::UpdateFunc(DebugWin *debSys)
{
  gfl2::ui::Button *uiButton = this->GetSysUiButton();
  gfl2::ui::VectorDevice *uiKey = this->GetSysUiCrossKey();
  if( uiKey->IsTrigger( gfl2::ui::BUTTON_LEFT ) ||
      uiKey->IsTrigger( gfl2::ui::BUTTON_RIGHT ) ||
      uiButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    *mBoolTarget = !*mBoolTarget;
    debSys->SoundCallBack(SE_CHANGE_VALUE);
    ChangedValue();
  }
}



//---------------------------------------------------------
//数値操作用クラスVec3型(独自処理追加)
const wchar_t* DebugWinItemValueVec3::GetDispName(DebugWin *debSys)
{
  GFL_UNUSED(debSys);

  wchar_t* workStr = this->GetSysWorkStr();
  wchar_t mark1 = (&mVecTarget->x == DebugWinItemValue<f32>::mTarget ? L'>' : L' ' );
  wchar_t mark2 = (&mVecTarget->y == DebugWinItemValue<f32>::mTarget ? L'>' : L' ' );
  wchar_t mark3 = (&mVecTarget->z == DebugWinItemValue<f32>::mTarget ? L'>' : L' ' );
  
  WCHAR_PRINT_FUNC( workStr ,
                    DEBUGWIN_NAME_LEN,
                    DEBUGWIN_NAME_LEN-1, 
                    L"%ls%c[%.1f]%c[%.1f]%c[%.1f]",
                    DebugWinItemValue<f32>::GetName(),
                    mark1, mVecTarget->x,
                    mark2, mVecTarget->y,
                    mark3, mVecTarget->z );
  return workStr;
}

void DebugWinItemValueVec3::UpdateFunc(DebugWin *debSys)
{
  GFL_UNUSED(debSys);

  gfl2::ui::Button *uiButton = this->GetSysUiButton();
  
  //操作対象の変更
  if( uiButton->IsTrigger( gfl2::ui::BUTTON_Y ) )
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
//数値操作用クラスVector3型(独自処理追加)
const wchar_t* DebugWinItemValueVector3::GetDispName(DebugWin *debSys)
{
  GFL_UNUSED(debSys);

  wchar_t* workStr = this->GetSysWorkStr();
  wchar_t mark1 = (&mVecTarget->x == DebugWinItemValue<f32>::mTarget ? L'>' : L' ' );
  wchar_t mark2 = (&mVecTarget->y == DebugWinItemValue<f32>::mTarget ? L'>' : L' ' );
  wchar_t mark3 = (&mVecTarget->z == DebugWinItemValue<f32>::mTarget ? L'>' : L' ' );
  
  WCHAR_PRINT_FUNC( workStr ,
                    DEBUGWIN_NAME_LEN,
                    DEBUGWIN_NAME_LEN-1, 
                    L"%ls%c[%.1f]%c[%.1f]%c[%.1f]",
                    DebugWinItemValue<f32>::GetName(),
                    mark1, mVecTarget->x,
                    mark2, mVecTarget->y,
                    mark3, mVecTarget->z );
  return workStr;
}

void DebugWinItemValueVector3::UpdateFunc(DebugWin *debSys)
{
  GFL_UNUSED(debSys);

  gfl2::ui::Button *uiButton = this->GetSysUiButton();
  
  //操作対象の変更
  if( uiButton->IsTrigger( gfl2::ui::BUTTON_Y ) )
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
  GFL_UNUSED(debSys);

  wchar_t* workStr = this->GetSysWorkStr();
  wchar_t mark1 = (&mVecTarget->x == DebugWinItemValue<f32>::mTarget ? L'>' : L' ' );
  wchar_t mark2 = (&mVecTarget->y == DebugWinItemValue<f32>::mTarget ? L'>' : L' ' );
  wchar_t mark3 = (&mVecTarget->z == DebugWinItemValue<f32>::mTarget ? L'>' : L' ' );
  wchar_t mark4 = (&mVecTarget->w == DebugWinItemValue<f32>::mTarget ? L'>' : L' ' );
  
  

  WCHAR_PRINT_FUNC( workStr ,DEBUGWIN_NAME_LEN,DEBUGWIN_NAME_LEN-1, L"%ls%c[%.1f]%c[%.1f]%c[%.1f]%c[%.1f]",DebugWinItemValue<f32>::GetName() 
                                                                      ,mark1, mVecTarget->x
                                                                      ,mark2, mVecTarget->y
                                                                      ,mark3, mVecTarget->z
                                                                      ,mark4, mVecTarget->w );
  return workStr;
}

void DebugWinItemValueVec4::UpdateFunc(DebugWin *debSys)
{
  gfl2::ui::Button *uiButton = this->GetSysUiButton();
  
  //操作対象の変更
  if( uiButton->IsTrigger( gfl2::ui::BUTTON_Y ) )
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
//数値操作用クラスVector4型(独自処理追加)
const wchar_t* DebugWinItemValueVector4::GetDispName(DebugWin *debSys)
{
  GFL_UNUSED(debSys);

  wchar_t* workStr = this->GetSysWorkStr();
  wchar_t mark1 = (&mVecTarget->x == DebugWinItemValue<f32>::mTarget ? L'>' : L' ' );
  wchar_t mark2 = (&mVecTarget->y == DebugWinItemValue<f32>::mTarget ? L'>' : L' ' );
  wchar_t mark3 = (&mVecTarget->z == DebugWinItemValue<f32>::mTarget ? L'>' : L' ' );
  wchar_t mark4 = (&mVecTarget->w == DebugWinItemValue<f32>::mTarget ? L'>' : L' ' );
  
  WCHAR_PRINT_FUNC( workStr ,
                    DEBUGWIN_NAME_LEN,
                    DEBUGWIN_NAME_LEN-1, 
                    L"%ls%c[%.2f]%c[%.2f]%c[%.2f]%c[%.2f]",
                    DebugWinItemValue<f32>::GetName(),
                    mark1, mVecTarget->x,
                    mark2, mVecTarget->y,
                    mark3, mVecTarget->z,
                    mark4, mVecTarget->w );
  return workStr;
}

void DebugWinItemValueVector4::UpdateFunc(DebugWin *debSys)
{
  GFL_UNUSED(debSys);

  gfl2::ui::Button *uiButton = this->GetSysUiButton();
  
  //操作対象の変更
  if( uiButton->IsTrigger( gfl2::ui::BUTTON_Y ) )
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
//数値操作用クラス整数型MsgData版(独自処理追加)
DebugWinItemValueIntMsg::DebugWinItemValueIntMsg( DebugWinGroup *parent , const wchar_t *name , int *target , fs::ArcFile::ARCID arcID, u32 datID , int min, int max , int msgOfs , heap::HeapBase *heap )
  :DebugWinItemValueInt<int>( parent , name , target , min , max , 1 )
  ,mMsgOffset(msgOfs)
  ,mDispStrNo(-1)
{

  mMsgData = GFL_NEW( heap ) gfl2::str::MsgData( arcID, datID, heap );
  mWorkStr = GFL_NEW( heap ) gfl2::str::StrBuf(DEBUGWIN_NAME_LEN,heap);
  
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
  DebugWin *debWin = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin);
  str::StrBuf *workStr = debWin->GetWorkStrBuf();
  msgData->GetString( strId,*workStr );
  return workStr->GetPtr();
}

//---------------------------------------------------------
//数値系
DebugWinItemValueU64<u64>* DebugWin_AddItemValueU64( DebugWinGroup *parent , heap::HeapBase *heap , const wchar_t *name , u64 *target , u64 min , u64 max , u64 step )
{
  DebugWin *debWin = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin);
  heap::HeapBase *useHeap = (heap?heap:debWin->GetHeap());

  DebugWinItemValueU64<u64> *item = GFL_NEW(useHeap)DebugWinItemValueU64<u64>( parent , name , target , min , max , step );
  
  debWin->AddItem( item , parent );
  return item;
}
DebugWinItemValueU64<u64>* DebugWin_AddItemValueU64( DebugWinGroup *parent , heap::HeapBase *heap , str::MsgData *msgData , u32 strId , u64 *target , u64 min , u64 max , u64 step )
{
  return DebugWin_AddItemValueU64( parent , heap , DebugWinUtil_GetMsgString(msgData,strId) , target , min , max , step );
}


DebugWinItemValueU8<u8>* DebugWin_AddItemValueU8( DebugWinGroup *parent , heap::HeapBase *heap , const wchar_t *name , u8 *target , u8 min , u8 max , u8 step )
{
  DebugWin *debWin = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin);
  heap::HeapBase *useHeap = (heap?heap:debWin->GetHeap());

  DebugWinItemValueU8<u8> *item = GFL_NEW(useHeap)DebugWinItemValueU8<u8>( parent , name , target , min , max , step );

  debWin->AddItem( item , parent );
  return item;
}
DebugWinItemValueU8<u8>* DebugWin_AddItemValueU8( DebugWinGroup *parent , heap::HeapBase *heap , str::MsgData *msgData , u32 strId , u8 *target , u8 min , u8 max , u8 step )
{
  return DebugWin_AddItemValueU8( parent , heap , DebugWinUtil_GetMsgString(msgData,strId) , target , min , max , step );
}
DebugWinItemValueU16<u16>* DebugWin_AddItemValueU16( DebugWinGroup *parent , heap::HeapBase *heap , const wchar_t *name , u16 *target , u16 min , u16 max , u16 step )
{
  DebugWin *debWin = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin);
  heap::HeapBase *useHeap = (heap?heap:debWin->GetHeap());

  DebugWinItemValueU16<u16> *item = GFL_NEW(useHeap)DebugWinItemValueU16<u16>( parent , name , target , min , max , step );

  debWin->AddItem( item , parent );
  return item;
}
DebugWinItemValueU16<u16>* DebugWin_AddItemValueU16( DebugWinGroup *parent , heap::HeapBase *heap , str::MsgData *msgData , u32 strId , u16 *target , u16 min , u16 max , u16 step )
{
  return DebugWin_AddItemValueU16( parent , heap , DebugWinUtil_GetMsgString(msgData,strId) , target , min , max , step );
}

DebugWinItemValueU32<u32>* DebugWin_AddItemValueU32( DebugWinGroup *parent , heap::HeapBase *heap , const wchar_t *name , u32 *target , u32 min , u32 max , u32 step )
{
  DebugWin *debWin = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin);
  heap::HeapBase *useHeap = (heap?heap:debWin->GetHeap());

  DebugWinItemValueU32<u32> *item = GFL_NEW(useHeap)DebugWinItemValueU32<u32>( parent , name , target , min , max , step );

  debWin->AddItem( item , parent );
  return item;
}
DebugWinItemValueU32<u32>* DebugWin_AddItemValueU32( DebugWinGroup *parent , heap::HeapBase *heap , str::MsgData *msgData , u32 strId , u32 *target , u32 min , u32 max , u32 step )
{
  return DebugWin_AddItemValueU32( parent , heap , DebugWinUtil_GetMsgString(msgData,strId) , target , min , max , step );
}

DebugWinItemValueInt<int>* DebugWin_AddItemValueInt( DebugWinGroup *parent , heap::HeapBase *heap , const wchar_t *name , int *target , int min , int max , int step )
{
  DebugWin *debWin = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin);
  heap::HeapBase *useHeap = (heap?heap:debWin->GetHeap());

  DebugWinItemValueInt<int> *item = GFL_NEW(useHeap)DebugWinItemValueInt<int>( parent , name , target , min , max , step );

  debWin->AddItem( item , parent );
  return item;
}
DebugWinItemValueInt<int>* DebugWin_AddItemValueInt( DebugWinGroup *parent , heap::HeapBase *heap , str::MsgData *msgData , u32 strId , int *target , int min , int max , int step )
{
  return DebugWin_AddItemValueInt( parent , heap , DebugWinUtil_GetMsgString(msgData,strId) , target , min , max , step );
}

DebugWinItemValueFloat<f32>* DebugWin_AddItemValueFloat( DebugWinGroup *parent , heap::HeapBase *heap , const wchar_t *name , f32 *target , f32 min , f32 max , f32 step )
{
  DebugWin *debWin = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin);
  heap::HeapBase *useHeap = (heap?heap:debWin->GetHeap());

  DebugWinItemValueFloat<f32> *item = GFL_NEW(useHeap)DebugWinItemValueFloat<f32>( parent , name , target , min , max , step );
  
  debWin->AddItem( item , parent );
  return item;
}
DebugWinItemValueFloat<f32>* DebugWin_AddItemValueFloat( DebugWinGroup *parent , heap::HeapBase *heap , str::MsgData *msgData , u32 strId , f32 *target , f32 min , f32 max , f32 step )
{
  return DebugWin_AddItemValueFloat( parent , heap , DebugWinUtil_GetMsgString(msgData,strId) , target , min , max , step );
}

DebugWinItemValueBool* DebugWin_AddItemValueBool( DebugWinGroup *parent , heap::HeapBase *heap , const wchar_t *name , bool *target )
{
  DebugWin *debWin = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin);
  heap::HeapBase *useHeap = (heap?heap:debWin->GetHeap());

  DebugWinItemValueBool *item = GFL_NEW(useHeap)DebugWinItemValueBool( parent , name , target );
  
  debWin->AddItem( item , parent );
  return item;
}
DebugWinItemValueBool* DebugWin_AddItemValueBool( DebugWinGroup *parent , heap::HeapBase *heap , str::MsgData *msgData , u32 strId , bool *target )
{
  return DebugWin_AddItemValueBool( parent , heap , DebugWinUtil_GetMsgString(msgData,strId) , target );
}

DebugWinItemValueVec3* DebugWin_AddItemValueVec3( DebugWinGroup *parent , heap::HeapBase *heap , const wchar_t *name , gfl2::math::VEC3 *target , f32 min , f32 max , f32 step )
{
  DebugWin *debWin = GFL_SINGLETON_INSTANCE(DebugWin);
  heap::HeapBase *useHeap = (heap?heap:debWin->GetHeap());

  DebugWinItemValueVec3 *item = GFL_NEW(useHeap)DebugWinItemValueVec3( parent , name , target , min , max , step );
  
  debWin->AddItem( item , parent );
  return item;
}
DebugWinItemValueVec3* DebugWin_AddItemValueVec3( DebugWinGroup *parent , heap::HeapBase *heap , str::MsgData *msgData , u32 strId , gfl2::math::VEC3 *target , f32 min , f32 max , f32 step )
{
  return DebugWin_AddItemValueVec3( parent , heap , DebugWinUtil_GetMsgString(msgData,strId) , target , min , max , step );
}

DebugWinItemValueVector3* DebugWin_AddItemValueVector3( DebugWinGroup *parent , heap::HeapBase *heap , const wchar_t *name , gfl2::math::Vector3 *target , f32 min , f32 max , f32 step )
{
  DebugWin *debWin = GFL_SINGLETON_INSTANCE(DebugWin);
  heap::HeapBase *useHeap = (heap?heap:debWin->GetHeap());

  DebugWinItemValueVector3 *item = GFL_NEW(useHeap)DebugWinItemValueVector3( parent , name , target , min , max , step );
  
  debWin->AddItem( item , parent );
  return item;
}
DebugWinItemValueVector3* DebugWin_AddItemValueVector3( DebugWinGroup *parent , heap::HeapBase *heap , str::MsgData *msgData , u32 strId , gfl2::math::Vector3 *target , f32 min , f32 max , f32 step )
{
  return DebugWin_AddItemValueVector3( parent , heap , DebugWinUtil_GetMsgString(msgData,strId) , target , min , max , step );
}

DebugWinItemValueVec4* DebugWin_AddItemValueVec4( DebugWinGroup *parent , heap::HeapBase *heap , const wchar_t *name , gfl2::math::VEC4 *target , f32 min , f32 max , f32 step )
{
  DebugWin *debWin = GFL_SINGLETON_INSTANCE(DebugWin);
  heap::HeapBase *useHeap = (heap?heap:debWin->GetHeap());

  DebugWinItemValueVec4 *item = GFL_NEW(useHeap)DebugWinItemValueVec4( parent , name , target , min , max , step );
  
  debWin->AddItem( item , parent );
  return item;
}
DebugWinItemValueVec4* DebugWin_AddItemValueVec4( DebugWinGroup *parent , heap::HeapBase *heap , str::MsgData *msgData , u32 strId , gfl2::math::VEC4 *target , f32 min , f32 max , f32 step )
{
  return DebugWin_AddItemValueVec4( parent , heap , DebugWinUtil_GetMsgString(msgData,strId) , target , min , max , step );
}

DebugWinItemValueVector4* DebugWin_AddItemValueVector4( DebugWinGroup *parent , heap::HeapBase *heap , const wchar_t *name , gfl2::math::Vector4 *target , f32 min , f32 max , f32 step )
{
  DebugWin *debWin = GFL_SINGLETON_INSTANCE(DebugWin);
  heap::HeapBase *useHeap = (heap?heap:debWin->GetHeap());

  DebugWinItemValueVector4 *item = GFL_NEW(useHeap)DebugWinItemValueVector4( parent , name , target , min , max , step );
  
  debWin->AddItem( item , parent );
  return item;
}
DebugWinItemValueVector4* DebugWin_AddItemValueVector4( DebugWinGroup *parent , heap::HeapBase *heap , str::MsgData *msgData , u32 strId , gfl2::math::Vector4 *target , f32 min , f32 max , f32 step )
{
  return DebugWin_AddItemValueVector4( parent , heap , DebugWinUtil_GetMsgString(msgData,strId) , target , min , max , step );
}

DebugWinItemValueIntMsg* DebugWin_AddItemValueIntMsg( DebugWinGroup *parent , heap::HeapBase *heap , const wchar_t *name , int *target , fs::ArcFile::ARCID arcID, u32 datID , int min, int max , int msgOfs )
{
  DebugWin *debWin = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin);
  heap::HeapBase *useHeap = (heap?heap:debWin->GetHeap());

  DebugWinItemValueIntMsg *item = GFL_NEW(useHeap)DebugWinItemValueIntMsg( parent , name , target , arcID , datID , min , max , msgOfs , useHeap );
  
  debWin->AddItem( item , parent );
  return item;
}
DebugWinItemValueIntMsg* DebugWin_AddItemValueIntMsg( DebugWinGroup *parent , heap::HeapBase *heap , str::MsgData *msgData , u32 strId , int *target , fs::ArcFile::ARCID arcID, u32 datID , int min, int max , int msgOfs )
{
  return DebugWin_AddItemValueIntMsg( parent , heap , DebugWinUtil_GetMsgString(msgData,strId) , target , arcID , datID , min , max , msgOfs );
  
}

//---------------------------------------------------------
//Heap操作
//管理ワーク
typedef struct
{
  gfl2::heap::HeapBase *heap;
  
  
}DEBWIN_ITEM_HEAP;

wchar_t* DebugWin_HeapDrawTotalSize( void* userWork  , DebugWinItem *item )
{
  GFL_UNUSED(item);
  DEBWIN_ITEM_HEAP *work = static_cast<DEBWIN_ITEM_HEAP*>(userWork);

  gfl2::debug::DebugWin *debWin = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin);
  wchar_t* workStr = debWin->GetWorkStr();

  swprintf( workStr , gfl2::debug::DEBUGWIN_NAME_LEN 
            ,L"TotalSize [0x%08x]" ,work->heap->GetTotalSize() );

  return workStr;
  
}
wchar_t* DebugWin_HeapDrawTotalFree( void* userWork  , DebugWinItem *item )
{
  GFL_UNUSED(item);
  DEBWIN_ITEM_HEAP *work = static_cast<DEBWIN_ITEM_HEAP*>(userWork);

  gfl2::debug::DebugWin *debWin = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin);
  wchar_t* workStr = debWin->GetWorkStr();

  swprintf( workStr , gfl2::debug::DEBUGWIN_NAME_LEN 
            ,L"TotalFree [0x%08x]" ,work->heap->GetTotalFreeSize() );
  
  return workStr;
}
wchar_t* DebugWin_HeapDrawNowAlloctable( void* userWork  , DebugWinItem *item )
{
  GFL_UNUSED(item);
  DEBWIN_ITEM_HEAP *work = static_cast<DEBWIN_ITEM_HEAP*>(userWork);

  gfl2::debug::DebugWin *debWin = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin);
  wchar_t* workStr = debWin->GetWorkStr();

  swprintf( workStr , gfl2::debug::DEBUGWIN_NAME_LEN 
            ,L"Allocatable [0x%08x]" ,work->heap->GetTotalAllocatableSize() );
  
  return workStr;
}
wchar_t* DebugWin_HeapDrawMaxLeast( void* userWork  , DebugWinItem *item )
{
  GFL_UNUSED(item);
  DEBWIN_ITEM_HEAP *work = static_cast<DEBWIN_ITEM_HEAP*>(userWork);

  gfl2::debug::DebugWin *debWin = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin);
  wchar_t* workStr = debWin->GetWorkStr();

  swprintf( workStr , gfl2::debug::DEBUGWIN_NAME_LEN 
            ,L"MinLeastSize[0x%08x]" ,work->heap->GetMaxLeastSize() );
  
  return workStr;
  
}

void DebugWin_AddItemHeap( DebugWinGroup *parent , heap::HeapBase *heap , const wchar_t *name , gfl2::heap::HeapBase *target )
{
  DebugWin *debWin = GFL_SINGLETON_INSTANCE(DebugWin);
  gfl2::debug::DebugWinGroup *grp = debWin->AddGroup(heap,name,parent);

  DEBWIN_ITEM_HEAP *heapWork = static_cast<DEBWIN_ITEM_HEAP*>(grp->CreateGroupWork(sizeof(DEBWIN_ITEM_HEAP),heap));
  heapWork->heap = target;
  
  DebugWin_AddItemUpdateFunc( grp , heap , L"Size" , heapWork , NULL , DebugWin_HeapDrawTotalSize );
  DebugWin_AddItemUpdateFunc( grp , heap , L"Free" , heapWork , NULL , DebugWin_HeapDrawTotalFree );
  DebugWin_AddItemUpdateFunc( grp , heap , L"Alloc" , heapWork , NULL , DebugWin_HeapDrawNowAlloctable );
  DebugWin_AddItemUpdateFunc( grp , heap , L"Least" , heapWork , NULL , DebugWin_HeapDrawMaxLeast );
}

void DebugWin_AddItemHeap( DebugWinGroup *parent , heap::HeapBase *heap , str::MsgData *msgData , u32 strId , gfl2::heap::HeapBase *target )
{
  return DebugWin_AddItemHeap( parent , heap , DebugWinUtil_GetMsgString(msgData,strId) , target );
}

//---------------------------------------------------------
//独自更新型項目追加
DebugWinFunc* DebugWin_AddItemUpdateFunc( DebugWinGroup *parent , heap::HeapBase *heap , const wchar_t *name , void *work , DebugWinUpdateFunc updateFunc , DebugWinDrawFunc drawFunc, const s32 iScrId )
{
  DebugWin *debWin = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin);
  heap::HeapBase *useHeap = (heap?heap:debWin->GetHeap());

  DebugWinFunc *item = GFL_NEW(useHeap)DebugWinFunc( parent , name , work , updateFunc , drawFunc, iScrId );
  
  debWin->AddItem( item , parent );
  return item;
}

DebugWinFunc* DebugWin_AddItemUpdateFunc( DebugWinGroup *parent , heap::HeapBase *heap , str::MsgData *msgData , u32 strId , void *work , DebugWinUpdateFunc updateFunc , DebugWinDrawFunc drawFunc, const s32 iScrId )
{
  return DebugWin_AddItemUpdateFunc( parent , heap , DebugWinUtil_GetMsgString(msgData,strId) , work , updateFunc , drawFunc, iScrId );
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


GFL_NAMESPACE_END(debug)
GFL_NAMESPACE_END(gfl2)
