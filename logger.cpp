#include "logger.h"
#include "statuswidget.h"
CStatusPrinter *printer;
void initLogger(void* v)
{
    printer = (CStatusPrinter*) v;
}

void logStatus(QString s,QColor c)
{    
    printer->textBuffer.enqueue(CStatusText(s,c));
}

