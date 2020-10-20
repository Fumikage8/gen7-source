//======================================================================
/**
 * @file btl_InsertWazaInfoStorage.cpp
 * @date 2016/05/19 16:42:20
 * @author taya
 * @brief 割り込みで追加されたワザアクションを実行する前の情報表示用パラメータ管理
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <macro/include/gfl2_Macros.h>

#include "./btl_InsertWazaInfoStorage.h"

GFL_NAMESPACE_BEGIN(btl)

// ctor
InsertWazaInfoStorage::InsertWazaInfoStorage()
{
  Clear();
}
// dtor
InsertWazaInfoStorage::~InsertWazaInfoStorage()
{

}


/**
 * @brief  パラメータを１件、格納する
 * @param[in]   actionParam    情報表示コマンドの生成時に参照するパラメータ
 * @return   格納ID（格納失敗時には無効値 ID_NULL ）
 */
uint InsertWazaInfoStorage::AddParam( const BTL_HANDEX_PARAM_INSERT_WAZA_ACTION* actionParam )
{
  for( uint i=0; i<GFL_NELEMS(m_storage); ++i )
  {
    if( !m_storage[i].isEnable )
    {
      m_storage[i].actionParam = *actionParam;
      m_storage[i].isEnable = true;
      return ID_MIN + i;
    }
  }
  return ID_NULL;
}

/**
 * @brief  格納したパラメータへのポインタを取得する
 * @param[in]   ID    AddParam の戻り値
 * @return   格納されたパラメータが見つかれば、そのポインタ／なければNULL
 */
const BTL_HANDEX_PARAM_INSERT_WAZA_ACTION*  
  InsertWazaInfoStorage::GetParam( uint ID ) const
{
  if( !isValidID(ID) ){
    GFL_ASSERT_MSG(0, "Invalid ID:%d", ID);
    return NULL;
  }

  uint idx = idToIdx( ID );
  if( idx < GFL_NELEMS(m_storage) )
  {
    if( m_storage[idx].isEnable ){
      return &(m_storage[idx].actionParam);
    }
  }

  GFL_ASSERT_MSG(0, "Invalid ID:%d", ID);
  return NULL;
}

/**
 * @brief  格納したパラメータを削除
 * @param[in]   ID    AddParam の戻り値
 * @detail  STORAGE_SIZE分だけ格納するとそれ以上は格納できなくなる。そうなる前に適宜この関数を呼び出し情報を破棄する必要がある。
 */
void InsertWazaInfoStorage::RemoveParam( uint ID )
{
  if( !isValidID(ID) ){
    GFL_ASSERT_MSG(0, "Invalid ID:%d", ID);
    return;
  }

  uint idx = idToIdx( ID );
  if( idx < GFL_NELEMS(m_storage) )
  {
    m_storage[idx].isEnable = false;
  }
}

/**
 * @brief   パラメータ格納領域の情報をクリアし、再度、１から格納することが可能な状態にする。
 * @detail  STORAGE_SIZE分だけ格納するとそれ以上は格納できなくなる。
 *          毎ターン呼び出すことで問答無用に全内容がRemoveParamされる。
 */
void InsertWazaInfoStorage::Clear( void )
{
  for(uint ID=ID_MIN; ID<=ID_MAX; ++ID)
  {
    RemoveParam( ID );
  }
}


GFL_NAMESPACE_END(btl)
