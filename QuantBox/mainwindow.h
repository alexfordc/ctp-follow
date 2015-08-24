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
	MainTrader* m_pMainTrader;
	vector<FollowTraderSpi*> m_pFollowTrader;

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
	void FollowTraderLogin();
    void connectFollow2Main(int index);

	// �˵�����
	void mainSettingClicked();
	void followSettingClicked();
    void brokerManageClicked();
    void strategySetttingClicked();

	// ���ٰ�ť
	void beginFollowBtnClicked();
	void stopFollowBtnClicked();

    // ���½���
    void UpdateMainStatus(QString message);
    void UpdateMainBalance(double balance, double positionProfit, double closeProfit);
	void UpdateFollowStatus(int index, QString message);
	void UpdateFollowBalance(int index, double balance, double positionProfit, double closeProfit);
	void InsertEventTable(QString message);

signals:
	void TraderLogout();
};

#endif // MAINWINDOW_H
