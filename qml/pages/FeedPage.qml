import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import org.kde.kirigami 2.20 as Kirigami
import uk.co.piggz 1.0

Kirigami.ScrollablePage {
    id: feedpage
    title: "Feeds"

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
    }

    Component {
        id: feedDelegate

        Kirigami.BasicListItem {

            onClicked: {
                if (!NewsInterface.busy) {
                    console.log("click", feedid);
                    NewsInterface.viewItems(feedid);
                    var found = false;
                    for (var idx = 0; idx < pageStack.depth; ++idx) {
                        if (pageStack.get(idx) == itemPage) {
                            found = true;
                            console.log("Found itemPage in stack");
                            break;
                        }
                    }

                    if (!found) {
                        pageStack.push(itemPage)
                    }
                    itemPage.feedTitle = feedtitle
                }
            }

            label: feedtitle
            bold: true
            subtitle: feedurl
        }
    }
}
