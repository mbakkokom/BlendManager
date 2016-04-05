#ifndef BLEND_INSTALL_H
#define BLEND_INSTALL_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QDataStream>
#include <QStringList>
#include <QDir>
#include <QEventLoop>
#include <QtMath>
#include <QProcess>

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QDesktopServices>
#include <QMessageBox>

#include "blend_exec.h"

class blend_install : public QObject
{
    Q_OBJECT
public:
    blend_install(QObject *parent = 0);
    blend_install(
            const QString &fileURL,
            const QString &downloadFileName,
            const QString &downloadDir,
            const QString &installBasePath,
            const QString &extractProgram,
            QStringList extractArguments);

    virtual ~blend_install();

    enum BLEND_INSTALL_ERROR {
        BLEND_INSTALL_NO_ERROR,
        BLEND_INSTALL_FILE_EXISTS,
        BLEND_INSTALL_FILE_NOT_OPEN,
        BLEND_INSTALL_FILE_WRITE_ERROR,
        BLEND_INSTALL_DOWNLOAD_FAILED,
        BLEND_INSTALL_EXTRACT_FAILED,
        BLEND_INSTALL_FOLDER_NOT_FOUND
    };

    enum BLEND_INSTALL_PROGRESS {
        BLEND_INSTALL_NONE,
        BLEND_INSTALL_DOWNLOAD,
        BLEND_INSTALL_EXTRACT,
        BLEND_INSTALL_FINISH
    };

    void setFileURL(const QString &fileURL);
    void setDownloadFileName(const QString &filename);
    void setDownloadDirectory(const QString &dir);
    void setInstallBasePath(const QString &path);

    void setExtractProgram(const QString &ex);
    void setExtractArguments(QStringList ag);

    QString fileURL();
    QString downloadFileName();
    QString downloadDirectory();
    QString installBasePath();

    QString extractProgram();
    QStringList extractArguments();

    QString installedOn();

    BLEND_INSTALL_ERROR error_code = BLEND_INSTALL_NO_ERROR;

    void download();
    void is_installed();

    void show_progress();
    void hide_progress();
    bool is_progress();

signals:
    void error_occured(BLEND_INSTALL_ERROR err);
    void progress_changed(BLEND_INSTALL_PROGRESS prog);
    void installed(QString install_path);
    void finished();

private slots:
    void downloadError(QNetworkReply::NetworkError ext);
    void downloadProgressL(qint64 dwn, qint64 tot);
    void extractError(QProcess::ProcessError error);
    void extractFinished(int ret, QProcess::ExitStatus ext);

    void man_erroroccured(BLEND_INSTALL_ERROR err);

    void man_progresslbl(BLEND_INSTALL_PROGRESS prog);
    void man_installed(QString install_path);
    void man_finished();

    void man_abort();
    //void man_close();
    void man_folder();

private:

    void render_progress();

    BLEND_INSTALL_PROGRESS current_prog;

    bool is_render = false;
    bool install_ok = false;

    QString downloadFrom;
    QString downloadTo;
    QString downloadIn;
    QString installTo;

    QString extractWith;
    QStringList extractUsing;

    QEventLoop *event;

    QString installOn;

    qint64 progress_percent;

    QWidget prog_manager;
    QLabel prog_lbl;
    QProgressBar prog_bar;
    QPushButton prog_btn_folder;
    QPushButton prog_btn_abort;
    QPushButton prog_btn_close;
    QVBoxLayout main_lay;
    QHBoxLayout bef;

    blend_exec proc;

    QNetworkAccessManager *manager;
    QNetworkReply *reply;
    QFile *file_handler;
};

#endif // BLEND_INSTALL_H
