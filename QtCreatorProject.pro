QT += core gui opengl
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += openglwidgets

CONFIG += c++11

INCLUDEPATH += AppTinyMesh/Include
INCLUDEPATH += $$(GLEW_DIR)
INCLUDEPATH += $$(OUT_PWD)

VPATH += AppTinyMesh

SOURCES += \
    AppTinyMesh/Source/bezier.cpp \
    AppTinyMesh/Source/box.cpp \
    AppTinyMesh/Source/evector.cpp \
    AppTinyMesh/Source/implicits.cpp \
    AppTinyMesh/Source/main.cpp \
    AppTinyMesh/Source/camera.cpp \
    AppTinyMesh/Source/mesh.cpp \
    AppTinyMesh/Source/meshcolor.cpp \
    AppTinyMesh/Source/mesh-widget.cpp \
    AppTinyMesh/Source/node.cpp \
    AppTinyMesh/Source/qtemainwindow.cpp \
    AppTinyMesh/Source/ray.cpp \
    AppTinyMesh/Source/shader-api.cpp \
    AppTinyMesh/Source/signed.cpp \
    AppTinyMesh/Source/triangle.cpp \

HEADERS += \
    AppTinyMesh/Include/bezier.h \
    AppTinyMesh/Include/box.h \
    AppTinyMesh/Include/camera.h \
    AppTinyMesh/Include/color.h \
    AppTinyMesh/Include/implicits.h \
    AppTinyMesh/Include/mathematics.h \
    AppTinyMesh/Include/mesh.h \
    AppTinyMesh/Include/meshcolor.h \
    AppTinyMesh/Include/node.h \
    AppTinyMesh/Include/point.h \
    AppTinyMesh/Include/qte.h \
    AppTinyMesh/Include/realtime.h \
    AppTinyMesh/Include/sdf.h \
    AppTinyMesh/Include/shader-api.h \
    AppTinyMesh/Include/signed.h

FORMS += \
    AppTinyMesh/UI/interface.ui

win32 {
    LIBS += -L$$(GLEW_DIR) -lglew32
    LIBS += -lopengl32 -lglu32
}
unix:!macx {
    LIBS += -lGLEW -lGL -lGLU
}
macx {
    LIBS += -lGLEW -lGL -lGLU
}

# Copy shader files
# $$shell_quote puts quote around the path, to make it work if it contains space or other special characters.
copydata.commands = $(COPY_DIR) $$shell_quote($$PWD/AppTinyMesh/Shaders) $$shell_quote($$OUT_PWD/Shaders)
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)
QMAKE_EXTRA_TARGETS += first copydata

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
