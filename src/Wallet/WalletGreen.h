// Copyright (c) 2011-2016 The Cryptonote developers
// Copyright (c) 2018, Karbo developers
// Copyright (c) 2018-2020 The Cash2 developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <queue>
#include <unordered_map>

#include "Common/ConsoleTools.h"
#include "CryptoNoteCore/Currency.h"
#include "IFusionManager.h"
#include "IWallet.h"
#include "System/Dispatcher.h"
#include "System/Event.h"
#include "Transfers/BlockchainSynchronizer.h"
#include "Transfers/TransfersSynchronizer.h"
#include "WalletIndexes.h"

namespace CryptoNote {

class WalletGreen : public IWallet, ITransfersObserver, IBlockchainSynchronizerObserver, ITransfersSynchronizerObserver, IFusionManager {

public:

  WalletGreen(System::Dispatcher& dispatcher, const Currency& currency, INode& node, uint32_t transactionSoftLockTime = 1);
  virtual ~WalletGreen();
  virtual void changePassword(const std::string& oldPassword, const std::string& newPassword) override;
  virtual void commitTransaction(size_t transactionIndex) override;
  virtual std::string createAddress() override;
  virtual std::string createAddress(const Crypto::PublicKey& spendPublicKey) override;
  virtual std::string createAddress(const Crypto::SecretKey& spendPrivateKey) override;
  virtual std::vector<std::string> createAddresses(const std::vector<Crypto::SecretKey>& spendPrivateKeys) override;
  virtual size_t createFusionTransaction(uint64_t threshold, uint64_t mixin) override; // Plan to remove
  virtual void deleteAddress(const std::string& address) override;
  virtual IFusionManager::EstimateResult estimate(uint64_t threshold) const override; // Plan to remove
  virtual uint64_t getActualBalance() const override;
  virtual uint64_t getActualBalance(const std::string& address) const override;
  virtual std::string getAddress(size_t index) const override;
  virtual size_t getAddressCount() const override;
  virtual KeyPair getAddressSpendKeyPair(const std::string& address) const override;
  virtual KeyPair getAddressSpendKeyPair(size_t index) const override;
  virtual uint32_t getBlockCount() const override;
  virtual std::vector<Crypto::Hash> getBlockHashes(uint32_t startBlockIndex, size_t count) const override;
  virtual std::vector<size_t> getDelayedTransactionIndexes() const override;
  virtual WalletEvent getEvent() override;
  virtual uint64_t getPendingBalance() const override;
  virtual uint64_t getPendingBalance(const std::string& address) const override;
  virtual WalletTransactionWithTransfers getTransaction(const Crypto::Hash& transactionHash) const override;
  virtual WalletTransaction getTransaction(size_t transactionIndex) const override;
  virtual size_t getTransactionCount() const override;
  virtual Crypto::SecretKey getTransactionSecretKey(size_t transactionIndex) const override;                                                                                          
  virtual WalletTransfer getTransactionTransfer(size_t transactionIndex, size_t transferIndex) const override;
  virtual size_t getTransactionTransferCount(size_t transactionIndex) const override;
  virtual std::vector<TransactionsInBlockInfo> getTransactions(const Crypto::Hash& blockHash, size_t count) const override;
  virtual std::vector<TransactionsInBlockInfo> getTransactions(uint32_t startBlockIndex, size_t count) const override;
  virtual std::vector<WalletTransactionWithTransfers> getUnconfirmedTransactions() const override;
  virtual KeyPair getViewKeyPair() const override;
  virtual void initialize(const std::string& password) override;
  virtual void initializeWithViewKey(const Crypto::SecretKey& viewPrivateKey, const std::string& password) override;
  virtual bool isFusionTransaction(size_t transactionIndex) const override;
  virtual void load(std::istream& source, const std::string& password) override;
  virtual size_t makeTransaction(const TransactionParameters& sendingTransactionParameters) override;
  virtual void rollbackUncommitedTransaction(size_t) override;
  virtual void save(std::ostream& destination, bool saveDetails = true, bool saveCache = true) override;
  virtual void shutdown() override;
  virtual void start() override;
  virtual void stop() override;
  virtual size_t transfer(const TransactionParameters& sendingTransaction, Crypto::SecretKey& transactionPrivateKey) override;
  
protected :

  enum class WalletState {
    INITIALIZED,
    NOT_INITIALIZED
  };

  enum class WalletTrackingMode {
    TRACKING,
    NOT_TRACKING,
    NO_ADDRESSES
  };

  struct AddressAmounts {
    int64_t input = 0;
    int64_t output = 0;
  };

  struct ContainerAmounts {
    ITransfersContainer* container;
    AddressAmounts amounts;
  };

  struct InputInfo {
    TransactionTypes::InputKeyInfo keyInfo;
    WalletRecord* walletRecord = nullptr;
    KeyPair ephKeys;
  };

  struct OutputToTransfer {
    TransactionOutputInformation out;
    WalletRecord* wallet;
  };

  struct PreparedTransaction {
    std::unique_ptr<ITransaction> transaction;
    std::vector<WalletTransfer> destinations;
    uint64_t neededMoney;
    uint64_t changeAmount;
  };

  struct ReceiverAmounts {
    CryptoNote::AccountPublicAddress receiver;
    std::vector<uint64_t> amounts;
  };

  struct WalletOuts {
    WalletRecord* wallet;
    std::vector<TransactionOutputInformation> outs;
  };

  typedef std::unordered_map<std::string, AddressAmounts> TransfersMap;
  typedef std::pair<WalletTransfers::const_iterator, WalletTransfers::const_iterator> TransfersRange;

  void addBlockHashes(const std::vector<Crypto::Hash>& blockHashes);
  void addUnconfirmedTransaction(const ITransactionReader& transaction);
  std::string addWallet(const Crypto::PublicKey& spendPublicKey, const Crypto::SecretKey& spendPrivateKey, uint64_t creationTimestamp);
  bool adjustTransfer(size_t transactionIndex, size_t firstTransferIndex, const std::string& address, int64_t amount);
  void appendTransfer(size_t transactionIndex, size_t firstTransferIndex, const std::string& address, int64_t amount);
  void blocksRollback(uint32_t blockIndex);
  void clearCaches();
  uint64_t countNeededMoney(const std::vector<CryptoNote::WalletTransfer>& destinations, uint64_t fee);
  const CryptoNote::Currency& currency() const { return m_currency; }
  ReceiverAmounts decomposeFusionOutputs(uint64_t inputsAmount);
  void deleteContainerFromUnlockTransactionJobs(const ITransfersContainer* container);
  void deleteFromUncommitedTransactions(const std::vector<size_t>& deletedTransactionIndexes);
  std::vector<size_t> deleteTransfersForAddress(const std::string& address, std::vector<size_t>& deletedTransactions);
  void deleteUnlockTransactionJob(const Crypto::Hash& transactionHash);
  std::string doCreateAddress(const Crypto::PublicKey& spendPublicKey, const Crypto::SecretKey& spendPrivateKey, uint64_t creationTimestamp);
  void doShutdown();
  size_t doTransfer(const TransactionParameters& transactionParameters, Crypto::SecretKey& transactionPrivateKey);
  bool eraseForeignTransfers(size_t transactionIndex, size_t firstTransferIndex, const std::unordered_set<std::string>& knownAddresses, bool eraseOutputTransfers);
  bool eraseTransfers(size_t transactionIndex, size_t firstTransferIndex, std::function<bool(bool, const std::string&)>&& predicate);
  bool eraseTransfersByAddress(size_t transactionIndex, size_t firstTransferIndex, const std::string& address, bool eraseOutputTransfers);
  void filterOutTransactions(WalletTransactions& transactions, WalletTransfers& transfers, std::function<bool (const WalletTransaction&)>&& pred) const;
  Crypto::Hash getBlockHashByIndex(uint32_t blockIndex) const;
  CryptoNote::AccountPublicAddress getChangeDestination(const std::string& changeDestinationAddress, const std::vector<std::string>& sourceAddresses) const;
  TransfersMap getKnownTransfersMap(size_t transactionIndex, size_t firstTransferIndex) const;
  WalletTrackingMode getTrackingMode() const;
  size_t getTransactionIndex(const Crypto::Hash& transactionHash) const;
  std::vector<WalletTransfer> getTransactionTransfers(const WalletTransaction& transaction) const;
  TransfersRange getTransactionTransfersRange(size_t transactionIndex) const;
  std::vector<TransactionsInBlockInfo> getTransactionsInBlocks(uint32_t startBlockIndex, size_t count) const;
  const WalletRecord& getWalletRecord(const std::string& address) const;
  const WalletRecord& getWalletRecord(CryptoNote::ITransfersContainer* container) const;
  const WalletRecord& getWalletRecord(const Crypto::PublicKey& spendPublicKey) const;
  void initWithKeys(const Crypto::PublicKey& viewPublicKey, const Crypto::SecretKey& viewPrivateKey, const std::string& password);
  size_t insertBlockchainTransaction(const TransactionInformation& txInfo, int64_t txTotalAmount);
  size_t insertOutgoingTransactionAndPushEvent(const Crypto::Hash& transactionHash, uint64_t fee, const BinaryArray& extra, uint64_t unlockTimestamp, Crypto::SecretKey& transactionPrivateKey);
  void insertUnlockTransactionJob(const Crypto::Hash& transactionHash, uint32_t blockHeight, CryptoNote::ITransfersContainer* container);
  bool isFusionTransaction(const WalletTransaction& walletTx) const;
  bool isMyAddress(const std::string& address) const;
  AccountKeys makeAccountKeys(const WalletRecord& walletRecord) const;
  std::unique_ptr<CryptoNote::ITransaction> makeTransaction(const std::vector<ReceiverAmounts>& decomposedOutputs, std::vector<InputInfo>& keysInfo, const std::string& extra, uint64_t unlockTimestamp, Crypto::SecretKey& transactionPrivateKey);
  const CryptoNote::INode& node() const { return m_node; }
  virtual void onBlockchainDetach(const Crypto::PublicKey& viewPublicKey, uint32_t blockIndex) override;
  virtual void onBlocksAdded(const Crypto::PublicKey& viewPublicKey, const std::vector<Crypto::Hash>& blockHashes) override;
  virtual void onError(ITransfersSubscription* object, uint32_t height, std::error_code ec) override;
  void onSynchronizationCompleted();
  void onSynchronizationProgressUpdated(uint32_t processedBlockCount, uint32_t totalBlockCount);
  virtual void onTransactionDeleteBegin(const Crypto::PublicKey& viewPublicKey, Crypto::Hash transactionHash) override;
  virtual void onTransactionDeleteEnd(const Crypto::PublicKey& viewPublicKey, Crypto::Hash transactionHash) override;
  virtual void onTransactionDeleted(ITransfersSubscription* object, const Crypto::Hash& transactionHash) override;
  virtual void onTransactionUpdated(ITransfersSubscription* object, const Crypto::Hash& transactionHash) override;
  virtual void onTransactionUpdated(const Crypto::PublicKey& viewPublicKey, const Crypto::Hash& transactionHash, const std::vector<ITransfersContainer*>& containers) override;
  CryptoNote::AccountPublicAddress parseAddress(const std::string& address) const;
  std::vector<OutputToTransfer> pickRandomFusionInputs(uint64_t threshold, size_t minInputCount, size_t maxInputCount);
  WalletOuts pickWallet(const std::string& address);
  std::vector<WalletOuts> pickWallets(const std::vector<std::string>& addresses);
  std::vector<WalletOuts> pickWalletsWithMoney() const;
  void prepareInputs(const std::vector<OutputToTransfer>& selectedTransfers, std::vector<CryptoNote::CORE_RPC_COMMAND_GET_RANDOM_OUTPUTS_FOR_AMOUNTS::outs_for_amount>& mixinResult, uint64_t mixIn, std::vector<InputInfo>& keysInfo);
  void prepareTransaction(std::vector<WalletOuts>&& wallets, const std::vector<WalletOrder>& orders, uint64_t fee, uint64_t mixIn, const std::string& extra, uint64_t unlockTimestamp, const DonationSettings& donation, const CryptoNote::AccountPublicAddress& changeDestinationAddress, PreparedTransaction& preparedTransaction, Crypto::SecretKey& transactionPrivateKey);
  void pushBackOutgoingTransfers(size_t transactionIndex, const std::vector<WalletTransfer>& destinations);
  void pushEvent(const WalletEvent& event);
  void removeUnconfirmedTransaction(const Crypto::Hash& transactionHash);
  void requestMixinOuts(const std::vector<OutputToTransfer>& selectedTransfers, uint64_t mixIn, std::vector<CryptoNote::CORE_RPC_COMMAND_GET_RANDOM_OUTPUTS_FOR_AMOUNTS::outs_for_amount>& mixinResult);
  uint64_t selectTransfers(uint64_t needeMoney, bool dust, uint64_t dustThreshold, std::vector<WalletOuts>&& wallets, std::vector<OutputToTransfer>& selectedTransfers);
  void sendTransaction(const CryptoNote::Transaction& transaction);
  ReceiverAmounts splitAmount(uint64_t amount, const AccountPublicAddress& destination, uint64_t dustThreshold);
  std::vector<ReceiverAmounts> splitDestinations(const std::vector<WalletTransfer>& destinations, uint64_t dustThreshold, const Currency& currency);
  void startBlockchainSynchronizer();
  void stopBlockchainSynchronizer();
  virtual void synchronizationCompleted(std::error_code result) override;
  virtual void synchronizationProgressUpdated(uint32_t processedBlockCount, uint32_t totalBlockCount) override;
  void throwIfNotInitialized() const;
  void throwIfStopped() const;
  void throwIfTrackingMode() const;
  void transactionDeleteBegin(Crypto::Hash transactionHash);
  void transactionDeleteEnd(Crypto::Hash transactionHash);
  void transactionDeleted(ITransfersSubscription* object, const Crypto::Hash& transactionHash);
  void transactionUpdated(const TransactionInformation& transactionInfo, const std::vector<ContainerAmounts>& containerAmountsList);
  void unlockBalances(uint32_t height);
  void unsafeLoad(std::istream& source, const std::string& password);
  void unsafeSave(std::ostream& destination, bool saveDetails, bool saveCache);
  bool updateAddressTransfers(size_t transactionIndex, size_t firstTransferIndex, const std::string& address, int64_t knownAmount, int64_t targetAmount);
  void updateBalance(CryptoNote::ITransfersContainer* container);
  void updateBlockHashesContainerWithViewKey(const Crypto::PublicKey& viewPublicKey);
  void updateTransactionStateAndPushEvent(size_t transactionIndex, WalletTransactionState state);
  bool updateTransactionTransfers(size_t transactionIndex, const std::vector<ContainerAmounts>& containerAmountsList, int64_t allInputsAmount, int64_t allOutputsAmount);
  bool updateUnknownTransfers(size_t transactionIndex, size_t firstTransferIndex, const std::unordered_set<std::string>& myAddresses, int64_t knownAmount, int64_t myAmount, int64_t totalAmount, bool isOutput);
  bool updateWalletTransactionInfo(size_t transactionIndex, const CryptoNote::TransactionInformation& info, int64_t totalAmount);
  size_t validateSaveAndSendTransaction(const ITransactionReader& transaction, const std::vector<WalletTransfer>& destinations, bool isFusion, bool send);
  void validateTransactionParameters(const TransactionParameters& transactionParameters);

  uint64_t m_actualBalance;
  BlockchainSynchronizer m_blockchainSynchronizer;
  bool m_blockchainSynchronizerStarted;
  BlockHashesContainer m_blockHashesContainer;
  const Currency& m_currency;
  System::Dispatcher& m_dispatcher;
  System::Event m_eventOccurred;
  std::queue<WalletEvent> m_eventsQueue;
  mutable std::unordered_map<size_t, bool> m_fusionTxsCache; // txIndex -> isFusion
  INode& m_node;
  std::string m_password;
  uint64_t m_pendingBalance;
  mutable System::Event m_readyEvent;
  bool m_stopped;
  TransfersSyncronizer m_transfersSynchronizer;
  uint32_t m_transactionSoftLockTime;
  UncommitedTransactions m_uncommitedTransactions;
  UnlockTransactionJobs m_unlockTransactionsJob;
  uint64_t m_upperTransactionSizeLimit;
  Crypto::PublicKey m_viewPublicKey;
  Crypto::SecretKey m_viewPrivateKey;
  WalletState m_walletState;
  WalletsContainer m_walletsContainer;
  WalletTransactions m_walletTransactions;
  WalletTransfers m_walletTransfers; //sorted
  
  friend class refresh_progress_reporter_t;

  class refresh_progress_reporter_t
  {
  public:
    refresh_progress_reporter_t(CryptoNote::WalletGreen& wallet_green) :
      m_wallet_green(wallet_green),
      m_blockchain_height(0),
      m_blockchain_height_update_time(),
      m_print_time() {}

    void update(uint64_t height, bool force = false)
    {
      auto current_time = std::chrono::system_clock::now();
      if (std::chrono::seconds(m_wallet_green.currency().difficultyTarget() / 2) < current_time - m_blockchain_height_update_time || m_blockchain_height <= height) {
        update_blockchain_height();
        m_blockchain_height = (std::max)(m_blockchain_height, height);
      }

      uint64_t printWidth = 9;

      if (std::chrono::milliseconds(1) < current_time - m_print_time || force) {
        Common::Console::setTextColor(Common::Console::Color::BrightCyan);
        std::cout << "Height " << height << " of " << m_blockchain_height << '\r';
        std::cout << "Height " <<
          std::setw(printWidth) << centerAlignHeight(printWidth, height) <<
          " of " <<
          std::setw(printWidth) << centerAlignHeight(printWidth, m_blockchain_height) <<
          '\r' << std::flush;
        Common::Console::setTextColor(Common::Console::Color::Default);
        m_print_time = current_time;
      }
    }

  private:
    void update_blockchain_height()
    {
      uint64_t blockchain_height = m_wallet_green.node().getLastLocalBlockHeight() + 1;
      m_blockchain_height = blockchain_height;
      m_blockchain_height_update_time = std::chrono::system_clock::now();
    }

    std::string centerAlignHeight(uint64_t printWidth, uint64_t blockHeight)
    {

      std::string blockHeightStr = std::to_string(blockHeight);

      uint64_t blockHeightStrLength = blockHeightStr.length();

      if (printWidth < blockHeightStrLength)
      {
        return blockHeightStr;
      }

      uint64_t leftPadding = (printWidth - blockHeightStrLength) / 2;
      uint64_t rightPadding = printWidth - blockHeightStrLength - leftPadding;

      return std::string(leftPadding, ' ') + blockHeightStr + std::string(rightPadding, ' ');
    }

    uint64_t m_blockchain_height;
    std::chrono::system_clock::time_point m_blockchain_height_update_time;
    std::chrono::system_clock::time_point m_print_time;
    CryptoNote::WalletGreen& m_wallet_green;
  }; // class refresh_progress_reporter_t

  refresh_progress_reporter_t m_refresh_progress_reporter;

}; // end class WalletGreen

} // end namespace CryptoNote
