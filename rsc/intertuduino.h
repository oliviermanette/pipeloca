#ifndef INTERTUDUINO_H
#define INTERTUDUINO_H

#include <QObject>
#include <QDebug>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

class interTUDuino : public QObject
{
    Q_OBJECT
public:
    explicit interTUDuino(QObject *parent = nullptr);
    ~interTUDuino();
    Q_INVOKABLE void addTempUnitNeuron();
    Q_INVOKABLE void learnTU(char TUId);
    Q_INVOKABLE int getRightPeak(short shtPosition);
    Q_INVOKABLE int getLeftPeak(short shtPosition);
    Q_INVOKABLE float getResult(short shtPosition);
private:
    QSerialPort serial;
    enum Mode {LeftPeak, RightPeak, Normal, Results };
    Mode gMode;
    void askPeaks();
    void askResults();
    int gintLeftPeak[4096];
    int gintRightPeak[4096];
    float gfltResults[4];
    short gPosCursor;

signals:
    void peakDetected(int valPos);
    void gotPeaks();
    void gotResults();

public slots:
    void receiveSignal();
};


#endif // INTERTUDUINO_H
