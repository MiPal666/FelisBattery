#ifndef BATTERYMONITOR_H
#define BATTERYMONITOR_H

#include <QObject>
#include <QVariantMap>
#include <QMap>
#include <QString>
#include <QTimer>

class BatteryMonitor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantMap summary READ summary NOTIFY dataChanged)
    Q_PROPERTY(QString technicalText READ technicalText NOTIFY dataChanged)
    Q_PROPERTY(QString shortReport READ shortReport NOTIFY dataChanged)
    Q_PROPERTY(QString diagnosticReport READ diagnosticReport NOTIFY dataChanged)
    Q_PROPERTY(QString updatedText READ updatedText NOTIFY dataChanged)
    Q_PROPERTY(bool jp2026Diagnostics READ jp2026Diagnostics NOTIFY dataChanged)

public:
    explicit BatteryMonitor(QObject *parent = nullptr);

    QVariantMap summary() const { return m_summary; }
    QString technicalText() const { return m_technicalText; }
    QString shortReport() const { return m_shortReport; }
    QString diagnosticReport() const { return m_diagnosticReport; }
    QString updatedText() const { return m_updatedText; }
    bool jp2026Diagnostics() const { return m_jp2026Diagnostics; }

    Q_INVOKABLE void refresh();

signals:
    void dataChanged();

private:
    typedef QMap<QString, QString> SupplyData;

    static QString readTextFile(const QString &path);
    static SupplyData readUevent(const QString &supplyName);
    static qlonglong integerValue(const SupplyData &data, const QString &key, qlonglong fallback = 0);
    static double realValue(const SupplyData &data, const QString &key, double divisor, double fallback = 0.0);
    static QString value(const SupplyData &data, const QString &key, const QString &fallback = QString());
    static QString selectedUsbType(const SupplyData &data);
    static QString fmt(double number, int decimals);
    static QString fmtCurrent(double amps);
    static QString fmtPower(double watts);
    static QString fmtVoltage(double volts);
    static QString fmtTemperature(double celsius);
    static double chargerVoltageToVolts(qlonglong raw);

    QString buildRawPowerSupplyDump() const;
    void buildSummary(const SupplyData &battery,
                      const SupplyData &primary,
                      const SupplyData &direct,
                      const SupplyData &master);

    QVariantMap m_summary;
    QString m_technicalText;
    QString m_shortReport;
    QString m_diagnosticReport;
    QString m_updatedText;
    bool m_jp2026Diagnostics;
    QTimer m_timer;
};

#endif
