TEMPLATE = app

# Make sure we do not accidentally #include files placed in 'resources'
CONFIG += no_include_pwd

SOURCES += $$PWD/src/*.cpp
SOURCES += $$PWD/lib/StanfordCPPLib/*.cpp

HEADERS += $$PWD/src/*.h
HEADERS += $$PWD/lib/StanfordCPPLib/*.h

# set up flags for the compiler and Stanford C++ libraries
QMAKE_CXXFLAGS += -std=c++0x \
    -Wall \
    -Wextra \
    -Wreturn-type \
    -Werror=return-type \
    -Wunreachable-code \
    -Wno-dangling-field \
    -Wno-missing-field-initializers \
    -Wno-sign-compare \
    -Wno-write-strings \
    -g \
    -O0 \
    -DSPL_CONSOLE_WIDTH=750 \
    -DSPL_CONSOLE_HEIGHT=400 \
    -DSPL_CONSOLE_FONTSIZE=16 \
    -DSPL_CONSOLE_ECHO \
    -DSPL_CONSOLE_EXIT_ON_CLOSE \
    -DSPL_CONSOLE_PRINT_EXCEPTIONS \
    -DSPL_CONSOLE_X=999999 \
    -DSPL_CONSOLE_Y=999999

INCLUDEPATH += $$PWD/lib/StanfordCPPLib/

# Copies the given files to the destination directory
# The rest of this file defines how to copy the resources folder
defineTest(copyToDestdir) {
    files = $$1

    for(FILE, files) {
        DDIR = $$OUT_PWD

        # Replace slashes in paths with backslashes for Windows
        win32:FILE ~= s,/,\\,g
        win32:DDIR ~= s,/,\\,g

        !win32 {
            copyResources.commands += cp -r '"'$$FILE'"' '"'$$DDIR'"' $$escape_expand(\\n\\t)
        }
        win32 {
            copyResources.commands += xcopy '"'$$FILE'"' '"'$$DDIR'"' /e /y $$escape_expand(\\n\\t)
        }
    }
    export(copyResources.commands)
}

!win32 {
    copyToDestdir($$files($$PWD/res/*))
    copyToDestdir($$files($$PWD/lib/*.jar))
}
win32 {
    copyToDestdir($$PWD/res)
    copyToDestdir($$PWD/lib/*.jar)
}

copyResources.input = $$files($$PWD/res/*)
OTHER_FILES = $$files(res/*)
QMAKE_EXTRA_TARGETS += copyResources
POST_TARGETDEPS += copyResources

# Make sure that release builds are statically linked and that .o files and the Makefile get nuked.
CONFIG(release, debug|release) {
    macx {
        QMAKE_POST_LINK += 'macdeployqt $${OUT_PWD}/$${TARGET}.app && rm $${OUT_PWD}/*.o && rm $${OUT_PWD}/Makefile'
    }
    unix:!macx {
        QMAKE_POST_LINK += 'rm $${OUT_PWD}/*.o && rm $${OUT_PWD}/Makefile'
    }
    win32 {
        TARGET_PATH = $${OUT_PWD}/release/$${TARGET}.exe
        TARGET_PATH ~= s,/,\\,g

        OUT_PATH = $${OUT_PWD}/
        OUT_PATH ~= s,/,\\,g

        REMOVE_DIRS = '$${OUT_PWD}/release \
            $${OUT_PWD}/debug'
        REMOVE_FILES = '$${OUT_PWD}/Makefile \
            $${OUT_PWD}/Makefile.Debug \
            $${OUT_PWD}/Makefile.Release \
            $${OUT_PWD}/object_script.$${TARGET}.Release \
            $${OUT_PWD}/object_script.$${TARGET}.Debug'
        REMOVE_DIRS ~= s,/,\\,g
        REMOVE_FILES ~= s,/,\\,g

        QMAKE_LFLAGS = -static -static-libgcc -static-libstdc++
        QMAKE_POST_LINK += 'move $${TARGET_PATH} $${OUT_PWD} \
            && rmdir /s /q $${REMOVE_DIRS} \
            && del $${REMOVE_FILES}'
    }
}

macx {
    cache()
    QMAKE_MAC_SDK = macosx
}
