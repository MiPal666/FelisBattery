TARGET = harbour-felisbattery

CONFIG += sailfishapp c++11 sailfishapp_i18n
QT += core qml quick

SOURCES += \
    src/main.cpp \
    src/batterymonitor.cpp

HEADERS += \
    src/batterymonitor.h

DISTFILES += \
    harbour-felisbattery.desktop \
    qml/harbour-felisbattery.qml \
    qml/pages/MainPage.qml \
    qml/pages/TechnicalPage.qml \
    qml/pages/AboutPage.qml \
    qml/components/CompactValue.qml \
    qml/components/LeftPageHeader.qml \
    qml/components/LeftSectionHeader.qml \
    qml/cover/CoverPage.qml \
    rpm/harbour-felisbattery.spec \
    rpm/harbour-felisbattery.changes \
    translations/*.ts \
    README.md \
    LICENSE

TRANSLATIONS += translations/harbour-felisbattery-cs.ts

SAILFISHAPP_ICONS = 86x86 108x108 128x128 172x172
