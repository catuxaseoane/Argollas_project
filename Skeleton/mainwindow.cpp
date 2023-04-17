#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    GoOn=0;
    HaveCams=0;

    Pako=new Worker();
    connect(Pako,SIGNAL(Result(const int&)),this,SLOT(FeedBack(const int&)));
    connect(Pako,SIGNAL(TimeOut()),this,SLOT(TimeOut()));
    connect(Pako,SIGNAL(finished()),this,SLOT(AtEnd()));

    // if(!HaveCams) ... // Abort normal use, cams are not present or successfully connected

}

MainWindow::~MainWindow()
{
    delete Pako;

    delete ui;
}


void MainWindow::TimeOut(void)
{
    statusBar()->showMessage("Time out!",1000);
}


void MainWindow::FeedBack(const int& n)
{
    switch(Mode) {
        case 0:
            break;
        case 1:
            ui->EFeed->setText(QString::number(n));

            ui->image->setPixmap(QPixmap::fromImage(QImage(pix,WW,HH,QImage::Format_Grayscale8)));
            ui->image2->setPixmap(QPixmap::fromImage(QImage(pix2,WW,HH,QImage::Format_Grayscale8)));
            statusBar()->showMessage("Total "+QString::number(total));

            break;
    }
}

void MainWindow::on_BGo_clicked()
{
    if(!GoOn) {
          ui->BGo->setText("Stop");
          Mode=1;
          GoOn=1;
          statusBar()->showMessage("Sampling...");
          Pako->start(QThread::HighPriority);
      } else {
          GoOn=0;
          statusBar()->showMessage("Stopping...");
      }
}

void MainWindow::AtEnd()
{
    switch(Mode) {
        case 0:
            break;
        case 1:
            ui->EFeed->setText("Done");
            break;
    }

    if(Kaboum==1) {statusBar()->showMessage("Ka Boumm!!"); Kaboum=0;}
    else statusBar()->showMessage("Ready");

    ui->BGo->setText("Go");
}


//-------------------------------------------------------
// How to receive data from microcontroller, e.g. Arduino
//-------------------------------------------------------


// Include the following headers
//
// #include <QtSerialPort/QSerialPort>
// #include <QtSerialPort/QSerialPortInfo>
//
//
// Also add this line at beginning of .pro file
// to be able to use the serial comms module of Qt
//
// QT       += serialport


// Must add to .h:
//
// QSerialPort *uC;
//
//     and in the section protected slots:
//
// void onReadyRead_uC(void);
//

// Then in .cpp add initialization code to Constructor:

// If serial port ID is fixed and known (faster), e.g. ttyS2 (Linux) or COM5 (Win)
/*
QThread::msleep(500);  // Give time for uC to startup if this happens just after power is on
uC=new QSerialPort("ttyS2");
if(uC->open(QIODevice::ReadOnly)) {
    uC->setBaudRate(QSerialPort::Baud9600);
    uC->setFlowControl(QSerialPort::NoFlowControl);
    uC->setParity(QSerialPort::NoParity);
    uC->setDataBits(QSerialPort::Data8);
    uC->setStopBits(QSerialPort::OneStop);
    connect(uC,&QIODevice::readyRead,this,onReadyRead_uC);
} else { // Failed
    delete uC;
	uC=nullptr;
}
*/

// Alt: If serial port unknown (slower; to use instead of the above)
/*
QThread::msleep(500); 
const auto infos = QSerialPortInfo::availablePorts();
for (const QSerialPortInfo &info : infos) if(info.manufacturer()=="Arduino") {
    uC=new QSerialPort(info.portName());
    if(uC->open(QIODevice::ReadWrite)) {
        uC->setBaudRate(QSerialPort::Baud115200);
        uC->setFlowControl(QSerialPort::NoFlowControl);
        uC->setParity(QSerialPort::NoParity);
        uC->setDataBits(QSerialPort::Data8);
        uC->setStopBits(QSerialPort::OneStop);
        connect(uC,SIGNAL(readyRead()),this,SLOT(onReadyRead_uC));
    } else {
        delete uC;
		uC=nullptr;
    }
    break;
}
*/

// Sample reader slot (this gets asynchronously triggered every time data is received from uC)
/*void MainWindow::onReadyRead_uC()
{

    QSerialPort* port = qobject_cast<QSerialPort*>(sender());
    int nbytes=int(port->bytesAvailable());
    QByteArray buff=port->readAll();
    if(nbytes<12) return; // Assuming 12 bytes is the minimum message size
	
	// Do something with read buffer,
	// e.g. feed the raw data to the screen:
    QString mess;
    for(int j=0;j<nbytes;j++) mess+="0x"+QString::number((uchar)buff.at(j),16)+" ";
    statusBar()->showMessage(mess);

}*/


// Example of how to send data to the microcontroller:
//
//   Add to .h: 
// const char Magic[5]={'M','A','G','I','C'}; // Prologue
// int data;
//
//   Use wherever needed in .cpp:
// if(uC) {uC->write(Magic,5); const char comm='#'; uC->write(&comm,1); uC->write((char*)&data,4);}
// 
void MainWindow::on_image_linkActivated(const QString &link)
{

}

