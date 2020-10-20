//==============================================================================
/**
 * @file   RegulationExtSaveData.cpp
 * @date   2015/11/27 Fri. 11:49:47
 * @author muto_yu
 * @brief  sangoより移植及びnijiにあわせて修正
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "GameSys/include/GameManager.h"
#include  "Battle/Regulation/include/Regulation.h"
#include  "ExtSavedata/include/RegulationExtSaveData.h"

#include <System/include/SystemEventManager.h>

namespace ExtSavedata {
// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
RegulationExtSaveData::RegulationExtSaveData(gfl2::heap::HeapBase* pHeap)
  : m_pSaveData(NULL)
  , m_pLoadData(NULL)
  , m_result(gfl2::fs::ExtSaveFile::ACCESS_RESULT_OK)
  , m_pHeap(pHeap)
  , m_mode(SAVE_LOAD_MODE_none)
  , m_saveLoadIndex(0)
  , m_pSerializedRegulationData(NULL)
  , m_bIsFatalError(false)
{
  GFL_SINGLETON_INSTANCE(System::SystemEventManager)->RegistSystemEventListener(this);
  m_pSerializedRegulationData = GFL_NEW_ARRAY( pHeap ) u8[ Regulation::REGULATION_MAX_DATA_SIZE ];
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
RegulationExtSaveData::~RegulationExtSaveData()
{
  GFL_SAFE_DELETE_ARRAY( m_pSerializedRegulationData );
  GFL_SINGLETON_INSTANCE(System::SystemEventManager)->RemoveSystemEventListener(this);
}


bool RegulationExtSaveData::serializeRegulationDataToStaticTemp( const Regulation& rRegulation )
{
  GFL_ASSERT( rRegulation.GetSerializeDataSize() <= Regulation::REGULATION_MAX_DATA_SIZE );

  if( rRegulation.GetSerializeDataSize() <= Regulation::REGULATION_MAX_DATA_SIZE )
  {
    rRegulation.Serialize( m_pSerializedRegulationData );   /*  destination buffer のサイズチェックをしておらず危険  */
    return true;
  }
    
  return false;
}


//------------------------------------------------------------------
/**
  *  @brief   データロード（同期）
  */
//------------------------------------------------------------------
bool RegulationExtSaveData::LoadRegulationData(const REGULATION_TYPE type, Regulation& rRegulation)
{
  NijiExtSaveSystem*  pExtGameSystem  = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetNijiExtSaveSystem();
  bool                ret             = false;


  if( pExtGameSystem->IsFileExists( EXTID_REGULATION, type ) )
  {
    gfl2::fs::ExtSaveFile::Result result = pExtGameSystem->ReadFile(EXTID_REGULATION, (int)type, m_pSerializedRegulationData, Regulation::REGULATION_MAX_DATA_SIZE);

    GFL_PRINT("RegulationExtSaveData::LoadRegulationData : ReadFile result[%d]\n",result);
    switch( result )
    {
      case gfl2::fs::ExtSaveFile::ACCESS_RESULT_OK:                     //成功
        rRegulation.Deserialize(m_pSerializedRegulationData, Regulation::REGULATION_MAX_DATA_SIZE);
        ret = true;
        break;

      case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_PATH_NOT_EXIST:      //パスが存在しない＝指定したファイルが見つからない
        //帰ってこないはず
        GFL_ASSERT(0);
        break;

      case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_PATH_ALREADY_EXIST:  //すでにパスが存在する＝存在するファイル名でCreate/Renameの操作を行った
        //帰ってこないはず
        GFL_ASSERT(0);
        break;

      case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_BROKEN_DATA:         //ファイルが壊れている＝正常でないため、削除が必要です
        break;

      case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_LACK_OF_SPACE:       //空き容量が不足している＝ファイルを作成した時に発生
        //帰ってこないはず
        GFL_ASSERT(0);
        break;

      case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_ERROR:               //接触不良など＝ゲーム継続不能になるかと思います。
      case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_WRITE_PROTECTED:     //接触不良など＝ゲーム継続不能になるかと思います。
      case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_MEDIA_NOT_FOUND:     //接触不良など＝ゲーム継続不能になるかと思います。
      case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_MEDIA_ACCESS_ERROR:  //接触不良など＝ゲーム継続不能になるかと思います。
      case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_ARCHIVE_INVALIDATED: //接触不良など＝ゲーム継続不能になるかと思います。
      case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_UNKNOWN:             //接触不良など＝ゲーム継続不能になるかと思います。
      default:                                                          //Fatalエラー
        break;
    }

    if( !m_bIsFatalError )
    {
      m_bIsFatalError = NijiExtSaveSystem::IsFatalError( result );
    }
  }
  else
  {
    GFL_PRINT("RegulationExtSaveData::LoadRegulationData : type[%2d] file not exist\n", type);
  }

  return ret;
}


//------------------------------------------------------------------
/**
  *  @brief   データセーブ（同期）
  */
//------------------------------------------------------------------
bool RegulationExtSaveData::SaveRegulationData(const REGULATION_TYPE type, const Regulation& rRegulation)
{
  NijiExtSaveSystem*  pExtGameSystem  = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetNijiExtSaveSystem();
  bool                ret             = true;

  /*
    create file
  */
  if( !pExtGameSystem->IsFileExists( EXTID_REGULATION, type ) )
  {
    ret = false;

    gfl2::fs::ExtSaveFile::Result result = pExtGameSystem->CreateExtFile( EXTID_REGULATION, type, Regulation::REGULATION_MAX_DATA_SIZE );
    GFL_PRINT("ExtSaveData::SaveRegulationData : CreateFile result[%d]\n",result);
    switch( result )
    {
      case gfl2::fs::ExtSaveFile::ACCESS_RESULT_OK:                       //成功
        ret = true;
        break;

      case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_PATH_NOT_EXIST:        //パスが存在しない＝指定したファイルが見つからない
        //帰ってこないはず
        GFL_ASSERT(0);
        break;

      case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_PATH_ALREADY_EXIST:    //すでにパスが存在する＝存在するファイル名でCreate/Renameの操作を行った
        //帰ってこないはず
        GFL_ASSERT(0);
        break;

      case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_BROKEN_DATA:           //ファイルが壊れている＝正常でないため、削除が必要です
        //帰ってこないはず
        GFL_ASSERT(0);
        break;

      case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_LACK_OF_SPACE:         //空き容量が不足している＝ファイルを作成した時に発生
        break;

      case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_ERROR:                 //接触不良など＝ゲーム継続不能になるかと思います。
      case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_WRITE_PROTECTED:       //接触不良など＝ゲーム継続不能になるかと思います。
      case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_MEDIA_NOT_FOUND:       //接触不良など＝ゲーム継続不能になるかと思います。
      case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_MEDIA_ACCESS_ERROR:    //接触不良など＝ゲーム継続不能になるかと思います。
      case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_ARCHIVE_INVALIDATED:   //接触不良など＝ゲーム継続不能になるかと思います。
      case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_UNKNOWN:               //接触不良など＝ゲーム継続不能になるかと思います。
      default://Fatalエラー
        break;
    }

    if( !m_bIsFatalError )
    {
      m_bIsFatalError = NijiExtSaveSystem::IsFatalError( result );
    }
  }


  /*
    write file
  */
  if( ret )
  {
    ret = false;

    /*  serialize  */
    if(serializeRegulationDataToStaticTemp(rRegulation))
    {
      /*  write  */
      gfl2::fs::ExtSaveFile::Result   result  = pExtGameSystem->WriteFile( EXTID_REGULATION, type, m_pSerializedRegulationData, Regulation::REGULATION_MAX_DATA_SIZE );

      GFL_PRINT("RegulationExtSaveData::SaveRegulationData : WriteFile result[%d]\n", result);
      switch( result )
      {
        case gfl2::fs::ExtSaveFile::ACCESS_RESULT_OK:                       //成功
          ret = true;
          break;

        case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_PATH_NOT_EXIST:        //パスが存在しない＝指定したファイルが見つからない
          //帰ってこないはず
          GFL_ASSERT(0);
          break;

        case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_PATH_ALREADY_EXIST:    //すでにパスが存在する＝存在するファイル名でCreate/Renameの操作を行った
          //帰ってこないはず
          GFL_ASSERT(0);
          break;

        case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_BROKEN_DATA:           //ファイルが壊れている＝正常でないため、削除が必要です
          //帰ってこないはず
          GFL_ASSERT(0);
          break;

        case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_LACK_OF_SPACE:         //空き容量が不足している＝ファイルを作成した時に発生
          //帰ってこないはず
          GFL_ASSERT(0);
          break;

        case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_ERROR:                 //接触不良など＝ゲーム継続不能になるかと思います。
        case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_WRITE_PROTECTED:       //接触不良など＝ゲーム継続不能になるかと思います。
        case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_MEDIA_NOT_FOUND:       //接触不良など＝ゲーム継続不能になるかと思います。
        case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_MEDIA_ACCESS_ERROR:    //接触不良など＝ゲーム継続不能になるかと思います。
        case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_ARCHIVE_INVALIDATED:   //接触不良など＝ゲーム継続不能になるかと思います。
        case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_UNKNOWN:               //接触不良など＝ゲーム継続不能になるかと思います。
        default://Fatalエラー
            break;
      }

      if( !m_bIsFatalError )
      {
        m_bIsFatalError = NijiExtSaveSystem::IsFatalError( result );
      }
    }
  }

  return ret;
}



//------------------------------------------------------------------
/**
  *  @brief   ファイル存在確認（同期）
  */
//------------------------------------------------------------------
bool RegulationExtSaveData::IsExistsRegulationData(const REGULATION_TYPE type)
{
  return GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetNijiExtSaveSystem()->IsFileExists( EXTID_REGULATION, type );
}


//------------------------------------------------------------------
/**
  *  @brief   ファイル削除
  */
//------------------------------------------------------------------
bool RegulationExtSaveData::DeleteRegulationData(const REGULATION_TYPE type)
{
  NijiExtSaveSystem*  pExtGameSystem  = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetNijiExtSaveSystem();
  bool                ret             = false;

  gfl2::fs::ExtSaveFile::Result result = pExtGameSystem->DeleteExtFile(EXTID_REGULATION, type);
  GFL_PRINT("RegulationExtSaveData::DeleteRegulationData : DeleteFile result[%d]\n",result);
  switch( result )
  {
    case gfl2::fs::ExtSaveFile::ACCESS_RESULT_OK:                       //成功
      ret = true;
      break;

    case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_PATH_NOT_EXIST:        //パスが存在しない＝指定したファイルが見つからない
      //ret = true;//成功扱い
      break;

    case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_PATH_ALREADY_EXIST:    //すでにパスが存在する＝存在するファイル名でCreate/Renameの操作を行った
      //帰ってこないはず
      GFL_ASSERT(0);
      break;

    case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_BROKEN_DATA:           //ファイルが壊れている＝正常でないため、削除が必要です
      //帰ってこないはず
      GFL_ASSERT(0);
      break;
    case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_LACK_OF_SPACE:         //空き容量が不足している＝ファイルを作成した時に発生
      //帰ってこないはず
      GFL_ASSERT(0);
      break;

    case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_ERROR:                 //接触不良など＝ゲーム継続不能になるかと思います。
    case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_WRITE_PROTECTED:       //接触不良など＝ゲーム継続不能になるかと思います。
    case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_MEDIA_NOT_FOUND:       //接触不良など＝ゲーム継続不能になるかと思います。
    case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_MEDIA_ACCESS_ERROR:    //接触不良など＝ゲーム継続不能になるかと思います。
    case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_ARCHIVE_INVALIDATED:   //接触不良など＝ゲーム継続不能になるかと思います。
    case gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_UNKNOWN:               //接触不良など＝ゲーム継続不能になるかと思います。
    default://Fatalエラー
      break;
  }

  if( !m_bIsFatalError )
  {
    m_bIsFatalError = NijiExtSaveSystem::IsFatalError( result );
  }

  return ret;
}


//------------------------------------------------------------------
/**
  *  @brief    Fatalエラー発生したかどうか（true: 発生した 、false: 発生してない）
  */
//------------------------------------------------------------------
bool RegulationExtSaveData::IsFatalError(void)
{
  return m_bIsFatalError;
}


//------------------------------------------------------------------
/**
  *  @brief    データロード
  */
//------------------------------------------------------------------
bool RegulationExtSaveData::StartLoadRegulationData(const REGULATION_TYPE type , Regulation& rRegulation)
{
  NijiExtSaveSystem*  pExtGameSystem  = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetNijiExtSaveSystem();

  m_pLoadData     = &rRegulation;
  m_saveLoadIndex = type;

  if( pExtGameSystem->StartIsFileExists( m_pHeap , EXTID_REGULATION, m_saveLoadIndex ) )
  {
    m_mode = SAVE_LOAD_MODE_isExistsLoad;
    return true;
  }

  return false;
}


//------------------------------------------------------------------
/**
  *  @brief    データセーブ（渡すデータはセーブ処理が終わるまでDeleteしないでください）
  */
//------------------------------------------------------------------
bool RegulationExtSaveData::StartSaveRegulationData(const REGULATION_TYPE type , const Regulation& rRegulation)
{
  NijiExtSaveSystem*  pExtGameSystem  = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetNijiExtSaveSystem();

  m_pSaveData     = &rRegulation;
  m_saveLoadIndex = type;
  if( pExtGameSystem->StartIsFileExists( m_pHeap , EXTID_REGULATION, m_saveLoadIndex ) )
  {
    m_mode = SAVE_LOAD_MODE_isExistsSave;
    return true;
  }

  return false;
}


//------------------------------------------------------------------
/**
  *  @brief    データ作成（渡すデータはセーブ処理が終わるまでDeleteしないでください）
  */
//------------------------------------------------------------------
bool RegulationExtSaveData::StartCreateRegulationData(const REGULATION_TYPE type , const Regulation& rRegulation)
{
  NijiExtSaveSystem*  pExtGameSystem  = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetNijiExtSaveSystem();

  m_pSaveData     = &rRegulation;
  m_saveLoadIndex = type;

  if( pExtGameSystem->StartCreateFile( m_pHeap, EXTID_REGULATION, m_saveLoadIndex, Regulation::REGULATION_MAX_DATA_SIZE) )
  {
    m_mode = SAVE_LOAD_MODE_create;
    return true;
  }

  return false;
}


//------------------------------------------------------------------
/**
  *  @brief    データ削除
  */
//------------------------------------------------------------------
bool RegulationExtSaveData::StartDeleteRegulationData(const REGULATION_TYPE type)
{
  NijiExtSaveSystem*  pExtGameSystem  = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetNijiExtSaveSystem();

  m_saveLoadIndex = type;
  if( pExtGameSystem->StartDeleteFile( m_pHeap , EXTID_REGULATION, m_saveLoadIndex ) )
  {
    m_mode = SAVE_LOAD_MODE_waitDeleteFile;
    return true;
  }

  return false;
}

//------------------------------------------------------------------
/**
  *  @brief    更新　処理終了ならfalse、処理中ならtrue
  */
//------------------------------------------------------------------
bool RegulationExtSaveData::Update(void)
{
  NijiExtSaveSystem*  pExtGameSystem  = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetNijiExtSaveSystem();

  switch( m_mode )
  {
    case SAVE_LOAD_MODE_none:
      break;

    case SAVE_LOAD_MODE_isExistsSave:
    {
      if( pExtGameSystem->WaitIsFileExists( &m_result ) )
      {//終了
        if( m_result == gfl2::fs::ExtSaveFile::ACCESS_RESULT_OK )
        {//存在する
          if(serializeRegulationDataToStaticTemp(*m_pSaveData))
          {
            if( pExtGameSystem->StartWriteFile( m_pHeap , EXTID_REGULATION, m_saveLoadIndex , m_pSerializedRegulationData, Regulation::REGULATION_MAX_DATA_SIZE) )
            {
              m_mode = SAVE_LOAD_MODE_save;

              return true;
            }
          }
        }
        else if( m_result == gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_PATH_NOT_EXIST )
        {//新しく作る
          if( pExtGameSystem->StartCreateFile( m_pHeap, EXTID_REGULATION, m_saveLoadIndex, Regulation::REGULATION_MAX_DATA_SIZE) )
          {
            m_mode = SAVE_LOAD_MODE_create;

            return true;
          }
        }
      }
      else
      {//処理中
        return true;
      }
    }
      break;

    case SAVE_LOAD_MODE_isExistsLoad:
    {
      if( pExtGameSystem->WaitIsFileExists( &m_result ) )
      {//終了
        if( m_result == gfl2::fs::ExtSaveFile::ACCESS_RESULT_OK )
        {//存在する
          if( pExtGameSystem->StartReadFile( m_pHeap , EXTID_REGULATION, m_saveLoadIndex , m_pSerializedRegulationData , Regulation::REGULATION_MAX_DATA_SIZE) )
          {
            m_mode = SAVE_LOAD_MODE_load;

            return true;
          }
        }
      }
      else
      {//処理中
        return true;
      }
    }
      break;

    case SAVE_LOAD_MODE_create:
    {
      if( pExtGameSystem->WaitCreateFile( &m_result ) )
      {//終了
        if( m_result == gfl2::fs::ExtSaveFile::ACCESS_RESULT_OK )
        {//成功
          if(serializeRegulationDataToStaticTemp(*m_pSaveData))
          {
            if( pExtGameSystem->StartWriteFile( m_pHeap , EXTID_REGULATION, m_saveLoadIndex , m_pSerializedRegulationData, Regulation::REGULATION_MAX_DATA_SIZE) )
            {
              m_mode = SAVE_LOAD_MODE_save;

              return true;
            }
          }
        }
      }
      else
      {//処理中
        return true;
      }
    }
      break;

    case SAVE_LOAD_MODE_load:
    {
      if( pExtGameSystem->WaitReadFile( &m_result ) )
      {//終了
        if( m_result == gfl2::fs::ExtSaveFile::ACCESS_RESULT_OK )
        {//成功

          if( m_pLoadData )
          {
            m_pLoadData->Deserialize( m_pSerializedRegulationData, Regulation::REGULATION_MAX_DATA_SIZE );
          }

          m_mode = SAVE_LOAD_MODE_finish;
        }
      }
      else
      {//処理中
        return true;
      }
    }
    break;
    case SAVE_LOAD_MODE_save:
    {
      if( pExtGameSystem->WaitWriteFile( &m_result ) )
      {//終了
        if( m_result == gfl2::fs::ExtSaveFile::ACCESS_RESULT_OK )
        {//成功
          m_mode = SAVE_LOAD_MODE_finish;
        }
      }
      else
      {//処理中
        return true;
      }
    }
    break;
    case SAVE_LOAD_MODE_waitDeleteFile:
      {
        if( pExtGameSystem->WaitDeleteFile( &m_result ) )
        {//終了
          if( m_result == gfl2::fs::ExtSaveFile::ACCESS_RESULT_OK )
          {//成功
            m_mode = SAVE_LOAD_MODE_finish;
          }
        }
        else
        {//処理中
          return true;
        }
      }
    break;
    case SAVE_LOAD_MODE_finish:
    break;
    case SAVE_LOAD_MODE_error:
    break;
    default:
    break;
  };

  if(!( m_mode == SAVE_LOAD_MODE_finish || m_mode == SAVE_LOAD_MODE_none ))
  {
    if( !m_bIsFatalError )
    {
      m_bIsFatalError = NijiExtSaveSystem::IsFatalError( m_result );
    }
    m_mode = SAVE_LOAD_MODE_error;
  }

  return false;
}

bool RegulationExtSaveData::IsSuccess(void) const
{
#if PM_DEBUG
  if ( *( NijiExtSaveSystem::GetDebugExtSaveFatalErrorFlag() ) == true )
  {
    return false;
  }
#endif
  return (m_mode == SAVE_LOAD_MODE_finish);
}

//------------------------------------------------------------------
/**
  * @brief   ホーム、スリープ、ゲーム終了処理発生時イベント
  *
  * @retval  true    終了処理完了
  * @retval  false   終了処理中
  *
  * @note    適切な終了処理を記述してください。
  * @attention  On～という名前なのに他のメンバとふるまいが違うので注意
  * @attention  Observerパターンとして名前が適切なのかは疑問の余地あり
  */
//------------------------------------------------------------------
bool RegulationExtSaveData::OnSuspendWithHomeSleepAndPower( void )
{
  GFL_PRINT("RegulationExtSaveData::OnSuspendWithHomeSleepAndPower\n");
  return !Update();
}


} /*  namespace ExtSavedata */
