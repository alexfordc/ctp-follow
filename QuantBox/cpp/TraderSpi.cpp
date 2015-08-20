#include <qt_windows.h>
#include <iostream>
using namespace std;

#include "include\TraderSpi.h"


void CTraderSpi::OnFrontConnected()
{
	qDebug() << "<<<---" << "OnFrontConnected:" << QThread::currentThreadId() <<endl;
	///�û���¼����
	// ReqUserLogin();
	emit frontConnected();
}

void CTraderSpi:: OnFrontDisconnected(int nReason)
{
	qDebug() << "<<<---" << "OnFrontDisconnected" << endl;
	qDebug() << "<<<--- Reason = " << nReason << endl;
	emit frontDisconnected();
}

void CTraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	qDebug() << "<<<---" << "OnRspUserLogin:" << QThread::currentThreadId() <<endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		int iNextOrderRef = atoi(pRspUserLogin->MaxOrderRef);
		iNextOrderRef++;
		
		emit rspUserLogin(pRspUserLogin->FrontID, pRspUserLogin->SessionID, iNextOrderRef);
	}
}

void CTraderSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	qDebug() << "<<<---" << "OnRspSettlementInfoConfirm:" << QThread::currentThreadId() <<endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		emit rspSettlementInfoConfirm();
	}
}

void CTraderSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	qDebug() << "<<<---" << "OnRspOrderInsert:" << QThread::currentThreadId() <<endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		//emit rspOrderInsert();
	}
}

void CTraderSpi::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	qDebug() << "<<<---" << "OnRspOrderAction:" << QThread::currentThreadId() <<endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		//emit rspOrderAciton();
	}
}

///����֪ͨ
void CTraderSpi::OnRtnOrder(CThostFtdcOrderField *pOrder)
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
void CTraderSpi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
	qDebug() << "<<<---" << "OnRtnTrade:"  <<  QThread::currentThreadId() <<endl;
	qDebug() << pTrade->InvestorID << ":" << pTrade->Direction <<":" << pTrade->InstrumentID << ":" << pTrade->Price << endl;
    emit RtnTradeEvent(*pTrade);
}

void CTraderSpi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	qDebug() << "<<<---" << "OnRspQryTradingAccount:"  <<  QThread::currentThreadId() <<endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		// ��̬Ȩ�� = ���ս��� - ������ + �����
		double preBalance = pTradingAccount->PreBalance - pTradingAccount->Withdraw + pTradingAccount->Deposit;
		cout << "preBalance:"<<preBalance<< endl;
		// ��̬Ȩ�� = ��̬Ȩ�� + ƽ��ӯ�� + �ֲ�ӯ�� -������
		double currentBalance = preBalance + pTradingAccount->CloseProfit + pTradingAccount->PositionProfit - pTradingAccount->Commission;	
		cout << "currentBalance" << currentBalance << endl;
		emit rspQryTradingAccount(currentBalance, pTradingAccount->CloseProfit, pTradingAccount->PositionProfit);
	}
}
		
void CTraderSpi::OnHeartBeatWarning(int nTimeLapse)
{
	qDebug() << "<<<---" << "OnHeartBeatWarning" <<  QThread::currentThreadId() <<endl;
	qDebug() << "<<<--- nTimerLapse = " << nTimeLapse << endl;
}

void CTraderSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	qDebug() << "<<<---" << "OnRspError" <<  QThread::currentThreadId() <<endl;
	IsErrorRspInfo(pRspInfo);
}

bool CTraderSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// ���ErrorID != 0, ˵���յ��˴������Ӧ
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
	{
		emit rspErrorInfo();
		qDebug() << "<<<--- ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
	}
	return bResult;
}

bool CTraderSpi::IsTradingOrder(CThostFtdcOrderField *pOrder)
{
	return ((pOrder->OrderStatus != THOST_FTDC_OST_PartTradedNotQueueing) &&
			(pOrder->OrderStatus != THOST_FTDC_OST_Canceled) &&
			(pOrder->OrderStatus != THOST_FTDC_OST_AllTraded));
}


// �������ʱ����

void CTraderSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspQryInstrument" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		//ReqQryTradingAccount();
	}
}

void CTraderSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspQryInvestorPosition" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		///����¼������
		//ReqOrderInsert();
		//ִ������¼������
		//ReqExecOrderInsert();
		//ѯ��¼��
		//ReqForQuoteInsert();
		//�����̱���¼��
		//ReqQuoteInsert();
	}
}

void CTraderSpi::OnRspExecOrderInsert(CThostFtdcInputExecOrderField *pInputExecOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//���ִ��������ȷ���򲻻����ûص�
	cerr << "--->>> " << "OnRspExecOrderInsert" << endl;
	IsErrorRspInfo(pRspInfo);
}

void CTraderSpi::OnRspForQuoteInsert(CThostFtdcInputForQuoteField *pInputForQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//���ѯ����ȷ���򲻻����ûص�
	cerr << "--->>> " << "OnRspForQuoteInsert" << endl;
	IsErrorRspInfo(pRspInfo);
}

void CTraderSpi::OnRspQuoteInsert(CThostFtdcInputQuoteField *pInputQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//���������ȷ���򲻻����ûص�
	cerr << "--->>> " << "OnRspQuoteInsert" << endl;
	IsErrorRspInfo(pRspInfo);
}

void CTraderSpi::OnRspExecOrderAction(CThostFtdcInputExecOrderActionField *pInpuExectOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//��ȷ�ĳ����������������ûص�
	cerr << "--->>> " << "OnRspExecOrderAction" << endl;
	IsErrorRspInfo(pRspInfo);
}

void CTraderSpi::OnRspQuoteAction(CThostFtdcInputQuoteActionField *pInpuQuoteAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//��ȷ�ĳ����������������ûص�
	cerr << "--->>> " << "OnRspQuoteAction" << endl;
	IsErrorRspInfo(pRspInfo);
}


//ִ������֪ͨ
void CTraderSpi::OnRtnExecOrder(CThostFtdcExecOrderField *pExecOrder)
{
	cerr << "--->>> " << "OnRtnExecOrder"  << endl;
	//if (IsMyExecOrder(pExecOrder))
	//{
	//	if (IsTradingExecOrder(pExecOrder))
	//		ReqExecOrderAction(pExecOrder);
	//	else if (pExecOrder->ExecResult == THOST_FTDC_OER_Canceled)
	//		cout << "--->>> ִ�����泷���ɹ�" << endl;
	//}
}

//ѯ��֪ͨ
void CTraderSpi::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp)
{
	//�������н���ѯ��֪ͨͨ���ýӿڷ��أ�ֻ�������̿ͻ������յ���֪ͨ
	cerr << "--->>> " << "OnRtnForQuoteRsp"  << endl;
}

//����֪ͨ
void CTraderSpi::OnRtnQuote(CThostFtdcQuoteField *pQuote)
{
	cerr << "--->>> " << "OnRtnQuote"  << endl;
	//if (IsMyQuote(pQuote))
	//{
	//	if (IsTradingQuote(pQuote))
	//		ReqQuoteAction(pQuote);
	//	else if (pQuote->QuoteStatus == THOST_FTDC_OST_Canceled)
	//		cout << "--->>> ���۳����ɹ�" << endl;
	//}
}

bool CTraderSpi::IsTradingExecOrder(CThostFtdcExecOrderField *pExecOrder)
{
	return (pExecOrder->ExecResult != THOST_FTDC_OER_Canceled);
}

bool CTraderSpi::IsTradingQuote(CThostFtdcQuoteField *pQuote)
{
	return (pQuote->QuoteStatus != THOST_FTDC_OST_Canceled);
}
