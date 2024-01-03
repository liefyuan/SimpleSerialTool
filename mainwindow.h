#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QSettings>
#include <QTimer>

#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    //界面初始化函数
    void initUI();
    //端口初始化函数
    void initSerialPort();
    //定时器初始化
    void initTimer();
    //串口定时更新端口
    void timerUpdateSerialPort();
    //定时器事件
    void timerEvent(QTimerEvent *event);
    //重写closeEvent函数
    void closeEvent(QCloseEvent *);
    //封包函数
    QByteArray encodeData(QByteArray in);
    //解包函数
    void decodeData(QString in, QString *out, uint8_t len);

private slots:
    void receiveData();


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnOpenPort_clicked();

    void on_btnSend_clicked();

    void on_checkBox_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;

    //串口类对象
    QSerialPort *gSerialPort;

    //串口号定时更新定时器
    int gSerialPortTimer;
    //发送数据定时器
    int gSendDataTimer;

    // 标记串口的开关状态
    bool gSerialSwitchFlg;
    // 记录上一次的配置
    QSettings *gSetting;
};
#endif // MAINWINDOW_H
