#ifndef AUCTION_HOUSE_BOT_H
#define AUCTION_HOUSE_BOT_H

#include "World.h"
#include "Config/ConfigEnv.h"
#include "ace/Vector_T.h"

#define AHB_REVISION "[AHBot-005-Alpha-005]"

enum AHBotCommands  // All commands that AHBot supports
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
    AHB_CMD_MINSTACK        = 10,
    AHB_CMD_MAXSTACK        = 11,
    AHB_CMD_BUYERPRICE      = 12,
    AHB_CMD_BIDINTERVAL     = 13,
    AHB_CMD_BIDSPERINTERVAL = 14
};

enum AHBotVectorIds // All vectors used by AHBot
{
    AHB_VECTOR_ID_PERCENTITEMS  = 0,        // ID - ACE_Vector<ACE_Vector<uint32, MAX_ITEM_QUALITY>, MAX_ITEM_CLASS> percentItems
    AHV_VECTOR_ID_COUNTITEMS    = 1,        // ID - ACE_Vector<ACE_Vector<uint32, MAX_ITEM_QUALITY>, MAX_ITEM_CLASS> countItems
    AHB_VECTOR_ID_MINPRICE      = 2,        // ID - ACE_Vector<ACE_Vector<uint32, MAX_ITEM_QUALITY>, MAX_ITEM_CLASS> minPrice
    AHB_VECTOR_ID_MAXPRICE      = 3,        // ID - ACE_Vector<ACE_Vector<uint32, MAX_ITEM_QUALITY>, MAX_ITEM_CLASS> maxPrice
    AHB_VECTOR_ID_MINBIDPRICE   = 4,        // ID - ACE_Vector<ACE_Vector<uint32, MAX_ITEM_QUALITY>, MAX_ITEM_CLASS> minBidPrice
    AHB_VECTOR_ID_MAXBIDPRICE   = 5,        // ID - ACE_Vector<ACE_Vector<uint32, MAX_ITEM_QUALITY>, MAX_ITEM_CLASS> maxBidPrice
    AHB_VECTOR_ID_MINSTACK      = 7,        // ID - ACE_Vector<ACE_Vector<uint32, MAX_ITEM_QUALITY>, MAX_ITEM_CLASS> minStack
    AHB_VECTOR_ID_MAXSTACK      = 8,        // ID - ACE_Vector<ACE_Vector<uint32, MAX_ITEM_QUALITY>, MAX_ITEM_CLASS> maxStack
    AHB_VECTOR_ID_BUYERPRICE    = 9,        // ID - ACE_Vector<ACE_Vector<uint32, MAX_ITEM_QUALITY>, MAX_ITEM_CLASS> buyerPrice
    AHB_MAX_VECTORS                         // Total number of vectors used in AHBConfig class
};

enum AHBotHeaders   // Defines the header structure for each vector in the 'data' DB field
{
    AHB_HEADER_VECTOR_ID        = 0,        // The ID of the vector, see AHBotVectorIds
    AHB_HEADER_DIM_CLASS_SIZE   = 1,        // The size of the first dimension in the vector, based on ITEM_CLASS
    AHB_HEADER_DIM_QUALITY_SIZE = 2,        // The size of the second dimension in the vector, based on ITEM_QUALITY
    AHB_MAX_HEADERS
};

enum AHBotData      // Defines the actual structure size of each vector
{
    AHB_DATA_HEADER     = AHB_MAX_HEADERS,                                          // Number of headers per vector
    AHB_DATA_ROWS       = MAX_ITEM_QUALITY,                                           // Number of rows per vector
    AHB_DATA_COLUMNS    = MAX_ITEM_CLASS,                                         // Number of columns per vector
    AHB_DATA_UNIT       = ((AHB_DATA_COLUMNS * AHB_DATA_ROWS) + AHB_DATA_HEADER),  // Vector size including headers
    AHB_DATA_SIZE       = (AHB_DATA_UNIT * AHB_MAX_VECTORS)                         // Maximum data size
};

enum AHBotDataIndex // Defines the beginning index positions inside the 'data' DB field
{
    AHB_INDEX_PERCENTITEMS      = (AHB_VECTOR_ID_PERCENTITEMS * AHB_DATA_UNIT),
    AHB_INDEX_COUNTITEMS        = (AHV_VECTOR_ID_COUNTITEMS * AHB_DATA_UNIT),
    AHB_INDEX_MINPRICE          = (AHB_VECTOR_ID_MINPRICE * AHB_DATA_UNIT),
    AHB_INDEX_MAXPRICE          = (AHB_VECTOR_ID_MAXPRICE * AHB_DATA_UNIT),
    AHB_INDEX_MINBIDPRICE       = (AHB_VECTOR_ID_MINBIDPRICE * AHB_DATA_UNIT),
    AHB_INDEX_MAXBIDPRICE       = (AHB_VECTOR_ID_MAXBIDPRICE * AHB_DATA_UNIT),
    AHB_INDEX_MINSTACK          = (AHB_VECTOR_ID_MINSTACK * AHB_DATA_UNIT),
    AHB_INDEX_MAXSTACK          = (AHB_VECTOR_ID_MAXSTACK * AHB_DATA_UNIT),
    AHB_INDEX_BUYERPRICE        = (AHB_VECTOR_ID_BUYERPRICE * AHB_DATA_UNIT)
};

class AHBConfig
{
private:
    uint32 AHID;
    uint32 AHFID;
    uint32 AuctioneerGUID;

    uint32 minItems, maxItems;      // Defines min/max items - Only used if percentages are enabled
    uint32 minTime, maxTime;        // Defines min/max time an item is placed on the auction house

    uint32 buyerBiddingInterval;    // AHBuyer - defines how much time should pass between buying auctions from players
    uint32 buyerBidsPerInterval;    // AHBuyer - defines how many items the bot should place bids/buyouts on

    ACE_Vector<ACE_Vector<uint32, AHB_DATA_ROWS>, AHB_DATA_COLUMNS> percentItems;
    ACE_Vector<ACE_Vector<uint32, AHB_DATA_ROWS>, AHB_DATA_COLUMNS> countItems;
    ACE_Vector<ACE_Vector<uint32, AHB_DATA_ROWS>, AHB_DATA_COLUMNS> minPrice;
    ACE_Vector<ACE_Vector<uint32, AHB_DATA_ROWS>, AHB_DATA_COLUMNS> maxPrice;
    ACE_Vector<ACE_Vector<uint32, AHB_DATA_ROWS>, AHB_DATA_COLUMNS> minBidPrice;
    ACE_Vector<ACE_Vector<uint32, AHB_DATA_ROWS>, AHB_DATA_COLUMNS> maxBidPrice;
    ACE_Vector<ACE_Vector<uint32, AHB_DATA_ROWS>, AHB_DATA_COLUMNS> minStack;
    ACE_Vector<ACE_Vector<uint32, AHB_DATA_ROWS>, AHB_DATA_COLUMNS> maxStack;
    ACE_Vector<ACE_Vector<uint32, AHB_DATA_ROWS>, AHB_DATA_COLUMNS> buyerPrice;

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

    uint32 GetMinPrice(ItemClass itemClass, ItemQualities itemQuality) { return minPrice[itemClass][itemQuality]; }
    uint32 GetMaxPrice(ItemClass itemClass, ItemQualities itemQuality) { return maxPrice[itemClass][itemQuality]; }

    void SetMinBidPrice(ItemClass itemClass, ItemQualities itemQuality, uint32 value)
    {
        minBidPrice[itemClass][itemQuality] = ((value <= 99) && (value > 0)) ? ((value < maxBidPrice[itemClass][itemQuality]) ? value : (maxBidPrice[itemClass][itemQuality] - 1)) : 80;
    }
    uint32 GetMinBidPrice(ItemClass itemClass, ItemQualities itemQuality) { return minBidPrice[itemClass][itemQuality]; }

    void SetMaxBidPrice(ItemClass itemClass, ItemQualities itemQuality, uint32 value) {
        maxBidPrice[itemClass][itemQuality] = ((value <= 100) && (value > 1)) ? ((value > minBidPrice[itemClass][itemQuality]) ? value : (minBidPrice[itemClass][itemQuality] + 1)) : 100;
    }
    uint32 GetMaxBidPrice(ItemClass itemClass, ItemQualities itemQuality) { return maxBidPrice[itemClass][itemQuality]; }

    void SetMinStack(ItemClass itemClass, ItemQualities itemQuality, uint32 value) { minStack[itemClass][itemQuality] = (value > 0) ? ((value <= maxStack[itemClass][itemQuality]) ? value : maxStack[itemClass][itemQuality]) : 0; }
    uint32 GetMinStack(ItemClass itemClass, ItemQualities itemQuality) { return minStack[itemClass][itemQuality]; }

    void SetMaxStack(ItemClass itemClass, ItemQualities itemQuality, uint32 value) { maxStack[itemClass][itemQuality] = (value > 0) ? ((value >= minStack[itemClass][itemQuality]) ? value : minStack[itemClass][itemQuality]) : 0; }
    uint32 GetMaxStack(ItemClass itemClass, ItemQualities itemQuality) { return maxStack[itemClass][itemQuality]; }

    void SetBuyerPrice(ItemClass itemClass, ItemQualities itemQuality, uint32 value) { buyerPrice[itemClass][itemQuality] = (value > 0) ? value : 1; }
    uint32 GetBuyerPrice(ItemClass itemClass, ItemQualities itemQuality) { return buyerPrice[itemClass][itemQuality]; }

    void SetBiddingInterval(uint32 value) { buyerBiddingInterval = (value > 0) ? value : 1; }
    uint32 GetBiddingInterval() { return buyerBiddingInterval; }

    void SetBidsPerInterval(uint32 value) { buyerBidsPerInterval = (value > 0) ? value : 1; }
    uint32 GetBidsPerInterval() { return buyerBidsPerInterval; }

    void SetPercentage(ItemClass itemClass, ItemQualities itemQuality, uint32 value)
    {
        percentItems[itemClass][itemQuality] = value;
    }

    uint32 GetPercentage(ItemClass itemClass, ItemQualities itemQuality)
    {
        return percentItems[itemClass][itemQuality];
    }

    void SetCounts(ItemClass itemClass, ItemQualities itemQuality, uint32 value)
    {
        countItems[itemClass][itemQuality] = value;
    }

    uint32 GetCounts(ItemClass itemClass, ItemQualities itemQuality)
    {
        return countItems[itemClass][itemQuality];
    }

    void SetMinPrice(ItemClass itemClass, ItemQualities itemQuality, uint32 value)
    {
        minPrice[itemClass][itemQuality] = (value > 0) ? ((value < maxPrice[itemClass][itemQuality]) ? value : maxPrice[itemClass][itemQuality]) : 0;
    }

    void SetMaxPrice(ItemClass itemClass, ItemQualities itemQuality, uint32 value)
    {
        maxPrice[itemClass][itemQuality] = (value > 0) ? ((value > minPrice[itemClass][itemQuality]) ? value : minPrice[itemClass][itemQuality]) : 0;
    }

    uint32 GetItemCount(ItemClass itemClass, ItemQualities itemQuality)
    {
        return uint32(((double)percentItems[itemClass][itemQuality] / 100.0f) * maxItems);
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

    ACE_Vector<ACE_Vector<ACE_Vector<uint32>, MAX_ITEM_QUALITY>, MAX_ITEM_CLASS> binItems;

    void addNewAuctions(Player*, AHBConfig*);
    void addNewAuctionBuyerBotBid(Player*, AHBConfig*, WorldSession*);

    bool LoadValuesArrayFromDB(Tokens&, AHBConfig*);
    bool SaveValuesArrayInDB(Tokens const&, AHBConfig*);

    uint32 GetUInt32ValueFromDB(uint16 index, AHBConfig*);
    uint32 GetUInt32ValueFromArray(Tokens const&, uint16);
    void SetUInt32ValueInDB(uint16, uint32, AHBConfig*);
    uint32 minValue(uint32 a, uint32 b) { return (a <= b) ? a : b; };

    int lookupClass(char[]);
    const char* lookupClass(ItemClass);

    int lookupQuality(char[]);
    const char* lookupQuality(ItemQualities);

    int lookupCommand(char[]);

public:
    AuctionHouseBot();
    ~AuctionHouseBot();

    void Command(const char*, char*);

    uint32 GetAHBplayerGUID() { return AHBplayerGUID; };
    
    void Initialize();
    
    bool isValidMapID(uint32);
    
    bool LoadValues(AHBConfig*);
    
    void Update();
};

#define auctionbot MaNGOS::Singleton<AuctionHouseBot>::Instance()

#endif
