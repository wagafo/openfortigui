#ifndef VPNLOGGER_H
#define VPNLOGGER_H

#include <QObject>
#include <QSignalMapper>
#include <QProcess>
#include <QMap>
#include <QFile>

#include "ticonfmain.h"

class vpnLogger : public QObject
{
    Q_OBJECT
public:
    explicit vpnLogger(QObject *parent = nullptr);
    ~vpnLogger();

public slots:
    void addVPN(const QString &name, QProcess *proc);

private:
    QSignalMapper *logMapperStdout, *logMapperFinished;
    QMap<QString, QProcess*> loggers;
    QMap<QString, QFile*> logfiles;
    QMap<QString, bool> loglocker;
    tiConfMain main_settings;

private slots:
    void logVPNOutput(const QString &name);
    void procFinished(const QString &name);

signals:
    void OTPRequest(QProcess *proc);

public slots:
    void process();
};

#endif // VPNLOGGER_H
