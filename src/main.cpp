#ifdef QRC_SOURCE_PATH
#include "runtimeqml/runtimeqml.h"
#endif

#include "MpvPlayerBackend.h"
#include "utils.hpp"
#include <cstdlib>

#include <QApplication>
#include <QProcessEnvironment>
#include <QQmlApplicationEngine>
#include <QtCore>
#include <stdbool.h>

#ifdef WIN32
#include "setenv_mingw.hpp"
#endif

#ifdef GIT_COMMIT_HASH
#include <QtNetwork>
#include <QSequentialIterable>
#include <QJsonDocument>

#endif

#ifdef __linux__
#include <initializer_list>
#include <signal.h>
#include <unistd.h>
void catchUnixSignals(std::initializer_list<int> quitSignals) {
    auto handler = [](int sig) -> void {
        QCoreApplication::quit();
    };
    
    sigset_t blocking_mask;   
    sigemptyset(&blocking_mask);  
    for (auto sig : quitSignals) 
        sigaddset(&blocking_mask, sig);  
        
    struct sigaction sa;   
    sa.sa_handler = handler;   
    sa.sa_mask    = blocking_mask;  
    sa.sa_flags   = 0;    
    
    for (auto sig : quitSignals)   
        sigaction(sig, &sa, nullptr);
}
#endif


int main(int argc, char* argv[])
{
  setenv("QT_QUICK_CONTROLS_STYLE", "Desktop", 1);
  QApplication app(argc, argv);
#ifdef __linux__
  catchUnixSignals({SIGQUIT, SIGINT, SIGTERM, SIGHUP});
#endif

#ifdef GIT_COMMIT_HASH
QString current_version = QString(GIT_COMMIT_HASH);
qDebug() << "Current Version: " << current_version;

QNetworkRequest request(QUrl("https://api.github.com/repos/NamedKitten/KittehPlayer/releases/tags/continuous"));
QNetworkAccessManager nam;
QNetworkReply *reply = nam.get(request);

while(!reply->isFinished()) {
qApp->processEvents();
}
QByteArray response_data = reply->readAll();
QJsonDocument json = QJsonDocument::fromJson(response_data);
QJsonArray jsonArray = json["assets"].toArray();
qDebug() << "Latest Version: " << json["target_commitish"].toString();
if (json["target_commitish"].toString().endsWith(current_version) == 0) {
  qDebug() << "Update Available. Please update ASAP.";
}
#endif

  app.setOrganizationName("KittehPlayer");
  app.setOrganizationDomain("namedkitten.pw");
  app.setApplicationName("KittehPlayer");
  for (int i = 1; i < argc; ++i) {
    if (!qstrcmp(argv[i], "--update")) {
      QString program = QProcessEnvironment::systemEnvironment().value("APPDIR", "") + "/usr/bin/appimageupdatetool";
      QProcess updater;
      updater.setProcessChannelMode(QProcess::ForwardedChannels);
      updater.start(program, QStringList() << QProcessEnvironment::systemEnvironment().value("APPIMAGE", ""));
      updater.waitForFinished();
      exit(0);
    }
  }

  SetDPMS(false);

  QString newpath = QProcessEnvironment::systemEnvironment().value("APPDIR", "") +
                    "/usr/bin:" + QProcessEnvironment::systemEnvironment().value("PATH", "");
  setenv("PATH", newpath.toUtf8().constData(), 1);

  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QApplication::setAttribute(Qt::AA_NativeWindows);

  qmlRegisterType<MpvPlayerBackend>("player", 1, 0, "PlayerBackend");
  std::setlocale(LC_NUMERIC, "C");

  QQmlApplicationEngine engine;
#ifdef QRC_SOURCE_PATH
  RuntimeQML* rt = new RuntimeQML(&engine, QRC_SOURCE_PATH "/qml.qrc");

  rt->setAutoReload(true);
  rt->setMainQmlFilename("main.qml");
  rt->reload();
#else
  engine.load(QUrl(QStringLiteral("qrc:///player/main.qml")));
#endif

  return app.exec();
}
