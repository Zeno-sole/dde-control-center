/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wubw <wubowen_cm@deepin.com>
 *
 * Maintainer: wubw <wubowen_cm@deepin.com>
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

#include "usebatterywidget.h"
#include "widgets/titledslideritem.h"
#include "widgets/dccslider.h"
#include "widgets/dccsliderannotated.h"
#include "widgets/optionitem.h"
#include "widgets/switchwidget.h"
#include "widgets/settingsgroup.h"
#include "modules/power/powermodel.h"
#include "window/utils.h"
#include "widgets/comboxwidget.h"
#include "widgets/contentwidget.h"

#include <QPushButton>
#include <QComboBox>
#include <QGSettings>

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::power;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::power;

UseBatteryWidget::UseBatteryWidget(PowerModel *model, QWidget *parent)
    : QWidget(parent)
    , m_layout(new QVBoxLayout)
    , m_monitorSleepOnBattery(new TitledSliderItem(tr("Monitor will suspend after")))
    , m_computerSleepOnBattery(new TitledSliderItem(tr("Computer will suspend after")))
    , m_autoLockScreen(new TitledSliderItem(tr("Lock screen after")))
    , m_cmbPowerBtn(new ComboxWidget(tr("When pressing the power button")))
    , m_cmbCloseLid(new ComboxWidget(tr("When the lid is closed")))
    , m_swBatteryHint(new SwitchWidget(tr("Low Battery Notification")))
    , m_sldLowBatteryHint(new TitledSliderItem(tr("Low battery level")))
    , m_sldAutoSuspend(new TitledSliderItem(tr("Auto suspend battery level")))
//    , m_suspendOnLidClose(new SwitchWidget(tr("Suspend on lid close")))
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QStringList options;
    QStringList annos;
    annos << "1m"
          << "5m"
          << "10m"
          << "15m"
          << "30m"
          << "1h" << tr("Never");

    /*** 超时关闭显示器 ***/
    //~ contents_path /power/On Battery
    //~ child_page On Battery
    m_monitorSleepOnBattery->setAccessibleName(tr("Monitor will suspend after"));
    m_monitorSleepOnBattery->slider()->setType(DCCSlider::Vernier);
    m_monitorSleepOnBattery->slider()->setRange(1, 7);
    m_monitorSleepOnBattery->slider()->setTickPosition(QSlider::TicksBelow);
    m_monitorSleepOnBattery->slider()->setTickInterval(1);
    m_monitorSleepOnBattery->slider()->setPageStep(1);
    m_monitorSleepOnBattery->setAnnotations(annos);
    m_monitorSleepOnBattery->addBackground();
    m_layout->addWidget(m_monitorSleepOnBattery);

    /*** 超时进入待机模式 ***/
    //~ contents_path /power/On Battery
    //~ child_page On Battery
    m_computerSleepOnBattery->setAccessibleName(tr("Computer will suspend after"));
    m_computerSleepOnBattery->slider()->setType(DCCSlider::Vernier);
    m_computerSleepOnBattery->slider()->setRange(1, 7);
    m_computerSleepOnBattery->slider()->setTickPosition(QSlider::TicksBelow);
    m_computerSleepOnBattery->slider()->setTickInterval(1);
    m_computerSleepOnBattery->slider()->setPageStep(1);
    m_computerSleepOnBattery->setAnnotations(annos);
    m_computerSleepOnBattery->addBackground();
    m_layout->addWidget(m_computerSleepOnBattery);

    /*** 超时自动锁屏 ***/
    //~ contents_path /power/On Battery
    //~ child_page On Battery
    m_autoLockScreen->setAccessibleName(tr("Lock screen after"));
    m_autoLockScreen->slider()->setType(DCCSlider::Vernier);
    m_autoLockScreen->slider()->setRange(1, 7);
    m_autoLockScreen->slider()->setTickPosition(QSlider::TicksBelow);
    m_autoLockScreen->slider()->setTickInterval(1);
    m_autoLockScreen->slider()->setPageStep(1);
    m_autoLockScreen->setAnnotations(annos);
    m_autoLockScreen->addBackground();
    m_layout->addWidget(m_autoLockScreen);

    //~ contents_path /power/On Battery
    //~ child_page On Battery
//    m_suspendOnLidClose->setAccessibleName(tr("Suspend on lid close"));

    /*** 按电源按钮功能 ***/
    options << tr("Shut down");
    if (model->getSuspend()) {
        options << tr("Suspend");
    }
    if (model->getHibernate()) {
        options << tr("Hibernate");
    }
    options << tr("Turn off the monitor") << tr("Do nothing");
    m_cmbPowerBtn->setComboxOption(options);
    m_cmbPowerBtn->addBackground();
    m_layout->addWidget(m_cmbPowerBtn);

    /*** 笔记本合盖功能 ***/
    options.pop_front();
    m_cmbCloseLid->setComboxOption(options);
    m_cmbCloseLid->addBackground();
    m_layout->addWidget(m_cmbCloseLid);

    /*** 低电量设置 ***/
    SettingsGroup *lowBatteryGrp = new SettingsGroup(nullptr, SettingsGroup::GroupBackground);
    lowBatteryGrp->layout()->setContentsMargins(0, 0, 0, 0);
    options.clear();
    for (int i = 0; i <= 9; i++) {
        options.append(QString());
    }
    options[0] = "16%";
    options[4] = "20%";
    options[9] = "25%";
    m_sldLowBatteryHint->setAnnotations(options);
    m_sldLowBatteryHint->slider()->setRange(16, 25);
    m_sldLowBatteryHint->slider()->setType(DCCSlider::Vernier);
    m_sldLowBatteryHint->slider()->setTickPosition(QSlider::NoTicks);
    lowBatteryGrp->appendItem(m_swBatteryHint);
    lowBatteryGrp->appendItem(m_sldLowBatteryHint);
    m_layout->addWidget(lowBatteryGrp);

    /*** 自动待机电量设置 ***/
    options.clear();
    for (int i = 0; i <= 8; i++) {
        options.append(QString("%1%").arg(i + 1));
    }
    m_sldAutoSuspend->setAnnotations(options);
    m_sldAutoSuspend->slider()->setRange(1, 9);
    m_sldAutoSuspend->slider()->setType(DCCSlider::Vernier);
    m_sldAutoSuspend->slider()->setTickPosition(QSlider::NoTicks);
    m_sldAutoSuspend->addBackground();
    m_layout->addWidget(m_sldAutoSuspend);

    /*********************/
    m_layout->setAlignment(Qt::AlignTop);
    m_layout->setSpacing(10);
    m_layout->setContentsMargins(10, 10, 10, 5);

    //add scroll
    ContentWidget *contentWgt = new ContentWidget;
    QWidget *mainWgt = new TranslucentFrame;
    mainWgt->setLayout(m_layout);
    contentWgt->setContent(mainWgt);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(contentWgt);
    setLayout(mainLayout);

    setModel(model);

    connect(m_monitorSleepOnBattery->slider(), &DCCSlider::valueChanged, this, &UseBatteryWidget::requestSetScreenBlackDelayOnBattery);
    connect(m_computerSleepOnBattery->slider(), &DCCSlider::valueChanged, this, &UseBatteryWidget::requestSetSleepDelayOnBattery);
    connect(m_autoLockScreen->slider(), &DCCSlider::valueChanged, this, &UseBatteryWidget::requestSetAutoLockScreenOnBattery);



    connect(m_cmbPowerBtn, &ComboxWidget::onIndexChanged, this, [ = ](int nIndex) {
        if (!model->getSuspend()) {
            if (!model->getHibernate()) {
                Q_EMIT requestSetBatteryPressPowerBtnAction(nIndex > 0 ? nIndex + 2 : nIndex);
            } else {
                Q_EMIT requestSetBatteryPressPowerBtnAction(nIndex > 0 ? nIndex + 1 : nIndex);
            }
        } else {
            if (!model->getHibernate()) {
                Q_EMIT requestSetBatteryPressPowerBtnAction(nIndex > 1 ? nIndex + 1 : nIndex);
            } else {
                Q_EMIT requestSetBatteryPressPowerBtnAction(nIndex);
            }
        }
    });
    connect(m_cmbCloseLid, &ComboxWidget::onIndexChanged, [ = ](int nIndex) {
        if (!model->getSuspend()) {
            if (!model->getHibernate()) {
                Q_EMIT requestSetBatteryLidClosedAction(nIndex + 3);
            } else {
                Q_EMIT requestSetBatteryLidClosedAction(nIndex + 2);
            }
        } else {
            if (!model->getHibernate()) {
                Q_EMIT requestSetBatteryLidClosedAction(nIndex > 0 ? nIndex + 2 : nIndex + 1);
            } else {
                Q_EMIT requestSetBatteryLidClosedAction(nIndex + 1);
            }
        }
    });
}

UseBatteryWidget::~UseBatteryWidget()
{

}

void UseBatteryWidget::setModel(const PowerModel *model)
{
    connect(model, &PowerModel::sleepDelayChangedOnBattery, this, &UseBatteryWidget::setSleepDelayOnBattery);
    connect(model, &PowerModel::screenBlackDelayChangedOnBattery, this, &UseBatteryWidget::setScreenBlackDelayOnBattery);
//    connect(model, &PowerModel::sleepOnLidOnBatteryCloseChanged, m_suspendOnLidClose, &SwitchWidget::setChecked);
    connect(model, &PowerModel::batteryLockScreenDelayChanged, this, &UseBatteryWidget::setAutoLockScreenOnBattery);

    setScreenBlackDelayOnBattery(model->screenBlackDelayOnBattery());
    setSleepDelayOnBattery(model->sleepDelayOnBattery());

    connect(m_swBatteryHint, &SwitchWidget::checkedChanged, this, [ = ](bool bLowPowerNotifyEnable) {
        Q_EMIT requestSetLowPowerNotifyEnable(bLowPowerNotifyEnable);
        m_sldLowBatteryHint->setVisible(bLowPowerNotifyEnable);
    });

    Q_EMIT m_swBatteryHint->checkedChanged(model->lowPowerNotifyEnable());

//    m_suspendOnLidClose->setChecked(model->sleepOnLidOnBatteryClose());
    setAutoLockScreenOnBattery(model->getBatteryLockScreenDelay());

    m_computerSleepOnBattery->setVisible(model->canSleep() && model->getSuspend());
    m_sldAutoSuspend->setVisible(model->getSuspend());
//    m_suspendOnLidClose->setVisible(model->canSleep());

    //--------------sp2 add-----------------
    m_cmbCloseLid->setVisible(model->lidPresent());
    if (!model->getSuspend()) {
        if (!model->getHibernate()) {
            m_cmbCloseLid->comboBox()->setCurrentIndex(model->batteryLidClosedAction() - 3);
        } else {
            m_cmbCloseLid->comboBox()->setCurrentIndex(model->batteryLidClosedAction() - 2);
        }
    } else {
        if (!model->getHibernate()) {
            int serverIndex = model->batteryLidClosedAction();
            m_cmbCloseLid->comboBox()->setCurrentIndex(serverIndex > 2 ? serverIndex - 2 : serverIndex - 1);
        } else {
            m_cmbCloseLid->comboBox()->setCurrentIndex(model->batteryLidClosedAction() - 1);
        }
    }
    connect(model, &PowerModel::batteryLidClosedActionChanged, this, [ = ](const int reply) {
        if (reply - 1 < m_cmbCloseLid->comboBox()->count() && reply >= 1) {
            if (!model->getSuspend()) {
                if (!model->getHibernate()) {
                    m_cmbCloseLid->comboBox()->setCurrentIndex(reply - 3);
                } else {
                    m_cmbCloseLid->comboBox()->setCurrentIndex(reply - 2);
                }
            } else {
                if (!model->getHibernate()) {
                    m_cmbCloseLid->comboBox()->setCurrentIndex(reply > 2 ? reply - 2 : reply - 1);
                } else {
                    m_cmbCloseLid->comboBox()->setCurrentIndex(reply - 1);
                }
            }
        }
    });

    int powIndex = model->batteryPressPowerBtnAction();
    if (!model->getSuspend()) {
        if (!model->getHibernate()) {
            m_cmbPowerBtn->comboBox()->setCurrentIndex(powIndex > 0 ? powIndex - 2 : powIndex);
        } else {
            m_cmbPowerBtn->comboBox()->setCurrentIndex(powIndex > 0 ? powIndex - 1 : powIndex);
        }
    } else {
        if (!model->getHibernate()) {
            m_cmbPowerBtn->comboBox()->setCurrentIndex(powIndex > 2 ? powIndex - 1 : powIndex);
        } else {
            m_cmbPowerBtn->comboBox()->setCurrentIndex(model->batteryPressPowerBtnAction());
        }
    }
    connect(model, &PowerModel::batteryPressPowerBtnActionChanged, this, [ = ](const int reply) {
        if (reply < m_cmbPowerBtn->comboBox()->count()) {
            if (!model->getSuspend()) {
                if (!model->getHibernate()) {
                    m_cmbPowerBtn->comboBox()->setCurrentIndex(reply > 0 ? reply - 2 : reply);
                } else {
                    m_cmbPowerBtn->comboBox()->setCurrentIndex(reply > 0 ? reply - 1 : reply);
                }
            } else {
                if (!model->getHibernate()) {
                    m_cmbPowerBtn->comboBox()->setCurrentIndex(reply > 2 ? reply - 1 : reply);
                } else {
                    m_cmbPowerBtn->comboBox()->setCurrentIndex(reply);
                }
            }
        }
    });

    m_swBatteryHint->setChecked(model->lowPowerNotifyEnable());
    connect(model, &PowerModel::lowPowerNotifyEnableChanged, m_swBatteryHint, &SwitchWidget::setChecked);

    onLowPowerNotifyThreshold(model->lowPowerNotifyThreshold());
    connect(model, &PowerModel::lowPowerNotifyThresholdChanged, this, &UseBatteryWidget::onLowPowerNotifyThreshold);
    connect(m_sldLowBatteryHint->slider(), &DCCSlider::valueChanged, this, [ = ](int value) {
        Q_EMIT  requestSetLowPowerNotifyThreshold(value);
    });

    onLowPowerAutoSleepThreshold(model->lowPowerAutoSleepThreshold());
    connect(model, &PowerModel::lowPowerAutoSleepThresholdChanged, this, &UseBatteryWidget::onLowPowerAutoSleepThreshold);
    connect(m_sldAutoSuspend->slider(), &DCCSlider::valueChanged, this, [ = ](int value) {
        Q_EMIT  requestSetLowPowerAutoSleepThreshold(value);
    });
    //--------------------------------------
}

void UseBatteryWidget::setScreenBlackDelayOnBattery(const int delay)
{
    m_monitorSleepOnBattery->slider()->blockSignals(true);
    m_monitorSleepOnBattery->slider()->setValue(delay);
    m_monitorSleepOnBattery->setValueLiteral(delayToLiteralString(delay));
    m_monitorSleepOnBattery->slider()->blockSignals(false);
}

void UseBatteryWidget::setSleepDelayOnBattery(const int delay)
{
    m_computerSleepOnBattery->slider()->blockSignals(true);
    m_computerSleepOnBattery->slider()->setValue(delay);
    m_computerSleepOnBattery->setValueLiteral(delayToLiteralString(delay));
    m_computerSleepOnBattery->slider()->blockSignals(false);
}

void UseBatteryWidget::setAutoLockScreenOnBattery(const int delay)
{
    m_autoLockScreen->slider()->blockSignals(true);
    m_autoLockScreen->slider()->setValue(delay);
    m_autoLockScreen->setValueLiteral(delayToLiteralString(delay));
    m_autoLockScreen->slider()->blockSignals(false);
}

void UseBatteryWidget::onLowPowerNotifyThreshold(const int value)
{
    m_sldLowBatteryHint->slider()->blockSignals(true);
    m_sldLowBatteryHint->slider()->setValue(value);
    m_sldLowBatteryHint->setValueLiteral(QString("%1%").arg(value));
    m_sldLowBatteryHint->slider()->blockSignals(false);
}

void UseBatteryWidget::onLowPowerAutoSleepThreshold(const int value)
{
    m_sldAutoSuspend->slider()->blockSignals(true);
    m_sldAutoSuspend->slider()->setValue(value);
    m_sldAutoSuspend->setValueLiteral(QString("%1%").arg(value));
    m_sldAutoSuspend->slider()->blockSignals(false);
}

QString UseBatteryWidget::delayToLiteralString(const int delay) const
{
    QString strData = "";

    switch (delay) {
    case 1:
        strData = tr("1 Minute");
        break;
    case 2:
        strData = tr("%1 Minutes").arg(5);
        break;
    case 3:
        strData = tr("%1 Minutes").arg(10);
        break;
    case 4:
        strData = tr("%1 Minutes").arg(15);
        break;
    case 5:
        strData = tr("%1 Minutes").arg(30);
        break;
    case 6:
        strData = tr("1 Hour");
        break;
    case 7:
        strData = tr("Never");
        break;
    default:
        strData = tr("%1 Minutes").arg(15);
        break;
    }

    return strData;
}
