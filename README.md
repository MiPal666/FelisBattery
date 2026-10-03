# FelisBattery

FelisBattery is a small native Sailfish OS battery monitor and read-only charging diagnostic utility designed for **Jolla Phone 2026**.

Although it runs without sandboxing, it only reads battery and charging information from the system and does not modify any settings or write to sysfs.

## 0.1.2

- English is the default UI language
- Czech translation included (`cs_CZ`)
- all user-facing QML text is prepared for translation
- left-aligned page/section headings for a more natural left-to-right diagnostic layout
- main menu moved to the top as a Sailfish `PullDownMenu`
- all battery and charging values remain on one compact main page
- raw technical information stays on a separate page with standard Sailfish back/edge-swipe navigation
- `USB type` renamed to `Power source type`; FelisBattery does not guess the physical connector
- UI health/status values are localized; copied diagnostic reports deliberately stay English/raw
- cat + battery application icon retained

## Displayed battery data

- percentage and state
- instantaneous and average current
- voltage and calculated power
- temperature
- current charge
- full and design capacity
- calculated condition
- cycle count
- health
- technology

## Charging diagnostics

- power source connected
- charging mode
- power-source classification reported by the charging driver (e.g. SDP/DCP/CDP)
- active charging path
- input voltage
- input current and input power when the active driver exposes meaningful values

## Hardware support

Detailed charging diagnostics are designed and tested for **Jolla Phone 2026** and its MediaTek charging stack:

- `battery`
- `primary_chg`
- `primary_dvchg`
- `mtk-master-charger`

Other devices are **unsupported, but you can try them**. Generic battery values may still work if the device exposes the standard Linux power-supply interface.

The application is read-only. It does not modify charging controls.

## Sailjail

FelisBattery intentionally uses `Sandboxing=Disabled`. It is a diagnostic utility that reads low-level power-supply information from sysfs.

## Translations

The source/default language is English. Czech is provided in:

`translations/harbour-felisbattery-cs.ts`

The project uses SailfishApp i18n support (`CONFIG += sailfishapp_i18n`). Additional languages can be added by adding another `.ts` file to `TRANSLATIONS`.

## Build

```sh
sfdk config target=SailfishOS-5.1.0.11-aarch64
sfdk build
```
## License

Copyright © 2026 Michal Kasík

FelisBattery is licensed under the GNU General Public License v3.0 or later (GPL-3.0-or-later).

You are free to use, modify and redistribute the software. If you distribute a modified or derivative version, its corresponding source code must also be made available under the GPL.
