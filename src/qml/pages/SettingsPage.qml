// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2023 Adam Pigg <adam@piggz.co.uk>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as Controls
import org.kde.kirigami 2.20 as Kirigami
import org.kde.kirigamiaddons.labs.mobileform 0.1 as MobileForm
import uk.co.piggz 1.0

Kirigami.ScrollablePage {
    id: page
    title: i18n("News Fish for Nextcloud")

    leftPadding: 0
    rightPadding: 0

    ColumnLayout {
        width: parent.width

        MobileForm.FormCard {
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
            contentItem: ColumnLayout {
                spacing: 0

                MobileForm.FormCardHeader {
                    title: i18n("Welcome to newsFish")
                }

                MobileForm.FormTextFieldDelegate {
                    id: txtNextcloudURL
                    label: i18n("Nextcloud Url:")
                    inputMethodHints: Qt.ImhUrlCharactersOnly | Qt.ImhNoPredictiveText
                    onAccepted: txtNextcloudUsername.forceActiveFocus();
                }

                MobileForm.FormDelegateSeparator {}

                MobileForm.FormTextFieldDelegate {
                    id: txtNextcloudUsername
                    label: i18n("Username:")
                    onAccepted: txtNextcloudPassword.forceActiveFocus();
                }

                MobileForm.FormDelegateSeparator {}

                MobileForm.FormTextFieldDelegate {
                    id: txtNextcloudPassword
                    label: i18n("Password:")
                    onAccepted: done.clicked();
                    echoMode: TextInput.PasswordEchoOnEdit
                }

                MobileForm.FormDelegateSeparator { above: btnContinue }

                MobileForm.FormButtonDelegate {
                    id: btnContinue
                    text: i18n("Connect")
                    onClicked: {
                        _ownCloudURL = txtNextcloudURL.text;
                        _username = txtNextcloudUsername.text;
                        _password = txtNextcloudPassword.text;

                        saveSettings();

                        pageStack.replace(Qt.resolvedUrl("FeedPage.qml"))
                    }
                }
            }

        }
    }

    Component.onCompleted: {
        loadSettings();
    }

    function loadSettings() {
        console.log("Loading Settings");
        _ownCloudURL = Helper.getSetting("ownCloudURL", "");
        _username = Helper.getSetting("username", "");
        _password = Helper.getSetting("password", "");

        console.log(_ownCloudURL, _username, _password);

        if (_ownCloudURL != "") {
            txtNextcloudURL.text = _ownCloudURL;
        }

        if (_username != "") {
            txtNextcloudUsername.text = _username;
        }

        if (_password != "") {
            txtNextcloudPassword.text = _password;
        }
    }

    function saveSettings() {
        console.log("Saving Settings");
        Helper.setSetting("ownCloudURL", _ownCloudURL);
        Helper.setSetting("username", _username);
        Helper.setSetting("password", _password);

        console.log(_ownCloudURL, _username, _password);
    }
}
