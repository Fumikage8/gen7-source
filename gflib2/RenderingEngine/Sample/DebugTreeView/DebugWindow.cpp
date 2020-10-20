
#include "DebugWindow.h"

#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
//#include <math/include/gfl2_MathUtil.h>
#include <math/include/gfl2_MathCommon.h>

#include <sstream>
#include <iomanip>

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;

//============================================================================================
//============================================================================================

// コンストラクタ
DebugTreeItem::DebugTreeItem() : 
  m_pNode(NULL)
{
}

// ノード設定
void DebugTreeItem::SetNode(gfl2::renderingengine::scenegraph::DagNode* pNode)
{
  m_pNode = pNode;
}

// ノード取得
gfl2::renderingengine::scenegraph::DagNode* DebugTreeItem::GetNode()
{
  return m_pNode;
}

//============================================================================================
//============================================================================================

// コンストラクタ
DebugWindow::DebugWindow(QWidget* pParent) : 
  QMainWindow(pParent, Qt::SubWindow),
  m_pTreeWidget(NULL),
  m_pTextEdit(NULL),
  m_pSelectedNode(NULL)
{
  // 適当なサイズと位置で起動
  this->resize(640, 800);
  this->move(1400, 200);

  // ツリーウィジェット作成
  m_pTreeWidget = new QTreeWidget(this);
  m_pTreeWidget->resize(620, 450);
  m_pTreeWidget->setHeaderLabel(QString("Hierarchy"));

  // テキストエディット作成
  m_pTextEdit = new QTextEdit(this);
  m_pTextEdit->resize(620, 300);
  m_pTextEdit->move(0, 460);
}

// デストラクタ
DebugWindow::~DebugWindow()
{
}

// 表示
void DebugWindow::Show()
{
  this->show();
}

// ノードの解析
void DebugWindow::ParseNode(gfl2::renderingengine::scenegraph::DagNode* pNode)
{
  DebugTreeItem* pItem = this->GetTreeItem(pNode);
  m_pTreeWidget->insertTopLevelItem(0, pItem);
}

// ノード名取得
std::string DebugWindow::GetNodeName(gfl2::renderingengine::scenegraph::DagNode* pNode)
{
  //! @note 型名をとりあえずtypeid()で取得しておく、要修正
  std::string typeName = typeid(*pNode).name();
  size_t pos = typeName.find_last_of(":");
  typeName = typeName.substr(pos + 1);
  std::string nodeName = pNode->GetNodeName();
  if (nodeName == "")
  {
    nodeName = "__no_name__";
  }

  return nodeName + "  [" + typeName + "]";
}

// ツリーアイテム取得
DebugTreeItem* DebugWindow::GetTreeItem(gfl2::renderingengine::scenegraph::DagNode* pNode)
{
  std::string nodeName = this->GetNodeName(pNode);

  DebugTreeItem* pItem = new DebugTreeItem();
  pItem->setText(0, nodeName.c_str());
  pItem->SetNode(pNode);

  // 子供も同じように
  for (u32 i = 0; i < pNode->GetChildCount(); ++i)
  {
    pItem->addChild(this->GetTreeItem(pNode->GetChild(i)));
  }

  return pItem;
}

// 更新
void DebugWindow::Update()
{
  m_pTextEdit->clear();

  // 選択されていなければ無視
  QTreeWidgetItem* pCurrent = m_pTreeWidget->currentItem();
  if (pCurrent == NULL)
  {
    return;
  }

  // トランスフォームノード以外は無視
  DebugTreeItem* pItem = static_cast<DebugTreeItem*>(pCurrent);
  m_pSelectedNode = pItem->GetNode()->SafeCast<gfl2::renderingengine::scenegraph::instance::TransformNode>();
}

// 描画
void DebugWindow::Draw()
{
  this->DrawTextEdit();
}

// テキスト領域の描画
void DebugWindow::DrawTextEdit()
{
  // 選択中のノードが空なら無視
  if (m_pSelectedNode == NULL)
  {
    return;
  }

  // テキストにローカル行列の位置、回転、拡縮の値を表示
  math::Matrix rotMatrix;
  m_pSelectedNode->GetLocalRotation().QuaternionToMatrix(rotMatrix);

  math::Vector pos = m_pSelectedNode->GetLocalTranslate();
  math::Vector rot = rotMatrix.GetElemRotationZYX();
  math::Vector scl = m_pSelectedNode->GetLocalScale();

  std::stringstream ss;

  char tmp[256];
  sprintf_s(tmp, sizeof(tmp), ">> Position X: %10.5f  Y: %10.5f  Z: %10.5f\n", pos.x, pos.y, pos.z);
  ss << tmp;
  sprintf_s(tmp, sizeof(tmp), ">> Rotation X: %10.5f  Y: %10.5f  Z: %10.5f\n", rot.x, rot.y, rot.z);
  ss << tmp;
  sprintf_s(tmp, sizeof(tmp), ">> Scaling  X: %10.5f  Y: %10.5f  Z: %10.5f\n", scl.x, scl.y, scl.z);
  ss << tmp;

  m_pTextEdit->setText(ss.str().c_str());
}

// 選択中のノードを取得
gfl2::renderingengine::scenegraph::instance::TransformNode* DebugWindow::GetSelectedNode()
{
  return m_pSelectedNode;
}

//============================================================================================
//============================================================================================

// コンストラクタ
DebugWindowProc::DebugWindowProc() : 
  m_pApplication(NULL),
  m_pDebugWindow(NULL)
{
  // ダミー引数
  int appArgc = 0;
  m_pApplication = new QApplication(appArgc, NULL);

  // WindowsXPモードにしておかないと罫線が表示されなかった
  QApplication::setStyle("windowsxp");

  // ウィンドウ作成
  m_pDebugWindow = new DebugWindow();
  m_pDebugWindow->Show();
}

// ノードの解析
void DebugWindowProc::ParseNode(gfl2::renderingengine::scenegraph::DagNode* pNode)
{
  m_pDebugWindow->ParseNode(pNode);
}

// 更新
void DebugWindowProc::Update()
{
  m_pDebugWindow->Update();
}

// 描画
void DebugWindowProc::Draw()
{
  m_pDebugWindow->Draw();
  m_pApplication->processEvents();
}

// 選択中のノードを取得
gfl2::renderingengine::scenegraph::instance::TransformNode* DebugWindowProc::GetSelectedNode()
{
  return m_pDebugWindow->GetSelectedNode();
}

// eof
