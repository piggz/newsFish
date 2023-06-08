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

#include <QtQuick>

//#include <sailfishapp.h>
#include "ownnews/newsinterface.h"
#include "ownnews/feedsmodel.h"
#include "ownnews/itemsmodel.h"
#include "Helper.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    qmlRegisterType<FeedsModel>("uk.co.piggz", 1, 0, "FeedsModel");
    qmlRegisterType<ItemsModel>("uk.co.piggz", 1, 0, "ItemsModel");

    auto applicationPath = []()
    {
        QString argv0 = QCoreApplication::arguments()[0];

        if (argv0.startsWith(QChar('/'))) {
            // First, try argv[0] if it's an absolute path (needed for booster)
            return argv0;
        } else {
            // If that doesn't give an absolute path, use /proc-based detection
            return QCoreApplication::applicationFilePath();
        }
    };

    auto appName = [applicationPath]() {
        QFileInfo exe = QFileInfo(applicationPath());
        return exe.fileName();
    };

    auto dataDir = [appName, applicationPath]()
    {
        QFileInfo exe = QFileInfo(applicationPath());

        // "/usr/bin/<appname>" --> "/usr/share/<appname>/"
        QString path = exe.absolutePath();
        int i = path.lastIndexOf(QChar('/')) + 1;
        return path.replace(i, path.length() - i, "share/") + appName();
    };

    auto pathToMainQml = [dataDir, appName]()
    {
        return QUrl::fromLocalFile(
                    QDir::cleanPath(dataDir() + "/qml/newsFish.qml"));
    };

    QQmlApplicationEngine engine;
    const QUrl url(pathToMainQml());
    Helper h;

    NewsInterface interface;

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.rootContext()->setContextProperty("Helper", &h);
    engine.rootContext()->setContextProperty("NewsInterface", &interface);

    engine.load(url);

    return app.exec();
}

