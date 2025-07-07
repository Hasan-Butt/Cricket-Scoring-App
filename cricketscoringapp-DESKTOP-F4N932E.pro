QT       += core gui
QT += multimedia
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    auctionmodescreen.cpp \
    demomodescreen.cpp \
    gameplayscreen.cpp \
    main.cpp \
    mainwindow.cpp \
    matchmodescreen.cpp \
    matchsummaryscreen.cpp \
    play.cpp \
    player.cpp \
    scorecardscreen.cpp \
    settingsscreen.cpp \
    statsscreen.cpp \
    team.cpp \
    tossscreen.cpp \
    tournamentmodescreen.cpp

HEADERS += \
    auctionmodescreen.h \
    demomodescreen.h \
    gameplayscreen.h \
    mainwindow.h \
    matchmodescreen.h \
    matchsummaryscreen.h \
    play.h \
    player.h \
    scorecardscreen.h \
    settingsscreen.h \
    statsscreen.h \
    team.h \
    tossscreen.h \
    tournamentmodescreen.h

FORMS += \
    auctionmodescreen.ui \
    demomodescreen.ui \
    gameplayscreen.ui \
    mainwindow.ui \
    matchmodescreen.ui \
    matchsummaryscreen.ui \
    scorecardscreen.ui \
    settingsscreen.ui \
    statsscreen.ui \
    tossscreen.ui \
    tournamentmodescreen.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icons.qrc \
    resources.qrc
