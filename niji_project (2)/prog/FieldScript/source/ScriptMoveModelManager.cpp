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

#include "FieldScript/include/ScriptMoveModelManager.h"
#include "FieldScript/include/ScriptWork.h"
#include "Field/FieldRo/include/Fieldmap.h"

#include "Field/FieldRo/include/Effect/content/FieldEffectBallonIcon.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectTrainerEncount.h"
#include "niji_conv_header/field/FieldEffectTypes.h"


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( FieldScript );

//------------------------------------------------------------------------------
/**
 * @func        ScriptMoveModelManager
 * @brief       ScriptMoveModelManager関数.
 * @author      N.Takeda
 * @date        2015/12/09(水) 18:02:18
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
ScriptMoveModelManager::ScriptMoveModelManager( GameSys::GameManager *gman )
{
  m_pMoveModelManager = gman->GetGameData()->GetFieldCharaModelManager();

  for( s32 ii=0 ; ii<MODEL_RESOURCE_MAX ; ii++ )  m_iChrId[ ii ] = MODEL_RESOURCE_ID_NONE;
}

//------------------------------------------------------------------------------
/**
 * @func        LoadResourceReq
 * @brief       LoadResourceReq関数.
 * @author      N.Takeda
 * @date        2015/12/09(水) 18:02:18
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
bool ScriptMoveModelManager::LoadResourceReq( const s32 chrID )
{
  for( s32 ii=0 ; ii<MODEL_RESOURCE_MAX ; ii++ ){
    if( m_iChrId[ ii ] == chrID){
      GFL_ASSERT_MSG(0,"chrId=%d 二重登録\n");
      return false; ///< 既に登録されている.
    }
  }
  for( s32 ii=0 ; ii<MODEL_RESOURCE_MAX ; ii++ )
  {
    if( m_iChrId[ ii ] != MODEL_RESOURCE_ID_NONE){
      continue;
    }
    if( m_pMoveModelManager->IsLoaded( chrID ) ){
      GFL_ASSERT_MSG(0,"chrId=%d 既にエリアにいる\n");
      return false; //登録済み 
    }
    m_pMoveModelManager->LoadDynamicAsync(MoveModel::FieldMoveModelManager::DYNAMIC_RES_LOAD_INDEX_EVENT, chrID );
    m_iChrId[ ii ] = chrID;
    return true;
  }
  GFL_ASSERT_MSG(0,"chrId=%d 登録数オーバー\n");
  return false;                               ///< いっぱいだ.
}

//------------------------------------------------------------------------------
/**
 * @func        ResourceIsLoaded
 * @brief       ResourceIsLoaded関数.
 * @author      N.Takeda
 * @date        2015/12/09(水) 18:02:18
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
bool ScriptMoveModelManager::ResourceIsLoaded( const s32 chrID )
{
  for( s32 ii=0 ; ii<MODEL_RESOURCE_MAX ; ii++ )
  {
    if( chrID != MODEL_RESOURCE_ID_NONE &&
        m_iChrId[ ii ] != chrID ){
      continue;
    }
    if( m_pMoveModelManager->IsLoaded( chrID ) == false ){
      return false; //ロード中 
    }
  }
  return true;  //読み込み終了
}

//------------------------------------------------------------------------------
/**
 * @func        UnloadResource 
 * @brief       UnloadResource関数.
 * @author      N.Takeda
 * @date        2015/12/09(水) 18:02:18
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
bool ScriptMoveModelManager::UnloadResource( const s32 chrID )
{
  for( s32 ii=0 ; ii<MODEL_RESOURCE_MAX ; ii++ )
  {
    if( m_iChrId[ ii ] == chrID){
      m_pMoveModelManager->Unload( chrID );
      m_iChrId[ ii ] = MODEL_RESOURCE_ID_NONE;
      return true;
    }
  }
  GFL_ASSERT_MSG(0,"未登録のCharaリソース%d\n", chrID );
  return false;
}

//------------------------------------------------------------------------------
/**
 * @func        UnloadResourceForce
 * @brief       UnloadResourceForce関数.
 * @author      N.Takeda
 * @date        2015/12/09(水) 18:02:18
 *
 * @param[in,out] GameSys::GameManager* gman  .
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void ScriptMoveModelManager::UnloadResourceForce()
{
  for( s32 ii=0 ; ii<MODEL_RESOURCE_MAX ; ii++ )
  {
    if( m_iChrId[ ii ] != MODEL_RESOURCE_ID_NONE ){
      GFL_ASSERT_MSG(0,"MoveModelResoruceDelが呼ばれていない chr_id=%d\n",m_iChrId[ ii ]);
    
      m_pMoveModelManager->Unload( m_iChrId[ ii ] );
      m_iChrId[ ii ] = MODEL_RESOURCE_ID_NONE;
    }
  }
}


GFL_NAMESPACE_END( FieldScript );
GFL_NAMESPACE_END( Field );

