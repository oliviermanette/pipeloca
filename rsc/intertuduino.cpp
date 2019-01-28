#include "intertuduino.h"

interTUDuino::interTUDuino(QObject *parent) : QObject(parent)
{
    serial.setPortName("/dev/cu.usbmodem3314241");
    serial.open(QIODevice::ReadWrite);
    QObject::connect(&serial, SIGNAL(readyRead()),this,SLOT(receiveSignal()));
    gMode = Normal;
}

interTUDuino::~interTUDuino()
{
    serial.close();
}

void interTUDuino::addTempUnitNeuron()
{
    QByteArray data="a";
    serial.write(data);
    if( serial.waitForBytesWritten(100) )
        qDebug() << "sent: " << data;
}

void interTUDuino::learnTU(char TUId)
{
    QByteArray data="l";
    data.append(TUId+48);
    serial.write(data);
    if( serial.waitForBytesWritten(100) )
        qDebug() << "sent: " << data;
}

int interTUDuino::getRightPeak(short shtPosition)
{
    return gintRightPeak[shtPosition];
}

int interTUDuino::getLeftPeak(short shtPosition)
{
    return gintLeftPeak[shtPosition];
}

void interTUDuino::askPeaks()
{
    QByteArray data;
    if (gMode == Normal){
        gMode = LeftPeak;
        gPosCursor = 0;
        data="L";
    }
    else if ((gMode==LeftPeak) &&(gPosCursor==4096)) {
        gMode = RightPeak;
        gPosCursor = 0;
        data="R";
    }
    else
        return;

    serial.write(data);
    if( serial.waitForBytesWritten(100) )
        qDebug() << "sent: " << data;
}

void interTUDuino::askResults()
{
    QByteArray data="r";
    gMode = Results;
    gPosCursor = 0;
    serial.write(data);
    if( serial.waitForBytesWritten(100) )
        qDebug() << "sent: " << data;
}

float interTUDuino::getResult(short shtPosition)
{
    if ((gfltResults[shtPosition]<0)||(gfltResults[shtPosition]>1))
        return 0;
    return gfltResults[shtPosition];
}

void interTUDuino::receiveSignal()
{
    QString data=serial.readAll();
    if (gMode==Normal){
        data = data.split("\r\n")[0];
        if (data.length()==1){
            int position = data.toInt();
            if (position<9){
                qDebug()<<position;
                emit peakDetected(position);
                askPeaks();
            }
        }
    }
    else if (gMode==LeftPeak) {
        for (int i=0;i<data.split("\r\n").size();i++){
            gintLeftPeak[gPosCursor] = data.split("\r\n")[i].toInt();
            //qDebug()<<gPosCursor;//"Entered here to start collecting Left Peak";
            if(gPosCursor<4096)
                gPosCursor++;
            if (gPosCursor==4096)
                askPeaks();
        }
    }
    else if (gMode==RightPeak) {
        for (int i=0;i<data.split("\r\n").size();i++){
            gintRightPeak[gPosCursor] = data.split("\r\n")[i].toInt();
            if(gPosCursor<4096)
                gPosCursor++;
            if (gPosCursor==4096){
                gMode = Normal;
                emit gotPeaks();
                askResults();
            }
        }
    }
    else if (gMode==Results) {
        qDebug()<<"trying to get the results";
        for (int i=0;i<data.split("\r\n").size();i++){
            if (gPosCursor>5)
                gPosCursor=0;
            gfltResults[gPosCursor] = data.split("\r\n")[i].toFloat();

            if(gPosCursor<4)
                gPosCursor++;
            if (gPosCursor==4){
                gMode = Normal;
                emit gotResults();
            }
        }
    }

}
