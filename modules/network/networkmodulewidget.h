/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NETWORKMODULEWIDGET_H
#define NETWORKMODULEWIDGET_H

#include "modulewidget.h"
#include "nextpagewidget.h"

namespace dcc{
namespace widgets {
class SettingsGroup;
}

namespace network {

class NetworkDevice;
class WirelessDevice;
class NetworkModel;
class NetworkModuleWidget : public dcc::ModuleWidget
{
    Q_OBJECT

public:
    explicit NetworkModuleWidget();

    void setModel(NetworkModel *model);

signals:
    void requestShowVpnPage() const;
    void requestShowPppPage() const;
    void requestShowProxyPage() const;
    void requestShowInfomation() const;
    void requestShowChainsPage() const;
    void requestHotspotPage(WirelessDevice *wdev) const;
    void requestShowDeviceDetail(NetworkDevice *device) const;
    void requestDeviceEnable(const QString &devPath, const bool enable) const;

private slots:
    void onDeviceListChanged(const QList<NetworkDevice *> &devices);
    void onDevStatusChanged(const QString &statString);
    void onNextPageClicked();

private:
    void createDeviceGroup(NetworkDevice *dev, const int number, const bool multiple);

private:
    QVBoxLayout *m_devicesLayout;
    dcc::widgets::NextPageWidget *m_pppBtn;
    dcc::widgets::NextPageWidget *m_vpnBtn;
    dcc::widgets::NextPageWidget *m_proxyBtn;
    dcc::widgets::NextPageWidget *m_detailBtn;

    QMap<dcc::widgets::NextPageWidget *, NetworkDevice *> m_devicesWidgets;
};

}   // namespace network

}   // namespace dcc

#endif // NETWORKMODULEWIDGET_H
