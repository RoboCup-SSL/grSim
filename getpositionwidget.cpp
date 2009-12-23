#include "getpositionwidget.h"
#include <QtGui>
#include <QGridLayout>
#include <QLabel>

GetPositionWidget::GetPositionWidget()
{
    QGridLayout *layout = new QGridLayout(this);
    okBtn = new QPushButton("&Ok",this);
    cancelBtn = new QPushButton("&Cancel",this);
    x = new QLineEdit(this);
    y = new QLineEdit(this);
    a = new QLineEdit(this);
    layout->addWidget(new QLabel("X"),0,0);
    layout->addWidget(new QLabel("Y"),1,0);
    layout->addWidget(new QLabel("Angle"),2,0);
    layout->addWidget(okBtn,3,1);
    layout->addWidget(cancelBtn,3,0);
    layout->addWidget(x,0,1);
    layout->addWidget(y,1,1);
    layout->addWidget(a,2,1);
    setLayout(layout);
    connect(cancelBtn,SIGNAL(clicked()),this,SLOT(cancelBtnClicked()));
}

void GetPositionWidget::cancelBtnClicked()
{
    close();
}
