#include "tuduino.h"

tuduino::tuduino(QObject *parent) : QObject(parent)
{
    serial[0].setPortName("/dev/cu.usbmodem3226300");
    serial[0].open(QIODevice::ReadWrite);
    QObject::connect(&serial[0], SIGNAL(readyRead()),this,SLOT(receiveSignal()));
    gMode[0] = Normal;

    serial[1].setPortName("/dev/cu.usbmodem3415320");
    serial[1].open(QIODevice::ReadWrite);
    QObject::connect(&serial[1], SIGNAL(readyRead()),this,SLOT(receiveSignal2()));
    gMode[1] = Normal;
}

tuduino::~tuduino()
{
    serial[0].close();
}

void tuduino::addTempUnitNeuron()
{
    QByteArray data="a";
    serial[0].write(data);
    if( serial[0].waitForBytesWritten(100) )
        qDebug() << "sent: " << data;
}

void tuduino::learnTU(char TUId)
{
    QByteArray data="l";
    data.append(TUId+48);
    serial[0].write(data);
    if( serial[0].waitForBytesWritten(100) )
        qDebug() << "sent: " << data;
}

int tuduino::getRightPeak(short shtPosition)
{
    return gintRightPeak[shtPosition];
}

int tuduino::getLeftPeak(short shtPosition)
{
    return gintLeftPeak[shtPosition];
}

void tuduino::askPeaks(int sensorID)
{
    QByteArray data;
    if (gMode[sensorID] == Normal){
        gMode[sensorID] = LeftPeak;
        gPosCursor[sensorID] = 0;
        data="L";
    }
    else if ((gMode[sensorID]==LeftPeak) &&(gPosCursor[sensorID]==4096)) {
        gMode[sensorID] = RightPeak;
        gPosCursor[sensorID] = 0;
        data="R";
    }
    else
        return;

    serial[sensorID].write(data);
    if( serial[sensorID].waitForBytesWritten(100) )
        qDebug() << "sent: " << data <<"("<<sensorID<<")";
}

void tuduino::askResults()
{
    QByteArray data="r";
    gMode[0] = Results;
    gPosCursor[0] = 0;
    serial[0].write(data);
    if( serial[0].waitForBytesWritten(100) )
        qDebug() << "sent: " << data;
}

void tuduino::askFFTR()
{
    gMode[0] = FFTR;
    QByteArray data="F";
    gPosCursor[0] = 0;
    serial[0].write(data);
    if( serial[0].waitForBytesWritten(100) )
        qDebug() << "sent: " << data;
}

void tuduino::askFFTL(int sensorID)
{
    gMode[sensorID] = FFTL;
    QByteArray data="f";
    gPosCursor[sensorID] = 0;
    serial[sensorID].write(data);
    if( serial[sensorID].waitForBytesWritten(100) )
        qDebug() << "sent: " << data << "("<<sensorID<<")";
}

void tuduino::traitementSignal(int sensorID)
{
    QString data=serial[sensorID].readAll();
    if (gMode[sensorID]==Normal){
        data = data.split("\r\n")[0];
        if (data.length()==1){
            int position = data.toInt();
            if (position<9){
                qDebug()<<position;
                emit peakDetected(position);
                askPeaks(sensorID);
            }
        }
    }
    else if (gMode[sensorID]==LeftPeak) {
        //qDebug()<<"Entered here to start collecting Left Peak from sensor : "<<sensorID;
        //qDebug()<<data;
        for (int i=0;i<data.split("\r\n").size();i++){
            if (sensorID==0)
                gintLeftPeak[gPosCursor[sensorID]] = data.split("\r\n")[i].toInt();
            else if (sensorID==1)
                gintRightPeak[gPosCursor[sensorID]] = data.split("\r\n")[i].toInt();

            if(gPosCursor[sensorID]<4096)
                gPosCursor[sensorID]++;
            if (gPosCursor[sensorID]==4096){
                askFFTL(sensorID);
                emit gotPeaks();
            }

        }
    }
    else if (gMode[sensorID]==RightPeak) {
        for (int i=0;i<data.split("\r\n").size();i++){
            gintRightPeak[gPosCursor[sensorID]] = data.split("\r\n")[i].toInt();
            if(gPosCursor[sensorID]<4096)
                gPosCursor[sensorID]++;
            if (gPosCursor[sensorID]==4096){
                gMode[sensorID] = Normal;
                emit gotPeaks();
                askResults();
            }
        }
    }
    else if (gMode[sensorID]==Results) {
        qDebug()<<"trying to get the results";
        for (int i=0;i<data.split("\r\n").size();i++){
            if (gPosCursor[sensorID]>5)
                gPosCursor[sensorID]=0;
            gfltResults[gPosCursor[sensorID]] = data.split("\r\n")[i].toFloat();

            if(gPosCursor[sensorID]<4)
                gPosCursor[sensorID]++;
            if (gPosCursor[sensorID]==4){
                gMode[sensorID] = Normal;
                emit gotResults();
            }
        }
    }
    else if (gMode[sensorID]==FFTL) {
        for (int i=0;i<data.split("\r\n").size();i++){
            if (sensorID==0)
                gfltFFTL[gPosCursor[sensorID]] = data.split("\r\n")[i].toFloat();
            else if (sensorID==1)
                gfltFFTR[gPosCursor[sensorID]] = data.split("\r\n")[i].toFloat();
            if(gPosCursor[sensorID]<1024)
                gPosCursor[sensorID]++;
            if (gPosCursor[sensorID]==1024){
                gMode[sensorID] = Normal;
                emit gotFFTL();
            }
        }
    }
    else if (gMode[sensorID]==FFTR) {
        for (int i=0;i<data.split("\r\n").size();i++){
            gfltFFTR[gPosCursor[sensorID]] = data.split("\r\n")[i].toFloat();
            if(gPosCursor[sensorID]<1024)
                gPosCursor[sensorID]++;
            if (gPosCursor[sensorID]==1024){
                gMode[sensorID] = Normal;
                emit gotFFTR();
            }
        }
    }
}

void tuduino::receiveSignal2()
{
    traitementSignal(1);
}

float tuduino::getResult(short shtPosition)
{
    if ((gfltResults[shtPosition]<0)||(gfltResults[shtPosition]>1))
        return 0;
    return gfltResults[shtPosition];
}

float tuduino::getFFTL(short shtPosition)
{
    return gfltFFTL[shtPosition];
}

float tuduino::getFFTR(short shtPosition)
{
    return gfltFFTR[shtPosition];
}

void tuduino::receiveSignal()
{
    traitementSignal(0);
}
