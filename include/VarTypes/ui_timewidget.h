/********************************************************************************
** Form generated from reading ui file 'timewidget.ui'
**
** Created: Sat Dec 5 14:43:00 2009
**      by: Qt User Interface Compiler version 4.5.1
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_TIMEWIDGET_H
#define UI_TIMEWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QScrollBar>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TimeWidget
{
public:
    QVBoxLayout *vboxLayout;
    QHBoxLayout *hboxLayout;
    QLabel *label1;
    QSpacerItem *spacerItem;
    QLabel *label2;
    QScrollBar *scroller;

    void setupUi(QWidget *TimeWidget)
    {
        if (TimeWidget->objectName().isEmpty())
            TimeWidget->setObjectName(QString::fromUtf8("TimeWidget"));
        TimeWidget->resize(429, 39);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(TimeWidget->sizePolicy().hasHeightForWidth());
        TimeWidget->setSizePolicy(sizePolicy);
        const QIcon icon = QIcon(QString::fromUtf8(":/icons/xclock.png"));
        TimeWidget->setWindowIcon(icon);
        vboxLayout = new QVBoxLayout(TimeWidget);
        vboxLayout->setSpacing(0);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        vboxLayout->setContentsMargins(0, 0, 0, 0);
        hboxLayout = new QHBoxLayout();
        hboxLayout->setSpacing(6);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        hboxLayout->setContentsMargins(4, -1, 4, -1);
        label1 = new QLabel(TimeWidget);
        label1->setObjectName(QString::fromUtf8("label1"));

        hboxLayout->addWidget(label1);

        spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem);

        label2 = new QLabel(TimeWidget);
        label2->setObjectName(QString::fromUtf8("label2"));

        hboxLayout->addWidget(label2);


        vboxLayout->addLayout(hboxLayout);

        scroller = new QScrollBar(TimeWidget);
        scroller->setObjectName(QString::fromUtf8("scroller"));
        scroller->setOrientation(Qt::Horizontal);

        vboxLayout->addWidget(scroller);


        retranslateUi(TimeWidget);

        QMetaObject::connectSlotsByName(TimeWidget);
    } // setupUi

    void retranslateUi(QWidget *TimeWidget)
    {
        TimeWidget->setWindowTitle(QApplication::translate("TimeWidget", "TimeControl", 0, QApplication::UnicodeUTF8));
        label1->setText(QApplication::translate("TimeWidget", "No Info Available", 0, QApplication::UnicodeUTF8));
        label2->setText(QApplication::translate("TimeWidget", "No Time Selected", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(TimeWidget);
    } // retranslateUi

};

namespace Ui {
    class TimeWidget: public Ui_TimeWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TIMEWIDGET_H
