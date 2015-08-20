#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include "ui_mainwindow.h"
#include <QtSql>
#include <QStandardItemModel>

#include "include\Trader.h"
#include <vector>
#include <string>
using namespace std;


class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

private:
	Ui::MainWindowClass ui;
	CTrader* m_pMainTrader;
	vector<CTrader*> m_pFollowTrader;

	QSqlDatabase db;
	Account m_mainAccount;
    vector<Account> m_followAccount;

    QStandardItemModel* m_eventModel;

private:
	void setupFollowTable();
	void setupEventTable();

private slots:
	// װ������
	void loadMainAccount();
	void loadFollowAccount();

	void mainTraderLogin();
	void followTraderLogin();
    void connectFollow2Main(int index);

	// �˵�����
	void mainSettingClicked();
	void followSettingClicked();

	// ���ٰ�ť
	void beginFollowBtnClicked();
	void stopFollowBtnClicked();

	// ���½���
	void updateStatus(int index, QString message);
	void updateBalance(int index, double balance, double positionProfit, double closeProfit);
	void insertEventTable(QString message);

signals:
	void traderLogout();
};

#endif // MAINWINDOW_H
