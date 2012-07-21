/*
grSim - RoboCup Small Size Soccer Robots Simulator
Copyright (C) 2011, Parsian Robotic Center (eew.aut.ac.ir/~parsian/grsim)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CSTATUSWIDGET_H
#define CSTATUSWIDGET_H

#include <QDockWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QColor>
#include <QTextDocument>
#include <QTime>
#include <QQueue>
#include <QColor>


class CStatusText
{
    public:
    CStatusText(QString _text = "", QColor _color = QColor("black"), int _size = 12)
    {
        text= _text;
        color = _color;
        size = _size;
    }

    QString text;
    QColor color;
    int size;
};

class CStatusPrinter
{
    public:
    CStatusPrinter() {}

    QQueue<CStatusText> textBuffer;
};


class CStatusWidget : public QDockWidget
{
    Q_OBJECT
public:
    CStatusWidget(CStatusPrinter* _statusPrinter);
    QTextEdit *statusText;
    QLabel *titleLbl;
    QTextDocument content;

public slots:
    void write(QString str, QColor color = QColor("black"));
    void update();

private:
    CStatusPrinter *statusPrinter;
    QTime logTime;

};

#endif // CSTATUSWIDGET_H
