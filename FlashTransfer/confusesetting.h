#ifndef CONFUSESETTING_H
#define CONFUSESETTING_H

#include <QDialog>

namespace Ui {
class ConfuseSetting;
}

class ConfuseSetting : public QDialog
{
    Q_OBJECT

public:
    explicit ConfuseSetting(QWidget *parent = 0);
    bool eventFilter(QObject *o, QEvent *e);
    ~ConfuseSetting();

public slots:
    void onItemSelectionChanged();

private:
    Ui::ConfuseSetting *ui;
};

#endif // CONFUSESETTING_H
