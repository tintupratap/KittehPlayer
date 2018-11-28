import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.11
import QtQuick.Window 2.11
import Qt.labs.settings 1.0
import Qt.labs.platform 1.0 as LabsPlatform
import player 1.0

Item {
    id: controlsBarItem
    anchors.bottom: parent.bottom
    anchors.left: parent.left
    anchors.right: parent.right

    property var background: controlsBackground
    property var progress: progressBar
    property var controls: controlsBar
    property var duration: progressBar.to

    Component.onCompleted: {
        setControlsTheme(appearance.themeName)
    }

    function setControlsTheme(themeName) {
        for (var i = 0; i < controlsBar.children.length; ++i) {
            if (controlsBar.children[i].objectName == "buttonLayout") {
                controlsBar.children[i].destroy()
            }
        }

        var component = Qt.createComponent(themeName + "ButtonLayout.qml")
        component.createObject(controlsBar, {
                                   "anchors.fill": controlsBar
                               })
    }

    Item {
        id: subtitlesBar
        visible: !appearance.useMpvSubs
        height: player.height / 8
        anchors.bottom: controlsBackground.top
        anchors.bottomMargin: 5
        anchors.right: parent.right
        anchors.left: parent.left

        RowLayout {
            id: nativeSubtitles
            visible: true
            anchors.left: subtitlesBar.left
            anchors.right: subtitlesBar.right
            height: childrenRect.height
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 10

            Item {
                id: subsContainer
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.rightMargin: 0
                Layout.leftMargin: 0
                Layout.maximumWidth: nativeSubtitles.width
                height: childrenRect.height

                Label {
                    id: nativeSubs
                    objectName: "nativeSubs"
                    onWidthChanged: {

                        if (width > parent.width - 10)
                            width = parent.width - 10
                    }
                    onTextChanged: if (width <= parent.width - 10)
                                       width = undefined
                    color: "white"
                    anchors.horizontalCenter: parent.horizontalCenter
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    font.pixelSize: Screen.height / 24
                    font.family: appearance.fontName
                    horizontalAlignment: Text.AlignHCenter
                    opacity: 1
                    background: Rectangle {
                        id: subsBackground
                        color: appearance.mainBackground
                        width: subsContainer.childrenRect.width
                        height: subsContainer.childrenRect.height
                    }
                    Connections {
                        target: player
                        enabled: true
                        onSubtitlesChanged: function (subtitles) {
                            nativeSubs.text = subtitles
                        }
                    }
                }
            }
        }
    }

    Rectangle {
        id: controlsBackground
        height: controlsBar.visible ? controlsBar.height + progressBar.topPadding
                                      + (fun.nyanCat ? 0 : 1) : 0
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        Layout.fillWidth: true
        Layout.fillHeight: true
        color: appearance.mainBackground
    }

    Item {
        id: controlsBar
        height: controlsBar.visible ? Screen.height / 24 : 0
        anchors.right: parent.right
        anchors.rightMargin: parent.width / 128
        anchors.left: parent.left
        anchors.leftMargin: parent.width / 128
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 2
        visible: true
        VideoProgress {
            id: progressBar
            anchors.bottom: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottomMargin: 0
            bottomPadding: 0
        }
    }
}
