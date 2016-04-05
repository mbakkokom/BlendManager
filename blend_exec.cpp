#include "blend_exec.h"

blend_exec::blend_exec() {}

blend_exec::~blend_exec() {}

blend_exec::blend_exec(const QString &path, const QString &filename, const QStringList &args, bool start_n, bool console_n)
{
    //setParent(parent);
    setWorkingDirectory(path);
    setProgram(filename);
    setArguments(args);
    if (start_n == true) start();
    if (console_n == true) open_console();
}

void blend_exec::open_console() {
    //qDebug(QString("Starting with command: %1 %2").arg(program()).arg(arguments().join(" ")).toLocal8Bit().constData());

    draw_console();
    w_console.show();
    connect(this,SIGNAL(readyReadStandardOutput()),this,SLOT(out_pipe_flusher()));
    connect(this,SIGNAL(readyReadStandardError()),this,SLOT(err_pipe_flusher()));

    connect(this,SIGNAL(finished(int)),this,SLOT(prog_exit(int)));
    connect(this,SIGNAL(stateChanged(QProcess::ProcessState)),this,SLOT(prog_state(QProcess::ProcessState)));
}

void blend_exec::draw_console() {
    //w_console.setParent(this);
    w_console.setFixedHeight(600);
    w_console.setFixedWidth(600);
    //w_console.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    /*
    QWidget *h;
    QVBoxLayout *l;
    l->addWidget(text_area);
    h->setLayout(l);

    mainbay->setWidget(h);
    h->show();

    QVBoxLayout *lay;
    lay->addWidget(mainbay);
    */

    //QVBoxLayout mainlay;
    QScrollArea *mainbay = new QScrollArea(&w_console);

    //text_area.setText("[CONSOLE OPEN]\n");
    //text_area.setMinimumSize(600,600);
    //text_area.setMargin(10);
    text_area.setAlignment(Qt::AlignTop | Qt::AlignLeft);
    text_area.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    text_area.setBaseSize(600, 600);
    //text_area.setTextFormat(Qt::RichText);

    mainbay->setWidget(&text_area);
    mainbay->setWidgetResizable(1);
    mainbay->setFixedSize(600, 600);
    //mainbay->setViewport(&w_console);
    //mainbay->setGeometry(0,0,600,600);
    //mainbay->setupViewport(&w_console);
    //text_area.show();
    //mainlay.addWidget(mainbay, 1, Qt::AlignTop);

    //w_console.setLayout(&mainlay);
}

void blend_exec::close_console() {
    //stopConsole = 2;
    w_console.close();
}

bool blend_exec::is_console() {
    return w_console.isVisible();
}

void blend_exec::out_pipe_flusher() {
    text_area.setText(text_area.text() +
                      QString (readAllStandardOutput())
                      );
}

void blend_exec::err_pipe_flusher() {
    text_area.setText(text_area.text() +
                      QString (readAllStandardError())
                      );
}

void blend_exec::prog_exit(int exit_code) {
    if(it_exit) {
        text_area.setText(text_area.text() +
                          QString ("\nBlender exit with error code %1.").arg(exit_code)
                          );
        it_exit = false;
    }
    if(close_exit) close_console();
}

void blend_exec::prog_state(QProcess::ProcessState state) {
    if (state == QProcess::Starting) {
        text_area.setText("");
        it_exit = true;
    }
}

bool blend_exec::is_finished() {
    return (!it_exit);
}

void blend_exec::setCloseOnFinished(bool exit) {
    close_exit = exit;
}

QString blend_exec::readOutput() {
    return text_area.text();
}
