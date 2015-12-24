TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    vec_match.c

DISTFILES += \
    style.astylerc

HEADERS += \
    vec_match.h
