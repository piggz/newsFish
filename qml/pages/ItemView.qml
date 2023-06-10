import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import org.kde.kirigami 2.20 as Kirigami
import uk.co.piggz 1.0

Kirigami.ScrollablePage {
    id: itemView

    property string ititle: ""
    property string body: ""
    property string link: ""
    property string author: ""
    property string pubdate: ""
    property bool unread: false
    property bool starred: false

    title: ititle

    background: Rectangle {
        color: Kirigami.Theme.backgroundColor
        anchors.fill: parent
    }

    actions.main: Kirigami.Action {
        id: addAction
        // Name of icon associated with the action
        icon.name: "applications-internet"
        // Action text, i18n function returns translated string
        text: "Open in Browser"
        // What to do when triggering the action
        onTriggered: Qt.openUrlExternally(link)
    }


    Column {
        id:column
        width: parent.width

        Item {
            id: authorRow
            anchors.left: parent.left
            anchors.right: parent.right

            height: childrenRect.height
            anchors.margins: 5
            Controls.Label {
                id: txtAuthor
                text: author
                anchors.left: parent.left
                width: parent.width / 2
                clip: true
                wrapMode: Text.WordWrap
                font.bold: true
            }

            Controls.Label {
                id: txtPubDate
                text: pubdate
                anchors.right: parent.right
                width: parent.width / 2
                clip: true
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignRight
            }
        }

        Controls.Label {

            id: txtBody
            text: "<html>" + strip_tags(body, "<a><b><p><strong><em><i>") + "</html>"
            textFormat: Text.RichText
            font.pointSize: 12
            wrapMode: Text.Wrap
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 5

            onLinkActivated: {
                console.log(link, ".activated");
                Qt.openUrlExternally(link);
            }

            function strip_tags (input, allowed) {
                allowed = (((allowed || "") + "").toLowerCase().match(/<[a-z][a-z0-9]*>/g) || []).join(''); // making sure the allowed arg is a string containing only tags in lowercase (<a><b><c>)
                var tags = /<\/?([a-z][a-z0-9]*)\b[^>]*>/gi,
                commentsAndPhpTags = /<!--[\s\S]*?-->|<\?(?:php)?[\s\S]*?\?>/gi;
                return input.replace(commentsAndPhpTags, '').replace(tags, function ($0, $1) {
                    return allowed.indexOf('<' + $1.toLowerCase() + '>') > -1 ? $0 : '';
                });
            }
        }
    }

}

