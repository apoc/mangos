#ifndef AUCTION_HOUSE_BOT_H
#define AUCTION_HOUSE_BOT_H

#include "World.h"
#include "Config/ConfigEnv.h"
#include "ace/Vector_T.h"

#define AHB_REVISION "[AHBot-005-Alpha-003]"

enum AHBotCommands
{
    AHB_CMD_AHEXPIRE        = 0,
    AHB_CMD_MINITEMS        = 1,
    AHB_CMD_MAXITEMS        = 2,
    AHB_CMD_MINTIME         = 3,
    AHB_CMD_MAXTIME         = 4,
    AHB_CMD_PERCENTAGES     = 5,
    AHB_CMD_MINPRICE        = 6,
    AHB_CMD_MAXPRICE        = 7,
    AHB_CMD_MINBIDPRICE     = 8,
    AHB_CMD_MAXBIDPRICE     = 9,
    AHB_CMD_MAXSTACK        = 10,
    AHB_CMD_BUYERPRICE      = 11,
    AHB_CMD_BIDINTERVAL     = 12,
    AHB_CMD_BIDSPERINTERVAL = 13
};

enum AHBotVectorIds
{
    AHB_VECTOR_ID_PERCENTTG     = 0,        // ID - ACE_Vector<uint32, MAX_ITEM_QUALITY> percentTradeGoods
    AHB_VECTOR_ID_PERCENTITEM   = 1,        // ID - ACE_Vector<uint32, MAX_ITEM_QUALITY> percentItems
    AHB_VECTOR_ID_MINPRICE      = 2,        // ID - ACE_Vector<uint32, MAX_ITEM_QUALITY> minPrice
    AHB_VECTOR_ID_MAXPRICE      = 3,        // ID - ACE_Vector<uint32, MAX_ITEM_QUALITY> maxPrice
    AHB_VECTOR_ID_MINBIDPRICE   = 4,        // ID - ACE_Vector<uint32, MAX_ITEM_QUALITY> minBidPrice
    AHB_VECTOR_ID_MAXBIDPRICE   = 5,        // ID - ACE_Vector<uint32, MAX_ITEM_QUALITY> maxBidPrice
    AHB_VECTOR_ID_MAXSTACK      = 6,        // ID - ACE_Vector<uint32, MAX_ITEM_QUALITY> maxStack
    AHB_VECTOR_ID_BUYERPRICE    = 7,        // ID - ACE_Vector<uint32, MAX_ITEM_QUALITY> buyerPrice
    AHB_MAX_VECTORS                         // Total number of vectors used in AHBConfig class
};

enum AHBotData
{
    AHB_DATA_HEADER             = 2,                                        // Number of headers per vector
    AHB_DATA_UNIT               = (MAX_ITEM_QUALITY + AHB_DATA_HEADER),     // Vector size including headers
    AHB_DATA_SIZE               = (AHB_DATA_UNIT * AHB_MAX_VECTORS)         // Maximum data size
};

enum AHBotDataIndex
{
    AHB_INDEX_PERCENTTG         = (AHB_VECTOR_ID_PERCENTTG * AHB_DATA_UNIT),
    AHB_INDEX_PERCENTITEM       = (AHB_VECTOR_ID_PERCENTITEM * AHB_DATA_UNIT),
    AHB_INDEX_MINPRICE          = (AHB_VECTOR_ID_MINPRICE * AHB_DATA_UNIT),
    AHB_INDEX_MAXPRICE          = (AHB_VECTOR_ID_MAXPRICE * AHB_DATA_UNIT),
    AHB_INDEX_MINBIDPRICE       = (AHB_VECTOR_ID_MINBIDPRICE * AHB_DATA_UNIT),
    AHB_INDEX_MAXBIDPRICE       = (AHB_VECTOR_ID_MAXBIDPRICE * AHB_DATA_UNIT),
    AHB_INDEX_MAXSTACK          = (AHB_VECTOR_ID_MAXSTACK * AHB_DATA_UNIT),
    AHB_INDEX_BUYERPRICE        = (AHB_VECTOR_ID_BUYERPRICE * AHB_DATA_UNIT)
};

class AHBConfig
{
private:
    uint32 AHID;
    uint32 AHFID;
    uint32 AuctioneerGUID;

    uint32 minItems, maxItems;
    uint32 minTime, maxTime;

    ACE_Vector<uint32, MAX_ITEM_QUALITY> percentTradeGoods;
    ACE_Vector<uint32, MAX_ITEM_QUALITY> percentItems;
    ACE_Vector<uint32, MAX_ITEM_QUALITY> minPrice;
    ACE_Vector<uint32, MAX_ITEM_QUALITY> maxPrice;
    ACE_Vector<uint32, MAX_ITEM_QUALITY> minBidPrice;
    ACE_Vector<uint32, MAX_ITEM_QUALITY> maxBidPrice;
    ACE_Vector<uint32, MAX_ITEM_QUALITY> maxStack;

    ACE_Vector<uint32, MAX_ITEM_QUALITY> buyerPrice;

    uint32 buyerBiddingInterval;
    uint32 buyerBidsPerInterval;

public:
    AHBConfig(uint32 ahid)
    {
        AHID = ahid;
        switch(ahid)
        {
        case 2:
            AHFID = 55;
            AuctioneerGUID = 79707; //Human in Stormwind
            break;
        case 6:
            AHFID = 29;
            AuctioneerGUID = 4656; //Orc in Orgrimmar
            break;
        case 7:
            AHFID = 120;
            AuctioneerGUID = 23442; //Goblin in Gadgetzan
            break;
        default:
            AHFID = 120;
            AuctioneerGUID = 23442; //Goblin in Gadgetzan
            break;
        }
    }

    AHBConfig()
    {
    }

    ~AHBConfig()
    {
    }

    uint32 GetAHID() { return AHID; }
    uint32 GetAHFID() { return AHFID; }
    uint32 GetAuctioneerGUID() { return AuctioneerGUID; }

    void SetMinTime(uint32 value) { minTime = (value > 0) ? ((value <= maxTime) ? value : maxTime) : 1; }
    uint32 GetMinTime() { return minTime; }

    void SetMaxTime(uint32 value) { maxTime = (value > 0) ? ((value >= minTime) ? value : minTime) : minTime; }
    uint32 GetMaxTime() { return maxTime; }

    void SetMinItems(uint32 value) { minItems = (value >= 0) ? ((value <= maxItems) ? value : maxItems) : 0; }
    uint32 GetMinItems() { return minItems; }

    void SetMaxItems(uint32 value) { maxItems = (value >= 0) ? ((value >= minItems) ? value : minItems) : minItems; }
    uint32 GetMaxItems() { return maxItems; }

    uint32 GetMinPrice(ItemQualities itemQuality) { return minPrice[itemQuality]; }
    uint32 GetMaxPrice(ItemQualities itemQuality) { return maxPrice[itemQuality]; }

    void SetMinBidPrice(ItemQualities itemQuality, uint32 value)
    {
        minBidPrice[itemQuality] = ((value <= 99) && (value > 0)) ? ((value < maxBidPrice[itemQuality]) ? value : (maxBidPrice[itemQuality] - 1)) : 80;
    }
    uint32 GetMinBidPrice(ItemQualities itemQuality) { return minBidPrice[itemQuality]; }

    void SetMaxBidPrice(ItemQualities itemQuality, uint32 value) {
        maxBidPrice[itemQuality] = ((value <= 100) && (value > 1)) ? ((value > minBidPrice[itemQuality]) ? value : (minBidPrice[itemQuality] + 1)) : 100;
    }
    uint32 GetMaxBidPrice(ItemQualities itemQuality) { return maxBidPrice[itemQuality]; }

    void SetMaxStack(ItemQualities itemQuality, uint32 value) { maxStack[itemQuality] = (value > 0) ? value : 1; }
    uint32 GetMaxStack(ItemQualities itemQuality) { return maxStack[itemQuality]; }

    void SetBuyerPrice(ItemQualities itemQuality, uint32 value) { buyerPrice[itemQuality] = (value > 0) ? value : 1; }
    uint32 GetBuyerPrice(ItemQualities itemQuality) { return buyerPrice[itemQuality]; }

    void SetBiddingInterval(uint32 value) { buyerBiddingInterval = (value > 0) ? value : 1; }
    uint32 GetBiddingInterval() { return buyerBiddingInterval; }

    void SetBidsPerInterval(uint32 value) { buyerBidsPerInterval = (value > 0) ? value : 1; }
    uint32 GetBidsPerInterval() { return buyerBidsPerInterval; }

    void SetPercentage(ItemQualities itemQuality, ItemClass itemClass, uint32 value)
    {
        switch(itemClass)
        {
        case ITEM_CLASS_TRADE_GOODS:
            percentTradeGoods[itemQuality] = value;
            break;
        default:
            percentItems[itemQuality] = value;
            break;
        }
    }

    uint32 GetPercentage(ItemQualities itemQuality, ItemClass itemClass)
    {
        switch(itemClass)
        {
        case ITEM_CLASS_TRADE_GOODS:
            return percentTradeGoods[itemQuality];
            break;
        default:
            return percentItems[itemQuality];
            break;
        }
    }

    void SetMinPrice(ItemQualities itemQuality, uint32 value)
    {
        minPrice[itemQuality] = (value > 0) ? ((value < maxPrice[itemQuality]) ? value : maxPrice[itemQuality]) : 0;
    }


    void SetMaxPrice(ItemQualities itemQuality, uint32 value)
    {
        maxPrice[itemQuality] = (value > 0) ? ((value > minPrice[itemQuality]) ? value : minPrice[itemQuality]) : 0;
    }

    uint32 GetItemCount(ItemQualities itemQuality, ItemClass itemClass)
    {
        switch(itemClass)
        {
        case ITEM_CLASS_TRADE_GOODS:
            return uint32(((double)percentTradeGoods[itemQuality] / 100.0f) * maxItems);
        default:
            return uint32(((double)percentItems[itemQuality] / 100.0f) * maxItems);
        }
    }
};

class AuctionHouseBot
{
private:
    bool AHBSeller;
    bool AHBBuyer;
    bool BuyMethod;
    bool SellMethod;

    bool debugOut;

    bool Vendor_Items;
    bool Loot_Items;
    bool Other_Items;
    bool No_Bind;
    bool Bind_When_Picked_Up;
    bool Bind_When_Equipped;
    bool Bind_When_Use;
    bool Bind_Quest_Item;

    uint32 AHBplayerAccount;
    uint32 AHBplayerGUID;
    uint32 ItemsPerCycle;

    time_t _lastrun_a;
    time_t _lastrun_h;
    time_t _lastrun_n;

    AHBConfig AllianceConfig;
    AHBConfig HordeConfig;
    AHBConfig NeutralConfig;

    ACE_Vector<uint32> npcItems;
    ACE_Vector<uint32> lootItems;

    ACE_Vector<ACE_Vector<uint32>, MAX_ITEM_QUALITY> binTradeGoods;
    ACE_Vector<ACE_Vector<uint32>, MAX_ITEM_QUALITY> binItems;

    void addNewAuctions(Player*, AHBConfig*);
    void addNewAuctionBuyerBotBid(Player*, AHBConfig*, WorldSession*);

    bool LoadValuesArrayFromDB(Tokens&, AHBConfig*);
    bool SaveValuesArrayInDB(Tokens const&, AHBConfig*);

    uint32 GetUInt32ValueFromDB(uint16 index, AHBConfig*);
    uint32 GetUInt32ValueFromArray(Tokens const&, uint16);
    void SetUInt32ValueInDB(uint16, uint32, AHBConfig*);
    uint32 minValue(uint32 a, uint32 b) { return (a <= b) ? a : b; };

public:
    AuctionHouseBot();
    ~AuctionHouseBot();

    void Command(AHBotCommands, uint32);
    void Command(AHBotCommands, uint32, char*);
    void Command(AHBotCommands, uint32, char*, ItemQualities);

    uint32 GetAHBplayerGUID() { return AHBplayerGUID; };
    void Initialize();
    bool isValidMapID(uint32);
    bool LoadValues(AHBConfig*);
    int lookupQuality(char[]);
    const char* lookupQuality(ItemQualities);
    void Update();
};

#define auctionbot MaNGOS::Singleton<AuctionHouseBot>::Instance()

#endif
