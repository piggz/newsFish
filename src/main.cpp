/*
  Copyright (C) 2013 Jolla Ltd.
  Contact: Thomas Perl <thomas.perl@jollamobile.com>
  All rights reserved.

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Jolla Ltd nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// #include <sailfishapp.h>
#include "../newsfish-version.h"
#include "Helper.h"
#include "newsinterface.h"
#include <KAboutData>
#include <KLocalizedContext>
#include <KLocalizedString>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#if defined(Q_OS_ANDROID) || defined(Q_OS_SAILFISHOS)
#include <QGuiApplication>
#else
#include <QApplication>
#endif

#ifdef Q_OS_ANDROID
Q_DECL_EXPORT
#endif
int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

#if defined(Q_OS_ANDROID) || defined(Q_OS_SAILFISHOS)
    QGuiApplication app(argc, argv);
#else
    QApplication app(argc, argv);
#endif

#if defined(Q_OS_ANDROID)
    QQuickStyle::setStyle(QStringLiteral("org.kde.breeze"));
#endif

    KLocalizedString::setApplicationDomain("newsfish");

    KAboutData about(QStringLiteral("newsfish"),
                     i18n("newsFish"),
                     QStringLiteral(NEWSFISH_VERSION_STRING),
                     i18n("Nextcloud feed client"),
                     KAboutLicense::GPL_V3,
                     i18n("© Adam Pigg <adam@piggz.co.uk>"));
    about.addAuthor(i18n("Adam Pigg"), i18n("Maintainer"), QStringLiteral("adam@piggz.co.uk"));
    about.setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"), i18nc("EMAIL OF TRANSLATORS", "Your emails"));
    about.setOrganizationDomain("piggz.co.uk");
    about.setBugAddress("https://github.com/piggz/newsFish/issues");

    KAboutData::setApplicationData(about);
    QGuiApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("uk.co.piggz.newsfish")));

    qmlRegisterSingletonType<Helper>("uk.co.piggz", 1, 0, "Helper", [](QQmlEngine *, QJSEngine *) {
        return new Helper;
    });

    qmlRegisterSingletonType<NewsInterface>("uk.co.piggz", 1, 0, "NewsInterface", [](QQmlEngine *, QJSEngine *) {
        return new NewsInterface;
    });

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    QObject::connect(&engine, &QQmlApplicationEngine::quit, &app, &QCoreApplication::quit);

    engine.load(QUrl(QStringLiteral("qrc:/qml/newsFish.qml")));

    return app.exec();
}
