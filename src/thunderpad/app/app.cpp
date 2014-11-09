//
//  This file is part of Thunderpad
//
//  Copyright (c) 2013-2014 Alex Spataru <alex.racotta@gmail.com>
//  Please check the license.txt file for more information.
//

#include "app.h"

Application::Application (int &argc, char **argv) :
    QApplication (argc, argv)
  , m_show_all_updater_messages (false)
{
    setApplicationName (APP_NAME);
    setOrganizationName (APP_COMPANY);
    setApplicationVersion (APP_VERSION);

    m_window = new Window();
    m_updater = new QSimpleUpdater();
    m_settings = new QSettings (APP_COMPANY, APP_NAME);

    if (argc != 1)
    {
        QString _args;

        for (int i = 0; i < argc; i++) _args = argv[i];

        m_window->openFile (_args);
    }

    connect (m_window, SIGNAL (checkForUpdates()),
             this,     SLOT   (checkForUpdates()));
    connect (m_updater, SIGNAL (checkingFinished()),
             this,      SLOT   (onCheckingFinished()));

    QString _download_url;
    QString _url_base = "https://raw.githubusercontent.com/"
                        "alex-97/thunderpad/updater/";

    if (MAC_OS_X)
        _download_url = _url_base + "files/thunderpad-latest.dmg";

    else if (WINDOWS)
        _download_url = _url_base + "files/thunderpad-latest.exe";

    else if (LINUX)
        _download_url = _url_base + "files/thunderpad-latest.tar.gz";

    m_updater->setDownloadUrl (_download_url);
    m_updater->setApplicationVersion (APP_VERSION);
    m_updater->setReferenceUrl (_url_base + "latest.txt");
    m_updater->setChangelogUrl (_url_base + "changelog.txt");

    if (m_settings->value ("first-launch", true).toBool())
    {
        QMessageBox _message;
        _message.setParent(m_window);
        _message.setWindowModality(Qt::WindowModal);
        _message.setWindowTitle (tr ("Thunderpad"));
        _message.setWindowIcon (QIcon (":/icons/dummy.png"));
        _message.setIconPixmap (QPixmap (":/icons/logo.png"));

        _message.setStandardButtons (QMessageBox::Close);
        _message.setText (tr ("Thank you for downloading Thunderpad!") + "                       ");
        _message.setInformativeText (tr ("If you find this program useful and would like to help "
                                         "contribute to future development, please consider "
                                         "a small donation. You can  use the Donate item in the "
                                         "Help menu to send your much needed assistance via BitCoins.\n\n"
                                         "Please share Thunderpad with your friends and colleagues, "
                                         "and feel free to send me feedback!"));
        _message.exec();

        _message.setDefaultButton (QMessageBox::Yes);
        _message.setStandardButtons (QMessageBox::Yes | QMessageBox::No);
        _message.setText (tr ("Do you want to check for updates automatically?"));
        _message.setInformativeText (tr ("You can always check for updates from the "
                                         "Help menu"));

        if (_message.exec() == QMessageBox::Yes)
            m_settings->setValue ("check-for-updates", true);

        else
            m_settings->setValue ("check-for-updates", false);

        m_settings->setValue ("first-launch", false);
    }

    if (m_settings->value ("check-for-updates", true).toBool())
        m_updater->checkForUpdates();
}

int Application::showInitError()
{
    QMessageBox::warning (NULL, tr ("Application error"),
                          tr ("There's already a running instance of %1")
                          .arg (applicationName()));

    return -1;
}

void Application::checkForUpdates()
{
    m_updater->checkForUpdates();
    m_show_all_updater_messages = true;
}

void Application::showLatestVersion()
{
    QMessageBox _message;
    _message.setStandardButtons (QMessageBox::Ok);
    _message.setWindowIcon (QIcon (":/icons/dummy.png"));
    _message.setIconPixmap (QPixmap (":/icons/logo.png"));
    _message.setWindowTitle (tr ("No updates available"));
    _message.setInformativeText (
                tr ("The latest release of Thunderpad is version %1")
                .arg (qApp->applicationVersion()));
    _message.setText ("<b>" + tr ("Congratulations! You are running the latest "
                                  "version of Thunderpad!") +
                      "</b>");

    _message.exec();
}

void Application::showUpdateAvailable()
{
    QMessageBox _message;
    _message.setDetailedText (m_updater->changeLog());
    _message.setWindowIcon (QIcon (":/icons/dummy.png"));
    _message.setIconPixmap (QPixmap (":/icons/logo.png"));
    _message.setWindowTitle (tr ("New updates available"));
    _message.setStandardButtons (QMessageBox::Yes | QMessageBox::No);
    _message.setText ("<b>" + tr ("There's a new version of Thunderpad!") +
                      " (" + m_updater->latestVersion() + ")</b>");
    _message.setInformativeText (
                tr ("Do you want to download the newest version?"));

    if (_message.exec() == QMessageBox::Yes)
        m_updater->downloadLatestVersion();
}

void Application::onCheckingFinished()
{
    if (m_updater->newerVersionAvailable())
        showUpdateAvailable();

    else if (m_show_all_updater_messages)
    {
        showLatestVersion();
        m_show_all_updater_messages = false;
    }
}

bool Application::event (QEvent *_event)
{
    if (_event->type() == QEvent::FileOpen)
        m_window->openFile (static_cast<QFileOpenEvent *> (_event)->file());

    else
        return QApplication::event (_event);

    return true;
}
