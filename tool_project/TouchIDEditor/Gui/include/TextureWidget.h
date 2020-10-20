//==============================================================================
/**
 * @file TextureWidget.h
 * @brief
 * @author kimura_shin
 * @data 2012/03/22, 12:45
 */
// =============================================================================

#if !defined( __TEXTUREWIDGET_H__ )
#define __TEXTUREWIDGET_H__

#include <gflib.h>
#include "PaintAction.h"

class TextureWidget;

class MaterialWidget : public QTabWidget 
{
  Q_OBJECT

public:
  gfl::cmdl::Material* m_pMaterial;
  QComboBox* m_pType;
  TextureWidget* m_pTextureWidget;
  s32 m_TypeIndex;
  void ChangeAllByValue( s32 type );
  public slots:
    void ChangeAll();
    void MaterialChanged(int);
    void SetCurrentTabIndex( int );
};

class MaterialComboBox : public QComboBox 
{
  Q_OBJECT

public:
  MaterialWidget* m_pMaterialWidget;

  public slots:
    void SetCurrentIndex( int );
};

/**
 * Texture widget
 */
class TextureWidget : public QWidget 
{
public:
  TextureWidget( void );

  virtual ~TextureWidget( void ) 
  {
    for( s32 i = 0; i < m_Images.size(); ++i )
    {
      delete m_Images.at(i);
    }
  }

  void LoadTexture( gfl::cmdl::Model* model, gfl::cmdl::Model* id_model );

  /**
   * ID タブを取得
   * @return IDタブ
   */

  enum {
#define SET_MATERIAL_TREATMENT(name,txt) MATERIAL_ ## name,
#include "TextureWidgetDef.h"
  };

  /**
   * マテリアルをどう処理するのか？
   * @param name マテリアル名
   * @return enum　処理番号　MATERIAL_
   */

  s32 GetMaterialTreatment( const QString& name )
  {
    return m_MaterialTreatment.value(name)->currentIndex();
  }

  QMap<QString, QComboBox*>* GetMaterialTreatment( )
  {
    return &m_MaterialTreatment;
  }

  QVector<MaterialWidget*>& GetMaterials( )
  {
    return m_Materials;
  }

  QTabWidget* GetOriginalTab( ) const
  {
    return m_pOriginalTab;
  }

  void SetTypeIndexChange( bool TypeIndexChange ) 
  {
    this->m_TypeIndexChange = TypeIndexChange;
  }

  bool IsTypeIndexChange( ) const
  {
    return m_TypeIndexChange;
  }

private:
  QVector<u8*> m_Images;
  QVector<MaterialWidget*> m_Materials;

  QTabWidget* m_pOriginalTab;
  QMap<QString,QComboBox*> m_MaterialTreatment;
  bool m_TypeIndexChange;
};

#endif /* __TEXTUREWIDGET_H__ */

