import QtQuick 2.0
import Sailfish.Silica 1.0

CoverBackground {
    Column {
        anchors {
            left: parent.left
            right: parent.right
            verticalCenter: parent.verticalCenter
        }
        spacing: Theme.paddingSmall

        Label {
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            text: batteryMonitor.summary.capacity || "—"
            color: Theme.highlightColor
            font.pixelSize: Theme.fontSizeHuge
        }

        Label {
            width: parent.width - 2 * Theme.paddingMedium
            x: Theme.paddingMedium
            horizontalAlignment: Text.AlignHCenter
            text: batteryMonitor.summary.coverStatus || "—"
            color: Theme.primaryColor
            font.pixelSize: Theme.fontSizeMedium
            wrapMode: Text.WordWrap
        }

        Label {
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            text: batteryMonitor.summary.current || "—"
            color: Theme.secondaryColor
            font.pixelSize: Theme.fontSizeSmall
        }
    }
}
