import QtQuick 2.9
import QtQuick.Window 2.2
import QtCharts 2.2

Window {
    visible: true
    //width: 1280
    //height: 860
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
            model: ListModel {
                ListElement {
                    name: "Grey"
                    colorCode: "grey"
                }

                ListElement {
                    name: "Red"
                    colorCode: "red"
                }

                ListElement {
                    name: "Blue"
                    colorCode: "blue"
                }

                ListElement {
                    name: "Green"
                    colorCode: "green"
                }
            }
            delegate: Item {
                x: 5
                width: 80
                height: 40
                Row {
                    id: row1
                    spacing: 10
                    Rectangle {
                        width: 40
                        height: 40
                        color: colorCode
                    }

                    Text {
                        text: name
                        anchors.verticalCenter: parent.verticalCenter
                        font.bold: true
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
