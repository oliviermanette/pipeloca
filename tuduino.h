#ifndef TUDUINO_H
#define TUDUINO_H

#include <QObject>
#include <QDebug>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

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
private:
    QSerialPort serial[2];
    enum Mode {LeftPeak, RightPeak, Normal, Results, FFTR, FFTL};
    Mode gMode[2];
    void askPeaks(int sensorID=0);
    void askResults();
    void askFFTR();
    void askFFTL(int sensorID=0);
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
    void gotResults();
    void gotFFTL();
    void gotFFTR();

public slots:
    void receiveSignal();
    void receiveSignal2();
};

#endif // TUDUINO_H
