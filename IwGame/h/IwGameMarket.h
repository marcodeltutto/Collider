#if !defined(_CIW_GAME_MARKET_H_)
#define _CIW_GAME_MARKET_H_

#include "IwGameString.h"
#include "IwGameUtil.h"

#include "s3eIOSAppStoreBilling.h"
#include "s3eAndroidMarketBilling.h"

//
//
// CProduct - A product that can be purchased
// 
//
struct CIwGameMarketProduct
{
public:
	CIwGameString		Name;		// Name of product
	int					ID;			// Internal product ID
	CIwGameString		ExternalID;	// External product ID (com.companyname.appname.productname for example)
	bool				Purchased;	// Purchased state
	bool				Consumable;	// True if this is a consumable item that can be repurchased when depleted
};

enum eIwGameMarketStatus
{
	MS_IDLE, 
	MS_PURCHASING, 
	MS_PURCHASE_COMPLETED, 
	MS_RESTORE_COMPLETED, 
	MS_ERROR_CLIENT_INVALID, 
	MS_ERROR_PAYMENT_CANCELLED, 
	MS_ERROR_PAYMENT_INVALID, 
	MS_ERROR_PAYMENT_NOT_ALLOWED, 
	MS_ERROR_PURCHASE_UNKNOWN, 
	MS_ERROR_PURCHASE_DISABLED, 
	MS_ERROR_NO_CONNECTION, 
	MS_ERROR_RESTORE_FAILED, 
	MS_ERROR_UNKNOWN_PRODUCT, 
	MS_ERROR_DEVELOPER_ERROR, 
	MS_ERROR_UNAVAILABLE 
};

//
//
//
//
// IIwGameMarket - Common interface that all In-app purchasing classes derive from
//
//
// 
//
class IIwGameMarket
{
public:
	typedef CIwList<CIwGameMarketProduct*>::iterator _ProductIterator;

protected:
	// Properties
	bool					Available;				// Is extension available
	eIwGameMarketStatus		Status;					// Status
	CIwList<CIwGameMarketProduct*>	Products;		// Available products
	CIwGameMarketProduct*	CurrentProduct;			// Current product
public:
	bool					isAvailable() const									{ return Available; }
	void					addProduct(CIwGameMarketProduct* product)			{ Products.push_back(product); }
	CIwGameMarketProduct*	findProduct(int product_id);
	CIwGameMarketProduct*	findProduct(const char* external_id);
	virtual void			setCurrentProduct(CIwGameMarketProduct* product)	{ CurrentProduct = product; }
	CIwGameMarketProduct*	getCurrentProduct()									{ return CurrentProduct; }
	eIwGameMarketStatus		getStatus() const									{ return Status; }
	void					setStatus(eIwGameMarketStatus status)				{ Status = status; NotifyStatusChanged(); }
	void					setPurchased(int product_id)
	{
		CIwGameMarketProduct* product = findProduct(product_id);
		if (product != NULL)
		{
			product->Purchased = true;
			Save();
		}
	}
	void					setLastPurchaseID(int product_id)					{ LastPurchaseID = product_id; Save(); }
	int						getLastPurchaseID() const							{ return LastPurchaseID; }

	// Properties end

protected:
	static IIwGameMarket*	CurrentMarket;
	CIwGameCallback			InfoAvailableHandler;
	CIwGameCallbackData		InfoAvailableHandlerData;
	CIwGameCallback			BuyAvailableHandler;
	CIwGameCallbackData		BuyAvailableHandlerData;
	CIwGameCallback			RestoreAvailableHandler;
	CIwGameCallbackData		RestoreAvailableHandlerData;
	CIwGameCallback			ErrorHandler;
	CIwGameCallbackData		ErrorHandlerData;
	CIwGameCallback			ReceiptAvailableHandler;
	CIwGameCallbackData		ReceiptAvailableHandlerData;
	CIwGameCallback			StatusChangedeHandler;
	CIwGameCallbackData		StatusChangedeHandlerData;
	CIwGameCallback			BillingDisabledeHandler;
	CIwGameCallbackData		BillingDisabledeHandlerData;
	CIwGameCallback			RefundHandler;
	CIwGameCallbackData		RefundHandlerData;
	int						LastPurchaseID;

public:
	void					setInfoAvailableHandler(CIwGameCallback handler, CIwGameCallbackData data)		{ InfoAvailableHandler = handler; InfoAvailableHandlerData = data; }
	void					setBuyAvailableHandler(CIwGameCallback handler, CIwGameCallbackData data)		{ BuyAvailableHandler = handler; BuyAvailableHandlerData = data; }
	void					setRestoreAvailableHandler(CIwGameCallback handler, CIwGameCallbackData data)	{ RestoreAvailableHandler = handler; RestoreAvailableHandlerData = data; }
	void					setReceiptAvailableHandler(CIwGameCallback handler, CIwGameCallbackData data)	{ ReceiptAvailableHandler = handler; ReceiptAvailableHandlerData = data; }
	void					setStatusChangedHandler(CIwGameCallback handler, CIwGameCallbackData data)		{ StatusChangedeHandler = handler; StatusChangedeHandlerData = data; }
	void					setErrorHandler(CIwGameCallback handler, CIwGameCallbackData data)				{ ErrorHandler = handler; ErrorHandlerData = data; }
	void					setBillingDisabledeHandler(CIwGameCallback handler, CIwGameCallbackData data)	{ BillingDisabledeHandler = handler; BillingDisabledeHandlerData = data; }
	void					setRefundHandler(CIwGameCallback handler, CIwGameCallbackData data)				{ RefundHandler = handler; RefundHandlerData = data; }

	// Event notification
	void					NotifyInfoAvailable();
	void					NotifyBuyAvailable();
	void					NotifyRestoreAvailable();
	void					NotifyReceiptAvailable();
	void					NotifyStatusChanged();
	void					NotifyBillingDisabled();
	void					NotifyRefund();
	void					NotifyError();

	void					Save();
	void					Load();

	// Required implementation
	bool					Init();
	void					Release();
	virtual void			Update() = 0;
	virtual bool			QueryProduct(int product_id) = 0;
	virtual bool			PurchaseProduct(int product_id) = 0;

	// Utility
	static bool				Create(void* data = NULL);
	static void				Destroy();
	static IIwGameMarket*	getMarket() { return IIwGameMarket::CurrentMarket; }

};


//
//
//
//
// CIwGameMarketiOS - Handles in-app purchasing for iOS
//
//
// 
//
class CIwGameMarketiOS : public IIwGameMarket
{
	CDEFINE_SINGLETON(CIwGameMarketiOS)

	// Properties
protected:
	void					setCurrentProduct(CIwGameMarketProduct* product)	{ IIwGameMarket::setCurrentProduct(product); CurrentProductID = product->ExternalID.c_str(); }
	s3ePaymentRequest*		getPaymentRequest()									{ return &PaymentRequest; }
	void					setTransactionReceipt(void *receipt_data, uint receipt_size)
	{
		TransactionReceipt.m_ReceiptSize = receipt_size;
		TransactionReceipt.m_ReceiptData = s3eMalloc(receipt_size + 1);
		memcpy(TransactionReceipt.m_ReceiptData, receipt_data, receipt_size);
		PaymentTransaction->m_TransactionReceipt = &TransactionReceipt;
	}
	s3eTransactionReceipt*	getTransactionReceipt()								{ return &TransactionReceipt; }
	void					setProductInformation(s3eProductInformation* info)
	{
		ReleaseProductInfo();
		ProductInformation = (s3eProductInformation*)s3eMalloc(sizeof(s3eProductInformation));
		memcpy(ProductInformation, info, sizeof(s3eProductInformation));
	}
	s3eProductInformation*	getProductInformation()								{ return ProductInformation; }
	void					setPaymentTransaction(s3ePaymentTransaction* trans)
	{
		ReleasePayment();
		PaymentTransaction = (s3ePaymentTransaction*)s3eMalloc(sizeof(s3ePaymentTransaction));
		memcpy(PaymentTransaction, trans, sizeof(s3ePaymentTransaction));
		PaymentTransaction->m_Request = &PaymentRequest;
	}
	s3ePaymentTransaction*	getPaymentTransaction()								{ return PaymentTransaction; }

	// Properties end

protected:
	const char*				CurrentProductID;
	s3ePaymentRequest		PaymentRequest;
	s3eTransactionReceipt	TransactionReceipt;
	s3eProductInformation*	ProductInformation;
	s3ePaymentTransaction*	PaymentTransaction;

public:

	bool				Init();
	void				Release();
	void				Update();

	bool				StartPurchase();
	bool				GetProductInfo();
	void				StopProductinfo();
	void				GetReceipt();
	void				RestoreTransaction();

	bool				QueryProduct(int product_id);
	bool				PurchaseProduct(int product_id);

	// Internal
	static void			TransactionUpdateCallback(s3ePaymentTransaction* transaction, void* userData);
	static void			ProductInfoCallback(s3eProductInformation* productInfo, void* userData);
	void				ReleaseReceipt();
	void				ReleasePayment();
	void				ReleaseProductInfo();

};

#define IW_GAME_MARKET_IOS (CIwGameMarketiOS::getInstance())

//
//
//
//
// CIwGameMarketAndroid - Handles in-app purchasing fro Android
//
//
// 
//
class CIwGameMarketAndroid : public IIwGameMarket
{
	CDEFINE_SINGLETON(CIwGameMarketAndroid)

	// Properties
protected:
	CIwGameString		PublicKey;
	CIwGameString		RefundedID;
public:
	void				setPublicKey(const char* key)	{ PublicKey = key; }
	void				setRefundedID(const char* id)	{ RefundedID = id; }
	CIwGameString&		getRefundedID()					{ return RefundedID; }
	// Properties end

protected:

public:

	bool				Init();
	void				Release();
	void				Update();

	bool				QueryProduct(int product_id);		// Does nothing
	bool				PurchaseProduct(int product_id);

	// Internal
	static int32		AsyncResponse(void* system, void* user);
	static int32		OrderInformationChanged(void* system, void* user);

};

#define IW_GAME_MARKET_ANDROID (CIwGameMarketAndroid::getInstance())
//
//
//
//
// CIwGameMarketTest - A nop class that simply allows testing of in-app purchase without performing any actual purchasing
//
//
// 
//
class CIwGameMarketTest : public IIwGameMarket
{
	CDEFINE_SINGLETON(CIwGameMarketTest)

	// Properties
protected:
	// Properties end

protected:
	CIwGameTimer	PurchaseTimer;

public:

	bool				Init();
	void				Release();
	void				Update();

	bool				QueryProduct(int product_id);
	bool				PurchaseProduct(int product_id);

	void				ForceError(eIwGameMarketStatus status);
};

#define IW_GAME_MARKET_TEST (CIwGameMarketTest::getInstance())

#endif	// _CIW_GAME_MARKET_H_

