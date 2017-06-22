import QtQuick 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import com.aucalic.client 1.0

Pane {
    id: pane
    property JamApiManager jamApiManager
    signal loggedIn

    ColumnLayout {
        function loginFinished() {
            mouseArea.cursorShape = Qt.ArrowCursor
            if (jamApiManager.isLoggedIn()) {
                pane.loggedIn()
            }
        }

        Component.onCompleted: jamApiManager.loginFinished.connect(loginFinished)

        Text {
            text: qsTr("Login")
        }
        Text {
            id: errorField
            text: jamApiManager.loginError
        }
        TextField {
            id: username
            placeholderText: qsTr("Username")
        }
        TextField {
            id: password
            placeholderText: qsTr("Password")
            echoMode: TextInput.Password
        }
        Button {
            text: qsTr("Log in")
            onClicked: {
                mouseArea.cursorShape = Qt.WaitCursor
                jamApiManager.username = username.text
                jamApiManager.password = password.text
                jamApiManager.login()
            }
        }
        Text {
            text: '<a href="https://aucalic.com/accounts/register/">' + qsTr("Click here") + '</a> ' + qsTr("to create a new account")
            onLinkActivated: Qt.openUrlExternally(link)
            MouseArea {
                anchors.fill: parent
                cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.Arrowcursor;
                acceptedButtons: Qt.NoButton
            }
        }
        MouseArea {
            id: mouseArea
            anchors.fill: parent
            acceptedButtons: Qt.NoButton // don't consume events
        }
    }
}
