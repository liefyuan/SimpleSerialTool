#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initUI();
    initSerialPort();
    initTimer();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*
 * 函数功能：完成对串口助手界面的初始化
 */
void MainWindow::initUI()
{
    gSerialSwitchFlg = false;
    gSetting = new QSettings("./Setting.ini", QSettings::IniFormat);

    //设置端口号波特率
    ui->cboBaudrate->addItem(QString("1200"),QSerialPort::Baud1200);
    ui->cboBaudrate->addItem(QString("2400"),QSerialPort::Baud2400);
    ui->cboBaudrate->addItem(QString("4800"),QSerialPort::Baud4800);
    ui->cboBaudrate->addItem(QString("9600"),QSerialPort::Baud9600);
    ui->cboBaudrate->addItem(QString("19200"),QSerialPort::Baud19200);
    ui->cboBaudrate->addItem(QString("38400"),QSerialPort::Baud38400);
    ui->cboBaudrate->addItem(QString("57600"),QSerialPort::Baud57600);
    ui->cboBaudrate->addItem(QString("115200"),QSerialPort::Baud115200);

    //设置端口数据位数
    ui->cboDataBit->addItem("8",QSerialPort::Data8);
    ui->cboDataBit->addItem("7",QSerialPort::Data7);
    ui->cboDataBit->addItem("6",QSerialPort::Data6);
    ui->cboDataBit->addItem("5",QSerialPort::Data5);

    //设置端口校验方式
    ui->cboParity->addItem("None",QSerialPort::NoParity);
    ui->cboParity->addItem("Odd",QSerialPort::OddParity);
    ui->cboParity->addItem("Even",QSerialPort::EvenParity);

    //设置端口停止位
    ui->cboStopBit->addItem("1",QSerialPort::OneStop);
    ui->cboStopBit->addItem("1.5",QSerialPort::OneAndHalfStop);
    ui->cboStopBit->addItem("2",QSerialPort::TwoStop);

    //设置端口流控制
    ui->cboFlowContral->addItem("None",QSerialPort::NoFlowControl);
    ui->cboFlowContral->addItem("RTS/CTS",QSerialPort::HardwareControl);
    ui->cboFlowContral->addItem("XON/XOFF",QSerialPort::SoftwareControl);

    ui->btnSend->setDisabled(true);

}
/*
 * 函数功能：串口设置的初始化
 */
void MainWindow::initSerialPort()
{
    if(!gSerialSwitchFlg)
    {
        QStringList newPortStringList;
        static QStringList oldPortStringList;

        const auto infos = QSerialPortInfo::availablePorts();
        for (const QSerialPortInfo &info : infos)
        {
            newPortStringList += info.portName();
        }
        //更新串口号
        if(newPortStringList.size() != oldPortStringList.size())
        {
            oldPortStringList = newPortStringList;
            ui->cboPortName->clear();
            ui->cboPortName->addItems(oldPortStringList);
        }

        // 读取上次的设置
        ui->cboPortName->setCurrentText(gSetting->value("LastSelectSerialPort").toString());
        ui->cboBaudrate->setCurrentText(gSetting->value("LastSelectSerialBaud").toString());
    }
}
/*
 * 函数功能：初始化定时器
 */
void MainWindow::initTimer()
{
    gSerialPortTimer = startTimer(1000, Qt::CoarseTimer);
}
/*
 * 函数功能：定时更新当前系统中的串口号
 */
void MainWindow::timerUpdateSerialPort()
{
    initSerialPort();
}
/*
 * 函数功能：接收串口发出数据将其显示到界面上
 */
void MainWindow::receiveData()
{
    QByteArray data = gSerialPort->readAll();
    //将接收的数据进行转换
    QString strText = QString(data);

    //显示当前接收的数据
    ui->RecveeiveplainTextEdit->appendPlainText(strText);
}
/*
 * 函数功能：定义窗口关闭时弹出的提示窗口的操作
 */
void MainWindow::closeEvent(QCloseEvent *e)
{
    Q_UNUSED(e);
    qDebug() << "exist!";

    // 保存设置
    gSetting->setValue("LastSelectSerialPort", ui->cboPortName->currentText());
    gSetting->setValue("LastSelectSerialBaud", ui->cboBaudrate->currentText());
}
/*
 * 函数功能：根据定时器ID执行对应的逻辑
 */
void MainWindow::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == gSerialPortTimer)
    {
        if(!gSerialSwitchFlg)
            initSerialPort();
    }
    else if(event->timerId() == gSendDataTimer)
    {
        if(gSerialSwitchFlg)
        {
            gSerialPort->write(ui->SendplainTextEdit->toPlainText().toUtf8());
        }
    }
}
/*
 * 函数功能：打开串口的按键功能
 */
void MainWindow::on_btnOpenPort_clicked()
{
    if(!gSerialSwitchFlg) // 串口是关闭的
    {
        gSerialPort = new QSerialPort();

        gSerialPort->setPortName(ui->cboPortName->currentText());

        gSerialPort->setBaudRate((QSerialPort::BaudRate)ui->cboBaudrate->currentData().toInt());//波特率

        gSerialPort->setDataBits((QSerialPort::DataBits)ui->cboDataBit->currentData().toInt());//数据位

        gSerialPort->setParity((QSerialPort::Parity)ui->cboParity->currentData().toInt());//奇偶校验

        gSerialPort->setStopBits((QSerialPort::StopBits)ui->cboStopBit->currentData().toInt());//停止位

        gSerialPort->setFlowControl((QSerialPort::FlowControl)ui->cboFlowContral->currentData().toInt());//控制位

        gSerialPort->open(QIODevice::ReadWrite);//读写方式打开

        connect(gSerialPort, SIGNAL(readyRead()), \
                this, SLOT(receiveData()), Qt::DirectConnection);

        gSerialSwitchFlg = true;

        ui->btnOpenPort->setText(QStringLiteral("关闭串口"));

        ui->groupBox->setDisabled(true);
        ui->btnSend->setDisabled(false);
    }
    else
    {
        gSerialPort->close();

        gSerialSwitchFlg = false;

        ui->btnOpenPort->setText(QStringLiteral("打开串口"));

        ui->groupBox->setDisabled(false);
        ui->btnSend->setDisabled(true);
    }
}
/*
 * 函数功能：发送串口数据的按键功能
 */
void MainWindow::on_btnSend_clicked()
{
    //获取所要发送的文本
    QString strSend = ui->SendplainTextEdit->toPlainText();
    //将所要发送的文本存入arr变量
    QByteArray arr = strSend.toUtf8();

    gSerialPort->write(arr);
}
/*
 * 函数功能：定时发送状态改变
 */
void MainWindow::on_checkBox_stateChanged(int arg1)
{
    qDebug() << "state:" << arg1;
    if(arg1 == 2)
    {
        gSendDataTimer = startTimer(ui->lineEdit->text().toInt(), Qt::CoarseTimer);
        ui->lineEdit->setDisabled(true);
    }
    else
    {
        killTimer(gSendDataTimer);
        ui->lineEdit->setDisabled(false);
    }
}
