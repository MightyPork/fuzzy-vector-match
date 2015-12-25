TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    vec_match.c \
    src/vec_match.c

DISTFILES += \
    style.astylerc \
    Makefile

HEADERS += \
    vec_match.h \
    src/vec_match.h
