#include "include\Trader.h"
#include <QDir>
#include <iostream>
using namespace std;

CTrader::CTrader(int index, Account account)
{
	cout << "CTrader init:"<< QThread::currentThreadId() <<endl;

	strcpy(m_FRONT_ADDR, account.td_front_addr);
	strcpy(m_BROKER_ID, account.broker_id);
	strcpy(m_INVESTOR_ID, account.inverst_id);
	strcpy(m_PASSWORD, account.password);
	m_iRequestID = 0;
	m_index = index;
    m_followDirect = account.direct;
    m_followRatio = account.ratio;

	sprintf(m_flowPath, "%s\\%s\\", QCoreApplication::applicationDirPath().toStdString().c_str(), m_INVESTOR_ID);
    QDir dir;
    if(!dir.exists(m_flowPath))
    {
        dir.mkdir(QString::fromLocal8Bit(m_flowPath));
    }
    m_pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi(m_flowPath);			// ����UserApi
    m_pTraderSpi = new CTraderSpi();
}

CTrader::~CTrader()
{
	cout << "CTrader destroy:" << QThread::currentThreadId() << endl;
    m_pTraderApi->Release();  // һ��Ҫ��ʼ������api
    delete m_pTraderSpi;
}

void CTrader::SetThread(QThread * thread)
{
	m_pThread = thread;
	this->moveToThread(thread);
}

bool CTrader::IsFlowControl(int iResult)
{
	return ((iResult == -2) || (iResult == -3));
}

void CTrader::ReqConnect()
{
	cout << "user Login:"<< QThread::currentThreadId() <<endl;

	QObject::connect(m_pTraderSpi, &CTraderSpi::frontConnected, this, &CTrader::ReqUserLogin);
	QObject::connect(m_pTraderSpi, &CTraderSpi::rspUserLogin, this, &CTrader::rspUserLogin);
	QObject::connect(m_pTraderSpi, &CTraderSpi::rspSettlementInfoConfirm, this, &CTrader::ReqQryTradingAccount);
    QObject::connect(m_pTraderSpi, &CTraderSpi::rspQryTradingAccount, this, &CTrader::rspQryTradingAccount);
    QObject::connect(m_pTraderSpi, &CTraderSpi::RtnTradeEvent, this, &CTrader::ProcessTradeEvent);

	m_pTraderApi->RegisterSpi((CThostFtdcTraderSpi*)m_pTraderSpi);		// ע���¼���
	m_pTraderApi->SubscribePublicTopic(THOST_TERT_QUICK);				// ע�ṫ����
	m_pTraderApi->SubscribePrivateTopic(THOST_TERT_QUICK);				// ע��˽����
	m_pTraderApi->RegisterFront(m_FRONT_ADDR);							// connect
	cout << "front_addr" << m_FRONT_ADDR <<endl;
	m_pTraderApi->Init();
	// m_pTraderApi->Join();
	emit traderStatusUpdated(m_index, QString::fromLocal8Bit("�����ӽ��׷�����"));
}

void CTrader::ReqLogout()
{
    emit traderStatusUpdated(m_index, QString::fromLocal8Bit("�˻����˳�"));
    emit traderBalanceUpdated(m_index, 0.0, 0.0, 0.0);
    emit eventTableUpdated(QString::fromLocal8Bit("�˺�%1�Ѿ��˳���¼").arg(m_INVESTOR_ID));
    m_pThread->quit();
}

void CTrader::ReqUserLogin()
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
		emit traderStatusUpdated(m_index, QString::fromLocal8Bit("�ɹ����͵�¼����"));
	}
	else
	{
        cout << "--->>> �����û���¼����: ʧ��" << QThread::currentThreadId() << endl;
        emit traderStatusUpdated(m_index, QString::fromLocal8Bit("���͵�¼����ʧ��"));
	}
}

void CTrader::rspUserLogin(int front_id ,int session_id, int iNextOrderRef)
{
	m_FRONT_ID = front_id;
	m_SESSION_ID = session_id;
    m_iNextOrderRef = iNextOrderRef;
	sprintf(m_EXECORDER_REF, "%d", 1);
	sprintf(m_FORQUOTE_REF, "%d", 1);
	sprintf(m_QUOTE_REF, "%d", 1);
	//��ȡ��ǰ������
	cout << "--->>> ��ȡ��ǰ������ = " << m_pTraderApi->GetTradingDay() << endl;
	///Ͷ���߽�����ȷ��
	ReqSettlementInfoConfirm();
    emit traderLogined(m_index);
	emit traderStatusUpdated(m_index, QString::fromLocal8Bit("��¼�ɹ�"));
}

void CTrader::ReqSettlementInfoConfirm()
{
	CThostFtdcSettlementInfoConfirmField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, m_BROKER_ID);
	strcpy(req.InvestorID, m_INVESTOR_ID);
	int iResult = m_pTraderApi->ReqSettlementInfoConfirm(&req, ++m_iRequestID);
	cout << "--->>> Ͷ���߽�����ȷ��: " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl;
}

void CTrader::ReqQryTradingAccount()
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

void CTrader::rspQryTradingAccount(double balance, double positionProfit, double closeProfit)
{
	cout << "--->>> �Ѿ���ѯ���û��ֲ�"<< balance << QThread::currentThreadId() << endl;
	emit traderBalanceUpdated(m_index, balance, positionProfit, closeProfit);
}

void CTrader::ReqOrderInsert(TThostFtdcInstrumentIDType INSTRUMENT_ID, TThostFtdcDirectionType DIRECTION, 
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

void CTrader::ReqOrderAction(CThostFtdcOrderField *pOrder)
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

bool CTrader::IsMyOrder(CThostFtdcOrderField *pOrder)
{
	return ((pOrder->FrontID == m_FRONT_ID) &&
			(pOrder->SessionID == m_SESSION_ID) &&
			(strcmp(pOrder->OrderRef, m_ORDER_REF) == 0));
}

void CTrader::rspTradingAciton()
{
}

void CTrader::FollowRtnTrade(CThostFtdcTradeField tradeField)
{
    TThostFtdcVolumeType volume = int(tradeField.Volume*2.0);
    TThostFtdcDirectionType direct = tradeField.Direction;
    if (m_followDirect == 1) // �������
    {
        direct = tradeField.Direction==THOST_FTDC_D_Buy?THOST_FTDC_D_Sell:THOST_FTDC_D_Buy;
    }
    ReqOrderInsert(tradeField.InstrumentID, direct, volume, tradeField.Price, tradeField.OffsetFlag);
    emit eventTableUpdated(QString::fromLocal8Bit("���˺Ÿ�������Լ%1,����:%2,����:%3,�۸�:%4")
        .arg(tradeField.InstrumentID).arg(direct).arg(volume).arg(tradeField.Price));
}

void CTrader::ProcessTradeEvent(CThostFtdcTradeField tradeField)
{
    if(m_index == -1) // ���˺ųɽ�
    {
        emit eventTableUpdated(QString::fromLocal8Bit("���˺ųɽ�����Լ%1,����:%2,����:%3")
            .arg(tradeField.InstrumentID).arg(tradeField.Direction).arg(tradeField.Volume));
    }
    else
    {
        emit RtnTradeEvent(tradeField);
    }
}


// ����Ĳ���
void CTrader::ReqQryInstrument(TThostFtdcInstrumentIDType instrument_id)
{
	CThostFtdcQryInstrumentField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.InstrumentID, instrument_id);
	while (true)
	{
		int iResult = m_pTraderApi->ReqQryInstrument(&req, ++m_iRequestID);
		if (!IsFlowControl(iResult))
		{
			cout << "--->>> �����ѯ��Լ: "  << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl;
			break;
		}
		else
		{
			cout << "--->>> �����ѯ��Լ: "  << iResult << ", �ܵ�����" << endl;
			QThread::sleep(1);
		}
	} // while
}

void CTrader::ReqQryInvestorPosition(TThostFtdcInstrumentIDType instrument_id)
{
	CThostFtdcQryInvestorPositionField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, m_BROKER_ID);
	strcpy(req.InvestorID, m_INVESTOR_ID);
	strcpy(req.InstrumentID, instrument_id);
	while (true)
	{
		int iResult = m_pTraderApi->ReqQryInvestorPosition(&req, ++m_iRequestID);
		if (!IsFlowControl(iResult))
		{
			cerr << "--->>> �����ѯͶ���ֲ߳�: "  << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl;
			break;
		}
		else
		{
			cerr << "--->>> �����ѯͶ���ֲ߳�: "  << iResult << ", �ܵ�����" << endl;
			QThread::sleep(1);
		}
	} // while
}

//ִ������¼������
void CTrader::ReqExecOrderInsert(TThostFtdcInstrumentIDType INSTRUMENT_ID)
{
	CThostFtdcInputExecOrderField req;
	memset(&req, 0, sizeof(req));
	///���͹�˾����
	strcpy(req.BrokerID, m_BROKER_ID);
	///Ͷ���ߴ���
	strcpy(req.InvestorID, m_INVESTOR_ID);
	///��Լ����
	strcpy(req.InstrumentID, INSTRUMENT_ID);
	///��������
	strcpy(req.ExecOrderRef, m_EXECORDER_REF);
	///�û�����
	//	TThostFtdcUserIDType	UserID;
	///����
	req.Volume=1;
	///������
	//TThostFtdcRequestIDType	RequestID;
	///ҵ��Ԫ
	//TThostFtdcBusinessUnitType	BusinessUnit;
	///��ƽ��־
	req.OffsetFlag=THOST_FTDC_OF_Close;//���������������Ҫ��ƽ���ƽ��
	///Ͷ���ױ���־
	req.HedgeFlag=THOST_FTDC_HF_Speculation;
	///ִ������
	req.ActionType=THOST_FTDC_ACTP_Exec;//�������ִ������THOST_FTDC_ACTP_Abandon
	///����ͷ������ĳֲַ���
	req.PosiDirection=THOST_FTDC_PD_Long;
	///��Ȩ��Ȩ���Ƿ����ڻ�ͷ��ı��
	req.ReservePositionFlag=THOST_FTDC_EOPF_UnReserve;//�����н��������������֣������THOST_FTDC_EOPF_Reserve
	///��Ȩ��Ȩ�����ɵ�ͷ���Ƿ��Զ�ƽ��
	req.CloseFlag=THOST_FTDC_EOCF_AutoClose;//�����н��������������֣������THOST_FTDC_EOCF_NotToClose

	int iResult = m_pTraderApi->ReqExecOrderInsert(&req, ++m_iRequestID);
	cerr << "--->>> ִ������¼������: " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl;
}

//ѯ��¼������
void CTrader::ReqForQuoteInsert(TThostFtdcInstrumentIDType INSTRUMENT_ID)
{
	CThostFtdcInputForQuoteField req;
	memset(&req, 0, sizeof(req));
	///���͹�˾����
	strcpy(req.BrokerID, m_BROKER_ID);
	///Ͷ���ߴ���
	strcpy(req.InvestorID, m_INVESTOR_ID);
	///��Լ����
	strcpy(req.InstrumentID, INSTRUMENT_ID);
	///��������
	strcpy(req.ForQuoteRef, m_EXECORDER_REF);
	///�û�����
	//	TThostFtdcUserIDType	UserID;

	int iResult = m_pTraderApi->ReqForQuoteInsert(&req, ++m_iRequestID);
	cerr << "--->>> ѯ��¼������: " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl;
}
//����¼������
void CTrader::ReqQuoteInsert(TThostFtdcInstrumentIDType INSTRUMENT_ID, TThostFtdcPriceType LIMIT_PRICE)
{
	CThostFtdcInputQuoteField req;
	memset(&req, 0, sizeof(req));
	///���͹�˾����
	strcpy(req.BrokerID, m_BROKER_ID);
	///Ͷ���ߴ���
	strcpy(req.InvestorID, m_INVESTOR_ID);
	///��Լ����
	strcpy(req.InstrumentID, INSTRUMENT_ID);
	///��������
	strcpy(req.QuoteRef, m_QUOTE_REF);
	///���۸�
	req.AskPrice=LIMIT_PRICE;
	///��۸�
	req.BidPrice=LIMIT_PRICE-1.0;
	///������
	req.AskVolume=1;
	///������
	req.BidVolume=1;
	///������
	//TThostFtdcRequestIDType	RequestID;
	///ҵ��Ԫ
	//TThostFtdcBusinessUnitType	BusinessUnit;
	///����ƽ��־
	req.AskOffsetFlag=THOST_FTDC_OF_Open;
	///��ƽ��־
	req.BidOffsetFlag=THOST_FTDC_OF_Open;
	///��Ͷ���ױ���־
	req.AskHedgeFlag=THOST_FTDC_HF_Speculation;
	///��Ͷ���ױ���־
	req.BidHedgeFlag=THOST_FTDC_HF_Speculation;
	
	int iResult = m_pTraderApi->ReqQuoteInsert(&req, ++m_iRequestID);
	cerr << "--->>> ����¼������: " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl;
}

void CTrader::ReqExecOrderAction(CThostFtdcExecOrderField *pExecOrder)
{
	static bool EXECORDER_ACTION_SENT = false;		//�Ƿ����˱���
	if (EXECORDER_ACTION_SENT)
		return;

	CThostFtdcInputExecOrderActionField req;
	memset(&req, 0, sizeof(req));

	///���͹�˾����
	strcpy(req.BrokerID,pExecOrder->BrokerID);
	///Ͷ���ߴ���
	strcpy(req.InvestorID,pExecOrder->InvestorID);
	///ִ�������������
	//TThostFtdcOrderActionRefType	ExecOrderActionRef;
	///ִ����������
	strcpy(req.ExecOrderRef,pExecOrder->ExecOrderRef);
	///������
	//TThostFtdcRequestIDType	RequestID;
	///ǰ�ñ��
	req.FrontID=m_FRONT_ID;
	///�Ự���
	req.SessionID=m_SESSION_ID;
	///����������
	//TThostFtdcExchangeIDType	ExchangeID;
	///ִ������������
	//TThostFtdcExecOrderSysIDType	ExecOrderSysID;
	///������־
	req.ActionFlag=THOST_FTDC_AF_Delete;
	///�û�����
	//TThostFtdcUserIDType	UserID;
	///��Լ����
	strcpy(req.InstrumentID,pExecOrder->InstrumentID);

	int iResult = m_pTraderApi->ReqExecOrderAction(&req, ++m_iRequestID);
	cerr << "--->>> ִ�������������: "  << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl;
	EXECORDER_ACTION_SENT = true;
}

void CTrader::ReqQuoteAction(CThostFtdcQuoteField *pQuote)
{
	static bool QUOTE_ACTION_SENT = false;		//�Ƿ����˱���
	if (QUOTE_ACTION_SENT)
		return;

	CThostFtdcInputQuoteActionField req;
	memset(&req, 0, sizeof(req));
	///���͹�˾����
	strcpy(req.BrokerID, pQuote->BrokerID);
	///Ͷ���ߴ���
	strcpy(req.InvestorID, pQuote->InvestorID);
	///���۲�������
	//TThostFtdcOrderActionRefType	QuoteActionRef;
	///��������
	strcpy(req.QuoteRef,pQuote->QuoteRef);
	///������
	//TThostFtdcRequestIDType	RequestID;
	///ǰ�ñ��
	req.FrontID=m_FRONT_ID;
	///�Ự���
	req.SessionID=m_SESSION_ID;
	///����������
	//TThostFtdcExchangeIDType	ExchangeID;
	///���۲������
	//TThostFtdcOrderSysIDType	QuoteSysID;
	///������־
	req.ActionFlag=THOST_FTDC_AF_Delete;
	///�û�����
	//TThostFtdcUserIDType	UserID;
	///��Լ����
	strcpy(req.InstrumentID,pQuote->InstrumentID);

	int iResult = m_pTraderApi->ReqQuoteAction(&req, ++m_iRequestID);
	cerr << "--->>> ���۲�������: "  << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl;
	QUOTE_ACTION_SENT = true;
}

bool CTrader::IsMyExecOrder(CThostFtdcExecOrderField *pExecOrder)
{
	return ((pExecOrder->FrontID == m_FRONT_ID) &&
		(pExecOrder->SessionID == m_SESSION_ID) &&
		(strcmp(pExecOrder->ExecOrderRef, m_EXECORDER_REF) == 0));
}

bool CTrader::IsMyQuote(CThostFtdcQuoteField *pQuote)
{
	return ((pQuote->FrontID == m_FRONT_ID) &&
		(pQuote->SessionID == m_SESSION_ID) &&
		(strcmp(pQuote->QuoteRef, m_QUOTE_REF) == 0));
}