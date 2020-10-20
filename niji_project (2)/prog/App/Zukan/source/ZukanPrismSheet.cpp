//==============================================================================
/**
 @file    ZukanPrismSheet.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2016.02.20
 @brief   図鑑のキラカード
 */
//==============================================================================


// gfl2のインクルード
#ifdef GFL2_UI_USE_GYROSCOPE
#include <ui/include/gfl2_UI_Gyroscope.h>
#endif

// nijiのインクルード
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/app_util_2d.h>

// nijiのデータのヘッダーファイルのインクルード
#include <niji_conv_header/app/zukan/alola_top.h>
#include <niji_conv_header/app/zukan/alola_top_pane.h>
#include <niji_conv_header/app/zukan/alola_top_anim_list.h>

// 図鑑のインクルード
#include "ZukanDefine.h"
#include "ZukanPrismParam.h"

#include "ZukanPrismSheet.h"




#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE


#define NIJI_PROJECT_APP_ZUKAN_ZUKANPRISMSHEET_20160307 (1)  // これが1のとき、2016.03.07のコードになる


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
//
//  ZukanPrismSheet
//
//==============================================================================

const s32 ZukanPrismSheet::BLOCK_X_NUM = 7;  //=400/64;
const s32 ZukanPrismSheet::BLOCK_Y_NUM = 4;  //=240/64;
const gfl2::math::VEC2 ZukanPrismSheet::ORIGIN_BLOCK_POS = gfl2::math::VEC2(
    -(64/2-(ZukanPrismSheet::BLOCK_X_NUM*64-400)/2) -400/2,
    -(64/2-(ZukanPrismSheet::BLOCK_Y_NUM*64-240)/2) +240/2
);  // 起点となるblockを置く位置(layout座標)(一番左上に来るblockの位置)
const gfl2::math::VEC2 ZukanPrismSheet::BLOCK_SIZE = gfl2::math::VEC2(64.0f, 64.0f);  // pieceの辺を合計した長さ
const gfl2::math::VEC2 ZukanPrismSheet::PIECE_SIZE = gfl2::math::VEC2(32.0f, 32.0f);  // pieceの辺の長さ

const s32 ZukanPrismSheet::GYRO_INIT_COUNT_MAX = 30;  // ジャイロ起動中のカウント

const s32 ZukanPrismSheet::GYRO_LOG_NUM_MAX = 10;  // ジャイロ稼働中の角度のログ


ZukanPrismSheet::ZukanPrismSheet(
  app::util::Heap*          appHeap,
  gfl2::lyt::LytWk*         pLytWk,
  u32                       lytWkId
)
  : m_appHeap(appHeap),
    m_pLytWk(pLytWk),
    m_lytWkId(lytWkId),
    m_canDestroyFlag(false),
    m_rootPane(NULL),
    m_blockTopPane(NULL),
    m_originalBlock(),
    m_block(NULL),
    m_gyroCreateFlag(false),
    m_gyroState(GYRO_STATE_NONE),
    m_gyroInitCount(0),
    m_prismState(PRISM_STATE_OFF),
    m_prismFadeRate(0.0f),
    m_reqIsOn(false),
    m_reduceState(REDUCE_STATE_STOP),
    m_reduceStateCount(0),
    m_reduceRate(1.0f),
    m_gyroAngle(NULL),
    m_gyroAngleValid(NULL),
    m_gyroAngleNum(0),
    m_vertexXNum(0),  // kawa_test2
    m_vertexYNum(0),
    m_vertexColor(NULL),
    m_alphaValDrawPane(NULL),
    m_bgPane(NULL)
{
  this->createBlock();
  this->initGyro();
}
ZukanPrismSheet::~ZukanPrismSheet()
{
  GFL_ASSERT(this->CanDestroy());  // 開発中に気付かせるためのASSERT。CanDestroyがtrueを返してから破棄して下さい。
  this->termGyro();
  this->destroyBlock();
}
bool ZukanPrismSheet::CanDestroy(void) const
{
#ifdef GFL2_UI_USE_GYROSCOPE
  return m_canDestroyFlag;
#else
  return true;
#endif
}

void ZukanPrismSheet::Update(void)
{
  this->updateGyro();

  if(m_gyroState == GYRO_STATE_WORK)
  {
    this->getGyroAngle();
    this->updatePrism();
  }
}
 
void ZukanPrismSheet::SetVisible(bool bVisible)
{
  m_rootPane->SetVisible(bVisible);
}

void ZukanPrismSheet::Req(ReqId reqId)
{
#ifdef GFL2_UI_USE_GYROSCOPE
  // リクエストを処理する
  if(reqId == REQ_ID_PRISM_ON)
  {
    m_reqIsOn = true;
  }
  else if(reqId == REQ_ID_PRISM_OFF)
  {
    m_reqIsOn = false;
    // REQ_ID_PRISM_OFFのときは直ちにキラ状態をPRISM_STATE_ON_OUTに変更し、必ず最後まで処理する
    if(m_prismState == PRISM_STATE_ON_IN || m_prismState == PRISM_STATE_ON_WORK)
    {
      m_prismState = PRISM_STATE_ON_OUT;
    }
  }
#endif
}

void ZukanPrismSheet::createBlock(void)
{
  // オリジナルペイン
  m_rootPane = m_pLytWk->GetPane(PANENAME_PRISM_SHEET_PANE_ROOT);
  m_blockTopPane = m_pLytWk->GetPane(PANENAME_PRISM_SHEET_PANE_BLOCK_TOP);
  m_originalBlock.blockPane = m_pLytWk->GetPane(PANENAME_PRISM_SHEET_PANE_BLOCK);
  static const u32 ORIGINAL_PIECE_PANE_ID[PIECE_POS_TYPE_NUM] =
  {
    PANENAME_PRISM_SHEET_PANE_PIECE_RT,
    PANENAME_PRISM_SHEET_PANE_PIECE_RB,
    PANENAME_PRISM_SHEET_PANE_PIECE_LB,
    PANENAME_PRISM_SHEET_PANE_PIECE_LT,
  };
  for(s32 i=0; i<PIECE_POS_TYPE_NUM; ++i)
  {
    m_originalBlock.piecePane[i] = m_pLytWk->GetPicturePane(ORIGINAL_PIECE_PANE_ID[i]);
  } 
  gfl2::math::VEC3 originalBlockPanePos = m_originalBlock.blockPane->GetTranslate();

  // プログラム作成ペイン
#if defined(GF_PLATFORM_CTR)
  nw::os::IAllocator* prevAllocator             = nw::lyt::Layout::GetAllocator();
  nw::os::IAllocator* prevDeviceMemoryAllocator = nw::lyt::Layout::GetDeviceMemoryAllocator();
  nw::lyt::Layout::SetAllocator(m_appHeap->GetDeviceAllocator());
  nw::lyt::Layout::SetDeviceMemoryAllocator(m_appHeap->GetDeviceAllocator());
#elif defined(GF_PLATFORM_WIN32)
  nw::ut::IAllocator* prevAllocator             = nw::lyt::Layout::GetAllocator();
  nw::lyt::Layout::SetAllocator(m_appHeap->GetDeviceAllocator());
#endif  // GF_PLATFORM_

  m_block = GFL_NEW_ARRAY(m_appHeap->GetDeviceHeap()) Block*[BLOCK_X_NUM];
  for(s32 x=0; x<BLOCK_X_NUM; ++x)
  {
    m_block[x] = GFL_NEW_ARRAY(m_appHeap->GetDeviceHeap()) Block[BLOCK_Y_NUM];
  }
  for(s32 x=0; x<BLOCK_X_NUM; ++x)
  {
    for(s32 y=0; y<BLOCK_Y_NUM; ++y)
    {
      m_block[x][y].blockPane = GFL_NEW(m_appHeap->GetDeviceHeap()) nw::lyt::Pane(*m_originalBlock.blockPane);
      m_blockTopPane->AppendChild(m_block[x][y].blockPane);
      for(s32 i=0; i<PIECE_POS_TYPE_NUM; ++i)
      {
        m_block[x][y].piecePane[i] = GFL_NEW(m_appHeap->GetDeviceHeap()) nw::lyt::Picture(*(m_originalBlock.piecePane[i]));
        m_block[x][y].blockPane->AppendChild(m_block[x][y].piecePane[i]);
      }
      m_block[x][y].blockPane->SetTranslate( gfl2::math::VEC3(ORIGIN_BLOCK_POS.x+BLOCK_SIZE.x*x, ORIGIN_BLOCK_POS.y-BLOCK_SIZE.y*y, originalBlockPanePos.z) );
    }
  }
  
#if defined(GF_PLATFORM_CTR)
  nw::lyt::Layout::SetAllocator(prevAllocator);
  nw::lyt::Layout::SetDeviceMemoryAllocator(prevDeviceMemoryAllocator);
#elif defined(GF_PLATFORM_WIN32)
  nw::lyt::Layout::SetAllocator(prevAllocator);
#endif  // GF_PLATFORM_

  // オリジナルペインは使わない
  m_originalBlock.blockPane->SetVisible(false);

  // 初期カラー
  for(s32 x=0; x<BLOCK_X_NUM; ++x)
  {
    for(s32 y=0; y<BLOCK_Y_NUM; ++y)
    {
      for(s32 i=0; i<PIECE_POS_TYPE_NUM; ++i)
      {
        for(u32 idx=0; idx<nw::lyt::VERTEXCOLOR_MAX; ++idx)
        {
          m_block[x][y].piecePane[i]->SetVtxColor(idx, nw::ut::Color8::BLACK);  // (0,0,0,255)
        }
      }
    }
  }

  // kawa_test
  {
    // 回転
    //m_blockTopPane->SetRotate(gfl2::math::VEC3(0, 0, 55));
    //m_blockTopPane->SetRotate(gfl2::math::VEC3(0, 0, 225));
  }

  // kawa_test2
  {
    m_vertexXNum = BLOCK_X_NUM* 2 +1;
    m_vertexYNum = BLOCK_Y_NUM* 2 +1;
    m_vertexColor = GFL_NEW_ARRAY(m_appHeap->GetDeviceHeap()) nw::ut::Color8*[m_vertexXNum];
    for(s32 x=0; x<m_vertexXNum; ++x)
    {
      m_vertexColor[x] = GFL_NEW_ARRAY(m_appHeap->GetDeviceHeap()) nw::ut::Color8[m_vertexYNum];
      for(s32 y=0; y<m_vertexYNum; ++y)
      {
        m_vertexColor[x][y] = nw::ut::Color8::BLACK;  // (0,0,0,255)
      }
    }
  }

  {
    m_alphaValDrawPane = m_pLytWk->GetPicturePane(PANENAME_PRISM_SHEET_PANE_ALPHA_VAL_DRAW);
    //描画順を変更したのでコメントアウト。背景より手前にあるペインにキラキラを載せるのはあきらめた。
    //nw::ut::Color8 color = nw::ut::Color8(255,255,255,128);  // RGBは何でもいい。
          // Aで「背景より手前にあるペインにどれだけキラキラを載せるか」を調整する。
          // 255だとキラキラが載らず、128だとキラキラが半分載り、0だとキラキラが完全に載る。
    //for(u32 idx=0; idx<nw::lyt::VERTEXCOLOR_MAX; ++idx)
    //{
    //  m_alphaValDrawPane->SetVtxColor(idx, color);
    //}


    m_bgPane = m_pLytWk->GetPicturePane(PANENAME_PRISM_SHEET_PANE_BG);
#if NIJI_PROJECT_APP_ZUKAN_ZUKANPRISMSHEET_20160307
    for(u32 idx=0; idx<nw::lyt::VERTEXCOLOR_MAX; ++idx)
    {
      m_bgPane->SetVtxColor(idx, nw::ut::Color8::BLACK);
    }
#else  // NIJI_PROJECT_APP_ZUKAN_ZUKANPRISMSHEET_20160307
    for(u32 idx=0; idx<nw::lyt::VERTEXCOLOR_MAX; ++idx)
    {
      f32 alpha = (80)/255.0f;
      nw::ut::Color8 color = nw::ut::Color8(128,128,128,255);
      if(idx==0)      color.r = 255;
      else if(idx==1) color.g = 255;
      else if(idx==2) color.b = 255;
      else if(idx==3) color.r = color.g = 255;
      color.r = color.r * alpha;
      color.g = color.g * alpha;
      color.b = color.b * alpha;
      m_bgPane->SetVtxColor(idx, color);
    }
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANPRISMSHEET_20160307
  }
}

void ZukanPrismSheet::destroyBlock(void)
{
  // kawa_test2
  {
    for(s32 x=0; x<m_vertexXNum; ++x)
    {
      GFL_SAFE_DELETE_ARRAY(m_vertexColor[x]);
    }
    GFL_SAFE_DELETE_ARRAY(m_vertexColor);
  }
  
  // プログラム作成ペインの破棄
#if defined(GF_PLATFORM_CTR)
  nw::os::IAllocator* prevAllocator             = nw::lyt::Layout::GetAllocator();
  nw::os::IAllocator* prevDeviceMemoryAllocator = nw::lyt::Layout::GetDeviceMemoryAllocator();
  nw::lyt::Layout::SetAllocator(m_appHeap->GetDeviceAllocator());
  nw::lyt::Layout::SetDeviceMemoryAllocator(m_appHeap->GetDeviceAllocator());
#elif defined(GF_PLATFORM_WIN32)
  nw::ut::IAllocator* prevAllocator             = nw::lyt::Layout::GetAllocator();
  nw::lyt::Layout::SetAllocator(m_appHeap->GetDeviceAllocator());
#endif  // GF_PLATFORM_

  for(s32 x=0; x<BLOCK_X_NUM; ++x)
  {
    for(s32 y=0; y<BLOCK_Y_NUM; ++y)
    {
      for(s32 i=0; i<PIECE_POS_TYPE_NUM; ++i)
      {
        m_block[x][y].blockPane->RemoveChild(m_block[x][y].piecePane[i]);
        GFL_SAFE_DELETE(m_block[x][y].piecePane[i]);
      }
      m_blockTopPane->RemoveChild(m_block[x][y].blockPane);
      GFL_SAFE_DELETE(m_block[x][y].blockPane);
    }
  }
  for(s32 x=0; x<BLOCK_X_NUM; ++x)
  {
    GFL_SAFE_DELETE_ARRAY(m_block[x]);
  }
  GFL_SAFE_DELETE_ARRAY(m_block);

#if defined(GF_PLATFORM_CTR)
  nw::lyt::Layout::SetAllocator(prevAllocator);
  nw::lyt::Layout::SetDeviceMemoryAllocator(prevDeviceMemoryAllocator);
#elif defined(GF_PLATFORM_WIN32)
  nw::lyt::Layout::SetAllocator(prevAllocator);
#endif
}

void ZukanPrismSheet::initGyro(void)
{
#ifdef GFL2_UI_USE_GYROSCOPE
  gfl2::ui::DeviceManager* uiDeviceManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
  gfl2::ui::Gyroscope* gyro = uiDeviceManager->GetGyroscope(gfl2::ui::DeviceManager::GYROSCOPE_STANDARD);
  m_gyroCreateFlag = (gyro == NULL);
  GFL_PRINT("Gyro: m_gyroCreateFlag=%d\n", m_gyroCreateFlag); 
  if(m_gyroCreateFlag)
  {
    uiDeviceManager->BootGyroscope(m_appHeap->GetDeviceHeap(), gfl2::ui::DeviceManager::GYROSCOPE_STANDARD);  // ブート後、数フレームはgyro->IsValid()がfalseを返す。
  } 
  m_canDestroyFlag = false;
  m_gyroInitCount = 0;
  m_gyroState = GYRO_STATE_INIT;

  m_gyroAngle      = GFL_NEW_ARRAY(m_appHeap->GetDeviceHeap()) gfl2::math::VEC3[GYRO_LOG_NUM_MAX];
  m_gyroAngleValid = GFL_NEW_ARRAY(m_appHeap->GetDeviceHeap()) bool[GYRO_LOG_NUM_MAX];
  for(s32 i=0; i<GYRO_LOG_NUM_MAX; ++i)
  {
    m_gyroAngle[i] = gfl2::math::VEC3(0,0,0);
    m_gyroAngleValid[i] = false;
  }
  m_gyroAngleNum = 0;
#endif
}

void ZukanPrismSheet::termGyro(void)
{
#ifdef GFL2_UI_USE_GYROSCOPE
  gfl2::ui::DeviceManager* uiDeviceManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
  if(m_gyroCreateFlag)
  {
    uiDeviceManager->StopGyroscope(gfl2::ui::DeviceManager::GYROSCOPE_STANDARD);
  }
  m_gyroCreateFlag = false;
  m_gyroState = GYRO_STATE_NONE;

  GFL_SAFE_DELETE_ARRAY(m_gyroAngleValid);
  GFL_SAFE_DELETE_ARRAY(m_gyroAngle);
#endif
}

void ZukanPrismSheet::updateGyro(void)
{
#ifdef GFL2_UI_USE_GYROSCOPE
  gfl2::ui::DeviceManager* uiDeviceManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
  gfl2::ui::Gyroscope* gyro = uiDeviceManager->GetGyroscope(gfl2::ui::DeviceManager::GYROSCOPE_STANDARD);
 
  if(m_gyroState == GYRO_STATE_INIT)
  {
    if(gyro->IsValid())
    {
      GFL_PRINT("Gyro: IsValid, m_gyroInitCount=%d\n", m_gyroInitCount);
      gyro->SetAxisRotationMatrixFromCurrentDirection();
      m_canDestroyFlag = true;  // 念のためジャイロが有効になるまで破棄しないようにするためのフラグ。
      m_gyroState = GYRO_STATE_WORK;
    }
    else
    {
      ++m_gyroInitCount;
      if(m_gyroInitCount >= GYRO_INIT_COUNT_MAX)  // いつまで経ってもジャイロが有効にならないときは、破棄を許可しておく。
      {
        m_canDestroyFlag = true;
      }
    }
  }
#endif
}

void ZukanPrismSheet::updatePrism(void)
{
#ifdef GFL2_UI_USE_GYROSCOPE
  //static const nw::lyt::VertexColor VERTEX_COLOR_INDEX[PIECE_POS_TYPE_NUM] =
  //{
  //  nw::lyt::VERTEXCOLOR_RT,  // blockの中心から最も遠い縦横斜め方向にある頂点
  //  nw::lyt::VERTEXCOLOR_RB,  // blockの中心から横方向にある頂点
  //  nw::lyt::VERTEXCOLOR_LB,  // blockの中心にある頂点
  //  nw::lyt::VERTEXCOLOR_LT,  // blockの中心から縦方向にある頂点
  //};

  static const f32 PRISM_FADE_SPEED = 0.03334f*5.0f;  // 1/30[color/frame]のx倍

  // キラ状態を遷移する
  GyroMoveRatio gyroMoveRatio = GYRO_MOVE_RATIO_MOVE_BIG;  // ずっとキラキラを表示しっぱなしにするため、必ずGYRO_MOVE_RATIO_MOVE_BIGを入れておく。
  switch(m_prismState)
  {
  case PRISM_STATE_OFF:
    {
      if(m_reqIsOn)
      {
        // 動いたならINへ
        if(gyroMoveRatio == GYRO_MOVE_RATIO_MOVE_SMALL || gyroMoveRatio == GYRO_MOVE_RATIO_MOVE_BIG)
        {
          m_prismState =  PRISM_STATE_ON_IN;
        }
      }
      break;
    }
  case PRISM_STATE_ON_IN:
    {
      // 動いているなら継続
      //if(gyroMoveRatio == GYRO_MOVE_RATIO_MOVE_BIG)  // 画面がちらつくのでコメントアウトし、最後まで処理することにした。1/2
      {
        m_prismFadeRate += PRISM_FADE_SPEED;
        if(m_prismFadeRate >= 1.0f)
        {
          m_prismFadeRate = 1.0f;
          m_prismState = PRISM_STATE_ON_WORK;
        }
      }
      //// 動いていないならOUTへ  // しかし、外部からのリクエストでOFFにするときは、この状態から直ちにPRISM_STATE_ON_OUTにされる。2/2
      //else                    
      //{
      //  m_prismState = PRISM_STATE_ON_OUT;
      //}
      break;
    }
  case PRISM_STATE_ON_WORK:
    {
      // 動いているなら継続
      if(gyroMoveRatio == GYRO_MOVE_RATIO_MOVE_BIG)
      {
        // このまま継続。何もしない。
      }
      // 動いていないならOUTへ
      else
      {
        m_prismState = PRISM_STATE_ON_OUT;
      }
      break;
    }
  case PRISM_STATE_ON_OUT:
    {
      // 動きに関係なく、必ず最後まで処理する
      m_prismFadeRate -= PRISM_FADE_SPEED;
      if(m_prismFadeRate <= 0.0f)
      {
        m_prismFadeRate = 0.0f;
        m_prismState = PRISM_STATE_OFF;
      }
      break;
    }
  }

  // 明るさ減衰
  {
#if NIJI_APP_ZUKAN_PRISM_DEVELOP
    f32 REDUCE_SPEED_FROM_DARK = ZukanPrismDevelopParam::s_reduceParam.reduceSpeedFromDark / ZukanPrismDevelopParam::BAIRITSU / 100.0f / 30.0f;  // 1フレーム(1/30秒)の色変化量(0(暗い)～1(明るい))
    f32 REDUCE_SPEED_TO_DARK   = ZukanPrismDevelopParam::s_reduceParam.reduceSpeedToDark   / ZukanPrismDevelopParam::BAIRITSU / 100.0f / 30.0f;
    f32 REDUCE_RATE_SMALL      = ZukanPrismDevelopParam::s_reduceParam.reduceRateSmall / ZukanPrismDevelopParam::BAIRITSU / 100.0f;  // 色(0(暗い)～1(明るい))
    f32 REDUCE_RATE_BIG        = ZukanPrismDevelopParam::s_reduceParam.reduceRateBig   / ZukanPrismDevelopParam::BAIRITSU / 100.0f;

    GyroMoveRatio gyroMoveRatioForReduce = this->getGyroMoveRatio();  // 明るさを減衰させるのに使う。
  
    s32 rateDir = 0;  // 正のとき明るくする、負のとき暗くする
    f32 goalReduceRate = 0.0f;
    switch(gyroMoveRatioForReduce)
    {
    case GYRO_MOVE_RATIO_STOP:
      {
        goalReduceRate = 1.0f;
        if( m_reduceRate < 1.0f ) rateDir = 1;
        break;
      }
    case GYRO_MOVE_RATIO_MOVE_SMALL:
      {
        goalReduceRate = REDUCE_RATE_SMALL;
        if( m_reduceRate < REDUCE_RATE_SMALL ) rateDir = 1;
        else if( m_reduceRate > REDUCE_RATE_SMALL ) rateDir = -1;
        break;
      }
    case GYRO_MOVE_RATIO_MOVE_BIG:
      {
        goalReduceRate = REDUCE_RATE_BIG;
        if( m_reduceRate > REDUCE_RATE_BIG ) rateDir = -1;
        break;
      }
    }

    if( rateDir > 0 )
    {
      m_reduceRate += REDUCE_SPEED_FROM_DARK;
      if( m_reduceRate > goalReduceRate ) m_reduceRate = goalReduceRate;
    }
    else if( rateDir < 0 )
    {
      m_reduceRate -= REDUCE_SPEED_TO_DARK;
      if( m_reduceRate < goalReduceRate ) m_reduceRate = goalReduceRate;
    }
#else  //  NIJI_APP_ZUKAN_PRISM_DEVELOP
    static const f32 REDUCE_SPEED_TO_STOP   = 0.03334f*0.7f;  // 1/30[color/frame]のx倍
    static const f32 REDUCE_SPEED_TO_MOVE   = 0.03334f*0.5f;  // 1/30[color/frame]のx倍
    static const f32 REDUCE_RATE_MIN        = 0.3f;
    static const s32 REDUCE_STATE_COUNT_MAX = 30*10;

    GyroMoveRatio gyroMoveRatioForReduce = this->getGyroMoveRatio();  // 明るさを減衰させるのに使う。
    
    ++m_reduceStateCount;
    if(m_reduceStateCount > REDUCE_STATE_COUNT_MAX)
    {
      m_reduceStateCount = REDUCE_STATE_COUNT_MAX;
    }

    switch(m_reduceState)
    {
    case REDUCE_STATE_STOP:
      {
        // 停止中から動き始めへの変化は、滞在時間は気にしない。
        if(gyroMoveRatioForReduce == GYRO_MOVE_RATIO_MOVE_SMALL || gyroMoveRatioForReduce == GYRO_MOVE_RATIO_MOVE_BIG)
        {
          this->changeReduceState(REDUCE_STATE_STOP_TO_MOVE);
        }
        break;
      }
    case REDUCE_STATE_STOP_TO_MOVE:
      {
        // 動き始めたら最後まで到達させる。
        m_reduceRate -= REDUCE_SPEED_TO_MOVE;
        if(m_reduceRate <= REDUCE_RATE_MIN)  // 最小値0.3f
        {
          m_reduceRate = REDUCE_RATE_MIN;
          this->changeReduceState(REDUCE_STATE_MOVE);
        }
        break;
      }
    case REDUCE_STATE_MOVE:
      {
        // 動いている状態には最低滞在時間を設ける
        if(m_reduceStateCount >= 30*2)
        {
          if(gyroMoveRatioForReduce == GYRO_MOVE_RATIO_STOP)
          {
            this->changeReduceState(REDUCE_STATE_MOVE_TO_STOP);
          }
        }
        break;
      }
    case REDUCE_STATE_MOVE_TO_STOP:
      {
        // 動き始めたら、状態を変える。最後まで待たない。
        m_reduceRate += REDUCE_SPEED_TO_STOP;
        if(m_reduceRate >= 1.0f)  // 最大値1.0f
        {
          m_reduceRate = 1.0f;
          this->changeReduceState(REDUCE_STATE_STOP);
        }
        else if(gyroMoveRatioForReduce == GYRO_MOVE_RATIO_MOVE_SMALL || gyroMoveRatioForReduce == GYRO_MOVE_RATIO_MOVE_BIG)
        {
          this->changeReduceState(REDUCE_STATE_STOP_TO_MOVE);
        }
        break;
      }
    } 
#endif  //  NIJI_APP_ZUKAN_PRISM_DEVELOP
  }

  // 頂点カラー
  nw::ut::Color8 vertexColor;
#if NIJI_PROJECT_APP_ZUKAN_ZUKANPRISMSHEET_20160307
  if(m_gyroAngleValid[0])
  {
#if NIJI_APP_ZUKAN_PRISM_DEVELOP
    s32 targetPlace = 0;
    s32 targetColor = 0;
 
    //gfl2::math::VEC3 angleDegree;
    //angleDegree.x = normalizeAngle(m_gyroAngle[0].x) * 360.0f;  // ジャイロが動いたかの判定のときには0以上1未満におさめてはならないが(勝手に変更すると0.98から0になったときに動いたと判定されてしまうから)、
    //angleDegree.y = normalizeAngle(m_gyroAngle[0].y) * 360.0f;  // 平均値を求める前にはおさめておかないと大きい値に引っ張られ過ぎることになりそうと思ったが
    //angleDegree.z = normalizeAngle(m_gyroAngle[0].z) * 360.0f;  // この後角度にしているから引っ張られ過ぎることはないかもしれない。
    //f32 angleDegreeAverage = (angleDegree.x + angleDegree.y + angleDegree.z) / 3.0f;

    gfl2::math::VEC3 angleDegree;
    angleDegree.x = m_gyroAngle[0].x * 360.0f;  // 0以上1未満にはおさめない。おさめると359度と361度の平均が360度にならず、359度と1度の平均180度になってしまうから。
    angleDegree.y = m_gyroAngle[0].y * 360.0f;  // この後も角度としてしか使っていないので、大き過ぎて困るということはない。
    angleDegree.z = m_gyroAngle[0].z * 360.0f;  // 平均が大き過ぎる角度に引っ張られるからもしれないが、それはそれであきらめる。
    f32 angleDegreeAverage = (angleDegree.x + angleDegree.y + angleDegree.z) / 3.0f;

#if PM_DEBUG
    if(ZukanPrismDevelopParam::s_gyroAnglePrintFlag != 0)
    {
      GFL_PRINT("(%f, %f, %f) Ave%f; Reduce%f\n", angleDegree.x, angleDegree.y, angleDegree.z, angleDegreeAverage, m_reduceRate);
    }
#endif  // PM_DEBUG

    //// 格子 ////
    f32 val = 0.0f;
    f32 valR = 0.0f;
    f32 valG = 0.0f;
    f32 valB = 0.0f;
   
    for(s32 x=0; x<m_vertexXNum; ++x)
    {
      for(s32 y=0; y<m_vertexYNum; ++y)
      {
        if( this->isVertexPosBlockCenter(x, y) )
        {
          m_vertexColor[x][y].r = calcVertexColor(ZukanPrismDevelopParam::TARGET_PLACE_GRID_CENTER, ZukanPrismDevelopParam::TARGET_COLOR_R, x, y, angleDegree, angleDegreeAverage) * m_prismFadeRate * m_reduceRate;
          m_vertexColor[x][y].g = calcVertexColor(ZukanPrismDevelopParam::TARGET_PLACE_GRID_CENTER, ZukanPrismDevelopParam::TARGET_COLOR_G, x, y, angleDegree, angleDegreeAverage) * m_prismFadeRate * m_reduceRate;
          m_vertexColor[x][y].b = calcVertexColor(ZukanPrismDevelopParam::TARGET_PLACE_GRID_CENTER, ZukanPrismDevelopParam::TARGET_COLOR_B, x, y, angleDegree, angleDegreeAverage) * m_prismFadeRate * m_reduceRate;
        }
        else
        {
          m_vertexColor[x][y].r = calcVertexColor(ZukanPrismDevelopParam::TARGET_PLACE_GRID_CORNER, ZukanPrismDevelopParam::TARGET_COLOR_R, x, y, angleDegree, angleDegreeAverage) * m_prismFadeRate * m_reduceRate;
          m_vertexColor[x][y].g = calcVertexColor(ZukanPrismDevelopParam::TARGET_PLACE_GRID_CORNER, ZukanPrismDevelopParam::TARGET_COLOR_G, x, y, angleDegree, angleDegreeAverage) * m_prismFadeRate * m_reduceRate;
          m_vertexColor[x][y].b = calcVertexColor(ZukanPrismDevelopParam::TARGET_PLACE_GRID_CORNER, ZukanPrismDevelopParam::TARGET_COLOR_B, x, y, angleDegree, angleDegreeAverage) * m_prismFadeRate * m_reduceRate;
        }
      }
    }

    s32 vertexX = 0;
    s32 vertexY = 0;
    for(s32 x=0; x<BLOCK_X_NUM; ++x)
    {
      for(s32 y=0; y<BLOCK_Y_NUM; ++y)
      {
        for(s32 i=0; i<PIECE_POS_TYPE_NUM; ++i)
        {
          for(u32 idx=0; idx<nw::lyt::VERTEXCOLOR_MAX; ++idx)
          {
            this->getVertexPos(&vertexX, &vertexY, x, y, i, idx);
            m_block[x][y].piecePane[i]->SetVtxColor(idx, m_vertexColor[vertexX][vertexY]);
          }
        }
      }
    }

    //// BG ////
    for(u32 idx=0; idx<nw::lyt::VERTEXCOLOR_MAX; ++idx)
    {
      switch(idx)
      {
      case nw::lyt::VERTEXCOLOR_RT: targetPlace = ZukanPrismDevelopParam::TARGET_PLACE_BG_RT; break;
      case nw::lyt::VERTEXCOLOR_RB: targetPlace = ZukanPrismDevelopParam::TARGET_PLACE_BG_RB; break;
      case nw::lyt::VERTEXCOLOR_LB: targetPlace = ZukanPrismDevelopParam::TARGET_PLACE_BG_LB; break;
      case nw::lyt::VERTEXCOLOR_LT: targetPlace = ZukanPrismDevelopParam::TARGET_PLACE_BG_LT; break;
      }
      vertexColor.r = calcVertexColor(targetPlace, ZukanPrismDevelopParam::TARGET_COLOR_R, 0, 0, angleDegree, angleDegreeAverage) * m_prismFadeRate * m_reduceRate;
      vertexColor.g = calcVertexColor(targetPlace, ZukanPrismDevelopParam::TARGET_COLOR_G, 0, 0, angleDegree, angleDegreeAverage) * m_prismFadeRate * m_reduceRate;
      vertexColor.b = calcVertexColor(targetPlace, ZukanPrismDevelopParam::TARGET_COLOR_B, 0, 0, angleDegree, angleDegreeAverage) * m_prismFadeRate * m_reduceRate;
      vertexColor.a = 255;
      m_bgPane->SetVtxColor(idx, vertexColor);
    }
#else  // NIJI_APP_ZUKAN_PRISM_DEVELOP
    //// 格子 ////
    for(s32 x=0; x<BLOCK_X_NUM; ++x)
    {
      for(s32 y=0; y<BLOCK_Y_NUM; ++y)
      {
        for(s32 i=0; i<PIECE_POS_TYPE_NUM; ++i)
        {
          for(u32 idx=0; idx<nw::lyt::VERTEXCOLOR_MAX; ++idx)
          {
            vertexColor = m_block[x][y].piecePane[i]->GetVtxColor(idx);

            vertexColor.r = vertexColor.r * m_prismFadeRate * m_reduceRate;
            vertexColor.g = vertexColor.g * m_prismFadeRate * m_reduceRate;
            vertexColor.b = vertexColor.b * m_prismFadeRate * m_reduceRate;
            vertexColor.a = 255;
          
            m_block[x][y].piecePane[i]->SetVtxColor(idx, vertexColor);
          }
        }
      }
    }

    //// BG ////
    for(u32 idx=0; idx<nw::lyt::VERTEXCOLOR_MAX; ++idx)
    {
      vertexColor = m_bgPane->GetVtxColor(idx);
      vertexColor.r = vertexColor.r * m_prismFadeRate * m_reduceRate;
      vertexColor.g = vertexColor.g * m_prismFadeRate * m_reduceRate;
      vertexColor.b = vertexColor.b * m_prismFadeRate * m_reduceRate;
      vertexColor.a = 255;
      m_bgPane->SetVtxColor(idx, vertexColor);
    }
#endif  // NIJI_APP_ZUKAN_PRISM_DEVELOP
  }
  else
  {
    //// 格子 ////
    for(s32 x=0; x<BLOCK_X_NUM; ++x)
    {
      for(s32 y=0; y<BLOCK_Y_NUM; ++y)
      {
        for(s32 i=0; i<PIECE_POS_TYPE_NUM; ++i)
        {
          for(u32 idx=0; idx<nw::lyt::VERTEXCOLOR_MAX; ++idx)
          {
            vertexColor = m_block[x][y].piecePane[i]->GetVtxColor(idx);

            vertexColor.r = vertexColor.r * m_prismFadeRate * m_reduceRate;
            vertexColor.g = vertexColor.g * m_prismFadeRate * m_reduceRate;
            vertexColor.b = vertexColor.b * m_prismFadeRate * m_reduceRate;
            vertexColor.a = 255;
          
            m_block[x][y].piecePane[i]->SetVtxColor(idx, vertexColor);
          }
        }
      }
    }

    //// BG ////
    for(u32 idx=0; idx<nw::lyt::VERTEXCOLOR_MAX; ++idx)
    {
      vertexColor = m_bgPane->GetVtxColor(idx);
      vertexColor.r = vertexColor.r * m_prismFadeRate * m_reduceRate;
      vertexColor.g = vertexColor.g * m_prismFadeRate * m_reduceRate;
      vertexColor.b = vertexColor.b * m_prismFadeRate * m_reduceRate;
      vertexColor.a = 255;
      m_bgPane->SetVtxColor(idx, vertexColor);
    }
  }
#else  // NIJI_PROJECT_APP_ZUKAN_ZUKANPRISMSHEET_20160307
  if(m_gyroAngleValid[0])
  {
    /*
    gfl2::math::VEC3 normalizedAngle = m_gyroAngle[0];
    //GFL_PRINT("Gyro: angle[degree] pitch%f, yaw=%f, roll=%f\n", normalizedAngle.x*360.0f, normalizedAngle.y*360.0f, normalizedAngle.z*360.0f);
    while(normalizedAngle.x <  0.0f) normalizedAngle.x += 1.0f;  while(normalizedAngle.x >= 1.0f) normalizedAngle.x -= 1.0f;
    while(normalizedAngle.y <  0.0f) normalizedAngle.y += 1.0f;  while(normalizedAngle.y >= 1.0f) normalizedAngle.y -= 1.0f;
    while(normalizedAngle.z <  0.0f) normalizedAngle.z += 1.0f;  while(normalizedAngle.z >= 1.0f) normalizedAngle.z -= 1.0f;

    vertexColor = nw::ut::Color8(
        convertAngleToColor(normalizedAngle.x*20) * m_prismFadeRate,
        convertAngleToColor(normalizedAngle.y*20) * m_prismFadeRate,
        convertAngleToColor(normalizedAngle.z*20) * m_prismFadeRate,
        255
    );
    //GFL_PRINT("Gyro: color (%d, %d, %d)\n", vertexColor.r, vertexColor.g, vertexColor.b);

    for(s32 x=0; x<BLOCK_X_NUM; ++x)
    {
      for(s32 y=0; y<BLOCK_Y_NUM; ++y)
      {
        for(s32 i=0; i<PIECE_POS_TYPE_NUM; ++i)
        {
          for(u32 idx=0; idx<nw::lyt::VERTEXCOLOR_MAX; ++idx)
          {
            m_block[x][y].piecePane[i]->SetVtxColor(idx, vertexColor);
          }
        }
      }
    }
    */

    gfl2::math::VEC3 angleDegree;
    angleDegree.x = normalizeAngle(m_gyroAngle[0].x) * 360.0f;  // ジャイロが動いたかの判定のときには0以上1未満におさめてはならないが(勝手に変更すると0.98から0になったときに動いたと判定されてしまうから)、
    angleDegree.y = normalizeAngle(m_gyroAngle[0].y) * 360.0f;  // 平均値を求める前にはおさめておかないと大きい値に引っ張られ過ぎることになりそうと思ったが
    angleDegree.z = normalizeAngle(m_gyroAngle[0].z) * 360.0f;  // この後角度にしているから引っ張られ過ぎることはないかもしれない。
    f32 angleDegreeAverage = (angleDegree.x + angleDegree.y + angleDegree.z) / 3.0f;
    
    f32 val = 0.0f;
    f32 valR = 0.0f;
    f32 valG = 0.0f;
    f32 valB = 0.0f;
   
#if 0
    vertexColor.a = 255;
    
    for(s32 x=0; x<BLOCK_X_NUM; ++x)
    {
      for(s32 y=0; y<BLOCK_Y_NUM; ++y)
      {
        for(s32 i=0; i<PIECE_POS_TYPE_NUM; ++i)
        {
          for(u32 idx=0; idx<nw::lyt::VERTEXCOLOR_MAX; ++idx)
          {
            if(idx == nw::lyt::VERTEXCOLOR_LB)
            {
              //val = nn::math::SinDeg( 20.0f * ( angleDegreeAverage + 4.5f*x + 7.0f*y ) ) + 1.0f;
              // kawa_test
              val = nn::math::SinDeg( 5.0f * ( angleDegreeAverage + 4.5f*x + 7.0f*y ) ) + 1.0f;
              if(val > 1.0f) val = 1.0f;
              vertexColor.r = vertexColor.g = vertexColor.b = convertAngleToColorFast(val) * m_prismFadeRate;
            }
            else
            {
              //valR = 0.5f + nn::math::CosDeg( 20.0f * ( angleDegree.x + 4.5f*x + 7.5f*y + 11.25f*i + 15.0f*idx         ) ) + 0.5f;  // 先頭 0.5f * が
              //valG = 0.5f + nn::math::SinDeg( 20.0f * ( angleDegree.y + 4.5f*x + 7.5f*y + 11.25f*i + 15.0f*idx +  5.0f ) ) + 0.5f;  // 正しいから
              //valB = 0.5f + nn::math::CosDeg( 20.0f * ( angleDegree.z + 4.5f*x + 7.5f*y + 11.25f*i + 15.0f*idx + 10.0f ) ) + 0.5f;  // ミスしている
             
              //vertexColor.r = convertAngleToColorFast(valR) * m_prismFadeRate;
              //vertexColor.g = convertAngleToColorFast(valG) * m_prismFadeRate;
              //vertexColor.b = convertAngleToColorFast(valB) * m_prismFadeRate;
              
              // kawa_test
              //valR = 0.25f * nn::math::CosDeg( 5.0f * ( angleDegree.x + 4.5f*x + 7.5f*y + 11.25f*i + 15.0f*idx         ) ) + 0.75f;
              //valG = 0.25f * nn::math::SinDeg( 5.0f * ( angleDegree.y + 4.5f*x + 7.5f*y + 11.25f*i + 15.0f*idx +  5.0f ) ) + 0.75f;
              //valB = 0.25f * nn::math::CosDeg( 5.0f * ( angleDegree.z + 4.5f*x + 7.5f*y + 11.25f*i + 15.0f*idx + 10.0f ) ) + 0.75f;
              
              valR = 0.5f * nn::math::CosDeg( 5.0f * ( angleDegree.x + 4.5f*x + 7.5f*y + 11.25f*i + 15.0f*idx         ) ) + 0.5f;
              valG = 0.5f * nn::math::SinDeg( 5.0f * ( angleDegree.y + 4.5f*x + 7.5f*y + 11.25f*i + 15.0f*idx +  5.0f ) ) + 0.5f;
              valB = 0.5f * nn::math::CosDeg( 5.0f * ( angleDegree.z + 4.5f*x + 7.5f*y + 11.25f*i + 15.0f*idx + 10.0f ) ) + 0.5f;
 
              vertexColor.r = convertAngleToColorFast(valR) * m_prismFadeRate ;//* 0.5f;
              vertexColor.g = convertAngleToColorFast(valG) * m_prismFadeRate ;//* 0.5f;
              vertexColor.b = convertAngleToColorFast(valB) * m_prismFadeRate ;//* 0.5f;
            }
            m_block[x][y].piecePane[i]->SetVtxColor(idx, vertexColor);
          }
        }
      }
    }
#else
    // kawa_test2
    for(s32 x=0; x<m_vertexXNum; ++x)
    {
      for(s32 y=0; y<m_vertexYNum; ++y)
      {
        if( this->isVertexPosBlockCenter(x, y) )
        {
          //val = nn::math::SinDeg( 5.0f * ( angleDegreeAverage + 4.5f*x + 7.0f*y ) ) + 1.0f;
          //if(val > 1.0f) val = 1.0f;
          //m_vertexColor[x][y].r = m_vertexColor[x][y].g = m_vertexColor[x][y].b = convertAngleToColorFast(val) * m_prismFadeRate;
          
          val = nn::math::SinDeg( 5.0f*angleDegreeAverage + 4.5f*x + 7.0f*y ) + 1.0f;
          m_vertexColor[x][y].r = m_vertexColor[x][y].g = m_vertexColor[x][y].b = convertF32ColorToU8Color(val) * m_prismFadeRate;
        }
        else
        {
          //valR = 0.5f * nn::math::CosDeg( 5.0f * ( angleDegree.x + 4.5f*x + 7.5f*y         ) ) + 0.5f;
          //valG = 0.5f * nn::math::SinDeg( 5.0f * ( angleDegree.y + 4.5f*x + 7.5f*y +  5.0f ) ) + 0.5f;
          //valB = 0.5f * nn::math::CosDeg( 5.0f * ( angleDegree.z + 4.5f*x + 7.5f*y + 10.0f ) ) + 0.5f;
          //m_vertexColor[x][y].r = convertAngleToColorFast(valR) * m_prismFadeRate;
          //m_vertexColor[x][y].g = convertAngleToColorFast(valG) * m_prismFadeRate;
          //m_vertexColor[x][y].b = convertAngleToColorFast(valB) * m_prismFadeRate;
 
          valR = 0.5f * nn::math::CosDeg( 5.0f*angleDegree.x + 4.5f*x + 7.5f*y         ) + 0.5f;
          valG = 0.5f * nn::math::SinDeg( 5.0f*angleDegree.y + 4.5f*x + 7.5f*y +  5.0f ) + 0.5f;
          valB = 0.5f * nn::math::CosDeg( 5.0f*angleDegree.z + 4.5f*x + 7.5f*y + 10.0f ) + 0.5f;
          m_vertexColor[x][y].r = convertF32ColorToU8Color(valR) * m_prismFadeRate;
          m_vertexColor[x][y].g = convertF32ColorToU8Color(valG) * m_prismFadeRate;
          m_vertexColor[x][y].b = convertF32ColorToU8Color(valB) * m_prismFadeRate;
        }
      }
    }

    s32 vertexX = 0;
    s32 vertexY = 0;
    for(s32 x=0; x<BLOCK_X_NUM; ++x)
    {
      for(s32 y=0; y<BLOCK_Y_NUM; ++y)
      {
        for(s32 i=0; i<PIECE_POS_TYPE_NUM; ++i)
        {
          for(u32 idx=0; idx<nw::lyt::VERTEXCOLOR_MAX; ++idx)
          {
            this->getVertexPos(&vertexX, &vertexY, x, y, i, idx);
            m_block[x][y].piecePane[i]->SetVtxColor(idx, m_vertexColor[vertexX][vertexY]);
          }
        }
      }
    }
#endif
  }
  else
  {
    for(s32 x=0; x<BLOCK_X_NUM; ++x)
    {
      for(s32 y=0; y<BLOCK_Y_NUM; ++y)
      {
        for(s32 i=0; i<PIECE_POS_TYPE_NUM; ++i)
        {
          for(u32 idx=0; idx<nw::lyt::VERTEXCOLOR_MAX; ++idx)
          {
            vertexColor = m_block[x][y].piecePane[i]->GetVtxColor(idx);

            vertexColor.r = vertexColor.r * m_prismFadeRate;
            vertexColor.g = vertexColor.g * m_prismFadeRate;
            vertexColor.b = vertexColor.b * m_prismFadeRate;
            vertexColor.a = 255;
          
            m_block[x][y].piecePane[i]->SetVtxColor(idx, vertexColor);
          }
        }
      }
    }
  }

  // kawa_test2
  {
    for(u32 idx=0; idx<nw::lyt::VERTEXCOLOR_MAX; ++idx)
    {
      f32 alpha = (80)/255.0f;
      nw::ut::Color8 color = nw::ut::Color8(128,128,128,255);
      if(idx==0)      color.r = 255;
      else if(idx==1) color.g = 255;
      else if(idx==2) color.b = 255;
      else if(idx==3) color.r = color.g = 255;
      color.r = color.r * alpha * m_prismFadeRate;
      color.g = color.g * alpha * m_prismFadeRate;
      color.b = color.b * alpha * m_prismFadeRate;
      m_bgPane->SetVtxColor(idx, color);
    }
  }
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANPRISMSHEET_20160307
#endif
} 

void ZukanPrismSheet::getGyroAngle(void)
{
#ifdef GFL2_UI_USE_GYROSCOPE
  // ジャイロのログを１つずらす
  s32 num = m_gyroAngleNum;
  if(m_gyroAngleNum < GYRO_LOG_NUM_MAX)
  {
    ++m_gyroAngleNum;
  }
  else
  {
    num = m_gyroAngleNum-1;
  }
  for(s32 i=num; i>0; --i)
  {
    m_gyroAngle[i]      = m_gyroAngle[i-1];
    m_gyroAngleValid[i] = m_gyroAngleValid[i-1];
  }

  // 今の値
  gfl2::ui::DeviceManager* uiDeviceManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
  gfl2::ui::Gyroscope* gyro = uiDeviceManager->GetGyroscope(gfl2::ui::DeviceManager::GYROSCOPE_STANDARD);

  if(gyro->IsValid())
  {
    m_gyroAngle[0]      = gyro->GetAngle();
    m_gyroAngleValid[0] = true;
  }
  else
  {
    m_gyroAngle[0]      = gfl2::math::VEC3(0,0,0);
    m_gyroAngleValid[0] = false;
  }
#endif
}

ZukanPrismSheet::GyroMoveRatio ZukanPrismSheet::getGyroMoveRatio(void) const
{
#ifdef GFL2_UI_USE_GYROSCOPE

#if NIJI_APP_ZUKAN_PRISM_DEVELOP
  f32 MOVE_SMALL_ANGLE =  ZukanPrismDevelopParam::s_reduceParam.moveAngleSmall / ZukanPrismDevelopParam::BAIRITSU / 360.0f;  // x/360でx度を表す
  f32 MOVE_BIG_ANGLE   =  ZukanPrismDevelopParam::s_reduceParam.moveAngleBig   / ZukanPrismDevelopParam::BAIRITSU / 360.0f;
#else
  static const f32 MOVE_SMALL_ANGLE =  0.4f/360.0f;  // x/360でx度を表す
  static const f32 MOVE_BIG_ANGLE   =  0.6f/360.0f;
#endif

  GyroMoveRatio gyroMoveRatio = GYRO_MOVE_RATIO_STOP;

  // 直前チェック
  gfl2::math::VEC3 recentDiff(0,0,0);
  if(m_gyroAngleNum >= 2)
  {
    if(m_gyroAngleValid[0] && m_gyroAngleValid[1])
    {
      recentDiff = m_gyroAngle[0] - m_gyroAngle[1];
    }
  }

  // 合計チェック
  gfl2::math::VEC3 totalDiff(0,0,0);
  bool             tempAngleFlag = false;
  gfl2::math::VEC3 tempAngle(0,0,0);
  gfl2::math::VEC3 tempDiff(0,0,0);
  for(s32 i=0; i<m_gyroAngleNum-1; ++i)
  {
    if(tempAngleFlag == false)
    {
      if(m_gyroAngleValid[i])
      {
        tempAngle = m_gyroAngle[i];
        tempAngleFlag = true;
      }
    }
    else
    {
      if(m_gyroAngleValid[i])
      {
        tempDiff = tempAngle - m_gyroAngle[i];
        if(totalDiff.x*tempDiff.x >= 0.0f) { totalDiff.x += tempDiff.x; }
        if(totalDiff.y*tempDiff.y >= 0.0f) { totalDiff.y += tempDiff.y; }
        if(totalDiff.z*tempDiff.z >= 0.0f) { totalDiff.z += tempDiff.z; }
        tempAngle = m_gyroAngle[i];
      }
    }
  }

  do
  {
    if(recentDiff.x <= -MOVE_BIG_ANGLE || MOVE_BIG_ANGLE <= recentDiff.x) { gyroMoveRatio = GYRO_MOVE_RATIO_MOVE_BIG; break; }
    if(recentDiff.y <= -MOVE_BIG_ANGLE || MOVE_BIG_ANGLE <= recentDiff.y) { gyroMoveRatio = GYRO_MOVE_RATIO_MOVE_BIG; break; }
    if(recentDiff.z <= -MOVE_BIG_ANGLE || MOVE_BIG_ANGLE <= recentDiff.z) { gyroMoveRatio = GYRO_MOVE_RATIO_MOVE_BIG; break; }

    if(totalDiff.x <= -MOVE_BIG_ANGLE || MOVE_BIG_ANGLE <= totalDiff.x) { gyroMoveRatio = GYRO_MOVE_RATIO_MOVE_BIG; break; }
    if(totalDiff.y <= -MOVE_BIG_ANGLE || MOVE_BIG_ANGLE <= totalDiff.y) { gyroMoveRatio = GYRO_MOVE_RATIO_MOVE_BIG; break; }
    if(totalDiff.z <= -MOVE_BIG_ANGLE || MOVE_BIG_ANGLE <= totalDiff.z) { gyroMoveRatio = GYRO_MOVE_RATIO_MOVE_BIG; break; }

    if(recentDiff.x <= -MOVE_SMALL_ANGLE || MOVE_SMALL_ANGLE <= recentDiff.x) { gyroMoveRatio = GYRO_MOVE_RATIO_MOVE_SMALL; break; }
    if(recentDiff.y <= -MOVE_SMALL_ANGLE || MOVE_SMALL_ANGLE <= recentDiff.y) { gyroMoveRatio = GYRO_MOVE_RATIO_MOVE_SMALL; break; }
    if(recentDiff.z <= -MOVE_SMALL_ANGLE || MOVE_SMALL_ANGLE <= recentDiff.z) { gyroMoveRatio = GYRO_MOVE_RATIO_MOVE_SMALL; break; }

    if(totalDiff.x <= -MOVE_SMALL_ANGLE || MOVE_SMALL_ANGLE <= totalDiff.x) { gyroMoveRatio = GYRO_MOVE_RATIO_MOVE_SMALL; break; }
    if(totalDiff.y <= -MOVE_SMALL_ANGLE || MOVE_SMALL_ANGLE <= totalDiff.y) { gyroMoveRatio = GYRO_MOVE_RATIO_MOVE_SMALL; break; }
    if(totalDiff.z <= -MOVE_SMALL_ANGLE || MOVE_SMALL_ANGLE <= totalDiff.z) { gyroMoveRatio = GYRO_MOVE_RATIO_MOVE_SMALL; break; }
  }
  while(0);
  
  //GFL_PRINT("Gyro: recentDiff=(%f,%f,%f), totalDiff=(%f,%f,%f)\n", recentDiff.x,recentDiff.y,recentDiff.z, totalDiff.x,totalDiff.y,totalDiff.z); 

  return gyroMoveRatio;
#else
  return GYRO_MOVE_RATIO_STOP;
#endif
}

f32 ZukanPrismSheet::normalizeAngle(f32 angle)  // angleを0.0f<=angle<1.0fにして返す
{
  while(angle < 0.0f) angle += 1.0f;  while(angle >= 1.0f) angle -= 1.0f;
  return angle;
}
u8 ZukanPrismSheet::convertAngleToColor(f32 angle)  // 0.0f(=0度)<=angle<1.0f(=360度)
{
  angle = normalizeAngle(angle);
  return convertAngleToColorFast(angle);
}
u8 ZukanPrismSheet::convertAngleToColorFast(f32 angle)  // angleは必ず0.0f(=0度)<=angle<1.0f(=360度)にして渡すこと
{
  // angle         color
  // 0.0f (=  0度)   0
  // 0.25f(= 90度) 128
  // 0.5f (=180度) 255
  // 0.75f(=270度) 128
  // 1.0f (=360度)   0

  s32 color = 0;
  if(angle <= 0.5f)
  {
    color = 255 * angle*2.0f;
  }
  else
  {
    color = 255 * (1.0f-angle)*2.0f;
  }
  if(color < 0)        color =   0;
  else if(color > 255) color = 255;

  return (static_cast<u8>(color));
}
u8 ZukanPrismSheet::convertF32ColorToU8Color(f32 color)  // 0.0f<=f32<=1.0fの色を0<=u8<=255にして返す。(color<0.0f||1.0f<color)のときは範囲内におさめられる。
{
  s32 c = color * 255;
  if(c < 0)        c =   0;
  else if(c > 255) c = 255;
  return ( static_cast<u8>( c ) );
}

u8 ZukanPrismSheet::calcVertexColor(s32 targetPlace, s32 targetColor, s32 x, s32 y, const gfl2::math::VEC3& angleDegree, f32 angleDegreeAverage)
{
  u8 vertexColor = 0;
#ifdef GFL2_UI_USE_GYROSCOPE
#if NIJI_APP_ZUKAN_PRISM_DEVELOP
  ZukanPrismDevelopParam::TargetParam* targetParam = &(ZukanPrismDevelopParam::s_targetParam[targetPlace][targetColor]);
  if(targetParam->mode == ZukanPrismDevelopParam::MODE_GYRO)
  {
    f32 angleDeg = angleDegreeAverage;
    switch(targetParam->gyroAngle)
    {
    case ZukanPrismDevelopParam::GYRO_ANGLE_X: angleDeg = angleDegree.x; break;
    case ZukanPrismDevelopParam::GYRO_ANGLE_Y: angleDeg = angleDegree.y; break;
    case ZukanPrismDevelopParam::GYRO_ANGLE_Z: angleDeg = angleDegree.z; break;
    }

    f32 shinpuku2    = ( targetParam->funcParam[ZukanPrismDevelopParam::FUNC_PARAM_SHINPUKU_MAX] - targetParam->funcParam[ZukanPrismDevelopParam::FUNC_PARAM_SHINPUKU_MIN] ) / ZukanPrismDevelopParam::BAIRITSU / 2.0f;
    f32 shinpuku_min = targetParam->funcParam[ZukanPrismDevelopParam::FUNC_PARAM_SHINPUKU_MIN] / ZukanPrismDevelopParam::BAIRITSU;
    f32 syuuki       = targetParam->funcParam[ZukanPrismDevelopParam::FUNC_PARAM_SYUUKI] / ZukanPrismDevelopParam::BAIRITSU;
    f32 isou         = targetParam->funcParam[ZukanPrismDevelopParam::FUNC_PARAM_ISOU] / ZukanPrismDevelopParam::BAIRITSU;
    f32 isou_x       = targetParam->funcParam[ZukanPrismDevelopParam::FUNC_PARAM_ISOU_X] / ZukanPrismDevelopParam::BAIRITSU;
    f32 isou_y       = targetParam->funcParam[ZukanPrismDevelopParam::FUNC_PARAM_ISOU_Y] / ZukanPrismDevelopParam::BAIRITSU;

    f32 colorVal = 0.0f;
    if(targetParam->funcType == ZukanPrismDevelopParam::FUNC_TYPE_COS)
    {
      colorVal = shinpuku2 * nn::math::CosDeg( syuuki*angleDeg + isou_x*x + isou_y*y + isou ) + shinpuku2+shinpuku_min;
    }
    else  // ZukanPrismDevelopParam::FUNC_TYPE_SIN
    {
      colorVal = shinpuku2 * nn::math::SinDeg( syuuki*angleDeg + isou_x*x + isou_y*y + isou ) + shinpuku2+shinpuku_min;
    }
    vertexColor = convertF32ColorToU8Color(colorVal);
  }
  else  // ZukanPrismDevelopParam::MODE_CONST
  {
    vertexColor = static_cast<u8>(targetParam->constColor);
  }
#endif  // NIJI_APP_ZUKAN_PRISM_DEVELOP
#endif
  return vertexColor;
}

// kawa_test2
bool ZukanPrismSheet::isVertexPosBlockCenter(s32 vertexX, s32 vertexY) const
{
  return (vertexX%2==1 && vertexY%2==1);
}
void ZukanPrismSheet::getVertexPos(s32* vertexX, s32* vertexY, const s32 blockX, const s32 blockY, const s32 piecePos, const u32 vertexIndex) const
{
  *vertexX = 0;
  *vertexY = 0;

  // ブロックのセンターの位置
  s32 blockCenterX = blockX*2+1;
  s32 blockCenterY = blockY*2+1;

  // ピースの４つ角の位置(ブロックのセンターの位置からの差分)
  static const s32 PIECE_VERTEX_X_DIFF[PIECE_POS_TYPE_NUM][nw::lyt::VERTEXCOLOR_MAX] =
  {
    // LT, RT, LB, RB  // nw::lyt::VERTEXCOLOR_
    {   0, +1,  0, +1 },  // PIECE_POS_TYPE_RT  // ピースRTの４つ角の位置
    {   0, +1,  0, +1 },  // PIECE_POS_TYPE_RB  // ピースRBの４つ角の位置
    {   0, -1,  0, -1 },  // PIECE_POS_TYPE_LB  // ピースLBの４つ角の位置
    {   0, -1,  0, -1 },  // PIECE_POS_TYPE_LT  // ピースLTの４つ角の位置
  };
  static const s32 PIECE_VERTEX_Y_DIFF[PIECE_POS_TYPE_NUM][nw::lyt::VERTEXCOLOR_MAX] =
  {
    // LT, RT, LB, RB  // nw::lyt::VERTEXCOLOR_
    {  -1, -1,  0,  0 },  // PIECE_POS_TYPE_RT  // ピースRTの４つ角の位置
    {  +1, +1,  0,  0 },  // PIECE_POS_TYPE_RB  // ピースRBの４つ角の位置
    {  +1, +1,  0,  0 },  // PIECE_POS_TYPE_LB  // ピースLBの４つ角の位置
    {  -1, -1,  0,  0 },  // PIECE_POS_TYPE_LT  // ピースLTの４つ角の位置
  };

  // ピースの指定した角の位置
  *vertexX = blockCenterX + PIECE_VERTEX_X_DIFF[piecePos][vertexIndex];
  *vertexY = blockCenterY + PIECE_VERTEX_Y_DIFF[piecePos][vertexIndex];
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE

