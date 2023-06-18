import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import org.kde.kirigami 2.20 as Kirigami
import uk.co.piggz 1.0

Kirigami.ScrollablePage {
    id: feedpage
    title: i18n("Feeds")

    actions.main: Kirigami.Action {
        id: addAction
        // Name of icon associated with the action
        icon.name: "view-refresh"
        // Action text, i18n function returns translated string
        text: "Sync"
        // What to do when triggering the action
        onTriggered: NewsInterface.sync(_ownCloudURL, _username, _password, 10)
    }

    supportsRefreshing: !NewsInterface.busy
    onRefreshingChanged: {
        if (refreshing) {
            NewsInterface.sync(_ownCloudURL, _username, _password, 10)
        }
    }

    ListView {
        id: listView
        model: NewsInterface.feedsModel

        delegate: feedDelegate

        Controls.BusyIndicator {
            anchors.centerIn: parent
            running: NewsInterface.busy
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            text: i18n("No feeds found")
            visible: listView.count === 0 && !NewsInterface.busy
            width: parent.width - Kirigami.Units.gridUnit * 4
        }
    }

    Component {
        id: feedDelegate

        Kirigami.BasicListItem {
            id: delegate

            required property int index
            required property string feedId
            required property string title
            required property string link
            required property string faviconLink

            leading: Image {
                source: delegate.faviconLink
                width: height
            }

            onClicked: if (!NewsInterface.busy) {
                NewsInterface.viewItems(delegate.feedId);
                applicationWindow().pageStack.push("qrc:/qml/pages/ItemPage.qml", {
                    title: title,
                });
            }

            label: title
            bold: true
            subtitle: link
        }
    }
}
