import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.20 as Kirigami
import "pages"
import uk.co.piggz 1.0

Kirigami.ApplicationWindow {
    id: root

    property string _ownCloudURL: ""
    property string _username: ""
    property string _password: ""

    pageStack {
        defaultColumnWidth: Kirigami.Units.gridUnit * 20
        globalToolBar {
            style: Kirigami.ApplicationHeaderStyle.ToolBar
            showNavigationButtons: if (root.pageStack.currentIndex > 0 || root.pageStack.layers.currentIndex > 0) {
                return Kirigami.ApplicationHeaderStyle.ShowBackButton
            } else {
                return 0
            }
        }
    }

    globalDrawer: Kirigami.GlobalDrawer {
        isMenu: true
        actions: Kirigami.Action {
            text: i18nc("@action:button", "Settings")
            icon.name: "configure"
            onTriggered: {
                root.pageStack.pushDialogLayer(Qt.resolvedUrl("pages/SettingsPage.qml"))
            }
        }
    }

    Component.onCompleted: {
        console.log("Loading Settings");
        _ownCloudURL = Helper.getSetting("ownCloudURL", "");
        _username = Helper.getSetting("username", "");
        _password = Helper.getSetting("password", "");

        NewsInterface.serverPath = _ownCloudURL;
        NewsInterface.username = _username;
        NewsInterface.password = _password;

        if (!_ownCloudURL || !_username || !_password) {
            pageStack.push(Qt.resolvedUrl("pages/SettingsPage.qml"), {
                initial: true
            })
        } else {
            pageStack.push(Qt.resolvedUrl("pages/FeedPage.qml"))
        }
    }

    function timeDifference(current, previous) {
        var msPerMinute = 60 * 1000;
        var msPerHour = msPerMinute * 60;
        var msPerDay = msPerHour * 24;
        var msPerMonth = msPerDay * 30;
        var msPerYear = msPerDay * 365;

        var elapsed = current - previous;

        if (elapsed < msPerMinute) {
            return i18n("%1 seconds ago", Math.round(elapsed/1000));
        }

        else if (elapsed < msPerHour) {
            return i18n("%1 minutes ago", Math.round(elapsed/msPerMinute));
        }

        else if (elapsed < msPerDay ) {
            return i18n("%1 hours ago", Math.round(elapsed/msPerHour));
        }

        else if (elapsed < msPerMonth) {
            return i18n("Approximately %1 days ago", Math.round(elapsed / msPerDay));
        }

        else if (elapsed < msPerYear) {
            return i18n("Approximately %1 months ago", Math.round(elapsed / msPerMonth));
        }

        else {
            return i18n("Approximately %1 years ago", Math.round(elapsed / msPerYear));
        }
    }
}


