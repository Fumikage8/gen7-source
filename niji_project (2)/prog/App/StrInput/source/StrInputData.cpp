//==============================================================================
/**
 * @file    StrInputData.cpp
 * @brief   キーボードデータ
 * @author  fukushima_yuya
 * @date    2015.05.22
 * @note    sango_project app strinput_data.cppから移植
 */
//==============================================================================

// niji
#include "App/StrInput/source/StrInputData.h"

#include <AppLib/include/Util/app_util_FileAccessor.h>

// gflib2
#include <fs/include/gfl2_Fs.h>

// arc
#include <arc_index/strinput_data.gaix>


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * @namespace   App::StrInput
 * @brief       文字入力画面
 */
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( StrInput )


static const gfl2::fs::ArcFile::ARCDATID keymap_dat[] = {
  GARC_strinput_data_keymap_kana_DAT,
  GARC_strinput_data_keymap_kata_DAT,
  GARC_strinput_data_keymap_abc_DAT,
  GARC_strinput_data_keymap_kigou_DAT,

  GARC_strinput_data_keymap_qwerty_DAT,
  GARC_strinput_data_keymap_qwerty_shift_DAT,

  GARC_strinput_data_keymap_qwerty_hangul_DAT,
  GARC_strinput_data_keymap_qwerty_shift_hangul_DAT,
};

static const gfl2::fs::ArcFile::ARCDATID change_dat[] = {
  GARC_strinput_data_change_dakuten_DAT,
  GARC_strinput_data_change_handakuten_DAT,
  GARC_strinput_data_change_qwerty_DAT,
};

static const gfl2::fs::ArcFile::ARCDATID loopcode_dat[] = {
  GARC_strinput_data_changeex_all_DAT
};


//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   heap  ヒープ
 */
//------------------------------------------------------------------------------
KeyMap::KeyMap( app::util::Heap * heap )
{
  CreateData( heap );
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
KeyMap::~KeyMap()
{
  DeleteData();
}

//------------------------------------------------------------------------------
/**
 * @brief   キーコード取得
 *
 * @param   mode    入力モード
 * @param   type    入力タイプ
 * @param   shift    シフトが押されているか
 * @param   id      入力位置
 *
 * @return  キーコード
 */
//------------------------------------------------------------------------------
gfl2::str::STRCODE & KeyMap::GetKeyCode( u32 mode, u32 type, u32 shift, u32 id )
{
  KEY_DATA * key_data;
  if( mode == INPUT_MODE_AIUEO )
  {
    key_data = static_cast<KEY_DATA *>( dat.key[type] );
  }
  else {
    key_data = static_cast<KEY_DATA *>( dat.key[type+shift] );
  }
  return key_data->code[id];
}

//------------------------------------------------------------------------------
/**
 * @brief   文字変換
 *
 * @param   mode    変換データモード
 * @param   in      入力文字列
 * @param   out     変換後の文字列
 *
 * @retval  "true = 変換した"
 * @retval  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool KeyMap::GetChangeCode( u32 mode, const gfl2::str::STRCODE * in, gfl2::str::STRCODE * out )
{
  CHANGE_DATA * chg = static_cast<CHANGE_DATA *>( changeData[mode] );

  for( int i=0; i<chg->num; i++ )
  {
    if( CheckCode( chg->word[i].input, in ) == true )
    {
      for( int j=0; j<CHANGE_CODE_LEN; j++ )
      {
        out[j] = chg->word[i].change[j];
        if( out[j] == CHANGE_END_CODE )
        {
          out[j] = gfl2::str::EOM_CODE;
        }
      }
      return true;
    }
  }
  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief    ローマ字変換
 *
 * @param   type    入力タイプ
 * @param   in      入力文字列
 * @param   out     変換後の文字列
 * @param   rest    余剰データの格納場所
 *
 * @retval  "true = 変換した"
 * @retval  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool KeyMap::GetChangeRomajiCode( u32 type, const gfl2::str::STRCODE * in, gfl2::str::STRCODE * out, gfl2::str::STRCODE * rest )
{
  CHANGE_DATA * chg = static_cast<CHANGE_DATA *>( changeData[CHG_DATA_QWERTY] );

  for( int i=0; i<chg->num; i++ )
  {
    if( CheckCode( chg->word[i].input, in ) == true )
    {
      const gfl2::str::STRCODE * dst;
      if( type == INPUT_TYPE_HIRAGANA )
      {
        dst = chg->word[i].change;
      }
      else {
        dst = chg->word[i].shift;
      }

      for( int j=0; j<CHANGE_CODE_LEN; j++ )
      {
        out[j] = dst[j];
        if( out[j] == CHANGE_END_CODE )
        {
          const gfl2::str::STRCODE * w_rest = chg->word[i].rest;
          if( w_rest[0] != CHANGE_REST_NULL_CODE )
          {
            rest[0] = w_rest[0];
            rest[1] = gfl2::str::EOM_CODE;
          }
          out[j] = gfl2::str::EOM_CODE;
          break;
        }
      }
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief    循環コード変換
 *
 * @param   in      入力文字列
 * @param   out     変換後の文字列
 *
 * @retval  "true = 変換した"
 * @retval  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool KeyMap::GetLoopCode( const gfl2::str::STRCODE in, gfl2::str::STRCODE * out )
{
  LOOP_CODE_DATA * dat = static_cast<LOOP_CODE_DATA *>( loopData );

  for( int i=0; i<dat->num; i++ )
  {
    gfl2::str::STRCODE * code = &dat->code[i*CHANGE_LOOP_CODE_MAX];
    for( int j=0; j<CHANGE_LOOP_CODE_MAX; j++ )
    {
      if( code[j] == CHANGE_END_CODE || code[j] == CHANGE_END_CODE_LOCALIZE )
      {
        break;
      }

      if( code[j] == in )
      {
        if( j == (CHANGE_LOOP_CODE_MAX-1) )
        {
          j = 0;
        }
        else {
          j++;
          if( code[j] == CHANGE_END_CODE || code[j] == CHANGE_END_CODE_LOCALIZE )
          {
            j = 0;
          }
        }
        out[0] = code[j];
        return true;
      }
    }
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   変換コード比較
 *
 * @param   c1  文字列１
 * @param   c2  文字列２
 *
 * @retval  "true = 同じ"
 * @retval  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool KeyMap::CheckCode( const gfl2::str::STRCODE * c1, const gfl2::str::STRCODE * c2 )
{
  for( int i=0; i<CHANGE_CODE_LEN; i++ )
  {
    if( c1[i] != c2[i] )
    {
      return false;
    }
  }

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   終端コードを変換終了コードに置き換え
 *
 * @param   str    文字列
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void KeyMap::MakeCheckCode( gfl2::str::STRCODE * str )
{
  for( int i=0; i<CHANGE_CODE_LEN; i++ )
  {
    if( str[i] == gfl2::str::EOM_CODE )
    {
      str[i] = CHANGE_END_CODE;
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   データ作成
 *
 * @param   heap  ヒープ
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void KeyMap::CreateData( app::util::Heap * heap )
{
  // データの読み込みを変更
  //gfl2::heap::HeapBase* pHeap = heap->GetDeviceHeap();
  //gfl2::fs::ArcFile * af = GFL_NEW_LOW(pHeap) gfl2::fs::ArcFile();

  //CreateKeyMap( af, heap );
  //CreateChangeCode( af, heap );
  //CreateLoopCode( af, heap );

  //GFL_DELETE af;
}

//------------------------------------------------------------------------------
/**
 * @brief   データ削除
 *
 * @param   heap  ヒープ
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void KeyMap::DeleteData( void )
{
  DeleteLoopCode();
  DeleteChangeCode();
  DeleteKeyMap();
}

//------------------------------------------------------------------------------
/**
 * @brief   キーマップデータ作成
 *
 * @param   af    アークファイル
 * @param   heap  ヒープ
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void KeyMap::CreateKeyMap( gfl2::fs::ArcFile * af, app::util::Heap * heap )
{
  //static const gfl2::fs::ArcFile::ARCID tbl[] = {
  //  GARC_strinput_data_keymap_kana_DAT,
  //  GARC_strinput_data_keymap_kata_DAT,
  //  GARC_strinput_data_keymap_abc_DAT,
  //  GARC_strinput_data_keymap_kigou_DAT,

  //  GARC_strinput_data_keymap_qwerty_DAT,
  //  GARC_strinput_data_keymap_qwerty_shift_DAT,

  //  GARC_strinput_data_keymap_qwerty_hangul_DAT,
  //  GARC_strinput_data_keymap_qwerty_shift_hangul_DAT,
  //};

  //gfl2::heap::HeapBase* pHeapForFile = heap->GetDeviceHeap();
  //gfl2::heap::HeapBase* pHeapForReq  = heap->GetDeviceHeap();

  //for( int i=0; i<KEY_DATA_MAX; i++ )
  //{
  //  size_t siz, rsiz;
  //  af->GetDataSize( &siz, tbl[i], NULL );
  //  dat.key[i] = GflHeapAllocMemory( heap->GetDeviceHeap(), siz );

  //  af->LoadData( &dat.key[i], &siz, &rsiz, tbl[i], pHeapForFile, 128, pHeapForReq );
  //}
}

//------------------------------------------------------------------------------
/**
 * @brief   キーマップデータ削除
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void KeyMap::DeleteKeyMap( void )
{
  for( int i=0; i<KEY_DATA_MAX; i++ )
  {
    GflHeapFreeMemory( dat.key[i] );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   変換データ作成
 *
 * @param   af    アークファイル
 * @param   heap  ヒープ
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void KeyMap::CreateChangeCode( gfl2::fs::ArcFile * af, app::util::Heap * heap )
{
  //static const gfl2::fs::ArcFile::ARCID ChangeDataTable[] = {
  //  GARC_strinput_data_change_dakuten_DAT,
  //  GARC_strinput_data_change_handakuten_DAT,
  //  GARC_strinput_data_change_qwerty_DAT,
  //};

  //gfl2::heap::HeapBase* pHeapForFile = heap->GetDeviceHeap();
  //gfl2::heap::HeapBase* pHeapForReq  = heap->GetDeviceHeap();

  //for( int i=0; i<CHG_DATA_MAX; i++ )
  //{
  //  size_t siz, rsiz;
  //  af->GetDataSize( &siz, ChangeDataTable[i], NULL );
  //  changeData[i] = GflHeapAllocMemory( heap->GetDeviceHeap(), siz );
  //  af->LoadData( &changeData[i], &siz, &rsiz, ChangeDataTable[i], pHeapForFile, 128, pHeapForReq );
  //}
}

//------------------------------------------------------------------------------
/**
 * @brief   変換データ削除
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void KeyMap::DeleteChangeCode( void )
{
  for( int i=0; i<CHG_DATA_MAX; i++ )
  {
    GflHeapFreeMemory( changeData[i] );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   循環データ作成
 *
 * @param   af    アークファイル
 * @param   heap  ヒープ
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void KeyMap::CreateLoopCode( gfl2::fs::ArcFile * af, app::util::Heap * heap )
{
  //gfl2::heap::HeapBase* pHeapForFile = heap->GetDeviceHeap();
  //gfl2::heap::HeapBase* pHeapForReq  = heap->GetDeviceHeap();

  //gfl2::fs::ArcFile::ARCID arcID = GARC_strinput_data_changeex_all_DAT;
  //size_t siz, rsiz;

  //af->GetDataSize( &siz, arcID, NULL );
  //loopData = GflHeapAllocMemory( heap->GetDeviceHeap(), siz );
  //af->LoadData( &loopData, &siz, &rsiz, arcID, pHeapForFile, 128, pHeapForReq );
}

//------------------------------------------------------------------------------
/**
 * @brief   循環データ削除
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void KeyMap::DeleteLoopCode( void )
{
  GflHeapFreeMemory( loopData );
}

//----------------------------------------------------------------------------
/**
 * @brief   ファイルの読み込み
 *
 * @param   arcID   ARCファイルID
 * @param   heap    ヒープ
 * @param   dataIdx データインデックス
 * @param   fileIdx ファイルインデックス
 *
 * @return  none
 */
//----------------------------------------------------------------------------
void KeyMap::LoadData( gfl2::fs::ArcFile::ARCID arcID, gfl2::heap::HeapBase* heap, u32 dataIdx, u32 fileIdx )
{
  switch( dataIdx )
  {
    case 0:
      app::util::FileAccessor::FileLoad(
        arcID,
        keymap_dat[fileIdx],
        &dat.key[fileIdx],
        heap,
        false,
        nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );
      FUKUSHIMA_PRINT(
        "FileLoad : arcID[%d], dataID[%d], dataIdx[%d], fileIdx[%d]\n",
        arcID, keymap_dat[fileIdx], dataIdx, fileIdx );
      break;
    case 1:
      app::util::FileAccessor::FileLoad(
        arcID,
        change_dat[fileIdx],
        &changeData[fileIdx],
        heap,
        false,
        nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );
      FUKUSHIMA_PRINT(
        "FileLoad : arcID[%d], dataID[%d], dataIdx[%d], fileIdx[%d]\n",
        arcID, change_dat[fileIdx], dataIdx, fileIdx );
      break;
    case 2:
      app::util::FileAccessor::FileLoad(
        arcID,
        loopcode_dat[fileIdx],
        &loopData,
        heap,
        false,
        nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );
      FUKUSHIMA_PRINT(
        "FileLoad : arcID[%d], dataID[%d], dataIdx[%d], fileIdx[%d]\n",
        arcID, loopcode_dat[fileIdx], dataIdx, fileIdx );
      break;
    default:
      GFL_ASSERT(0);
  }
}

//----------------------------------------------------------------------------
/**
 * @brief   ファイルの読み込み終了チェック
 *
 * @param   dataIdx データインデックス
 * @param   fileIdx ファイルインデックス
 *
 * @return  "true = 読み込み終了"
 * @return  "false = それ以外"
 */
//----------------------------------------------------------------------------
bool KeyMap::IsLoadData( u32 dataIdx, u32 fileIdx )
{
  void ** ppBuf = NULL;

  switch( dataIdx )
  {
    case 0: ppBuf = &dat.key[fileIdx];    break;
    case 1: ppBuf = &changeData[fileIdx]; break;
    case 2: ppBuf = &loopData;            break;
    default: GFL_ASSERT(0);
  }

  bool res = app::util::FileAccessor::IsFileLoad( ppBuf );

  if( res ) { FUKUSHIMA_PRINT( "IsFileLoad : pBuf[%p], dataIdx[%d], fileIdx[%d]\n", ppBuf, dataIdx, fileIdx ); }
  else      { FUKUSHIMA_PRINT( "IsFileLoad : FALSE\n" ); }
  
  return res;
}

//----------------------------------------------------------------------------
/**
 * @brief   全てのファイルを読み込み終えたか
 *
 * @param   dataIdx データインデックス
 * @param   fileIdx ファイルインデックス
 *
 * @return  "true = 全て完了"
 * @return  "false = それ以外"
 */
//----------------------------------------------------------------------------
bool KeyMap::IsDataLoadFinished( u32* dataIdx, u32* fileIdx )
{
  u32 data = *dataIdx;
  u32 file = *fileIdx;

  // 読み込み完了
  if( data == 2 && file == 0 ) return true;

  u32 fileCount = 0;

  switch( data )
  {
    case 0:
      fileCount = GFL_NELEMS(keymap_dat);
      break;
    case 1:
      fileCount = GFL_NELEMS(change_dat);
      break;
    case 2:
      fileCount = GFL_NELEMS(loopcode_dat);
      break;
    default:
      GFL_ASSERT(0);
  }

  // 次のデータ種の読み込みへ
  if( file == (fileCount - 1) )
  {
    *dataIdx += 1;
    *fileIdx = 0;
    //FUKUSHIMA_PRINT( "DATA[%d] : Index[0] != IndexMax[%d]\n", data + 1, fileCount );
  }
  else {
    *fileIdx += 1;
    //FUKUSHIMA_PRINT( "DATA[%d] : Index[%d] != IndexMax[%d]\n", data, file + 1, fileCount );
  }

  return false;
}


GFL_NAMESPACE_END( StrInput )
GFL_NAMESPACE_END( App )
