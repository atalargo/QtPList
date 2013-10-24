QT += xml xmlpatterns

INCLUDEPATH += $$PWD
SOURCES += \
        $$PWD/plistloader.cpp


HEADERS += \
        $$PWD/plistloader.h

options_image_provider = $$find(QT, "quick")

count( options_image_provider, 1) {
    SOURCES += \
        $$PWD/plistquickimageprovider.cpp \

    HEADERS += \
        $$PWD/plistquickimageprovider.h \
}

