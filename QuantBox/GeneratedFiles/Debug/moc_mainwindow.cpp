/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[20];
    char stringdata[289];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_MainWindow_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10),
QT_MOC_LITERAL(1, 11, 12),
QT_MOC_LITERAL(2, 24, 0),
QT_MOC_LITERAL(3, 25, 15),
QT_MOC_LITERAL(4, 41, 17),
QT_MOC_LITERAL(5, 59, 15),
QT_MOC_LITERAL(6, 75, 17),
QT_MOC_LITERAL(7, 93, 18),
QT_MOC_LITERAL(8, 112, 5),
QT_MOC_LITERAL(9, 118, 18),
QT_MOC_LITERAL(10, 137, 20),
QT_MOC_LITERAL(11, 158, 21),
QT_MOC_LITERAL(12, 180, 20),
QT_MOC_LITERAL(13, 201, 12),
QT_MOC_LITERAL(14, 214, 7),
QT_MOC_LITERAL(15, 222, 13),
QT_MOC_LITERAL(16, 236, 7),
QT_MOC_LITERAL(17, 244, 14),
QT_MOC_LITERAL(18, 259, 11),
QT_MOC_LITERAL(19, 271, 16)
    },
    "MainWindow\0traderLogout\0\0loadMainAccount\0"
    "loadFollowAccount\0mainTraderLogin\0"
    "followTraderLogin\0connectFollow2Main\0"
    "index\0mainSettingClicked\0followSettingClicked\0"
    "beginFollowBtnClicked\0stopFollowBtnClicked\0"
    "updateStatus\0message\0updateBalance\0"
    "balance\0positionProfit\0closeProfit\0"
    "insertEventTable\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   79,    2, 0x06,

 // slots: name, argc, parameters, tag, flags
       3,    0,   80,    2, 0x08,
       4,    0,   81,    2, 0x08,
       5,    0,   82,    2, 0x08,
       6,    0,   83,    2, 0x08,
       7,    1,   84,    2, 0x08,
       9,    0,   87,    2, 0x08,
      10,    0,   88,    2, 0x08,
      11,    0,   89,    2, 0x08,
      12,    0,   90,    2, 0x08,
      13,    2,   91,    2, 0x08,
      15,    4,   96,    2, 0x08,
      19,    1,  105,    2, 0x08,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,    8,   14,
    QMetaType::Void, QMetaType::Int, QMetaType::Double, QMetaType::Double, QMetaType::Double,    8,   16,   17,   18,
    QMetaType::Void, QMetaType::QString,   14,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
        switch (_id) {
        case 0: _t->traderLogout(); break;
        case 1: _t->loadMainAccount(); break;
        case 2: _t->loadFollowAccount(); break;
        case 3: _t->mainTraderLogin(); break;
        case 4: _t->followTraderLogin(); break;
        case 5: _t->connectFollow2Main((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->mainSettingClicked(); break;
        case 7: _t->followSettingClicked(); break;
        case 8: _t->beginFollowBtnClicked(); break;
        case 9: _t->stopFollowBtnClicked(); break;
        case 10: _t->updateStatus((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 11: _t->updateBalance((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3])),(*reinterpret_cast< double(*)>(_a[4]))); break;
        case 12: _t->insertEventTable((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (MainWindow::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainWindow::traderLogout)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow.data,
      qt_meta_data_MainWindow,  qt_static_metacall, 0, 0}
};


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::traderLogout()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
