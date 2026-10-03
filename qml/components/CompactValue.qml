import QtQuick 2.0
import Sailfish.Silica 1.0

Item {
    id: root
    property string label: ""
    property string value: "—"
    height: Theme.itemSizeExtraSmall

    Label {
        id: labelItem
        anchors { left: parent.left; right: parent.right; top: parent.top }
        text: root.label
        color: Theme.secondaryColor
        font.pixelSize: Theme.fontSizeExtraSmall
        truncationMode: TruncationMode.Fade
    }

    Label {
        anchors { left: parent.left; right: parent.right; top: labelItem.bottom; topMargin: 1 }
        text: root.value
        color: Theme.primaryColor
        font.pixelSize: Theme.fontSizeSmall
        truncationMode: TruncationMode.Fade
    }
}
