#include "logger.h"
#include "statuswidget.h"
#include <QDebug>
CStatusPrinter *printer;
void initLogger(void* v)
{
    printer = (CStatusPrinter*) v;
}

void logStatus(QString s,QColor c)
{    
    qDebug() << s;
    printer->textBuffer.enqueue(CStatusText(s,c));
}

