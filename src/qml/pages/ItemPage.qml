import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.20 as Kirigami
import uk.co.piggz 1.0

Kirigami.ScrollablePage {
    id: itempage

    ListView {
        id: listView
        model: NewsInterface.itemsModel

        delegate: Kirigami.AbstractListItem {
            id: delegate

            required property int index
            required property string itemid
            required property string itemtitle
            required property string itemauthor
            required property string itembodyhtml
            required property string itemlink
            required property date itempubdate
            required property bool itemunread
            required property bool itemstarred

            separatorVisible: true
            activeBackgroundColor: Qt.rgba(Kirigami.Theme.highlightColor.r, Kirigami.Theme.highlightColor.g, Kirigami.Theme.highlightColor.b, 0.5)
            topPadding: Kirigami.Units.largeSpacing
            bottomPadding: Kirigami.Units.largeSpacing

            onClicked: {
                applicationWindow().pageStack.push("qrc:/qml/pages/ItemView.qml", {
                    title: itemtitle,
                    body: itembodyhtml,
                    link: itemlink,
                    author: itemauthor,
                    pubdate: timeDifference(new Date(), itempubdate),
                    unread: itemunread,
                    starred: itemstarred,
                })

                NewsInterface.setItemRead(itemid, true);

                ListView.view.currentIndex = index;

                // Hack force navigation to the next page on desktop
                applicationWindow().pageStack.currentIndex = 1
                applicationWindow().pageStack.currentIndex = 2
            }

            contentItem: ColumnLayout {
                spacing: Kirigami.Units.smallSpacing * 2

                Controls.Label {
                    Layout.fillWidth: true
                    text: delegate.itemtitle
                    font.pointSize: Math.round(Kirigami.Theme.defaultFont.pointSize * 1)
                    font.weight: Font.Medium
                    wrapMode: Text.Wrap
                    elide: Text.ElideRight
                    maximumLineCount: 3
                    opacity: delegate.itemunread ? 1 : 0.7
                }

                Controls.Label {
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                    font.pointSize: Kirigami.Theme.smallFont.pointSize
                    opacity: 0.9
                    text: delegate.itempubdate.toLocaleString(Qt.locale(), Locale.ShortFormat) + (delegate.itemauthor.length === 0 ? "" : " " + i18nc("by <author(s)>", "by") + " " + delegate.itemauthor)
                }
            }
        }
    }
}
