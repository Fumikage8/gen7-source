/****************************************************************************
** Meta object code from reading C++ file 'gfl_QtProperty.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "gfl_QtProperty.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'gfl_QtProperty.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_gfl__QtProperty[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x0a,
      32,   16,   16,   16, 0x0a,
      47,   16,   16,   16, 0x0a,
      62,   16,   16,   16, 0x0a,
      77,   16,   16,   16, 0x0a,
      91,   16,   16,   16, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_gfl__QtProperty[] = {
    "gfl::QtProperty\0\0SetValue_S32()\0"
    "SetValue_U32()\0SetValue_S16()\0"
    "SetValue_U16()\0SetValue_S8()\0SetValue_U8()\0"
};

void gfl::QtProperty::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QtProperty *_t = static_cast<QtProperty *>(_o);
        switch (_id) {
        case 0: _t->SetValue_S32(); break;
        case 1: _t->SetValue_U32(); break;
        case 2: _t->SetValue_S16(); break;
        case 3: _t->SetValue_U16(); break;
        case 4: _t->SetValue_S8(); break;
        case 5: _t->SetValue_U8(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData gfl::QtProperty::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject gfl::QtProperty::staticMetaObject = {
    { &QSpinBox::staticMetaObject, qt_meta_stringdata_gfl__QtProperty,
      qt_meta_data_gfl__QtProperty, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &gfl::QtProperty::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *gfl::QtProperty::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *gfl::QtProperty::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_gfl__QtProperty))
        return static_cast<void*>(const_cast< QtProperty*>(this));
    return QSpinBox::qt_metacast(_clname);
}

int gfl::QtProperty::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSpinBox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}
static const uint qt_meta_data_gfl__QtPropertyFloat[] = {

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
      22,   21,   21,   21, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_gfl__QtPropertyFloat[] = {
    "gfl::QtPropertyFloat\0\0SetValue_F32()\0"
};

void gfl::QtPropertyFloat::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QtPropertyFloat *_t = static_cast<QtPropertyFloat *>(_o);
        switch (_id) {
        case 0: _t->SetValue_F32(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData gfl::QtPropertyFloat::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject gfl::QtPropertyFloat::staticMetaObject = {
    { &QDoubleSpinBox::staticMetaObject, qt_meta_stringdata_gfl__QtPropertyFloat,
      qt_meta_data_gfl__QtPropertyFloat, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &gfl::QtPropertyFloat::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *gfl::QtPropertyFloat::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *gfl::QtPropertyFloat::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_gfl__QtPropertyFloat))
        return static_cast<void*>(const_cast< QtPropertyFloat*>(this));
    return QDoubleSpinBox::qt_metacast(_clname);
}

int gfl::QtPropertyFloat::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDoubleSpinBox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_gfl__QtPropertyClass[] = {

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
      22,   21,   21,   21, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_gfl__QtPropertyClass[] = {
    "gfl::QtPropertyClass\0\0Clicked(bool)\0"
};

void gfl::QtPropertyClass::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QtPropertyClass *_t = static_cast<QtPropertyClass *>(_o);
        switch (_id) {
        case 0: _t->Clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData gfl::QtPropertyClass::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject gfl::QtPropertyClass::staticMetaObject = {
    { &QPushButton::staticMetaObject, qt_meta_stringdata_gfl__QtPropertyClass,
      qt_meta_data_gfl__QtPropertyClass, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &gfl::QtPropertyClass::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *gfl::QtPropertyClass::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *gfl::QtPropertyClass::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_gfl__QtPropertyClass))
        return static_cast<void*>(const_cast< QtPropertyClass*>(this));
    return QPushButton::qt_metacast(_clname);
}

int gfl::QtPropertyClass::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QPushButton::qt_metacall(_c, _id, _a);
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
