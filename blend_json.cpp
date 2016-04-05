#include "blend_json.h"

blend_json::blend_json()
{
}

blend_json::blend_json(const QString &filename) {
    setFileName(filename);
    load_vars();
}

void blend_json::setFileName(const QString &filename) {
    file_handler.setFileName(filename);
}

void blend_json::load_vars() {
    if(file_handler.isOpen()) file_handler.close();
    if(!file_handler.exists()) return;

    file_handler.open(QFile::ReadOnly | QFile::Text);
    if(!file_handler.isOpen()) return;
    setObject(fromJson(QString(file_handler.readAll()).toUtf8()).object());

    file_handler.close();
}

void blend_json::save_vars() {
    if(file_handler.isOpen()) file_handler.close();
    //if(!file_handler.exists()) return; // not used.

    file_handler.open(QFile::WriteOnly | QFile::Text);
    if(!file_handler.isOpen()) return;

    //setObject();
    file_handler.write(toJson(QJsonDocument::Indented));

    file_handler.close();
}

void blend_json::load_vars(const QString &filename) {
    setFileName(filename);
    if(file_handler.isOpen()) file_handler.close();
    if(!file_handler.exists()) return; // not used.

    if(!file_handler.open(QFile::ReadOnly | QFile::Text)) return;
    setObject(fromJson(QString(file_handler.readAll()).toUtf8()).object());
    //qDebug(QString("[Read from file \"%1\"] %2").arg(filename).arg(QString(a)).toLocal8Bit().constData());

    file_handler.close();
}

void blend_json::save_vars(const QString &filename) {
    setFileName(filename);
    if(file_handler.isOpen()) file_handler.close();
    if(!file_handler.exists()) return; // not used.

    if(!file_handler.open(QFile::WriteOnly | QFile::Text)) return;
    file_handler.write(toJson(QJsonDocument::Indented));
    //setObject(fromJson(QString(file_handler.readAll()).toUtf8()).object());
    //qDebug(QString("[Read from file \"%1\"] %2").arg(filename).arg(QString(a)).toLocal8Bit().constData());

    file_handler.close();
}
