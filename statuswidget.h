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
