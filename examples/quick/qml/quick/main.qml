import QtQuick 2.0

Rectangle {
    width: 640
    height: 480
    Image {
        id: clock_back
        source: "image://plist/clock_background"
        anchors.centerIn: parent
    }
    Image {
        id: clock_night
        source: 'image://plist/clock_night'
        x: clock_back.x
        y: clock_back.y
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            Qt.quit();
        }
    }
}
