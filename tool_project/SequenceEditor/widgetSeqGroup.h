/* 
 * File:   widgetSeqGroup.h
 * Author: ariizumi_nobuhiko
 *
 * Created on 2011/03/01, 15:19
 */

#ifndef WIDGETSEQGROUP_H
#define	WIDGETSEQGROUP_H

#include "widgetSeqBar.h"
#include "localDef.h"

class DataGroup;
class WidSequenceGroup : public QWidget
{
	Q_OBJECT
public:
	WidSequenceGroup(DataEditor *dataEditor , DataGroup *dataGroupe , QWidget *parent = NULL);
  virtual ~WidSequenceGroup();

	void AddCommand( DataCommand *com , DataCommand *nextCom = NULL );
	void RemoveCommand( DataCommand *com );
  
  virtual void update();
  virtual void updateGeometry();

public Q_SLOTS:
  void SetIsOpen( bool isOpen );

Q_SIGNALS:
	void SetVisibleSeqBar(bool);

protected:
  //継承
  virtual void paintEvent(QPaintEvent *ev);

private:
	DataGroup  *mDataGroup;
	DataEditor *mDataEditor;

	QVBoxLayout		  *mMainLyt;
	WidSequenceBarList *mMainBar;

};

#endif	/* WIDGETSEQGROUP_H */

