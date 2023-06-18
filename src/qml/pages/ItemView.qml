import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as Controls
import org.kde.kirigami 2.20 as Kirigami
import uk.co.piggz 1.0

Kirigami.ScrollablePage {
    id: root

    required property string body
    required property string link
    required property string author
    required property string pubdate
    required property bool unread
    required property bool starred

    Kirigami.Theme.colorSet: Kirigami.Theme.View
    Kirigami.Theme.inherit: false

    actions.main: Kirigami.Action {
        id: addAction
        icon.name: "globe"
        text: i18n("Open in Browser")
        onTriggered: Qt.openUrlExternally(link)
    }

    ColumnLayout {
        Kirigami.Heading {
            text: root.author
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            type: Kirigami.Heading.Primary
        }

        Kirigami.Heading {
            text: root.pubdate
            wrapMode: Text.WordWrap
            opacity: 0.8
            level: 3
            Layout.fillWidth: true
        }

        Controls.Label {
            text: Helper.adjustedContent(width, font.pixelSize, root.body)

            onWidthChanged: text = Helper.adjustedContent(width, font.pixelSize, root.body)
            textFormat: Text.RichText
            font.pixelSize: Kirigami.Theme.defaultFont.pixelSize + 2
            wrapMode: Text.WordWrap

            leftPadding: 0
            rightPadding: 0

            Layout.fillWidth: true

            onLinkActivated: Qt.openUrlExternally(link);
        }
    }
}
