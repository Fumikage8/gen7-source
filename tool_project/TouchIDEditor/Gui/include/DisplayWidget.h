//==============================================================================
/**
 * @file	DisplayWidget.h
 * @brief	
 * @author	kimura_shin
 * @data	2012/03/23, 16:12
 */
// =============================================================================

#if !defined( __DISPLAYWIDGET_H__ )
#define	__DISPLAYWIDGET_H__

#include <gflib.h>
#include "GLWidget.h"

/**
 * Display Widget
 */
class DisplayWidget : public QGroupBox {
	Q_OBJECT

public:
	
	DisplayWidget();
	virtual ~DisplayWidget(){
		delete m_pLight; // 未使用なので
	}
	
	/**
	 * モデルのα設定値
     * @return 0x00 - 0xff
     */
    s32 GetAlpha( s32 id ) const {
    	return m_pAlpha[id]->value();
    }
    void SetAlpha( s32 id, s32 val ){
    	m_pAlpha[id]->setValue(val);
    }

	bool CheckLight( ) const {
    	return m_pLight->isChecked();
    }
private:
	QCheckBox* m_pLight;
	QSlider* m_pAlpha[GLWidget::MODEL_MAX];
	void SetCamera(const f32 yang);
public slots:
	void LightChanged(int);
	void OriginalAlphaChanged(int);
	void IdAlphaChanged(int);
	void SetCameraFront();
	void SetCameraBack();
	void SetCameraLeft();
	void SetCameraRight();
	void ShowAll();
};

#endif	/* __DISPLAYWIDGET_H__ */

