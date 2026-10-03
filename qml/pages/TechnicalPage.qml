import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"

Page {
    id: page
    allowedOrientations: Orientation.All

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: contentColumn.height + Theme.paddingLarge

        PullDownMenu {
            MenuItem { text: qsTr("Copy short info"); onClicked: Clipboard.text = batteryMonitor.shortReport }
            MenuItem { text: qsTr("Copy diagnostics"); onClicked: Clipboard.text = batteryMonitor.diagnosticReport }
        }

        VerticalScrollDecorator {}

        Column {
            id: contentColumn
            width: parent.width
            spacing: Theme.paddingSmall

            LeftPageHeader { text: qsTr("Technical information") }

            Label {
                width: parent.width - 2 * Theme.horizontalPageMargin
                x: Theme.horizontalPageMargin
                text: qsTr("Updated: %1").arg(batteryMonitor.updatedText)
                color: Theme.secondaryColor
                font.pixelSize: Theme.fontSizeExtraSmall
            }

            Label {
                width: parent.width - 2 * Theme.horizontalPageMargin
                x: Theme.horizontalPageMargin
                text: batteryMonitor.technicalText
                color: Theme.secondaryColor
                font.pixelSize: Theme.fontSizeExtraSmall
                font.family: "monospace"
                wrapMode: Text.WrapAnywhere
            }
        }
    }
}
