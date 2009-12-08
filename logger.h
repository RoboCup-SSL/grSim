#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QColor>

void initLogger(void*); //MUST BE INITED FROM MAINWINDOW.CPP
void logStatus(QString s,QColor c);

#endif // LOGGER_H
