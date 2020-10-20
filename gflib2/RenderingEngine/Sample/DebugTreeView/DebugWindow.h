#ifndef DebugWindow_H_INCLUDED
#define DebugWindow_H_INCLUDED

#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTextEdit>

#include <renderingengine/include/scenegraph/gfl2_DagNode.h>
#include <renderingengine/include/scenegraph/Instance/gfl2_TransformNode.h>

// デバッグツリーアイテム
class DebugTreeItem : public QTreeWidgetItem
{
public:

  // コンストラクタ
  DebugTreeItem();

  // ノード設定
  void SetNode(gfl2::renderingengine::scenegraph::DagNode* pNode);

  // ノード取得
  gfl2::renderingengine::scenegraph::DagNode* GetNode();

private:
  gfl2::renderingengine::scenegraph::DagNode* m_pNode;  // ノード
};

// デバッグウィンドウ
class DebugWindow : public QMainWindow
{
public:

  // コンストラクタ
  explicit DebugWindow(QWidget* pParent = NULL);

  // デストラクタ
  ~DebugWindow();

  // 表示
  void Show();

  // ノードの解析
  void ParseNode(gfl2::renderingengine::scenegraph::DagNode* pNode);

  // ノード名取得
  std::string GetNodeName(gfl2::renderingengine::scenegraph::DagNode* pNode);

  // ツリーアイテム取得
  DebugTreeItem* GetTreeItem(gfl2::renderingengine::scenegraph::DagNode* pNode);

  // 更新
  void Update();

  // 描画
  void Draw();

  // テキスト領域の描画
  void DrawTextEdit();

  // 選択中のノードを取得
  gfl2::renderingengine::scenegraph::instance::TransformNode* GetSelectedNode();

private:
    QTreeWidget* m_pTreeWidget;                                                   // ツリーウィジェット
    QTextEdit*   m_pTextEdit;                                                     // テキストエディット
    gfl2::renderingengine::scenegraph::instance::TransformNode* m_pSelectedNode;  // 選択中のノード
};

// デバッグウィンドウプロセス
class DebugWindowProc
{
public:

  // コンストラクタ
  DebugWindowProc();

  // ノードの解析
  void ParseNode(gfl2::renderingengine::scenegraph::DagNode* pNode);

  // 更新
  void Update();

  // 描画
  void Draw();

  // 選択中のノードを取得
  gfl2::renderingengine::scenegraph::instance::TransformNode* GetSelectedNode();

private:
  
  QApplication* m_pApplication;     // Qtアプリケーション
  DebugWindow* m_pDebugWindow;      // デバッグウィンドウ
};

#endif
