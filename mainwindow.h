#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QAction>
#include <QPixmap>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QColor>
#include <QImage>
#include <QPushButton>
#include <QDesktopServices>
#include <QUrl>
#include <QEventLoop>

#include "blend_datastruct.h"
#include "blend_exec.h"
#include "blend_install.h"
#include "blend_json.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    void setArgument(int argc, char **argv);

    void bootstrap();

    ~MainWindow();

public slots:
    void seeText(QListWidgetItem *idx);
    void seeAbout();
    void seeUpdate();
    void seeConsole();
    void seeBlendExit();
    void seeBlendProc(QProcess::ProcessState state);
    void seeBlendProcFinished(int ex);

private:
    void startBlender(const QString &exec, const QString &work_dir, QStringList arguments);

    void renderBlenderVersions();

    QList<BlendVersioning> convertJsontoBlendVersioning(QJsonObject json_obj);
    QJsonObject convertBlendVersioningtoJson(QList<BlendVersioning> bversion);

    int findInHaystack(int major, int minor);

    QStringList jsontoStringList(QJsonArray arr);

    int ag_int;
    char **ag_char;

    int progress_o = -1;

    bool renderOkay = false;

    QLabel lbltop_icon;
    QLabel lbltop_name;
    QLabel lbltop_details;
    QLabel lblact_stat;
    QPushButton btnAct;
    QPushButton btnTggConsole;
    QPushButton btnUpdate;
    QPushButton btnAbout;
    QListWidget *viewer = new QListWidget(this);

    // process
    blend_exec blendproc;
    blend_json blendsettings;
    blend_json blendversions;
    blend_install blendinstaller;
    QList<BlendVersioning> blendvers;
};

#endif // MAINWINDOW_H
