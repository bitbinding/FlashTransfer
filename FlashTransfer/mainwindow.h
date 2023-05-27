#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    bool eventFilter(QObject *o, QEvent *e);
    void initOpenText(const char *pathname);
    ~MainWindow();

public slots:
    void onComboBox();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
