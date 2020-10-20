//==============================================================================
/**
 @file    app_tool_RaderChart.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.06.07
 @brief   レーダーチャート
 */
//==============================================================================


// gfl2のインクルード
#include <debug/include/gfl2_DebugPrint.h>
#include <thread/include/gfl2_ThreadStatic.h>
#include <math/include/gfl2_math.h>
#include <fs/include/gfl2_Fs.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>

// poke_3d_libのインクルード
#include <model/include/gfl2_BaseModel.h>

// nijiのデータのヘッダーファイルのインクルード
#include <arc_def_index/arc_def.h>
#include <arc_index/rader_chart.gaix>

// nijiのインクルード
#include <AppLib/include/Util/app_util_heap.h>
#include <System/include/ArcIdGetter.h>
#include <AppLib/include/Util/app_util_FileAccessor.h>

#include <App/RaderChart/include/app_tool_RaderChart.h>


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)


//==============================================================================
//
//     RaderChart
//
//==============================================================================


//const f32 RaderChart::ADJUST_RADIUS_TIMES = 1.1456f;  // 先細りして指定した半径に届かないので、これだけ拡大して届かせる。
const f32 RaderChart::ADJUST_RADIUS_TIMES = 1.0f;  // 拡大しない。


RaderChart::RaderChart(void)
  : m_createState(CREATE_STATE_NONE),
    m_pHeap(NULL),
    m_pAsyncFileManager(NULL),
    m_buf(NULL),
    m_resNode(NULL),
    m_model(NULL),
    m_pos(0.0f, 0.0f),
    m_insideRadius(1.0f),
    m_outsideRadius(1.0f),
    m_insideMaterialIndex(0),
    m_outsideMaterialIndex(0)
{
  for(u32 i=0; i<FIGURE_NUM; ++i)
  {
    for(u32 j=0; j<ITEM_NUM; ++j)
    {
      m_val[i][j] = 1.0f;
    }
  }
}

RaderChart::~RaderChart()
{
  if( !(this->CanDestroy()) )
  {
    // 破棄してはいけない状態のとき
    GFL_ASSERT(0);  // 破棄してはいけないときに破棄しようとしている。ケア済みのASSERT。
  }
  u32 loopCount = 0;
  while( !(this->CanDestroy()) )
  {
    this->Update();
    gfl2::thread::ThreadStatic::CurrentSleep(1);
    ++loopCount;
    if(loopCount >= 30*120)  // 無限ループ対策
    {
      GFL_ASSERT(0);  // 大分経過したが破棄できる状態になっていない。ケアしようがないASSERT。もう破棄しにいくしかない。
      break;
    }
  }

  // ここまで来たら、破棄してもいい状態になったので破棄する
  GFL_SAFE_DELETE(m_model);
  GFL_SAFE_DELETE(m_resNode);
  GflHeapSafeFreeMemory(m_buf);
}

void RaderChart::StartAsyncCreate(
    app::util::Heap*            pHeap,           
    gfl2::fs::AsyncFileManager* pAsyncFileManager
)
{
  if(m_createState == CREATE_STATE_NONE)
  {
    m_createState = CREATE_STATE_CREATING;
    
    {
      // ファイルを読み込みバッファに格納
      app::util::FileAccessor::FileOpen(NIJI_ARCID_NUM(ARCID_RADER_CHART), pHeap->GetDeviceHeap(), true);

      app::util::FileAccessor::FileLoad(
          NIJI_ARCID_NUM(ARCID_RADER_CHART),
          GARC_rader_chart_rader_chart_gfbmdlp_COMP,
          &m_buf,
          pHeap->GetDeviceHeap(),
          true,
          128
      );
      
      app::util::FileAccessor::FileClose(NIJI_ARCID_NUM(ARCID_RADER_CHART), pHeap->GetDeviceHeap());
    }

    m_pHeap             = pHeap;
    m_pAsyncFileManager = pAsyncFileManager;
  }
  else
  {
    GFL_ASSERT(0);  // 既に生成済み。開発中に気付かせるためのASSERT。
  }
}

void RaderChart::SyncCreate(
    app::util::Heap*            pHeap,           
    gfl2::fs::AsyncFileManager* pAsyncFileManager
)
{
  if(m_createState == CREATE_STATE_NONE)
  {
    m_createState = CREATE_STATE_CREATING;

    {
      // ファイルを読み込みバッファに格納
      app::util::FileAccessor::FileOpenSync(NIJI_ARCID_NUM(ARCID_RADER_CHART), pHeap->GetDeviceHeap(), true);

      app::util::FileAccessor::FileLoadSync(
          NIJI_ARCID_NUM(ARCID_RADER_CHART),
          GARC_rader_chart_rader_chart_gfbmdlp_COMP,
          &m_buf,
          pHeap->GetDeviceHeap(),
          true,
          128
      );
      
      app::util::FileAccessor::FileCloseSync(NIJI_ARCID_NUM(ARCID_RADER_CHART));
    }

    this->createModel(pHeap);

    m_createState = CREATE_STATE_CREATED;
  }
  else
  {
    GFL_ASSERT(0);  // 既に生成済み。開発中に気付かせるためのASSERT。
  }
}

bool RaderChart::IsCreated(void) const
{
  return ( m_createState == CREATE_STATE_CREATED );
}

bool RaderChart::CanDestroy(void) const
{
  //return ( m_createState == CREATE_STATE_NONE || m_createState == CREATE_STATE_CREATED );
  // @fix GFNMCat[5379] ボックス:ランダムキー入力で上画面終了処理時に描画参照のままというASSERTで停止  →「描画中のときは破棄できない」という判定をすることにした。

  if(m_createState == CREATE_STATE_NONE)
  {
    return true;
  }
  else if(m_createState == CREATE_STATE_CREATED)
  {
    if(m_model)
    {
      return m_model->CanDestroy();
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。m_createStateとm_modelの状態が合っていない。
      return true;  // ASSERTを抜けたら、m_modelがないので終了させることにする。
    }
  }
  else
  {
    return false;
  }
}

void RaderChart::Update(void)
{
  if(m_createState == CREATE_STATE_CREATING)
  {
    if(    app::util::FileAccessor::IsFileOpen( NIJI_ARCID_NUM(ARCID_RADER_CHART ) )
        && app::util::FileAccessor::IsFileLoad( &m_buf )
        && app::util::FileAccessor::IsFileClose( NIJI_ARCID_NUM(ARCID_RADER_CHART) ) )  // 発行したリクエスト全部見張る
    {
      this->createModel(m_pHeap);

      // もう使わないのでNULLを設定しておく
      m_pHeap             = NULL;
      m_pAsyncFileManager = NULL;
    
      m_createState = CREATE_STATE_CREATED;
    }
  }
  //////// ↓サイズ調整テスト↓ ////////
  /*
  else if(m_createState == CREATE_STATE_CREATED)
  {
    this->updateVal();
  }
  */
  //////// ↑サイズ調整テスト↑ ////////
}

void RaderChart::SetPos(const gfl2::math::Vector2& pos)
{
  m_pos = pos;
  gfl2::math::Vector3 pos3(m_pos.x, m_pos.y, 0.0f);
  m_model->SetPosition(pos3);

  {
    gfl2::renderingengine::scenegraph::instance::DrawableNode::UpdateDrawTagInfo info;
    m_model->GetModelInstanceNode()->UpdateDrawTag(info);
  }
}
const gfl2::math::Vector2& RaderChart::GetPos(void) const
{
  return m_pos;
}

void RaderChart::SetRadius(f32 insideRadius, f32 outsideRadius)
{
  m_insideRadius  = insideRadius;
  m_outsideRadius = outsideRadius;
  this->updateVal();
}

void RaderChart::SetColor(const gfl2::math::Vector4& insideColor, const gfl2::math::Vector4& outsideColor)
{
  m_model->SetMaterialColorConstant(m_insideMaterialIndex, 0, insideColor);
  m_model->SetMaterialColorConstant(m_outsideMaterialIndex, 0, outsideColor);
}

void RaderChart::SetVal(Figure fig, u32 item, f32 val)
{
  if(fig < FIGURE_NUM && item < ITEM_NUM)
  {
    f32 l_val = (val<0.0f)?(0.0f):((val>1.0f)?(1.0f):(val));
    m_val[fig][item] = l_val * ADJUST_RADIUS_TIMES;
    this->updateVal();
  }
  else
  {
    GFL_ASSERT(0);  // figかitemが値オーバー。  // 開発中に気付かせるためのASSERT
  }
}

void RaderChart::createModel(app::util::Heap* pHeap)
{
  // m_bufに読み込み済みのときに呼ぶこと。
  // m_resNodeとm_modelを生成し、初期値を反映します。

  {    
    // リソースノード
    gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack modelDataPack;
    modelDataPack.SetModelData(static_cast<c8*>(m_buf));
    m_resNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(pHeap->GetDevGLHeapAllocator(), &modelDataPack);
  }
  {
    // BaseModel
    m_model = GFL_NEW( pHeap->GetDeviceHeap() ) poke_3d::model::BaseModel;
    m_model->Create(pHeap->GetDevGLHeapAllocator(), pHeap->GetDeviceHeap(), m_resNode);
    //gfl2::renderingengine::scenegraph::SceneGraphManager::RemoveChild( m_model->GetModelInstanceNode() );  // Removeしていると描画されなくなってしまうのでコメントアウト

    GFL_ASSERT( m_model->GetMeshCount() == 2 );                            // 引っかかったら変なデータになってしまっている。開発中に気付かせるためのASSERT。
    GFL_ASSERT( m_model->GetModelInstanceNode()->GetDrawTagNum() == 2 );  // 引っかかったら変なデータになってしまっている。開発中に気付かせるためのASSERT。

    this->setupMaterialIndex();
  }

  // 初期値を反映
  this->updateVal();
}

void RaderChart::setupMaterialIndex(void)
{
  m_insideMaterialIndex  = m_model->GetMaterialIndex("Comb2");
  m_outsideMaterialIndex = m_model->GetMaterialIndex("Comb1");
  GFL_ASSERT(m_insideMaterialIndex>=0 && m_outsideMaterialIndex>=0);  // 開発中に気付かせるためのASSERT
}

void RaderChart::updateVal(void)
{
  // 単位方向ベクトル
  static const gfl2::math::Vector3 UNIT_DIR_VEC[ITEM_NUM] =
  {
    gfl2::math::Vector3(  1.0f,  0.0f  , 0.0f ),
    gfl2::math::Vector3(  0.5f, -0.866f, 0.0f ),
    gfl2::math::Vector3( -0.5f, -0.866f, 0.0f ),
    gfl2::math::Vector3( -1.0f,  0.0f  , 0.0f ),
    gfl2::math::Vector3( -0.5f,  0.866f, 0.0f ),
    gfl2::math::Vector3(  0.5f,  0.866f, 0.0f ),
  };

  gfl2::renderingengine::scenegraph::instance::JointInstanceNode* joint = NULL;

  /*
  // 確認用
  {
    u32 jointNum = m_model->GetModelInstanceNode()->GetJointNum();
    GFL_PRINT("joint num=%d\n", jointNum);
    for(u32 i=0; i<jointNum; ++i)
    {
      joint = m_model->GetModelInstanceNode()->GetJointInstanceNode(i);
      gfl2::math::Vector3 t = joint->GetLocalTranslate();
      GFL_PRINT("node=%d (%f, %f, %f)\n", i, t.x, t.y, t.z);
    }
  }
  */

  Figure fig;
  u32 item0;

  fig = FIGURE_INSIDE;
  item0 = 11;
  for(u32 i=0; i<ITEM_NUM; ++i)
  {
    joint = m_model->GetModelInstanceNode()->GetJointInstanceNode(item0+i);
    joint->SetLocalTranslate( UNIT_DIR_VEC[i] * m_insideRadius * m_val[fig][i] * GetAdjustVal(m_val,fig,i) );
  }

  fig = FIGURE_OUTSIDE;
  item0 = 3;
  for(u32 i=0; i<ITEM_NUM; ++i)
  {
    joint = m_model->GetModelInstanceNode()->GetJointInstanceNode(item0+i);
    joint->SetLocalTranslate( UNIT_DIR_VEC[i] * m_outsideRadius * m_val[fig][i] * GetAdjustVal(m_val,fig,i) );
  }

  {
    gfl2::renderingengine::scenegraph::instance::DrawableNode::UpdateDrawTagInfo info;
    m_model->GetModelInstanceNode()->UpdateDrawTag(info);
  }
}

f32 RaderChart::GetHermiteValue(f32 frame, const HermitePoint* hermitePointArray, u32 hermitePointNum)
{
  if(hermitePointNum == 0)
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。個数1以上を指定して下さい。
    return 0.0f;
  }
  
  f32 value = 0.0f;

  u32 endIndex = 0;
  while(endIndex < hermitePointNum)
  {
    if(frame <= hermitePointArray[endIndex].frame)
    {
      break;
    }
    else
    {
      ++endIndex;
    }
  }

  if(endIndex == 0)
  {
    // [0]より前の位置にある
    value = hermitePointArray[0].value;
  }
  else if(endIndex == hermitePointNum)
  {
    // [hermitePointNum-1]より後ろの位置にある
    value = hermitePointArray[hermitePointNum-1].value;
  }
  else
  {
    u32 startIndex = endIndex -1;
    const HermitePoint* startPoint = &(hermitePointArray[startIndex]);
    const HermitePoint* endPoint   = &(hermitePointArray[endIndex]);

    f32 t1 = frame - startPoint->frame;
    f32 t2 = 1.0f / (endPoint->frame - startPoint->frame);
    f32 v0 = startPoint->value;
    f32 v1 = endPoint->value;
    f32 s0 = startPoint->slopeNext;
    f32 s1 = endPoint->slopePrev;

    f32 t1t1t2 = t1 * t1 * t2;
    f32 t1t1t2t2 = t1t1t2 * t2;
    f32 t1t1t1t2t2 = t1 * t1t1t2t2;
    f32 t1t1t1t2t2t2 = t1t1t1t2t2 * t2;

    value =   v0 * (2.0f * t1t1t1t2t2t2 - 3.0f * t1t1t2t2 + 1.0f)
            + v1 * (-2.0f * t1t1t1t2t2t2 + 3.0f * t1t1t2t2)
            + s0 * (t1t1t1t2t2 - 2.0f * t1t1t2 + t1)
            + s1 * (t1t1t1t2t2 - t1t1t2);
  }

  return value;
}


GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

