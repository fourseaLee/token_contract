#include "contract/contract.h"
#include "assettypes.h"
#include <boost/algorithm/hex.hpp>
#include <glog/logging.h>

bool Asset::Serialize(const Payload* asset, AssetType asset_type, std::string& payload_data)
{
    /* Do not use "using namespace boost::algorithm", because you only  use the namespace function "hex" and  "unhex".
     * if you when to code fewer ,you can use "typedef" or macro .
     *
     * */

    payload_data.clear();
    std::string hex_data;
    hex_data.clear();
    boost::algorithm::hex(asset->version, std::back_inserter(hex_data));
    payload_data += hex_data;

    hex_data.clear();
    boost::algorithm::hex(asset->trading_restricion, std::back_inserter(hex_data));
    payload_data += hex_data;
    hex_data.clear();

    //use for Transmission parameter
    SHCPayload* shc_asset = NULL;
    MOVPayload* mov_asset = NULL;
    TICPayload* tic_asset = NULL;
    COUPayload* cou_asset = NULL;

    bool ret = false;
    switch(asset_type)
    {
        case SHC:
        {
            shc_asset = (SHCPayload*)asset;
            ret = SerializeSHC(shc_asset,hex_data);
            break;
        }
        case MOV:
        {
            mov_asset = (MOVPayload*)asset;
            ret = SerializeMOV(mov_asset,hex_data);
            break;
        }
        case TIC:
        {
            tic_asset = (TICPayload*)asset;
            ret = SerializeTIC(tic_asset,hex_data);
            break;
        }
        case COU:
        {
            cou_asset = (COUPayload*)asset;
            ret = SerializeCOU(cou_asset,hex_data);
            break;
        }
        default:
            break;
    }
    return ret;
}

bool Asset::UnSerialize(Payload* asset, AssetType asset_type, const std::string& payload_data)
{
    if ( asset != NULL)
    {
        delete asset;
        asset = NULL;
    }

    std::string unhex_payload_data;
    boost::algorithm::unhex(payload_data,std::back_inserter(unhex_payload_data));
    LOG(INFO)<<"unhex payload data : "<< unhex_payload_data;

    std::string version;
    std::string substr_data = payload_data.substr(0,2);
    boost::algorithm::unhex(substr_data, std::back_inserter(version));
    LOG(INFO)<<"version : "<< version;

    substr_data = payload_data.substr(2,6);
    std::string  trading_restricion;
    boost::algorithm::unhex(substr_data, std::back_inserter(trading_restricion));
    LOG(INFO)<<"trading_restricion : " <<  trading_restricion;

    bool ret = false;

    switch(asset_type)
    {
        case SHC:
        {
            asset = new SHCPayload();
            asset->version = version;
            asset->trading_restricion = trading_restricion;
            ret = UnSerializeSHC((SHCPayload*)asset,payload_data);
            break;
        }
        case MOV:
        {
            asset = new MOVPayload();
            asset->version = version;
            asset->trading_restricion = trading_restricion;
            ret = UnSerializeMOV((MOVPayload*)asset,payload_data);
            break;
        }
        case TIC:
        {
            asset = new MOVPayload();
            asset->version = version;
            asset->trading_restricion = trading_restricion;
            ret = UnSerializeMOV((MOVPayload*)asset,payload_data);
            break;
        }
        case COU:
        {
            asset = new COUPayload();
            asset->version = version;
            asset->trading_restricion = trading_restricion;
            ret = UnSerializeCOU((COUPayload*)asset,payload_data);
            break;
        }
        default:
            break;
    }
    return ret;
}

bool Asset::SerializeSHC(const Asset::SHCPayload *asset, std::string &payload_data)
{

    std::string hex_data;

    hex_data.clear();
    boost::algorithm::hex(asset->dividend_type,std::back_inserter(hex_data));
    payload_data += hex_data;

    hex_data.clear();
    Contract::FloatToBytes(asset->dividend_var,hex_data);
    payload_data += hex_data;

    hex_data.clear();
    Contract::FloatToBytes(asset->dividend_fixed,hex_data);
    payload_data += hex_data;

    hex_data.clear();
    boost::algorithm::hex(asset->disttribution_interval,std::back_inserter(hex_data));
    payload_data += hex_data;

    hex_data.clear();
    boost::algorithm::hex(asset->guaranteed,std::back_inserter(hex_data));
    payload_data += hex_data;

    hex_data.clear();
    boost::algorithm::hex(asset->ticker,std::back_inserter(hex_data));
    payload_data += hex_data;

    hex_data.clear();
    boost::algorithm::hex(asset->ISIN,std::back_inserter(hex_data));
    payload_data += hex_data;

    hex_data.clear();
    boost::algorithm::hex(asset->description,std::back_inserter(hex_data));
    payload_data += hex_data;

    return true;
}

bool Asset::SerializeMOV(const Asset::MOVPayload *asset, std::string &payload_data)
{
    std::string hex_data;

    hex_data.clear();
    boost::algorithm::hex(asset->age_restriction,std::back_inserter(hex_data));
    payload_data += hex_data;

    hex_data.clear();
    boost::algorithm::hex(asset->venue,std::back_inserter(hex_data));
    payload_data += hex_data;

    hex_data.clear();
    Contract::LongToBytes(asset->valid_from,hex_data);
    payload_data += hex_data;

    hex_data.clear();
    Contract::LongToBytes(asset->expiration_timestamp,hex_data);
    payload_data += hex_data;

    hex_data.clear();
    boost::algorithm::hex(asset->description,std::back_inserter(hex_data));
    payload_data += hex_data;

    return true;
}

bool Asset::SerializeTIC(const Asset::TICPayload *asset, std::__cxx11::string &payload_data)
{
    std::string hex_data;

    hex_data.clear();
    boost::algorithm::hex(asset->age_restriction,std::back_inserter(hex_data));
    payload_data += hex_data;

    hex_data.clear();
    Contract::LongToBytes(asset->valid_from,hex_data);
    payload_data += hex_data;

    hex_data.clear();
    Contract::LongToBytes(asset->expiration_timestamp,hex_data);
    payload_data += hex_data;

    hex_data.clear();
    boost::algorithm::hex(asset->description,std::back_inserter(hex_data));
    payload_data += hex_data;
    return true;
}

bool Asset::SerializeCOU(const Asset::COUPayload *asset, std::string &payload_data)
{
    std::string hex_data;

    hex_data.clear();
    boost::algorithm::hex(asset->age_restriction,std::back_inserter(hex_data));
    payload_data += hex_data;

    hex_data.clear();
    Contract::LongToBytes(asset->valid_from,hex_data);
    payload_data += hex_data;

    hex_data.clear();
    Contract::LongToBytes(asset->expiration_timestamp,hex_data);
    payload_data += hex_data;

    hex_data.clear();
    boost::algorithm::hex(asset->description,std::back_inserter(hex_data));
    payload_data += hex_data;

    return true;
}

bool Asset::UnSerializeSHC(Asset::SHCPayload *asset, const std::string &payload_data)
{
    std::string substr_data = payload_data.substr(8,2);
    boost::algorithm::unhex(substr_data, std::back_inserter(asset->dividend_type));
    LOG(INFO)<<"dividend_type : "<< asset->dividend_type;

    substr_data = payload_data.substr(10,8);
    Contract::BytesToFloat(asset->dividend_var,substr_data);
    LOG(INFO)<<"dividend_var: "<< asset->dividend_var;

    substr_data = payload_data.substr(18,8);
    Contract::BytesToFloat(asset->dividend_fixed,payload_data.substr(18,8));
    LOG(INFO)<<"dividend_fixed: "<< asset->dividend_fixed;

    substr_data = payload_data.substr(26,2);
    boost::algorithm::unhex(substr_data, std::back_inserter(asset->disttribution_interval));
    LOG(INFO)<<"disttribution_interval : "<< asset->disttribution_interval;

    substr_data = payload_data.substr(28,2);
    boost::algorithm::unhex(substr_data, std::back_inserter(asset->guaranteed));
    LOG(INFO)<<"guaranteed : "<< asset->guaranteed;

    substr_data = payload_data.substr(30,8);
    boost::algorithm::unhex(substr_data, std::back_inserter(asset->ticker));
    LOG(INFO)<<"ticker : "<< asset->ticker;

    substr_data = payload_data.substr(38,24);
    boost::algorithm::unhex(substr_data, std::back_inserter(asset->ISIN));
    LOG(INFO)<<"ISIN : "<< asset->ISIN;

    substr_data = payload_data.substr(62,226);
    boost::algorithm::unhex(substr_data, std::back_inserter(asset->description));
    LOG(INFO)<<"description : "<< asset->description;

    return true;
}

bool Asset::UnSerializeMOV(Asset::MOVPayload *asset, const std::string &payload_data)
{
    std::string substr_data = payload_data.substr(8,10);
    boost::algorithm::unhex(substr_data, std::back_inserter(asset->age_restriction));
    LOG(INFO)<<"age_restriction : "<< asset->age_restriction;

    substr_data = payload_data.substr(18,64);
    boost::algorithm::unhex(substr_data, std::back_inserter(asset->venue));
    LOG(INFO)<<"venue : "<< asset->venue;

    substr_data = payload_data.substr(82,8);
    Contract::BytesToLong(asset->valid_from,substr_data);
    LOG(INFO)<<"valid_from: "<< asset->valid_from;

    substr_data = payload_data.substr(90,8);
    Contract::BytesToLong(asset->expiration_timestamp,substr_data);
    LOG(INFO)<<"expiration_timestamp: "<< asset->expiration_timestamp;

    substr_data = payload_data.substr(98,190);
    boost::algorithm::unhex(substr_data, std::back_inserter(asset->description));
    LOG(INFO)<<"description : "<< asset->description;

    return true;
}

bool Asset::UnSerializeTIC(Asset::TICPayload *asset, const std::string &payload_data)
{

    return true;
}

bool Asset::UnSerializeCOU(Asset::COUPayload *asset, const std::string &payload_data)
{
    return true;
}
