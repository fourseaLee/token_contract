#include <string>

class Asset
{
public:
    enum AssetType
    {
        SHC = 1,    // Share Common
        MOV,        // Movie Ticket
        TIC,        // Admission Ticket
        COU         // Coupon
    };

    struct Payload
    {
        std::string operation_code;
        std::string version;//size 1
        std::string trading_restricion;//size 3
        Payload()
        {}
        virtual ~Payload()
        {}
    };
    
    struct SHCPayload:public Payload
    {
        std::string dividend_type; //size 1 (C: Combination V: Variable F:Fixed)
        float dividend_var; //size 4
        float dividend_fixed ;//size 4

        //size 1 (Q: Quarterly M: Monthly W: Weekly A: Annuall B: Biannually)
        std::string disttribution_interval;
        std::string guaranteed; //size 1 (Y or N)
        std::string ticker; //size 4
        std::string ISIN; //size 12
        std::string description; //size 113
        SHCPayload()
        {}
        virtual ~SHCPayload()
        {}
    };

    struct MOVPayload:public Payload
    {
        std::string age_restriction; // 5 
        std::string venue; // 32
        std::string description; // 95
        uint64_t valid_from; // 8
        uint64_t expiration_timestamp; // 8
        MOVPayload()
        {}
        virtual ~MOVPayload()
        {}
    };

    struct TICPayload:public Payload
    {
        std::string age_restriction; // 5 
        std::string description; // 95
        uint64_t valid_from; // 8
        uint64_t expiration_timestamp; // 8
        TICPayload()
        {}
        virtual ~TICPayload()
        {}
    };

    struct COUPayload:public Payload
    {
        std::string age_restriction; // 5 
        std::string description; // 95
        time_t valid_from; // 8
        time_t expiration_timestamp; // 8
        COUPayload()
        {}
        virtual ~COUPayload()
        {}
    };
public:
   bool Serialize(const Payload* asset, AssetType asset_type, std::string& payload_data);
   bool UnSerialize(Payload* asset, AssetType asset_type, const std::string& payload_data);


protected:
   bool SerializeSHC(const SHCPayload* asset, std::string& payload_data);
   bool SerializeMOV(const MOVPayload* asset, std::string& payload_data);
   bool SerializeTIC(const TICPayload* asset, std::string& payload_data);
   bool SerializeCOU(const COUPayload* asset, std::string& payload_data);

protected:
   bool UnSerializeSHC(SHCPayload* asset, const std::string& payload_data);
   bool UnSerializeMOV(MOVPayload* asset, const std::string& payload_data);
   bool UnSerializeTIC(TICPayload* asset, const std::string& payload_data);
   bool UnSerializeCOU(COUPayload* asset, const std::string& payload_data);


};






