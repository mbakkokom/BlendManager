#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent)
{

    QVBoxLayout *main_layout = new QVBoxLayout;

    QHBoxLayout *header_layout = new QHBoxLayout;
    QVBoxLayout *subhead_layout = new QVBoxLayout;

    QImage img_ic;
    img_ic.load(":/icons/blender256.png");
    lbltop_icon.setPixmap(QPixmap::fromImage(img_ic).scaled(128,128,Qt::KeepAspectRatio, Qt::SmoothTransformation));
    lbltop_icon.setFixedWidth(138);
    lbltop_icon.setStyleSheet("padding: 5px;");

    lbltop_name.setText("BlendManager");
    lbltop_details.setText("This version is updated!");
    subhead_layout->addWidget(&lbltop_name);
    subhead_layout->addWidget(&lbltop_details);
    subhead_layout->addStretch(1);

    QHBoxLayout *action_lod = new QHBoxLayout;
    lblact_stat.setText("I'm doing something here...");
    lblact_stat.hide();
    //btnAct.setText("Cancel");
    btnAct.setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogCancelButton));
    btnAct.setToolTip("Terminate");
    btnAct.hide();
    btnTggConsole.setIcon(QApplication::style()->standardIcon(QStyle::SP_TitleBarNormalButton)); //
    btnTggConsole.setToolTip("Toggle Console");
    btnTggConsole.hide();
    action_lod->addWidget(&lblact_stat, 1);
    action_lod->addWidget(&btnTggConsole);
    action_lod->addWidget(&btnAct);

    subhead_layout->addLayout(action_lod);

    QHBoxLayout *subhead_menus = new QHBoxLayout;
    btnUpdate.setText("Update Versions");
    btnAbout.setText("About");
    subhead_menus->addWidget(&btnUpdate);
    subhead_menus->addWidget(&btnAbout);

    subhead_layout->addLayout(subhead_menus);

    header_layout->addWidget(&lbltop_icon);
    header_layout->addLayout(subhead_layout);

    main_layout->addLayout(header_layout);

    viewer->setStyleSheet("QListWidget::item { padding: 10px; };");
    main_layout->addWidget(viewer, 1);

    setLayout(main_layout);

    setFixedHeight(600);
    setFixedWidth(400);
    setWindowTitle("BlendManager: Aloha!");
    setWindowIcon(QIcon(":/icons/blender48.png"));

    connect(viewer, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(seeText(QListWidgetItem*)));
    connect(&btnTggConsole, SIGNAL(released()), this, SLOT(seeConsole()));
    connect(&btnAct, SIGNAL(released()), this, SLOT(seeBlendExit()));
    connect(&btnAbout, SIGNAL(released()), this, SLOT(seeAbout()));
    connect(&btnUpdate, SIGNAL(released()), this, SLOT(seeUpdate()));
    connect(&blendproc, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(seeBlendProc(QProcess::ProcessState)));
    connect(&blendproc, SIGNAL(finished(int)), this, SLOT(seeBlendProcFinished(int)));

    renderOkay = true;

    //renderBlenderVersions();

    //ui->setupUi(this);
}

QList<BlendVersioning> MainWindow::convertJsontoBlendVersioning(QJsonObject json_obj) {
    QList<BlendVersioning> ret;
    BlendVersioning tmp;
    foreach(const QJsonValue &value, json_obj) {
        tmp = BlendVersioning();
        QJsonObject obj = value.toObject();
        tmp.major = obj["major"].toInt();
        tmp.minor = obj["minor"].toInt();
        tmp.rev = obj["rev"].toString();
        if (obj["OS"].toString() == "BLENDER_OS_WIN32") tmp.OS = BLENDER_OS_WIN32;
        else if (obj["OS"].toString() == "BLENDER_OS_WIN64") tmp.OS = BLENDER_OS_WIN64;
        else if (obj["OS"].toString() == "BLENDER_OS_LINUX32") tmp.OS = BLENDER_OS_LINUX32;
        else if (obj["OS"].toString() == "BLENDER_OS_LINUX64") tmp.OS = BLENDER_OS_LINUX64;
        else if (obj["OS"].toString() == "BLENDER_OS_MACOSX64") tmp.OS = BLENDER_OS_MACOSX64;
        else if (obj["OS"].toString() == "BLENDER_OS_FREEBSD64") tmp.OS = BLENDER_OS_FREEBSD64;
        tmp.branch = obj["branch"].toString();
        tmp.downloadURL = obj["downloadURL"].toString();
        tmp.dhash = obj["downloadHash"].toString();
        tmp.downloaded = obj["localAvailable"].toBool();
        tmp.path = obj["localPath"].toString();
        tmp.exec_bin = obj["localExecutable"].toString();

        QJsonValue t;
        foreach(t, obj["localArguments"].toArray()) tmp.arguments.push_back(t.toString());

        qDebug(
                    QString("Blender %1.%2%3 [os=%4] [branch=%5] [downloaded=%6]")
                    .arg(tmp.major)
                    .arg(tmp.minor)
                    .arg(tmp.rev)
                    .arg(tmp.OS)
                    .arg(tmp.branch)
                    .arg(tmp.downloaded)
                    .toLocal8Bit().constData()
                    );

        ret.push_back(tmp);
    }
    return ret;
}

QJsonObject MainWindow::convertBlendVersioningtoJson(QList<BlendVersioning> bversion) {
    QJsonObject ret;

    int tt = 0;
    foreach(BlendVersioning bv, bversion) {
        QJsonObject l;
        QJsonArray ag;
        l.insert("major", QJsonValue(bv.major));
        l.insert("minor", QJsonValue(bv.minor));

        if (bv.OS == BLENDER_OS_WIN32) l.insert("OS", QJsonValue(QString("BLENDER_OS_WIN32")));
        else if (bv.OS == BLENDER_OS_WIN64) l.insert("OS", QJsonValue(QString("BLENDER_OS_WIN64")));
        else if (bv.OS == BLENDER_OS_LINUX32) l.insert("OS", QJsonValue(QString("BLENDER_OS_LINUX32")));
        else if (bv.OS == BLENDER_OS_LINUX64) l.insert("OS", QJsonValue(QString("BLENDER_OS_LINUX64")));
        else if (bv.OS == BLENDER_OS_MACOSX64) l.insert("OS", QJsonValue(QString("BLENDER_OS_MACOSX64")));
        else if (bv.OS == BLENDER_OS_FREEBSD64) l.insert("OS", QJsonValue(QString("BLENDER_OS_FREEBSD64")));

        l.insert("branch", QJsonValue(bv.branch));
        l.insert("downloadURL", QJsonValue(bv.downloadURL));
        l.insert("downloadHash", QJsonValue(bv.dhash));
        l.insert("localAvailable", QJsonValue(bv.downloaded));
        l.insert("localPath", QJsonValue(bv.path));
        l.insert("localExecutable", QJsonValue(bv.exec_bin));

        foreach(QString k, bv.arguments) ag.append(QJsonValue(k));
        l.insert("localArguments", QJsonValue(ag));

        QString lnk = QUrl(bv.downloadURL).fileName();
        if (lnk == "") ret.insert(QString(tt), l);
        else ret.insert(lnk, l);

        tt++;
    }
    return ret;
}

void MainWindow::seeText(QListWidgetItem *idx) {
    //idx->setBackgroundColor(QColor(0,0,255));

    if (progress_o != -1) {
        QMessageBox::warning(this, "BlendManager",
                             QString("There's already a running progress!"),
                             QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    QStringList ag;

    QVariant data = idx->data(Qt::UserRole);
    int vid = data.toInt();

    if (vid < 0 || vid >= blendvers.size()) return;

    // blendproc.processId()
    if (blendproc.pid() == 0 && blendproc.state() == QProcess::NotRunning && blendvers[vid].downloaded == true) {

        QMessageBox *msg = new QMessageBox(this);
        msg->setText("This will execute Blender according to your configuration.");
        msg->setInformativeText(QString("This will execute \"%1\"\nfrom \"%2\".")
                                .arg(blendvers[vid].exec_bin)
                                .arg(blendvers[vid].path));
        QPushButton *cancel = msg->addButton(QMessageBox::Cancel);
        QPushButton *ok = msg->addButton(QMessageBox::Ok);
        QPushButton *op_f = msg->addButton("Open Folder",QMessageBox::NoRole);
        msg->setDefaultButton(cancel);
        msg->setIcon(QMessageBox::Warning);

        msg->exec();

        if(msg->clickedButton() == ok) {
            progress_o = 1; // Running Blender

            //blendproc = new QProcess(this);

            if (blendsettings.object()["ignoreCommandLineArguments"].toBool() != true)
                for (int i=1; i<ag_int; i++) ag << ag_char[i];
            foreach(QJsonValue p, blendsettings.object()["forceArguments"].toArray())
                ag << p.toString();

            ag.append(blendvers[vid].arguments);

            startBlender(blendvers[vid].exec_bin, blendvers[vid].path, ag);

            /*
            blendproc.close_console();

            blendproc.setWorkingDirectory(blendvers[vid].path);
            blendproc.setProgram(blendvers[vid].exec_bin);
            blendproc.setArguments(ag);
            blendproc.start();

            blendproc.open_console();
            */

            progress_o = -1;
        } else if (msg->clickedButton() == op_f) {
            if(QDesktopServices::openUrl(QUrl(blendvers[vid].path, QUrl::TolerantMode)) == false)
                QMessageBox::warning(this, "Error", QString("Couldn't open \"%1\"!").arg(blendvers[vid].path), QMessageBox::Ok);
        }


    } else if ( !blendvers[vid].downloaded ) {

        // TODO. Download.

        QMessageBox *msg = new QMessageBox(this);
        msg->setText("This will download your preffered Blender version.");
        msg->setDefaultButton(QMessageBox::Cancel);
        msg->setInformativeText(
                    (blendvers[vid].branch == "vanilla") ?
                        QString("This will download Blender %1.%2.")
                        .arg(blendvers[vid].major)
                        .arg(blendvers[vid].minor) :
                        QString("This will download Blender %1.%2 (%3).")
                        .arg(blendvers[vid].major)
                        .arg(blendvers[vid].minor)
                        .arg(blendvers[vid].branch)
                        );
        msg->setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
        msg->setIcon(QMessageBox::Warning);

        int ret = msg->exec();

        if ( ret == QMessageBox::Ok ) {

            progress_o = 2; // Downloading Blender

            // TODO. uses aria2, update with own downloader.
            //blendproc = new QProcess(this);
            /*
                blendproc.close_console();

                QStringList a;
                a << QString("%1").arg(blendvers[vid].downloadURL);
                a << jsontoStringList(blendsettings.object()["aria2Arguments"].toArray());

                blendproc.setWorkingDirectory(blendsettings.object()["aria2WorkingDir"].toString());
                blendproc.setProgram(blendsettings.object()["aria2Executable"].toString());
                blendproc.setArguments(a);

                QEventLoop ac;
                connect(&blendproc, SIGNAL(finished(int)), &ac, SLOT(quit()));

                blendproc.start();
                blendproc.open_console();

                ac.exec();
                */
            /*
                qDebug(QString("%1/%2").arg(
                       blendsettings.object()["defaultDownloadDir"].toString()).arg(
                       QUrl(blendvers[vid].downloadURL).fileName()
                       ).toLocal8Bit().constData());
                qDebug(QString(blendvers[vid].downloadURL).toLocal8Bit().constData());
                */

            lblact_stat.setText("Downloading Blender...");
            lblact_stat.show();
            btnTggConsole.show();

            blendinstaller.setFileURL(blendvers[vid].downloadURL);
            blendinstaller.setDownloadFileName(
                        QString(blendsettings.object()["defaultDownloadDir"].toString()) +
                    QString("/") +
                    QUrl(blendvers[vid].downloadURL).fileName()
                    );
            blendinstaller.setDownloadDirectory(blendsettings.object()["defaultDownloadDir"].toString());
            blendinstaller.setInstallBasePath(blendsettings.object()["defaultBlenderRoot"].toString());
            blendinstaller.setExtractProgram(blendsettings.object()["extractCommandExecutable"].toString());
            blendinstaller.setExtractArguments(jsontoStringList(blendsettings.object()["extractCommandArguments"].toArray()));

            //QEventLoop ac;
            //connect(&blendinstaller, SIGNAL(finished()), &ac, SLOT(quit()));

            blendinstaller.show_progress();
            blendinstaller.download();

            //ac.exec();

            if(blendinstaller.error_code != blendinstaller.BLEND_INSTALL_NO_ERROR) {
                //blendinstaller.~blend_install();
                lblact_stat.hide();
                btnTggConsole.hide();

                progress_o = -1;
                return;
            }

            blendvers[vid].path = blendinstaller.installedOn();
            blendvers[vid].exec_bin = blendinstaller.installedOn();
            if (blendvers[vid].OS == BLENDER_OS_WIN32 ||
                    blendvers[vid].OS == BLENDER_OS_WIN64)
                blendvers[vid].exec_bin += "/blender.exe";
            else if (blendvers[vid].OS == BLENDER_OS_MACOSX64)
                blendvers[vid].exec_bin += "/blender.app";
            else
                /*
                    if (blendvers[vid].OS == BLENDER_OS_FREEBSD64 ||
                    blendvers[vid].OS == BLENDER_OS_LINUX32 ||
                    blendvers[vid].OS == BLENDER_OS_LINUX64)
                    */
                blendvers[vid].exec_bin += "/blender";

            blendvers[vid].downloaded = true;

            renderBlenderVersions();
            blendversions.setObject(convertBlendVersioningtoJson(blendvers));
            blendversions.save_vars("versions.json");

            /*
                        setFileURL(fileURL);
                        setDownloadFileName(downloadFileName);
                        setDownloadDirectory(downloadDir);
                        setInstallBasePath(installBasePath);
                        setExtractProgram(extractProgram);
                        setExtractArguments(extractArguments);*/

            //QMessageBox::warning(this, "BlendManager", QString("Finished downloading Blender %1.%2!").arg(blendvers[vid].major).arg(blendvers[vid].minor), QMessageBox::Ok);

            lblact_stat.hide();
            btnTggConsole.hide();

            progress_o = -1;

        } else {
            // nothing.
            return;
        }

    } else {

        QMessageBox *mg = new QMessageBox(this);
        mg->setText(QString("Blender is already running [pid:%1]!").arg(blendproc.pid()));
        mg->exec();

    }

    //QMessageBox *newt = new QMessageBox(this);
    //newt->setText(QString("Executed from \"%1\"!").arg(blendvers[vid].path));
    //newt->exec();
}

MainWindow::~MainWindow()
{

}

void MainWindow::renderBlenderVersions() {
    if (renderOkay && !blendvers.isEmpty()) {

        viewer->clear(); // clear everything for whatever reason.

        QListWidgetItem *itm;
        QString nm;
        const QIcon icn(":/icons/blender16.png");
        int i = 0;
        foreach (BlendVersioning a, blendvers) {
            itm = new QListWidgetItem;
            if (a.branch == "vanilla")
                nm = QString("Blender %1.%2%3")
                        .arg(a.major)
                        .arg(a.minor)
                        .arg(a.rev);
            else
                nm = QString("Blender %1.%2%3 (%4)")
                        .arg(a.major)
                        .arg(a.minor)
                        .arg(a.rev)
                        .arg(a.branch);
            itm->setText(nm);
            itm->setIcon(icn);

            qDebug(
                        QString("Blender %1.%2%3 [idx=%4] [downloaded=%5]")
                        .arg(a.major)
                        .arg(a.minor)
                        .arg(a.rev)
                        .arg(i)
                        .arg(a.downloaded)
                        .toLocal8Bit().constData()
                        );


            itm->setData(Qt::UserRole, QVariant (i));
            if (a.downloaded == true) itm->setBackgroundColor(QColor(0,255,75,85));
            else itm->setBackgroundColor(QColor(255,0,75,85));

            viewer->addItem(itm);
            i++;
        }

        qDebug(
                    QString("[blendvers] has %1 and been itterated %2 times")
                    .arg(blendvers.size())
                    .arg(i)
                    .toLocal8Bit().constData()
                    );
    } else qDebug(
                QString("[blendvers] is empty.")
                .toLocal8Bit().constData()
                );
}

void MainWindow::seeConsole() {
    if (progress_o == 2) {
        if (blendinstaller.is_progress()) blendinstaller.hide_progress();
        else blendinstaller.show_progress();
    } else if (blendproc.state() != QProcess::NotRunning) {
        if (blendproc.is_console() == true) blendproc.close_console();
        else blendproc.open_console();
    } else {
        seeBlendProc(blendproc.state());
    }
}

void MainWindow::seeBlendProc(QProcess::ProcessState state) {
    switch(state) {
    case QProcess::NotRunning:
        lblact_stat.setText("Blender is not running.");
        //lblact_stat.hide();
        btnAct.hide();
        btnTggConsole.hide();
        break;
    case QProcess::Starting:
        lblact_stat.setText("Blender is starting.");
        lblact_stat.show();
        btnAct.show();
        break;
    case QProcess::Running:
        lblact_stat.setText("Blender is running.");
        btnTggConsole.show();
        break;
    default:
        // should not be here.
        break;
    }
}

void MainWindow::seeBlendExit() {
    QMessageBox *msg = new QMessageBox(this);
    msg->setText("This will terminate Blender.\nAre you certain about this?");
    msg->setDefaultButton(QMessageBox::Cancel);
    msg->setInformativeText("You may lose your work if you didn't close Blender safely!");
    msg->setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
    msg->setIcon(QMessageBox::Warning);

    int ret = msg->exec();
    switch(ret) {
    case QMessageBox::Cancel: break;
    case QMessageBox::Ok:
        blendproc.terminate();
        break;
    default:
        break;
    }

}

void MainWindow::seeAbout() {
    QMessageBox *msg = new QMessageBox(this);
    msg->setWindowTitle("About BlendManager");
    msg->setText(
                QString("BlendManager is a Blender version manager program.\n") +
                QString("It manages your available Blender executables and let you pick the version you wish.\n\n") +
                QString("Blender is a free open-source 3D animation software from Blender Foundation.")
                );
    //msg->setDefaultButton(QMessageBox::Ok);
    //msg->setInformativeText("");
    QPushButton *vs = msg->addButton("Visit Blender's Homepage", QMessageBox::YesRole);
    QPushButton *cl = msg->addButton(QMessageBox::Close);
    msg->setDefaultButton(cl);

    msg->setIconPixmap(QPixmap::fromImage(QImage(":/icons/blender48.png")));

    msg->exec();

    if (msg->clickedButton() == cl) {
        return;
    } else if (msg->clickedButton() == vs) {
        if(QDesktopServices::openUrl(QUrl("http://www.blender.org", QUrl::TolerantMode)) == false)
            QMessageBox::warning(this, "Error", "Couldn't open http://www.blender.org!", QMessageBox::Ok);
        return;
    }
}

void MainWindow::seeUpdate() {
    // TODO. Download.
    // Not implemented yet.
    QMessageBox::warning(this, "Error!", "This is not implemented yet!", QMessageBox::Ok, QMessageBox::Ok);
}

void MainWindow::setArgument(int argc, char **argv) {
    ag_int = argc;
    ag_char = argv;
}

QStringList MainWindow::jsontoStringList(QJsonArray arr) {
    QStringList ret;
    QJsonValue t;
    foreach(t, arr) ret << t.toString();
    return ret;
}

void MainWindow::seeBlendProcFinished(int ex) {
    progress_o = -1; // TODO. just in case we need something extraordinary with this.
}

void MainWindow::bootstrap() {
    blendsettings.load_vars(QApplication::applicationDirPath() + "/settings.json");
    blendversions.load_vars(QApplication::applicationDirPath() + "/versions.json");

    blendvers = convertJsontoBlendVersioning(blendversions.object());
    QList<BlendVersioning> bl = blendvers;

    // TODO. read cmd args for file, interpret version.
    QFile tp;

    int major_prf = -1;
    int minor_prf = -1;

    for(int i=1; i<ag_int; i++) {
        tp.setFileName(QString(ag_char[i]));
        if (tp.exists()) break;
    }

    if(tp.exists()) {
        if(tp.open(QFile::ReadOnly | QFile::Text)) {
            QString t(tp.read(12));
            if (t.left(7) == "BLENDER") {
                major_prf = t.right(3).left(1).toInt();
                minor_prf = t.right(2).toInt();
                qDebug(
                            QString("I prefer version %1.%2.")
                            .arg(major_prf)
                            .arg(minor_prf)
                            .toLocal8Bit().constData()
                            );
            }
        }
    }

    if (blendsettings.object()["versionAutoSelect"].toBool() == true) {
        int major_latest = 0;
        int minor_latest = 0;
        int rev_latest = 0;
        QString revs_latest = 0;
        int major_local = 0;
        int minor_local = 0;
        int rev_local = 0;
        QString revs_local = "";

        int idx =-1;
        int sel_idx_latest = -1;
        int sel_idx_local = -1;

        foreach(BlendVersioning a, bl) {
            idx++;
            if ((major_local < a.major || major_local == a.major) && a.downloaded) {
                major_local = a.major;
                minor_local = a.minor;
                rev_local = 0;
                revs_local = "";
                if (minor_local < a.minor || minor_local == a.minor) {
                    minor_local = a.minor;
                    rev_local = 0;
                    revs_local = "";

                    int v = 0;
                    foreach(QChar b, a.rev) v += b.toLatin1();
                    if (rev_local < v || rev_local == v) {
                        rev_local = v;
                        revs_local = a.rev;
                        sel_idx_local = idx;
                    }
                }
            }
            if ((major_latest < a.major || major_latest == a.major)) {
                major_latest = a.major;
                minor_latest = a.minor;
                rev_latest = 0;
                revs_latest = "";
                if (minor_latest < a.minor || minor_latest == a.minor) {
                    minor_latest = a.minor;
                    rev_latest = 0;
                    revs_latest = "";

                    int v = 0;
                    foreach(QChar b, a.rev) v += b.toLatin1();
                    if (rev_latest < v || rev_latest == v) {
                        rev_latest = v;
                        revs_latest = a.rev;
                        sel_idx_latest = idx;
                    }
                }
            }
        }

        if ((sel_idx_local == -1 && (sel_idx_latest == -1 || blendsettings.object()["useLocalOnly"].toBool())) ||
                (!blendvers[sel_idx_latest].downloaded &&
                 blendsettings.object()["forceLatest"].toBool() &&
                 blendsettings.object()["useLocalOnly"].toBool()))
            QMessageBox::warning(this, "Error!", "The option \"auto-select version\" has no candidate to execute!", QMessageBox::Ok, QMessageBox::Ok);

        else {
            int sel_idx;
            if (((major_prf != -1 && minor_prf != -1  && findInHaystack(major_prf, minor_prf) != -1 && blendvers[findInHaystack(major_prf, minor_prf)].downloaded) &&
                (!blendsettings.object()["forceLatest"].toBool()))
                || (major_prf == major_latest && minor_prf == minor_latest && blendsettings.object()["forceLatest"].toBool())
                )
                sel_idx = findInHaystack(major_prf, minor_prf);
            else if (blendsettings.object()["forceLatest"].toBool() && blendvers[sel_idx_latest].downloaded) sel_idx = sel_idx_latest;
            else if (!blendvers[sel_idx_latest].downloaded &&
                   !blendsettings.object()["useLocalOnly"].toBool()) {
                if (major_prf != -1 && minor_prf != -1  && findInHaystack(major_prf, minor_prf) != -1 && !blendvers[findInHaystack(major_prf, minor_prf)].downloaded && !blendsettings.object()["forceLatest"].toBool())
                    sel_idx_latest = findInHaystack(major_prf, minor_prf);
                QListWidgetItem *o = new QListWidgetItem();
                o->setData(Qt::UserRole, QVariant (sel_idx_latest));
                seeText(o);

                sel_idx = sel_idx_latest;
            } else sel_idx = sel_idx_local;

            // else if (sel_idx_local == sel_idx_latest && blendvers[sel_idx_local].downloaded) {
            // TODO. just execute.

            QStringList ag;

            if (blendsettings.object()["ignoreCommandLineArguments"].toBool() == false)
                for (int i=1; i<ag_int; i++) ag << ag_char[i];
            foreach(QJsonValue p, blendsettings.object()["forceArguments"].toArray())
                ag << p.toString();

            ag.append(blendvers[sel_idx].arguments);

            QEventLoop ac;
            startBlender(blendvers[sel_idx].exec_bin,
                         blendvers[sel_idx].path,
                         ag);
            connect(&blendproc, SIGNAL(finished(int)), &ac, SLOT(quit()));

            close();
        }
        close();

    } else {
        show();
        renderBlenderVersions();
    }
}


void MainWindow::startBlender(const QString &exec, const QString &work_dir, QStringList arguments) {
    blendproc.close_console();

    blendproc.setWorkingDirectory(work_dir);
    blendproc.setProgram(exec);
    blendproc.setArguments(arguments);
    blendproc.start();

    blendproc.open_console();
}

int MainWindow::findInHaystack(int major, int minor) {
    if (!blendvers.isEmpty()) {
        int i = 0;
        foreach(BlendVersioning a, blendvers) {
            if(a.major == major && a.minor == minor) return i;
            i++;
        }
    }
    return -1;
}
