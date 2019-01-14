#ifndef CONTRACTOBJECT_H
#define CONTRACTOBJECT_H
#include<bitcoin/bitcoin.hpp>

class ContractObject
{
public:

    //the struct saves the on chain data
    struct BlockChainData
    {
      std::string contract_address;//the hash of the contract content
      std::string contract_sign_content;// the sign of the contract,
      std::string contract_pubkey;//the public key of verify sign
    };

    enum ChainNet
    {
        MAINNET    = 0,
        TESTNET    = 1,
        REGTESTNET = 2
    };

    struct BuildTxVin
    {
        std::string txid;
        int vin_index;
        std::string value;
        BuildTxVin()
        {
            vin_index = 0;
        }
    };

    struct BuildTx
    {
//        std::map<std::string,int> map_vin;
        std::vector<BuildTxVin> vect_vin;
        std::map<std::string,std::string> map_vout;
        std::string opreturn_data;
        std::string priv_key_base58;
        ChainNet chain_net;
        std::vector<int> vect_pos;
        bool sign_all;
        BuildTx()
        {
            chain_net = REGTESTNET;
            sign_all = true;
            opreturn_data.clear();
        }
    };

    ContractObject();
    ContractObject(const std::string& priv_key, const std::string& pub_key,const std::string& contract_content);

    virtual ~ContractObject();

    static bool GetContractHash(const std::string& contract_content, std::string &contract_hash);

    static bool GenerateEncryptKey(std::string& priv_key, std::string& pub_key);

    bool SignContract(const std::string contract, std::string&sign_contract);

    bool VerifySignature(const std::string& sign_contract, const std::string &org_contract);

    bool SetPrivateKey(const std::string& priv_key);

    bool GetPrivateKey(std::string& priv_key) const;

    bool SetPublicKey(const std::string& pub_key);

    bool GetPublicKey(std::string& pub_key) const;

    bool CreateNewAddress(std::string& address, ChainNet chain_net);

    BlockChainData* CreateBlockChainData();

    void SetContractContent(const std::string& contract_content);

    void GetContractAllowAddress(std::vector<std::string>& vect_allow_address);

    void GetPrivateKeyBase58(std::string& priv_key_base58);

    bool CreateTransaction(const BuildTx& tx, std::string &serialize_tx);

    bool SignTransaction(const std::string&priv_key_base58, const std::string& tx_hex,
                         const std::vector<BuildTxVin>& vect_vin,
                         std::string &tx_result);

    bool SignHash(const std::string& priv_key_base58,const std::string& hash_data);
 public:
    bool GetAddressByPrivKeyBase58(const std::string&priv_key_base58, ChainNet chain_net ,std::string& address);
    bool GetAddressByPrivKey(const std::string&priv_key, ChainNet chain_net ,std::string& address);


 protected:
    std::string contract_content_;
    std::string priv_key_;
    std::string pub_key_;
    std::string priv_key_base58_; //this uses for import to full node
    ChainNet chain_net_;
    std::string address_;
};

#endif // CONTRACTOBJECT_H
