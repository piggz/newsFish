import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import org.kde.kirigami 2.20 as Kirigami

Kirigami.ScrollablePage {
    id: page
    title: "News Fish for nextCloud"


    // Place our content in a Column.  The PageHeader is always placed at the top
    // of the page, followed by our content.
    Column {
        id: column
        width: page.width

        Controls.Label {
            text: "nextCloud URL:"
        }
        Controls.TextField {
            id: txtOwnCloudURL
            anchors.right: parent.right
            anchors.left: parent.left
            inputMethodHints: Qt.ImhNoPredictiveText
        }
        Controls.Label {
            text: "Username:"
        }
        Controls.TextField {
            id: txtOwnCloudUsername
            anchors.right: parent.right
            anchors.left: parent.left
        }
        Controls.Label {
            text: "Passowrd:"
        }
        Controls.TextField {
            id: txtOwnCloudPassword
            anchors.right: parent.right
            anchors.left: parent.left
            echoMode: TextInput.PasswordEchoOnEdit
        }

        Controls.Button {
            id: btnContinue
            text: "Done"

            onClicked: {

                console.log("continue");

                _ownCloudURL = txtOwnCloudURL.text;
                _username = txtOwnCloudUsername.text;
                _password = txtOwnCloudPassword.text;

                saveSettings();

                pageStack.replace(Qt.resolvedUrl("FeedPage.qml"))
            }
        }
    }

    Component.onCompleted: {
        loadSettings();
    }

    function loadSettings() {
        console.log("Loading Settings");
        _ownCloudURL = Helper.getSetting("ownCloudURL", "");
        _username = Helper.getSetting("username", "");
        _password = Helper.getSetting("password", "");

        console.log(_ownCloudURL, _username, _password);

        if (_ownCloudURL != "") {
            txtOwnCloudURL.text = _ownCloudURL;
        }

        if (_username != "") {
            txtOwnCloudUsername.text = _username;
        }

        if (_password != "") {
            txtOwnCloudPassword.text = _password;
        }
    }

    function saveSettings() {
        console.log("Saving Settings");
        Helper.setSetting("ownCloudURL", _ownCloudURL);
        Helper.setSetting("username", _username);
        Helper.setSetting("password", _password);

        console.log(_ownCloudURL, _username, _password);
    }
}
