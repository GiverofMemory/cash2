// Copyright (c) 2011-2016 The Cryptonote developers
// Copyright (c) 2016-2019, The Karbo Developers
// Copyright (c) 2018 The Turtlecoin developers
// Copyright (c) 2018-2019 The Cash2 developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "WalletdRpcCommands.h"
#include "version.h"

namespace PaymentService {

WalletdRpcCommands::WalletdRpcCommands(WalletService& walletService) : service(walletService) {}

std::error_code WalletdRpcCommands::handleReset(const WALLETD_RPC_COMMAND_RESET::Request& request, WALLETD_RPC_COMMAND_RESET::Response& response) {
  if (request.view_private_key.empty()) {
    return service.resetWallet();
  } else {
    return service.replaceWithNewWallet(request.view_private_key);
  }
}

std::error_code WalletdRpcCommands::handleCreateAddress(const WALLETD_RPC_COMMAND_CREATE_ADDRESS::Request& request, WALLETD_RPC_COMMAND_CREATE_ADDRESS::Response& response) {
  if (request.spend_private_key.empty() && request.spend_public_key.empty()) {
    return service.createAddress(response.address);
  } else if (!request.spend_private_key.empty()) {
    return service.createAddress(request.spend_private_key, response.address);
  } else {
    return service.createTrackingAddress(request.spend_public_key, response.address);
  }
}

std::error_code WalletdRpcCommands::handleDeleteAddress(const WALLETD_RPC_COMMAND_DELETE_ADDRESS::Request& request, WALLETD_RPC_COMMAND_DELETE_ADDRESS::Response& response) {
  return service.deleteAddress(request.address);
}

std::error_code WalletdRpcCommands::handleGetSpendPrivateKey(const WALLETD_RPC_COMMAND_GET_SPEND_PRIVATE_KEY::Request& request, WALLETD_RPC_COMMAND_GET_SPEND_PRIVATE_KEY::Response& response) {
  return service.getSpendPrivateKey(request.address, response.spend_private_key);
}

std::error_code WalletdRpcCommands::handleGetSpendPrivateKeys(const WALLETD_RPC_COMMAND_GET_SPEND_PRIVATE_KEYS::Request& request, WALLETD_RPC_COMMAND_GET_SPEND_PRIVATE_KEYS::Response& response) {
  return service.getSpendPrivateKeys(response.spend_private_keys);
}

std::error_code WalletdRpcCommands::handleGetBalance(const WALLETD_RPC_COMMAND_GET_BALANCE::Request& request, WALLETD_RPC_COMMAND_GET_BALANCE::Response& response) {
  if (!request.address.empty()) {
    return service.getBalance(request.address, response.available_balance, response.locked_amount);
  } else {
    return service.getBalance(response.available_balance, response.locked_amount);
  }
}

std::error_code WalletdRpcCommands::handleGetBlockHashes(const WALLETD_RPC_COMMAND_GET_BLOCK_HASHES::Request& request, WALLETD_RPC_COMMAND_GET_BLOCK_HASHES::Response& response) {
  uint32_t startBlockIndex = request.start_block_height - 1;
  return service.getBlockHashes(startBlockIndex, request.number_of_blocks, response.block_hashes);
}

std::error_code WalletdRpcCommands::handleGetTransactionHashes(const WALLETD_RPC_COMMAND_GET_TRANSACTION_HASHES::Request& request, WALLETD_RPC_COMMAND_GET_TRANSACTION_HASHES::Response& response) {
  if (!request.start_block_hash.empty()) {
    return service.getTransactionHashes(request.addresses, request.start_block_hash, request.number_of_blocks, request.payment_id, response.items);
  } else {
    uint32_t startBlockIndex = request.start_block_height - 1;
    return service.getTransactionHashes(request.addresses, startBlockIndex, request.number_of_blocks, request.payment_id, response.items);
  }
}

std::error_code WalletdRpcCommands::handleGetTransactions(const WALLETD_RPC_COMMAND_GET_TRANSACTIONS::Request& request, WALLETD_RPC_COMMAND_GET_TRANSACTIONS::Response& response) {
  if (!request.start_block_hash.empty()) {
    return service.getTransactions(request.addresses, request.start_block_hash, request.number_of_blocks, request.payment_id, response.items);
  } else {
    uint32_t startBlockIndex = request.start_block_height - 1;
    return service.getTransactions(request.addresses, startBlockIndex, request.number_of_blocks, request.payment_id, response.items);
  }
}

std::error_code WalletdRpcCommands::handleGetUnconfirmedTransactionHashes(const WALLETD_RPC_COMMAND_GET_UNCONFIRMED_TRANSACTION_HASHES::Request& request, WALLETD_RPC_COMMAND_GET_UNCONFIRMED_TRANSACTION_HASHES::Response& response) {
  return service.getUnconfirmedTransactionHashes(request.addresses, response.transaction_hashes);
}

std::error_code WalletdRpcCommands::handleGetTransaction(const WALLETD_RPC_COMMAND_GET_TRANSACTION::Request& request, WALLETD_RPC_COMMAND_GET_TRANSACTION::Response& response) {
  return service.getTransaction(request.transaction_hash, response.transaction);
}

std::error_code WalletdRpcCommands::handleSendTransaction(const WALLETD_RPC_COMMAND_SEND_TRANSACTION::Request& request, WALLETD_RPC_COMMAND_SEND_TRANSACTION::Response& response) {
  return service.sendTransaction(request, response.transaction_hash, response.transaction_private_key);
}

std::error_code WalletdRpcCommands::handleCreateDelayedTransaction(const WALLETD_RPC_COMMAND_CREATE_DELAYED_TRANSACTION::Request& request, WALLETD_RPC_COMMAND_CREATE_DELAYED_TRANSACTION::Response& response) {
  return service.createDelayedTransaction(request, response.transaction_hash);
}

std::error_code WalletdRpcCommands::handleGetDelayedTransactionHashes(const WALLETD_RPC_COMMAND_GET_DELAYED_TRANSACTION_HASHES::Request& request, WALLETD_RPC_COMMAND_GET_DELAYED_TRANSACTION_HASHES::Response& response) {
  return service.getDelayedTransactionHashes(response.transaction_hashes);
}

std::error_code WalletdRpcCommands::handleDeleteDelayedTransaction(const WALLETD_RPC_COMMAND_DELETE_DELAYED_TRANSACTION::Request& request, WALLETD_RPC_COMMAND_DELETE_DELAYED_TRANSACTION::Response& response) {
  return service.deleteDelayedTransaction(request.transaction_hash);
}

std::error_code WalletdRpcCommands::handleSendDelayedTransaction(const WALLETD_RPC_COMMAND_SEND_DELAYED_TRANSACTION::Request& request, WALLETD_RPC_COMMAND_SEND_DELAYED_TRANSACTION::Response& response) {
  return service.sendDelayedTransaction(request.transaction_hash);
}

std::error_code WalletdRpcCommands::handleGetViewKey(const WALLETD_RPC_COMMAND_GET_VIEW_KEY::Request& request, WALLETD_RPC_COMMAND_GET_VIEW_KEY::Response& response) {
  return service.getViewKey(response.view_private_key);
}

std::error_code WalletdRpcCommands::handleGetStatus(const WALLETD_RPC_COMMAND_GET_STATUS::Request& request, WALLETD_RPC_COMMAND_GET_STATUS::Response& response) {
  response.cash2_software_version = PROJECT_VERSION_LONG;
  return service.getStatus(response.block_count, response.known_block_count, response.last_block_hash, response.peer_count, response.minimal_fee);
}

std::error_code WalletdRpcCommands::handleGetAddresses(const WALLETD_RPC_COMMAND_GET_ADDRESSES::Request& request, WALLETD_RPC_COMMAND_GET_ADDRESSES::Response& response) {
  return service.getAddresses(response.addresses);
}

std::error_code WalletdRpcCommands::handleGetAddressesCount(const WALLETD_RPC_COMMAND_GET_ADDRESSES_COUNT::Request& request, WALLETD_RPC_COMMAND_GET_ADDRESSES_COUNT::Response& response) {
  return service.getAddressesCount(response.addresses_count);
}

std::error_code WalletdRpcCommands::handleValidateAddress(const WALLETD_RPC_COMMAND_VALIDATE_ADDRESS::Request& request, WALLETD_RPC_COMMAND_VALIDATE_ADDRESS::Response& response) {
  return service.validateAddress(request.address, response.address_valid);
}

} // end namespace PaymentService