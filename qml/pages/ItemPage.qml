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

        Rectangle {
            id: col
            width: listView.width
            height: childrenRect.height
            gradient: Gradient {
                GradientStop { position: 0.0; color: Kirigami.Theme.backgroundColor }
                GradientStop { position: 0.8; color: Kirigami.Theme.backgroundColor }
                GradientStop { position: 1.0; color: Kirigami.Theme.alternateBackgroundColor }
            }
            MouseArea {
                width: parent.width
                height: col.height

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
            }
            Column {
                id: itm
                spacing: Kirigami.Units.largeSpacing
                padding: spacing

                Controls.Label{
                    text: itemtitle
                    font.bold: true
                    width: listView.width - (2* Kirigami.Units.largeSpacing)
                    elide: Text.ElideRight
                }
                Row {
                    width: listView.width - (2* Kirigami.Units.largeSpacing)

                    Controls.Label {
                        text: itemauthor
                        width: parent.width / 2 - Kirigami.Units.largeSpacing
                    }
                    Controls.Label {
                        text: itempubdate
                        width: parent.width / 2 - Kirigami.Units.largeSpacing
                        horizontalAlignment: Text.AlignRight
                    }
                }
                Text {
                    id: txtBody
                    text: itembody
                    wrapMode: Text.Wrap
                    clip: true
                    maximumLineCount: 3
                    width: listView.width - (2* Kirigami.Units.largeSpacing)
                }
            }
        }

    }
}





