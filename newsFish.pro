# The name of your app.
# NOTICE: name defined in TARGET has a corresponding QML filename.
#         If name defined in TARGET is changed, following needs to be
#         done to match new name:
#         - corresponding QML filename must be changed
#         - desktop icon filename must be changed
#         - desktop filename must be changed
#         - icon definition filename in desktop file must be changed
TARGET = newsFish
VERSION = 0.2

QT += declarative

CONFIG += sailfishapp

SOURCES += src/newsFish.cpp \
    src/Helper.cpp

OTHER_FILES += qml/newsFish.qml \
    qml/cover/CoverPage.qml \
    qml/pages/FirstPage.qml \
    qml/pages/SecondPage.qml \
    rpm/newsFish.spec \
    rpm/newsFish.yaml \
    newsFish.desktop \
    qml/pages/ItemView.qml \
    qml/pages/SettingsPage.qml \
    qml/pages/ItemPage.qml \
    qml/pages/FeedPage.qml

include(ownnews/ownnews.pri)

HEADERS += \
    src/Helper.h
