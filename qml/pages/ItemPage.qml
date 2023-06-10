import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import org.kde.kirigami 2.20 as Kirigami
import uk.co.piggz 1.0

Kirigami.ScrollablePage {
    id: itempage
    property string feedTitle: ""
    title: feedTitle

    ListView {
        id: listView
        model: NewsInterface.itemsModel

        delegate: itemDelegate
    }

    Component {
        id: itemDelegate

        Kirigami.AbstractListItem {
            id: col
            width: listView.width - (2*Kirigami.Units.largeSpacing)


            onClicked: {
                var found = false;
                for (var idx = 0; idx < pageStack.depth; ++idx) {
                    if (pageStack.get(idx) === itemView) {
                        found = true;
                        console.log("Found itemView in stack");
                        break;
                    }
                }
                if (!found) {
                    pageStack.push(itemView)
                }
                itemView.title = itemtitle;
                itemView.body = itembodyhtml;
                itemView.link = itemlink;
                itemView.author = itemauthor;
                itemView.pubdate = timeDifference(new Date(), itempubdate);
                itemView.unread = itemunread;
                itemView.starred = itemstarred;

                NewsInterface.setItemRead(itemid, true);
            }
            Column {
                id: itm
                spacing: Kirigami.Units.largeSpacing
                width: listView.width - (2* Kirigami.Units.largeSpacing)
                anchors.margins: Kirigami.Units.largeSpacing

                Controls.Label{
                    text: itemtitle
                    font.bold: true
                    width: parent.width
                    elide: Text.ElideRight
                }
                Row {
                    width: parent.width

                    Controls.Label {
                        text: itemauthor
                        width: parent.width / 2
                    }
                    Controls.Label {
                        text: itempubdate
                        width: parent.width / 2
                        horizontalAlignment: Text.AlignRight
                    }
                }
                Text {
                    id: txtBody
                    text: itembody
                    wrapMode: Text.Wrap
                    clip: true
                    maximumLineCount: 3
                    width:parent.width
                }
                Rectangle {
                    height: 2
                    width: parent.width
                    x: Kirigami.Units.largeSpacing
                    border.color: Kirigami.Theme.activeTextColor
                    border.width: 1
                }
            }
        }

    }
}





