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
            showNavigationButtons: if (applicationWindow().pageStack.currentIndex > 0
                || applicationWindow().pageStack.currentIndex > 0) {
                Kirigami.ApplicationHeaderStyle.ShowBackButton
            } else {
                0
            }
        }
    }

    globalDrawer: Kirigami.GlobalDrawer {
        isMenu: true
        actions: Kirigami.Action {
            text: i18nc("@action:button", "Settings")
            icon.name: "configure"
            onTriggered: {
                pageStack.replace( Qt.resolvedUrl("pages/SettingsPage.qml") )
            }
        }
    }

    Component.onCompleted: {
        console.log("Loading Settings");
        _ownCloudURL = Helper.getSetting("ownCloudURL", "");
        _username = Helper.getSetting("username", "");
        _password = Helper.getSetting("password", "");

        if (!_ownCloudURL || !_username || !_password) {
            pageStack.push( Qt.resolvedUrl("pages/SettingsPage.qml") )
        } else {
            pageStack.push( Qt.resolvedUrl("pages/FeedPage.qml") )
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
            return Math.round(elapsed/1000) + ' seconds ago';
        }

        else if (elapsed < msPerHour) {
            return Math.round(elapsed/msPerMinute) + ' minutes ago';
        }

        else if (elapsed < msPerDay ) {
            return Math.round(elapsed/msPerHour ) + ' hours ago';
        }

        else if (elapsed < msPerMonth) {
            return 'approximately ' + Math.round(elapsed/msPerDay) + ' days ago';
        }

        else if (elapsed < msPerYear) {
            return 'approximately ' + Math.round(elapsed/msPerMonth) + ' months ago';
        }

        else {
            return 'approximately ' + Math.round(elapsed/msPerYear ) + ' years ago';
        }
    }
}


