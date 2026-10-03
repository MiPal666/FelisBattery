import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"

Page {
    id: page
    allowedOrientations: Orientation.All
    property string messageText: ""

    function copied(text) {
        messageText = text
        messageTimer.restart()
    }

    Timer {
        id: messageTimer
        interval: 1800
        repeat: false
        onTriggered: page.messageText = ""
    }

    SilicaFlickable {
        id: flickable
        anchors.fill: parent
        contentHeight: Math.max(height, contentColumn.height + Theme.paddingMedium)

        PullDownMenu {
            MenuItem {
                text: qsTr("Technical information")
                onClicked: pageStack.push(Qt.resolvedUrl("TechnicalPage.qml"))
            }
            MenuItem {
                text: qsTr("Copy short info")
                onClicked: {
                    Clipboard.text = batteryMonitor.shortReport
                    page.copied(qsTr("Short info copied"))
                }
            }
            MenuItem {
                text: qsTr("Copy diagnostics")
                onClicked: {
                    Clipboard.text = batteryMonitor.diagnosticReport
                    page.copied(qsTr("Diagnostics copied"))
                }
            }
            MenuItem {
                text: qsTr("About")
                onClicked: pageStack.push(Qt.resolvedUrl("AboutPage.qml"))
            }
        }

        VerticalScrollDecorator {
            visible: flickable.contentHeight > flickable.height + 2
        }

        Column {
            id: contentColumn
            width: parent.width
            spacing: 0

            LeftPageHeader { text: "FelisBattery" }

            Label {
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                text: batteryMonitor.summary.capacity || "—"
                color: Theme.highlightColor
                font.pixelSize: Theme.fontSizeHuge
            }
            Label {
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                text: batteryMonitor.summary.status || "—"
                color: Theme.primaryColor
                font.pixelSize: Theme.fontSizeLarge
            }
            Label {
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                text: batteryMonitor.summary.current || "—"
                color: Theme.secondaryColor
                font.pixelSize: Theme.fontSizeMedium
            }
            Label {
                visible: page.messageText.length > 0
                width: parent.width - 2 * Theme.horizontalPageMargin
                x: Theme.horizontalPageMargin
                horizontalAlignment: Text.AlignHCenter
                text: page.messageText
                color: Theme.highlightColor
                font.pixelSize: Theme.fontSizeExtraSmall
            }

            LeftSectionHeader { text: qsTr("Battery") }

            Grid {
                id: batteryGrid
                width: parent.width - 2 * Theme.horizontalPageMargin
                x: Theme.horizontalPageMargin
                columns: 2
                columnSpacing: Theme.paddingLarge
                rowSpacing: 0

                CompactValue { width: (batteryGrid.width - batteryGrid.columnSpacing) / 2; label: qsTr("Instant current"); value: batteryMonitor.summary.current || "—" }
                CompactValue { width: (batteryGrid.width - batteryGrid.columnSpacing) / 2; label: qsTr("Average current"); value: batteryMonitor.summary.averageCurrent || "—" }
                CompactValue { width: (batteryGrid.width - batteryGrid.columnSpacing) / 2; label: qsTr("Voltage"); value: batteryMonitor.summary.voltage || "—" }
                CompactValue { width: (batteryGrid.width - batteryGrid.columnSpacing) / 2; label: qsTr("Power"); value: batteryMonitor.summary.power || "—" }
                CompactValue { width: (batteryGrid.width - batteryGrid.columnSpacing) / 2; label: qsTr("Temperature"); value: batteryMonitor.summary.temperature || "—" }
                CompactValue { width: (batteryGrid.width - batteryGrid.columnSpacing) / 2; label: qsTr("Current charge"); value: batteryMonitor.summary.charge || "—" }
                CompactValue { width: (batteryGrid.width - batteryGrid.columnSpacing) / 2; label: qsTr("Full capacity"); value: batteryMonitor.summary.full || "—" }
                CompactValue { width: (batteryGrid.width - batteryGrid.columnSpacing) / 2; label: qsTr("Design capacity"); value: batteryMonitor.summary.design || "—" }
                CompactValue { width: (batteryGrid.width - batteryGrid.columnSpacing) / 2; label: qsTr("Condition"); value: batteryMonitor.summary.condition || "—" }
                CompactValue { width: (batteryGrid.width - batteryGrid.columnSpacing) / 2; label: qsTr("Cycle count"); value: batteryMonitor.summary.cycles || "—" }
                CompactValue { width: (batteryGrid.width - batteryGrid.columnSpacing) / 2; label: qsTr("Health"); value: batteryMonitor.summary.health || "—" }
                CompactValue { width: (batteryGrid.width - batteryGrid.columnSpacing) / 2; label: qsTr("Technology"); value: batteryMonitor.summary.technology || "—" }
            }

            LeftSectionHeader { text: qsTr("Charging") }

            Grid {
                id: chargingGrid
                width: parent.width - 2 * Theme.horizontalPageMargin
                x: Theme.horizontalPageMargin
                columns: 2
                columnSpacing: Theme.paddingLarge
                rowSpacing: 0

                CompactValue { width: (chargingGrid.width - chargingGrid.columnSpacing) / 2; label: qsTr("Source connected"); value: batteryMonitor.summary.sourceConnected || "—" }
                CompactValue { width: (chargingGrid.width - chargingGrid.columnSpacing) / 2; label: qsTr("Mode"); value: batteryMonitor.summary.chargingMode || "—" }
                CompactValue { width: (chargingGrid.width - chargingGrid.columnSpacing) / 2; label: qsTr("Power source type"); value: batteryMonitor.summary.usbType || "—" }
                CompactValue { width: (chargingGrid.width - chargingGrid.columnSpacing) / 2; label: qsTr("Active path"); value: batteryMonitor.summary.activePath || "—" }
                CompactValue { width: (chargingGrid.width - chargingGrid.columnSpacing) / 2; label: qsTr("Input voltage"); value: batteryMonitor.summary.inputVoltage || "—" }
                CompactValue { width: (chargingGrid.width - chargingGrid.columnSpacing) / 2; label: qsTr("Input current"); value: batteryMonitor.summary.inputCurrent || "—" }
                CompactValue { width: (chargingGrid.width - chargingGrid.columnSpacing) / 2; label: qsTr("Input power"); value: batteryMonitor.summary.inputPower || "—" }
            }

            Label {
                width: parent.width - 2 * Theme.horizontalPageMargin
                x: Theme.horizontalPageMargin
                visible: !batteryMonitor.jp2026Diagnostics
                wrapMode: Text.WordWrap
                color: Theme.secondaryColor
                font.pixelSize: Theme.fontSizeExtraSmall
                text: qsTr("Detailed charging diagnostics are designed for Jolla Phone 2026. Basic battery information may still work on another device.")
            }
        }
    }
}
