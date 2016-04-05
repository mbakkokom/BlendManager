#include "blend_install.h"

blend_install::blend_install(QObject *parent) :
    QObject(parent)
{
}

blend_install::~blend_install() {

}

blend_install::blend_install(
        const QString &fileURL,
        const QString &downloadFileName,
        const QString &downloadDir,
        const QString &installBasePath,
        const QString &extractProgram,
        QStringList extractArguments)
{
    setFileURL(fileURL);
    setDownloadFileName(downloadFileName);
    setDownloadDirectory(downloadDir);
    setInstallBasePath(installBasePath);
    setExtractProgram(extractProgram);
    setExtractArguments(extractArguments);

    download();
}

void blend_install::setFileURL(const QString &fileURL) {
    downloadFrom = fileURL;
}

void blend_install::setDownloadFileName(const QString &filename) {
    downloadTo = filename;
}

void blend_install::setDownloadDirectory(const QString &dir) {
    downloadIn = dir;
}

void blend_install::setInstallBasePath(const QString &path) {
    installTo = path;
}

void blend_install::setExtractProgram(const QString &ex) {
    extractWith = ex;
}

void blend_install::setExtractArguments(QStringList ag) {
    extractUsing = ag;
}

QString blend_install::fileURL() {
    return downloadFrom;
}

QString blend_install::downloadFileName() {
    return downloadTo;
}

QString blend_install::downloadDirectory() {
    return downloadIn;
}

QString blend_install::installBasePath() {
    return installTo;
}

QString blend_install::extractProgram() {
    return extractWith;
}

QStringList blend_install::extractArguments() {
    return extractUsing;
}

void blend_install::download() {
    error_code = BLEND_INSTALL_NO_ERROR;
    current_prog = BLEND_INSTALL_NONE;
    emit progress_changed(current_prog);

    manager = new QNetworkAccessManager;

    file_handler = new QFile(downloadTo);

    /*
    if (file_handler->exists()) {
        error_code = BLEND_INSTALL_FILE_EXISTS;
        current_prog = BLEND_INSTALL_NONE;
        emit progress_changed(current_prog);
        emit error_occured(error_code);
        emit finished();
        return;
    }*/

    file_handler->open(QFile::WriteOnly);
    if (!file_handler->isOpen()) {
        error_code = BLEND_INSTALL_FILE_NOT_OPEN;
        current_prog = BLEND_INSTALL_NONE;
        emit progress_changed(current_prog);
        emit error_occured(error_code);
        emit finished();
        return;
    }

    current_prog = BLEND_INSTALL_DOWNLOAD;
    emit progress_changed(current_prog);

    QNetworkRequest request;
    request.setUrl(QUrl(downloadFrom));


    reply = manager->get(request);

    event = new QEventLoop;
    connect(reply,SIGNAL(finished()),event,SLOT(quit()));
    connect(reply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(downloadError(QNetworkReply::NetworkError)));
    connect(reply,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(downloadProgressL(qint64,qint64)));

    event->exec();

    //if (error_code == BLEND_INSTALL_DOWNLOAD_FAILED) {
    if (error_code != BLEND_INSTALL_NO_ERROR) {
        current_prog = BLEND_INSTALL_NONE;

        //disconnect(reply,SIGNAL(finished()),event,SLOT(quit()));
        disconnect(reply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(downloadError(QNetworkReply::NetworkError)));
        disconnect(reply,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(downloadProgressL(qint64,qint64)));

        emit progress_changed(current_prog);
        emit error_occured(error_code);
        emit finished();
        return;
    }

    //QString f = ;
    file_handler->write(reply->readAll());
    reply->deleteLater();
    file_handler->close();

    /*
    if (file_handler != QDataStream::Ok) {
        error_code = BLEND_INSTALL_FILE_WRITE_ERROR;
        current_prog = BLEND_INSTALL_NONE;
        emit progress_changed(current_prog);
        emit error_occured(error_code);
        emit finished();
        return;
    }*/

    QStringList ar;
    foreach (QString i, extractUsing)
        ar.append(i.replace("%{FILE}", downloadTo).replace("%{OUTPUT_DIR}", installTo));

    current_prog = BLEND_INSTALL_EXTRACT;
    emit progress_changed(current_prog);

    proc.setProgram(extractWith);
    proc.setArguments(ar);
    proc.setWorkingDirectory(downloadIn);
    proc.setCloseOnFinished(true);

    event = new QEventLoop;
    connect(&proc,SIGNAL(finished(int)),event,SLOT(quit()));
    connect(&proc,SIGNAL(error(QProcess::ProcessError)),this,SLOT(extractError(QProcess::ProcessError)));
    connect(&proc,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(extractFinished(int,QProcess::ExitStatus)));

    proc.start();
    proc.open_console();

    event->exec();

    QFile lg(downloadFileName() + ".extract_log");
    if(lg.open(QFile::WriteOnly | QFile::Text)) {
        lg.write(proc.readOutput().toUtf8());
        lg.close();
    }
    lg.deleteLater();

    if (error_code == BLEND_INSTALL_EXTRACT_FAILED) {
        current_prog = BLEND_INSTALL_NONE;
        emit progress_changed(current_prog);
        emit error_occured(error_code);
        emit finished();
        return;
    }

    current_prog = BLEND_INSTALL_FINISH;
    emit progress_changed(current_prog);

    ar.clear(); // recycling.
    QDir i;
    i.setPath(installTo);
    ar = i.entryList();

    QString fn = file_handler->fileName();
    QString bingo;

    //qDebug(fn.toLocal8Bit().constData());

    foreach(QString j, ar) { // TODO. modernize.
        //qDebug(QString(j).toLocal8Bit().constData());
        if(j == ".") continue;
        if(fn.contains(j)) {
            bingo = j;
            break;
        }
    }

    if (bingo == "") {
        error_code = BLEND_INSTALL_FOLDER_NOT_FOUND;
        current_prog = BLEND_INSTALL_NONE;
        emit progress_changed(current_prog);
        emit error_occured(error_code);
        emit finished();
        return;
    }

    install_ok = true;

    current_prog = BLEND_INSTALL_NONE;
    emit progress_changed(current_prog);

    installOn = i.absolutePath() + QString("/") + bingo;

    emit installed(installOn);
    emit finished();

    //qDebug("It's finished installing!");
}

void blend_install::downloadError(QNetworkReply::NetworkError ext) {
    error_code = BLEND_INSTALL_DOWNLOAD_FAILED;
}

void blend_install::downloadProgressL(qint64 dwn, qint64 tot) {
    progress_percent = (dwn / tot);
    if(prog_bar.maximum()!=tot) prog_bar.setMaximum(tot);
    prog_bar.setValue(dwn);
    //emit download_progress_changed(progress_percent);
    /*
    qDebug(QString(
           QString("%1 [%2 %3]")
               .arg((dwn / tot) * 100)
               .arg(dwn)
               .arg(tot)
                ).toLocal8Bit().constData());*/
}

void blend_install::extractError(QProcess::ProcessError error) {
    error_code = BLEND_INSTALL_EXTRACT_FAILED;
}

void blend_install::extractFinished(int ret, QProcess::ExitStatus ext) {

}

void blend_install::show_progress() {
    if (!is_render) render_progress();
    prog_manager.show();
}

void blend_install::render_progress() {

    main_lay.setMargin(5);

    bef.addStretch(1);
    prog_btn_folder.setText("Open Folder");
    prog_btn_folder.hide();
    bef.addWidget(&prog_btn_folder);
    prog_btn_abort.setText("Abort");
    //prog_btn_abort.hide();
    bef.addWidget(&prog_btn_abort);
    prog_btn_close.setText("Close");
    prog_btn_close.hide();
    bef.addWidget(&prog_btn_close);

    //prog_lbl.setText();
    prog_lbl.setWordWrap(true);
    prog_lbl.setFixedWidth(275);
    prog_lbl.adjustSize();
    main_lay.addWidget(&prog_lbl);
    prog_bar.setMinimum(0);
    prog_bar.setMaximum(100);

    main_lay.addWidget(&prog_bar);
    main_lay.addLayout(&bef);

    prog_manager.setLayout(&main_lay);
    //prog_manager.setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    prog_manager.setFixedSize(300,200);

    connect(this, SIGNAL(progress_changed(BLEND_INSTALL_PROGRESS)), this, SLOT(man_progresslbl(BLEND_INSTALL_PROGRESS)));
    connect(this, SIGNAL(installed(QString)), this, SLOT(man_installed(QString)));
    connect(this, SIGNAL(finished()), this, SLOT(man_finished()));
    connect(this, SIGNAL(error_occured(BLEND_INSTALL_ERROR)), this, SLOT(man_erroroccured(BLEND_INSTALL_ERROR)));
    connect(&prog_btn_abort, SIGNAL(released()), this, SLOT(man_abort()));
    connect(&prog_btn_close, SIGNAL(released()), &prog_manager, SLOT(close()));
    connect(&prog_btn_folder, SIGNAL(released()), this, SLOT(man_folder()));

    is_render = true;
}

void blend_install::hide_progress() {
    prog_manager.hide();
}

void blend_install::man_progresslbl(BLEND_INSTALL_PROGRESS prog) {
    switch (prog) {
    case BLEND_INSTALL_NONE:
        if (error_code != BLEND_INSTALL_NO_ERROR) {
            // given.
            prog_btn_close.show();
            prog_btn_abort.hide();
        } else prog_lbl.setText("Waiting...");
        break;
    case BLEND_INSTALL_DOWNLOAD:
        prog_lbl.setText("Downloading...");
        prog_btn_close.hide();
        prog_btn_abort.show();
        prog_btn_folder.hide();
        break;
    case BLEND_INSTALL_EXTRACT:
        prog_lbl.setText("Extracting...");
        break;
    case BLEND_INSTALL_FINISH:
        prog_lbl.setText("Finishing...");
        break;
    default:
        break;
    }
}

void blend_install::man_installed(QString install_path) {
    prog_lbl.setText(QString("Blender installed on %1.").arg(install_path));
    prog_btn_folder.show();
    emit finished();
}

void blend_install::man_finished() {
    prog_btn_abort.hide();
    prog_btn_close.show();
}

void blend_install::man_abort() {
    switch(current_prog) {
        case BLEND_INSTALL_DOWNLOAD:
            reply->abort();
            //reply->finished();
            emit finished();
            break;
        case BLEND_INSTALL_EXTRACT:
            proc.terminate();
            error_code = BLEND_INSTALL_EXTRACT_FAILED;
            emit finished();
            break;
        default:
            break;
    }
}

void blend_install::man_folder() {
    if(QDesktopServices::openUrl(QUrl(installTo, QUrl::TolerantMode)) == false)
        QMessageBox::warning(&prog_manager, "Error", QString("Couldn't open \"%1\"!").arg(installTo), QMessageBox::Ok, QMessageBox::Ok);
    return;
}

void blend_install::man_erroroccured(BLEND_INSTALL_ERROR err) {
    switch(err) {
        case BLEND_INSTALL_FILE_EXISTS:
            prog_lbl.setText("Error: Conflicting file exists.");
            break;
        case BLEND_INSTALL_FILE_NOT_OPEN:
            prog_lbl.setText("Error: Could not open output file.");
            break;
        case BLEND_INSTALL_FILE_WRITE_ERROR:
            prog_lbl.setText("Error: Could not write to file.");
            break;
        case BLEND_INSTALL_DOWNLOAD_FAILED:
            prog_lbl.setText("Error: Could not download file.");
            break;
        case BLEND_INSTALL_EXTRACT_FAILED:
            prog_lbl.setText("Error: Could not extract file.");
            break;
        case BLEND_INSTALL_FOLDER_NOT_FOUND:
            prog_lbl.setText("Error: Could not find extracted folder.");
            break;
        default:
            break;
    }
    emit finished();
}

QString blend_install::installedOn() {
    return installOn;
}

bool blend_install::is_progress() {
    return prog_manager.isVisible();
}
