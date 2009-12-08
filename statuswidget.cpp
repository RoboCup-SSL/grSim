#include "statuswidget.h"

CStatusWidget::CStatusWidget(CStatusPrinter* _statusPrinter)
{    
    statusPrinter = _statusPrinter;
    logTime.start();

    this->setAllowedAreas(Qt::BottomDockWidgetArea);
    this->setFeatures(QDockWidget::NoDockWidgetFeatures);
    statusText = new QTextEdit(this);
    //statusText->setReadOnly(true);
    titleLbl = new QLabel(tr("Messages"));


    this->setWidget(statusText);
    this->setTitleBarWidget(titleLbl);

}

void CStatusWidget::write(QString str, QColor color)
{
    if( statusText->textCursor().blockNumber() > 2000 )
        statusText->clear();

    statusText->setTextColor(color);
    //statusText->append(QString::number(statusText->textCursor().blockNumber()) + " : " + str);
    //statusText->append(QString::number(logTime.elapsed()) + " : " + str);
    statusText->append(str);


    statusText->setTextColor(QColor("black"));
}

void CStatusWidget::update()
{
    CStatusText text;
    while(!statusPrinter->textBuffer.isEmpty())
    {
        text = statusPrinter->textBuffer.dequeue();
        write(text.text, text.color);
    }
}

