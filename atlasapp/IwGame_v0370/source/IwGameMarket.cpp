#include "IwGameMarket.h"
#include "IwGameFile.h"


//
//
//
// IIwGameMarket implementation
//
//
//
IIwGameMarket*	IIwGameMarket::CurrentMarket = NULL;
bool IIwGameMarket::Init()
{
	Available = false;
	Status = MS_IDLE;

	CurrentProduct = NULL;
	InfoAvailableHandler = NULL;
	BuyAvailableHandler = NULL;
	RestoreAvailableHandler = NULL;
	ReceiptAvailableHandler = NULL;
	StatusChangedeHandler = NULL;
	ErrorHandler = NULL;
	BillingDisabledeHandler = NULL;
	RefundHandler = NULL;
	LastPurchaseID = -1;

	Load();

	return true;
}

void IIwGameMarket::Release()
{
	if (!Available)
		return;

	for (_ProductIterator it = Products.begin(); it != Products.end(); ++it)
	{
		delete *it;
	}
	Products.clear();

	Available = false;
}

CIwGameMarketProduct* IIwGameMarket::findProduct(int product_id)
{
	for (_ProductIterator it = Products.begin(); it != Products.end(); ++it)
	{
		if (product_id == (*it)->ID)
			return *it;
	}

	return NULL;
}

CIwGameMarketProduct* IIwGameMarket::findProduct(const char* external_id)
{
	unsigned int id_hash = CIwGameString::CalculateHash(external_id);
	for (_ProductIterator it = Products.begin(); it != Products.end(); ++it)
	{
		if (id_hash == (*it)->ExternalID.getHash())
			return *it;
	}

	return NULL;
}

void IIwGameMarket::Save()
{
	CIwGameFile file;
	if (file.Open("\\lastpurchase.dat", "wb"))
	{
		file.Write((void*)&LastPurchaseID, sizeof(LastPurchaseID));
	}

}

void IIwGameMarket::Load()
{
	CIwGameFile file;
	if (file.Open("\\lastpurchase.dat", "rb"))
	{
		file.Read((void*)&LastPurchaseID, sizeof(LastPurchaseID));
	}
}

void IIwGameMarket::NotifyInfoAvailable()
{
	if (InfoAvailableHandler != NULL)
		InfoAvailableHandler(this, InfoAvailableHandlerData);
}
void IIwGameMarket::NotifyBuyAvailable()
{
	if (BuyAvailableHandler != NULL)
		BuyAvailableHandler(this, BuyAvailableHandlerData);
}
void IIwGameMarket::NotifyRestoreAvailable()
{
	if (RestoreAvailableHandler != NULL)
		RestoreAvailableHandler(this, RestoreAvailableHandlerData);
}
void IIwGameMarket::NotifyReceiptAvailable()
{
	if (ReceiptAvailableHandler != NULL)
		ReceiptAvailableHandler(this, ReceiptAvailableHandlerData);
	setLastPurchaseID(-1);
}
void IIwGameMarket::NotifyStatusChanged()
{
	if (StatusChangedeHandler != NULL)
		StatusChangedeHandler(this, StatusChangedeHandlerData);
}
void IIwGameMarket::NotifyError()
{
	if (ErrorHandler != NULL)
	{
		ErrorHandler(this, ErrorHandlerData);
	}
}
void IIwGameMarket::NotifyBillingDisabled()
{
	if (BillingDisabledeHandler != NULL)
		BillingDisabledeHandler(this, BillingDisabledeHandlerData);
}
void IIwGameMarket::NotifyRefund()
{
	if (RefundHandler != NULL)
		RefundHandler(this, RefundHandlerData);
}

bool IIwGameMarket::Create(void* data)
{
	int os = s3eDeviceGetInt(S3E_DEVICE_OS);

	IIwGameMarket::CurrentMarket = NULL;

	switch (os)
	{
		case S3E_OS_ID_IPHONE:
			CIwGameMarketiOS::Create();
			if (!IW_GAME_MARKET_IOS->Init())
				return false;
			IIwGameMarket::CurrentMarket = IW_GAME_MARKET_IOS;
			break;
		case S3E_OS_ID_ANDROID:
			CIwGameMarketAndroid::Create();
			IW_GAME_MARKET_ANDROID->setPublicKey((const char*)data);
			if (!IW_GAME_MARKET_ANDROID->Init())
				return false;
			IIwGameMarket::CurrentMarket = IW_GAME_MARKET_ANDROID;
			break;
		case S3E_OS_ID_WINDOWS:
			CIwGameMarketTest::Create();
			if (!IW_GAME_MARKET_TEST->Init())
				return false;
			IIwGameMarket::CurrentMarket = IW_GAME_MARKET_TEST;
			break;
	}

	return true;
}

void IIwGameMarket::Destroy()
{
	int os = s3eDeviceGetInt(S3E_DEVICE_OS);

	switch (os)
	{
		case S3E_OS_ID_IPHONE:
			if (IW_GAME_MARKET_IOS != NULL)
			{
				IW_GAME_MARKET_IOS->Release();
				CIwGameMarketiOS::Destroy();
			}
			break;
		case S3E_OS_ID_ANDROID:
			break;
		case S3E_OS_ID_WINDOWS:
			if (IW_GAME_MARKET_TEST != NULL)
			{
				IW_GAME_MARKET_TEST->Release();
				CIwGameMarketTest::Destroy();
			}
			break;
	}
}

//
//
//
// CIwGameMarketiOS implementation
//
//
//
CDECLARE_SINGLETON(CIwGameMarketiOS)

//void TransactionUpdateCallback(s3ePaymentTransaction* transaction, void* userData);
//void ProductInfoCallback(s3eProductInformation* productInfo, void* userData);


bool CIwGameMarketiOS::PurchaseProduct(int product_id)
{
	// Find the product
	CIwGameMarketProduct* product = findProduct(product_id);
	if (product == NULL)
	{
		return false;
	}

	// If product is not a consumable and is already purchased then we do not attempt to purchase again
	if (product->Purchased && !product->Consumable)
		return false;

	// Begin the purchasing process
	setCurrentProduct(product);
	StartPurchase();

	return true;
}

bool CIwGameMarketiOS::QueryProduct(int product_id)
{
	// Find the product
	CIwGameMarketProduct* product = findProduct(product_id);
	if (product == NULL)
	{
		return false;
	}

	// Begin product info retrieval
	setCurrentProduct(product);
	GetProductInfo();

	return true;
}

bool CIwGameMarketiOS::Init()
{
	IIwGameMarket::Init();

	if (s3eIOSAppStoreBillingAvailable())
		Available = true;
	else
		return false;

	s3eIOSAppStoreBillingInit(ProductInfoCallback, TransactionUpdateCallback, NULL);

	TransactionReceipt.m_ReceiptData = NULL;
	ProductInformation = NULL;
	PaymentTransaction = NULL;

	return true;
}

void CIwGameMarketiOS::Release()
{
	IIwGameMarket::Release();

	ReleaseProductInfo();
	ReleaseReceipt();
	ReleasePayment();

	s3eIOSAppStoreBillingTerminate();
}

void CIwGameMarketiOS::ReleaseReceipt()
{
	if (TransactionReceipt.m_ReceiptData != NULL)
	{
		s3eFree(TransactionReceipt.m_ReceiptData);
		TransactionReceipt.m_ReceiptData = NULL;
	}
}

void CIwGameMarketiOS::ReleasePayment()
{
	if (PaymentTransaction != NULL)
	{
		s3eFree(PaymentTransaction);
		PaymentTransaction = NULL;
	}
}

void CIwGameMarketiOS::ReleaseProductInfo()
{
	if (ProductInformation != NULL)
	{
		s3eFree(ProductInformation);
		ProductInformation = NULL;
	}
}

void CIwGameMarketiOS::Update()
{
}

//
// TransactionUpdateCallback
//
// When a request ismade to make a transaction with s3eIOSAppStoreBillingRequestPayment() this callback is called when the status of the 
// transaction changes or an error occurs
//
void CIwGameMarketiOS::TransactionUpdateCallback(s3ePaymentTransaction* transaction, void* userData)
{
    s3eDebugTracePrintf("!TransactionUpdateCallback");

CIwGameError::LogError("transaction->m_TransactionStatus - ", CIwGameString((int)transaction->m_TransactionStatus).c_str());

    switch (transaction->m_TransactionStatus)
    {
		case S3E_PAYMENT_STATUS_PENDING:
			s3eDebugTracePrintf("Buying %s (transaction '%s') in progress...", transaction->m_Request->m_ProductID, transaction->m_TransactionID);
			IW_GAME_MARKET_IOS->setStatus(MS_PURCHASING);
            break;
        case S3E_PAYMENT_STATUS_PURCHASED:
        case S3E_PAYMENT_STATUS_RESTORED:
			IW_GAME_MARKET_IOS->ReleaseReceipt();
			IW_GAME_MARKET_IOS->ReleasePayment();

            s3eDebugTracePrintf("S3E_PAYMENT_STATUS_PURCHASED");
            s3eDebugTracePrintf("PaymentRequest.m_ProductID %s", IW_GAME_MARKET_IOS->getPaymentRequest()->m_ProductID);
            s3eDebugTracePrintf("PaymentTransaction->m_Request->m_ProductID %s", transaction->m_Request->m_ProductID);

            // Check product ID of completed transaction
            if (!strcmp(transaction->m_Request->m_ProductID, IW_GAME_MARKET_IOS->getPaymentRequest()->m_ProductID))
            {
CIwGameError::LogError("Product ID's match");
                s3eDebugTracePrintf("Product ID's match");

				if (transaction->m_TransactionStatus == S3E_PAYMENT_STATUS_PURCHASED)
				{
                    s3eDebugTracePrintf("%s purchase completed!", IW_GAME_MARKET_IOS->getCurrentProduct()->Name.c_str());
					IW_GAME_MARKET_IOS->setStatus(MS_PURCHASE_COMPLETED);
CIwGameError::LogError("Purchased");
				}
                else
				{
                    s3eDebugTracePrintf("Restoring %s completed!", IW_GAME_MARKET_IOS->getCurrentProduct()->Name.c_str());
					IW_GAME_MARKET_IOS->setStatus(MS_RESTORE_COMPLETED);
CIwGameError::LogError("Restored");
				}

                // Copy transaction to local memory
				IW_GAME_MARKET_IOS->setPaymentTransaction(transaction);

                // Copy receipt data to memory
				IW_GAME_MARKET_IOS->setTransactionReceipt(transaction->m_TransactionReceipt->m_ReceiptData, transaction->m_TransactionReceipt->m_ReceiptSize);
				IW_GAME_MARKET_IOS->NotifyReceiptAvailable();

				// Complete the transaction (if you wish to validate the receipt data then you need to remove this next line and call it after validation)
				s3eIOSAppStoreBillingCompleteTransaction(transaction, S3E_TRUE);

				// Mark product as purchased
				IW_GAME_MARKET_IOS->setPurchased(IW_GAME_MARKET_IOS->getCurrentProduct()->ID);

                break;
            }
            else
            {
                // We enter here if a transaction was started in a previous session
                if (transaction->m_TransactionStatus == S3E_PAYMENT_STATUS_PURCHASED)
				{
                    s3eDebugTracePrintf("Buying completed for unexpected product ID: %s!", transaction->m_Request->m_ProductID);
					IW_GAME_MARKET_IOS->setStatus(MS_PURCHASE_COMPLETED);
CIwGameError::LogError("Purchased 2");
				}
                else
				{
                    s3eDebugTracePrintf("Restoring completed for unexpected product ID: %s!", transaction->m_Request->m_ProductID);
					IW_GAME_MARKET_IOS->setStatus(MS_RESTORE_COMPLETED);
CIwGameError::LogError("Restored 2");
				}

				// Mark product as purchased
				IW_GAME_MARKET_IOS->setPurchased(IW_GAME_MARKET_IOS->getLastPurchaseID());
			
				IW_GAME_MARKET_IOS->NotifyReceiptAvailable();
			}
			break;

        case S3E_PAYMENT_STATUS_FAILED_CLIENT_INVALID:
            s3eDebugTracePrintf("Buying %s FAILED. Client is not allowed to make the payment.", IW_GAME_MARKET_IOS->getCurrentProduct()->Name.c_str());
			IW_GAME_MARKET_IOS->setStatus(MS_ERROR_CLIENT_INVALID);
			IW_GAME_MARKET_IOS->NotifyError();
            break;

        case S3E_PAYMENT_STATUS_FAILED_PAYMENT_CANCELLED:
            s3eDebugTracePrintf("Buying %s FAILED. User cancelled the purchase.", IW_GAME_MARKET_IOS->getCurrentProduct()->Name.c_str());
			IW_GAME_MARKET_IOS->setStatus(MS_ERROR_PAYMENT_CANCELLED);
			IW_GAME_MARKET_IOS->NotifyError();
            break;

        case S3E_PAYMENT_STATUS_FAILED_PAYMENT_INVALID:
            s3eDebugTracePrintf("Buying %s FAILED. Invalid parameter/purchase ID.", IW_GAME_MARKET_IOS->getCurrentProduct()->Name.c_str());
			IW_GAME_MARKET_IOS->setStatus(MS_ERROR_PAYMENT_INVALID);
			IW_GAME_MARKET_IOS->NotifyError();
            break;

        case S3E_PAYMENT_STATUS_FAILED_PAYMENT_NOT_ALLOWED:
            s3eDebugTracePrintf("Buying %s FAILED. Device is not allowed to make the payment (check restriction settings).", IW_GAME_MARKET_IOS->getCurrentProduct()->Name.c_str());
			IW_GAME_MARKET_IOS->setStatus(MS_ERROR_PAYMENT_NOT_ALLOWED);
			IW_GAME_MARKET_IOS->NotifyError();
            break;

        default:
            s3eDebugTracePrintf("Buying %s FAILED for unknown reason...", IW_GAME_MARKET_IOS->getCurrentProduct()->Name.c_str());
			IW_GAME_MARKET_IOS->setStatus(MS_ERROR_PURCHASE_UNKNOWN);
			IW_GAME_MARKET_IOS->NotifyError();
            break;
    }
}

//
//
//
// TransactionUpdateCallback
//
// When a request is made to retrieve product information using s3eIOSAppStoreBillingRequestProductInformation() this callback is called when the product 
// informatin is available ot an error occurs
//
//
//
void CIwGameMarketiOS::ProductInfoCallback(s3eProductInformation* productInfo, void* userData)
{
    s3eDebugTracePrintf("!ProductInfoCallback %p", productInfo);

	IW_GAME_MARKET_IOS->setProductInformation(productInfo);

    s3eDebugTracePrintf("m_ProductID=%s, m_LocalisedTitle=%s", IW_GAME_MARKET_IOS->getProductInformation()->m_ProductID, IW_GAME_MARKET_IOS->getProductInformation()->m_LocalisedTitle);

    switch (IW_GAME_MARKET_IOS->getProductInformation()->m_ProductStoreStatus)
    {
        case S3E_PRODUCT_STORE_STATUS_VALID:
        {
            if (s3eIOSAppStoreBillingGetInt(S3E_IOSAPPSTOREBILLING_CAN_MAKE_PAYMENTS))
			{
                s3eDebugTracePrintf("%s\nPrice: %.2f", IW_GAME_MARKET_IOS->getProductInformation()->m_LocalisedDescription, (float)IW_GAME_MARKET_IOS->getProductInformation()->m_Price / 100.0f);
			}
            else
			{
                s3eDebugTracePrintf("%s\nPrice: %.2f - Cannot buy - Purchasing is disabled in device's Settings menu!", IW_GAME_MARKET_IOS->getProductInformation()->m_LocalisedDescription, (float)IW_GAME_MARKET_IOS->getProductInformation()->m_Price / 100.0f);
				IW_GAME_MARKET_IOS->setStatus(MS_ERROR_PURCHASE_DISABLED);
				IW_GAME_MARKET_IOS->NotifyBillingDisabled();
				IW_GAME_MARKET_IOS->NotifyError();
			}
	
			IW_GAME_MARKET_IOS->NotifyInfoAvailable();

            if (s3eIOSAppStoreBillingGetInt(S3E_IOSAPPSTOREBILLING_CAN_MAKE_PAYMENTS))
            {
                s3eDebugTracePrintf("Buy '%s", IW_GAME_MARKET_IOS->getProductInformation()->m_LocalisedTitle);
				IW_GAME_MARKET_IOS->NotifyBuyAvailable();
            }
            break;
        }
        case S3E_PRODUCT_STORE_STATUS_NO_CONNECTION:
            s3eDebugTracePrintf("Could not connect to store! Ongoing requests have been cancelled. Please check connection.");
			IW_GAME_MARKET_IOS->setStatus(MS_ERROR_NO_CONNECTION);
			IW_GAME_MARKET_IOS->NotifyError();
            break;
        case S3E_PRODUCT_STORE_STATUS_RESTORE_FAILED:
            s3eDebugTracePrintf("Restore products failed! Please try again.");
			IW_GAME_MARKET_IOS->setStatus(MS_ERROR_RESTORE_FAILED);
			IW_GAME_MARKET_IOS->NotifyError();
            break;
        case S3E_PRODUCT_STORE_STATUS_RESTORE_COMPLETED:
            break;
        default: // Item not found
            s3eDebugTracePrintf("Product not found in store: %s!", IW_GAME_MARKET_IOS->getCurrentProduct()->Name.c_str());
			IW_GAME_MARKET_IOS->setStatus(MS_ERROR_UNKNOWN_PRODUCT);
			IW_GAME_MARKET_IOS->NotifyError();
    }
}

void CIwGameMarketiOS::StartPurchase()
{
	// Setup product request
	strcpy(PaymentRequest.m_ProductID, getCurrentProduct()->ExternalID.c_str());
    PaymentRequest.m_Quantity = 1;
	setLastPurchaseID(getCurrentProduct()->ID);

    // Once the user has product information he/she can now buy the product.
    // Create a transaction request.
    // A new s3ePaymentRequest object is returned which the app should free when done with.
    // When the transaction has completed, the TransactionUpdateCallback will be called.
    if (s3eIOSAppStoreBillingRequestPayment(&PaymentRequest))
		s3eDebugTracePrintf("Purchasing %s...", CurrentProduct->Name.c_str());
    else
        s3eDebugTracePrintf("Purchasing %s FAILED", CurrentProduct->Name.c_str());

}

void CIwGameMarketiOS::GetProductInfo()
{
    // When the user presses the 'get product info' button, request product information
    // and set the status string accordingly.
    // s3eIOSAppStoreBillingRequestPayment will cause the ProductInfoCallback to be
    // called, which then fills in the g_ProductInformation structure
	if (s3eIOSAppStoreBillingRequestProductInformation(&CurrentProductID, 1) == S3E_RESULT_SUCCESS)
    {
        s3eDebugTracePrintf("Requesting product info for: %s...", CurrentProduct->Name.c_str());
    }
    else
    {
        s3eDebugTracePrintf("Requesting product info for %s FAILED", CurrentProduct->Name.c_str());
    }
}

void CIwGameMarketiOS::StopProductinfo()
{
    s3eIOSAppStoreBillingCancelProductInformationRequests();
    s3eDebugTracePrintf("Product info request cancelled");
}

void CIwGameMarketiOS::RestoreTransaction()
{
    s3eIOSAppStoreBillingRestoreCompletedTransactions();
    s3eDebugTracePrintf("Restoring products that were already purchased...");
}

//
//
//
// CIwGameMarketAndroid implementation
//
//
//
CDECLARE_SINGLETON(CIwGameMarketAndroid)

bool CIwGameMarketAndroid::Init()
{
	IIwGameMarket::Init();

    if (s3eAndroidMarketBillingAvailable())
		Available = true;
	else
		return false;

    s3eAndroidMarketBillingInit();

    if (S3E_ANDROIDMARKETBILLING_UNAVAILABLE == s3eAndroidMarketBillingIsSupported())
    {
		setStatus(MS_ERROR_PURCHASE_DISABLED);
		NotifyBillingDisabled();
		NotifyError();
		return false;
    }

	// Register for the S3E_ANDROIDMARKETBILLING_RESPONSE callback
	// This will inform us about the Android Market server's error codes
	s3eAndroidMarketBillingRegister(S3E_ANDROIDMARKETBILLING_RESPONSE, &AsyncResponse, NULL);

	// Register for the S3E_ANDROIDMARKETBILLING_ORDER_INFORMATION callback
	// This will inform us whenever a transaction changes status
	s3eAndroidMarketBillingRegister(S3E_ANDROIDMARKETBILLING_ORDER_INFORMATION, &OrderInformationChanged, NULL);

	// Supply our public key to allow signature checks
	if (!PublicKey.IsEmpty())
	{
		s3eAndroidMarketBillingSetPublicKey(PublicKey.c_str());
	}
	else
	{
		s3eDebugErrorShow(S3E_MESSAGE_CONTINUE, "Please supply your public key to publicKey in s3eAndroidMarketBilling to be able to validate signatures!");
		return false;
	}

	return true;
}

void CIwGameMarketAndroid::Release()
{
	s3eAndroidMarketBillingUnRegister(S3E_ANDROIDMARKETBILLING_ORDER_INFORMATION, OrderInformationChanged);
	s3eAndroidMarketBillingTerminate();

	IIwGameMarket::Release();
}

void CIwGameMarketAndroid::Update()
{
}

bool CIwGameMarketAndroid::QueryProduct(int product_id)
{
	// Find the product
	CIwGameMarketProduct* product = findProduct(product_id);
	if (product == NULL)
	{
		return false;
	}

	setCurrentProduct(product);

	return true;
}

bool CIwGameMarketAndroid::PurchaseProduct(int product_id)
{
	// Find the product
	CIwGameMarketProduct* product = findProduct(product_id);
	if (product == NULL)
	{
		return false;
	}

	// If product is not a consumable and is already purchased then we do not attempt to purchase again
	if (product->Purchased && !product->Consumable)
		return false;

	setCurrentProduct(product);
	setLastPurchaseID(product_id);

	// Start purchase request
CIwGameError::LogError("**** - Purchasing - ", CurrentProduct->ExternalID.c_str());
	s3eAndroidMarketBillingResult r = s3eAndroidMarketBillingRequestPurchase(CurrentProduct->ExternalID.c_str());


	return true;
}

// Callback function
int32 CIwGameMarketAndroid::AsyncResponse(void* system, void* user)
{
    s3eAndroidMarketBillingResult result = ((s3eAndroidMarketBillingResponse*)system)->m_ResponseCode;

CIwGameError::LogError("**** - AsyncResponse - ", CIwGameString((int)result).c_str());
	switch (result)
	{
	case S3E_ANDROIDMARKETBILLING_RESULT_OK:
		IW_GAME_MARKET_ANDROID->setStatus(MS_IDLE);
		break;
	case S3E_ANDROIDMARKETBILLING_RESULT_USER_CANCELLED:
		IW_GAME_MARKET_ANDROID->setStatus(MS_ERROR_PAYMENT_CANCELLED);
		IW_GAME_MARKET_ANDROID->NotifyError();
		break;
	case S3E_ANDROIDMARKETBILLING_RESULT_SERVICE_UNAVAILABLE:
		IW_GAME_MARKET_ANDROID->setStatus(MS_ERROR_NO_CONNECTION);
		IW_GAME_MARKET_ANDROID->NotifyError();
		break;
	case S3E_ANDROIDMARKETBILLING_RESULT_DEVELOPER_ERROR:
		IW_GAME_MARKET_ANDROID->setStatus(MS_ERROR_DEVELOPER_ERROR);
		IW_GAME_MARKET_ANDROID->NotifyError();
		break;
	case S3E_ANDROIDMARKETBILLING_RESULT_BILLING_UNAVAILABLE:
		IW_GAME_MARKET_ANDROID->setStatus(MS_ERROR_PURCHASE_DISABLED);
		IW_GAME_MARKET_ANDROID->NotifyError();
		break;
	case S3E_ANDROIDMARKETBILLING_RESULT_ITEM_UNAVAILABLE:
		IW_GAME_MARKET_ANDROID->setStatus(MS_ERROR_UNKNOWN_PRODUCT);
		IW_GAME_MARKET_ANDROID->NotifyError();
		break;
	case S3E_ANDROIDMARKETBILLING_RESULT_ERROR:
		IW_GAME_MARKET_ANDROID->setStatus(MS_ERROR_PURCHASE_UNKNOWN);
		IW_GAME_MARKET_ANDROID->NotifyError();
		break;
	}

    return 0;
}

// Callback function
int32 CIwGameMarketAndroid::OrderInformationChanged(void* system, void* user)
{
    s3eAndroidMarketBillingOrderInformation *info = (s3eAndroidMarketBillingOrderInformation*)system;
CIwGameError::LogError("**** - OrderInformationChanged");

    for (int i=0; i<info->m_NumOrders; i++)
    {
        s3eAndroidMarketBillingOrder o = info->m_Orders[i];
CIwGameError::LogError("**** - OrderInformationChanged - ", CIwGameString((int)o.m_PurchaseState).c_str());
/*        AppendMessage("Item: %s", o.m_ProductID);
        AppendMessage("Order state: %s", GetOrderState(o.m_PurchaseState));
        AppendMessage("At UTC timestamp: %ld",  o.m_PurchaseTime);
        AppendMessage("Processing order: %s", o.m_OrderID);
        AppendMessage("---------------------------------");*/

        if (o.m_PurchaseState == S3E_ANDROIDMARKETBILLING_PURCHASE_STATE_PURCHASED)
        {
			CIwGameError::LogError("Item is purchased, confirming this to Market");
			IW_GAME_MARKET_ANDROID->NotifyReceiptAvailable();
        }
        else if (o.m_PurchaseState == S3E_ANDROIDMARKETBILLING_PURCHASE_STATE_CANCELLED)
        {
            CIwGameError::LogError("Item is cancelled, confirming this to Market");
			IW_GAME_MARKET_ANDROID->setStatus(MS_ERROR_PAYMENT_CANCELLED);
			IW_GAME_MARKET_ANDROID->NotifyError();
        }
        else if (o.m_PurchaseState == S3E_ANDROIDMARKETBILLING_PURCHASE_STATE_REFUNDED)
        {
            CIwGameError::LogError("Item is refunded, confirming this to Market");
			IW_GAME_MARKET_ANDROID->setRefundedID(o.m_ProductID);
			IW_GAME_MARKET_ANDROID->NotifyRefund();
        }

        s3eAndroidMarketBillingConfirmNotifications(&o.m_NotificationID,1);
    }

    return 0;
}

//
//
//
// CIwGameMarketTest implementation
//
//
//
CDECLARE_SINGLETON(CIwGameMarketTest)

bool CIwGameMarketTest::Init()
{
	IIwGameMarket::Init();

	Available = true;

	return true;
}

void CIwGameMarketTest::Release()
{
	IIwGameMarket::Release();
}

void CIwGameMarketTest::Update()
{
	if (PurchaseTimer.hasStarted() && PurchaseTimer.HasTimedOut())
	{
//		NotifyError();				// Simulate an error during purchasing
//		NotifyRefund();				// Simulate a refund
		NotifyReceiptAvailable();	// Simulate successful purchase
	}
}

bool CIwGameMarketTest::QueryProduct(int product_id)
{
	// Find the product
	CIwGameMarketProduct* product = findProduct(product_id);
	if (product == NULL)
	{
		return false;
	}

	setCurrentProduct(product);
	NotifyInfoAvailable();	// Simulate info available
	NotifyBuyAvailable();	// Simuiate buy available

	return true;
}

bool CIwGameMarketTest::PurchaseProduct(int product_id)
{
	// Find the product
	CIwGameMarketProduct* product = findProduct(product_id);
	if (product == NULL)
	{
		return false;
	}

	// If product is not a consumable and is already purchased then we do not attempt to purchase again
	if (product->Purchased && !product->Consumable)
		return false;

	setCurrentProduct(product);
	setLastPurchaseID(product_id);

	PurchaseTimer.setDuration(2000);

	return true;
}

void CIwGameMarketTest::ForceError(eIwGameMarketStatus status)
{
	setStatus(status);
	NotifyError();	// Simulate an error
}

