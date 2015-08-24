#include "include\Trader.h"
#include <QDir>
#include <QtSql>
#include <iostream>
using namespace std;

MainTrader::MainTrader(Account &account)
{
	cout << "CTrader init:"<< QThread::currentThreadId() <<endl;

	strcpy(m_FRONT_ADDR, account.tdFrontAddr);
	strcpy(m_BROKER_ID, account.brokerID);
	strcpy(m_INVESTOR_ID, account.inverstorID);
	strcpy(m_PASSWORD, account.password);
	m_iRequestID = 0;

	QString flowPath = QString::fromLocal8Bit("%1\\%2\\")
        .arg(QCoreApplication::applicationDirPath().toStdString().c_str()).arg(m_INVESTOR_ID);
    QDir dir;
    if(!dir.exists(flowPath))
    {
        dir.mkdir(flowPath);
    }
    m_pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi(flowPath.toStdString().c_str()); // ����UserApi
    m_pTraderSpi = new MainTraderSpi();
    m_followStrategys.clear();
}

MainTrader::~MainTrader()
{
	cout << "CTrader destroy:" << QThread::currentThreadId() << endl;
    emit TraderStatusUpdated(QString::fromLocal8Bit("�˻����˳�"));
    emit TraderBalanceUpdated(0.0, 0.0, 0.0);
    emit EventTableUpdated(QString::fromLocal8Bit("�˺�%1�Ѿ��˳���¼").arg(m_INVESTOR_ID));
    m_pTraderApi->Release();  // һ��Ҫ���ͷŽ���api
    delete m_pTraderSpi;
}

void MainTrader::LoadStrategy()
{
    QSqlDatabase db = QSqlDatabase::database();
    if(!db.open())
    {
        qDebug() << "���Լ���ʧ��" << endl;
    }
	QSqlQuery query;
    Strategy strategy;
    m_orderStrategys.clear();
	query.exec("select id, follow_type, action_sec, price_type, price_jump, \
               action_times, final_type from strategy order by id");
    while (query.next())
    {
        strategy.id = query.value(0).toInt();
        strategy.followType = query.value(1).toInt();
        strategy.actionSec = query.value(2).toInt();
        strategy.priceType = query.value(3).toInt();
        strategy.priceJump = query.value(4).toInt();
        strategy.actionTimes = query.value(5).toInt();
        strategy.finalType = query.value(6).toInt();
        m_orderStrategys.push_back(strategy);
    }
    db.close();
}

void MainTrader::SetThread(QThread * mainThread)
{
	m_pThread = mainThread;
	this->moveToThread(mainThread);
}

void MainTrader::AddFollowStrategy(const char* investorID, FollowStrategy &followStrategy)
{
    m_followStrategys[investorID] = followStrategy;
}

bool MainTrader::IsFlowControl(int iResult)
{
	return ((iResult == -2) || (iResult == -3));
}

void MainTrader::ReqConnect()
{
	cout << "user Login:"<< QThread::currentThreadId() <<endl;

    QObject::connect(m_pTraderSpi, &MainTraderSpi::FrontConnected, this, &MainTrader::ReqUserLogin);
	QObject::connect(m_pTraderSpi, &MainTraderSpi::UserLogined, this, &MainTrader::RspUserLogin);
	QObject::connect(m_pTraderSpi, &MainTraderSpi::SettlementInfoConfirmed, this, &MainTrader::ReqQryTradingAccount);
    QObject::connect(m_pTraderSpi, &MainTraderSpi::TraderBalanceUpdated, this, &MainTrader::RspQryTradingAccount);
    QObject::connect(m_pTraderSpi, &MainTraderSpi::RtnTradeEvent, this, &MainTrader::ProcessTradeEvent);

	m_pTraderApi->RegisterSpi((CThostFtdcTraderSpi*)m_pTraderSpi);		// ע���¼���
	m_pTraderApi->SubscribePublicTopic(THOST_TERT_QUICK);				// ע�ṫ����
	m_pTraderApi->SubscribePrivateTopic(THOST_TERT_QUICK);				// ע��˽����
	m_pTraderApi->RegisterFront(m_FRONT_ADDR);							// connect
	m_pTraderApi->Init();
	// m_pTraderApi->Join();
	emit TraderStatusUpdated(QString::fromLocal8Bit("�����ӽ��׷�����"));
}

void MainTrader::ReqDisconnect()
{
    m_pThread->quit();
}

void MainTrader::ReqUserLogin()
{
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, m_BROKER_ID);
	strcpy(req.UserID, m_INVESTOR_ID);
	strcpy(req.Password, m_PASSWORD);
	int iResult = m_pTraderApi->ReqUserLogin(&req, ++m_iRequestID);
	if(iResult==0)
	{
		cout << "--->>> �����û���¼����: �ɹ�" << QThread::currentThreadId() << endl;
		emit TraderStatusUpdated(QString::fromLocal8Bit("�ɹ����͵�¼����"));
	}
	else
	{
        cout << "--->>> �����û���¼����: ʧ��" << QThread::currentThreadId() << endl;
        emit TraderStatusUpdated(QString::fromLocal8Bit("���͵�¼����ʧ��"));
	}
}

void MainTrader::RspUserLogin(int front_id ,int session_id, int iNextOrderRef)
{
	m_FRONT_ID = front_id;
	m_SESSION_ID = session_id;
    m_iNextOrderRef = iNextOrderRef;
	//��ȡ��ǰ������
	cout << "--->>> ��ȡ��ǰ������ = " << m_pTraderApi->GetTradingDay() << endl;
	///Ͷ���߽�����ȷ��
	ReqSettlementInfoConfirm();
    emit TraderLogined();
	emit TraderStatusUpdated(QString::fromLocal8Bit("��¼�ɹ�"));
}

void MainTrader::ReqSettlementInfoConfirm()
{
	CThostFtdcSettlementInfoConfirmField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, m_BROKER_ID);
	strcpy(req.InvestorID, m_INVESTOR_ID);
	int iResult = m_pTraderApi->ReqSettlementInfoConfirm(&req, ++m_iRequestID);
	cout << "--->>> Ͷ���߽�����ȷ��: " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl;
}

void MainTrader::ReqQryTradingAccount()
{
	CThostFtdcQryTradingAccountField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, m_BROKER_ID);
	strcpy(req.InvestorID, m_INVESTOR_ID);
	while (true)
	{
		int iResult = m_pTraderApi->ReqQryTradingAccount(&req, ++m_iRequestID);
		if (!IsFlowControl(iResult))
		{
			cout << "--->>> �����ѯ�ʽ��˻�: "  << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl;
			break;
		}
		else
		{
			cout << "--->>> �����ѯ�ʽ��˻�: "  << iResult << ", �ܵ�����" << endl;
			QThread::sleep(1);
		}
	} // while
}

void MainTrader::RspQryTradingAccount(double balance, double positionProfit, double closeProfit)
{
	cout << "--->>> �Ѿ���ѯ���û��ֲ�"<< balance << QThread::currentThreadId() << endl;
	emit TraderBalanceUpdated(balance, positionProfit, closeProfit);
}

void MainTrader::ReqOrderInsert(TThostFtdcInstrumentIDType INSTRUMENT_ID, TThostFtdcDirectionType DIRECTION, 
                             TThostFtdcVolumeType VOLUME, TThostFtdcPriceType LIMIT_PRICE, 
                             TThostFtdcOffsetFlagType OFFSET_FLAG)
{
	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, m_BROKER_ID);        //���͹�˾����
	strcpy(req.InvestorID, m_INVESTOR_ID);    //Ͷ���ߴ���
	strcpy(req.InstrumentID, INSTRUMENT_ID);  //��Լ����
    sprintf(m_ORDER_REF, "%012d", m_iNextOrderRef++);
    strcpy(req.OrderRef, m_ORDER_REF);        //��������
	req.Direction = DIRECTION;            //�������� 
	req.CombOffsetFlag[0] = OFFSET_FLAG;  //��Ͽ�ƽ��־
	req.LimitPrice = LIMIT_PRICE;         //�۸�
	req.VolumeTotalOriginal = VOLUME;     //����
	req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;     //�����۸�����: �޼�
	req.TimeCondition = THOST_FTDC_TC_GFD;              //��Ч������: ������Ч
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;   //���Ͷ���ױ���־
	req.ContingentCondition = THOST_FTDC_CC_Immediately;  //��������: ����
	req.VolumeCondition = THOST_FTDC_VC_AV;               //�ɽ�������: �κ�����
	req.MinVolume = 0;                                    //��С�ɽ���: 0
	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;  //ǿƽԭ��: ��ǿƽ
	req.IsAutoSuspend = 0;                                //�Զ������־: ��
	req.UserForceClose = 0;                               //�û�ǿ����־: ��
    req.StopPrice = 0;        //ֹ���
    req.GTDDate[0] = '\0';    //GTD����
    strcpy(req.UserID, m_INVESTOR_ID);
	///ҵ��Ԫ//	TThostFtdcBusinessUnitType	BusinessUnit;
	///������//	TThostFtdcRequestIDType	RequestID;

	int iResult = m_pTraderApi->ReqOrderInsert(&req, ++m_iRequestID);
	cerr << "--->>> ����¼������: " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl;
}

void MainTrader::ReqOrderAction(CThostFtdcOrderField *pOrder)
{
	static bool ORDER_ACTION_SENT = false;		//�Ƿ����˱���
	if (ORDER_ACTION_SENT)
		return;

	CThostFtdcInputOrderActionField req;
	memset(&req, 0, sizeof(req));
	///���͹�˾����
	strcpy(req.BrokerID, pOrder->BrokerID);
	///Ͷ���ߴ���
	strcpy(req.InvestorID, pOrder->InvestorID);
	///������������
//	TThostFtdcOrderActionRefType	OrderActionRef;
	///��������
	strcpy(req.OrderRef, pOrder->OrderRef);
	///������
//	TThostFtdcRequestIDType	RequestID;
	///ǰ�ñ��
	req.FrontID = m_FRONT_ID;
	///�Ự���
	req.SessionID = m_SESSION_ID;
	///����������
//	TThostFtdcExchangeIDType	ExchangeID;
	///�������
//	TThostFtdcOrderSysIDType	OrderSysID;
	///������־
	req.ActionFlag = THOST_FTDC_AF_Delete;
	///�۸�
//	TThostFtdcPriceType	LimitPrice;
	///�����仯
//	TThostFtdcVolumeType	VolumeChange;
	///�û�����
//	TThostFtdcUserIDType	UserID;
	///��Լ����
	strcpy(req.InstrumentID, pOrder->InstrumentID);

	int iResult = m_pTraderApi->ReqOrderAction(&req, ++m_iRequestID);
	cerr << "--->>> ������������: "  << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl;
	ORDER_ACTION_SENT = true;
}

bool MainTrader::IsMyOrder(CThostFtdcOrderField *pOrder)
{
	return ((pOrder->FrontID == m_FRONT_ID) &&
			(pOrder->SessionID == m_SESSION_ID) &&
			(strcmp(pOrder->OrderRef, m_ORDER_REF) == 0));
}

void MainTrader::RspTradingAciton()
{
}

void MainTrader::FollowRtnTrade(CThostFtdcTradeField tradeField)
{
    FollowStrategy strategy = m_followStrategys[tradeField.InvestorID];
    TThostFtdcVolumeType volume = int(tradeField.Volume*strategy.ratio);
    TThostFtdcDirectionType direct = tradeField.Direction;
    if (strategy.direct == 1) // �������
    {
        direct = tradeField.Direction==THOST_FTDC_D_Buy?THOST_FTDC_D_Sell:THOST_FTDC_D_Buy;
    }
    ReqOrderInsert(tradeField.InstrumentID, direct, volume, tradeField.Price, tradeField.OffsetFlag);
    emit EventTableUpdated(QString::fromLocal8Bit("���˺Ÿ�������Լ%1,����:%2,����:%3,�۸�:%4")
        .arg(tradeField.InstrumentID).arg(direct).arg(volume).arg(tradeField.Price));
}

void MainTrader::ProcessTradeEvent(CThostFtdcTradeField tradeField)
{
    emit EventTableUpdated(QString::fromLocal8Bit("���˺ųɽ�����Լ%1,����:%2,����:%3")
        .arg(tradeField.InstrumentID).arg(tradeField.Direction).arg(tradeField.Volume));
}
