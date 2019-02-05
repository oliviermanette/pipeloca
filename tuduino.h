#ifndef TUDUINO_H
#define TUDUINO_H

#include <QObject>
#include <QDebug>
#include <QFile>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QString>
#include <QDateTime>
#include <QDate>
#define BUFFER_SIZE 4096

class tuduino : public QObject
{
    Q_OBJECT
public:
    explicit tuduino(QObject *parent = nullptr);
    ~tuduino();
    Q_INVOKABLE void addTempUnitNeuron();
    Q_INVOKABLE void learnTU(char TUId);
    Q_INVOKABLE int getRightPeak(short shtPosition);
    Q_INVOKABLE int getLeftPeak(short shtPosition);
    Q_INVOKABLE float getResult(short shtPosition);
    Q_INVOKABLE float getFFTL(short shtPosition);
    Q_INVOKABLE float getFFTR(short shtPosition);
    Q_INVOKABLE qulonglong getIntensiteL();
    Q_INVOKABLE qulonglong getIntensiteR();
    Q_INVOKABLE float getRatio();
    Q_INVOKABLE float getAvgFreqL();
    Q_INVOKABLE float getAvgFreqR();
    Q_INVOKABLE uint getPeakTS();
private:
    QSerialPort serial[2];
    enum Mode {LeftPeak, RightPeak, Normal, Results, FFTR, FFTL, Calk};
    Mode gMode[2];
    void askPeaks(int sensorID=0);
    void askResults();
    void askFFTR();
    void askFFTL(int sensorID=0);
    void askCalk(int sensorID=0);

    void convertFloatToQByte(QByteArray*, float*, int size);
    void traitementSignal(int sensorID=0);
    void calcDiffParameters();
    double calcSumAbs(float *, int size);
    qulonglong calcSumAbs(int *, int size);
    qulonglong tmpLngSumL, tmpLngSumR;
    float calcFreqMoyen(float *, int size);
    float convId2Freq(int lintID, int lintfreqMax=11025, int lintNbStep=1024);
    uint getCurrentTS();
    bool saveCurrentPeak();
    uint currentPeakTS;
    short gShtBothPeak;
    bool gblStartedPeak;

    int gintLeftPeak[4096];
    int gintRightPeak[4096];
    float gfltDownL[1024],gfltDownR[1024];

    float gfltResults[4];
    short gPosCursor[2];
    short gFFTCursor[2];
    float gfltFFTL[1024];
    float gfltFFTR[1024];

    double gdblMeanL, gdblMeanR;
    int gIntMaxValL, gIntMaxValR, gintMaxPosL, gintMaxPosR;

signals:
    void peakDetected(int valPos);
    void gotPeaks();
    void gotBothPeaks();
    void gotResults();
    void gotFFTL();
    void gotFFTR();
    void gotCalk();

public slots:
    void receiveSignal();
    void receiveSignal2();
    bool savePeak2CSV();
    void receiptSigns();
};

#endif // TUDUINO_H
