/*
 * File:   widgetTitleName.h
 * Author: ariizumi_nobuhiko
 *
 * Created on 2011/02/24, 13:21
 */

#ifndef __WIDGET_TITLE_NAME_H__
#define	__WIDGET_TITLE_NAME_H__

#include "localDef.h"

class WidTitleNameGroup : public QWidget
{
  Q_OBJECT

public:
	WidTitleNameGroup(DataEditor *dataEditor , DataGroup *dataGroup , QWidget *parent = NULL);
	~WidTitleNameGroup();
	void GetDrawString(QString *str );

	void AddCommand( DataCommand *com );
	void RemoveCommand( DataCommand *com );

public Q_SLOTS:
	void SetIsOpen( bool flg );

protected:
  //åpè≥
  void paintEvent(QPaintEvent *ev);
  virtual QSize sizeHint() const;
	virtual void mousePressEvent( QMouseEvent *ev);
  virtual void contextMenuEvent(QContextMenuEvent *ev);
  virtual void	mouseDoubleClickEvent ( QMouseEvent *ev );

private:

  void MouseClickCore( const int posX , const int posY );

	DataGroup	*mDataGroup;
	DataEditor	*mDataEditor;
};


#endif	/* __WIDGET_TITLE_NAME_H__ */

