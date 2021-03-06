#include "vpnprofileeditor.h"
#include "ui_vpnprofileeditor.h"

#include <QFileDialog>
#include <QMessageBox>

#include "config.h"
#include "ticonfmain.h"

vpnProfileEditor::vpnProfileEditor(QWidget *parent, vpnProfileEditorMode smode) :
    QWidget(parent),
    ui(new Ui::vpnProfileEditor),
    mode(smode),
    config(0)
{
    ui->setupUi(this);

    // Validators
    QRegExp rx(openfortigui_config::validatorName);
    QValidator *validatorName = new QRegExpValidator(rx, this);
    ui->leName->setValidator(validatorName);

    // Default settings
    ui->cbSetRoutes->setChecked(true);
    ui->cbSetDNS->setChecked(true);
}

vpnProfileEditor::~vpnProfileEditor()
{
    delete ui;
}

void vpnProfileEditor::loadVpnProfile(const QString &profile, vpnProfile::Origin sourceOrigin)
{
    tiConfVpnProfiles vpns;
    config = vpns.getVpnProfileByName(profile, sourceOrigin);

    ui->leName->setText(config->name);
    ui->leGatewayHost->setText(config->gateway_host);
    ui->sBGatewayPort->setValue(config->gateway_port);
    ui->leUsername->setText(config->username);
    ui->lePassword->setText(config->password);

    if(!config->ca_file.isEmpty() || !config->user_cert.isEmpty() || !config->user_key.isEmpty() || !config->trusted_cert.isEmpty())
    {
        ui->gbCertificate->setChecked(true);

        ui->leCAFile->setText(config->ca_file);
        ui->leUserCert->setText(config->user_cert);
        ui->leUserKey->setText(config->user_key);
        ui->leTrustedCert->setText(config->trusted_cert);
        ui->cbVerifyCert->setChecked(config->verify_cert);
    }

    ui->cbSetRoutes->setChecked(config->set_routes);
    ui->cbSetDNS->setChecked(config->set_dns);
    ui->cbPPPDNoPeerDNS->setChecked(config->pppd_no_peerdns);
    ui->cbInsecureSSL->setChecked(config->insecure_ssl);
    ui->cbDebug->setChecked(config->debug);
    ui->leRealm->setText(config->realm);
    ui->cbAutostart->setChecked(config->autostart);
    ui->cbHalfInternetRoutes->setChecked(config->half_internet_routers);

    if(config->origin_location == vpnProfile::Origin_GLOBAL)
    {
        ui->leName->setDisabled(true);
        ui->leGatewayHost->setDisabled(true);
        ui->sBGatewayPort->setDisabled(true);
        ui->leUsername->setDisabled(true);
        ui->lePassword->setDisabled(true);
        ui->gbCertificate->setDisabled(true);
        ui->leCAFile->setDisabled(true);
        ui->leUserCert->setDisabled(true);
        ui->leUserKey->setDisabled(true);
        ui->leTrustedCert->setDisabled(true);
        ui->cbVerifyCert->setDisabled(true);
        ui->cbSetRoutes->setDisabled(true);
        ui->cbSetDNS->setDisabled(true);
        ui->cbPPPDNoPeerDNS->setDisabled(true);
        ui->cbInsecureSSL->setDisabled(true);
        ui->btnSave->setDisabled(true);
        ui->btnChooseCAFile->setDisabled(true);
        ui->btnChooseUserCert->setDisabled(true);
        ui->btnChooseUserKey->setDisabled(true);
        ui->cbDebug->setDisabled(true);
        ui->leRealm->setDisabled(true);
        ui->cbAutostart->setDisabled(true);
        ui->cbHalfInternetRoutes->setDisabled(true);
    }
}

void vpnProfileEditor::on_btnChooseUserCert_clicked()
{
    QString startDir = (ui->leUserCert->text().isEmpty()) ? QDir::homePath() : ui->leUserCert->text();

    QString dir = QFileDialog::getOpenFileName(this, trUtf8("Select the user-cert"), startDir);

    if(!dir.isEmpty())
        ui->leUserCert->setText(dir);
}

void vpnProfileEditor::on_btnChooseUserKey_clicked()
{
    QString startDir = (ui->leUserKey->text().isEmpty()) ? QDir::homePath() : ui->leUserKey->text();

    QString dir = QFileDialog::getOpenFileName(this, trUtf8("Select the user-key"), startDir);

    if(!dir.isEmpty())
        ui->leUserKey->setText(dir);
}

void vpnProfileEditor::on_btnChooseCAFile_clicked()
{
    QString startDir = (ui->leCAFile->text().isEmpty()) ? QDir::homePath() : ui->leCAFile->text();

    QString dir = QFileDialog::getOpenFileName(this, trUtf8("Select the CA-file"), startDir);

    if(!dir.isEmpty())
        ui->leCAFile->setText(dir);
}

void vpnProfileEditor::on_btnCancel_clicked()
{
    parentWidget()->close();
}

void vpnProfileEditor::on_btnSave_clicked()
{
    if(ui->leName->text().isEmpty())
    {
        QMessageBox::information(this, trUtf8("VPN"), trUtf8("You must set a name for the VPN."));
        return;
    }

    if(ui->leGatewayHost->text().isEmpty())
    {
        QMessageBox::information(this, trUtf8("VPN"), trUtf8("You must set a gateway for the VPN."));
        return;
    }

    if(ui->sBGatewayPort->text().isEmpty())
    {
        QMessageBox::information(this, trUtf8("VPN"), trUtf8("You must set a gateway-port for the VPN."));
        return;
    }

    tiConfVpnProfiles vpns;
    vpnProfile vpn;

    if(mode == vpnProfileEditorModeEdit)
    {
        vpn.name = ui->leName->text();
        if(vpn.name != config->name)
            vpns.renameVpnProfile(config->name, vpn.name);

        if(!ui->gbCertificate->isChecked())
        {
            vpn.ca_file = "";
            vpn.user_cert = "";
            vpn.user_key = "";
            vpn.verify_cert = false;
        }
    }

    vpn.name = ui->leName->text();
    vpn.gateway_host = ui->leGatewayHost->text();
    vpn.gateway_port = ui->sBGatewayPort->text().toInt();
    vpn.username = ui->leUsername->text();
    vpn.password = ui->lePassword->text();

    if(ui->gbCertificate->isChecked())
    {
        vpn.ca_file = ui->leCAFile->text();
        vpn.user_cert = ui->leUserCert->text();
        vpn.user_key = ui->leUserKey->text();
        vpn.trusted_cert = ui->leTrustedCert->text();
        vpn.verify_cert = ui->cbVerifyCert->isChecked();
    }

    vpn.set_routes = ui->cbSetRoutes->isChecked();
    vpn.set_dns = ui->cbSetDNS->isChecked();
    vpn.pppd_no_peerdns = ui->cbPPPDNoPeerDNS->isChecked();
    vpn.insecure_ssl = ui->cbInsecureSSL->isChecked();
    vpn.debug = ui->cbDebug->isChecked();
    vpn.realm = ui->leRealm->text();
    vpn.autostart = ui->cbAutostart->isChecked();
    vpn.half_internet_routers = ui->cbHalfInternetRoutes->isChecked();

    vpns.saveVpnProfile(vpn);

    parentWidget()->close();

    if(mode == vpnProfileEditorModeEdit)
        emit vpnEdited(vpn);
    else if(mode == vpnProfileEditorModeNew)
        emit vpnAdded(vpn);
}
