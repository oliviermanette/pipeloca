#include "tuduino.h"

tuduino::tuduino(QObject *parent) : QObject(parent)
{
    serial[0].setPortName("/dev/cu.usbmodem3226300");
    serial[0].open(QIODevice::ReadWrite);
    QObject::connect(&serial[0], SIGNAL(readyRead()),this,SLOT(receiveSignal()));
    gMode[0] = Normal;

    serial[1].setPortName("/dev/cu.usbmodem3422600");
    serial[1].open(QIODevice::ReadWrite);
    QObject::connect(&serial[1], SIGNAL(readyRead()),this,SLOT(receiveSignal2()));
    gMode[1] = Normal;
    gShtBothPeak = 0;
    gblStartedPeak = false;
    for (int i=0;i<1024;i++){
        gfltFFTL[i]=0;
        gfltFFTR[i]=0;
    }
    QObject::connect(this,SIGNAL(gotBothPeaks()),this,SLOT(savePeak2CSV()));
    QObject::connect(this,SIGNAL(gotFFTL()),this,SLOT(receiptSigns()));
    QObject::connect(this,SIGNAL(gotFFTR()),this,SLOT(receiptSigns()));
    QObject::connect(this,SIGNAL(gotCalk()),this,SLOT(receiptSigns()));
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
    serial[sensorID].write(data);
    if( serial[sensorID].waitForBytesWritten(100) )
        qDebug() << "sent: " << data << "("<<sensorID<<")";
    gPosCursor[sensorID] = 0;
}

void tuduino::askCalk(int sensorID)
{
    gMode[sensorID] = Calk;
    QByteArray data="c";
    serial[sensorID].write(data);
    if( serial[sensorID].waitForBytesWritten(100) )
        qDebug() << "sent: " << data << "("<<sensorID<<")";
}

void tuduino::convertFloatToQByte(QByteArray *qb , float *fl, int size)
{
    for (int i=0; i<size;i++)
        qb->append(reinterpret_cast<const char*>(&fl), sizeof(fl));
}

void tuduino::calcDiffParameters()
{
    qDebug()<<"Calcule les paramètres";
    //Down sample les pics pour commencer
    int j = 0;
    short DownSamplingRatio = 4;

    for (int i = 0; i < BUFFER_SIZE; i+=DownSamplingRatio) {
        gfltDownL[j]=0;
        gfltDownR[j]=0;
        for (int k = i; k < i+DownSamplingRatio; k++) {
              gfltDownL[j]+=gintLeftPeak[k];
              gfltDownR[j]+=gintRightPeak[k];
        }
        gfltDownL[j]/=DownSamplingRatio;
        gfltDownR[j]/=DownSamplingRatio;
        j++;
    }
}

double tuduino::calcSumAbs(float *vector, int size)
{
    double ldblOutput=0;
    for (int i=0;i<size;i++){
        if (vector[i]<0)
            ldblOutput += static_cast<double>(vector[i]*-1);
        else ldblOutput += static_cast<double>(vector[i]);
    }
    return ldblOutput;
}

qulonglong tuduino::calcSumAbs(int *vector, int size)
{
    qulonglong ldblOutput=0;
    for (int i=0;i<size;i++){
        if (vector[i]<0)
            ldblOutput += static_cast<qulonglong>(vector[i]*-1);
        else ldblOutput += static_cast<qulonglong>(vector[i]);
    }
    return ldblOutput;
}

float tuduino::calcFreqMoyen(float *vector, int size)
{
    float lFltOut=0;
    double lDblsum=0;
    double ldblCoeff=0;
    for (int i=0;i<size;i++){
        if (i<4)
            qDebug()<<"i="<<i<<", freq="<<vector[i];
        ldblCoeff += i*static_cast<double>(vector[i]);
        lDblsum += static_cast<double>(vector[i]);
    }
    lFltOut = static_cast<float>(ldblCoeff/lDblsum);
    qDebug()<<"Freq Moyenne:"<<lFltOut<<"; num:"<<ldblCoeff<<"; denum:"<<lDblsum;
    return lFltOut;
}

float tuduino::convId2Freq(int lintID, int lintfreqMax, int lintNbStep)
{
    return static_cast<float>(lintID*lintfreqMax/lintNbStep);
}

uint tuduino::getCurrentTS()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    /* Returns the datetime as the number of seconds that have passed since 1970-01-01T00:00:00, Coordinated Universal Time  */
    uint unixtime = currentDateTime.toTime_t();
    return unixtime;
}

bool tuduino::savePeak2CSV()
{
    qDebug() << "here to save the file";
    QFile file("/Users/oliviermanette/Documents/datapipes/FFT_"+QString::number(currentPeakTS)+".csv");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    QTextStream out(&file);
    out << "Timestamp; Intensite L;Intensite R;ratio;freq moyenne L;freq moyenne R;Mean L;Mean R; Max Val L; Max Val R; Max Pos L; Max Pos R\r\n";
    out << currentPeakTS << ";"
        << getIntensiteL() << ";"
        << getIntensiteR() << ";"
        << getRatio() << ";"
        << getAvgFreqL()<< ";"
        << getAvgFreqR()<< ";"
        << gdblMeanL << ";"
        << gdblMeanR << ";"
        << gIntMaxValL << ";"
        << gIntMaxValR << ";"
        << gintMaxPosL << ";"
        << gintMaxPosR << ";"
        << "\r\n";
    out << "\r\n";
    out << "Freq;FFTL;FFTR\r\n";
    for (int i = 0; i < 512; ++i) {
        out << convId2Freq(i) << ";" << gfltFFTL[i] << ";" << gfltFFTR[i] << "\r\n";
    }
    file.close();
    saveCurrentPeak();
    return true;
}
bool tuduino::saveCurrentPeak(){
    qDebug() << "here to save the file";
    QFile file("/Users/oliviermanette/Documents/datapipes/Peak_"+QString::number(currentPeakTS)+".csv");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    QTextStream out(&file);
    out << "Left;Right\r\n";
    for (short i = 0; i < 1024; ++i) {
        out << gfltDownL[i] << ";" << gfltDownR[i] << "\r\n";
    }
    file.close();
    return true;
}

void tuduino::receiptSigns()
{
    gShtBothPeak++;
    if (gShtBothPeak==2){
        askCalk(0);
        askCalk(1);
    }
    else if (gShtBothPeak==4){
        calcDiffParameters();
        gShtBothPeak=0;
        gblStartedPeak = false;
        gMode[0]=Normal;
        gMode[1]=Normal;
        emit gotBothPeaks();

    }
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
                if ((!gblStartedPeak)||((getCurrentTS()-currentPeakTS)>1500)){ //Vérifier que le timestamp est en ms
                    currentPeakTS = getCurrentTS();
                    gblStartedPeak = true;
                }
                emit peakDetected(position);
                askPeaks(sensorID);
            }
        }
    }
    else if (gMode[sensorID]==LeftPeak) {
        //qDebug()<<"Entered here to start collecting Left Peak from sensor : "<<sensorID;
        bool ok;
        for (int i=0;i<data.split("\r\n").size();i++){
            ok = false;
            if (sensorID==0)
                gintLeftPeak[gPosCursor[sensorID]] = data.split("\r\n")[i].toInt(&ok);
            else if (sensorID==1)
                gintRightPeak[gPosCursor[sensorID]] = data.split("\r\n")[i].toInt(&ok);

            if ((gPosCursor[sensorID]<4096)&&(ok))
                gPosCursor[sensorID]++;
            if (gPosCursor[sensorID]==4096){
                askFFTL(sensorID);
                break;
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
                break;
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
                break;
            }
        }
    }
    else if (gMode[sensorID]==FFTL) {
        bool ok;
        for (int i=0;i<data.split("\r\n").size();i++){
            ok = false;
            if (sensorID==0)
                gfltFFTL[gPosCursor[sensorID]] = data.split("\r\n")[i].toFloat(&ok);
            else if (sensorID==1)
                gfltFFTR[gPosCursor[sensorID]] = data.split("\r\n")[i].toFloat(&ok);
            if ((gPosCursor[sensorID]<1024)&&(ok))
                gPosCursor[sensorID]++;
            if (gPosCursor[sensorID]==1024){
                gMode[sensorID] = Normal;
                if (sensorID==0)
                    emit gotFFTL();
                else if (sensorID==1)
                    emit gotFFTR();
                gPosCursor[sensorID]=0;
                break;
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
    else if (gMode[sensorID]==Calk){
        bool ok=false;
        if (data.split("\r\n").size()>=2){
            if (sensorID==0){
                gdblMeanL = data.split("\r\n")[0].toDouble(&ok);
                if (ok){
                    gIntMaxValL = data.split("\r\n")[1].toInt(&ok);
                    gintMaxPosL = data.split("\r\n")[2].toInt(&ok);
                }
                else {
                    gdblMeanL = -1;
                    gIntMaxValL = -1;
                    gintMaxPosL = -1;
                    gMode[sensorID]=Normal;
                }
            }
            else if (sensorID==1){
                gdblMeanR = data.split("\r\n")[0].toDouble(&ok);
                if (ok){
                    gIntMaxValR = data.split("\r\n")[1].toInt(&ok);
                    gintMaxPosR = data.split("\r\n")[2].toInt(&ok);
                }
                else {
                    gdblMeanR = -1;
                    gIntMaxValR = -1;
                    gintMaxPosR = -1;
                    gMode[sensorID]=Normal;
                }

            }
            if (ok)
                emit gotCalk();
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

qulonglong tuduino::getIntensiteL()
{
    tmpLngSumL = calcSumAbs(&gintLeftPeak[0], 4096);
    return tmpLngSumL;
}

qulonglong tuduino::getIntensiteR()
{
    tmpLngSumR = calcSumAbs(&gintRightPeak[0], 4096);
    return tmpLngSumR;
}

float tuduino::getRatio()
{
    return static_cast<float>(tmpLngSumL)/static_cast<float>(tmpLngSumR);
}

float tuduino::getAvgFreqL()
{
    for (int i=0;i<4;i++)
        qDebug()<<"preFFTL "<<i<<": "<<gfltFFTL[i];
    return calcFreqMoyen(&gfltFFTL[0],512); //convId2Freq
}

float tuduino::getAvgFreqR()
{
    for (int i=0;i<4;i++)
        qDebug()<<"preFFTR "<<i<<": "<<gfltFFTR[i];
    return calcFreqMoyen(&gfltFFTR[0],512); //convId2Freq
}

uint tuduino::getPeakTS()
{
    return currentPeakTS;
}

void tuduino::receiveSignal()
{
    traitementSignal(0);
}
