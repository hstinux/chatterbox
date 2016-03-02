/*
 * Historie:
 *
 * rebase gemerged
 * checkout item001
 * branch item001 angelegt
 * Github pull request
 * commit
*/
#include "MainWindow.h"
#include <QMessageBox>
// We'll need some regular expression magic in this code:
#include <QRegExp>

#include <QMenu>
#include <QCloseEvent>
#include <QTimer>
#include <QDateTime>
#include <QRadioButton>

#define PORT_TDSC                  33000
#define PORT_DIRK                  33001

#define PORT                       PORT_DIRK

// This is our MainWindow constructor (you C++ n00b)
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    // When using Designer, you should always call setupUi(this)
    // in your constructor. This creates and lays out all the widgets
    // on the MainWindow that you setup in Designer.
    setupUi(this);
    this->setVisible(false);
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/trash.png"));
    trayIcon->show();

    QTimer *mytimer = new QTimer(this);
    connect(mytimer, SIGNAL(timeout()), this, SLOT(timer_handler()));
    mytimer->start(10000);

    open = new QAction(tr("&Open"), this);
    connect(open, SIGNAL(triggered()), this, SLOT(show()));


    close = new QAction(tr("&Quit"), this);
    connect(close, SIGNAL(triggered()), qApp, SLOT(quit()));

    aufwachen = new QAction(tr("&Aufwachen"), this);
    connect(aufwachen, SIGNAL(triggered()), this, SLOT(aufwachenslot()));

    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(aufwachen);
    trayIconMenu->addAction(open);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(close);
    trayIcon->setContextMenu(trayIconMenu);
    connect(    trayIcon,
                SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                this,
                SLOT(trayIconClicked(QSystemTrayIcon::ActivationReason))
               );
    // Make sure that we are showing the login page when we startup:
    stackedWidget->setCurrentWidget(loginPage);

    // Instantiate our socket (but don't actually connect to anything
    // yet until the user clicks the loginButton:
    socket = new QTcpSocket(this);

    // This is how we tell Qt to call our readyRead() and connected()
    // functions when the socket has text ready to be read, and is done
    // connecting to the server (respectively):
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));


}

// This gets called when the loginButton gets clicked:
// We didn't have to use connect() to set this up because
// Qt recognizes the name of this function and knows to set
// up the signal/slot connection for us.
void MainWindow::on_loginButton_clicked()
{
    // Start connecting to the chat server (on port 4200).
    // This returns immediately and then works on connecting
    // to the server in the background. When it's done, we'll
    // get a connected() function call (below). If it fails,
    // we won't get any error message because we didn't connect()
    // to the error() signal from this socket.
    if (userLineEdit->text().isEmpty() )
    {
        QMessageBox   msg;
        msg.setText("Geben Sie einen Nickname ein");
        msg.exec();
    }
    else
    {
        username =  userLineEdit->text();
        if (radioButton_DMRT->isChecked()){
           socket->connectToHost("localhost", PORT_DIRK);
           titleLabel->setText("Chatterbox mit Dirk");
        }
        if (radioButton_TDSC->isChecked()){
           socket->connectToHost("localhost", PORT_TDSC);
           titleLabel->setText("Chatterbox mit Schmiddi");
        }

    }


}

// This gets called when the user clicks the sayButton (next to where
// they type text to send to the chat room):
void MainWindow::on_sayButton_clicked()
{
    // What did they want to say (minus white space around the string):
    QString message = sayLineEdit->text().trimmed();

    // Only send the text to the chat server if it's not empty:
    if(!message.isEmpty())
    {
        socket->write(QString(message + "\n").toUtf8());
    }

    // Clear out the input box so they can type something else:
    sayLineEdit->clear();

    // Put the focus back into the input box so they can type again:
    sayLineEdit->setFocus();
}

// This function gets called whenever the chat server has sent us some text:
void MainWindow::readyRead()
{
    // We'll loop over every (complete) line of text that the server has sent us:
    while(socket->canReadLine())
    {
        // Here's the line the of text the server sent us (we use UTF-8 so
        // that non-English speakers can chat in their native language)
        QString line = QString::fromUtf8(socket->readLine()).trimmed();

        // These two regular expressions describe the kinds of messages
        // the server can send us:

        //  Normal messges look like this: "username:The message"
        QRegExp messageRegex("^([^:]+):(.*)$");

        // Any message that starts with "/users:" is the server sending us a
        // list of users so we can show that list in our GUI:
        QRegExp usersRegex("^/users:(.*)$");

        // Is this a users message:
        if(usersRegex.indexIn(line) != -1)
        {
            // If so, udpate our users list on the right:
            QStringList users = usersRegex.cap(1).split(",");
            userListWidget->clear();
            foreach(QString user, users)
                new QListWidgetItem(QPixmap(":/user.png"), user, userListWidget);
        }
        // Is this a normal chat message:
        else if(messageRegex.indexIn(line) != -1)
        {
            // If so, append this message to our chat box:
            QString user = messageRegex.cap(1);
            QString message = messageRegex.cap(2);

            roomTextEdit->append("["+QDateTime::currentDateTime().toString("hh:mm:ss")+"] " + "<b>" + user + "</b>: "+  message);
        }
        QRegExp peepRegex("^/peep:(.*)$");
        if(line.contains("peep") == true)
        {
            // If so, udpate our users list on the right:
//            QMessageBox  msg;
//            msg.setText("Peep");
//            msg.exec();

            QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(1);
            trayIcon->showMessage("Erinnerung","Peep", icon,300000);
        }
    }
}

// This function gets called when our socket has successfully connected to the chat
// server. (see the connect() call in the MainWindow constructor).
void MainWindow::connected()
{
    // Flip over to the chat page:
    stackedWidget->setCurrentWidget(chatPage);

    // And send our username to the chat server.
    socket->write(QString("/me:" + userLineEdit->text() + "\n").toUtf8());
}


void MainWindow::trayIconClicked(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::Trigger)
        this->show();
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    /*
    if (trayIcon->isVisible()) {
        trayIcon->showMessage(tr("Still here!!!"),
        tr("This application is still running. To quit please click this icon and select Quit"));
        hide();
        event->ignore(); // Don't let the event propagate to the base class
    }
    */

}


void MainWindow::on_pushButton_Message_clicked()
{
    QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(1);
    trayIcon->showMessage("Erinnerung","Peep", icon,300000);
}

void MainWindow::aufwachenslot()
{
    socket->write("peep\n");
}

void MainWindow::timer_handler()
{
    switch(socket->state() )
    {
       case QAbstractSocket::ConnectedState:
            checkBox_connected->setChecked(true);
            break;
       case QAbstractSocket::UnconnectedState:
            checkBox_connected->setChecked(false);
            socket->connectToHost("localhost", PORT);
            break;

    }


}

void MainWindow::on_pushButton_peep_clicked()
{
    socket->write("peep\n");
}
