#include "batterymonitor.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QStringList>
#include <QtMath>

static const QString POWER_ROOT = QStringLiteral("/sys/class/power_supply");

BatteryMonitor::BatteryMonitor(QObject *parent)
    : QObject(parent),
      m_jp2026Diagnostics(false)
{
    connect(&m_timer, &QTimer::timeout, this, &BatteryMonitor::refresh);
    m_timer.setInterval(2000);
    m_timer.start();
    refresh();
}

QString BatteryMonitor::readTextFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString();

    return QString::fromUtf8(file.readAll()).trimmed();
}

BatteryMonitor::SupplyData BatteryMonitor::readUevent(const QString &supplyName)
{
    SupplyData result;
    const QString text = readTextFile(POWER_ROOT + QLatin1Char('/') + supplyName + QStringLiteral("/uevent"));

    const QStringList lines = text.split(QLatin1Char('\n'), QString::SkipEmptyParts);
    for (const QString &line : lines) {
        const int pos = line.indexOf(QLatin1Char('='));
        if (pos <= 0)
            continue;

        QString key = line.left(pos);
        if (key.startsWith(QStringLiteral("POWER_SUPPLY_")))
            key.remove(0, 13);

        result.insert(key, line.mid(pos + 1));
    }

    return result;
}

qlonglong BatteryMonitor::integerValue(const SupplyData &data, const QString &key, qlonglong fallback)
{
    bool ok = false;
    const qlonglong result = data.value(key).toLongLong(&ok);
    return ok ? result : fallback;
}

double BatteryMonitor::realValue(const SupplyData &data, const QString &key, double divisor, double fallback)
{
    bool ok = false;
    const double raw = data.value(key).toDouble(&ok);
    return ok && divisor != 0.0 ? raw / divisor : fallback;
}

QString BatteryMonitor::value(const SupplyData &data, const QString &key, const QString &fallback)
{
    return data.contains(key) ? data.value(key) : fallback;
}

QString BatteryMonitor::selectedUsbType(const SupplyData &data)
{
    const QString usb = data.value(QStringLiteral("USB_TYPE"));
    QRegularExpression re(QStringLiteral("\\[([^\\]]+)\\]"));
    const QRegularExpressionMatch match = re.match(usb);
    if (match.hasMatch())
        return match.captured(1);

    return usb.isEmpty() ? QStringLiteral("—") : usb;
}

QString BatteryMonitor::fmt(double number, int decimals)
{
    return QString::number(number, 'f', decimals);
}

QString BatteryMonitor::fmtCurrent(double amps)
{
    if (qAbs(amps) < 1.0)
        return QStringLiteral("%1 mA").arg(fmt(amps * 1000.0, 0));

    return QStringLiteral("%1 A").arg(fmt(amps, 2));
}

QString BatteryMonitor::fmtPower(double watts)
{
    return QStringLiteral("%1 W").arg(fmt(watts, 2));
}

QString BatteryMonitor::fmtVoltage(double volts)
{
    return QStringLiteral("%1 V").arg(fmt(volts, 3));
}

QString BatteryMonitor::fmtTemperature(double celsius)
{
    return QStringLiteral("%1 °C").arg(fmt(celsius, 1));
}

double BatteryMonitor::chargerVoltageToVolts(qlonglong raw)
{
    if (raw <= 0)
        return 0.0;

    // Most power_supply drivers use µV. The MTK master charger on JP2026
    // reports VOLTAGE_NOW in mV (e.g. 4515 or 8766), so handle both.
    if (raw < 100000)
        return raw / 1000.0;

    return raw / 1000000.0;
}

void BatteryMonitor::buildSummary(const SupplyData &battery,
                                  const SupplyData &primary,
                                  const SupplyData &direct,
                                  const SupplyData &master)
{
    QVariantMap s;

    const int capacity = static_cast<int>(integerValue(battery, QStringLiteral("CAPACITY"), -1));
    const QString rawStatus = value(battery, QStringLiteral("STATUS"), QStringLiteral("Unknown"));
    const double current = realValue(battery, QStringLiteral("CURRENT_NOW"), 1000000.0);
    const double currentAvg = realValue(battery, QStringLiteral("CURRENT_AVG"), 1000000.0);
    const double voltage = realValue(battery, QStringLiteral("VOLTAGE_NOW"), 1000000.0);
    const double power = voltage * current;
    const double temp = realValue(battery, QStringLiteral("TEMP"), 10.0);

    const double counterMah = realValue(battery, QStringLiteral("CHARGE_COUNTER"), 1000.0);
    const double fullMah = realValue(battery, QStringLiteral("CHARGE_FULL"), 1000.0);
    const double designMah = realValue(battery, QStringLiteral("CHARGE_FULL_DESIGN"), 1000.0);
    const double condition = designMah > 0.0 ? (fullMah / designMah) * 100.0 : 0.0;

    const bool directOnline = integerValue(direct, QStringLiteral("ONLINE"), 0) > 0;
    const bool directCharging = directOnline
            && value(direct, QStringLiteral("STATUS")).compare(QStringLiteral("Charging"), Qt::CaseInsensitive) == 0;

    const bool primaryOnline = integerValue(primary, QStringLiteral("ONLINE"), 0) > 0;
    const bool primaryCharging =
            value(primary, QStringLiteral("STATUS")).compare(QStringLiteral("Charging"), Qt::CaseInsensitive) == 0;

    const bool masterOnline = integerValue(master, QStringLiteral("ONLINE"), 0) > 0;
    const bool batteryCharging = rawStatus.compare(QStringLiteral("Charging"), Qt::CaseInsensitive) == 0;
    const bool sourceConnected = masterOnline || primaryOnline || directOnline || batteryCharging;

    QString statusUi;
    if (rawStatus.compare(QStringLiteral("Charging"), Qt::CaseInsensitive) == 0)
        statusUi = tr("Charging");
    else if (rawStatus.compare(QStringLiteral("Discharging"), Qt::CaseInsensitive) == 0)
        statusUi = tr("Discharging");
    else if (rawStatus.compare(QStringLiteral("Full"), Qt::CaseInsensitive) == 0)
        statusUi = tr("Full");
    else if (rawStatus.compare(QStringLiteral("Not charging"), Qt::CaseInsensitive) == 0)
        statusUi = tr("Not charging");
    else
        statusUi = rawStatus;

    QString chargingModeUi;
    QString chargingModeExport;
    QString activePath;
    if (batteryCharging && directCharging) {
        chargingModeUi = tr("Direct charging");
        chargingModeExport = QStringLiteral("Direct charging");
        activePath = QStringLiteral("primary_dvchg");
    } else if (batteryCharging && primaryCharging) {
        chargingModeUi = tr("Standard charging");
        chargingModeExport = QStringLiteral("Standard charging");
        activePath = QStringLiteral("primary_chg");
    } else if (batteryCharging) {
        chargingModeUi = tr("Charging");
        chargingModeExport = QStringLiteral("Charging");
        activePath = tr("Unknown");
    } else if (sourceConnected) {
        chargingModeUi = tr("Source connected, not charging");
        chargingModeExport = QStringLiteral("Source connected, not charging");
        activePath = QStringLiteral("—");
    } else {
        chargingModeUi = tr("On battery");
        chargingModeExport = QStringLiteral("On battery");
        activePath = QStringLiteral("—");
    }

    QString usbType = selectedUsbType(primary);
    if (usbType == QStringLiteral("—") || usbType == QStringLiteral("Unknown")) {
        const QString masterType = selectedUsbType(master);
        if (masterType != QStringLiteral("—"))
            usbType = masterType;
    }

    double inputVoltage = 0.0;
    double inputCurrent = 0.0;
    bool inputCurrentKnown = false;

    if (directCharging) {
        inputVoltage = realValue(direct, QStringLiteral("VOLTAGE_NOW"), 1000000.0);
        inputCurrent = realValue(direct, QStringLiteral("CURRENT_NOW"), 1000000.0);
        inputCurrentKnown = true;
    } else if (masterOnline) {
        inputVoltage = chargerVoltageToVolts(integerValue(master, QStringLiteral("VOLTAGE_NOW"), 0));
    }

    const double inputPower = inputVoltage * inputCurrent;

    QString healthUi = value(battery, QStringLiteral("HEALTH"), QStringLiteral("—"));
    if (healthUi == QStringLiteral("Good")) healthUi = tr("Good");
    else if (healthUi == QStringLiteral("Overheat")) healthUi = tr("Overheat");
    else if (healthUi == QStringLiteral("Dead")) healthUi = tr("Dead");
    else if (healthUi == QStringLiteral("Over voltage")) healthUi = tr("Over voltage");
    else if (healthUi == QStringLiteral("Unspecified failure")) healthUi = tr("Unspecified failure");
    else if (healthUi == QStringLiteral("Cold")) healthUi = tr("Cold");

    s.insert(QStringLiteral("capacity"), capacity >= 0 ? QStringLiteral("%1 %").arg(capacity) : QStringLiteral("—"));
    s.insert(QStringLiteral("capacityNumber"), capacity);
    s.insert(QStringLiteral("status"), statusUi);
    s.insert(QStringLiteral("rawStatus"), rawStatus);
    s.insert(QStringLiteral("current"), fmtCurrent(current));
    s.insert(QStringLiteral("averageCurrent"), fmtCurrent(currentAvg));
    s.insert(QStringLiteral("voltage"), fmtVoltage(voltage));
    s.insert(QStringLiteral("power"), fmtPower(power));
    s.insert(QStringLiteral("temperature"), fmtTemperature(temp));
    s.insert(QStringLiteral("health"), healthUi);
    s.insert(QStringLiteral("technology"), value(battery, QStringLiteral("TECHNOLOGY"), QStringLiteral("—")));
    s.insert(QStringLiteral("cycles"), value(battery, QStringLiteral("CYCLE_COUNT"), QStringLiteral("—")));
    s.insert(QStringLiteral("charge"), counterMah > 0.0 ? QStringLiteral("%1 mAh").arg(fmt(counterMah, 0)) : QStringLiteral("—"));
    s.insert(QStringLiteral("full"), fullMah > 0.0 ? QStringLiteral("%1 mAh").arg(fmt(fullMah, 0)) : QStringLiteral("—"));
    s.insert(QStringLiteral("design"), designMah > 0.0 ? QStringLiteral("%1 mAh").arg(fmt(designMah, 0)) : QStringLiteral("—"));
    s.insert(QStringLiteral("condition"), designMah > 0.0 ? QStringLiteral("%1 %").arg(fmt(condition, 0)) : QStringLiteral("—"));

    s.insert(QStringLiteral("sourceConnected"), sourceConnected ? tr("Yes") : tr("No"));
    s.insert(QStringLiteral("chargingMode"), chargingModeUi);
    s.insert(QStringLiteral("usbType"), usbType);
    s.insert(QStringLiteral("activePath"), activePath);
    s.insert(QStringLiteral("inputVoltage"), inputVoltage > 0.0 ? fmtVoltage(inputVoltage) : QStringLiteral("—"));
    s.insert(QStringLiteral("inputCurrent"), inputCurrentKnown ? fmtCurrent(inputCurrent) : QStringLiteral("—"));
    s.insert(QStringLiteral("inputPower"), inputCurrentKnown ? fmtPower(inputPower) : QStringLiteral("—"));

    QString coverStatus;
    if (batteryCharging)
        coverStatus = tr("Charging");
    else if (sourceConnected)
        coverStatus = tr("Connected / not charging");
    else
        coverStatus = tr("Discharging");

    s.insert(QStringLiteral("coverStatus"), coverStatus);

    m_summary = s;

    // Reports intentionally stay English/raw so they are useful on forums and in bug reports.
    m_shortReport =
            QStringLiteral("Battery %1, %2\n"
                           "Battery: %3 / %4 / %5\n"
                           "Charging: %6 / %7\n"
                           "Input: %8 / %9 / %10\n"
                           "Temperature: %11")
            .arg(s.value(QStringLiteral("capacity")).toString())
            .arg(rawStatus)
            .arg(s.value(QStringLiteral("voltage")).toString())
            .arg(s.value(QStringLiteral("current")).toString())
            .arg(s.value(QStringLiteral("power")).toString())
            .arg(chargingModeExport)
            .arg(usbType)
            .arg(s.value(QStringLiteral("inputVoltage")).toString())
            .arg(s.value(QStringLiteral("inputCurrent")).toString())
            .arg(s.value(QStringLiteral("inputPower")).toString())
            .arg(s.value(QStringLiteral("temperature")).toString());
}

QString BatteryMonitor::buildRawPowerSupplyDump() const
{
    QDir dir(POWER_ROOT);
    QStringList names;
    const QStringList all = dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::System, QDir::Name);
    for (const QString &name : all) {
        if (QFileInfo::exists(POWER_ROOT + QLatin1Char('/') + name + QStringLiteral("/uevent")))
            names.append(name);
    }
    names.sort();

    QStringList out;
    for (const QString &name : names) {
        const QString uevent = readTextFile(POWER_ROOT + QLatin1Char('/') + name + QStringLiteral("/uevent"));
        if (uevent.isEmpty())
            continue;

        out << QStringLiteral("===== %1 =====").arg(name);
        out << uevent;
        out << QString();
    }

    return out.join(QLatin1Char('\n')).trimmed();
}

void BatteryMonitor::refresh()
{
    const SupplyData battery = readUevent(QStringLiteral("battery"));
    const SupplyData primary = readUevent(QStringLiteral("primary_chg"));
    const SupplyData direct = readUevent(QStringLiteral("primary_dvchg"));
    const SupplyData master = readUevent(QStringLiteral("mtk-master-charger"));

    m_jp2026Diagnostics = !primary.isEmpty() && !direct.isEmpty() && !master.isEmpty();

    buildSummary(battery, primary, direct, master);

    m_technicalText = buildRawPowerSupplyDump();
    m_updatedText = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"));

    m_diagnosticReport =
            QStringLiteral("FelisBattery 0.1.2\n"
                           "Updated: %1\n"
                           "JP2026 charger diagnostics: %2\n\n"
                           "%3\n\n"
                           "%4")
            .arg(m_updatedText,
                 m_jp2026Diagnostics ? QStringLiteral("available") : QStringLiteral("unsupported / incomplete"),
                 m_shortReport,
                 m_technicalText);

    emit dataChanged();
}
