//==============================================================================
/**
 * @file        ScriptMoveModelManager.h
 * @brief       要約説明
  
    このファイルの詳細説明
 * 
 * @author      N.Takeda
 * @date        2015/12/09(水) 18:33:16
 */
//==============================================================================

#if !defined(__SCRIPTMOVEMODELMANAGER_H__) // 重複定義防止
#define __SCRIPTMOVEMODELMANAGER_H__ // 重複定義防止マクロ
#pragma once

#include <GameSys/include/GameManager.h>

namespace Field{
  namespace MoveModel{
    class FieldMoveModelManager;
  }
}

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( FieldScript );


//------------------------------------------------------------------------------
/**
 * @class       MoveModelManager
 * @brief       動作モデル管理クラス.
 * @author      N.Takeda
 * @date        2015/12/09(水) 18:33:16
 */
//------------------------------------------------------------------------------
class ScriptMoveModelManager
{
  enum{ 
    MODEL_RESOURCE_MAX = 1,
    MODEL_RESOURCE_ID_NONE = -1,
  };
  
  s32 m_iChrId[ MODEL_RESOURCE_MAX ];
  
public:
  /// ------- getters ---------
  MoveModel::FieldMoveModelManager* GetMoveModelManager() const { return m_pMoveModelManager; }
  
  ScriptMoveModelManager( GameSys::GameManager *gman );
  bool LoadResourceReq( const s32 chrID );
  bool ResourceIsLoaded( const s32 chrID );
  bool UnloadResource( const s32 chrID );
  void UnloadResourceForce();

private:
  MoveModel::FieldMoveModelManager* m_pMoveModelManager;
  
};

GFL_NAMESPACE_END( FieldScript );
GFL_NAMESPACE_END( Field );

#endif // __SCRIPTMOVEMODELMANAGER_H__ 重複定義防止

