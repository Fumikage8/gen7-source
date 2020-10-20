//======================================================================
/**
 * @file FieldMoveModelAreaCharaAccessor.cpp
 * @date 2015/09/04 23:06:52
 * @author takahashi_tomoya
 * @brief 動作モデル　エリアリソースパックアクセサー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

#include "Field/FieldStatic/include/MoveModel/FieldMoveModelAreaCharaAccessor.h"


GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(MoveModel)

/**
 *  @brief コンストラクタ
 */
FieldMoveModelAreaCharaAccessor::FieldMoveModelAreaCharaAccessor() : 
  m_pAccessor(NULL)
{
}


/**
 *  @brief デストラクタ
 */
FieldMoveModelAreaCharaAccessor::~FieldMoveModelAreaCharaAccessor()
{
}

/**
 *  @brief 初期化
 */
void FieldMoveModelAreaCharaAccessor::Initialize( gfl2::fs::BinLinkerAccessor* pAccessor )
{
  m_pAccessor = pAccessor;
}

/**
 *  @brief 破棄
 */
void FieldMoveModelAreaCharaAccessor::Terminate()
{
  m_pAccessor = NULL;
}

/**
 *  @brief データ数の取得
 */
s32 FieldMoveModelAreaCharaAccessor::GetDataNum() const
{
  if( m_pAccessor )
  {
    return m_pAccessor->GetDataMax();
  }
  return 0;
}

/**
 *  @brief キャラクターIDの取得
 */
u32 FieldMoveModelAreaCharaAccessor::GetCharacterID(u32 index) const
{
  if( m_pAccessor )
  {
    if( index < m_pAccessor->GetDataMax() )
    {
      gfl2::fs::BinLinkerAccessor accessor( m_pAccessor->GetData(index) );
      u32 size = accessor.GetDataSize(0);
      u32 number = 0;
      u8 * data = reinterpret_cast<u8*>(accessor.GetData(0));
      for( u32 i=0; i<size; ++i )
      {
        // 終了条件
        if(!data[i])
        {
          break;
        }

        // 桁アップ
        if( i>=1 )
        {
          number *= 10;
        }

        // 加算
        number += data[i] - '0';
      }
      TOMOYA_PRINT( "%s == %d\n", data, number );
      return number;
    }
  }

  GFL_ASSERT(0);
  return 0;
}

/**
 *  @brief　キャラクターリソースの取得
 */
void * FieldMoveModelAreaCharaAccessor::GetCharacterResource(u32 index) const
{
  if( m_pAccessor )
  {
    if( index < m_pAccessor->GetDataMax() )
    {
      gfl2::fs::BinLinkerAccessor accessor( m_pAccessor->GetData(index) );
      return accessor.GetData(1);
    }
  }

  GFL_ASSERT(0);
  return NULL;
}




GFL_NAMESPACE_END(MoveModel)
GFL_NAMESPACE_END(Field)
