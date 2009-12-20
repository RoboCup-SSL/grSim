#ifndef GETPOSITIONWIDGET_H
#define GETPOSITIONWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
class GetPositionWidget : public QWidget
{
    Q_OBJECT
public:
    GetPositionWidget();
    QLineEdit *x,*y,*a;
    QPushButton *okBtn,*cancelBtn;
public slots:
    void cancelBtnClicked();
};

#endif // GETPOSITIONWIDGET_H
