//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldEnvSoundCollisionAccessor.cpp
 *	@brief  環境音コリジョンデータアクセサー
 *	@author	tomoya takahashi
 *	@date		2015.06.29
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "Field/FieldRo/include/PlacementData/accessor/FieldEnvSoundCollisionAccessor.h"


GFL_NAMESPACE_BEGIN(Field);


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					実装
*/
//-----------------------------------------------------------------------------
/**
 *  @biref  コンストラクタ
 */
FieldEnvSoundCollisionAccessor::FieldEnvSoundCollisionAccessor() : 
  m_pData(NULL)
{
}

/**
 *  @brief  デストラクタ
 */
FieldEnvSoundCollisionAccessor::~FieldEnvSoundCollisionAccessor()
{
}

/**
 *  @brief  初期化
 */
void FieldEnvSoundCollisionAccessor::Initialize( const void * data )
{
  m_pData = reinterpret_cast<const CollisionDataHeader*>(data);
}

/**
 *  @brief  最大数
 */
u32 FieldEnvSoundCollisionAccessor::GetDataMax() const
{
  if( !m_pData ){ return 0; }
  switch( m_pData->type )
  {
  case CYLINDER:     /// シリンダー
    return 1;

  case SPHERE:     /// スフィア
    return 1;

  case SNAKE:        /// スネーク
    return (m_pData->snake.vertexnum - 1);

  case PIPE:        /// パイプ
    return (m_pData->pipe.vertexnum - 1);
  }


  GFL_ASSERT(0);
  return 0;
}

/**
 *  @brief 情報取得
 */
bool FieldEnvSoundCollisionAccessor::GetData( u32 idx, FieldEnvSoundCollisionAccessor::CollisionData* p_out ) const
{
  bool retval = false;

  // データ最大数チェック
  u32 datamax = GetDataMax();
  if( idx >= datamax )
  {
    return retval;
  }

  // データ抽出
  switch( m_pData->type ){
  case CYLINDER:
    {
      p_out->type      = CYLINDER;
      p_out->cylinder  = m_pData->cylinder;
      retval = true;
    }
    break;

  case SPHERE:
    {
      p_out->type      = SPHERE;
      p_out->sphere    = m_pData->sphere;
      retval = true;
    }
    break;


  case SNAKE:
    {
      p_out->type  = LINE;
      p_out->line.height = m_pData->snake.height;
      p_out->line.width  = m_pData->snake.width;
      p_out->line.startpos[0] = m_pData->snake.vertexList[ (idx * 3) + 0 ];
      p_out->line.startpos[1] = m_pData->snake.vertexList[ (idx * 3) + 1 ];
      p_out->line.startpos[2] = m_pData->snake.vertexList[ (idx * 3) + 2 ];

      p_out->line.endpos[0] = m_pData->snake.vertexList[ ((idx+1) * 3) + 0 ];
      p_out->line.endpos[1] = m_pData->snake.vertexList[ ((idx+1) * 3) + 1 ];
      p_out->line.endpos[2] = m_pData->snake.vertexList[ ((idx+1) * 3) + 2 ];
      retval = true;
    }
    break;

  case PIPE:
    {
      p_out->type  = PIPE_LINE;
      p_out->pipe_line.width  = m_pData->pipe.width;
      p_out->pipe_line.startpos[0] = m_pData->pipe.vertexList[ (idx * 3) + 0 ];
      p_out->pipe_line.startpos[1] = m_pData->pipe.vertexList[ (idx * 3) + 1 ];
      p_out->pipe_line.startpos[2] = m_pData->pipe.vertexList[ (idx * 3) + 2 ];

      p_out->pipe_line.endpos[0] = m_pData->pipe.vertexList[ ((idx+1) * 3) + 0 ];
      p_out->pipe_line.endpos[1] = m_pData->pipe.vertexList[ ((idx+1) * 3) + 1 ];
      p_out->pipe_line.endpos[2] = m_pData->pipe.vertexList[ ((idx+1) * 3) + 2 ];
      retval = true;
    }
    break;

  default:
    GFL_ASSERT_MSG(0, "type not found %d", m_pData->type);
    retval = false;
    break;
  }

  return retval;
}

//******************************************************************************************************
// private
//******************************************************************************************************


GFL_NAMESPACE_END(Field);

