#ifndef TUDUINO_H
#define TUDUINO_H

#include <QObject>
#include <QDebug>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include <QDateTime>
#include <QDate>

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
    enum Mode {LeftPeak, RightPeak, Normal, Results, FFTR, FFTL};
    Mode gMode[2];
    void askPeaks(int sensorID=0);
    void askResults();
    void askFFTR();
    void askFFTL(int sensorID=0);

    void calcDiffParameters();
    double calcSumAbs(float *, int size);
    qulonglong calcSumAbs(int *, int size);
    qulonglong tmpLngSumL, tmpLngSumR;
    float calcFreqMoyen(float *, int size);
    float convId2Freq(int lintID, int lintfreqMax=11025, int lintNbStep=1024);
    uint getCurrentTS();
    uint currentPeakTS;
    short gShtBothPeak;
    bool gblStartedPeak;

    int gintLeftPeak[4096];
    int gintRightPeak[4096];
    float gfltResults[4];
    short gPosCursor[2];
    float gfltFFTL[1024];
    float gfltFFTR[1024];
    void traitementSignal(int sensorID=0);

signals:
    void peakDetected(int valPos);
    void gotPeaks();
    void gotBothPeaks();
    void gotResults();
    void gotFFTL();
    void gotFFTR();

public slots:
    void receiveSignal();
    void receiveSignal2();
};

#endif // TUDUINO_H
