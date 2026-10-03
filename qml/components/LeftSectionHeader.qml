import QtQuick 2.0
import Sailfish.Silica 1.0

Item {
    id: root
    property string text: ""
    width: parent ? parent.width : 0
    height: Theme.itemSizeExtraSmall

    Label {
        anchors {
            left: parent.left
            leftMargin: Theme.horizontalPageMargin
            verticalCenter: parent.verticalCenter
        }
        text: root.text
        color: Theme.highlightColor
        font.pixelSize: Theme.fontSizeMedium
        truncationMode: TruncationMode.Fade
    }
}
