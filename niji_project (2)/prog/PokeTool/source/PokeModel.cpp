//======================================================================
/**
 * @file	PokeModel.cpp
 * @brief	niji ポケモンモデル
 * @author	ariizumi
 * @data	15/05/07
 */
//======================================================================
#include <Debug/include/gfl2_DebugPrint.h>
#include <util/include/gfl2_std_string.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryShdData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMcnfData.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_TextureResourceNodeFactory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_TextureResourceNode.h>

#include <RenderingEngine/include/Renderer/gfl2_CombinerShaderDriver.h>
#include <RenderingEngine/include/Renderer/gfl2_CombinerCtrShaderDriver.h>

#include "System/include/GflUse.h"
#include "Gamesys/include/GameManager.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/pokemon_model.gaix>

#include "PokeTool/include/PokeModelSystem.h"
#include "PokeTool/include/PokeModel.h"
#include "PokeTool/include/PokeAnimReplace.h"
#include "./PokeModelLocal.h"

#include "System/include/nijiAllocator.h"

#include <arc_index/wazaeff_resource.gaix>

GFL_NAMESPACE_BEGIN(PokeTool)

// 表情補間を無効にするポケモンリスト
// 目か口どちらかに問題が出たら両方無効にする
static const u16 sc_DisableFaceAnimSmoothMonsNoList[] =
{
  // ●待機モーション中などにポケモンがまばたきをした際、まつげとまぶたの動きが一致していません
  MONSNO_AMEMOOSU,    // アメモース
  MONSNO_SAKURABISU,  // サクラビス
  MONSNO_HURUUTU3,    // アマージュ
  MONSNO_KAWAHAGI,    // ハギギシリ
  MONSNO_BIISUTOo,    // アクジキング

  // ●ポケリフレにて視線合わせを行った際に開いた口に表示されている牙が不自然な挙動をとります
  MONSNO_FOKKO,       // フォッコ
  MONSNO_TEERUNAA,    // テールナー
  MONSNO_ONIHITODE1,  // ヒドイデ
  MONSNO_TORA2,       // ニャヒート
  MONSNO_ASIKA2,      // オシャマリ
};

PokeModel::PokeModel()
{
  mIsEnable = false;
}

PokeModel::~PokeModel()
{
}

//生成・削除
//----------------------------------------------------------------------------
/**
  *  @brief  PokeModel生成
  *
  *  @param  const SimpleParam *param                    シンプルパラム
  *  @param  const PokeModel::SetupOption *opt = NULL    セットアップオプション
  *
  *  ※この関数は使わずPokeModelSysからのCreateModel○○関数を使ってください。
  */
//-----------------------------------------------------------------------------
void PokeModel::CreateSimple( PokeModelSystem *modelSys , const SimpleParam *paramTemp , const SetupOption &opt ,gfl2::heap::HeapBase *dataHeap , void *vramAddr)
{
  mPokeModelSys = modelSys;
  mSimpleParam = *paramTemp;
  mSetupOpt = opt;
  if( dataHeap )
  {
    mSetupOpt.dataHeap = dataHeap;
  }
  if( vramAddr )
  {
    mSetupOpt.vramAddr = vramAddr;
  }

  mAsyncBufNum = 0;
  for( int i=0;i<BINPACK_MAX;i++ )
  {
    mDataBuf[i] = NULL;
    mAsyncBuf[i] = NULL;
  }

  if( mSimpleParam.isEgg )
  {
    mSimpleParam.monsNo = MONSNO_TAMAGO;
  }

  // APP用モーションをMODEL_ANIMETYPE_BATTLEかMODEL_ANIMETYPE_KAWAIGARIで割り当て直す
  if( mSetupOpt.animeType == MODEL_ANIMETYPE_APP )
  {
    if( PokeTool::CheckUsingKawaigariInApp( mSimpleParam.monsNo , mSimpleParam.formNo ) )
    {
      mSetupOpt.animeType = MODEL_ANIMETYPE_KAWAIGARI;
    }
    else
    {
      mSetupOpt.animeType = MODEL_ANIMETYPE_BATTLE;
    }
  }

  if( modelSys )
  {
    //共通ファイルの個数
    static const int datIdxOffset = 1;

    //管理データを取得
    const PokeModelSystem::POKE_MNG_DATA *mngData = modelSys->GetMngData( mSimpleParam.monsNo );
    const PokeModelSystem::POKE_FLG_DATA *flgData = modelSys->GetFlgData( mSimpleParam.monsNo , mSimpleParam.formNo , mSimpleParam.sex );

    //フォルム違いのデータが無い場合0扱い
    int formNo = mSimpleParam.formNo;
    if( mngData->mDataNum <= formNo )
    {
      formNo = 0;
    }

    //雌雄差かつメガ進化を考慮する
    const int mainDataIdx = modelSys->GetDataIdx(mSimpleParam.monsNo,formNo,mSimpleParam.sex);
    const int subDataIdx = modelSys->GetDataIdx(mSimpleParam.monsNo,flgData->mSubFrom,0);
    u32 dataTop = mainDataIdx*BINTYPE_MAX + datIdxOffset;
    u32 dataSubTop = subDataIdx*BINTYPE_MAX + datIdxOffset;

    //モデル
    if( flgData->mFlgData & PokeModelSystem::POKE_DATA_FLG_SHARE_MODEL )
    {
      DataLoadFunc(&mDataBuf[BINPACK_COMMON],dataSubTop + BINTYPE_COMMON);
    }
    else
    {
      DataLoadFunc(&mDataBuf[BINPACK_COMMON],dataTop + BINTYPE_COMMON);
    }
    //テクスチャ
    u32 texDataIdx = dataTop;
    if( flgData->mFlgData & PokeModelSystem::POKE_DATA_FLG_SHARE_TEX )
    {
      texDataIdx = dataSubTop;
    }
    if( mSimpleParam.isRare )
    {
      DataLoadFunc(&mDataBuf[BINPACK_TEXTURE],texDataIdx+BINTYPE_TEX_RARE);
    }
    else
    {
      DataLoadFunc(&mDataBuf[BINPACK_TEXTURE],texDataIdx+BINTYPE_TEX);
    }
    //IDテクスチャ
    if( mSetupOpt.useIdModel )
    {
      DataLoadFunc(&mDataBuf[BINPACK_ID],texDataIdx+BINTYPE_ID_TEX);
    }
    //モーション
    u32 motDataIdx = dataTop;
    if( flgData->mFlgData & PokeModelSystem::POKE_DATA_FLG_SHARE_ANIME )
    {
      motDataIdx = dataSubTop;

      //@attention
      //ミミッキュは大きい姿としてフォルム２と３がある。
      //フォルム３はモデルは0を読むが、アニメは１を読む。
      if( mSimpleParam.monsNo == MONSNO_HORAA &&
          mSimpleParam.formNo == 3)
      {
        const int exDataIdx = modelSys->GetDataIdx(mSimpleParam.monsNo,1,mSimpleParam.sex);
        motDataIdx = exDataIdx*BINTYPE_MAX + datIdxOffset;
      }
    }
    if( mSetupOpt.animeType == MODEL_ANIMETYPE_BATTLE )
    {
      DataLoadFunc(&mDataBuf[BINPACK_ANIME],motDataIdx+BINTYPE_BATTLE);
    }
    else
    if( mSetupOpt.animeType == MODEL_ANIMETYPE_KAWAIGARI )
    {
      DataLoadFunc(&mDataBuf[BINPACK_ANIME],motDataIdx+BINTYPE_KAWAIGARI);
    }
    else
    if( mSetupOpt.animeType == MODEL_ANIMETYPE_FIELD )
    {
      DataLoadFunc(&mDataBuf[BINPACK_ANIME],motDataIdx+BINTYPE_FIELD);
    }
    else
    if( mSetupOpt.animeType == MODEL_ANIMETYPE_POKE_FINDER )
    {
      DataLoadFunc(&mDataBuf[BINPACK_ANIME],motDataIdx+BINTYPE_POKE_FINDER);
    }
    else
    {
      GFL_ASSERT_MSG(0,"PokeModel:アニメタイプが不正！[%d]\n",mSetupOpt.animeType);
      DataLoadFunc(&mDataBuf[BINPACK_ANIME],motDataIdx+BINTYPE_BATTLE);
    }
    //設定データ
    DataLoadFunc(&mDataBuf[BINPACK_SETTING],dataTop+BINTYPE_SETTING);

    //フォルムチェンジでモーションだけ変わるやつら
    if( mSimpleParam.monsNo == MONSNO_GIRUGARUDO &&
        mSetupOpt.animeType  == MODEL_ANIMETYPE_BATTLE )
    {
      int exDataIdx;
      //もう一個のフォルムの方も読んでおく
      if( mSimpleParam.formNo == 0 )
      {
        exDataIdx = modelSys->GetDataIdx(mSimpleParam.monsNo,1,mSimpleParam.sex);
      }
      else
      {
        exDataIdx = modelSys->GetDataIdx(mSimpleParam.monsNo,0,mSimpleParam.sex);
      }
      u32 exDataTop = exDataIdx*BINTYPE_MAX + datIdxOffset;
      DataLoadFunc(&mDataBuf[BINPACK_ANIME_EX],exDataTop+BINTYPE_BATTLE);
    }

    if( mSimpleParam.monsNo == MONSNO_HORAA &&
        (mSimpleParam.formNo == 0 || mSimpleParam.formNo == 2) &&
        mSetupOpt.animeType  == MODEL_ANIMETYPE_BATTLE )
    {
      //ミミッキュのフォルム２→から３についてはモーションは１のものを使ってOK
      //もう一個のフォルムの方も読んでおく
      const int exDataIdx = modelSys->GetDataIdx(mSimpleParam.monsNo,1,mSimpleParam.sex);
      u32 exDataTop = exDataIdx*BINTYPE_MAX + datIdxOffset;
      DataLoadFunc(&mDataBuf[BINPACK_ANIME_EX],exDataTop+BINTYPE_BATTLE);
    }

    if( mSimpleParam.monsNo == MONSNO_NAGAREBOSI &&
        mSimpleParam.formNo < 7 &&
        mSetupOpt.animeType  == MODEL_ANIMETYPE_BATTLE )
    {
      //メテノは殻つき7種、韓無し７種
      //もう一個のフォルムの方も読んでおく
      const int exDataIdx = modelSys->GetDataIdx(mSimpleParam.monsNo,7,mSimpleParam.sex);
      u32 exDataTop = exDataIdx*BINTYPE_MAX + datIdxOffset;
      DataLoadFunc(&mDataBuf[BINPACK_ANIME_EX],exDataTop+BINTYPE_BATTLE);
    }
  }
  else
  {
    GFL_ASSERT_MSG(0,"PokeModel:PokeModelSystemがありません。\n");
  }
}

void PokeModel::DataLoadFunc( void **buf , int dataIdx )
{
  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
  req.arcId = ARCID_POKEMON_MODEL;
  req.datId = dataIdx;
  req.align = 128;
  req.heapForBuf = mSetupOpt.dataHeap;
  req.heapForReq = mSetupOpt.workHeap->GetLowerHandle();
  req.heapForCompressed = mSetupOpt.workHeap->GetLowerHandle();
  req.ppBuf = buf;

  mPokeModelSys->GetAsyncFileManager()->AddArcFileLoadDataReq(req);

  mAsyncBuf[mAsyncBufNum] = buf;
  mAsyncBufNum++;
}

//----------------------------------------------------------------------------
/**
  *  @brief  非同期読み完了待ち
  *
  *  @return bool 読み込みが完了したらtrue
  */
//-----------------------------------------------------------------------------
bool PokeModel::IsFinishLoad(void)
{
  for( int i=0;i<BINPACK_MAX;i++ )
  {
    if( mAsyncBuf[i] != NULL )
    {
      const bool ret = mPokeModelSys->GetAsyncFileManager()->IsArcFileLoadDataFinished(mAsyncBuf[i]);
      if( ret )
      {
        mAsyncBuf[i] = NULL;
      }
      else
      {
        return false;
      }
    }
  }
  return true;
}

//----------------------------------------------------------------------------
/**
  *  @brief  非同期ロード後の生成処理
  *
  *  IsFinishLoadからtrueが帰ってきたら読んでください。
  */
//-----------------------------------------------------------------------------
void PokeModel::Create( void )
{
  for( int i=0;i<BINPACK_MAX;i++)
  {
    if( mDataBuf[i] )
    {
      mBinData[i].Initialize(mDataBuf[i]);
    }
  }

  if( mSetupOpt.vramAddr )
  {
    mGLAllocator = GFL_NEW(mSetupOpt.dataHeap)PokeModelAllocator(mSetupOpt.dataHeap,mSetupOpt.vramAddr,POKEMODEL_VRAM_SIZE);
  }
  else
  {
    mGLAllocator = GFL_NEW(mSetupOpt.dataHeap)System::nijiAllocator(mSetupOpt.dataHeap);
  }

  CreateMcnf();
  CreateModel();
  CreateIdModel();
  CreateAnime();

  //Settingの反映
  if( mBinData[BINPACK_SETTING].GetDataSize(BINDATA_SETTING_SETTING) > 0 )
  {
    PokeSettingData *data = static_cast<PokeSettingData*>(mBinData[BINPACK_SETTING].GetData(BINDATA_SETTING_SETTING));
    gfl2::std::MemCopy( data   , &mPokeSettingData , sizeof(PokeSettingData) );
  }
  else
  {
    gfl2::std::MemClear( &mPokeSettingData , sizeof(PokeSettingData) );

    mPokeSettingData.cmHeight = 100;
    mPokeSettingData.minX = -50.0f;
    mPokeSettingData.minZ = -50.0f;
    mPokeSettingData.maxX =  50.0f;
    mPokeSettingData.maxY = 100.0f;
    mPokeSettingData.maxZ =  50.0f;
  }
  mBaseHeight = mPokeSettingData.cmHeight*0.01f;
  if (mBaseHeight == 0.0f)
  {
    mBaseHeight = 1.0f;
  }
//レアカラーのコンスタントカラーオフセットの対応
  if( mSimpleParam.isRare )
  {
    if( mBinData[BINPACK_SETTING].GetDataSize(BINDATA_SETTING_RARE) > 0 )
    {
      char* data = static_cast<char*>(mBinData[BINPACK_SETTING].GetData(BINDATA_SETTING_RARE));
      int dataPos = 0;
      int dataNum = data[dataPos];
      dataPos++;

      ARIIZUMI_PRINT("RareSetting Num[%d]\n",dataNum);
  
      for( int i=0;i<dataNum;i++ )
      {
        ARIIZUMI_PRINT("DataTop[%d]\n",dataPos);

        char name[POKE_MATNAME_LEN+1];
        int nameLen = 0;
        while( data[dataPos] != '\0' && nameLen<POKE_MATNAME_LEN )
        {
          name[nameLen] = data[dataPos];
          nameLen++;
          dataPos++;
        }
        name[nameLen] = '\0';
        dataPos++;
    
        GFL_ASSERT_MSG(nameLen<POKE_MATNAME_LEN,"Pokemodel RareSetting Matname is OVER!![%s]",name ); //@fix 対処。一応止まらないがレアの見た目がおかしくなる。作成時に気づく。

        u8 colNo = data[dataPos];
        dataPos++;
    
        while( (dataPos%4) != 0 )
        {
          ARIIZUMI_PRINT("Padding.[%d]\n",dataPos);
          dataPos++;
        }
    
        gfl2::math::Vector4 ofs;
        gfl2::std::MemCopy( &data[dataPos   ] , &ofs.x , 4 );
        gfl2::std::MemCopy( &data[dataPos+ 4] , &ofs.y , 4 );
        gfl2::std::MemCopy( &data[dataPos+ 8] , &ofs.z , 4 );
        gfl2::std::MemCopy( &data[dataPos+12] , &ofs.w , 4 );
        dataPos += 16;
    
        ARIIZUMI_PRINT("RareSetting[%d]\n",i);
        ARIIZUMI_PRINT("ColorNo[%d] Material[%s]\n",colNo,name);
        ARIIZUMI_PRINT("Col[%f][%f][%f][%f]\n",ofs.x,ofs.y,ofs.z,ofs.w);
        
        s32 matNo = GetMaterialIndex(name);
        if( matNo != -1 )
        {
          AddRareConstantColor(matNo,colNo,ofs);
        }
        else
        {
          GFL_ASSERT_MSG(0,"Pokemodel RareSetting Matname is not found!![%s]",name ); //@fix 対処。止まらないがレアの見た目がおかしくなる。作成時に気づく。
        }
      }
    }
  }
  SetAdjustHeight( mPokeSettingData.adjustHeight*0.01f );

  SetPosition(0,0,0);
  SetRotation(0,0,0);
  SetScale(1,1,1);
  SetOffset(0,0,0);
  mShaderColor.Set(0,0,0,0);

  //最初は絶対変わるようにしておく
  mPlayAnimeType = -1;
  mIsEnable = true;

  for( int i=0;i<POKE_ANIME_SLOT_EYE_NUM;i++ )
  {
    mActiveEyeAnime[i] = false;
  }
  mBlinkEnable = false;
  mBlinkState = 0;
  if( mSetupOpt.animeType == MODEL_ANIMETYPE_BATTLE )
  {
    //自動目パチの有効化
    SetAutoBlink(true);
  }

  // パッチールのブチの対応(個性乱数からブチのフレームを取得して設定)
  if (mSimpleParam.monsNo == MONSNO_PATTIIRU)
  {
    // loop01の固定フレームは上位16bit の 上位8bit
    u8 markingFrame1 = static_cast<u8>((mSimpleParam.perRand & 0x00FF0000) >> 16);
    // loop02の固定フレームは上位16bit の 下位8bit
    u8 markingFrame2 = static_cast<u8>((mSimpleParam.perRand & 0xFF000000) >> 24);
    // loop03の固定フレームは下位16bit の 上位8bit
    u8 markingFrame3 = static_cast<u8>((mSimpleParam.perRand & 0x000000FF));
    // loop04の固定フレームは下位16bit の 下位8bit
    u8 markingFrame4 = static_cast<u8>((mSimpleParam.perRand & 0x0000FF00) >> 8);
    poke_3d::model::PokemonModel::SetMarkingFrames(markingFrame1, markingFrame2, markingFrame3, markingFrame4);
  }
}

//1.0で1m
void PokeModel::SetAdjustHeight( f32 height )
{
  mAdjustHeight = height;

  if (mAdjustHeight == 0.0f)
  {
    mAdjustHeight = 1.0f/pow((mBaseHeight),POKE_BASE_ADJUST_RATE)*mBaseHeight;
  }
  poke_3d::model::PokemonModel::SetHeight(mBaseHeight, mAdjustHeight);
  mAdjustScale = poke_3d::model::PokemonModel::CalculateAdjustScale();

  mIsUpdate = true;
}


void PokeModel::CreateMcnf(void)
{
  //モデルより前に初期化する必要があるが、アニメの関係なので、分離。
  if( mSetupOpt.animeType == MODEL_ANIMETYPE_BATTLE )
  {
    mAnimeNum = MODEL_ANIME_BATTLE_NUM;
  }
  else
  if( mSetupOpt.animeType == MODEL_ANIMETYPE_KAWAIGARI )
  {
    mAnimeNum = MODEL_ANIME_KAWAIGARI_NUM;
  }
  else
  if( mSetupOpt.animeType == MODEL_ANIMETYPE_FIELD )
  {
    mAnimeNum = MODEL_ANIME_FIELD_NUM;
  }
  else
  if( mSetupOpt.animeType == MODEL_ANIMETYPE_POKE_FINDER )
  {
    mAnimeNum = MODEL_ANIME_POKE_FINDER_NUM;
  }
  else
  {
    GFL_ASSERT_MSG(0,"PokeModel;AnimeType is invalid!!\n");
  }

  //mcnfファイル
  if( mBinData[BINPACK_ANIME].GetDataSize(mAnimeNum+BINDATA_OFS_ANM_MCNF) > 0 )
  {
    gfl2::renderingengine::scenegraph::resource::GfBinaryMcnfData mcnfRes;
    mcnfRes.SetMcnfData((c8*)mBinData[BINPACK_ANIME].GetData(mAnimeNum+BINDATA_OFS_ANM_MCNF));
    mMcnfResNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( mGLAllocator,&mcnfRes );		
  }
  else
  {
    mMcnfResNode = NULL;
  }
}

//モデル生成
void PokeModel::CreateModel(void)
{

  //モデル
  {
    gfl2::renderingengine::scenegraph::resource::GfBinaryMdlData mdlRes;
    mdlRes.SetModelData((c8*)mBinData[BINPACK_COMMON].GetData(BINDATA_CO_MODEL));
    mModelResNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( mGLAllocator , &mdlRes );
  }

  //テクスチャ
  for( int i=0;i<mBinData[BINPACK_TEXTURE].GetDataMax();i++ )
  {
    gfl2::renderingengine::scenegraph::resource::GfBinaryTexData	texRes;
    gfl2::renderingengine::scenegraph::resource::ResourceNode		*texResNode;
    texRes.SetTextureData( (c8*)mBinData[BINPACK_TEXTURE].GetData(i) );
    texResNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( mGLAllocator , &texRes );
    mModelResNode->AddChild(texResNode);
  }

  //シェーダー
  {
    gfl2::fs::BinLinkerAccessor shaderPack;
    shaderPack.Initialize(mBinData[BINPACK_COMMON].GetData(BINDATA_CO_SHADER_PACK));
    for( int i=0;i<shaderPack.GetDataMax();i++ )
    {
      gfl2::renderingengine::scenegraph::resource::GfBinaryShdData shaderRes;
      gfl2::renderingengine::scenegraph::resource::ResourceNode *shaderResNode;
      shaderRes.SetShaderData( (c8*)shaderPack.GetData(i) );
    
      shaderResNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( mGLAllocator , &shaderRes );
      mModelResNode->AddChild(shaderResNode);
    }
  }

  {
    //モデル生成
    const bool ret = poke_3d::model::PokemonModel::Create(mGLAllocator, mSetupOpt.dataHeap, mModelResNode,mMcnfResNode);
    GFL_ASSERT_MSG(ret,"PokeModel:ModelCreate failue!\n");

  #if defined(GF_PLATFORM_CTR)
    this->CopyMaterialDepthStencilStateToStateObject();
  #endif
  }

  // アニメーション補間機能の有効無効設定
  if( mSetupOpt.animeType == MODEL_ANIMETYPE_BATTLE )
  {
    this->SetAnimationBlendEnable(false);
  }
  else
  if( mSetupOpt.animeType == MODEL_ANIMETYPE_KAWAIGARI )
  {
    this->SetAnimationBlendEnable(false);
  }
  else
  if( mSetupOpt.animeType == MODEL_ANIMETYPE_FIELD )
  {
    this->SetAnimationBlendEnable(true);
  }
  else
  if( mSetupOpt.animeType == MODEL_ANIMETYPE_POKE_FINDER )
  {
    this->SetAnimationBlendEnable(true);
  }
  else
  {
    GFL_ASSERT_MSG(0,"PokeModel;AnimeType is invalid!!\n");
  }

    //影モデル
  if( mSetupOpt.useShadow && mBinData[BINPACK_COMMON].GetDataSize(BINDATA_CO_SHADOW_MODEL) > 0 )
  {
    mShadowModel = GFL_NEW(mSetupOpt.dataHeap)poke_3d::model::PokemonModel();
    gfl2::renderingengine::scenegraph::resource::GfBinaryMdlData mdlRes;
    mdlRes.SetModelData((c8*)mBinData[BINPACK_COMMON].GetData(BINDATA_CO_SHADOW_MODEL));
    mShadowModelResNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( mGLAllocator , &mdlRes );

    //シェーダー
    gfl2::fs::BinLinkerAccessor shaderPack;
    shaderPack.Initialize(mBinData[BINPACK_COMMON].GetData(BINDATA_CO_SHADOW_SHADER_PACK));
    for( int i=0;i<shaderPack.GetDataMax();i++ )
    {
      gfl2::renderingengine::scenegraph::resource::GfBinaryShdData shaderRes;
      gfl2::renderingengine::scenegraph::resource::ResourceNode *shaderResNode;
      shaderRes.SetShaderData( (c8*)shaderPack.GetData(i) );
    
      shaderResNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( mGLAllocator , &shaderRes );
      mShadowModelResNode->AddChild(shaderResNode);
    }
    //テクスチャ
    for( int i=0;i<mBinData[BINPACK_TEXTURE].GetDataMax();i++ )
    {
      gfl2::renderingengine::scenegraph::resource::GfBinaryTexData	texRes;
      gfl2::renderingengine::scenegraph::resource::ResourceNode		*texResNode;
      texRes.SetTextureData( (c8*)mBinData[BINPACK_TEXTURE].GetData(i) );
      texResNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( mGLAllocator , &texRes );
      mShadowModelResNode->AddChild(texResNode);
    }
    const bool ret2 = mShadowModel->Create(mGLAllocator, mSetupOpt.dataHeap, mShadowModelResNode,this,mMcnfResNode);
    GFL_ASSERT_MSG(ret2,"PokeModel:ModelCreate failue!\n");
    //mShadowModel->SetLightDirection(gfl2::math::Vector3(0.5f,0.5f,0.5f));

    int matNum = mShadowModel->GetMaterialCount();
    for( int i=0;i<matNum;i++ )
    {
      gfl2::math::Vector4 col = mShadowModel->GetMaterialColorConstant(i,0);
      col.x = 0.5;
      mShadowModel->SetMaterialColorConstant(i,5,col);
      mShadowModel->GetModelInstanceNode()->SetUpDrawTag();
    }

    // 本体マテリアルとの対応テーブルを作成
    mShadowMaterialIndexList = NULL;
    if (mShadowModel->GetMaterialCount() > 0)
    {
      mShadowMaterialIndexList = GFL_NEW_ARRAY(mSetupOpt.dataHeap) s32[mShadowModel->GetMaterialCount()];
      for (u32 i = 0; i < mShadowModel->GetMaterialCount(); ++i)
      {
        const c8* pName = mShadowModel->GetModelInstanceNode()->GetMaterialInstanceNode(i)->GetName();
        mShadowMaterialIndexList[i] = this->GetMaterialIndex(pName);
      }
    }
  }
  else
  {
    mShadowModel = NULL;
    mShadowModelResNode = NULL;
    mShadowMaterialIndexList = NULL;
  }

  SetLightDirection(gfl2::math::Vector3(0.5f,-0.5f,0.5f));
}

//アニメ生成
void PokeModel::CreateAnime(bool createEffNode )
{

  mAnimeResNode = GFL_NEW_ARRAY(mSetupOpt.dataHeap)gfl2::renderingengine::scenegraph::resource::ResourceNode*[mAnimeNum];

  for( int i=0;i<mAnimeNum;i++ )
  {
    if( mBinData[BINPACK_ANIME].GetDataSize(i) > 0 )
    {
      gfl2::renderingengine::scenegraph::resource::GfBinaryMotData anmRes;
      anmRes.SetMotionData((c8*)mBinData[BINPACK_ANIME].GetData(i));
      mAnimeResNode[i] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( mGLAllocator,&anmRes );		
    }
  }

  if( createEffNode && mSetupOpt.animeType != MODEL_ANIMETYPE_POKE_FINDER )
  {
    for( int i=0;i<MODEL_NODE_MAX;i++ )
    {
      for( int j=0;j<MODEL_NODE_NUM_MAX;j++ )
      {
        mEffectNode[i][j] = NULL;
      }
    }

    //EffectNodeの読み込みと適用
    if( mBinData[BINPACK_ANIME].GetDataSize(mAnimeNum+BINDATA_OFS_ANM_EFFECT_NODE) > 0 )
    {
      int size = mBinData[BINPACK_ANIME].GetDataSize(mAnimeNum+BINDATA_OFS_ANM_EFFECT_NODE);
      mEffectNodeData = static_cast<EffectNodeData*>(mBinData[BINPACK_ANIME].GetData(mAnimeNum+BINDATA_OFS_ANM_EFFECT_NODE));
      mEffectNodeDataNum = size/sizeof(EffectNodeData);
      //パディングで正しい個数は取れない！
      //GFL_PRINT("size[%d] / [%d] = [%d]\n",size,sizeof(EffectNodeData),mEffectNodeDataNum);
    }
    else
    {
      mEffectNodeDataNum = 0;
    }

    for( int i=0;i<mEffectNodeDataNum;i++ )
    {
      //パディングで正規のデータ数がわからんので、こうする。
      if( strlen(mEffectNodeData[i].parentNode) > 0 )
      {
        if( mEffectNodeData[i].nodeIdx-1 >= MODEL_NODE_NUM_MAX )
        {
#if defined(DEBUG_ONLY_FOR_ariizumi_nobuhiko)
          GFL_ASSERT_MSG(0,"PokeModelSys NodeTypeIndex is over!![%d][%d]\n",mEffectNodeData[i].nodeType,mEffectNodeData[i].nodeIdx-1);
#endif
          break;
        }

        gfl2::math::Vector3 ofs( mEffectNodeData[i].ofsX,mEffectNodeData[i].ofsY,mEffectNodeData[i].ofsZ );
        gfl2::renderingengine::scenegraph::instance::EffectNodeLocatorInstanceNode *node = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateEffectNodeLocatorInstanceNode(mGLAllocator,mEffectNodeData[i].parentNode);
        node->SetLocalTranslate(ofs);
        if( i == 0 )
        {
          node->SetNodeName("TestNode");
        }
        AttachLocatorNode(node);
        mEffectNode[mEffectNodeData[i].nodeType][mEffectNodeData[i].nodeIdx-1] = node;
        //GFL_PRINT("[%s][%d][%d][%f][%f][%f]\n",mEffectNodeData[i].parentNode,mEffectNodeData[i].nodeType,mEffectNodeData[i].nodeIdx,mEffectNodeData[i].ofsX,mEffectNodeData[i].ofsY,mEffectNodeData[i].ofsZ);
      }
    }
  }

  //フェイシャルアニメの設定
  for( int i=0;i<POKE_OVERRIDE_ANIME_NUM;i++ )
  {
    const gfl2::animation::OverridePartsType type = static_cast<gfl2::animation::OverridePartsType>(i);
    if( mBinData[BINPACK_ANIME].GetDataSize(mAnimeNum+BINDATA_OFS_ANM_EYE01+i) > 0 )
    {
      gfl2::renderingengine::scenegraph::resource::GfBinaryMotData anmRes;
      anmRes.SetMotionData((c8*)mBinData[BINPACK_ANIME].GetData(mAnimeNum+BINDATA_OFS_ANM_EYE01+i));
      mOverrideAnimeResNode[i] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( mGLAllocator,&anmRes );		
    }
    else
    {
      mOverrideAnimeResNode[i] = NULL;
    }
    // 下記SetMotionResourceNode()で設定するモーションのインデックス
    s32 facialMotionIndex = POKE_ANIME_SLOT_EYE1 + i;
    SetMotionResourceNode(facialMotionIndex, mOverrideAnimeResNode[i]);
    
    // 表情のモーションとして使用するスロットのインデックス
    s32 facialMotionSlotIndex = POKE_ANIME_SLOT_EYE1 + i;

    // 上書きするモーションの種類
    // その際使用するスロットのインデックス
    // PokemonModelで設定したどのモーションを使用するのか
    // を設定
    SetOverridePartsMotionIndex(
      type,
      facialMotionSlotIndex,
      facialMotionIndex);
  }

  for( int i=0;i<POKE_REPLACE_ANIM_NUM;i++ )
  {
    mpFaceReplaceList[i] = GFL_NEW(mSetupOpt.dataHeap) PokeAnimReplace();
    mpFaceReplaceList[i]->Initialize(mSetupOpt.dataHeap, this->GetModelInstanceNode(), mOverrideAnimeResNode[i]);
  }

  SetUpLoopMotion(0,true);
  SetUpLoopMotion(1,true);
  SetUpLoopMotion(2,true);
  SetUpLoopMotion(3,true);

  // AK_PartsSkel01, AK_PartsMaterial01, AK_PartsVisible01の3つのフレームを初期化
  for( int i=0;i<POKE_ANIME_PATTERN_NUM;i++ )
  {
    mPartsLoopAnimeFrame[i] = 0.0f;
  }

  // 表情補間を有効化する
  // @note サクラビスやテールナーなど表情補間に問題があるポケモンは関数内で強制的に補間を無効にされる
  this->SetFaceAnimeSmoothEnable(true);
}

//IDモデルの作成
void PokeModel::CreateIdModel(void)
{
  if( mSetupOpt.useIdModel )
  {
    GFL_ASSERT_MSG(mPokeModelSys->GetIdTextureResourceNode(),"Please create IdModel shader!\n");

    //一部処理がResourceNodeを直接書き換えるので、別個で作る必要がある。
    gfl2::renderingengine::scenegraph::resource::GfBinaryMdlData mdlRes;
    mdlRes.SetModelData((c8*)mBinData[BINPACK_COMMON].GetData(BINDATA_CO_MODEL));
    mIdModelResNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( mGLAllocator , &mdlRes );

    //テクスチャ
    for( int i=0;i<mBinData[BINPACK_TEXTURE].GetDataMax();i++ )
    {
      gfl2::renderingengine::scenegraph::resource::GfBinaryTexData	texRes;
      gfl2::renderingengine::scenegraph::resource::ResourceNode		*texResNode;
      texRes.SetTextureData( (c8*)mBinData[BINPACK_TEXTURE].GetData(i) );
      texResNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( mGLAllocator , &texRes );
      mIdModelResNode->AddChild(texResNode);
    }

    //シェーダー
    {
      gfl2::fs::BinLinkerAccessor shaderPack;
      shaderPack.Initialize(mBinData[BINPACK_COMMON].GetData(BINDATA_CO_SHADER_PACK));
      for( int i=0;i<shaderPack.GetDataMax();i++ )
      {
        gfl2::renderingengine::scenegraph::resource::GfBinaryShdData shaderRes;
        gfl2::renderingengine::scenegraph::resource::ResourceNode *shaderResNode;
        shaderRes.SetShaderData( (c8*)shaderPack.GetData(i) );

        shaderResNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( mGLAllocator , &shaderRes );
        mIdModelResNode->AddChild(shaderResNode);
      }
    }

    //モデルの生成
    mIdModel = GFL_NEW(mSetupOpt.dataHeap)poke_3d::model::PokemonModel();
    mIdModel->Create(mGLAllocator, mSetupOpt.dataHeap, mIdModelResNode,this,mMcnfResNode);

    int dataSize = mBinData[BINPACK_ID].GetDataSize(BINDATA_ID_MAT_SETTING);
    //サイズが128でパディングされているのであまりあてにならない
    u32 dataNum = dataSize/(POKE_MATNAME_LEN+1);  
    //GFL_PRINT("[%d][%d]\n",dataSize,dataNum);
    char *idSetting = (char*)mBinData[BINPACK_ID].GetData(BINDATA_ID_MAT_SETTING);

    //マテリアルの書き換え処理
    for( u32 datIdx = 0 ;datIdx < dataNum ; datIdx++ )
    {
      char *datName = &idSetting[datIdx*(POKE_MATNAME_LEN+1)];
      const s32 matNo = mIdModel->GetMaterialIndex(datName);
      if( matNo != -1 )
      {
        switch(datName[POKE_MATNAME_LEN])
        {
        case 0:
          break;
        case 1:
          {
            //最終段を取得
            const s32 combCnt = 5;

            gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* matNode;
            matNode = mIdModel->GetModelInstanceNode()->GetMaterialInstanceNode( matNo );
            const c8* shaderName = matNode->GetMaterialResourceNode()->GetShaderTypeName();
#if defined(GF_PLATFORM_CTR)
            if ( std::strcmp( shaderName, "CombCtr" ) == 0 )
            {
              gfl2::renderingengine::renderer::CombinerCtrShaderDriver	*shaderDriver = static_cast<gfl2::renderingengine::renderer::CombinerCtrShaderDriver*>( matNode->GetShaderDriver() );
              shaderDriver->SetPixelShaderResourceNode( mPokeModelSys->GetIdTextureResourceNode() );
            }

#elif defined(GF_PLATFORM_WIN32)
            if ( std::strcmp( shaderName, "Comb" ) == 0 )
            {
              gfl2::renderingengine::renderer::CombinerShaderDriver	*shaderDriver = static_cast<gfl2::renderingengine::renderer::CombinerShaderDriver*>( matNode->GetShaderDriver() );
              shaderDriver->SetPixelShaderResourceNode( mPokeModelSys->GetIdTextureResourceNode() );
            }
#endif      
            //プラットフォーム分岐
            //@todo
            //mIdModel->SetMaterialTextureCombinerCombineRgb( materialNo , combCnt , gfl::grp::g3d::H3dTextureCombinerExConst::COMBINE_REPLACE );
            //mIdModel->SetMaterialTextureCombinerCombineAlpha( materialNo , combCnt , gfl::grp::g3d::H3dTextureCombinerExConst::COMBINE_REPLACE );
            //mIdModel->SetMaterialTextureCombinerOperandRgb( materialNo ,  combCnt , 0 , gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDRGB_SRC_COLOR );
            //mIdModel->SetMaterialTextureCombinerOperandAlpha( materialNo ,  combCnt , 0 , gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDALPHA_SRC_ALPHA );
            //mIdModel->SetMaterialTextureCombinerSourceRgb( materialNo ,  combCnt , 0 , gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_TEXTURE0 );
            //mIdModel->SetMaterialTextureCombinerSourceAlpha( materialNo ,  combCnt , 0 , gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_TEXTURE0 );
            
            mIdModel->SetMaterialAlphaTestEnable( matNo , false );
          }
          break;
        case 2:
          {
            mIdModel->SetMaterialDepthTestFunction( matNo, poke_3d::model::BaseModel::MATERIAL_TEST_FUNCTION_NEVER );
            mIdModel->GetModelInstanceNode()->GetMaterialInstanceNode(matNo)->GetDepthStencilStateObject()->UpdateState();
          }
          break;
        case 255:
#if defined(DEBUG_ONLY_FOR_ariizumi_nobuhiko)
          GFL_ASSERT_MSG(0,"Id mat setting error!!![%s]\n",datName);//@check 一応問題なし。タッチはおかしくなるかも。
#endif
          break;
        }
        gfl2::renderingengine::scenegraph::resource::MaterialResourceNode                   *pMaterialResourceNode;
        pMaterialResourceNode = const_cast<gfl2::renderingengine::scenegraph::resource::MaterialResourceNode*>(mIdModel->GetModelInstanceNode()->GetMaterialInstanceNode(matNo)->GetMaterialResourceNode());

        for( u32 i = 0; i < pMaterialResourceNode->GetTextureNum(); ++i )
        {
          pMaterialResourceNode->GetTextureInfo( i ).m_Attribute.m_MappingType = 0;//0=UvMappingを意味します。
          pMaterialResourceNode->GetTextureInfo( i ).m_Attribute.m_ScaleU = 1.0f;
          pMaterialResourceNode->GetTextureInfo( i ).m_Attribute.m_ScaleV = 1.0f;
          pMaterialResourceNode->GetTextureInfo( i ).m_Attribute.m_TranslateU = 0.0f;
          pMaterialResourceNode->GetTextureInfo( i ).m_Attribute.m_TranslateV = 0.0f;
#if defined(GF_PLATFORM_CTR)
          pMaterialResourceNode->GetTextureInfo( i ).m_Attribute.m_RepeatU = PICA_DATA_TEXTURE_WRAP_REPEAT;
          pMaterialResourceNode->GetTextureInfo( i ).m_Attribute.m_MinFilter = PICA_DATA_TEXTURE_MIN_FILTER_NEAREST_MIPMAP_LINEAR;
          pMaterialResourceNode->GetTextureInfo( i ).m_Attribute.m_MagFilter = PICA_DATA_TEXTURE_MAG_FILTER_NEAREST;
#elif defined(GF_PLATFORM_WIN32)
          gfl2::gfx::SamplerStateObject   *pSampler = const_cast<gfl2::gfx::SamplerStateObject*>( pMaterialResourceNode->GetSamplerStateObject( i ) );
          if(pSampler)
            {
              pSampler->SetSamplerAddress( gfl2::gfx::TextureAddress::Wrap, gfl2::gfx::TextureAddress::Wrap );//WrapU,WrapV同時設定
              pSampler->SetSamplerFilter(gfl2::gfx::TextureFilter::Point, gfl2::gfx::TextureFilter::Point);
              pSampler->UpdateState();
            }
#endif
        }
      }
      else
      {
#if defined(DEBUG_ONLY_FOR_ariizumi_nobuhiko)
        //GFL_ASSERT_MSG(0,"Material not found!!![%s]\n",datName);//@check テスト用
#endif
      }
    }

    //テクスチャの差し替え処理
    if( mBinData[BINPACK_ID].GetDataMax() > 0 ) // @fix cov_ctr[11814] unsignedのマイナスオーバーフロー対策
    {
      mIdTexNum = mBinData[BINPACK_ID].GetDataMax() - 1;
      mIdTexResNode = GFL_NEW_ARRAY(mSetupOpt.dataHeap)gfl2::renderingengine::scenegraph::resource::TextureResourceNode*[mIdTexNum];

      // テクスチャ生成用のファクトリを取得
      gfl2::renderingengine::scenegraph::resource::IResourceNodeFactory* pFactory =
        gfl2::renderingengine::scenegraph::resource::ResourceManager::GetResourceNodeFactory(
          gfl2::renderingengine::scenegraph::resource::IResourceData::Type("texture")
          );
			gfl2::renderingengine::scenegraph::resource::TextureResourceNodeFactory* pTextureResourceNodeFactory = 
				reinterpret_cast<gfl2::renderingengine::scenegraph::resource::TextureResourceNodeFactory*>(pFactory);

      for( int i=0;i<mIdTexNum;i++ )
      {
        gfl2::renderingengine::scenegraph::resource::GfBinaryTexData texRes;
        texRes.SetTextureData((c8*)mBinData[BINPACK_ID].GetData(i+1));
			  mIdTexResNode[i] = 
				  reinterpret_cast<gfl2::renderingengine::scenegraph::resource::TextureResourceNode*>(
				    pTextureResourceNodeFactory->CreateResourceNode(mGLAllocator,texRes.GetData(0),texRes.GetDataSize(0)) );
        
        mIdModel->ReplaceWithIdTextureResourceNode(mIdTexResNode[i]);
      }
    }
    mIdModel->GetModelInstanceNode()->SetUpDrawTag();
    gfl2::math::Vector3 lightVec(0,0,1);
    mIdModel->SetLightDirection(lightVec);
  }
  else
  {
    mIdModel = NULL;
    mIdTexResNode = NULL;
  }
}

//歪み用モデル取得(NULLなら存在無し・実態はメインのモデルと同等
poke_3d::model::PokemonModel* PokeModel::GetDistortionModel(void)
{
  if( mSimpleParam.monsNo == MONSNO_YAMIRAMI && 
      mSimpleParam.formNo == FORMNO_YAMIRAMI_MEGA )
  {
    return this;
  }
  return NULL;
}

//IsFinishLoad後に確認可能
bool PokeModel::IsAvailableIdModel(void)
{
  if( mSetupOpt.useIdModel )
  {
    //IDテクスチャがあるか？
    if( mIdTexNum > 0 )
    {
      return true;
    }
  }
  return false;
}
bool PokeModel::IsEnableEffectNode(MODEL_NODE node , int idx )
{
  if( mEffectNode[node][idx] )
  {
    return true;
  }
  return false;
}

bool PokeModel::GetEffectNodePosition(MODEL_NODE node , int idx , gfl2::math::Vector3 *ret )
{
  if( IsEnableEffectNode(node,idx) )
  {
    *ret = mEffectNode[node][idx]->GetWorldMatrix().GetElemPosition();
    return true;
  }
  else
  {
    *ret = GetPosition() + mOffset;
    return false;
  }
}

//----------------------------------------------------------------------------
/**
  *  @brief  削除チェック
  */
//-----------------------------------------------------------------------------
bool PokeModel::CanDelete( void )
{
  return CanDestroy();
}

bool PokeModel::CanDestroy() const
{
  if( poke_3d::model::PokemonModel::CanDestroy() == false )
  {
    return false;
  }
  if( mShadowModel && mShadowModel->CanDestroy() == false )
  {
    return false;
  }
  if( mIdModel && mIdModel->CanDestroy() == false )
  {
    return false;
  }
  return true;
}

/**
  * @brief 解放チェック有効無効設定
  */
void PokeModel::SetDestroyCheckEnable(bool enable)
{
  poke_3d::model::PokemonModel::SetDestroyCheckEnable(enable);
  if( mShadowModel )
  {
    mShadowModel->SetDestroyCheckEnable(enable);
  }  if( mIdModel )
  {
    mIdModel->SetDestroyCheckEnable(enable);
  }
}


//----------------------------------------------------------------------------
/**
  *  @brief  削除
  */
//-----------------------------------------------------------------------------
void PokeModel::Delete( void )
{
  if( mIdModel )
  {
    mIdModel->Destroy();
    GFL_SAFE_DELETE(mIdModel);
    for( int i=0;i<mIdTexNum;i++ )
    {
      GFL_SAFE_DELETE(mIdTexResNode[i]);
    }
    GFL_SAFE_DELETE_ARRAY(mIdTexResNode);
    GFL_SAFE_DELETE(mIdModelResNode);
  }

  if( mShadowModel )
  {
    mShadowModel->Destroy();
    GFL_SAFE_DELETE(mShadowModel);
    GFL_SAFE_DELETE(mShadowModelResNode);
    GFL_SAFE_DELETE_ARRAY(mShadowMaterialIndexList);
  }

  poke_3d::model::PokemonModel::Destroy();
  GFL_SAFE_DELETE(mMcnfResNode);
  for( int i=0;i<POKE_OVERRIDE_ANIME_NUM;i++ )
  {
    GFL_SAFE_DELETE(mOverrideAnimeResNode[i]);
  }

  for( int i=0;i<POKE_REPLACE_ANIM_NUM;i++ )
  {
    // @fix GFNMCat[2310] NULLチェックを追加
    if (mpFaceReplaceList[i] != NULL)
    {
      mpFaceReplaceList[i]->Finalize();
      GFL_SAFE_DELETE(mpFaceReplaceList[i]);
    }
  }

  for( int i=0;i<mAnimeNum;i++ )
  {
    GFL_SAFE_DELETE(mAnimeResNode[i]);
  }
  GFL_SAFE_DELETE_ARRAY(mAnimeResNode);
  mAnimeNum = 0;
  GFL_SAFE_DELETE(mModelResNode);
  GFL_SAFE_DELETE(mGLAllocator);

  for( int i=0;i<BINPACK_MAX;i++)
  {
    if( mDataBuf[i] )
    {
      GflHeapFreeMemory(mDataBuf[i]);
      mDataBuf[i] = NULL;
    }
  }
  mIsEnable = false;

}

//更新
void PokeModel::Update(void)
{
  if( mIsUpdate )
  {
    gfl2::math::Vector3 scale = mBaseScale*mAdjustScale;
    poke_3d::model::PokemonModel::SetScale( scale );
    
    gfl2::math::Vector3 pos = GetPosition()+GetOffset(); 
    poke_3d::model::PokemonModel::SetPosition( pos );

    mIsUpdate = false;
  }

  // キーアニメーションを更新し, コールバック情報によりループを制御
  // PokemonModel::UpdateAnimation()より前に呼ぶ必要があるため, ここで更新
  UpdateKeyAnimationCallBack();

  // GameManagerが初期化されていないとは考えにくいが念のため確認
  GameSys::GameManager *gameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  GFL_ASSERT_STOP((gameManager != NULL) && "GameSys::GameManager is not initialized.");

  // 本体の再生速度をループにも使う
  f32 loopSpeed = this->GetAnimationStepFrame();
  for (int animationPatternNo = 0; animationPatternNo < POKE_ANIME_PATTERN_NUM; ++animationPatternNo)
  {
    //@attention ここはPOKE_ANIME_SLOT_LOOP1のほうではなく、システムの方(パーツアニメ文の配列なので)
    if( !mOverrideAnimeResNode[gfl2::animation::OverridePartsType::Loop01] )
    {// 上書き用のループモーションがなければ設定の必要はないので抜ける
      break;
    }

    // ループモーションのコールバックはloop01のみのため, 設定すべきインデックスは0のみ
    const u32 loopAnimationIndex = 0;
    const poke_3d::model::BaseModel::AnimationBit anmBit = poke_3d::model::PokemonModel::s_LoopAnimationBit[animationPatternNo];

    // ジョイントを持っている & 描画スキップした場合はフレームを進めない
    if (HasJointAnimationInLoopMotion(loopAnimationIndex) && (gameManager->GetGpuRunSkipCount() > 0))
    {
      loopSpeed = 0.0f;
    }

    // ループのアニメーションのフレームを更新
    mPartsLoopAnimeFrame[animationPatternNo] += loopSpeed;

    f32 maxFrame = 0.0f;
    maxFrame = GetAnimationEndFrame(POKE_ANIME_SLOT_LOOP1,anmBit);
    if( mPartsLoopAnimeFrame[animationPatternNo] >= maxFrame )
    {
      mPartsLoopAnimeFrame[animationPatternNo] -= maxFrame;
    }

    bool isEnableLoopMotion = this->GetLoopAnimationFlag(animationPatternNo);
    if (!isEnableLoopMotion)
    {// 該当のループモーションが有効でなければフレームを0に戻す
      mPartsLoopAnimeFrame[animationPatternNo] = 0.0f;
    }
    SetAnimationFrame(mPartsLoopAnimeFrame[animationPatternNo],POKE_ANIME_SLOT_LOOP1,anmBit);
  }

  UpdateBlink();
}

void PokeModel::UpdateAfter(void)
{
  // 表情アニメの上書き処理更新
  for (s32 i = 0; i < POKE_REPLACE_ANIM_NUM; ++i)
  {
    mpFaceReplaceList[i]->UpdateReplace();
  }

  // 影モデルに本体のマテリアルUV座標をコピー
  if (mShadowMaterialIndexList != NULL)
  {
    for (u32 shadowMaterialIndex = 0; shadowMaterialIndex < mShadowModel->GetMaterialCount(); ++shadowMaterialIndex)
    {
      s32 materialIndex = mShadowMaterialIndexList[shadowMaterialIndex];
      if (materialIndex < 0)
      {
        continue;
      }

      for (u32 slotNo = 0; slotNo < gfl2::renderingengine::scenegraph::resource::CombinerAttribute::TextureMax; ++slotNo)
      {
        gfl2::math::Vector2 scale = this->GetMaterialTextureCoordinatorScaleUV(materialIndex, slotNo);
        f32 rotate = this->GetMaterialTextureCoordinatorRotateUV(materialIndex, slotNo);
        gfl2::math::Vector2 translate = this->GetMaterialTextureCoordinatorTranslateUV(materialIndex, slotNo);

        mShadowModel->SetMaterialTextureCoordinatorScaleUV(shadowMaterialIndex, slotNo, scale);
        mShadowModel->SetMaterialTextureCoordinatorRotateUV(shadowMaterialIndex, slotNo, rotate);
        mShadowModel->SetMaterialTextureCoordinatorTranslateUV(shadowMaterialIndex, slotNo, translate);
      }
    }
  }

}

void PokeModel::SetupColorShader(void)
{
  CreateColorShaderTextures(mGLAllocator);
  SetEnabledColorShaderTexture( true );

  u32			colTexNum = GetColorTextureNum();
  for( u32 texNo = 0; texNo < colTexNum; ++texNo )
  {
	  const gfl2::renderingengine::scenegraph::resource::TextureResourceNode			*pTextureNode;
	  const gfl2::gfx::Texture																										*pOriginalColorTexture;
	  const gfl2::gfx::Texture																										*pColorShaderTexture;

	  pTextureNode = GetOriginalColorTextureResourceNode(texNo);
	  //const c8*				pName = pTextureNode->GetName();
	  pOriginalColorTexture = pTextureNode->GetTexture();
	  pColorShaderTexture = GetColorShaderTextureResourceNode(texNo)->GetTexture();

    gfl2::renderingengine::renderer::util::ColorShaderTextureRenderPath *path = mPokeModelSys->CreatePokeShaderPath(mSetupOpt.dataHeap);
    path->SetOriginalColorTexture(pOriginalColorTexture);
    path->SetColorShaderTexture(pColorShaderTexture);

	  path->SetColorShaderTextureColor( gfl2::math::Vector4( 0, 0, 1, 0.0 ) );

    mPokeShaderRenderPaths.push_back(path);

  }
  GetModelInstanceNode()->SetUpDrawTag();
}

  //※開放１フレ前に呼ぶこと！
void PokeModel::CleanColorShader(void)
{
  if( IsEnabledColorShaderTexture() )
  {
	  for( u32 i = 0; i < mPokeShaderRenderPaths.size(); ++i )
	  {
      mPokeModelSys->DeletePokeShaderPath(mPokeShaderRenderPaths[i]);
	  }
	  mPokeShaderRenderPaths.clear();
    SetEnabledColorShaderTexture( false );
  }
}

/**
 * @brief カラーシェーダの色の設定
 * @param col カラーシェーダで適用する色
 * @param executeFrame 実行するフレーム
 * @note executeFrameは基本1. 処理落ち発生の際に, 確実に実行してほしいフレーム数を指定する
 */
void PokeModel::SetShaderColor(const gfl2::math::Vector4& col, s32 executeFrame)
{
  mShaderColor = col;
  SetInterpColor(mShaderColor);
  for( u32 i=0;i<mPokeShaderRenderPaths.size();i++ )
  {
    gfl2::math::Vector4 vec( mShaderColor.x, mShaderColor.y, mShaderColor.z, mShaderColor.w );
    mPokeShaderRenderPaths[i]->SetColorShaderTextureColor( vec, executeFrame );
  }
  GetModelInstanceNode()->SetUpDrawTag();
}


//アニメーション関係
// @value forceReset モーション番号が同じだった場合リセットをかけるか？
void PokeModel::ChangeAnimation( MODEL_ANIME type , bool forceReset )
{
  ChangeAnimationDirect(type,forceReset);
}
void PokeModel::ChangeAnimation( MODEL_ANIME_POKE_FINDER type , bool forceReset )
{
  ChangeAnimationDirect(type,forceReset);
}
void PokeModel::ChangeAnimationDirect( int type , bool forceReset )
{
  if (!IsNecessaryToChangeAnimation(type, forceReset))
  {// アニメーションの切替の必要がなければ終了
    return;
  }

  // 同じアニメーションのときはコールバックステータスをクリアしない
  if (mPlayAnimeType != type)
  {
    // モーション切替時にコールバックの設定とループモーションのフラグのリセットを行う
    ResetAnimationStatus();
  }
  mPlayAnimeType = type;
  // リセットの後にアニメーションの切替を行う
  ChangeAnimationByResourceNode( mAnimeResNode[type] );

  // コールバックを再設定
  this->SetKeyCallBackInfo();
}

void PokeModel::ChangeAnimationSmooth( MODEL_ANIME type , int smoothFrame,bool forceReset )
{
  if (!IsNecessaryToChangeAnimation(type, forceReset))
  {// アニメーションの切替の必要がなければ終了
    return;
  }

  // 同じアニメーションのときはコールバックステータスをクリアしない
  if (mPlayAnimeType != type)
  {
    // モーション切替時にコールバックの設定とループモーションのフラグのリセットを行う
    ResetAnimationStatus();
  }
  mPlayAnimeType = type;
  // リセットの後にアニメーションの切替を行う
  ChangeAnimationSmoothByResourceNode( mAnimeResNode[type],smoothFrame );

  // コールバックを再設定
  this->SetKeyCallBackInfo();
}

void PokeModel::ChangeAnimationSmooth( MODEL_ANIME_POKE_FINDER type , int smoothFrame,bool forceReset )
{
  if (!IsNecessaryToChangeAnimation(type, forceReset))
  {// アニメーションの切替の必要がなければ終了
    return;
  }

  // 同じアニメーションのときはコールバックステータスをクリアしない
  if (mPlayAnimeType != type)
  {
    // モーション切替時にコールバックの設定とループモーションのフラグのリセットを行う
    ResetAnimationStatus();
  }
  mPlayAnimeType = type;
  // リセットの後にアニメーションの切替を行う
  ChangeAnimationSmoothByResourceNode( mAnimeResNode[type],smoothFrame );

  // コールバックを再設定
  this->SetKeyCallBackInfo();
}

//リソース渡し再生(全力技・デモ等外部でモーションを持つ場合)
void PokeModel::ChangeAnimationResource( gfl2::renderingengine::scenegraph::resource::ResourceNode *anmRes,int aniSlot )
{
  GFL_ASSERT_MSG(aniSlot<POKE_ANIME_SLOT_EYE1,"Slot index is Over!!\n");
  if( anmRes != NULL )
  {
    //クリアするためにNULLが呼ばれる場合がある
    mPlayAnimeType = -1;
  }
  // 同じアニメーションのときはコールバックステータスをクリアしない
  if (anmRes != this->GetAnimationResourceNode(aniSlot))
  {
    // モーション切替時にコールバックの設定とループモーションのフラグのリセットを行う
    ResetAnimationStatus();
  }
  // リセットの後にアニメーションの切替を行う
  ChangeAnimationByResourceNode( anmRes,aniSlot );

  // コールバックを再設定
  this->SetKeyCallBackInfo();
}

//ランダム再生時に再生して良いモーションか？(待機で始まり、待機で戻るか？)
bool PokeModel::CheckCanOneshotMotion( MODEL_ANIME type )
{
  //ランダム再生時に再生して良いモーションか？(待機で始まり、待機で戻るか？)
  if( IsAvailableAnimation(type) == false )
  {
    return false;
  }

  bool isNijiPoke = false;

  if( mSimpleParam.monsNo >= MONSNO_HUKUROU1 )
  {
    isNijiPoke = true;
  }
  else if( mSimpleParam.monsNo == MONSNO_KORATTA &&
    mSimpleParam.formNo >= FORMNO_KORATTA_ALOLA )
  {
    isNijiPoke = true;
  }
  else if( mSimpleParam.monsNo == MONSNO_RATTA &&
    mSimpleParam.formNo >= FORMNO_RATTA_ALOLA )
  {
    isNijiPoke = true;
  }
  else if( mSimpleParam.monsNo == MONSNO_PIKATYUU &&
    mSimpleParam.formNo >= FORMNO_PIKATYUU_SATOPIKAMU )
  {
    isNijiPoke = true;
  }
  else if( mSimpleParam.monsNo == MONSNO_SANDO &&
    mSimpleParam.formNo >= FORMNO_SANDO_ALOLA )
  {
    isNijiPoke = true;
  }
  else if( mSimpleParam.monsNo == MONSNO_SANDOPAN &&
    mSimpleParam.formNo >= FORMNO_SANDOPAN_ALOLA )
  {
    isNijiPoke = true;
  }
  else if( mSimpleParam.monsNo == MONSNO_ROKON &&
    mSimpleParam.formNo >= FORMNO_ROKON_ALOLA )
  {
    isNijiPoke = true;
  }
  else if( mSimpleParam.monsNo == MONSNO_KYUUKON &&
    mSimpleParam.formNo >= FORMNO_KYUUKON_ALOLA )
  {
    isNijiPoke = true;
  }
  else if( mSimpleParam.monsNo == MONSNO_DHIGUDA &&
    mSimpleParam.formNo >= FORMNO_DHIGUDA_ALOLA )
  {
    isNijiPoke = true;
  }
  else if( mSimpleParam.monsNo == MONSNO_DAGUTORIO &&
    mSimpleParam.formNo >= FORMNO_DAGUTORIO_ALOLA )
  {
    isNijiPoke = true;
  }
  else if( mSimpleParam.monsNo == MONSNO_NYAASU &&
    mSimpleParam.formNo >= FORMNO_NYAASU_ALOLA )
  {
    isNijiPoke = true;
  }
  else if( mSimpleParam.monsNo == MONSNO_PERUSIAN &&
    mSimpleParam.formNo >= FORMNO_PERUSIAN_ALOLA )
  {
    isNijiPoke = true;
  }
  else if( mSimpleParam.monsNo == MONSNO_ISITUBUTE &&
    mSimpleParam.formNo >= FORMNO_ISITUBUTE_ALOLA )
  {
    isNijiPoke = true;
  }
  else if( mSimpleParam.monsNo == MONSNO_GOROON &&
    mSimpleParam.formNo >= FORMNO_GOROON_ALOLA )
  {
    isNijiPoke = true;
  }
  else if( mSimpleParam.monsNo == MONSNO_GOROONYA &&
    mSimpleParam.formNo >= FORMNO_GOROONYA_ALOLA )
  {
    isNijiPoke = true;
  }
  else if( mSimpleParam.monsNo == MONSNO_BETOBETAA &&
    mSimpleParam.formNo >= FORMNO_BETOBETAA_ALOLA )
  {
    isNijiPoke = true;
  }
  else if( mSimpleParam.monsNo == MONSNO_BETOBETON &&
    mSimpleParam.formNo >= FORMNO_BETOBETON_ALOLA )
  {
    isNijiPoke = true;
  }
  else if( mSimpleParam.monsNo == MONSNO_NASSII &&
    mSimpleParam.formNo >= FORMNO_NASSII_ALOLA )
  {
    isNijiPoke = true;
  }
  else if( mSimpleParam.monsNo == MONSNO_GARAGARA &&
    mSimpleParam.formNo >= FORMNO_GARAGARA_ALOLA )
  {
    isNijiPoke = true;
  }
  else if( mSimpleParam.monsNo == MONSNO_SANDO &&
    mSimpleParam.formNo >= FORMNO_SANDO_ALOLA )
  {
    isNijiPoke = true;
  }
  else if( mSimpleParam.monsNo == MONSNO_GEKKOUGA &&
    mSimpleParam.formNo >= FORMNO_GEKKOUGA_SATOSHI )
  {
    isNijiPoke = true;
  }
  else if( mSimpleParam.monsNo == MONSNO_ZIGARUDE &&
    mSimpleParam.formNo >= FORMNO_ZIGARUDE_10PER )
  {
    isNijiPoke = true;
  }

  if( mSetupOpt.animeType == MODEL_ANIMETYPE_BATTLE )
  {
    //バトルモーション
    if( isNijiPoke == false )
    {
      //sango準拠チェック
      if( (type >= MODEL_ANIME_LAND_A && type <= MODEL_ANIME_APPEAL ) ||
        type == MODEL_ANIME_DOWN )
      {
        return false;
      }

      //フォルム別例外
      if( mSimpleParam.monsNo == MONSNO_GIRUGARUDO &&
        mSimpleParam.formNo == FORMNO_GIRUGARUDO_TATE )
      {
        if( type == MODEL_ANIME_TOKUSHU2 )
        {
          return false;
        }
      }
      if( mSimpleParam.monsNo == MONSNO_GIRUGARUDO &&
        mSimpleParam.formNo == FORMNO_GIRUGARUDO_HOKO )
      {
        if( type == MODEL_ANIME_BUTURI2 )
        {
          return false;
        }
      }

      //フーパ例外
      if( mSimpleParam.monsNo == MONSNO_HUUPA &&
        mSimpleParam.formNo == FORMNO_HUUPA_EARNEST )
      {
        if( type == MODEL_ANIME_BUTURI1 ||
          type == MODEL_ANIME_BT_WAIT_C )
        {
          return false;
        }
      }
      //レックウザ例外
      if( mSimpleParam.monsNo == MONSNO_REKKUUZA &&
        mSimpleParam.formNo == FORMNO_REKKUUZA_DELTA )
      {
        if( type == MODEL_ANIME_TOKUSHU2 )
        {
          return false;
        }
      }
    }
    else
    {
      //niji基準チェック
      if( type == MODEL_ANIME_BT_WAIT_A ||
        type == MODEL_ANIME_BT_WAIT_B ||
        type == MODEL_ANIME_BT_WAIT_C ||
          type == MODEL_ANIME_BUTURI1 ||
          type == MODEL_ANIME_TOKUSHU1 )
      {
        if( mSimpleParam.monsNo == MONSNO_NAGAREBOSI &&
          mSimpleParam.formNo <= FORMNO_NAGAREBOSI_M_PURPLE )
        {
          if( type == MODEL_ANIME_TOKUSHU1 )
          {
            //フォルムチェンジ用
            return false;
          }
        }
      }
      else
      {
        return false;
      }
    }
  }
  else
  {
    //かわいがりモーション
    if( isNijiPoke == false  )
    {
      //sango準拠チェック
      if( (type >= MODEL_ANIME_RESPOND && type <= MODEL_ANIME_SLEEP_B ) ||
        type == MODEL_ANIME_LONELY || 
        (type >= MODEL_ANIME_EAT_A && type <= MODEL_ANIME_EAT_C ) )
      {
        return false;
      }
    }
    else
    {
      //niji基準チェック
      if( type == MODEL_ANIME_KW_WAIT ||
          type == MODEL_ANIME_HAPPY_B ||
          type == MODEL_ANIME_MAD ||
          type == MODEL_ANIME_HATE )
      {
        return true;
      }
      else
      {
        return false;
      }
    }

  }
  return true;

}

bool PokeModel::IsAvailableAnimation( MODEL_ANIME type )
{
  return IsAvailableAnimationDirect(type);
}

bool PokeModel::IsAvailableAnimation( MODEL_ANIME_POKE_FINDER type )
{
  return IsAvailableAnimationDirect(type);
}

bool PokeModel::IsAvailableAnimationDirect( int type )
{
  if( mAnimeNum <= type )
  {
    return false;
  }
  if( mAnimeResNode[type] )
  {
    // @attention 9末に仮ポケモンのデータでジョイントが無く対処を入れた痕跡・
    // 3月にヒヒダルマのダルマモードのWaitAでジョイントアニメが無くはまりが発生。
    // なのでチェックをはずす  [niji] System 198
    //gfl2::renderingengine::scenegraph::resource::AnimationResourceNode* pAnimResourceNode = mAnimeResNode[type]->GetChild()->SafeCast<gfl2::renderingengine::scenegraph::resource::AnimationResourceNode>();
    //if( pAnimResourceNode->HasJointAnimation() )
    {
      return true;
    }
  }
  return false;
}
MODEL_ANIME PokeModel::GetAnimationType(void)
{
  return static_cast<MODEL_ANIME>(mPlayAnimeType);
}
MODEL_ANIME_POKE_FINDER PokeModel::GetAnimationType_PokeFinder(void)
{
  return static_cast<MODEL_ANIME_POKE_FINDER>(mPlayAnimeType);
}

void PokeModel::SetAnimationIsLoop( bool flg )
{
  mIsLoopAnime = flg;
  poke_3d::model::BaseModel::SetAnimationLoop(mIsLoopAnime);
}

bool PokeModel::GetAnimationIsLoop(void)
{
  return mIsLoopAnime;
}

//@berif キーアニメーションのタイミング取得(バトルの攻撃タイミング先調べ用)
//@param MODEL_ANIME type    モーション種類
//@param const char* keyName キーの名前(AK_EffectStart01)
//@return -1 で該当なし
s32 PokeModel::GetKeyTiming( MODEL_ANIME type ,  const char* keyName )
{
  if( mAnimeResNode[type] == NULL )
  {
    return -1;
  }

  // リソースノード保存
  gfl2::renderingengine::scenegraph::resource::AnimationResourceNode* pAnimResourceNode = mAnimeResNode[type]->GetChild()->SafeCast<gfl2::renderingengine::scenegraph::resource::AnimationResourceNode>();
  // リソースを巡回する
  if (pAnimResourceNode->m_pKeyAnimHeader != NULL)
  {
    uptr dataAddr = reinterpret_cast<uptr>(pAnimResourceNode->m_pKeyAnimHeader);
    dataAddr += sizeof(gfl2::renderingengine::scenegraph::resource::AnimationResourceNode::KeyAnimHeader);
    for (u32 attributeNo = 0; attributeNo < pAnimResourceNode->m_pKeyAnimHeader->attributeCount; ++attributeNo)
    {
      // 名前解決
      const gfl2::renderingengine::scenegraph::resource::AnimationResourceNode::Name* pName = reinterpret_cast<const gfl2::renderingengine::scenegraph::resource::AnimationResourceNode::Name*>(dataAddr);
      dataAddr += pName->length + sizeof(char);

      const gfl2::renderingengine::scenegraph::resource::AnimationResourceNode::KeyAnimBody* pBody = reinterpret_cast<const gfl2::renderingengine::scenegraph::resource::AnimationResourceNode::KeyAnimBody*>(dataAddr);

      if( (std::memcmp(pName->name, keyName, std::strlen(keyName)) == 0) )
      {
        // キーフレーム数
        for( u32 k=0;k<pBody->keyCount;k++ )
        {
          if( pBody->keyList[k].value >= 1.0f )
          {
            return pBody->keyList[k].frame;
          }
        }
      }
      dataAddr += (sizeof(gfl2::renderingengine::scenegraph::resource::AnimationResourceNode::KeyAnimBody) - sizeof(gfl2::renderingengine::scenegraph::resource::AnimationResourceNode::KeyAnimBody::Key)) + (pBody->keyCount * sizeof(gfl2::renderingengine::scenegraph::resource::AnimationResourceNode::KeyAnimBody::Key));
    }
  }
  return -1;
}

//表示切替(影の制御のため継承
void PokeModel::SetVisible(b32 flag)
{
  poke_3d::model::PokemonModel::SetVisible(flag);
  if( mShadowModel )
  {
    mShadowModel->SetVisible(flag);
  }
}

//複数体表示時のIDエッジのオフセット指定
void PokeModel::SetStencilReferenceOffset(u8 ofs)
{
  int matNum = GetMaterialCount();

  for( int i=0;i<matNum;i++ )
  { 
    gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode               *pMaterialInstanceNode = GetModelInstanceNode()->GetMaterialInstanceNode(i);
    gfl2::gfx::DepthStencilStateObject                                              *pDepthStencilStateObject = pMaterialInstanceNode->GetDepthStencilStateObject();
    

    s32 id = pDepthStencilStateObject->GetStencilReference();

    if( mSimpleParam.monsNo == MONSNO_KAIOOGA && mSimpleParam.formNo == FORMNO_KAIOOGA_ALPHA )
    {//void H3dPokemonShaderModel::ShiftIDEdgeColor_kyogresp( s32 shift, s32 materialNo )の移植
      //GFSBTS[902] カイオーガはステンシルテストのIDの値が248～254
      //内部的に通過時にインクリメントも使っているので普通のポケモンのずらす処理は使えない。
      //なので-1方向へずらす。数値が重なるのはソート処理が入っていれば問題ない(ゴースで必要だったので入っているはず)
      id -= ofs;
    }
    else
    {
      id += ofs;
    }
    if ( id > 255 )     id = 255;
    if ( id < 0 )       id = 0;

    pDepthStencilStateObject->SetStencilReference( static_cast<u8>(id) );
    pDepthStencilStateObject->UpdateState();

#if defined(GF_PLATFORM_WIN32)
    //win32ではステンシルバッファをテクスチャとして持てないので、EdgeMapRenderPathにてオフスクリーンにrUserData.m_EdgeIDを描画して擬似的に再現しています。
    //アクセッサを用意していなかったので、直接書き換えます。
    gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::UserData     &rUserData = const_cast<gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::UserData&>( pMaterialInstanceNode->GetUserData() );

    if (rUserData.m_IDEdgeEnable)
    {
      rUserData.m_EdgeID = static_cast<u32>(id);
    }
#endif
  }

}


//フェイシャル関係
void PokeModel::SetFaceAnime_Eye( int type , int eyeIdx , bool isBlink )
{
  for( int i=0;i<POKE_ANIME_SLOT_EYE_NUM;i++ )
  {
    if( eyeIdx == -1 || eyeIdx == i )
    {
      SetFaceAnime( type , gfl2::animation::OverridePartsType::Eye01 + i , isBlink );
    }
  }
}

void PokeModel::SetFaceAnime_Mouth( int type , int mouthIdx )
{
  for( int i=0;i<POKE_ANIME_SLOT_MOUTH_NUM;i++ )
  {
    if( mouthIdx == -1 || mouthIdx == i )
    {
      SetFaceAnime( type , gfl2::animation::OverridePartsType::Mouth01 + i );
    }
  }
}

void PokeModel::ResetFaceAnime_Eye( int eyeIdx )
{
  SetFaceAnime_Eye( -1 , eyeIdx );
}
void PokeModel::ResetFaceAnime_Mouth( int mouthIdx )
{
  SetFaceAnime_Mouth( -1 , mouthIdx );
}

void PokeModel::SetFaceAnime( int tempType , int slot , bool isBlink )
{
  const u32 anmSlot = POKE_ANIME_SLOT_EYE1+slot;

  const u32 anmBitTemp = //poke_3d::model::BaseModel::ANIMATION_BIT_JOINT | // ジョイントは専用の上書き処理なので不要
                    poke_3d::model::BaseModel::ANIMATION_BIT_MATERIAL | 
                    poke_3d::model::BaseModel::ANIMATION_BIT_VISIBILITY;
  const poke_3d::model::BaseModel::AnimationBit anmBit = static_cast<poke_3d::model::BaseModel::AnimationBit>(anmBitTemp);

  bool isSetAnime = false;
  f32 jointReplaceFrame = -1;
  int type = tempType;
  //リグレーなどすべて無効になっているポケモンもいるので注意！
  if( tempType >= 0 )
  {
    // アニメーションのスロットが目なのか口なのかを判定
    if ((POKE_ANIME_SLOT_EYE1 <= anmSlot) && (anmSlot <= POKE_ANIME_SLOT_EYE3))
    {// 目の使用フラグを確認. 使用していなければtypeを-1(リセット)にする
      type = mPokeSettingData.eyeUsedFlags[tempType] ? tempType : -1;
    }
    else if ((POKE_ANIME_SLOT_MOUTH1 <= anmSlot) && (anmSlot <= POKE_ANIME_SLOT_MOUTH3))
    {// 口の使用フラグを確認. 使用していなければtypeを-1(リセット)にする
      type = mPokeSettingData.mouthUsedFlags[tempType] ? tempType : -1;
    }
  }

  if( type >= 0 )
  {
    jointReplaceFrame = type*10.0f+1.0f;

    //フェイシャル設定
    if( mOverrideAnimeResNode[slot] )
    {
      ChangeAnimationByResourceNode( mOverrideAnimeResNode[slot],anmSlot,anmBit );
      SetAnimationFrame(type*10.0f+1.0f,anmSlot,anmBit );
      SetAnimationStepFrame(0,anmSlot,anmBit );

      isSetAnime = true;
    }
  }
  else
  {
    //フェイシャルリセット
    if( mOverrideAnimeResNode[slot] )
    {
      ChangeAnimationByResourceNode( NULL,anmSlot,anmBit );
    }

    jointReplaceFrame = -1; // マイナス指定でリセット
  }

  // フェイシャルを保存
  mEyeMouseState[slot] = type;

  // ジョイントは専用の上書き処理
  if ((slot >= 0) && (slot < POKE_REPLACE_ANIM_NUM))
  {
    f32 stepFrame = 0;
    u32 smoothFrame = 8;

    // 補間無効ならフレーム数を0に変更
    if (!mIsFaceAnimSmoothEnable)
    {
      smoothFrame = 0;
    }

    // 現在のフレームと変更がある場合だけ設定する
    if (mpFaceReplaceList[slot]->GetFrame() != jointReplaceFrame)
    {
      mpFaceReplaceList[slot]->ReplaceSmooth(jointReplaceFrame, stepFrame, smoothFrame);
    }
  }
  
  //自動目パチ抑制用
  if ((POKE_ANIME_SLOT_EYE1 <= anmSlot) && (anmSlot <= POKE_ANIME_SLOT_EYE3))
  {
    if( isBlink == false )
    {
      int eyeIdx = anmSlot-POKE_ANIME_SLOT_EYE1;
      mActiveEyeAnime[eyeIdx] = isSetAnime;
    }
  }
}

void PokeModel::SetEnvColor(POKE_ENV_TYPE type)
{
  f32 tempCol = 1.0f;
  
  switch( type )
  {
  case POKE_ENV_NONE:    //朝昼
    tempCol = 1.0f;
    break;
  case POKE_ENV_EVENING: //夕
    tempCol = 0.9f;
    break;
  case POKE_ENV_NIGHT:   //夜
    tempCol = 0.7f;
    break;
  case POKE_ENV_CAVE:    //洞窟(バトルで天球が無い場合はNONEへ流れる
    tempCol = 0.7f;
    break;
  }
  gfl2::math::Vector4 enbColor(tempCol,tempCol,tempCol,1.0f);
  SetEnabledAmbientColor(true);
  SetAmbientColor(enbColor);
}


//モーションを加味したボックスの取得
bool PokeModel::GetMotionCollision( gfl2::math::AABB *box )
{
  return GetMotionCollisionDirect(box);
}
bool PokeModel::GetMotionCollision( gfl2::math::AABB *box , MODEL_ANIME type )
{
  return GetMotionCollisionDirect(box,type);
}
bool PokeModel::GetMotionCollisionDirect( gfl2::math::AABB *box , int type )
{
/*
  if( mMotionBox == NULL )
  {
    gfl::math::VEC4 minVec(-0.5f,0,-0.5f,0);
    gfl::math::VEC4 maxVec( 0.5f,1, 0.5f,0);
    box->SetMinMax( minVec,maxVec );
    
    return false;
  }
  else
*/
  {
    int typeTemp = type;
    if( typeTemp == -1 )
    {
      typeTemp = GetAnimationType();
    }
    
    gfl2::math::Vector4 minVec(mPokeSettingData.minX,mPokeSettingData.minY,mPokeSettingData.minZ,0);
    gfl2::math::Vector4 maxVec(mPokeSettingData.maxX,mPokeSettingData.maxY,mPokeSettingData.maxZ,0);
    
    minVec *= GetAdjustScale();
    maxVec *= GetAdjustScale();
    
    minVec.x *= GetScale().x;
    minVec.y *= GetScale().y;
    minVec.z *= GetScale().z;
    maxVec.x *= GetScale().x;
    maxVec.y *= GetScale().y;
    maxVec.z *= GetScale().z;
    
    box->SetMinMax( minVec,maxVec );
   
#define gfl2_math_Vector4_IsZero(vec) (vec.x==0.f&&vec.y==0.f&&vec.z==0.f&&vec.w==0.f)
    if( gfl2_math_Vector4_IsZero(minVec) && 
        gfl2_math_Vector4_IsZero(maxVec) )
    {
      return false;
    }
    else
    {
      return true;
    }
#undef gfl2_math_Vector4_IsZero
  }
}

//サイズカテゴリの取得
POKE_SIZE PokeModel::GetPokeSize(void)const
{
  return static_cast<POKE_SIZE>(mPokeSettingData.size);
}

//全力シェーダー例外マテリアルの取得
const void* PokeModel::GetFullPowerDisableMaterialData(int *retSize)
{
  int dataSize = mBinData[BINPACK_COMMON].GetDataSize(BINDATA_CO_FP_DISABLE_SETTING);
  if( dataSize > 0 )
  {
    if( retSize )
    {
      *retSize = dataSize;
    }
    return mBinData[BINPACK_COMMON].GetData(BINDATA_CO_FP_DISABLE_SETTING);
  }
  if( retSize )
  {
    *retSize = 0;
  }
  return NULL;
}

//特殊フォルムチェンジ
void PokeModel::NoLoadFormChange(pml::FormNo formNo)
{
  if( mDataBuf[BINPACK_ANIME_EX] )
  {
    if( mSimpleParam.formNo != formNo )
    {
      //フェイシャルをリセットしておかないとまずい
      ResetFaceAnime_Eye();
      ResetFaceAnime_Mouth();

      //今のアニメの開放
      for( int i=0;i<POKE_OVERRIDE_ANIME_NUM;i++ )
      {
        const gfl2::animation::OverridePartsType type = static_cast<gfl2::animation::OverridePartsType>(i);
        SetMotionResourceNode(POKE_ANIME_SLOT_EYE1+i,NULL);
      }
      ChangeAnimationByResourceNode( NULL );

      for( int i=0;i<POKE_REPLACE_ANIM_NUM;i++ )
      {
        GFL_SAFE_DELETE( mpFaceReplaceList[i] );
      }

      for( int i=0;i<POKE_OVERRIDE_ANIME_NUM;i++ )
      {
        GFL_SAFE_DELETE(mOverrideAnimeResNode[i]);
      }
      for( int i=0;i<mAnimeNum;i++ )
      {
        GFL_SAFE_DELETE(mAnimeResNode[i]);
      }
      GFL_SAFE_DELETE_ARRAY(mAnimeResNode);
      GFL_SAFE_DELETE(mMcnfResNode);

      void *temp = mDataBuf[BINPACK_ANIME_EX];
      mDataBuf[BINPACK_ANIME_EX] = mDataBuf[BINPACK_ANIME];
      mDataBuf[BINPACK_ANIME] = temp;
      mBinData[BINPACK_ANIME].Initialize(mDataBuf[BINPACK_ANIME]);

      //再生成
      CreateMcnf();
      //mcnfの再設定
      InitializeInterest(mGLAllocator,mMcnfResNode);
      CreateAnime(false);

      mSimpleParam.formNo = formNo;
      ChangeAnimationDirect( MODEL_ANIME_BT_WAIT_A , true );
    }
  }
  else
  {
    GFL_ASSERT_MSG(0,"NoLoadFormChange is not ready!\n");//@check 問題なし。
  }
}

// --------------------------------------------------------------------------
/**
  * @brief  メガゲンガーシェーダー照り返し部分の開始位置設定
  * @param  startY       メガゲンガーシェーダー照り返し部分の開始位置（ワールド座標Ｙ）
  */
// --------------------------------------------------------------------------
void PokeModel::SetMegaGangarEffectState( f32 startY )
{
  using namespace gfl2::renderingengine::scenegraph;

  instance::ModelInstanceNode   *pModelInstanceNode = GetModelInstanceNode();
  u32                           materialNum = pModelInstanceNode->GetMaterialNum();

  for( u32 i = 0; i < materialNum; ++i )
  {
    instance::MaterialInstanceNode                      *pMaterialInstanceNode = pModelInstanceNode->GetMaterialInstanceNode( i );
    resource::MaterialResourceNode                      *pMaterialResourceNode = const_cast<resource::MaterialResourceNode*>( pMaterialInstanceNode->GetMaterialResourceNode() );
    const resource::MaterialResourceNode::UserData      &rUserData = pMaterialResourceNode->GetUserData();

    if( rUserData.m_VertexShaderType == resource::VertexShaderType::MegaGangar )
    {
      pMaterialResourceNode->SetUserDataShaderParam( resource::MaterialResourceNode::MEGAGANGAR_EFFECT_STATE_START_Y_PARAM_NO, startY );
    }
  }
}


void PokeModel::SetAutoBlink(bool tempFlg)
{
  bool flg = tempFlg;
  
  //kujira・sango殻の注釈
  //PMBTS476 ムシャーナはデフォルトで目を閉じているが、開き目のテクスチャがあるので、
  //目パチさせると一瞬開いて閉じてまた開いて戻るので、目パチしない。
  //PMBTS652 バイバニラは左の顔のデフォルトがにっこり目なので
  //目パチさせると一瞬開いて閉じてまた開いて戻るので、目パチしない。海野さん承認済み
  if( mSimpleParam.monsNo == MONSNO_MUSYAANA ||
      mSimpleParam.monsNo == MONSNO_BAIBANIRA )
  {
    flg = false;
  }
  //niji追加
  if( mSimpleParam.monsNo == MONSNO_ABU1 ||
      mSimpleParam.monsNo == MONSNO_ABU2 ||
      mSimpleParam.monsNo == MONSNO_DORAGON ||
      mSimpleParam.monsNo == MONSNO_NIKKOU ||
      mSimpleParam.monsNo == MONSNO_GEKKOU )
  {
    flg = false;
  }

  if( mBlinkEnable!=flg )
  {
    mBlinkEnable=flg;
    UpdateBlink();
  }
}

/**
 * @brief アニメーション関連のステータスのリセット
 * @note アニメーション最初の設定時, 切替時にコールバック設定とフラグリセット
 */
void PokeModel::ResetAnimationStatus()
{
  // 各AK_Partsのフラグをリセット
  for (u32 i = 0; i < KeySoundAttribute::AK_PartsShader01 - KeySoundAttribute::AK_PartsSkel01 + 1; ++i)
  {
    this->ResetLoopAnimationFlag(i);
  }
}

/**
 * @brief アニメーションの切替の必要があるかの判定
 * @param type アニメーションの種類
 * @param forceReset 力のリセットの必要あり/なし
 * @return true/false 変更の必要あり/なし
 */
b32 PokeModel::IsNecessaryToChangeAnimation(int type, bool forceReset)
{
  if (!IsAvailableAnimationDirect(type))
  {// アニメーションを直接切替できない場合はfalse
    return false;
  }

  b8 isSameType = GetAnimationType() == type; // 切替元と切替先の種類が同じかチェック
  b8 needToChangeAnimation = !isSameType;     // 切替の必要があるかないかをまず種類で判断
  if (isSameType)
  {// タイプが一致しているなら, 外力をリセットするかを判定条件に加える
    needToChangeAnimation = forceReset;   // リセットするならアニメーション切替の必要あり.
  }

  // アニメーションの切替の必要があるかないかを返す
  return needToChangeAnimation;
}

void PokeModel::UpdateBlink(void)
{
  //ステート
  enum
  {
    BLINK_NONE,

    BLINK_WAIT,
    BLINK_UPDATE,
  };

  static const int BLINK_INTERBAL_MIN = 120;  //30fps計算
  static const int BLINK_INTERBAL_RAND = 120;

/*
  bool isSkipFrame = false;
  if( mModelSys && mModelSys->GetGameData() )
  {
    isSkipFrame = mModelSys->GetGameData()->GetIsSkipDraw();
  }
*/
  bool isActiveEyeAnime = false;
  for( int i=0;i<POKE_ANIME_SLOT_EYE_NUM;i++ )
  {
    if( mActiveEyeAnime[i] == true )
    {
      isActiveEyeAnime = true;
    }
  }


  if( mBlinkState == BLINK_NONE )
  {
    //可愛がりもバトルもWaitは0なのでまとめてOK
    if( mBlinkEnable && 
        GetAnimationType() == MODEL_ANIME_BT_WAIT_A &&
        isActiveEyeAnime == false )
    {
      mBlinkState = BLINK_WAIT;
      mBlinkCnt = System::GflUse::GetPublicRand(BLINK_INTERBAL_RAND) + BLINK_INTERBAL_MIN;
    }
  }
  else
  {
    if( mBlinkEnable == false ||
      GetAnimationType() != MODEL_ANIME_BT_WAIT_A ||
      isActiveEyeAnime == true )
    {
      if( mBlinkState == BLINK_UPDATE )
      {
        for( int i=0;i<POKE_ANIME_SLOT_EYE_NUM;i++ )
        {
          //アニメが設定されているところはリセットしない
          if( mActiveEyeAnime[i] == false )
          {
            ResetFaceAnime_Eye(i);
          }
        }
      }
      mBlinkState = BLINK_NONE;
    }
    else
    {
      switch( mBlinkState )
      {
      case BLINK_WAIT:
        if( GetAnimationStepFrame() != 0.0f )
        {
          mBlinkCnt -= 1;
          if( mBlinkCnt < 0 )
          {
            mBlinkState = BLINK_UPDATE;
            mBlinkCnt = 0;
          }
        }
        break;
      case BLINK_UPDATE:
        static const int BLINK_ANM_NUM = 9;
        u8 eyeIdxArr[BLINK_ANM_NUM]=
        {
          EYETYPE_HALFOPEN,
          EYETYPE_HALFOPEN,
          EYETYPE_CLOSE,
          EYETYPE_CLOSE,
          EYETYPE_HALFOPEN,
          EYETYPE_HALFOPEN,
          EYETYPE_OPEN,
          EYETYPE_OPEN,
          EYETYPE_OPEN
        };
        SetFaceAnime_Eye(eyeIdxArr[mBlinkCnt],-1,true);
        mBlinkCnt += 1;

        if( mBlinkCnt >= BLINK_ANM_NUM )
        {
          ResetFaceAnime_Eye();
          mBlinkCnt = System::GflUse::GetPublicRand(BLINK_INTERBAL_RAND) + BLINK_INTERBAL_MIN;
          mBlinkState = BLINK_WAIT;
        }
        break;
      }
    }
  }
}

/**
 * @brief ループモーションのコールバック制御の更新
 * @note コールバックから帰ってきたフラグによってループの有効/無効切替
 */
void PokeModel::UpdateKeyAnimationCallBack()
{
  // キーアニメーションの更新
  // これを呼ぶことでコールバック処理からフラグを受け取る
  this->UpdateKeyAnimation();

  // キーアニメーション更新後のループモーションのフラグを取得
  // サウンドのキー(AK_ButuriStart01など)はバトルで別途制御しているため, 
  // ここで取得するのはループモーションに関連する
  // AK_PartsSkel01, AK_PartsMaterial01, AK_PartsVisible01 のみ.
  b32 loopFlags[poke_3d::model::PokemonModel::LOOP_ANIMATION_BIT_PATTERN_NUM];
  for (u32 i = 0; i < poke_3d::model::PokemonModel::LOOP_ANIMATION_BIT_PATTERN_NUM; ++i)
  {
    loopFlags[i] = this->GetLoopAnimationFlag(i);
  }

  // ループモーションはモデルの仕様上, loop01のみのため, ループのインデックスは0のみを指定する.
  // loop02～loop04を持っているのはnijiの時点でpm0327_00_patcheelのみ.
  // パッチールのぶちはPokemonModel::SetUpMarkings()を使用して固定しているため,
  // ここの処理は影響を受けない.
  u32 loopMotionIndex = 0;
  for (s32 i = 0; i < poke_3d::model::PokemonModel::LOOP_ANIMATION_BIT_PATTERN_NUM; ++i)
  {
    // フラグに応じてループモーションの有効/無効を切り替え
    this->SetUpLoopMotion(loopMotionIndex, loopFlags[i], poke_3d::model::PokemonModel::s_LoopAnimationBit[i]);
  }
}

// 表情補間の有効無効を設定
void PokeModel::SetFaceAnimeSmoothEnable(bool enable)
{
  mIsFaceAnimSmoothEnable = enable;

  // 表情補間に問題があるポケモンは補間を無効にする
  for (u32 i = 0; i < sizeof(sc_DisableFaceAnimSmoothMonsNoList) / sizeof(sc_DisableFaceAnimSmoothMonsNoList[0]); ++i)
  {
    if (mSimpleParam.monsNo == sc_DisableFaceAnimSmoothMonsNoList[i])
    {
      mIsFaceAnimSmoothEnable = false;
      break;
    }
  }
}

// 目の状態の取得
int PokeModel::GetEyeState(u32 slot)
{
  if(slot < POKE_ANIME_SLOT_EYE_NUM)
  {
    return mEyeMouseState[slot];
  }
  else
  {
    GFL_ASSERT(0);
    return -1;
  }
}
// 口の状態の取得
int PokeModel::GetMouseState(u32 slot)
{
  if(slot < POKE_ANIME_SLOT_MOUTH_NUM)
  {
    return mEyeMouseState[POKE_ANIME_SLOT_EYE_NUM + slot];
  }
  else
  {
    GFL_ASSERT(0);
    return -1;
  }
}

/// @brief ループアニメーションフレームのセット
/// @note POKE_ANIME_SLOT_LOOP1のフレームを変更するときはこの関数を使用する
void PokeModel::SetLoop1AnimationFrame(f32 frame)
{
  for(int animationPatternNo = 0; animationPatternNo < POKE_ANIME_PATTERN_NUM; ++animationPatternNo)
  {
    mPartsLoopAnimeFrame[animationPatternNo] = frame;
  }
}

GFL_NAMESPACE_END(PokeTool)
