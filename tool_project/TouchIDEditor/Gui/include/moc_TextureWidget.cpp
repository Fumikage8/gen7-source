/****************************************************************************
** Meta object code from reading C++ file 'TextureWidget.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "TextureWidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TextureWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MaterialWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x0a,
      28,   15,   15,   15, 0x0a,
      49,   15,   15,   15, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_MaterialWidget[] = {
    "MaterialWidget\0\0ChangeAll()\0"
    "MaterialChanged(int)\0SetCurrentTabIndex(int)\0"
};

void MaterialWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MaterialWidget *_t = static_cast<MaterialWidget *>(_o);
        switch (_id) {
        case 0: _t->ChangeAll(); break;
        case 1: _t->MaterialChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->SetCurrentTabIndex((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MaterialWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MaterialWidget::staticMetaObject = {
    { &QTabWidget::staticMetaObject, qt_meta_stringdata_MaterialWidget,
      qt_meta_data_MaterialWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MaterialWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MaterialWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MaterialWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MaterialWidget))
        return static_cast<void*>(const_cast< MaterialWidget*>(this));
    return QTabWidget::qt_metacast(_clname);
}

int MaterialWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTabWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}
static const uint qt_meta_data_MaterialComboBox[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_MaterialComboBox[] = {
    "MaterialComboBox\0\0SetCurrentIndex(int)\0"
};

void MaterialComboBox::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MaterialComboBox *_t = static_cast<MaterialComboBox *>(_o);
        switch (_id) {
        case 0: _t->SetCurrentIndex((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MaterialComboBox::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MaterialComboBox::staticMetaObject = {
    { &QComboBox::staticMetaObject, qt_meta_stringdata_MaterialComboBox,
      qt_meta_data_MaterialComboBox, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MaterialComboBox::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MaterialComboBox::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MaterialComboBox::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MaterialComboBox))
        return static_cast<void*>(const_cast< MaterialComboBox*>(this));
    return QComboBox::qt_metacast(_clname);
}

int MaterialComboBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QComboBox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
