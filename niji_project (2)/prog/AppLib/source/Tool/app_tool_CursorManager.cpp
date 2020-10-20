//============================================================================================
/**
 * @file    app_tool_CursorManager.cpp
 * @brief   カーソル移動処理
 * @author  Hiroyuki Nakamura
 * @date    11.07.26
 */
//============================================================================================
#include "../../include/Tool/app_tool_CursorManager.h"
#include <GameSys/include/GameManager.h>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * @namespace  app::tool
 * @brief      アプリツール
 */
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )

#define  KEY_MODE_DEFAULT    ( 0xff )    //!< 履歴デフォルトデータ

//! @brief 移動方向
enum {
  MV_UP = 0,  //!< 上
  MV_DOWN,    //!< 下
  MV_LEFT,    //!< 左
  MV_RIGHT,   //!< 右
  MV_NONE     //!< なし
};


//--------------------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ（移動履歴作成方式）
 *
 * @param   mvdata        カーソル移動データ
 * @param   data_max      データ数
 * @param   heap          ヒープ
 * @param   init_pos      初期カーソル位置
 * @param   stick_enable  アナログパッドを有効にするか
 */
//--------------------------------------------------------------------------------------------
CursorManager::CursorManager( const CursorManager::MOVE_DATA * mvdata, u32 data_max, gfl2::heap::HeapBase * heap, u8 init_pos, bool stick_enable )
{
  dat     = mvdata;
  dataMax = data_max;
  nowPos  = init_pos;
  oldPos  = nowPos;
  pHeap   = heap;

  stickCount = 0;
  stickWait = 3;

  InitMoveEnable();
  CreateMoveRecord();

  GameSys::GameManager * pgm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager * pdm = pgm->GetUiDeviceManager();
  if(stick_enable)
  {
    pKey = pdm->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STICK_EMU );
  }
  else
  {
    pKey = pdm->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ（移動履歴参照方式）
 *
 * @param   mvdata      カーソル移動データ
 * @param   data_max    データ数
 * @param   rec_work    移動履歴
 * @param   init_pos    初期カーソル位置
 *
 *  移動履歴はdata_max分必要
 */
//--------------------------------------------------------------------------------------------
/*
CursorManager::CursorManager( const CursorManager::MOVE_DATA * mvdata, u32 data_max, CursorManager::MOVE_KEY * rec_work, u8 init_pos ) :
  pHeap( NULL )
{
  dat     = mvdata;
  dataMax = data_max;
  nowPos  = init_pos;
  oldPos  = nowPos;
  mvRec   = rec_work;

  stickCount = 0;
  stickWait = 3;

  InitMoveEnable();
  CreateMoveRecord();

  gamesystem::GameManager * pgm = GFL_SINGLETON_INSTANCE( gamesystem::GameManager );
  gfl2::ui::DeviceManager * pdm = pgm->GetUiDeviceManager();
  pKey = pdm->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STICK_EMU );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ（移動履歴なし方式）
 *
 * @param   mvdata        カーソル移動データ
 * @param   data_max      データ数
 * @param   init_pos      初期カーソル位置
 * @param   stick_enable  アナログパッドを有効にするか
 */
//--------------------------------------------------------------------------------------------
CursorManager::CursorManager( const MOVE_DATA * mvdata, u32 data_max, u8 init_pos, bool stick_enable) :
  pHeap( NULL ),
  mvRec( NULL )
{
  dat     = mvdata;
  dataMax = data_max;
  nowPos  = init_pos;
  oldPos  = nowPos;

  stickCount = 0;
  stickWait = 3;

  InitMoveEnable();

  GameSys::GameManager * pgm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager * pdm = pgm->GetUiDeviceManager();
  if(stick_enable)
  {
    pKey = pdm->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STICK_EMU );
  }
  else
  {
    pKey = pdm->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//--------------------------------------------------------------------------------------------
CursorManager::~CursorManager()
{
  DeleteMoveRecord();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   移動許可設定初期化
 *
 * @param   none
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void CursorManager::InitMoveEnable(void)
{
  gfl2::std::MemFill( mvTbl, 0xff, MV_TBL_MAX*4 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   移動許可設定
 *
 * @param   pos    データ位置
 * @param   flg    true = 移動可, false = 移動不可
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void CursorManager::SetMoveEnable( u8 pos, bool flg )
{
  if( pos >= (MV_TBL_MAX*32) ){
    GFL_ASSERT( 0 );
    return;
  }

  u32 * tbl = &mvTbl[pos/32];

  if( flg == true ){
    *tbl = ( *tbl & 0xffffffff ) | ( 1<<(pos%32) );
  }else{
    *tbl = *tbl & ( 0xffffffff ^ ( 1<<(pos%32) ) );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   移動許可チェック
 *
 * @param   pos    データ位置
 *
 * @retval  "0 = 移動不可"
 * @retval  "1 = 移動可"
 */
//--------------------------------------------------------------------------------------------
u32 CursorManager::GetMoveEnable( u8 pos ) const
{
  if( pos >= (MV_TBL_MAX*32) ){
    GFL_ASSERT( 0 );
    return 0;
  }

  u32  tbl = mvTbl[pos/32];

  return ( ( tbl >> (pos%32) ) & 1 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   メイン処理（ベタ）
 *
 * @param   none
 *
 * @return  動作結果
 */
//--------------------------------------------------------------------------------------------
CursorManager::MOVE_RET CursorManager::MainHold(void)
{
  if( pKey->IsHold( gfl2::ui::DIR_UP ) ){
    if( MoveMain( MV_UP ) == false ){
      return CursorManager::RET_NO_MOVE_UP;
    }
    return CursorManager::RET_MOVE;
  }
  if( pKey->IsHold( gfl2::ui::DIR_DOWN ) ){
    if( MoveMain( MV_DOWN ) == false ){
      return CursorManager::RET_NO_MOVE_DOWN;
    }
    return CursorManager::RET_MOVE;
  }
  if( pKey->IsHold( gfl2::ui::DIR_LEFT ) ){
    if( MoveMain( MV_LEFT ) == false ){
      return CursorManager::RET_NO_MOVE_LEFT;
    }
    return CursorManager::RET_MOVE;
  }
  if( pKey->IsHold( gfl2::ui::DIR_RIGHT ) ){
    if( MoveMain( MV_RIGHT ) == false ){
      return CursorManager::RET_NO_MOVE_RIGHT;
    }
    return CursorManager::RET_MOVE;
  }
  return CursorManager::RET_MOVE_NONE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   メイン処理（トリガ）
 *
 * @param   none
 *
 * @return  動作結果
 */
//--------------------------------------------------------------------------------------------
CursorManager::MOVE_RET CursorManager::MainTrigger(void)
{
  if( pKey->IsTrigger( gfl2::ui::DIR_UP ) ){
    if( MoveMain( MV_UP ) == false ){
      return CursorManager::RET_NO_MOVE_UP;
    }
    return CursorManager::RET_MOVE;
  }
  if( pKey->IsTrigger( gfl2::ui::DIR_DOWN ) ){
    if( MoveMain( MV_DOWN ) == false ){
      return CursorManager::RET_NO_MOVE_DOWN;
    }
    return CursorManager::RET_MOVE;
  }
  if( pKey->IsTrigger( gfl2::ui::DIR_LEFT ) ){
    if( MoveMain( MV_LEFT ) == false ){
      return CursorManager::RET_NO_MOVE_LEFT;
    }
    return CursorManager::RET_MOVE;
  }
  if( pKey->IsTrigger( gfl2::ui::DIR_RIGHT ) ){
    if( MoveMain( MV_RIGHT ) == false ){
      return CursorManager::RET_NO_MOVE_RIGHT;
    }
    return CursorManager::RET_MOVE;
  }
  return CursorManager::RET_MOVE_NONE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   メイン処理（リピート）
 *
 * @param   none
 *
 * @return  動作結果
 */
//--------------------------------------------------------------------------------------------
CursorManager::MOVE_RET CursorManager::MainRepeat(void)
{
  if( pKey->IsRepeat( gfl2::ui::DIR_UP ) ){
    if( MoveMain( MV_UP ) == false ){
      return CursorManager::RET_NO_MOVE_UP;
    }
    return CursorManager::RET_MOVE;
  }
  if( pKey->IsRepeat( gfl2::ui::DIR_DOWN ) ){
    if( MoveMain( MV_DOWN ) == false ){
      return CursorManager::RET_NO_MOVE_DOWN;
    }
    return CursorManager::RET_MOVE;
  }
  if( pKey->IsRepeat( gfl2::ui::DIR_LEFT ) ){
    if( MoveMain( MV_LEFT ) == false ){
      return CursorManager::RET_NO_MOVE_LEFT;
    }
    return CursorManager::RET_MOVE;
  }
  if( pKey->IsRepeat( gfl2::ui::DIR_RIGHT ) ){
    if( MoveMain( MV_RIGHT ) == false ){
      return CursorManager::RET_NO_MOVE_RIGHT;
    }
    return CursorManager::RET_MOVE;
  }
  return CursorManager::RET_MOVE_NONE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   位置設定
 *
 * @param   pos    位置
 *
 * @retval  "true = 移動した"
 * @retval  "false = それ以外"
 */
//--------------------------------------------------------------------------------------------
bool CursorManager::SetPos( u8 pos )
{
  if( pos == nowPos ){
    return false;
  }
  oldPos = nowPos;
  nowPos = pos;
  return true;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   現在の位置取得
 *
 * @param   none
 *
 * @return  現在の位置
 */
//--------------------------------------------------------------------------------------------
u8 CursorManager::GetPos(void) const
{
  return nowPos;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   前回の位置取得
 *
 * @param   none
 *
 * @return  前回の位置
 */
//--------------------------------------------------------------------------------------------
u8 CursorManager::GetOldPos(void) const
{
  return oldPos;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   指定位置のID取得
 *
 * @param   pos    位置
 *
 * @return  ID
 */
//--------------------------------------------------------------------------------------------
u32 CursorManager::GetID( u32 pos ) const
{
  return dat[pos].id;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   現在位置のID取得
 *
 * @param   none
 *
 * @return  ID
 */
//--------------------------------------------------------------------------------------------
u32 CursorManager::GetID(void) const
{
  return GetID( nowPos );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   前回の位置のID取得
 *
 * @param   none
 *
 * @return  ID
 */
//--------------------------------------------------------------------------------------------
u32 CursorManager::GetOldID(void) const
{
  return GetID( oldPos );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   移動履歴作成
 *
 * @param   none
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void CursorManager::CreateMoveRecord(void)
{
  if( pHeap != NULL ){
    mvRec = GFL_NEW_ARRAY( pHeap ) MOVE_KEY[dataMax];
  }
  InitMoveRecord();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   移動履歴削除
 *
 * @param   none
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void CursorManager::DeleteMoveRecord(void)
{
  if( pHeap != NULL && mvRec != NULL ){
    GFL_DELETE_ARRAY  mvRec;
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   移動履歴初期化
 *
 * @param   none
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void CursorManager::InitMoveRecord(void)
{
  if( mvRec != NULL ){
    gfl2::std::MemFill( mvRec, KEY_MODE_DEFAULT, sizeof(MOVE_KEY)*dataMax );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   移動メイン処理
 *
 * @param   mv  移動方向
 *
 * @retval  "true = 移動した"
 * @retval  "false = それ以外"
 */
//--------------------------------------------------------------------------------------------
bool CursorManager::MoveMain( u32 mv )
{
  u8  tmpPos;
  u8  newPos;

  tmpPos = nowPos;
  newPos = nowPos;

  for ever
  {
    switch( mv ){
    case MV_UP:
      if( mvRec != NULL && mvRec[tmpPos].up != KEY_MODE_DEFAULT ){
        newPos = mvRec[tmpPos].up;
      }else{
        newPos = dat[tmpPos].key.up;
      }
      break;
    case MV_DOWN:
      if( mvRec != NULL && mvRec[tmpPos].down != KEY_MODE_DEFAULT ){
        newPos = mvRec[tmpPos].down;
      }else{
        newPos = dat[tmpPos].key.down;
      }
      break;
    case MV_LEFT:
      if( mvRec != NULL && mvRec[tmpPos].left != KEY_MODE_DEFAULT ){
        newPos = mvRec[tmpPos].left;
      }else{
        newPos = dat[tmpPos].key.left;
      }
      break;
    case MV_RIGHT:
      if( mvRec != NULL && mvRec[tmpPos].right != KEY_MODE_DEFAULT ){
        newPos = mvRec[tmpPos].right;
      }else{
        newPos = dat[tmpPos].key.right;
      }
      break;
    }

    if( newPos == tmpPos || newPos == nowPos ){
      return false;
    }

    if( GetMoveEnable( newPos ) == 1 ){
      break;
    }
    tmpPos = newPos;
  }

  if( mvRec != NULL ){
    switch( mv ){
    case MV_UP:
      mvRec[newPos].down = nowPos;
      break;
    case MV_DOWN:
      mvRec[newPos].up = nowPos;
      break;
    case MV_LEFT:
      mvRec[newPos].right = nowPos;
      break;
    case MV_RIGHT:
      mvRec[newPos].left = nowPos;
      break;
    }
  }

  SetPos( newPos );

  return true;
}


GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_END( app )
