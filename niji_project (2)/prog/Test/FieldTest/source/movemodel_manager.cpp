//=================================================================================================
/**
 *  @file  movemodel_manager.cpp
 *  @brief
 *  @author  tamada@gamefreak.co.jp
 *  @date  2014.03.11
 */
//=================================================================================================
#include "../include/movemodel_manager.h"
#include "../include/movemodel.h"
#include "../include/movemodel_define.h"
#include "../include/movecode.h"
#include "../include/movecode_player.h"
#include "../include/movecode_npc.h"
#include "../include/movecode_attach.h"

namespace fld{ namespace mmodel {

  const MoveCodeBase * MoveModelManager::m_MoveCodeTable[ MOVE_CODE_NUM ] ={};

  //----------------------------------------------------------------------------
  /**
  *  @brief  MoveCodeの取得
  *    
  *  @param  code   取得したいコードの要素
  *  @return MoveCodeのインスタンス
  */
  //-----------------------------------------------------------------------------
  const MoveCodeBase * MoveModelManager::GetMoveCode( MOVE_CODE code )
  {
    const MoveCodeBase * move_code = m_MoveCodeTable[static_cast<u32>(code)];
    if ( move_code )
    {
      return move_code;
    }
    return m_MoveCodeTable[0];  //MOVE_CODE_NONE
  }
  
  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  MoveModelManager::MoveModelManager( gfl2::heap::HeapBase * p_heap )
  : m_pHeap(p_heap)
  {
    for (int i = 0; i < MOVE_CODE_NUM; ++ i )
    {
      m_MoveCodeTable[i] = NULL;
    }

    m_MoveCodeTable[MOVE_CODE_NONE]   = GFL_NEW( p_heap ) MoveCodeBase();
    m_MoveCodeTable[MOVE_CODE_PLAYER] = GFL_NEW( p_heap ) MoveCodePlayer();
    m_MoveCodeTable[MOVE_CODE_NPC]    = GFL_NEW( p_heap ) MoveCodeNpc();
    m_MoveCodeTable[MOVE_CODE_ATTACH_PLAYER]  = GFL_NEW( p_heap ) MoveCodeAttachPlayer(p_heap,L"R:/home/niji/exec/data_win32/data/test/field_test/playerAttachData.xml" );

    for (int i = 0; i < MMDL_MAX; ++ i )
    {
      m_MoveModel[i] = GFL_NEW( p_heap ) MoveModel( p_heap );
    }
  }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  MoveModelManager::~MoveModelManager()
  {
    DeleteAllModels();

    for (int i = 0; i < MOVE_CODE_NUM; ++ i )
    {
      GFL_SAFE_DELETE( m_MoveCodeTable[i] );
    }

    for (int i = 0; i < MMDL_MAX; ++ i )
    {
      GFL_SAFE_DELETE( m_MoveModel[i] );
    }
  }

  //----------------------------------------------------------------------------
  /**
  *  @brief  初期化
  *    
  *  @return trueは成功、falseは失敗
  */
  //-----------------------------------------------------------------------------
  bool MoveModelManager::Initialize(void)
  {
    for (int i = 0; i < MMDL_MAX; ++ i )
    {
      MoveModel * m_mmdl = m_MoveModel[i];
      m_mmdl->activate(this);
    }
    return true;
  }

  //----------------------------------------------------------------------------
  /**
  *  @brief  解放
  */
  //-----------------------------------------------------------------------------
  void MoveModelManager::Terminate( void )
  {
    DeleteAllModels();
  }

  //----------------------------------------------------------------------------
  /**
  *  @brief  管理モデルの全更新
  *    
  *  @param  frame    進める時間
  */
  //-----------------------------------------------------------------------------
  void MoveModelManager::UpdateAllModels( void )
  {
    for (int i = 0; i < MMDL_MAX; ++ i )
    {
      m_MoveModel[i]->update();
    }
  }

  //----------------------------------------------------------------------------
  /**
  *  @brief  モデルの削除
  *
  *  @param  削除したいモデル
  */
  //-----------------------------------------------------------------------------
  void MoveModelManager::DeleteAllModels( void )
  {
    for (int i = 0; i < MMDL_MAX; ++ i )
    {
      if ( m_MoveModel[i] )
      {
        DeleteMoveModel( m_MoveModel[i] );
      }
    }
  }

  //----------------------------------------------------------------------------
  /**
  *  @brief  モデルの取得
  *    
  *  @param  id  取得したいモデルのID
  *  @return    モデルのインスタンス
  */
  //-----------------------------------------------------------------------------
  MoveModel * MoveModelManager::GetMoveModel( MMDL_ID id ) const
  {
    for ( int i = 0; i < MMDL_MAX; ++ i )
    {
      MoveModel * p_mmdl = m_MoveModel[i];
      if ( p_mmdl->GetExists() == true && p_mmdl->GetHeader().mmdl_id == id )
      {
        return p_mmdl;
      }
    }
    return NULL;
  }

  //----------------------------------------------------------------------------
  /**
  *  @brief  モデルの取得
  *    
  *  @param  dataIndex   取得したいモデルの要素
  *  @return モデルのインスタンス
  */
  //-----------------------------------------------------------------------------
  MoveModel * MoveModelManager::GetMoveModelByDataIndex( u32 dataIndex ) const
  {
    return m_MoveModel[ dataIndex ];
  }

  //----------------------------------------------------------------------------
  /**
  *  @brief  モデルの作成
  *    
  *  @param  p_header    リクエスト
  *  @return モデルのインスタンス
  */
  //-----------------------------------------------------------------------------
  MoveModel * MoveModelManager::CreateMoveModel( const MoveModelHeader * p_header )
  {
    MoveModel * m_mmdl = NULL;
    for (int i = 0; i < MMDL_MAX; ++ i )
    {
      if ( m_MoveModel[i] != NULL && m_MoveModel[i]->GetExists() == false )
      {
        m_mmdl = m_MoveModel[i];
        break;
      }
    }

    if ( m_mmdl == NULL )
    {
      return NULL;
    }

    if ( m_mmdl->Setup( p_header ) == false )
    {
      return NULL;
    }
    
    return m_mmdl;
  }

  //----------------------------------------------------------------------------
  /**
  *  @brief  モデルの削除
  *
  *  @param  削除したいモデル
  */
  //-----------------------------------------------------------------------------
  void MoveModelManager::DeleteMoveModel( MoveModel * p_model )
  {
    for (int i = 0; i < MMDL_MAX; ++ i )
    {
      if ( m_MoveModel[i] == p_model )
      {
        m_MoveModel[i]->Clear();
        return;
      }
    }
  }

}; //end of namespace mmodel
}; //end of namespace fld
