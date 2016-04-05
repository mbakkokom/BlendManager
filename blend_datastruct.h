#ifndef BLEND_DATASTRUCT_H
#define BLEND_DATASTRUCT_H

#include <QStringList>

// should be a class
struct BlendVersioning {
    // basic info
    int major;
    int minor;
    QString rev;
    int OS;
    QString branch;

    // is downloaded?
    QString downloadURL;
    QString dhash;

    // local
    bool downloaded;
    QString path;
    QString exec_bin;

    QStringList arguments;
};

enum BLENDER_OS {
    BLENDER_OS_WIN32,
    BLENDER_OS_WIN64,
    BLENDER_OS_LINUX32,
    BLENDER_OS_LINUX64,
    BLENDER_OS_MACOSX64,
    BLENDER_OS_FREEBSD64
};

#endif // BLEND_DATASTRUCT_H
