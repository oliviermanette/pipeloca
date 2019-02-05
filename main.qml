import QtQuick 2.9
import QtQuick.Window 2.2
import QtCharts 2.2

Window {
    visible: true
    width: 1280
    height: 860
    visibility: Window.FullScreen
    color: "#2f3564"
    title: qsTr("Pipeline Sentinel Demo by FLOD")

    Rectangle {
        id: rectangle
        x: 0
        y: 0
        width: 1280
        height: 117
        color: "#6383c6"
    }

    Rectangle {
        id: rectangle1
        x: 19
        y: 187
        width: 619
        height: 426
        color: "#6383c6"

        ListView {
            id: listView
            x: 8
            y: 8
            width: 603
            height: 410
            model: ListModel{
                id:peakModel
            }
            delegate: Item {
                x: 5
                width: 80
                height: 40
                Row {
                    id: row1
                    spacing: 10
                    Text {
                        text: datetime
                        anchors.verticalCenter: parent.verticalCenter
                        font.bold: true
                    }
                    Text {
                        text: intensiteL
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Text {
                        text: intensiteR
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Text {
                        text: Ratio
                        anchors.verticalCenter: parent.verticalCenter
                        font.bold: true
                    }
                    Text {
                        text: AvgFreqL
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Text {
                        text: AvgFreqR
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
            }
        }
    }

    Text {
        id: element
        x: 19
        y: 132
        color: "#58a0e7"
        text: qsTr("Liste des évènements :")
        styleColor: "#1180ee"
        font.pixelSize: 35
    }

    Text {
        id: element1
        x: 653
        y: 132
        color: "#58a0e7"
        text: qsTr("Graphique de l'évènement :")
        font.pixelSize: 35
        styleColor: "#1180ee"
    }

    ChartView {
        x: 653
        y: 193
        width: 313
        height: 223
        theme: ChartView.ChartThemeLight
        antialiasing: true
        id:chrtSensorL
        margins.top: 0
        margins.left:0
        margins.right:0
        margins.bottom: 0
        legend.visible:false

        ValueAxis
        {
            id: valueAxisY
            min:-4096
            max:4096
        }
        ValueAxis
        {
            id:valueAxisX
            min:0
            max:512
        }
        LineSeries
        {
            id: valeursSensorL
            axisY: valueAxisY
            axisX:valueAxisX
            name: "L"
        }
    }
    ChartView {
        x: 979
        y: 193
        width: 301
        height: 223
        antialiasing: true
            margins.top: 0
            margins.left:0
            margins.right:0
            margins.bottom: 0
            legend.visible:false
        theme: ChartView.ChartThemeLight
        ValueAxis
        {
            id: valueAxisY2
            min:-4096
            max:4096
        }
        ValueAxis
        {
            id:valueAxisX2
            min:0
            max:512
        }
        LineSeries
        {
            id: valeursSensorR
            axisY: valueAxisY2
            axisX:valueAxisX2
            name: "R"
        }
    }
    ChartView {
        x: 653
        y: 413
        width: 301
        height: 223
        antialiasing: true
            margins.top: 0
            margins.left:0
            margins.right:0
            margins.bottom: 0
            legend.visible:false
        theme: ChartView.ChartThemeLight
        ValueAxis
        {
            id: valueAxisYFL
            min:-0
            max:20048
        }
        ValueAxis
        {
            id:valueAxisXFL
            min:0
            max:512
        }
        LineSeries
        {
            id: valeursSensorFFTL
            axisY: valueAxisYFL
            axisX:valueAxisXFL
            name: "R"
        }
    }
    ChartView {
        x: 979
        y: 413
        width: 301
        height: 223
        antialiasing: true
            margins.top: 0
            margins.left:0
            margins.right:0
            margins.bottom: 0
            legend.visible:false
        theme: ChartView.ChartThemeLight
        ValueAxis
        {
            id: valueAxisYFR
            min:-0
            max:20048
        }
        ValueAxis
        {
            id:valueAxisXFR
            min:0
            max:512
        }
        LineSeries
        {
            id: valeursSensorFFTR
            axisY: valueAxisYFR
            axisX:valueAxisXFR
            name: "R"
        }
    }
    Connections{
        target: TuDuino
        onGotPeaks:{
            console.log("Got Peaks on QML");
            for (var i=0;i<4096;i+=8)
            {
                valeursSensorR.append(Number(i/8),TuDuino.getRightPeak(i));
                valeursSensorL.append(Number(i/8),TuDuino.getLeftPeak(i));
            }
        }
        onGotFFTL:{
            console.log("here because we got a FFTL signal");
            valeursSensorFFTL.clear();
            valeursSensorL.clear();
            for (var i=0;i<256;i++)
                valeursSensorFFTL.append(i,TuDuino.getFFTL(i));//*/
            for (var j=0;j<4096;j+=8)
                valeursSensorL.append(Number(j/8),TuDuino.getLeftPeak(j));
        }
        onGotFFTR:{
            console.log("here because we got a FFTR signal");
            valeursSensorR.clear();
            valeursSensorFFTR.clear();
            for (var i=0;i<256;i++)
                valeursSensorFFTR.append(i,TuDuino.getFFTR(i));//*/
            for (var j=0;j<4096;j+=8)
                valeursSensorR.append(Number(j/8),TuDuino.getRightPeak(j));//*/
        }
        onGotBothPeaks:{
            var lclChaine = {
                        "datetime":424255,
                        "intensiteL": 5.95,
                        "intensiteR":48,
                        "Ratio":1.5,
                        "AvgFreqL":3.4,
                        "AvgFreqR":0
                    };
            console.log("here because we got Both Peaks signal !");
            lclChaine.datetime = TuDuino.getPeakTS();
            lclChaine.intensiteL = TuDuino.getIntensiteL();
            lclChaine.intensiteR = TuDuino.getIntensiteR();
            lclChaine.Ratio = TuDuino.getRatio();
            lclChaine.AvgFreqL = TuDuino.getAvgFreqL();
            lclChaine.AvgFreqR = TuDuino.getAvgFreqR();
            peakModel.append(lclChaine);
        }
    }

    Rectangle {
        id: rectangle2
        x: 20
        y: 624
        width: 1240
        height: 221
        color: "#03e3fd"
        border.color: "#ffffff"
        border.width: 0

        Image {
            id: image
            x: 21
            y: 16
            width: 1198
            height: 190
            fillMode: Image.PreserveAspectFit
            source: "img/pipeline_500_2019-Jan-16_04-59-05PM-000.png"
        }
    }
}
