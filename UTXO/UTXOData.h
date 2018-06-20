/*
	This file is part of FISCO BCOS.

	FISCO BCOS is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	FISCO BCOS is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with FISCO BCOS.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
 * @file: UTXOData.h
 * @author: chaychen
 * 
 * @date: 2018
 */

#ifndef __UTXODATA_H__
#define __UTXODATA_H__

#include <string>

#include <libdevcore/RLP.h>
#include <libdevcore/CommonJS.h>
#include <libdevcore/SHA3.h>
#include <libdevcrypto/Common.h>

#include "Common.h"

using namespace std;
using namespace dev;

namespace UTXOModel
{
	// The TokenBase field remains unchanged after generated, and only one instance in DB.
	// The format of TokenBase key is "TB#transactionHash_idxInUTXOTx".
	class TokenBase {
	public:
		TokenBase() {}
		TokenBase(h256 transactionHash, 
			  u256 idxInUTXOTx, 
			  u256 value, 
			  const string& owner, 
			  const string& checkType, 
			  Address validationContract, 
			  const string& contractType):
			  m_transactionHash(transactionHash),
			  m_idxInUTXOTx(idxInUTXOTx),
			  m_value(value),
			  m_owner(owner),
			  m_checkType(checkType),
			  m_validationContract(validationContract),
			  m_contractType(contractType) {}
		TokenBase(const TokenBase& _s):
			  m_transactionHash(_s.m_transactionHash),
			  m_idxInUTXOTx(_s.m_idxInUTXOTx),
			  m_value(_s.m_value),
			  m_owner(_s.m_owner),
			  m_checkType(_s.m_checkType),
			  m_validationContract(_s.m_validationContract),
			  m_contractType(_s.m_contractType) {}
		TokenBase& operator=(const TokenBase& _s);
		TokenBase(const bytes &_rlp);
		~TokenBase() {}

		void streamRLP(RLPStream& _s) const;
		bytes rlp() const;
		void Print() const;
		string ToJsonString() const;
		
		void setTransactionHash(h256 transactionHash) { m_transactionHash = transactionHash; }
		h256 getTransactionHash() const { return m_transactionHash; }
		void setIdxInUTXOTx(u256 idxInUTXOTx) { m_idxInUTXOTx = idxInUTXOTx; }
		u256 getIdxInUTXOTx() const { return m_idxInUTXOTx; }
		void setValue(u256 value) { m_value = value; }
		u256 getValue() const { return m_value; }
		void setOwner(string const& owner) { m_owner = owner; }
		string getOwner() const { return m_owner; }
		void setCheckType(const string& checkType) { m_checkType = checkType; }
		string getCheckType() const { return m_checkType; }
		void setValidationContract(Address validationContract) { m_validationContract = validationContract; }
		Address getValidationContract() const { return m_validationContract; }
		void setContractType(const string& contractType) { m_contractType = contractType; }
		string getContractType() const { return m_contractType; }
		h256 getOwnerHash() const 
		{
			if (UTXO_OWNERSHIP_CHECK_TYPE_P2PK == m_checkType)
			{
				return sha3(m_owner);
			}
			else if (UTXO_OWNERSHIP_CHECK_TYPE_P2PKH == m_checkType)
			{
				return jsToFixed<32>(m_owner);
			}
			else 
			{
				return h256(0);
			}
		}
	private:
		h256 m_transactionHash;			// The ethereum transaction hash that generates this Token
		u256 m_idxInUTXOTx;				// The output index in UTXO transaction
		u256 m_value;					// The value of this Token
		string m_owner;					// Owner information（account for P2PK or account hash for P2PKH）
		string m_checkType;				// Ownership verification type（P2PK or P2PKH）
		Address m_validationContract;	// Contract address for verification（passed directly or generated by another call）
		string m_contractType;			// Contract type for verification（General or CaseBased）
	};

	// There are different versions of TokenExt related to a single TokenBase.
	// The latest TokenExt will be got through the block height  when transaction executed. 
	// The format of TokenExt key is "TE#transactionHash_idxInUTXOTx#blockNum".
	class TokenExt {
	public:
		TokenExt() {}
		TokenExt(TokenState TokenState, const string& tokenDetail):m_tokenState(TokenState),m_tokenDetail(tokenDetail) {}
		TokenExt(const TokenExt& _s):m_tokenState(_s.m_tokenState),m_tokenDetail(_s.m_tokenDetail) {}
		TokenExt& operator=(const TokenExt& _s);
		TokenExt(const bytes &_rlp);
		~TokenExt() {}

		void streamRLP(RLPStream& _s) const;
		bytes rlp() const;
		void Print() const;
		string ToJsonString() const;

		void setTokenState(TokenState TokenState) { m_tokenState = TokenState; }
		TokenState getTokenState() const { return m_tokenState; }
		void setTokenDetail(const string& tokenDetail) { m_tokenDetail = tokenDetail; }
		string getTokenDetail() const { return m_tokenDetail; }
	private:
		TokenState m_tokenState;		// Token spent or not
		string m_tokenDetail;			// Token detail
	};

	// The TokenExt will increase logically after generated.
	// It is used to find a token at a specific block height.
	// The format of TokenExtOnBlockNum key is "TN#transactionHash_idxInUTXOTx".
	class TokenExtOnBlockNum {
	public:
		TokenExtOnBlockNum() {}
		TokenExtOnBlockNum(const vector<u256>& updatedBlockNum):m_updatedBlockNum(updatedBlockNum) {}
		TokenExtOnBlockNum(const TokenExtOnBlockNum& _s):m_updatedBlockNum(_s.m_updatedBlockNum) {}
		TokenExtOnBlockNum& operator=(const TokenExtOnBlockNum& _s);
		TokenExtOnBlockNum(const bytes &_rlp);
		~TokenExtOnBlockNum() {}

		void streamRLP(RLPStream& _s) const;
		bytes rlp() const;
		void Print() const;

		void setUpdatedBlockNum(const vector<u256>& updatedBlockNum) { m_updatedBlockNum = updatedBlockNum; }
		void addUpdatedBlockNum(u256 updatedBlockNum) { m_updatedBlockNum.push_back(updatedBlockNum); }
		vector<u256> getUpdatedBlockNum() const { return m_updatedBlockNum; }
	
	private:
		vector<u256> m_updatedBlockNum;	// Used to record the block height when a token is changed
	};

	class Token {
	public:
		Token() {}
		Token(const TokenBase& tokenBase, const TokenExt& tokenExt):
			m_tokenBase(tokenBase),
			m_tokenExt(tokenExt) {}
		~Token() {}
		Token(const Token& _s):m_tokenBase(_s.m_tokenBase),m_tokenExt(_s.m_tokenExt) {}
		Token& operator=(const Token& _s);
		string ToJsonString() const;
		
		TokenBase m_tokenBase;
		TokenExt m_tokenExt;
	};

	// The UTXOTx remains unchanged after generated, and only one instance in DB.
	// Used for recording transformation relationship for tokens
	// The format of UTXOTx key is "TX#transactionHash".
	class UTXOTx {
	public:
		UTXOTx() {}
		UTXOTx(const vector<string>& inTokenKey, const vector<string>& outTokenKey):
			m_inTokenKey(inTokenKey),
			m_outTokenKey(outTokenKey) {}
		UTXOTx(const bytes &_rlp);
		~UTXOTx() {}

		void streamRLP(RLPStream& _s) const;
		bytes rlp() const;
		void Print() const;
		string ToJsonString() const;

		void setInTokenKey(const vector<string>& inTokenKey) { m_inTokenKey = inTokenKey; }
		vector<string> getInTokenKey() const { return m_inTokenKey; }
		void setOutTokenKey(const vector<string>& outTokenKey) { m_outTokenKey = outTokenKey; }
		vector<string> getOutTokenKey() const { return m_outTokenKey; }			

	private:
		vector<string> m_inTokenKey;	// A list of tokens for consumption
		vector<string> m_outTokenKey;	// Generated tokens list 
	};

	// The cache of vault
	// Used to find tokens that meet the payment amount
	class Token_Record {
	public:
		Token_Record() {}
		~Token_Record() {}
		Token_Record(const Token_Record& _s):tokenKey(_s.tokenKey),tokenValue(_s.tokenValue),tokenState(_s.tokenState) {}
		Token_Record(const string& _tokenKey, u256 _tokenValue, TokenState _tokenState):
			tokenKey(_tokenKey),
			tokenValue(_tokenValue),
			tokenState(_tokenState) {}

		string tokenKey;
		u256 tokenValue;
		TokenState tokenState;
	};

	// Recording a list of token that belongs to someone.
	// The format of Vault key is "transactionHash_idxInUTXOTx".
	// Vault data, Inonsistent data between nodes, is stored in an independent DB.
	// Recorded in memory after registering a account.
	class Vault {
	public:
		Vault() {}
		Vault(h256 ownerHash, const string& tokenKey):m_ownerHash(ownerHash),m_tokenKey(tokenKey) {}
		Vault(const Vault& _s):m_ownerHash(_s.getOwnerHash()),m_tokenKey(_s.getTokenKey()) {}
		Vault(const bytes &_rlp);
		~Vault() {}

		void streamRLP(RLPStream& _s) const;
		bytes rlp() const;
		void Print() const;
		string ToJsonString() const;

		void setOwnerHash(h256 ownerHash) { m_ownerHash = ownerHash; }
		h256 getOwnerHash() const { return m_ownerHash; }
		void setTokenKey(const string& tokenKey) { m_tokenKey = tokenKey; }
		string getTokenKey() const { return m_tokenKey; }
	private:
		h256 m_ownerHash;			// Owner information（account hash）
		string m_tokenKey;			// Token Key
	};

	// Recording datas generated in executing UTXO transaction, excluding vault data.
	// The UTXODBCache data will be stored in disk when commiting block data to disk.
	// The hash of UTXODBCache data will be recorded in the header of block.
	class UTXODBCache {
	public:
		UTXODBCache() {}
		UTXODBCache(const string& key, const string& value):m_key(key),m_value(value) {}
		~UTXODBCache() {}

		void streamRLP(RLPStream& _s) const { _s.appendList(2) << m_key << m_value; }

		string getKey() const { return m_key; }
		string getValue() const { return m_value; }
	private:
		string m_key;				// The key of TokenBase、TokenExt、TokenNum and Tx
		string m_value;				// The value of TokenBase、TokenExt、TokenNum and Tx
	};

	// Paging query parameters
	struct QueryUTXOParam {
		u256 start;					// [in]
		u256 cnt;					// [in]
		u256 end;					// [out]
		u256 total;					// [out]
		u256 totalValue;			// [out], Only used in the SelectTokens interface.
	};

	// The data of ethereum transaction, with preliminary verification
	struct UTXOTxIn {
		string tokenKey;			// The token for consumption
		string callFuncAndParams;	// The function and params, used for logical validation（in the process of General and CaseBased）
		string exeFuncAndParams;	// The function and params, used for updating the global state（only in the process of General）
		string detail;				// Comments for tokens being consumed
	};

	struct UTXOTxOut {
		string to;					// Payee
		u256 value;					// Turnovers
		string checkType;			// Ownership verification type
		Address initContract;		// The template contract address
		string initFuncAndParams;	// The function and params, used for calling template contract
		Address validationContract;	// The address of the contract used to perform validation logic
		string detail;				// Comments for new tokens
	};

	class AccountRecord {
	public:
		AccountRecord() {}
		AccountRecord(const vector<h256>& accountList):m_accountList(accountList) {}
		AccountRecord(const AccountRecord& _s):m_accountList(_s.m_accountList) {}
		AccountRecord& operator=(const AccountRecord& _s);
		AccountRecord(const bytes &_rlp);
		~AccountRecord() {}

		void streamRLP(RLPStream& _s) const;
		bytes rlp() const;
		void Print() const;

		void setAccountList(const vector<h256>& accountList) { m_accountList = accountList; }
		void addAccountList(h256 account) { m_accountList.push_back(account); }
		vector<h256> getAccountList() const { return m_accountList; }
	
	private:
		vector<h256> m_accountList;
	};
}//namespace UTXOModel

#endif//__UTXODATA_H__