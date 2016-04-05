#ifndef BLEND_JSON_H
#define BLEND_JSON_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariant>

#include <QFile>

class blend_json : public QJsonDocument
{
public:
    blend_json();
    blend_json(const QString &filename);

    void setFileName(const QString &filename);

    void load_vars();
    void load_vars(const QString &filename);
    void save_vars();
    void save_vars(const QString &filename);

private:
    QFile file_handler;

};

#endif // BLEND_JSON_H
