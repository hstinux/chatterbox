// We need to include a couple Qt classes that we'll use:
#include <QMainWindow>
#include <QTcpSocket>
#include <QSystemTrayIcon>
#include <QTimer>
// This is the include file that Qt generates for us from the
// GUI we built in Designer  
#include "ui_MainWindow.h"
#include <QSystemTrayIcon>

/*
 * This is the MainWindow class that we have told to inherit from
 * our Designer MainWindow (ui::MainWindow)
 */
class MainWindow : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT

    public:

        // Every QWidget needs a constructor, and they should allow
        // the user to pass a parent QWidget (or not).
        MainWindow(QWidget *parent=0);
        QString username;
    private slots:

        // This function gets called when a user clicks on the
        // loginButton on the front page (which you placed there
        // with Designer)
        void on_loginButton_clicked();

        // This gets called when you click the sayButton on
        // the chat page.
        void on_sayButton_clicked();

        // This is a function we'll connect to a socket's readyRead()
        // signal, which tells us there's text to be read from the chat
        // server.
        void readyRead();

        // This function gets called when the socket tells us it's connected.
        void connected();
        void trayIconClicked(QSystemTrayIcon::ActivationReason);

        void on_pushButton_Message_clicked();
        void aufwachenslot();
        void timer_handler();
        void on_pushButton_peep_clicked();

private:

        // This is the socket that will let us communitate with the server.
        QTcpSocket *socket;
        QTimer *mytimer;
        QSystemTrayIcon *trayIcon;
        QMenu *trayIconMenu;
        QAction *open;
        QAction *close;
        QAction *aufwachen;
        void closeEvent(QCloseEvent *); // Overriding the window's close event

};
