#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    timer = new QTimer();
    connect(timer,SIGNAL(timeout()),this,SLOT(FindSerialPort()));
    timer->start(100);

    //设置波特率下拉菜单默认显示第三项
    ui->BaudBox->setCurrentIndex(3);
    //关闭发送按钮的使能
    ui->sendButton->setEnabled(false);
    qDebug() << tr("界面设定成功！");
}

MainWindow::~MainWindow()
{
    timer->stop();
    delete timer;
    delete ui;
}

//清空接受窗口
void MainWindow::on_clearButton_clicked()
{
    ui->textEdit->clear();
}

//发送数据
void MainWindow::on_sendButton_clicked()
{
    serial->write(ui->textEdit_2->toPlainText().toLatin1());
}

//读取接收到的数据
void MainWindow::Read_Data()
{

    QByteArray buf = serial->readAll();

    if(!buf.isNull()){
        QString old = ui->textEdit->toPlainText();
        QString str = old + buf;
        ui->textEdit->clear();
        ui->textEdit->setPlainText(str);
    }
    buf.clear();

    QScrollBar *localVerticalScrollBar = ui->textEdit->verticalScrollBar();
    if(localVerticalScrollBar){
        localVerticalScrollBar->setSliderPosition(localVerticalScrollBar->maximum());
    }
}

void MainWindow::FindSerialPort()
{
    //查找可用的串口
    QList<QSerialPortInfo> localAvailablePorts = QSerialPortInfo::availablePorts();

    //删除无用串口
    for ( int i = 0 ; i < ui->PortBox->count(); i ++ ){
        QString value = ui->PortBox->itemText(i);
        bool mark = false;
        for(int j = 0; j< localAvailablePorts.size();j++){
            if(localAvailablePorts.at(j).portName() == value){
                mark = true;
                break;
            }
        }
        if(mark == false){
            ui->PortBox->removeItem(i);
        }
    }

    //更新可用串口
    for(int i=0;i<localAvailablePorts.size();i++){
        if(ui->PortBox->findText(localAvailablePorts.at(i).portName()) == -1){
            ui->PortBox->addItem(localAvailablePorts.at(i).portName());
        }
    }
}

void MainWindow::on_openButton_clicked()
{
    if(ui->openButton->text()==tr("打开串口"))
    {
        serial = new QSerialPort;
        //设置串口名
        serial->setPortName(ui->PortBox->currentText());
        //打开串口
        serial->open(QIODevice::ReadWrite);
        //设置波特率
        serial->setBaudRate(ui->BaudBox->currentText().toInt());
        //设置数据位数
        switch(ui->BitNumBox->currentIndex())
        {
        case 5:
            serial->setDataBits(QSerialPort::Data5);
            break;
        case 6:
            serial->setDataBits(QSerialPort::Data6);
            break;
        case 7:
            serial->setDataBits(QSerialPort::Data7);
            break;
        case 8:
            serial->setDataBits(QSerialPort::Data8);
            break;
        default:
            serial->setDataBits(QSerialPort::Data8);
            break;
        }
        //设置奇偶校验
        switch(ui->ParityBox->currentIndex())
        {
        case 0:
            serial->setParity(QSerialPort::NoParity);
            break;
        case 1:
            serial->setParity(QSerialPort::OddParity);
            break;
        case 2:
            serial->setParity(QSerialPort::EvenParity);
            break;
        default:
            serial->setParity(QSerialPort::NoParity);
            break;
        }
        //设置停止位
        switch(ui->StopBox->currentIndex())
        {
        case 1:
            serial->setStopBits(QSerialPort::OneStop);
            break;
        case 2:
            serial->setStopBits(QSerialPort::TwoStop);
            break;
        default:
            serial->setStopBits(QSerialPort::OneStop);
            break;
        }
        //设置流控制
        serial->setFlowControl(QSerialPort::NoFlowControl);

        //关闭设置菜单使能
        ui->PortBox->setEnabled(false);
        ui->BaudBox->setEnabled(false);
        ui->BitNumBox->setEnabled(false);
        ui->ParityBox->setEnabled(false);
        ui->StopBox->setEnabled(false);
        ui->openButton->setText(tr("关闭串口"));
        ui->sendButton->setEnabled(true);

        //连接信号槽
        QObject::connect(serial, &QSerialPort::readyRead, this, &MainWindow::Read_Data);
    }
    else
    {
        //关闭串口
        serial->clear();
        serial->close();
        delete serial;

        //恢复设置使能
        ui->PortBox->setEnabled(true);
        ui->BaudBox->setEnabled(true);
        ui->BitNumBox->setEnabled(true);
        ui->ParityBox->setEnabled(true);
        ui->StopBox->setEnabled(true);
        ui->openButton->setText(tr("打开串口"));
        ui->sendButton->setEnabled(false);
    }
}
