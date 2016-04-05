#ifndef BLEND_EXEC_H
#define BLEND_EXEC_H

#include <QWidget>
#include <QProcess>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>

class blend_exec : public QProcess
{
    Q_OBJECT
public:
    blend_exec();
    blend_exec(const QString &path, const QString &filename, const QStringList &args = QStringList(), bool start_n=false, bool console_n=false);

    void open_console();
    void close_console();

    void setCloseOnFinished(bool exit);

    bool is_console();
    bool is_finished();

    QString readOutput();

    virtual ~blend_exec();

public slots:
    void out_pipe_flusher();
    void err_pipe_flusher(); // separated for "future" purposes.
    void prog_exit(int exit_code);
    void prog_state(QProcess::ProcessState state);

private:
    //int stopConsole = -1;
    bool it_exit;

    bool close_exit = false;

    QWidget w_console;
    void draw_console();

    QLabel text_area;
};

#endif // BLEND_EXEC_H
