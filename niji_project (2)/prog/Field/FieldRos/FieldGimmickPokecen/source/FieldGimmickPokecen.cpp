//==============================================================================
/**
 * @file	FieldGimmickPokecen.cpp
 * @brief	フィールドギミック：ポケモンセンター
 * @author	ohhira_syo
 * @date	2015/11/12 18:56:00
 */
// =============================================================================

/*
誕生日まわりの対応でだいぶ複雑になっているため、次回作で参考にする場合は参考にとどめて書き直す事を推奨

誕生日演出まわりについて
・概要
　・内部データは「誕生日フラグ」と「誕生日ワーク」の２つ
　　・「誕生日フラグ（SYS_FLAG_BIRTHDAY）」は時間によって自動でオンオフされる
　　・「誕生日ワーク（WK_SYS_POKECEN_BIRTH）」はマップチェンジのたびに更新される
　　・後述の仕様やコードとスクリプトの挙動を一致させるために基本的にはワークを参照する
　・演出は「誕生日演出本体」と「演出後」の２種類
　　・コメントでは「【A】＝演出後」「【B】＝誕生日演出本体」「【C】＝演出なし」となっている
　　・「誕生日演出本体【B】」ではクラッカーなどを鳴らしメッセージも表示する
　　・「演出後【A】」ではケーキなどが最初から表示され、いつものように普通に動ける
　　・ポケセン内で誕生日を迎えた後のアプリ復帰などのため「演出なし【C】」の分岐が存在する
・仕様
　・「ポケセンの入り口から普通に入った場合のみ誕生日演出を行う」
　　・ポケセン内でアプリ復帰時も演出はしない
　　・ポケセン内でフェス遷移（マップチェンジあり）後も演出はしない
　　・ポケセン内でセーブしてからロードしても演出はしない
　　・ポケセン紹介が始まる場合も演出はしない（紹介を優先）
　　・全滅でポケセンに入ってきた時も演出はしない
　　　・ただしすでに演出済みであればケーキの表示は行う
　・「ポケセン内で誕生日を迎えたり過ぎたりしても演出のオンオフは行わない」
　　・ポケセン内で「誕生日前→誕生日」となっても演出は行わない
　　　・その後のアプリ復帰など（上記の遷移）でも演出は行わない
　　・ポケセン内で「誕生日→誕生日後」となっても演出は維持を続ける
　　　・その後のアプリ復帰など（上記の遷移）でも演出を中断はしない
*/


#include <debug/include/gfl2_DebugPrint.h>

// niji
#include "GameSys/include/GameManager.h"

// gflib2
#include <debug/include/gfl2_DebugPrint.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <system/include/Date/gfl2_Date.h>  // 時間を使用するアニメーション制御に使用
#include <util/include/gfl2_std_string.h>
#include <math/include/gfl2_Float.h>

// Field
#include "Field/FieldRo/include/Fieldmap.h"
#include <niji_conv_header/field/zone/zone_id.h> // zoneID
#include "Field/FieldRo/include/MyRenderingPipeLine.h"

//Monsterball
#include "arc_def_index/arc_def.h"
#include "niji_conv_header/field/monsterball/monsterball_garc_ctr.h"  // garc直下のBinlinkerアクセス用
#include "niji_conv_header/field/monsterball/monsterball_ctr.h"       // exports以下の特定のデータアクセス用

//Pokecen
#include <niji_conv_header\field\static_model\static_model_id.h>
#include "FieldScript/include/FieldScriptSystem.h"

//誕生日
#include <niji_conv_header/field/pokecen/pokecen_garc_ctr.h>
#include <niji_conv_header/field/pokecen/pc_birth_ctr.h>
#include <niji_conv_header/field/pokecen/pc_birth_mot.h>
#include <niji_conv_header/field/script/inc/pokecen.inc>
#include "FieldScript/include/FieldScriptSystem.h"
#include "FieldScript/include/ScriptWork.h"   //for field::script::SystemWork
#include <niji_conv_header/sound/SoundDefine.h>
#include "Field/FieldStatic/include/Sound/FieldSound.h"
#include <Sound/include/Sound.h>
#include "Field/FieldStatic/include/Time/EvTime.h"


// mine
#include "Field/FieldRos/FieldGimmickPokecen/include/FieldGimmickPokecen.h"



GFL_NAMESPACE_BEGIN( Field )


//==Const==



//==Func==


void FieldGimmickPokecen::TextureResourceNodeGetter::Visit(gfl2::renderingengine::scenegraph::DagNode* dagNode)
{
  if(m_textureResourceNode == NULL)
  {
    if( dagNode->IsKindOf<gfl2::renderingengine::scenegraph::resource::TextureResourceNode>() )
    {
      m_textureResourceNode = dagNode->SafeCast<gfl2::renderingengine::scenegraph::resource::TextureResourceNode>();
    }
  }
}


gfl2::renderingengine::scenegraph::resource::TextureResourceNode* FieldGimmickPokecen::TextureResourceNodeGetter::GetTextureResourceNode(gfl2::renderingengine::scenegraph::DagNode* dagNode)
{
  gfl2::renderingengine::scenegraph::Traverser::Visitor<gfl2::renderingengine::scenegraph::DagNode, TextureResourceNodeGetter>  visitor;
  gfl2::renderingengine::scenegraph::Traverser::Traverse<gfl2::renderingengine::scenegraph::DagNode, TextureResourceNodeGetter>( dagNode, &visitor, 0xFFFFFFFF );
  return visitor.getTextureResourceNode();
}


FieldGimmickPokecen::TextureResourceNodeGetter::TextureResourceNodeGetter(void)
  : m_textureResourceNode(NULL)
  {
  }

FieldGimmickPokecen::TextureResourceNodeGetter::~TextureResourceNodeGetter()
{
}

gfl2::renderingengine::scenegraph::resource::TextureResourceNode* FieldGimmickPokecen::TextureResourceNodeGetter::getTextureResourceNode(void)
{
  return m_textureResourceNode;
}


// コンストラクタ
FieldGimmickPokecen::FieldGimmickPokecen( gfl2::heap::HeapBase* pHeap, Fieldmap *pMap ) :
  FieldGimmick(pMap)
  ,m_pFieldDeviceHeap(pHeap)
{
  m_pAllocator = GFL_NEW( pHeap ) System::nijiAllocator( pHeap );

  for(int i = 0 ;i < 6; ++i){
    //モンスターボール
    {
      m_pResource[i] = NULL;
      m_pModel[i] = NULL;
    }

    //キャプチャ画像
    {
      m_pCapResourceNode[i] = NULL;
      m_pCapturePoke[i] = NULL;
      m_pMaterialNodeRef[i] = NULL;
    }
  }

  //誕生日用
  m_bAppRecover = false;
  m_pResourceBuff_Birthday = NULL;
  for( u32 i=0; i < POKECEN_MDLID_NUM; ++i ){
    m_AnimReq[i] = 0;
    m_pModelResource_Birthday[i] = NULL;
    m_pModel_Birthday[i] = NULL;
  }
}
// デストラクタ
FieldGimmickPokecen::~FieldGimmickPokecen()
{
  GFL_DELETE m_pAllocator;
}

/**
 *  @brief  生成
 */
void FieldGimmickPokecen::Create( void )
{
  //Param
  {
    m_pFileReadManager = m_pFieldmap->GetGameManager()->GetAsyncFileManager();
  }

  //モンスターボール：リソースだけ事前準備
  {
    // ARCファイルオープン
    {
      gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
      {
        openReq.arcId = ARCID_MONSTERBALL;
        openReq.heapForFile = m_pFieldDeviceHeap;
        openReq.heapForArcSrc = m_pFieldDeviceHeap;
        openReq.heapForReq = m_pFieldDeviceHeap->GetLowerHandle();
      }

      m_pFileReadManager->AddArcFileOpenReq( openReq );
    }

    // ファイル読み込みリクエスト
    {
      gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;
      {
        loadReq.arcId = ARCID_MONSTERBALL;
        loadReq.datId = 0;
        loadReq.ppBuf = &m_pResourceBuff;
        loadReq.heapForBuf = m_pFieldDeviceHeap;
        loadReq.heapForReq = m_pFieldDeviceHeap->GetLowerHandle();
        loadReq.heapForCompressed = m_pFieldDeviceHeap->GetLowerHandle();
        loadReq.pBufSize = NULL;
        loadReq.align = 128;
      }

      m_pFileReadManager->AddArcFileLoadDataReq( loadReq );
    }
  }

  //キャプチャ画像：アクセサまわりだけ事前準備
  {
    //m_ResourceManagerHeap
    {
      gfl2::heap::NwAllocator*      pNULLNwAllocator      = NULL;
      gfl2::util::GLHeapAllocator*  pNULLGLHeapAllocator  = NULL;
      m_ResourceManagerHeap.Create(
        m_pFieldDeviceHeap,     //  gfl2::heap::HeapBase        * heap,
        m_pFieldDeviceHeap,     //  gfl2::heap::HeapBase        * devHeap,
        pNULLNwAllocator,       //  gfl2::heap::NwAllocator     * heapAllocator,
        pNULLNwAllocator,       //  gfl2::heap::NwAllocator     * devHeapAllocator,
        pNULLGLHeapAllocator,   //  gfl2::util::GLHeapAllocator * glHeapAllocator,
        pNULLGLHeapAllocator    //  gfl2::util::GLHeapAllocator * devGLHeapAllocator
      );

      m_ResourceManagerHeap.SetFileReadHeap(m_pFieldDeviceHeap);
    }

    //m_pPokeCapAccesser
    {
  		m_pPokeCapAccesser = GFL_NEW(m_pFieldDeviceHeap) AppLib::Tool::PokeCapAccesser(&m_ResourceManagerHeap, m_pFileReadManager);
      m_pPokeCapAccesser->OpenPokemonCapture();
    }

    {
      for(int i = 0 ;i < 6; ++i){
        m_pCapturePoke[i] = NULL;
        m_pCapResourceNode[i] = NULL;
      }
    }
  }

  //誕生日演出：リソースだけ事前準備
  {
    // ARCファイルオープン
    {
      gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
      {
        openReq.arcId = ARCID_POKECEN;
        openReq.heapForFile = m_pFieldDeviceHeap;
        openReq.heapForArcSrc = m_pFieldDeviceHeap;
        openReq.heapForReq = m_pFieldDeviceHeap->GetLowerHandle();
      }

      m_pFileReadManager->AddArcFileOpenReq( openReq );
    }

    // ファイル読み込みリクエスト
    {
      gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;
      {
        loadReq.arcId = ARCID_POKECEN;
        loadReq.datId = 0;
        loadReq.ppBuf = &m_pResourceBuff_Birthday;
        loadReq.heapForBuf = m_pFieldDeviceHeap;
        loadReq.heapForReq = m_pFieldDeviceHeap->GetLowerHandle();
        loadReq.heapForCompressed = m_pFieldDeviceHeap->GetLowerHandle();
        loadReq.pBufSize = NULL;
        loadReq.align = 128;
      }

      m_pFileReadManager->AddArcFileLoadDataReq( loadReq );
    }
  }

  //AnimPack
  for(int i = 0; i < POKECEN_MDLID_NUM; ++i){
    m_AnimPackList[i].Initialize( m_pFieldDeviceHeap, 1 );
  }
}
void FieldGimmickPokecen::UpdateCreate( void )
{
}
bool FieldGimmickPokecen::IsCreated( void )
{
  //モンスターボール
  {
    // 読み込み完了待ち
    if( !m_pFileReadManager->IsArcFileOpenFinished( ARCID_MONSTERBALL ) ){
      return false;
    }
    if( !m_pFileReadManager->IsArcFileLoadDataFinished( &m_pResourceBuff ) ){
      return false;
    }
  }

  //誕生日演出
  {
    // 読み込み完了待ち
    if( !m_pFileReadManager->IsArcFileOpenFinished( ARCID_POKECEN ) ){
      return false;
    }
    if( !m_pFileReadManager->IsArcFileLoadDataFinished( &m_pResourceBuff_Birthday ) ){
      return false;
    }
  }

  //誕生日演出：アプリからの復帰かのチェック
  {
    m_bAppRecover = (m_pFieldmap->GetOpenMode() == FM_OPEN_MODE_APP_RECOVER);
  }

  return true;
}

/**
 *  @biref  破棄更新
 *  @return 破棄が完了したかどうか
 *
 *  破棄が完了するまでマイフレーム呼出がなされる関数
 */
bool FieldGimmickPokecen::Finalize( void )
{
  for(int i = 0; i < POKECEN_MDLID_NUM; ++i){
    //削除は描画解除の１フレーム後
    if(m_pModel_Birthday[i] != NULL && !m_pModel_Birthday[i]->CanDestroy()){
      return false;
    }
    GFL_SAFE_DELETE( m_pModel_Birthday[i] );
  }

  //FileCloseを待機する都合で、こちらで削除を行う
  //モンスターボール
  if(m_pResourceBuff)
  {
    for(int i = 0; i < 6; ++i){
      if(m_pResource[i]){
        GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetFieldRootNode()->RemoveResourceChild( m_pResource[i] );
      }
      GFL_SAFE_DELETE( m_pResource[i] );
    }

    //読み込んだファイルの削除
    if(m_pResourceBuff){
      GflHeapFreeMemory(m_pResourceBuff);
      m_pResourceBuff = NULL;
    }

    //ARCファイルクローズ
    {
      gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
      closeReq.arcId = ARCID_MONSTERBALL;
      closeReq.heapForReq = m_pFieldDeviceHeap->GetLowerHandle();
      m_pFileReadManager->SyncArcFileClose( closeReq );
    }
  }
  // 削除完了待ち
  if( !m_pFileReadManager->IsArcFileCloseFinished( ARCID_MONSTERBALL ) ){
    return false;
  }

  //キャプチャ
  if(m_pPokeCapAccesser)
  {
    {
     m_pPokeCapAccesser->ClosePokemonCapture();
     GFL_SAFE_DELETE(m_pPokeCapAccesser);
    }

    {
      m_BinLinker.Initialize(NULL);
    }

    {
      for(int i = 0 ;i < 6; ++i){
        GFL_SAFE_DELETE(m_pCapturePoke[i]);
        GFL_SAFE_DELETE(m_pCapResourceNode[i]);
      }
    }

    m_ResourceManagerHeap.Delete();
  }

  //誕生日演出
  if(m_pResourceBuff_Birthday)
  {
    for(int i = 0; i < POKECEN_MDLID_NUM; ++i){
      if(m_pModelResource_Birthday[i]){
        GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetFieldRootNode()->RemoveResourceChild( m_pModelResource_Birthday[i] );
      }
      GFL_SAFE_DELETE( m_pModelResource_Birthday[i] );

      //削除は描画解除の１フレーム後
      if(m_pModel_Birthday[i] != NULL){
        GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetUpperRenderingPipeLine()->RemoveEdgeRenderingTarget( m_pModel_Birthday[i]->GetModelInstanceNode() );
      }
      //GFL_SAFE_DELETE( m_pModel_Birthday[i] );
    }

    //読み込んだファイルの削除
    if(m_pResourceBuff_Birthday){
      GflHeapFreeMemory(m_pResourceBuff_Birthday);
      m_pResourceBuff_Birthday = NULL;
    }

    //ARCファイルクローズ
    {
      gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
      closeReq.arcId = ARCID_POKECEN;
      closeReq.heapForReq = m_pFieldDeviceHeap->GetLowerHandle();
      m_pFileReadManager->SyncArcFileClose( closeReq );
    }

    //AnimPack
    for(int i = 0; i < POKECEN_MDLID_NUM; ++i){
      m_AnimPackList[i].Finalize();
    }
  }
  // 削除完了待ち
  if( !m_pFileReadManager->IsArcFileCloseFinished( ARCID_POKECEN ) ){
    return false;
  }

  return true;
}


/**
 *  @brief  破棄
 */
void FieldGimmickPokecen::Delete( void )
{
}

/**
 *  @brief  Ready中1度だけ呼び出し
 */
void FieldGimmickPokecen::Ready( void )
{
  //モンスターボール
  {
    // リンカーに設定
    {
      m_BinLinker.Initialize( m_pResourceBuff );
      m_BinLinker.Initialize( m_BinLinker.GetData( BL_IDX_MONSTERBALL_GARC_CTR_MONSTERBALL_CTR_PACK ) );
    }
  }
}

/**
  *  @brief  他のReadyが終わった後に1度だけ呼び出し
  */
void FieldGimmickPokecen::ReadyAfter( void )
{
  //誕生日用処理
  {
    Ready_Birthday();
  }
}

/**
 *  @brief  誕生日用事前処理
 */
void FieldGimmickPokecen::Ready_Birthday( void )
{
  //Alias
  GameSys::GameManager*	          pGameManager = m_pFieldmap->GetGameManager();
  GameSys::GameData*		          pGameData = pGameManager->GetGameData();
  FieldScript::FieldScriptSystem*	pScript = pGameManager->GetFieldScriptSystem();

  //誕生日用シーケンスを行うかのチェック
  {
    //そもそも誕生日でなければ何もしない
//    if(!pGameData->GetEventWork()->CheckEventFlag( SYS_FLAG_BIRTHDAY ))
    //- 誕生日フラグは時間をまたぐと自動で変わるため、ポケセン内に居る限りは現状維持にするためWorkの方を参照する
    if(pGameData->GetEventWork()->GetEventWork( WK_SYS_POKECEN_BIRTH ) == 0)
    {
      return;
    }

    //他のスクリプトがリザーブされていればそちらを優先
    if(pScript->IsReserveScriptSetting()){
      //GFNMCat[1888]:誕生日イベント：誕生日イベント中に全滅復帰すると、ケーキが表示されない
      //- 特例として「すでに誕生日演出を行っており、最初からケーキを表示するだけの状態」であれば【A】の状態へ遷移する
      if(pScript->GetReservedScrID() == SCRID_POKECEN_RESERVE_GAMEOVER &&
         pGameData->GetEventWork()->CheckEventFlag( SYS_FLAG_BD_POKECEN_FIRST )
      ){
        //上記条件の場合はスルー
      }else{
        return;
      }
    }

  	//シーン発動があればそちらを優先
    if(pScript->IsEnableSceneScript()){
      return;
    }
  }

  //誕生日用シーケンス
  {
    //リンカー設定
    {
      m_BinLinker_Birthday.Initialize( m_pResourceBuff_Birthday );
      m_BinLinker_Birthday.Initialize( m_BinLinker_Birthday.GetData( BL_IDX_POKECEN_GARC_CTR_PC_BIRTH_CTR_PACK ) );
    }

    //リソース情報
    int BirthdayModelResourceIndexArr[] = {
      BL_IDX_PC_BIRTH_CTR_COM_BM_PCBIRTHLIGHT_CTR_GFBMDLP,
    	BL_IDX_PC_BIRTH_CTR_COM_BM_PCBIRTHCRACKER_CTR_GFBMDLP,
    	BL_IDX_PC_BIRTH_CTR_COM_BM_PCBIRTHCAKE_CTR_GFBMDLP,
    };
    int BirthdayMotionResourceIndexArr[] = {
    	BL_IDX_PC_BIRTH_CTR_PC_BIRTH_GFBMOTP,
    	BL_IDX_PC_BIRTH_CTR_PC_BIRTH_GFBMOTP,
    	BL_IDX_PC_BIRTH_CTR_PC_BIRTH_GFBMOTP,
    };
    gfl2::math::Vector3 BirthdayModelPos[] = {
      gfl2::math::Vector3(0, 0, 0),
      gfl2::math::Vector3(0, 0, 0),
      gfl2::math::Vector3(2100, 0, 2100),
    };

  	//各種フラグの組み合わせにより、以下のように対応する
    //SYS_FLAG_BD_POKECEN_FIRST=false,  m_bAppRecover=false  : 初回        : 【B】誕生日イベント
    //SYS_FLAG_BD_POKECEN_FIRST=true,   m_bAppRecover=true   : アプリ復帰  : 【A】いきなりケーキを出す
    //SYS_FLAG_BD_POKECEN_FIRST=true,   m_bAppRecover=false  : 再入場      : 【A】いきなりケーキを出す
    //SYS_FLAG_BD_POKECEN_FIRST=false,  m_bAppRecover=true   : ポケセン内誕生日を迎えてアプリ復帰  : 【C】なにもしない

    //【A】いきなりケーキを出す
    //- ２回目以降なら簡易対応で終了
    if(pGameData->GetEventWork()->CheckEventFlag( SYS_FLAG_BD_POKECEN_FIRST )){
      //ケーキだけ生成
      {
        int i = POKECEN_MDLID_CAKE;

        // モデルリソース
        {
          gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
          resourceData.SetModelData( static_cast<c8*>( m_BinLinker_Birthday.GetData(BirthdayModelResourceIndexArr[i]) ) );
          m_pModelResource_Birthday[i] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pAllocator, &resourceData );
        }

        //モーションリソース
        {
          c8* pAnime = reinterpret_cast<c8*>(m_BinLinker_Birthday.GetData( BirthdayMotionResourceIndexArr[i] ));
          m_AnimPackList[i].LoadData( 0, m_pAllocator, m_pFieldDeviceHeap, pAnime );
        }

        // モデル生成
        {
          m_pModel_Birthday[i] = GFL_NEW( m_pFieldDeviceHeap ) poke_3d::model::BaseModel();
          m_pModel_Birthday[i]->Create( m_pAllocator, m_pFieldDeviceHeap, m_pModelResource_Birthday[i]);
          m_pModel_Birthday[i]->SetPosition(BirthdayModelPos[i]);

          //モーション設定
          m_pModel_Birthday[i]->SetAnimationPackList( &m_AnimPackList[i] );

          //リソース登録
          GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetFieldRootNode()->AddResourceChild( m_pModelResource_Birthday[i] );

          //描画登録
          GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetUpperRenderingPipeLine()->AddEdgeRenderingTarget( m_pModel_Birthday[i]->GetModelInstanceNode() );
        }
      }

      //Alias
      GameSys::GameManager*            pGameManager = m_pFieldmap->GetGameManager();

      //StaticModel
      StaticModel::StaticModelManager* pStaticModelManager = pGameManager->GetFieldmap()->GetStaticModelManager();
      StaticModel::StaticModel*        pStaticModel = pStaticModelManager->GetStaticModelIDStaticModel(STATIC_MODEL_ID_COM_BM_PCOUNTER01);
      GFL_ASSERT( pStaticModel );

      //ケーキアニメを最初からループ設定
      m_pModel_Birthday[POKECEN_MDLID_CAKE]->ChangeAnimation( COM_BM_PCBIRTHCAKE_LOOP_GFMOT );
      m_pModel_Birthday[POKECEN_MDLID_CAKE]->SetAnimationLoop(true);

      //通常表示をケーキ表示のために消しておく
      {
        //Param
        s32   AnimId  = 3;
        f32   speed = 1;
        u32   lerpTime = 0;
        u32   slotIndex = 0;
        bool  isLoop = false;

        //Alias
        GameSys::GameManager*            pGameManager = m_pFieldmap->GetGameManager();

        //StaticModel
        StaticModel::StaticModelManager* pStaticModelManager = pGameManager->GetFieldmap()->GetStaticModelManager();
        StaticModel::StaticModel*        pStaticModel = pStaticModelManager->GetStaticModelIDStaticModel(STATIC_MODEL_ID_COM_BM_PCOUNTER01);
        GFL_ASSERT( pStaticModel );
        
        StaticModel::StaticModel::Handle_StartEventAnimationEx handle( slotIndex, AnimId, speed, isLoop, lerpTime );
        if( pStaticModel )  pStaticModel->Set( &handle);
      }

      return;
    }

    //【B】誕生日イベント
  	//- 初回なら正式対応
    if(!m_bAppRecover){
      for(int i = 0; i < POKECEN_MDLID_NUM; ++i){
        // モデルリソース
        {
          gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
          resourceData.SetModelData( static_cast<c8*>( m_BinLinker_Birthday.GetData(BirthdayModelResourceIndexArr[i]) ) );
          m_pModelResource_Birthday[i] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pAllocator, &resourceData );
        }

        //モーションリソース
        {
          c8* pAnime = reinterpret_cast<c8*>(m_BinLinker_Birthday.GetData( BirthdayMotionResourceIndexArr[i] ));
          m_AnimPackList[i].LoadData( 0, m_pAllocator, m_pFieldDeviceHeap, pAnime );
        }

        // モデル生成
        {
          m_pModel_Birthday[i] = GFL_NEW( m_pFieldDeviceHeap ) poke_3d::model::BaseModel();
          m_pModel_Birthday[i]->Create( m_pAllocator, m_pFieldDeviceHeap, m_pModelResource_Birthday[i]);
          m_pModel_Birthday[i]->SetPosition(BirthdayModelPos[i]);

          //モーション設定
          m_pModel_Birthday[i]->SetAnimationPackList( &m_AnimPackList[i] );
          //m_pModel_Birthday[i]->ChangeAnimationByResourceNode( m_pMotionResource_Birthday[i] );

          //リソース登録
          GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetFieldRootNode()->AddResourceChild( m_pModelResource_Birthday[i] );

          //描画登録
          GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetUpperRenderingPipeLine()->AddEdgeRenderingTarget( m_pModel_Birthday[i]->GetModelInstanceNode() );
        }
      }

      //ライト用モデルを０フレームで止めておくことで暗いままにしておく
      {
        m_pModel_Birthday[POKECEN_MDLID_LIGHT]->ChangeAnimation( COM_BM_PCBIRTHLIGHT_GFMOT );
        m_pModel_Birthday[POKECEN_MDLID_LIGHT]->SetAnimationStepFrame( 0.0f );
        m_pModel_Birthday[POKECEN_MDLID_CRACKER]->ChangeAnimation( COM_BM_PCBIRTHCRACKER_GFMOT );
        m_pModel_Birthday[POKECEN_MDLID_CRACKER]->SetAnimationStepFrame( 0.0f );
        m_pModel_Birthday[POKECEN_MDLID_CAKE]->ChangeAnimation( COM_BM_PCBIRTHCAKE_ON_GFMOT );
        m_pModel_Birthday[POKECEN_MDLID_CAKE]->SetAnimationStepFrame( 0.0f );
      }

      //通常表示をケーキ表示のために消しておく
      {
        //Param
        s32   AnimId  = 3;
        f32   speed = 1;
        u32   lerpTime = 0;
        u32   slotIndex = 0;
        bool  isLoop = false;

        //Alias
        GameSys::GameManager*            pGameManager = m_pFieldmap->GetGameManager();

        //StaticModel
        StaticModel::StaticModelManager* pStaticModelManager = pGameManager->GetFieldmap()->GetStaticModelManager();
        StaticModel::StaticModel*        pStaticModel = pStaticModelManager->GetStaticModelIDStaticModel(STATIC_MODEL_ID_COM_BM_PCOUNTER01);
        GFL_ASSERT( pStaticModel );
        
        StaticModel::StaticModel::Handle_StartEventAnimationEx handle( slotIndex, AnimId, speed, isLoop, lerpTime );
        if( pStaticModel )  pStaticModel->Set( &handle);
      }

      //BGM設定
      //- 最初は止めておく
      {
        Sound::EndBGMReq();
      }

/*
      // TVデータ作成 「きょうのハッピーバースデー」
      {
        u8 month;
        u8 day;
        gfl::cfg::Birthday birthday;

        gfl::cfg::Config config;
        config.Initialize();
        config.GetBirthday( &birthday );
        config.Finalize();

        month = birthday.month;
        day   = birthday.day;

        pml::PokeParty* p_party = pGameData->GetPlayerParty();
        u16 monsno;
        u32 pos = p_party->GetMarkingIndex();
        const pml::pokepara::PokemonParam * cp_pp = p_party->GetMemberPointerConst(pos);
        if (cp_pp)
        {
          monsno= cp_pp->GetMonsNo();
          netapp::util::SangoTVNaviProgramUtility::CreateTVProgramData_13(monsno, month, day);
        }
        else
        {
          GFL_ASSERT(0);
        }
      }
*/

      //誕生日用スクリプトを起動
      pScript->SetReserveScript( SCRID_POKECEN_BIRTHDAY );

      return;
    }

    //【C】なにもしない
    //- それ以外なら何もしない
  }
}
  
/**
 *  @brief  更新
 *  Fieldmap::Updateにて呼出
 *  キャラクタ前更新よりも手前で呼び出される
 */
void FieldGimmickPokecen::UpdateFirst( void )
{
  //誕生日用アニメーションの更新
  {
    for( u32 i=0; i < POKECEN_MDLID_NUM; ++i ){
      if(m_pModel_Birthday[i] == NULL){
        continue;
      }

      switch(m_AnimReq[i]){
      case 0://リクエスト待ち
        break;
      case 1://オン
      	{
      	  if(i == POKECEN_MDLID_CAKE){
            m_pModel_Birthday[i]->ChangeAnimation( COM_BM_PCBIRTHCAKE_ON_GFMOT );
            m_pModel_Birthday[i]->SetAnimationLoop(false);
      	  }

      	  m_pModel_Birthday[i]->SetAnimationStartFrame( 0 );
          m_pModel_Birthday[i]->SetAnimationStepFrame( 1.0f, 0 );
      	}
        ++m_AnimReq[i];
        break;
    case 2://終了待ち
        {
          if( m_pModel_Birthday[i]->IsAnimationLastFrame(0) ){//最初のスロットのが終わってれば全体も終わりとみなす
            switch(i){//ライトとクラッカーは終了したら外してしまう（変なエッジ表示が残ったりするしメモリ的にもムダ）
            case POKECEN_MDLID_LIGHT:
            case POKECEN_MDLID_CRACKER:
              //Resource
              {
                if(m_pModelResource_Birthday[i]){
                  GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetFieldRootNode()->RemoveResourceChild( m_pModelResource_Birthday[i] );
                }
//                GFL_SAFE_DELETE( m_pModelResource_Birthday[i] );
              }
              //Model
              {
                if(m_pModel_Birthday[i] != NULL){
                  GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetUpperRenderingPipeLine()->RemoveEdgeRenderingTarget( m_pModel_Birthday[i]->GetModelInstanceNode() );
                }
                //削除は描画の１フレーム後以降なので、ここでは削除保留
                //GFL_SAFE_DELETE( m_pModel_Birthday[i] );
              }
              m_AnimReq[i] = 5;//削除シーケンスに移行
              break;
            default:
              m_AnimReq[i] = 0;
              break;
            }
          }
        }
        break;
      case 3://オフ
      	{
      	  if(i == POKECEN_MDLID_CAKE){
            m_pModel_Birthday[i]->ChangeAnimation( COM_BM_PCBIRTHCAKE_OFF_GFMOT );
            m_pModel_Birthday[i]->SetAnimationLoop(false);
        	  m_pModel_Birthday[i]->SetAnimationStartFrame( 0 );
            m_pModel_Birthday[i]->SetAnimationStepFrame( 1.0f, 0 );
      	  }else{
            m_pModel_Birthday[i]->SetAnimationEndFrame( 0 );
            m_pModel_Birthday[i]->SetAnimationStepFrame( -1.0f, 0 );
      	  }
      	}
        ++m_AnimReq[i];
        break;
      case 4://終了待ち
        {
          if( m_pModel_Birthday[i]->IsAnimationLastFrame(0) ){//最初のスロットのが終わってれば全体も終わりとみなす
            m_AnimReq[i] = 0;
          }
        }
        break;
      case 5://削除
        {
          GFL_SAFE_DELETE( m_pModelResource_Birthday[i] );
          GFL_SAFE_DELETE( m_pModel_Birthday[i] );
          m_AnimReq[i] = 0;
        }
        continue;//以降はこのアニメのUpdateを行わない
      }

      //アニメ更新
      //- 削除中の可能性があるので、シーケンスによっては更新しない
      if( m_AnimReq[i] != 5 && m_pModel_Birthday[i]->HasAnimationController() ){
        m_pModel_Birthday[i]->UpdateAnimation();
      }
    }

    //ケーキ用
    //- Onからループに自動でつなげるための処理
    {
      int i = POKECEN_MDLID_CAKE;
      if(m_pModel_Birthday[i] != NULL){
        bool is_on = (m_pModel_Birthday[i]->GetAnimationId() == COM_BM_PCBIRTHCAKE_ON_GFMOT);
        bool is_end = static_cast<bool>(m_pModel_Birthday[i]->IsAnimationLastFrame());
        if(is_on && is_end){
          m_pModel_Birthday[i]->ChangeAnimation( COM_BM_PCBIRTHCAKE_LOOP_GFMOT );
          m_pModel_Birthday[i]->SetAnimationLoop(true);
        }
      }
    }
  }
}
/**
 *  @brief  更新
 *  Fieldmap::Updateにて呼出
 *  UpdateNormalが終了してから呼び出される
 */
void FieldGimmickPokecen::UpdateLast( void )
{
}
/**
 *  @brief  更新
 *  Fieldmap::UpdateNormalにて呼び出される
 */
void FieldGimmickPokecen::CalculateAfter( void )
{
}

/**
 *  @brief  特殊ステップチェック
 */
bool FieldGimmickPokecen::MoveRequestCheck( const EventCheckRequest *cpReq )
{
  return false;
}

/**
 *  @brief  イベント起動チェック
 */
bool FieldGimmickPokecen::EventRequestCheck( const EventCheckRequest *cpReq )
{
  return false;
}



//=スクリプト用インターフェース=

//#回復装置：基本

/**
 *  @brief 回復装置：開始
 */
void FieldGimmickPokecen::MachineInit()
{
  //Alias
  GameSys::GameManager*            pGameManager = m_pFieldmap->GetGameManager();

  //StaticModel
  StaticModel::StaticModelManager* pStaticModelManager = pGameManager->GetFieldmap()->GetStaticModelManager();
  StaticModel::StaticModel*        pStaticModel = pStaticModelManager->GetStaticModelIDStaticModel(STATIC_MODEL_ID_COM_BM_PCOUNTER01);

  //BaseModel
  poke_3d::model::BaseModel* pBaseModel = pStaticModel->GetBaseModel(pStaticModel->GetNowModelType());
  GFL_ASSERT(pBaseModel);

  //Party
  pml::PokeParty    *pParty  = pGameManager->GetGameData()->GetPlayerParty();
  int pokeTotalNum = pParty->GetMemberCount();
  int pokeNum = pParty->GetMemberCountEx(pml::PokeParty::POKE_COUNT_TYPE_NOT_EGG);

  //GFNMCat[1431]:ポケセンの回復処理でタマゴの中身が表示される
  //タマゴを除いた手持ちポケモンリスト
  pml::pokepara::PokemonParam* pPokeArr[6];
  {
    int poke_index = 0;
    for(int i = 0; i < pokeTotalNum; ++i){
      pml::pokepara::PokemonParam* pPoke = pParty->GetMemberPointer( i );
      if(pPoke->IsEgg(pml::pokepara::CHECK_BOTH_EGG)){
        continue;
      }
      pPokeArr[poke_index++] = pPoke;
    }
  }

  //モンスターボール
  {
    int BallResourceIndexArr[] = {
      //GFNMCat[1432]:ポケセンの回復処理で表示されるボールが実際のものと一致していない
      //- 0番はダミーらしいのでその部分を-1にしておく
      -1,//BALLID_NULL
    	BL_IDX_MONSTERBALL_CTR_MONSBALL_01_CTR_GFBMDLP,
    	BL_IDX_MONSTERBALL_CTR_MONSBALL_02_CTR_GFBMDLP,
    	BL_IDX_MONSTERBALL_CTR_MONSBALL_03_CTR_GFBMDLP,
    	BL_IDX_MONSTERBALL_CTR_MONSBALL_04_CTR_GFBMDLP,
    	BL_IDX_MONSTERBALL_CTR_MONSBALL_05_CTR_GFBMDLP,
    	BL_IDX_MONSTERBALL_CTR_MONSBALL_06_CTR_GFBMDLP,
    	BL_IDX_MONSTERBALL_CTR_MONSBALL_07_CTR_GFBMDLP,
    	BL_IDX_MONSTERBALL_CTR_MONSBALL_08_CTR_GFBMDLP,
    	BL_IDX_MONSTERBALL_CTR_MONSBALL_09_CTR_GFBMDLP,
    	BL_IDX_MONSTERBALL_CTR_MONSBALL_10_CTR_GFBMDLP,
    	BL_IDX_MONSTERBALL_CTR_MONSBALL_11_CTR_GFBMDLP,
    	BL_IDX_MONSTERBALL_CTR_MONSBALL_12_CTR_GFBMDLP,
    	BL_IDX_MONSTERBALL_CTR_MONSBALL_13_CTR_GFBMDLP,
    	BL_IDX_MONSTERBALL_CTR_MONSBALL_14_CTR_GFBMDLP,
    	BL_IDX_MONSTERBALL_CTR_MONSBALL_15_CTR_GFBMDLP,
    	BL_IDX_MONSTERBALL_CTR_MONSBALL_16_CTR_GFBMDLP,
    	BL_IDX_MONSTERBALL_CTR_MONSBALL_17_CTR_GFBMDLP,
    	BL_IDX_MONSTERBALL_CTR_MONSBALL_18_CTR_GFBMDLP,
    	BL_IDX_MONSTERBALL_CTR_MONSBALL_19_CTR_GFBMDLP,
    	BL_IDX_MONSTERBALL_CTR_MONSBALL_20_CTR_GFBMDLP,
    	BL_IDX_MONSTERBALL_CTR_MONSBALL_21_CTR_GFBMDLP,
    	BL_IDX_MONSTERBALL_CTR_MONSBALL_22_CTR_GFBMDLP,
    	BL_IDX_MONSTERBALL_CTR_MONSBALL_23_CTR_GFBMDLP,
    	BL_IDX_MONSTERBALL_CTR_MONSBALL_24_CTR_GFBMDLP,
    	BL_IDX_MONSTERBALL_CTR_MONSBALL_25_CTR_GFBMDLP,
    	BL_IDX_MONSTERBALL_CTR_MONSBALL_26_CTR_GFBMDLP,
    };

    const char* BallLocator[] = {
      "monsterball_loc_01",
      "monsterball_loc_02",
      "monsterball_loc_03",
      "monsterball_loc_04",
      "monsterball_loc_05",
      "monsterball_loc_06",
    };

    for(int i = 0; i < 6; ++i){
      if(pokeNum <= i){
        //空いてるとこは非表示にする

        continue;
      }

      s32 jointIndex = pBaseModel->GetJointIndex(BallLocator[i]);
//GFL_PRINT("JointIndex : %d", jointIndex);
//      GFL_ASSERT(0 <= jointIndex);

      gfl2::math::Matrix34 locatorMtx = pBaseModel->GetJointWorldMatrix(jointIndex);
      gfl2::math::Quaternion locatorRot; locatorRot.MatrixToQuaternion(locatorMtx);

      //ボールに対応したポケモン
      pml::pokepara::PokemonParam* pPoke = pPokeArr[i];

      int ballIndex = pPoke->GetGetBall();
//GFL_PRINT("BallIndex : %d", ballIndex);

      // モデルリソース
      {
        gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
        resourceData.SetModelData( static_cast<c8*>( m_BinLinker.GetData(BallResourceIndexArr[ballIndex]) ) );
        m_pResource[i] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pAllocator, &resourceData );
      }

      // モデル生成
      {
        m_pModel[i] = GFL_NEW( m_pFieldDeviceHeap ) poke_3d::model::BaseModel();
        m_pModel[i]->Create( m_pAllocator, m_pFieldDeviceHeap, m_pResource[i]);
        //m_pModel[i]->SetPosition(gfl2::math::Vector3(1961, 50, 2209));
        m_pModel[i]->SetPosition(locatorMtx.GetElemPosition());
        m_pModel[i]->SetRotationQuat(locatorRot);

        GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetFieldRootNode()->AddResourceChild( m_pResource[i] );
        GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetUpperRenderingPipeLine()->AddEdgeRenderingTarget( m_pModel[i]->GetModelInstanceNode() );

        //最初は隠しておく
        m_pModel[i]->GetModelInstanceNode()->SetVisible(false);
      }
    }
  }

  //キャプチャ画像
  {
    for(int i = 0; i < 6; ++i){
      if(pokeNum <= i){
        //空いてるとこは非表示にする
        break;
      }

      //キャプチャで表示するポケモン
      pml::pokepara::PokemonParam* pPoke = pPokeArr[i];

      //キャプチャデータの取得～ノード化
      {
        //pCapturePoke
        {
          int arci = m_pPokeCapAccesser->GetArcFileIndex(pPoke);
          m_pPokeCapAccesser->PokemonCaptureRead( arci , &m_pCapturePoke[i] );
        }

        //pCapResourceNode
        {
          gfl2::renderingengine::scenegraph::resource::GfBinaryTexData texData;
          {
            texData.SetTextureData(static_cast<c8*>(m_pCapturePoke[i]));
          }

          m_pCapResourceNode[i] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pAllocator, &texData);
        }
      }

      //実際の表示への反映
      {
        const char* MaterialNameArr[] = {
          "com_bm_pcounter_poke01",
          "com_bm_pcounter_poke02",
          "com_bm_pcounter_poke03",
          "com_bm_pcounter_poke04",
          "com_bm_pcounter_poke05",
          "com_bm_pcounter_poke06",
        };

        int material_index = pBaseModel->GetMaterialIndex(MaterialNameArr[i]);
        int texture_index = 0;
//GFL_PRINT("MaterialIndex : %d", material_index);

        gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pMaterialNode = pBaseModel->GetModelInstanceNode()->GetMaterialInstanceNode( material_index );

        gfl2::renderingengine::scenegraph::resource::TextureResourceNode* pTexNode = m_TexGetter.GetTextureResourceNode(m_pCapResourceNode[i]);
        pMaterialNode->SetTextureResourceNode( texture_index, pTexNode);

        //解除時に使うために記憶
        m_pMaterialNodeRef[i] = pMaterialNode;
      }
    }
  }
}

/**
 *  @brief 回復装置：ボールの表示のオンオフ
 */
void FieldGimmickPokecen::MachineBallSetVisible(int index, bool visible)
{
  if(m_pModel[index] != NULL){m_pModel[index]->GetModelInstanceNode()->SetVisible(visible);}
}

/**
 *  @brief 回復装置：終了
 */
void FieldGimmickPokecen::MachineFin()
{
  //画像系のデータは必ずHideして１フレーム以上経った後に削除する事
  //- GPUにデータとして転送されているものを削除すると問題が発生するため
  //- ポケセンではスクリプトで待機制御しているが、殿堂入りマシンとかで流用する場合はここに注意する必要がある

  //モンスターボール
  {
    for(int i = 0 ;i < 6; ++i){
      if(m_pModel[i] != NULL){
        GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetUpperRenderingPipeLine()->RemoveEdgeRenderingTarget( m_pModel[i]->GetModelInstanceNode() );
      }
      if(m_pResource[i] != NULL){
        GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetFieldRootNode()->RemoveResourceChild( m_pResource[i] );
      }
      GFL_SAFE_DELETE( m_pResource[i] );
      GFL_SAFE_DELETE( m_pModel[i] );
    }
  }

  //キャプチャ
  {
    {
      for(int i = 0 ;i < 6; ++i){
        //解除
        if(m_pMaterialNodeRef[i] != NULL){
          int texture_index = 0;
          m_pMaterialNodeRef[i]->SetTextureResourceNode( texture_index, NULL);
          m_pMaterialNodeRef[i] = NULL;
        }

        GFL_SAFE_DELETE(m_pCapturePoke[i]);
        GFL_SAFE_DELETE(m_pCapResourceNode[i]);
      }
    }
  }
}

/**
 *  @brief アニメーション開始
 */
void FieldGimmickPokecen::BirthdayAnimStart(u32 anim_index, bool is_on)
{
  if(is_on){
    m_AnimReq[anim_index] = 1;
  }else{
    m_AnimReq[anim_index] = 3;
  }
}

/**
 *  @brief アニメーション終了チェック
 */
bool FieldGimmickPokecen::BirthdayAnimIsEnd(u32 anim_index)
{
  return (m_AnimReq[anim_index] == 0);
}

GFL_NAMESPACE_END( Field )
