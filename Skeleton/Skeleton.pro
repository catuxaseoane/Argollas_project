

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets



# Example linking with Basler and Opencv under Windows
##########################################################

INCLUDEPATH +=  "C:/Program Files/Basler/pylon 6/Development/include" \
                C:/Users/34619/opencv/build/include/opencv2 \
                C:/Users/34619/opencv/build/include

LIBS+=  -L"C:/Program Files/Basler/pylon 6/Development/lib/x64" \
        -LC:/Users/34619/opencv/build/x64 \ #Basler
        -LC:/Users/34619/opencv/build/x64/vc15/lib \
        -lWinmm

# Add J:\opencv\build\x64\vc15\bin to PATH in Windows's Control Panel

CONFIG(debug, debug|release) {

    LIBS+=  -lopencv_world455d
}


CONFIG(release, debug|release) {

    LIBS+=  -lopencv_world455
}
############################################################



# Example linking with Basler and OpenCV (static) under Linux
#################################################################
#
#QMAKE_LFLAGS += -no-pie -static-libgcc
#
#QMAKE_CXXFLAGS_WARN_ON += -Wno-unknown-pragmas # Avoids zillions of "Unkown pragma warning" from Pylon
#
#QMAKE_CXXFLAGS += -std=c++11
#
#INCLUDEPATH +=  /home/calderon/OpenCVStatic/include \
#                /opt/pylon5/include/genapi \
#                /opt/pylon5/include
#
#QMAKE_LFLAGS += -Wl,--enable-new-dtags -Wl,-rpath,/opt/pylon5/lib64
#
#LIBS+=  -L/opt/pylon5/lib64 \ #Basler
#        -Wl,-E -lpylonbase -lpylonutility -lGenApi_gcc_v3_0_Basler_pylon_v5_0 -lGCBase_gcc_v3_0_Basler_pylon_v5_0 \
#        -L/home/calderon/OpenCVStatic/lib \
#        -lopencv_dnn -lopencv_ml -lopencv_objdetect -lopencv_shape -lopencv_stitching -lopencv_superres \
#        -lopencv_videostab -lopencv_calib3d -lopencv_features2d -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs \
#        -lopencv_video -lopencv_photo -lopencv_imgproc -lopencv_flann -lopencv_core \
#        -L/home/calderon/OpenCVStatic/share/OpenCV/3rdparty/lib \
#        -lIlmImf -llibjasper \
#        -lz -lippiw -lippicv -littnotify
#################################################################


TARGET = Skeleton
TEMPLATE = app

# emits warnings if code uses any Qt feature marked as deprecated
DEFINES += QT_DEPRECATED_WARNINGS


CONFIG += c++11

SOURCES += \
        globals.cpp \
        main.cpp \
        mainwindow.cpp \
        worker.cpp

HEADERS += \
        globals.h \
        mainwindow.h \
        worker.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
