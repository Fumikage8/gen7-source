//==============================================================================
/**
 * @file IdWidget.h
 * @brief
 * @author kimura_shin
 * @data 2012/03/22, 10:22
 */
// =============================================================================

#if !defined( __IDWIDGET_H__ )
#define __IDWIDGET_H__

#include <gflib.h>

/**
 * ID item
 */
class IdItem : public QListWidgetItem {
public:

  IdItem( const QString& str, const QString& col, const QStringList& id ) : QListWidgetItem(str), m_Color(col), m_Id(id) {
  }

  virtual ~IdItem( void ) {
  }

  const QColor& GetColor( ) const {
    return m_Color;
  }
  const QStringList& GetId( ) const {
    return m_Id;
  }
private:
  QColor m_Color;
  QStringList m_Id;
};

/**
 * ID
 */
class IdWidget : public QGroupBox {
  Q_OBJECT
public:

  IdWidget( void );

  virtual ~IdWidget( void ) {
  }

  /**
   * 現在選択されているID色を取得
   * @return ID色
   */
  const QColor GetIdColor( ) const {
    IdItem* item = (IdItem*)m_pIdList->currentItem();
    return item?item->GetColor():QColor(0,0,0);
  }

  /**
   * ID色から現在選択しているアイテムを設定する
   * @param col ID色
   * @return 真 成功 偽 col 色がリストになかった
   */
  bool SetCurrentIdColor( const QColor& col );

  QMap<u32,u8> m_IdMap;
  QMap<u32,QString> m_IdMapString;
  QString m_Dublicated;
  void CheckId(const QString& fname);

private:
  QListWidget* m_pIdList;
  QPushButton* m_pIdButton;

  public slots:
    void IdChanged(QListWidgetItem*);
};


#endif	/* __IDWIDGET_H__ */

