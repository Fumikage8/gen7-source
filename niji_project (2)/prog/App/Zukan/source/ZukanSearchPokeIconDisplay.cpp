//==============================================================================
/**
 @file    ZukanSearchPokeIconDisplay.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.10.08
 @brief   図鑑の検索のポケアイコン表示クラス
 */
//==============================================================================


// gfl2のインクルード
#include <Layout/include/gfl2_Layout.h>

// nijiのインクルード
#include <System/include/GflUse.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Frame/AppFrameManager.h>
#include <AppLib/include/Util/app_util_2d.h>

// nijiのデータのヘッダーファイルのインクルード
#include <niji_conv_header/app/zukan/search.h>
#include <niji_conv_header/app/zukan/search_pane.h>
#include <niji_conv_header/app/zukan/search_anim_list.h>

// 図鑑のインクルード
#include <App/Zukan/include/ZukanProcParam.h>
#include "ZukanDefine.h"
#include "ZukanSceneParam.h"
#include "ZukanType.h"
#include "ZukanUtil.h"
#include "ZukanContents.h"
#include "ZukanDataManager.h"
#include "ZukanSearchThread.h"
#include "ZukanTool.h"
#include "ZukanFrameSearch.h"
#include "ZukanViewSearch.h"

#include "ZukanSearchPokeIconDisplay.h"




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


ZukanSearchPokeIconDisplay::ZukanSearchPokeIconDisplay(
  gfl2::heap::HeapBase*           devHeap,
  ZukanDataManager*               dataManager,
  ZukanContents*                  contents,
  app::util::G2DUtil*             pG2DUtil,
  u32                             lytWkId,
  gfl2::lyt::LytWk*               lytWk,
  const gfl2::lyt::LytMultiResID& lytRes
)
  : m_devHeap(devHeap),
    m_dataManager(dataManager),
    m_contents(contents),
    m_pG2DUtil(pG2DUtil),
    m_lytWkId(lytWkId),
    m_lytWk(lytWk),
    m_lytRes(lytRes),
    m_drawFigureDataIndexList(NULL),
    m_dispPokeDataIndexNum(0),
    m_dispPokeDataIndexList(NULL),
    m_textureInfo(NULL),
    m_texBuf(NULL),
    m_topPane(NULL),
    m_pauseReq(false),
    m_pause(false),
    m_state(STATE_NOT_DISPLAY),
    m_stateCount(0),
    m_index(0),
    m_zeroIndex(0)
{
  // ペイン情報取得
  {
    for(u32 y=0; y<POKE_ICON_Y_NUM; ++y)
    {
      m_pokeIconParentPane[y] = m_lytWk->GetPane(POKE_ICON_PARENT_PANE_ID[y]);

      for(u32 x=0; x<POKE_ICON_X_NUM; ++x)
      {
        m_pokeIconPartsInfo[y][x].partsPane = m_lytWk->GetPartsPane(POKE_ICON_PARTS_CONST_INFO[y][x].partsPaneIndex);
        m_pokeIconPartsInfo[y][x].picPane   = m_lytWk->GetPicturePane(m_pokeIconPartsInfo[y][x].partsPane, POKE_ICON_PARTS_CONST_INFO[y][x].picPaneIndex, &m_lytRes);
        m_pokeIconPartsInfo[y][x].ballPane  = m_lytWk->GetPicturePane(m_pokeIconPartsInfo[y][x].partsPane, POKE_ICON_PARTS_CONST_INFO[y][x].ballPaneIndex, &m_lytRes);
        m_pokeIconPartsInfo[y][x].kiraPane  = m_lytWk->GetPicturePane(m_pokeIconPartsInfo[y][x].partsPane, POKE_ICON_PARTS_CONST_INFO[y][x].kiraPaneIndex, &m_lytRes);
      }
      
      gfl2::math::VEC3  pos   = m_pokeIconPartsInfo[y][0].partsPane->GetTranslate();
      m_pokeIconPosY[y]       = pos.y;
    }

    m_topPane = m_lytWk->GetPane(PANENAME_ZUKAN_SRC_UPP_000_PANE_TOP);
  }

  // テクスチャ情報
  m_textureInfo = GFL_NEW_ARRAY(m_devHeap) gfl2::lyt::LytResTextureInfo[m_contents->GetPokeDataNumMax()];
  m_texBuf      = GFL_NEW_ARRAY(m_devHeap) void*[m_contents->GetPokeDataNumMax()];

  // 表示をやめる
  this->CancelDisplay();
}

ZukanSearchPokeIconDisplay::~ZukanSearchPokeIconDisplay()
{
  for(u32 i=0; i<m_contents->GetPokeDataNumMax(); ++i)
  {
    GflHeapSafeFreeMemory(m_texBuf[i]);
  }
  GFL_SAFE_DELETE_ARRAY(m_texBuf);
  GFL_SAFE_DELETE_ARRAY(m_textureInfo);
}

void ZukanSearchPokeIconDisplay::StartDisplay(
  const u8*  drawFigureDataIndexList,
  u16        dispPokeDataIndexNum,
  const u16* dispPokeDataIndexList
)
{
  this->startDisplay(
    drawFigureDataIndexList,
    dispPokeDataIndexNum,
    dispPokeDataIndexList
  );
}

void ZukanSearchPokeIconDisplay::StartDisplayPage(
  const u8*  drawFigureDataIndexList,
  u16        dispPokeDataIndexNum,
  const u16* dispPokeDataIndexList
)
{
  this->startDisplay(
    drawFigureDataIndexList,
    dispPokeDataIndexNum,
    dispPokeDataIndexList
  );

  /*
  見栄えが悪いので1ページ分表示してのスタートは止める。
  // 1ページ分表示する
  while(m_index < m_dispPokeDataIndexNum)  // 1ページ埋まる前にポケアイコンが尽きたら終了
  {
    this->createPokeIconTexture();
    if(m_index >= POKE_ICON_NUM_IN_SCREEN) break;  // 1ページ埋まったら終了
  }
  */
}

void ZukanSearchPokeIconDisplay::CancelDisplay(void)
{
  this->changeState(STATE_NOT_DISPLAY);
  m_topPane->SetVisible(false);
}

void ZukanSearchPokeIconDisplay::Update(void)
{
  bool canPause = false;
  if(m_state != STATE_NOT_DISPLAY)  // 非表示でないとき
  {
    if(!m_pause)
    {
      switch(m_state)
      {
      case STATE_DISPLAY_CREATE:  // 1個ずつ生成して表示
        {
          // 生成
          this->createPokeIconTexture();
          if(m_index >= m_dispPokeDataIndexNum)
          {
            // 最後の1個を生成したとき
            if(m_index <= POKE_ICON_NUM_IN_SCREEN)
            {
              // 画面内におさまっているとき
              this->changeState(STATE_DISPLAY_WAIT);
            }
            else
            {
              // 画面内におさまっていないとき
              this->changeState(STATE_DISPLAY_WAIT_AT_BOTTOM);
            }
          }
          // スクリーンを1段上へ
          bool isRaised = this->raiseScreen();
          if(!isRaised)
          {
            canPause = true;
          }
          break;
        }
      case STATE_DISPLAY_SCROLL_UP:  // 上へスクロール中
        {
          bool isShifted = false;
          bool isFinished = scrollScreen(-1, &isShifted);
          if(isShifted || isFinished)
          {
            canPause = true;
            if(isFinished)
            {
              this->changeState(STATE_DISPLAY_WAIT_AT_TOP);
            }
          }
          break;
        }
      case STATE_DISPLAY_SCROLL_DOWN:  // 下へスクロール中
        {
          bool isShifted = false;
          bool isFinished = scrollScreen(1, &isShifted);
          if(isShifted || isFinished)
          {
            canPause = true;
            if(isFinished)
            {
              this->changeState(STATE_DISPLAY_WAIT_AT_BOTTOM);
            }
          }
          break;
        }
      case STATE_DISPLAY_WAIT_AT_TOP:  // 上で待機中
        {
          canPause = true;
          if( m_stateCount >= WAIT_COUNT_MAX )
          {
            this->changeState(STATE_DISPLAY_SCROLL_DOWN);
          }
          else
          {
            ++m_stateCount;
          }
          break;
        }
      case STATE_DISPLAY_WAIT_AT_BOTTOM:  // 下で待機中
        {
          // スクリーンを1段上へ
          bool isRaised = this->raiseScreen();
          if(!isRaised)
          {
            canPause = true;
          }
          if( m_stateCount >= WAIT_COUNT_MAX && (!isRaised))
          {
            this->changeState(STATE_DISPLAY_SCROLL_UP);
          }
          else
          {
            ++m_stateCount;
          }
          break;
        }
      case STATE_DISPLAY_WAIT:  // その場で待機中
        {
          canPause = true;
          break;
        }
      }
    }
  }
  else  // 非表示のとき
  {
    canPause = true;
  }

  // リクエストに対する処理
  if(m_pauseReq)
  {
    if(canPause)
    {
      m_pauseReq = false;
      m_pause = true;
    }
  }
}

void ZukanSearchPokeIconDisplay::RequestPause(void)
{
  if(!m_pause) m_pauseReq = true;
}
void ZukanSearchPokeIconDisplay::ForcePause(void)
{
  m_pauseReq = false;
  m_pause = true;
}

bool ZukanSearchPokeIconDisplay::IsDisplayProgressing(void) const
{
  bool progress = false;
  if(m_state != STATE_NOT_DISPLAY)
  {
    if(!m_pause) progress = true;
  }
  return progress;
}

void ZukanSearchPokeIconDisplay::ReleasePause(void)
{
  m_pauseReq = false;
  m_pause = false;
}

void ZukanSearchPokeIconDisplay::startDisplay(
  const u8*  drawFigureDataIndexList,
  u16        dispPokeDataIndexNum,
  const u16* dispPokeDataIndexList
)
{
  m_drawFigureDataIndexList = drawFigureDataIndexList;
  m_dispPokeDataIndexNum    = dispPokeDataIndexNum;
  m_dispPokeDataIndexList   = dispPokeDataIndexList;
  
  this->changeState(STATE_DISPLAY_CREATE);
  m_topPane->SetVisible(true);

  m_topPane->SetTranslate(gfl2::math::VEC3(0,0,0));

  for(u32 y=0; y<POKE_ICON_Y_NUM; ++y)
  {
    for(u32 x=0; x<POKE_ICON_X_NUM; ++x)
    {
      m_pokeIconPartsInfo[y][x].partsPane->SetVisible(false);
    }
  }

  // メモリに穴が開かないように最初からつくり直す
  for(u32 i=0; i<m_contents->GetPokeDataNumMax(); ++i)
  {
    GflHeapSafeFreeMemory(m_texBuf[i]);
  }

  m_index     = 0;
  m_zeroIndex = -POKE_ICON_NUM_AT_TOP;  // 上2段は最初は使わない
}

void ZukanSearchPokeIconDisplay::createPokeIconTexture(void)
{
  if(m_index < m_dispPokeDataIndexNum)
  {
    // 貼る対象となるペインを決定する
    s32 paneNo = m_index - m_zeroIndex;
    if(paneNo >= POKE_ICON_NUM)
    {
      // 最下段よりはみ出したので、1段上へずらす。( = 画面内におさまっていない下2段に突入したのではなく、用意されているペインをオーバーしたので、1段ずらす。)
      this->shiftPokeIconTexture(1);
      // これで、貼る対象となるペインも1段ずらせる
      paneNo -= POKE_ICON_X_NUM;
    }

    // ポケアイコンテクスチャを生成し、ペインに貼る
    {
      s32 y = paneNo / POKE_ICON_X_NUM;
      s32 x = paneNo % POKE_ICON_X_NUM;
      PokeTool::SimpleParam sp = m_contents->GetPokeSimpleParamByFigureDataIndex(m_dispPokeDataIndexList[m_index], m_drawFigureDataIndexList[m_dispPokeDataIndexList[m_index]]);
      m_texBuf[m_index] = ZukanUtil::CreatePokeIconTexture(
          m_devHeap,
          m_devHeap->GetLowerHandle(),
          m_pokeIconPartsInfo[y][x].picPane,
          0,
          0,
          sp,
          false,
          m_dataManager,
          &m_textureInfo[m_index]
      );
      // 貼ったペインを表示する
      this->setPaneVisible(x, y, m_index);
    }

    ++m_index;
  }
}

void ZukanSearchPokeIconDisplay::shiftPokeIconTexture(s32 upOrDown)
{
  // 1段上か下へずらす
  s32 nextParentYDiff = 0;
  if(upOrDown > 0)
  {
    m_zeroIndex += POKE_ICON_X_NUM;
    nextParentYDiff = -1;
  }
  else
  {
    m_zeroIndex -= POKE_ICON_X_NUM;
    nextParentYDiff =  1;
  }

  // ペインの親子関係を1段変更する(こうしないと、CPUとGPUの1フレームずれが発生したとき1段ずれたポケモンが一瞬表示されてしまうと思われたので、こうした)
  {
    // 親を変更する
    for(s32 y=0; y<POKE_ICON_Y_NUM; ++y)
    {
      s32 nextParentY = (y+POKE_ICON_Y_NUM+nextParentYDiff)%POKE_ICON_Y_NUM;
      gfl2::lyt::LytPane* currParentPane = m_pokeIconParentPane[y];
      gfl2::lyt::LytPane* nextParentPane = m_pokeIconParentPane[nextParentY];
      for(s32 x=0; x<POKE_ICON_X_NUM; ++x)
      {
        currParentPane->RemoveChild(m_pokeIconPartsInfo[y][x].partsPane);
        nextParentPane->AppendChild(m_pokeIconPartsInfo[y][x].partsPane);
      }
    }

    // インデックスを親に合わせる
    {
      // 最初に書き換えられるInfoを覚えておく
      PokeIconPartsInfo tempInfo[POKE_ICON_X_NUM];
      {
        s32 y = (nextParentYDiff>0) ? (0) : (POKE_ICON_Y_NUM-1) ;
        for(s32 x=0; x<POKE_ICON_X_NUM; ++x)
        {
          tempInfo[x] = m_pokeIconPartsInfo[y][x];
        }
      }
      // インデックスを親に合わせる
      if(nextParentYDiff>0)
      {
        for(s32 y=POKE_ICON_Y_NUM-1; y>0; --y)
        {
          s32 nextParentY = (y+POKE_ICON_Y_NUM+nextParentYDiff)%POKE_ICON_Y_NUM;
          for(s32 x=0; x<POKE_ICON_X_NUM; ++x)
          {
            m_pokeIconPartsInfo[nextParentY][x] = m_pokeIconPartsInfo[y][x];
          }
        }
      }
      else
      {
        for(s32 y=0; y<POKE_ICON_Y_NUM-1; ++y)
        {
          s32 nextParentY = (y+POKE_ICON_Y_NUM+nextParentYDiff)%POKE_ICON_Y_NUM;
          for(s32 x=0; x<POKE_ICON_X_NUM; ++x)
          {
            m_pokeIconPartsInfo[nextParentY][x] = m_pokeIconPartsInfo[y][x];
          }
        }
      }
      // 最初に覚えておいたInfoを使う
      {
        s32 y = (nextParentYDiff>0) ? (0) : (POKE_ICON_Y_NUM-1) ;
        s32 nextParentY = (y+POKE_ICON_Y_NUM+nextParentYDiff)%POKE_ICON_Y_NUM;
        for(s32 x=0; x<POKE_ICON_X_NUM; ++x)
        {
          m_pokeIconPartsInfo[nextParentY][x] = tempInfo[x];
        }
      }
    }

    // 位置を修正する
    for(s32 y=0; y<POKE_ICON_Y_NUM; ++y)
    {
      for(s32 x=0; x<POKE_ICON_X_NUM; ++x)
      {
        gfl2::math::VEC3 pos = m_pokeIconPartsInfo[y][x].partsPane->GetTranslate();
        pos.y = m_pokeIconPosY[y];
        m_pokeIconPartsInfo[y][x].partsPane->SetTranslate(pos);
      }
    }
  }

  // これまでの挙動から推測すると
  //          即時反映：表示非表示変更、頂点カラー変更、テクスチャ変更
  // 1フレーム後に反映：位置変更、透明度変更、マテリアル変更
  // となっていると思われる。
  // UpdateがDrawよりも前に呼ばれる普通の処理のときのアニメで「即時反映」の項目を変更していた場合、即時反映だろうかそれとも1フレーム後に反映だろうか？
  
  // ペインの親子関係を1段変更しなくても、全てのテクスチャを貼り直すこちらの方法だけでもうまくいくが、
  // この方法だけだとCPUとGPUの1フレームずれが発生したとき1段ずれたポケモンが一瞬表示されてしまうと思われる。
  // ペインの親子関係を1段変更する方法と併用しているので、画面内のところは同じテクスチャを貼ることになるから画面外のところだけテクスチャを貼り直せばいい（ここでは全部貼り直しているが）。
  // テクスチャ貼り直し
  for(s32 y=0; y<POKE_ICON_Y_NUM; ++y)
  {
    for(s32 x=0; x<POKE_ICON_X_NUM; ++x)
    {
      s32 index = y * POKE_ICON_X_NUM + x + m_zeroIndex;
      if( index < 0 || m_index <= index)
      {
        // 表示するポケアイコンがないときは、ペインを非表示にしておく
        m_pokeIconPartsInfo[y][x].partsPane->SetVisible(false);
      }
      else
      {
        // 表示するポケアイコンがあるときは、テクスチャを貼り直す
        bool replaceTexRes = app::tool::LytTexReplaceSystem::ReplaceTextureStatic(
            &m_textureInfo[index],
            m_pokeIconPartsInfo[y][x].picPane,
            0,
            0
        );
        if(replaceTexRes == false)
        {
          GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。テクスチャを置き換えるのに失敗。
        }
        // 貼ったペインを表示する
        this->setPaneVisible(x, y, index);
      }
    }
  }
}

void ZukanSearchPokeIconDisplay::setPaneVisible(u32 x, u32 y, u32 index)
{
  m_pokeIconPartsInfo[y][x].partsPane->SetVisible(true);
  const ZukanContents::PokeData* pokeData = m_contents->GetPokeData(m_dispPokeDataIndexList[index]);
  
  // 捕まえた、見たフラグ  // アニメにすると1フレームずれるので、表示/非表示と頂点カラーにした。
  //s32 getSeeAnimFrame = 0;  // レアマーク
  //if(pokeData->getFlag) getSeeAnimFrame = 2;
  //else if(pokeData->figureSeeNum>0) getSeeAnimFrame = 1;  // Statusという名前のアニメでペインの色を暗くしてくれているようだ。
  //ZukanUtil::BindLytAnim(m_pG2DUtil, m_lytWkId, POKE_ICON_PARTS_CONST_INFO[y][x].animIndex, getSeeAnimFrame);

  // 捕まえた、見たフラグ
  m_pokeIconPartsInfo[y][x].picPane->SetVisible(pokeData->figureSeeNum>0);
  m_pokeIconPartsInfo[y][x].ballPane->SetVisible(pokeData->figureSeeNum==0);
  m_pokeIconPartsInfo[y][x].kiraPane->SetVisible(false);
  nw::ut::Color8 vtxColor = ZukanType::POKE_GET_COLOR;
  if(pokeData->figureSeeNum>0 && pokeData->getFlag==0)
  {
    vtxColor = ZukanType::POKE_SEE_COLOR;
  }
  for(s32 vtxIdx=0; vtxIdx<nw::lyt::VERTEXCOLOR_MAX; ++vtxIdx) m_pokeIconPartsInfo[y][x].picPane->SetVtxColor(vtxIdx, vtxColor);
}

bool ZukanSearchPokeIconDisplay::raiseScreen(void)
{
  bool isRaised = false;
  if(1 <= m_index && m_index <= m_dispPokeDataIndexNum)  // 最後に貼ったのは(m_index-1)
  {
    // 最後に貼ったペインを決定する
    s32 paneNo = m_index -1 - m_zeroIndex;

    if(paneNo >= POKE_ICON_NUM_WITHOUT_BOTTOM)
    {
      // 下2段に貼っていたので、スクリーンを少しずつ上げる
      gfl2::math::VEC3 pos = m_topPane->GetTranslate();
      pos.y += RAISE_SCREEN_SPEED;
      
      if(pos.y >= POKE_ICON_DIFF_Y)
      {
        // スクリーンが1段分上へ上がり切ったので、1段上へずらす
        this->shiftPokeIconTexture(1);
        // スクリーンを上げていた分を戻す
        pos.y = 0;
      }
      else
      {
        isRaised = true;  // 上へ上げ中
      }
      
      m_topPane->SetTranslate(pos);
    }
  }
  return isRaised;
}

bool ZukanSearchPokeIconDisplay::scrollScreen(s32 upOrDown, bool* isShifted)
{
  bool isFinished = false;
  *isShifted = false;

  gfl2::math::VEC3 pos = m_topPane->GetTranslate();
  if(upOrDown > 0)
  {
    if(m_zeroIndex >= m_dispPokeDataIndexNum - POKE_ICON_NUM_WITHOUT_BOTTOM)  // ここまで来ていたら終了(下2段は使わない。xが中途半端なところで終わっていてもこの判定でよい)
    {
      isFinished = true;
      *isShifted = true;  // ずらしたわけではないがyが固定値なのでtrue。
      pos.y = 0.0f;
    }
    else
    {
      // スクリーンを少しずつ上げる
      pos.y += SCROLL_SCREEN_SPEED;
        
      if(pos.y >= POKE_ICON_DIFF_Y -0.0001f)  // 計算誤差を考慮した判定
      {
        // スクリーンが1段分上へ上がり切ったので、1段上へずらす
        this->shiftPokeIconTexture(1);
        // スクリーンを上げていた分を戻す
        pos.y = 0;  // 計算誤差をなくす
        *isShifted = true;
      }
    }
  }
  else
  {
    if(m_zeroIndex <= -POKE_ICON_NUM_AT_TOP)  // ここまで来ていたら終了(上2段は使わない)
    {
      isFinished = true;
      *isShifted = true;  // ずらしたわけではないがyが固定値なのでtrue。
      pos.y = 0.0f;
    }
    else
    {
      // スクリーンを少しずつ下げる
      pos.y -= SCROLL_SCREEN_SPEED;
        
      if(pos.y <= -POKE_ICON_DIFF_Y +0.0001f)  // 計算誤差を考慮した判定
      {
        // スクリーンが1段分下へ下がり切ったので、1段下へずらす
        this->shiftPokeIconTexture(-1);
        // スクリーンを下げていた分を戻す
        pos.y = 0;  // 計算誤差をなくす
        *isShifted = true;
      }
    }
  }
  m_topPane->SetTranslate(pos);

  return isFinished;
}

void ZukanSearchPokeIconDisplay::changeState(State state)
{
  m_state = state;
  m_stateCount = 0;
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

