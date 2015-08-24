#include <qt_windows.h>
#include "include\TraderSpi.h"


void MainTraderSpi::OnFrontConnected()
{
	qDebug() << "<<<---" << "OnFrontConnected:" << QThread::currentThreadId() <<endl;
	emit FrontConnected();
}

void MainTraderSpi:: OnFrontDisconnected(int nReason)
{
	qDebug() << "<<<---" << "OnFrontDisconnected: Reason = " << nReason << endl;
}

void MainTraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	qDebug() << "<<<---" << "OnRspUserLogin:" << QThread::currentThreadId() <<endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		int iNextOrderRef = atoi(pRspUserLogin->MaxOrderRef);
		iNextOrderRef++;
		emit UserLogined(pRspUserLogin->FrontID, pRspUserLogin->SessionID, iNextOrderRef);
	}
}

void MainTraderSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, 
                                               CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	qDebug() << "<<<---" << "OnRspSettlementInfoConfirm:" << QThread::currentThreadId() <<endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		emit SettlementInfoConfirmed();
	}
}

void MainTraderSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, 
                                     CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	qDebug() << "<<<---" << "OnRspOrderInsert:" << QThread::currentThreadId() <<endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		//emit rspOrderInsert();
	}
}

void MainTraderSpi::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, 
                                     CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	qDebug() << "<<<---" << "OnRspOrderAction:" << QThread::currentThreadId() <<endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		//emit rspOrderAciton();
	}
}

///����֪ͨ
void MainTraderSpi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	qDebug() << "<<<---" << "OnRtnOrder:"  <<  QThread::currentThreadId() <<endl;
	/*if (IsMyOrder(pOrder))
	{
		if (IsTradingOrder(pOrder))
			ReqOrderAction(pOrder);
		else if (pOrder->OrderStatus == THOST_FTDC_OST_Canceled)
			cout << "--->>> �����ɹ�" << endl;
	}*/
}

///�ɽ�֪ͨ
void MainTraderSpi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
	qDebug() << "<<<---" << "OnRtnTrade:"  <<  QThread::currentThreadId() <<endl;
	qDebug() << pTrade->InvestorID << ":" << pTrade->Direction <<":" 
        << pTrade->InstrumentID << ":" << pTrade->Price << endl;
    emit RtnTradeEvent(*pTrade);
}

void MainTraderSpi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, 
                                           CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	qDebug() << "<<<---" << "OnRspQryTradingAccount:"  <<  QThread::currentThreadId() <<endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		// ��̬Ȩ�� = ���ս��� - ������ + �����
		double preBalance = pTradingAccount->PreBalance - pTradingAccount->Withdraw + pTradingAccount->Deposit;
		// ��̬Ȩ�� = ��̬Ȩ�� + ƽ��ӯ�� + �ֲ�ӯ�� -������
		double currentBalance = preBalance + pTradingAccount->CloseProfit + pTradingAccount->PositionProfit - pTradingAccount->Commission;	
		emit TraderBalanceUpdated(currentBalance, pTradingAccount->CloseProfit, pTradingAccount->PositionProfit);
	}
}
		
void MainTraderSpi::OnHeartBeatWarning(int nTimeLapse)
{
	qDebug() << "<<<---" << "OnHeartBeatWarning" <<  QThread::currentThreadId() <<endl;
	qDebug() << "<<<--- nTimerLapse = " << nTimeLapse << endl;
}

void MainTraderSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	qDebug() << "<<<---" << "OnRspError" <<  QThread::currentThreadId() <<endl;
	IsErrorRspInfo(pRspInfo);
}

bool MainTraderSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// ���ErrorID != 0, ˵���յ��˴������Ӧ
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
	{
		// emit RspErrorInfo();
		qDebug() << "<<<--- ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
	}
	return bResult;
}

bool MainTraderSpi::IsTradingOrder(CThostFtdcOrderField *pOrder)
{
	return ((pOrder->OrderStatus != THOST_FTDC_OST_PartTradedNotQueueing) &&
			(pOrder->OrderStatus != THOST_FTDC_OST_Canceled) &&
			(pOrder->OrderStatus != THOST_FTDC_OST_AllTraded));
}


//====================== FollowTraderSpi =====================
FollowTraderSpi::FollowTraderSpi()
{
}

FollowTraderSpi::~FollowTraderSpi()
{
    qDebug() << "FollowTraderSpi destroy:" << QThread::currentThreadId() << endl;
    emit TraderStatusUpdated(m_index, QString::fromLocal8Bit("�˻����˳�"));
    emit TraderBalanceUpdated(m_index, 0.0, 0.0, 0.0);
    emit EventTableUpdated(QString::fromLocal8Bit("�˺�%1�Ѿ��˳���¼").arg(m_INVESTOR_ID));
    m_pTraderApi->Release();  // һ��Ҫ���ͷŽ���api
}

void FollowTraderSpi::Init(int index, const char* frontAddr, const char* brokerID, 
                           const char* investorID, const char* password)
{
    m_index = index;
    strcpy(m_FRONT_ADDR, frontAddr);
    strcpy(m_BROKER_ID, brokerID);
    strcpy(m_INVESTOR_ID, investorID);
    strcpy(m_PASSWORD, password);
    m_iRequestID = 0;
}

void FollowTraderSpi::ReqConnect()
{
    qDebug() << "<<<---" << "ReqConnect:"<< QThread::currentThreadId() <<endl;

    QString flowPath = QString::fromLocal8Bit("%1\\%2\\")
        .arg(QCoreApplication::applicationDirPath().toStdString().c_str()).arg(m_INVESTOR_ID);
    QDir dir;
    if(!dir.exists(flowPath))
    {
        dir.mkdir(flowPath);
    }
    m_pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi(flowPath.toStdString().c_str());  // ����UserApi

    m_pTraderApi->RegisterSpi((CThostFtdcTraderSpi*)this);  	// ע���¼���
    m_pTraderApi->SubscribePublicTopic(THOST_TERT_QUICK);		// ע�ṫ����
    m_pTraderApi->SubscribePrivateTopic(THOST_TERT_QUICK);		// ע��˽����
    m_pTraderApi->RegisterFront(m_FRONT_ADDR);					// connect
    m_pTraderApi->Init();
    emit TraderStatusUpdated(m_index, QString::fromLocal8Bit("�����ӽ��׷�����"));
}

void FollowTraderSpi::OnFrontConnected()
{
	qDebug() << "<<<---" << "OnFrontConnected:" << QThread::currentThreadId() <<endl;
	///�û���¼����
	ReqUserLogin();
}

void FollowTraderSpi:: OnFrontDisconnected(int nReason)
{
	qDebug() << "<<<---" << "OnFrontDisconnected:" << "Reason = " << nReason << endl;
}

void FollowTraderSpi::ReqUserLogin()
{
    CThostFtdcReqUserLoginField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, m_BROKER_ID);
    strcpy(req.UserID, m_INVESTOR_ID);
    strcpy(req.Password, m_PASSWORD);
    int iResult = m_pTraderApi->ReqUserLogin(&req, ++m_iRequestID);
    if(iResult==0)
    {
        qDebug() << "<<<--- �����û���¼����: �ɹ�" << QThread::currentThreadId() << endl;
        emit TraderStatusUpdated(m_index, QString::fromLocal8Bit("�ɹ����͵�¼����"));
    }
    else
    {
        qDebug() << "<<<--- �����û���¼����: ʧ��" << QThread::currentThreadId() << endl;
        emit TraderStatusUpdated(m_index, QString::fromLocal8Bit("���͵�¼����ʧ��"));
    }
}

void FollowTraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	qDebug() << "<<<---" << "OnRspUserLogin:" << QThread::currentThreadId() <<endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		int iNextOrderRef = atoi(pRspUserLogin->MaxOrderRef);
		iNextOrderRef++;

        m_FRONT_ID = pRspUserLogin->FrontID;
        m_SESSION_ID = pRspUserLogin->SessionID;
        m_iNextOrderRef = iNextOrderRef;
        //��ȡ��ǰ������
        qDebug() << "<<<--- ��ȡ��ǰ������ = " << m_pTraderApi->GetTradingDay() << endl;
        ///Ͷ���߽�����ȷ��
        ReqSettlementInfoConfirm();
        emit TraderLogined(m_index);
        emit TraderStatusUpdated(m_index, QString::fromLocal8Bit("��¼�ɹ�"));
	}
}

void FollowTraderSpi::ReqSettlementInfoConfirm()
{
    CThostFtdcSettlementInfoConfirmField req;
    memset(&req, 0, sizeof(req));
    strcpy(req.BrokerID, m_BROKER_ID);
    strcpy(req.InvestorID, m_INVESTOR_ID);
    int iResult = m_pTraderApi->ReqSettlementInfoConfirm(&req, ++m_iRequestID);
    qDebug() << "<<<--- Ͷ���߽�����ȷ��: " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl;
}

void FollowTraderSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, 
                                                 CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	qDebug() << "<<<---" << "OnRspSettlementInfoConfirm:" << QThread::currentThreadId() <<endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		ReqQryTradingAccount();
	}
}

void FollowTraderSpi::ReqQryTradingAccount()
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
            qDebug() << "--->>> �����ѯ�ʽ��˻�: "  << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl;
            break;
        }
        else
        {
            qDebug() << "--->>> �����ѯ�ʽ��˻�: "  << iResult << ", �ܵ�����" << endl;
            QThread::sleep(1);
        }
    } // while
}

void FollowTraderSpi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, 
                                             CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    qDebug() << "<<<---" << "OnRspQryTradingAccount:"  <<  QThread::currentThreadId() <<endl;
    if (bIsLast && !IsErrorRspInfo(pRspInfo))
    {
        // ��̬Ȩ�� = ���ս��� - ������ + �����
        double preBalance = pTradingAccount->PreBalance - pTradingAccount->Withdraw + pTradingAccount->Deposit;
        // ��̬Ȩ�� = ��̬Ȩ�� + ƽ��ӯ�� + �ֲ�ӯ�� -������
        double currentBalance = preBalance + pTradingAccount->CloseProfit + pTradingAccount->PositionProfit - pTradingAccount->Commission;	
        emit TraderBalanceUpdated(m_index, currentBalance, pTradingAccount->CloseProfit, pTradingAccount->PositionProfit);
    }
}

///����֪ͨ
void FollowTraderSpi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	qDebug() << "<<<---" << "OnRtnOrder:"  <<  QThread::currentThreadId() <<endl;
	/*if (IsMyOrder(pOrder))
	{
		if (IsTradingOrder(pOrder))
			ReqOrderAction(pOrder);
		else if (pOrder->OrderStatus == THOST_FTDC_OST_Canceled)
			cout << "--->>> �����ɹ�" << endl;
	}*/
}

///�ɽ�֪ͨ
void FollowTraderSpi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
	qDebug() << "<<<---" << "OnRtnTrade:"  <<  QThread::currentThreadId() <<endl;
	qDebug() << pTrade->InvestorID << ":" << pTrade->Direction <<":" << pTrade->InstrumentID << ":" << pTrade->Price << endl;
    emit RtnTradeEvent(*pTrade);
}
		
void FollowTraderSpi::OnHeartBeatWarning(int nTimeLapse)
{
	qDebug() << "<<<---" << "OnHeartBeatWarning" <<  QThread::currentThreadId() <<endl;
	qDebug() << "<<<--- nTimerLapse = " << nTimeLapse << endl;
}

void FollowTraderSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	qDebug() << "<<<---" << "OnRspError" <<  QThread::currentThreadId() <<endl;
	IsErrorRspInfo(pRspInfo);
}

bool FollowTraderSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// ���ErrorID != 0, ˵���յ��˴������Ӧ
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
	{
		emit RspErrorInfo();
		qDebug() << "<<<--- ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
	}
	return bResult;
}

bool FollowTraderSpi::IsTradingOrder(CThostFtdcOrderField *pOrder)
{
	return ((pOrder->OrderStatus != THOST_FTDC_OST_PartTradedNotQueueing) &&
			(pOrder->OrderStatus != THOST_FTDC_OST_Canceled) &&
			(pOrder->OrderStatus != THOST_FTDC_OST_AllTraded));
}

bool FollowTraderSpi::IsFlowControl(int iResult)
{
    return ((iResult == -2) || (iResult == -3));
}
