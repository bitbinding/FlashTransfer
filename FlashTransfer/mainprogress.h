#ifndef MAINPROGRESS_H
#define MAINPROGRESS_H

#include <QDialog>

namespace Ui {
class MainProgress;
}

class MainProgress : public QDialog
{
    Q_OBJECT

public:
    explicit MainProgress(QWidget *parent = 0);
    bool eventFilter(QObject *o, QEvent *e);
    void keyPressEvent(QKeyEvent *event);
    void resize();
    void closeEvent(QCloseEvent *event);
    ~MainProgress();
public slots:
    void attachLogSlot(QString s);
    void setCloseableSlot(bool closeable);
private:
    Ui::MainProgress *ui;
};

#endif // MAINPROGRESS_H
