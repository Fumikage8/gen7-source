/****************************************************************************
** Meta object code from reading C++ file 'ToolWidget.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "ToolWidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ToolWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ToolWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x0a,
      22,   11,   11,   11, 0x0a,
      39,   11,   11,   11, 0x0a,
      61,   11,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ToolWidget[] = {
    "ToolWidget\0\0Clicked()\0LineChanged(int)\0"
    "ToleranceChanged(int)\0PenetrateChanged(int)\0"
};

void ToolWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ToolWidget *_t = static_cast<ToolWidget *>(_o);
        switch (_id) {
        case 0: _t->Clicked(); break;
        case 1: _t->LineChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->ToleranceChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->PenetrateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ToolWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ToolWidget::staticMetaObject = {
    { &QGroupBox::staticMetaObject, qt_meta_stringdata_ToolWidget,
      qt_meta_data_ToolWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ToolWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ToolWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ToolWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ToolWidget))
        return static_cast<void*>(const_cast< ToolWidget*>(this));
    return QGroupBox::qt_metacast(_clname);
}

int ToolWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGroupBox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
