#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QTimer>
#include <QCheckBox>
#include <QComboBox>
#include <QTextEdit>
#include <QPushButton>
#include "../../proto/netraw.h"
#include "../../proto/grSim_Packet.pb.h"
#include "../../proto/grSim_Commands.pb.h"
#include "../../proto/grSim_Replacement.pb.h"


class MainWindow : public QDialog
{
    Q_OBJECT

public:    
    MainWindow(QWidget *parent = 0);
    ~MainWindow();    
public slots:
    void reconnectUdp();
    void sendPacket();
    void sendBtnClicked();
    void resetBtnClicked();
    void disconnectUdp();
private:
    bool sending, reseting;
    Net::UDP udpsocket;
    Net::Address _addr;
    QLineEdit* edtIp;
    QLineEdit* edtPort;
    QLineEdit* edtId;
    QLineEdit* edtVx, *edtVy, *edtW;
    QLineEdit* edtV1, *edtV2, *edtV3, *edtV4;
    QLineEdit* edtKick, *edtChip;
    QLabel* lblIp;
    QLabel* lblPort;
    QLabel* lblId;
    QComboBox* cmbTeam;
    QLabel* lblVx, *lblVy, *lblW;
    QLabel* lblV1, *lblV2, *lblV3, *lblV4;
    QLabel* lblKick, *lblChip;
    QTextEdit* txtInfo;
    QCheckBox* chkVel, *chkSpin;
    QPushButton* btnSend;
    QPushButton* btnReset;
    QPushButton* btnConnect;
    QTimer* timer;
};

#endif // MAINWINDOW_H
