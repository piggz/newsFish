import QtQuick 2.1
import QtQuick.Controls 2.0 as QQC2
import org.kde.kirigami 2.4 as Kirigami
import "pages"

Kirigami.ApplicationWindow
{
    globalDrawer: Kirigami.GlobalDrawer {
        title: "News Fish"
        titleIcon: "applications-graphics"
        actions: [
            Kirigami.Action {
                text: "Settings"
                icon.name: "view-list-icons"
                onTriggered: {
                    pageStack.replace( Qt.resolvedUrl("pages/SettingsPage.qml") )
                }
            }
        ]
    }

    ItemView {
        id: itemView
    }

    ItemPage {
        id: itemPage
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

    pageStack.popHiddenPages: true
    property string _ownCloudURL: ""
    property string _username: ""
    property string _password: ""

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


