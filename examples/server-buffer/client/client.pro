TEMPLATE = app
TARGET = client
INCLUDEPATH += .

QT += waylandclient-private
!contains(QT_CONFIG, no-pkg-config) {
    CONFIG += link_pkgconfig
    PKGCONFIG += wayland-client
} else {
    LIBS += -lwayland-client
}
CONFIG += wayland-scanner
WAYLANDCLIENTSOURCES += ../share-buffer.xml

SOURCES += \
    main.cpp \
    serverbufferrenderer.cpp

HEADERS += \
    serverbufferrenderer.h
