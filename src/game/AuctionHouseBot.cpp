#include "AuctionHouseBot.h"
#include "AuctionHouseMgr.h"
#include "ObjectMgr.h"

#include "Policies/SingletonImp.h"

INSTANTIATE_SINGLETON_1( AuctionHouseBot );

//using namespace std;

AuctionHouseBot::AuctionHouseBot()
{
    AHBSeller = 0;
    AHBBuyer = 0;

    Vendor_Items = 0;
    Loot_Items = 0;
    Other_Items = 0;

    No_Bind = 0;
    Bind_When_Picked_Up = 0;
    Bind_When_Equipped = 0;
    Bind_When_Use = 0;
    Bind_Quest_Item = 0;

    AllianceConfig = AHBConfig(2);
    HordeConfig = AHBConfig(6);
    NeutralConfig = AHBConfig(7);
}

AuctionHouseBot::~AuctionHouseBot()
{

}

void AuctionHouseBot::addNewAuctions(Player *AHBplayer, AHBConfig *config)
{
    if (!AHBSeller) // If the seller is disabled, lets exit this subroutine
        return;

    AuctionHouseEntry const* ahEntry = auctionmgr.GetAuctionHouseEntry(config->GetAHFID());
    AuctionHouseObject* auctionHouse = auctionmgr.GetAuctionsMap(config->GetAHFID());
    uint32 items = 0;
    uint32 minItems = config->GetMinItems();
    uint32 maxItems = config->GetMaxItems();
    uint32 auctions = auctionHouse->Getcount();
    uint32 AuctioneerGUID = config->GetAuctioneerGUID();

    if (auctions >= minItems)
        return;

    if (auctions <= maxItems)
    {
        if ((maxItems - auctions) > ItemsPerCycle)
            items = ItemsPerCycle;
        else
            items = (maxItems - auctions);
    }

    uint32 countConfigItems[MAX_ITEM_CLASS][MAX_ITEM_QUALITY];
    uint32 countActualItems[MAX_ITEM_CLASS][MAX_ITEM_QUALITY];

    std::vector<std::vector<uint8>> validChoiceItems;
    std::vector<uint8> validChoiceClasses;

    for (uint8 itemClass = 0; itemClass < MAX_ITEM_CLASS; ++itemClass)
    {
        for (uint8 itemQuality = 0; itemQuality < MAX_ITEM_QUALITY; ++itemQuality)
        {
            countConfigItems[itemClass][itemQuality] = config->GetItemCount(ItemClass(itemClass), ItemQualities(itemQuality));
            countActualItems[itemClass][itemQuality] = 0;
        }
    }

    for (AuctionHouseObject::AuctionEntryMap::const_iterator itr = auctionHouse->GetAuctionsBegin();itr != auctionHouse->GetAuctionsEnd();++itr)
    {
        AuctionEntry *auctionEntry = itr->second;
        Item *item = auctionmgr.GetAItem(auctionEntry->item_guidlow);
        if (!item)
        {
            sLog.outError("AuctionHouseBot: addNewAuctions() - !item");
            continue;
        }

        ItemPrototype const *itemPrototype = item->GetProto();
        if (!itemPrototype)
        {
            sLog.outError("AuctionHouseBot: addNewAuctions() - !itemPrototype");
            continue;
        }

        ++countActualItems[itemPrototype->Class][itemPrototype->Quality];
    }

    // only insert a few at a time, so as not to peg the processor
    for (uint32 count = 1; count <= items; ++count)
    {
        uint32 itemID = 0;      // Used to hold the itemID for the auction
        int vectorIndex = -1;   // Used to hold the index value of vectors

        // Empty out the choices at beginning of loop
        validChoiceItems.clear();
        validChoiceClasses.clear();

        // Populate valid choices for auctions based on item quality
        for (uint8 itemClass = 0; itemClass < MAX_ITEM_CLASS; ++itemClass)
        {
            bool bPushedClass = false;      // Used to detect if this itemClass has already been pushed into the vector
            for (uint8 itemQuality = 0; itemQuality < MAX_ITEM_QUALITY; ++itemQuality)
            {
                if ((binItems[itemClass][itemQuality].size() > 0) && (countActualItems[itemClass][itemQuality] < countConfigItems[itemClass][itemQuality]))
                {
                    if (!bPushedClass)
                    {
                        validChoiceItems.resize(validChoiceItems.size()+1); // Expand the vector by one
                        validChoiceClasses.push_back(itemClass);            // Add the itemClass valid choice
                        bPushedClass = true;                                // We added a new vector choice value so it becomes true
                        ++vectorIndex;                                      // Increment the index by 1
                    }
                    validChoiceItems[vectorIndex].push_back(itemQuality);   // Add the itemQuality valid choice
                }
            }
        }

        // Check to see if theres any valid choices to choose from.
        if (validChoiceClasses.empty())
        {
            if(debugOut)
                sLog.outDebug("AuctionHouseBot: addNewAuctions() - No valid choices");
            return;
        }

        // Since there must be a valid choice, lets add an item
        vectorIndex = urand(0, validChoiceClasses.size() - 1);
        ItemClass itemClass = ItemClass(validChoiceClasses[vectorIndex]);
        ItemQualities itemQuality = ItemQualities(validChoiceItems[vectorIndex][urand(0, validChoiceItems[vectorIndex].size() - 1)]);
        itemID = binItems[itemClass][itemQuality][urand(0, binItems[itemClass][itemQuality].size() - 1)];
        ++countActualItems[itemClass][itemQuality];

        ItemPrototype const* itemPrototype = objmgr.GetItemPrototype(itemID);
        if (itemPrototype == NULL)
        {
            sLog.outError("AuctionHouseBot: addNewAuctions() - itemPrototype == NULL");
            continue;
        }

        Item* item = Item::CreateItem(itemID, 1, AHBplayer);
        item->AddToUpdateQueueOf(AHBplayer);
        if (item == NULL)
        {
            sLog.outError("AuctionHouseBot: addNewAuctions() - item == NULL");
            break;
        }

        uint32 randomPropertyId = Item::GenerateItemRandomPropertyId(itemID);
        if (randomPropertyId != 0)
            item->SetItemRandomProperties(randomPropertyId);

        uint32 buyoutPrice;
        uint32 bidPrice = 0;
        uint32 stackCount = urand(1, item->GetMaxStackCount());

        switch (SellMethod)
        {
        case 0:
            buyoutPrice  = itemPrototype->SellPrice * item->GetCount();
            break;
        case 1:
            buyoutPrice  = itemPrototype->BuyPrice * item->GetCount();
            break;
        }

        if (config->GetMaxStack(ItemClass(itemPrototype->Class), ItemQualities(itemPrototype->Quality)) != 0)
            stackCount = urand(1, minValue(item->GetMaxStackCount(), urand(config->GetMinStack(ItemClass(itemPrototype->Class), ItemQualities(itemPrototype->Quality)), config->GetMaxStack(ItemClass(itemPrototype->Class), ItemQualities(itemPrototype->Quality)))));
        buyoutPrice *= urand(config->GetMinPrice(ItemClass(itemPrototype->Class), ItemQualities(itemPrototype->Quality)), config->GetMaxPrice(ItemClass(itemPrototype->Class), ItemQualities(itemPrototype->Quality))) * stackCount;
        buyoutPrice /= 100;
        bidPrice = buyoutPrice * urand(config->GetMinBidPrice(ItemClass(itemPrototype->Class), ItemQualities(itemPrototype->Quality)), config->GetMaxBidPrice(ItemClass(itemPrototype->Class), ItemQualities(itemPrototype->Quality)));
        bidPrice /= 100;

        item->SetCount(stackCount);

        AuctionEntry* auctionEntry = new AuctionEntry;
        auctionEntry->Id = objmgr.GenerateAuctionID();
        auctionEntry->auctioneer = AuctioneerGUID;
        auctionEntry->item_guidlow = item->GetGUIDLow();
        auctionEntry->item_template = item->GetEntry();
        auctionEntry->owner = AHBplayer->GetGUIDLow();
        auctionEntry->startbid = bidPrice;
        auctionEntry->buyout = buyoutPrice;
        auctionEntry->bidder = 0;
        auctionEntry->bid = 0;
        auctionEntry->deposit = 0;
        auctionEntry->expire_time = (time_t) (urand(config->GetMinTime(), config->GetMaxTime()) * 60 * 60 + time(NULL));
        auctionEntry->auctionHouseEntry = ahEntry;
        item->SaveToDB();
        item->RemoveFromUpdateQueueOf(AHBplayer);
        auctionmgr.AddAItem(item);
        auctionHouse->AddAuction(auctionEntry);
        auctionEntry->SaveToDB();
    }
}

void AuctionHouseBot::addNewAuctionBuyerBotBid(Player *AHBplayer, AHBConfig *config, WorldSession *session)
{
    if (!AHBBuyer)
        return;

    // Fetches content of selected AH
    AuctionHouseObject* auctionHouseObject = auctionmgr.GetAuctionsMap(config->GetAHFID());
    std::vector<uint32> possibleBids;

    for (AuctionHouseObject::AuctionEntryMap::const_iterator itr = auctionHouseObject->GetAuctionsBegin(); itr != auctionHouseObject->GetAuctionsEnd(); ++itr)
    {
        // Check if the auction is ours
        // if it is, we skip this iteration.
        if (itr->second->owner == AHBplayerGUID)
            continue;

        // Check that we haven't bidded in this auction already.
        if (itr->second->bidder != AHBplayerGUID)
            possibleBids.push_back(itr->second->Id);
    }

    for (uint32 count = 1; count < config->GetBidsPerInterval(); ++count)
    {

        // Do we have anything to bid? If not, stop here.
        if (possibleBids.empty())
        {
            count = config->GetBidsPerInterval();
            continue;
        }

        // Choose random auction from possible auctions
        uint32 vectorPos = urand(0, possibleBids.size() - 1);
        uint32 auctionID = possibleBids[vectorPos];

        // Erase the auction from the vector to prevent bidding on item in next iteration.
        std::vector<uint32>::iterator itr = possibleBids.begin();
        advance(itr, vectorPos);
        possibleBids.erase(itr);

        // from auctionhousehandler.cpp, creates auction pointer & player pointer
        AuctionEntry* auctionEntry = auctionHouseObject->GetAuction(auctionID);
        if (!auctionEntry)
        {
            if (debugOut)
                sLog.outDebug("AuctionHouseBot: addNewAuctionBuyerBotBid() - (!auctionEntry) Item doesn't exists, perhaps bought already?");
            continue;
        }

        // get exact item information
        Item *item = auctionmgr.GetAItem(auctionEntry->item_guidlow);
        if (!item)
        {
            if (debugOut)
                sLog.outDebug("AuctionHouseBot: addNewAuctionBuyerBotBid() - (!item) Item doesn't exists, perhaps bought already?");
            continue;
        }

        // get item prototype
        ItemPrototype const* itemPrototype = objmgr.GetItemPrototype(auctionEntry->item_template);

        // check which price we have to use, startbid or if it is bidded already
        if (debugOut)
        {
            sLog.outDebug("AuctionHouseBot: addNewAuctionBuyerBotBid() - Auction Number: %u", auctionEntry->Id);
            sLog.outDebug("AuctionHouseBot: addNewAuctionBuyerBotBid() - Item Template : %u", auctionEntry->item_template);
            sLog.outDebug("AuctionHouseBot: addNewAuctionBuyerBotBid() - Buy Price     : %u", itemPrototype->BuyPrice);
            sLog.outDebug("AuctionHouseBot: addNewAuctionBuyerBotBid() - Sell Price    : %u", itemPrototype->SellPrice);
            sLog.outDebug("AuctionHouseBot: addNewAuctionBuyerBotBid() - Quality       : %u", itemPrototype->Quality);
        }

        uint32 currentAuctionPrice;
        if (auctionEntry->bid)
        {
            currentAuctionPrice = auctionEntry->bid;
            if (debugOut)
            {
                sLog.outDebug("AuctionHouseBot: addNewAuctionBuyerBotBid() - Current Price : %u", auctionEntry->bid);
            }
        }
        else
        {
            currentAuctionPrice = auctionEntry->startbid;
            if (debugOut)
            {
                sLog.outDebug("AuctionHouseBot: addNewAuctionBuyerBotBid() - Current Price : %u", auctionEntry->startbid);
            }
        }

        // Initialize bidMax variable
        long double bidMax = 0;

        // Check that bid has acceptable value and take bid based on vendorprice, stacksize and quality
        switch (BuyMethod)
        {
        case 0:
            if (currentAuctionPrice < itemPrototype->SellPrice * item->GetCount() * config->GetBuyerPrice(ItemClass(itemPrototype->Class), ItemQualities(itemPrototype->Quality)))
                bidMax = itemPrototype->SellPrice * item->GetCount() * config->GetBuyerPrice(ItemClass(itemPrototype->Class), ItemQualities(itemPrototype->Quality));
            break;
        case 1:
            if (currentAuctionPrice < itemPrototype->BuyPrice * item->GetCount() * config->GetBuyerPrice(ItemClass(itemPrototype->Class), ItemQualities(itemPrototype->Quality)))
                bidMax = itemPrototype->BuyPrice * item->GetCount() * config->GetBuyerPrice(ItemClass(itemPrototype->Class), ItemQualities(itemPrototype->Quality));
            break;
        }

        if (debugOut)
            sLog.outDebug("AuctionHouseBot: addNewAuctionBuyerBotBid() - bidMax: %f", bidMax);

        // check some special items, and do recalculating to their prices
        switch (ItemClass(itemPrototype->Class))
        {
        case ITEM_CLASS_PROJECTILE:     // Dont bid on ammunition
            bidMax = 0;
            continue;
        default:
            break;
        }

        if (bidMax == 0)
        {
            // quality check failed to get bidmax, let's get out of here
            if (debugOut)
                sLog.outDebug("AuctionHouseBot: addNewAuctionBuyerBotBid() - bidMax == 0");
            continue;
        }

        // Calculate our bid
        uint32 bidprice = uint32(currentAuctionPrice + ((bidMax - currentAuctionPrice) * float(urand(0,100) / 100)));
        if (debugOut)
            sLog.outDebug("AuctionHouseBot: addNewAuctionBuyerBotBid() - bidprice: %u", bidprice);

        // Check our bid is high enough to be valid. If not, correct it to minimum.
        if ((currentAuctionPrice + auctionEntry->GetAuctionOutBid()) > bidprice)
        {
            bidprice = currentAuctionPrice + auctionEntry->GetAuctionOutBid();
            if (debugOut)
                sLog.outDebug("AuctionHouseBot: addNewAuctionBuyerBotBid() - bidprice(>): %u", bidprice);
        }

        // Check for normal bid versus buyout
        if ((bidprice < auctionEntry->buyout) || (auctionEntry->buyout == 0))
        {
            // Send mail to last bidder and return money
            if (auctionEntry->bidder != AHBplayer->GetGUIDLow())
                session->SendAuctionOutbiddedMail(auctionEntry , bidprice);

            auctionEntry->bidder = AHBplayer->GetGUIDLow();
            auctionEntry->bid = bidprice;

            // Saving auction into database
            CharacterDatabase.PExecute("UPDATE auctionhouse SET buyguid = '%u',lastbid = '%u' WHERE id = '%u'", auctionEntry->bidder, auctionEntry->bid, auctionEntry->Id);
        }
        else
        {
            // Do buyout
            if (AHBplayer->GetGUIDLow() != auctionEntry->bidder)
                session->SendAuctionOutbiddedMail(auctionEntry, auctionEntry->buyout);

            auctionEntry->bidder = AHBplayer->GetGUIDLow();
            auctionEntry->bid = auctionEntry->buyout;

            // Send mails to buyer & seller
            auctionmgr.SendAuctionSuccessfulMail(auctionEntry);
            auctionmgr.SendAuctionWonMail(auctionEntry);

            // Remove item from auctionhouse
            auctionmgr.RemoveAItem(auctionEntry->item_guidlow);
            // Remove auction
            auctionHouseObject->RemoveAuction(auctionEntry->Id);
            // Remove from database
            auctionEntry->DeleteFromDB();
        }
    }
}

void AuctionHouseBot::Command(AHBotCommands command, uint32 ahMapID)
{
    AHBConfig *config;

    switch (ahMapID)
    {
    case 2:
        config = &AllianceConfig;
        break;
    case 6:
        config = &HordeConfig;
        break;
    case 7:
        config = &NeutralConfig;
        break;
    default:
        sLog.outError("AuctionHouseBot: Commands(2) - switch(ahMapID) default reached");
        return;
    }

    switch (command)
    {
    case AHB_CMD_AHEXPIRE:
        {
            AuctionHouseObject* auctionHouse = auctionmgr.GetAuctionsMap(config->GetAHFID());

            AuctionHouseObject::AuctionEntryMap::iterator itr;
            itr = auctionHouse->GetAuctionsBegin();

            while (itr != auctionHouse->GetAuctionsEnd())
            {
                if (itr->second->owner == AHBplayerGUID)
                    itr->second->expire_time = sWorld.GetGameTime();

                ++itr;
            }
        }
        break;
    default:
        sLog.outError("AuctionHouseBot: Commands(2) - default reached");
        break;
    }
}

void AuctionHouseBot::Command(AHBotCommands command, uint32 ahMapID, char* args)
{
    AHBConfig *config;

    switch (ahMapID)
    {
    case 2:
        config = &AllianceConfig;
        break;
    case 6:
        config = &HordeConfig;
        break;
    case 7:
        config = &NeutralConfig;
        break;
    default:
        sLog.outError("AuctionHouseBot: Commands(3) - switch(ahMapID) default reached");
        return;
    }

    switch (command)
    {
    case AHB_CMD_MINITEMS:
        {
            char * param1 = strtok(args, " ");
            uint32 minItems = (uint32) strtoul(param1, NULL, 0);
            CharacterDatabase.PExecute("UPDATE auctionhousebot SET minitems = '%u' WHERE auctionhouse = '%u'", minItems, ahMapID);
            config->SetMinItems(minItems);
        }
        break;
    case AHB_CMD_MAXITEMS:
        {
            char * param1 = strtok(args, " ");
            uint32 maxItems = (uint32) strtoul(param1, NULL, 0);
            CharacterDatabase.PExecute("UPDATE auctionhousebot SET maxitems = '%u' WHERE auctionhouse = '%u'", maxItems, ahMapID);
            config->SetMaxItems(maxItems);
        }
        break;
    case AHB_CMD_MINTIME:
        {
            char * param1 = strtok(args, " ");
            uint32 minTime = (uint32) strtoul(param1, NULL, 0);
            CharacterDatabase.PExecute("UPDATE auctionhousebot SET mintime = '%u' WHERE auctionhouse = '%u'", minTime, ahMapID);
            config->SetMinTime(minTime);
        }
        break;
    case AHB_CMD_MAXTIME:
        {
            char * param1 = strtok(args, " ");
            uint32 maxTime = (uint32) strtoul(param1, NULL, 0);
            CharacterDatabase.PExecute("UPDATE auctionhousebot SET maxtime = '%u' WHERE auctionhouse = '%u'", maxTime, ahMapID);
            config->SetMaxTime(maxTime);
        }
        break;
    case AHB_CMD_BIDINTERVAL:
        {
            char * param1 = strtok(args, " ");
            uint32 bidInterval = (uint32) strtoul(param1, NULL, 0);
            CharacterDatabase.PExecute("UPDATE auctionhousebot SET buyerbiddinginterval = '%u' WHERE auctionhouse = '%u'", bidInterval, ahMapID);
            config->SetBiddingInterval(bidInterval);
        }
        break;
    case AHB_CMD_BIDSPERINTERVAL:
        {
            char * param1 = strtok(args, " ");
            uint32 bidsPerInterval = (uint32) strtoul(param1, NULL, 0);
            CharacterDatabase.PExecute("UPDATE auctionhousebot SET buyerbidsperinterval = '%u' WHERE auctionhouse = '%u'", bidsPerInterval, ahMapID);
            config->SetBidsPerInterval(bidsPerInterval);
        }
        break;
    default:
        sLog.outError("AuctionHouseBot: Commands(3) - default reached");
        break;
    }
}

void AuctionHouseBot::Command(AHBotCommands command, uint32 ahMapID, char* args, ItemClass itemClass, ItemQualities itemQuality)
{
    AHBConfig *config;

    switch (ahMapID)
    {
    case 2:
        config = &AllianceConfig;
        break;
    case 6:
        config = &HordeConfig;
        break;
    case 7:
        config = &NeutralConfig;
        break;
    default:
        sLog.outError("AuctionHouseBot: Commands(4) - switch(ahMapID) default reached");
        return;
    }

    switch (command)
    {
    case AHB_CMD_PERCENTAGES:
        {
            Tokens data = StrSplit(args, " ");
            int index = 0;

            for (Tokens::const_iterator iter = data.begin(); iter != data.end(); ++iter, ++index)
            {
                SetUInt32ValueInDB((AHB_INDEX_PERCENTITEMS + AHB_DATA_HEADER + index), uint32(atoi(iter->c_str())), config);
            }
        }
        break;
    case AHB_CMD_MINPRICE:
        {
            char * param1 = strtok(args, " ");
            uint32 minPrice = (uint32) strtoul(param1, NULL, 0);
            SetUInt32ValueInDB((AHB_INDEX_MINPRICE + AHB_DATA_HEADER + (AHB_DATA_COLUMNS * itemClass) + itemQuality), minPrice, config);
            config->SetMinPrice(itemClass, itemQuality, minPrice);
        }
        break;
    case AHB_CMD_MAXPRICE:
        {
            char * param1 = strtok(args, " ");
            uint32 maxPrice = (uint32) strtoul(param1, NULL, 0);
            SetUInt32ValueInDB((AHB_INDEX_MAXPRICE + AHB_DATA_HEADER + (AHB_DATA_COLUMNS * itemClass) + itemQuality), maxPrice, config);
            config->SetMaxPrice(itemClass, itemQuality, maxPrice);
        }
        break;
    case AHB_CMD_MINBIDPRICE:
        {
            char * param1 = strtok(args, " ");
            uint32 minBidPrice = (uint32) strtoul(param1, NULL, 0);
            SetUInt32ValueInDB((AHB_INDEX_MINBIDPRICE + AHB_DATA_HEADER + (AHB_DATA_COLUMNS * itemClass) + itemQuality), minBidPrice, config);
            config->SetMinBidPrice(itemClass, itemQuality, minBidPrice);
        }
        break;
    case AHB_CMD_MAXBIDPRICE:
        {
            char * param1 = strtok(args, " ");
            uint32 maxBidPrice = (uint32) strtoul(param1, NULL, 0);
            SetUInt32ValueInDB((AHB_INDEX_MAXBIDPRICE + AHB_DATA_HEADER + (AHB_DATA_COLUMNS * itemClass) + itemQuality), maxBidPrice, config);
            config->SetMaxBidPrice(itemClass, itemQuality, maxBidPrice);
        }
        break;
    case AHB_CMD_MAXSTACK:
        {
            char * param1 = strtok(args, " ");
            uint32 maxStack = (uint32) strtoul(param1, NULL, 0);
            SetUInt32ValueInDB((AHB_INDEX_MAXSTACK + AHB_DATA_HEADER + (AHB_DATA_COLUMNS * itemClass) + itemQuality), maxStack, config);
            config->SetMaxStack(itemClass, itemQuality, maxStack);
        }
        break;
    case AHB_CMD_BUYERPRICE:
        {
            char * param1 = strtok(args, " ");
            uint32 buyerPrice = (uint32) strtoul(param1, NULL, 0);
            SetUInt32ValueInDB((AHB_INDEX_BUYERPRICE + AHB_DATA_HEADER + (AHB_DATA_COLUMNS * itemClass) + itemQuality), buyerPrice, config);
            config->SetBuyerPrice(itemClass, itemQuality, buyerPrice);
        }
        break;
    default:
        sLog.outError("AuctionHouseBot: Commands(4) - default reached");
        break;
    }
}

void AuctionHouseBot::Initialize()
{
    AHBSeller = sConfig.GetBoolDefault("AuctionHouseBot.EnableSeller", 0);
    AHBBuyer = sConfig.GetBoolDefault("AuctionHouseBot.EnableBuyer", 0);
    AHBplayerAccount = sConfig.GetIntDefault("AuctionHouseBot.Account", 0);
    AHBplayerGUID = sConfig.GetIntDefault("AuctionHouseBot.GUID", 0);
    debugOut = sConfig.GetIntDefault("AuctionHouseBot.DEBUG", 0);
    No_Bind = sConfig.GetBoolDefault("AuctionHouseBot.No_Bind", 1);
    Bind_When_Picked_Up = sConfig.GetBoolDefault("AuctionHouseBot.Bind_When_Picked_Up", 0);
    Bind_When_Equipped = sConfig.GetBoolDefault("AuctionHouseBot.Bind_When_Equipped", 1);
    Bind_When_Use = sConfig.GetBoolDefault("AuctionHouseBot.Bind_When_Use", 1);
    Bind_Quest_Item = sConfig.GetBoolDefault("AuctionHouseBot.Bind_Quest_Item", 0);
    ItemsPerCycle = sConfig.GetIntDefault("AuctionHouseBot.ItemsPerCycle", 200);
    SellMethod = sConfig.GetIntDefault("AuctionHouseBot.UseBuyPriceForSeller", 1);
    BuyMethod = sConfig.GetIntDefault("AuctionHouseBot.UseBuyPriceForBuyer", 0);

    if (!sWorld.getConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_AUCTION))
    {
        if (!LoadValues(&AllianceConfig))
            sLog.outError("AuctionHouseBot: Initialization() - Load Alliance Config Failed");
        if (!LoadValues(&HordeConfig))
            sLog.outError("AuctionHouseBot: Initialization() - Load Horde Config Failed");
    }

    if(!LoadValues(&NeutralConfig))
        sLog.outError("AuctionHouseBot: Initialization() - Load Neutral Config Failed");

    if (AHBSeller)
    {
        Vendor_Items = sConfig.GetBoolDefault("AuctionHouseBot.VendorItems", 0);
        Loot_Items = sConfig.GetBoolDefault("AuctionHouseBot.LootItems", 1);
        Other_Items = sConfig.GetBoolDefault("AuctionHouseBot.OtherItems", 0);

        QueryResult* results = (QueryResult*) NULL;
        char npcQuery[] = "SELECT distinct `item` FROM `npc_vendor`";
        results = WorldDatabase.PQuery(npcQuery);

        uint32 binSize = 0;

        if (results != NULL)
        {
            do
            {
                Field* fields = results->Fetch();
                npcItems.push_back(fields[0].GetUInt32());

            } while (results->NextRow());

            delete results;
        }
        else
        {
            sLog.outError("AuctionHouseBot: Initialize() - \"%s\" failed", npcQuery);
        }

        char lootQuery[] = "SELECT `item` FROM `creature_loot_template` UNION "
            "SELECT `item` FROM `disenchant_loot_template` UNION "
            "SELECT `item` FROM `fishing_loot_template` UNION "
            "SELECT `item` FROM `gameobject_loot_template` UNION "
            "SELECT `item` FROM `item_loot_template` UNION "
            "SELECT `item` FROM `milling_loot_template` UNION "
            "SELECT `item` FROM `pickpocketing_loot_template` UNION "
            "SELECT `item` FROM `prospecting_loot_template` UNION "
            "SELECT `item` FROM `skinning_loot_template`";
        results = WorldDatabase.PQuery(lootQuery);

        if (results != NULL)
        {
            do
            {
                Field* fields = results->Fetch();
                lootItems.push_back(fields[0].GetUInt32());

            } while (results->NextRow());

            delete results;
        }
        else
        {
            sLog.outError("AuctionHouseBot: Initialize() - \"%s\" failed", lootQuery);
        }

        for (uint32 itemID = 0; itemID < sItemStorage.MaxEntry; itemID++)
        {
            ItemPrototype const* prototype = objmgr.GetItemPrototype(itemID);

            if (prototype == NULL)
            {
                //sLog.outError("AuctionHouseBot: Initialize() - prototype == NULL");
                continue;
            }

            switch (prototype->Bonding)
            {
            case NO_BIND:
                if (!No_Bind)
                    continue;
                break;
            case BIND_WHEN_PICKED_UP:
                if (!Bind_When_Picked_Up)
                    continue;
                break;
            case BIND_WHEN_EQUIPED:
                if (!Bind_When_Equipped)
                    continue;
                break;
            case BIND_WHEN_USE:
                if (!Bind_When_Use)
                    continue;
                break;
            case BIND_QUEST_ITEM:
                if (!Bind_Quest_Item)
                    continue;
                break;
            case BIND_QUEST_ITEM1:
                if (debugOut)
                    sLog.outDebug("AuctionHouseBot: Initialize() - switch(prototype->Bonding) BIND_QUEST_ITEM1 detected, ignoring item");
                continue;
            default:
                sLog.outError("AuctionHouseBot: Initialize() - switch(prototype->Bonding) default reached");
                continue;
            }

            switch (SellMethod)
            {
            case 0:
                if (prototype->SellPrice == 0)
                    continue;
                break;
            case 1:
                if (prototype->BuyPrice == 0)
                    continue;
                break;
            }

            if ((prototype->Quality < 0) || (prototype->Quality >= MAX_ITEM_QUALITY))
            {
                sLog.outError("AuctionHouseBot: Initialize() - prototype->Quality negative or greater/equal than MAX_ITEM_QUALITY");
                continue;
            }

            bool isVendorItem = false;
            bool isLootItem = false;

            for (uint32 i = 0; i < npcItems.size(); ++i)
                if (itemID == npcItems[i])
                    isVendorItem = true;

            for (uint32 i = 0; i < lootItems.size(); ++i)
                if (itemID == lootItems[i])
                    isLootItem = true;

            if (Vendor_Items == 0 && isVendorItem)
                continue;

            if (Loot_Items == 0 && isLootItem)
                continue;

            if (Other_Items == 0 && !isLootItem && !isVendorItem)
                continue;

            binItems[prototype->Class][prototype->Quality].push_back(itemID);
            ++binSize;
        }

        if (!binSize)
        {
            sLog.outString("AuctionHouseBot: No items");
            AHBSeller = 0;
        }

        sLog.outString("AuctionHouseBot:");
        for (uint8 itemClass = 0; itemClass < MAX_ITEM_CLASS; ++ itemClass)
        {
            sLog.outString("Item Class: %s", lookupClass(ItemClass(itemClass)));
            for (uint8 itemQuality = 0; itemQuality < MAX_ITEM_QUALITY; ++itemQuality)
            {
                if (itemQuality == MAX_ITEM_QUALITY - 1)
                    sLog.outString("  \\--> loaded %d %s item(s)", binItems[itemClass][itemQuality].size(), lookupQuality(ItemQualities(itemQuality)));
                else
                    sLog.outString("  |--> loaded %d %s item(s)", binItems[itemClass][itemQuality].size(), lookupQuality(ItemQualities(itemQuality)));
            }
        }
    }
    sLog.outString("AuctionHouseBot "AHB_REVISION" is now loaded");
    sLog.outString("AuctionHouseBot updated Naicisum (original by ChrisK and Paradox)");
    sLog.outString("AuctionHouseBot now includes AHBuyer by Kerbe and Paradox");
}

bool AuctionHouseBot::SaveValuesArrayInDB(Tokens const& data, AHBConfig* config)
{
    std::ostringstream ss2;
    ss2<<"UPDATE auctionhousebot SET data='";
    int i=0;
    for (Tokens::const_iterator iter = data.begin(); iter != data.end(); ++iter, ++i)
    {
        ss2<<data[i]<<" ";
    }
    ss2<<"' WHERE auctionhouse = '"<< config->GetAHID() <<"'";

    return CharacterDatabase.Execute(ss2.str().c_str());
}

bool AuctionHouseBot::LoadValuesArrayFromDB(Tokens& data, AHBConfig* config)
{
    QueryResult *result = CharacterDatabase.PQuery("SELECT data FROM auctionhousebot WHERE auctionhouse = '%u'",config->GetAHID());
    if( !result )
        return false;

    Field *fields = result->Fetch();

    data = StrSplit(fields[0].GetCppString(), " ");

    delete result;

    return true;
}

uint32 AuctionHouseBot::GetUInt32ValueFromDB(uint16 index, AHBConfig* config)
{
    Tokens data;
    if(!LoadValuesArrayFromDB(data,config))
        return 0;

    return GetUInt32ValueFromArray(data,index);
}

uint32 AuctionHouseBot::GetUInt32ValueFromArray(Tokens const& data, uint16 index)
{
    if(index >= data.size())
        return 0;

    return (uint32)atoi(data[index].c_str());
}

void AuctionHouseBot::SetUInt32ValueInDB(uint16 index, uint32 value, AHBConfig* config)
{
    Tokens data;
    if(!LoadValuesArrayFromDB(data,config))
        return;

    if(index >= data.size())
        return;

    char buf[11];
    snprintf(buf,11,"%u",value);
    data[index] = buf;

    SaveValuesArrayInDB(data,config);
}

bool AuctionHouseBot::LoadValues(AHBConfig *config)
{
    // Load all non-combined values seperated values
    config->SetMinItems(CharacterDatabase.PQuery("SELECT minitems FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32());
    config->SetMaxItems(CharacterDatabase.PQuery("SELECT maxitems FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32());
    config->SetMinTime(CharacterDatabase.PQuery("SELECT mintime FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32());
    config->SetMaxTime(CharacterDatabase.PQuery("SELECT maxtime FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32());
    config->SetBiddingInterval(CharacterDatabase.PQuery("SELECT buyerbiddinginterval FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32());
    config->SetBidsPerInterval(CharacterDatabase.PQuery("SELECT buyerbidsperinterval FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32());

    if (debugOut)
    {
        sLog.outDebug("AuctionHouseBot: LoadValues() - minItems = %u", config->GetMinItems());
        sLog.outDebug("AuctionHouseBot: LoadValues() - maxItems = %u", config->GetMaxItems());
        sLog.outDebug("AuctionHouseBot: LoadValues() - minTime = %u", config->GetMinTime());
        sLog.outDebug("AuctionHouseBot: LoadValues() - maxTime = %u", config->GetMaxTime());
        sLog.outDebug("AuctionHouseBot: LoadValues() - buyerBiddingInterval = %u", config->GetBiddingInterval());
        sLog.outDebug("AuctionHouseBot: LoadValues() - buyerBidsPerInterval = %u", config->GetBidsPerInterval());
    }

    // Fetch data from database and populate all vectors
    Tokens data;
    if (!LoadValuesArrayFromDB(data, config))
    {
        sLog.outError("AuctionHouseBot: LoadValues() - Error loading 'data' for AHID '%u'", config->GetAHID());
        AHBSeller = FALSE;
        AHBBuyer = FALSE;
        return false;
    }

    // Perform consistency check on data
    if (data.size() != AHB_DATA_SIZE)
    {
        sLog.outError("AuctionHouseBot: LoadValues() - Error loading 'data', inconsistency detected");
        AHBSeller = FALSE;
        AHBBuyer = FALSE;
        return false;
    }

    // Load data into vectors
    for (Tokens::const_iterator iter = data.begin(); iter != data.end();)
    {
        AHBotVectorIds vectorId = AHBotVectorIds(atoi(iter++->c_str()));    // First value is always ID field
        int vectorColumn = atoi(iter++->c_str());                           // Second value is always COLUMN size field
        int vectorRow = atoi(iter++->c_str());                              // Third value is always ROW size field

        switch(vectorId)
        {
        case AHB_VECTOR_ID_PERCENTITEMS:
            for(uint8 itemClass = 0; itemClass < vectorColumn; ++itemClass, ++iter)
            {
                for(int itemQuality = 0; itemQuality < vectorRow; ++itemQuality, ++iter)
                {
                    config->SetPercentage(ItemClass(itemClass), ItemQualities(itemQuality), uint32(atoi(iter->c_str())));
                    if (debugOut)
                        sLog.outDebug("AuctionHouseBot: LoadValues() - percentTradeGoods[%u][%u] = %u", itemClass, itemQuality, config->GetPercentage(ItemClass(itemClass), ItemQualities(itemQuality)));
                }
            }
            break;
        case AHV_VECTOR_ID_COUNTITEMS:
           for(uint8 itemClass = 0; itemClass < vectorColumn; ++itemClass, ++iter)
            {
                for(int itemQuality = 0; itemQuality < vectorRow; ++itemQuality, ++iter)
                {
                    config->SetCounts(ItemClass(itemClass), ItemQualities(itemQuality), uint32(atoi(iter->c_str())));
                    if (debugOut)
                        sLog.outDebug("AuctionHouseBot: LoadValues() - percentTradeGoods[%u][%u] = %u", itemClass, itemQuality, config->GetCounts(ItemClass(itemClass), ItemQualities(itemQuality)));
                }
            }
            break;
        case AHB_VECTOR_ID_MINPRICE:
            for(uint8 itemClass = 0; itemClass < vectorColumn; ++itemClass, ++iter)
            {
                for(int itemQuality = 0; itemQuality < vectorRow; ++itemQuality, ++iter)
                {
                    config->SetMinPrice(ItemClass(itemClass), ItemQualities(itemQuality), uint32(atoi(iter->c_str())));
                    if (debugOut)
                        sLog.outDebug("AuctionHouseBot: LoadValues() - minPrice[%u][%u]          = %u", itemClass, itemQuality, config->GetMinPrice(ItemClass(itemClass), ItemQualities(itemQuality)));
                }
            }
            break;
        case AHB_VECTOR_ID_MAXPRICE:
            for(uint8 itemClass = 0; itemClass < vectorColumn; ++itemClass, ++iter)
            {
                for(int itemQuality = 0; itemQuality < vectorRow; ++itemQuality, ++iter)
                {
                    config->SetMaxPrice(ItemClass(itemClass), ItemQualities(itemQuality), uint32(atoi(iter->c_str())));
                    if (debugOut)
                        sLog.outDebug("AuctionHouseBot: LoadValues() - maxPrice[%u][%u]          = %u", itemClass, itemQuality, config->GetMaxPrice(ItemClass(itemClass), ItemQualities(itemQuality)));
                }
            }
            break;
        case AHB_VECTOR_ID_MINBIDPRICE:
            for(uint8 itemClass = 0; itemClass < vectorColumn; ++itemClass, ++iter)
            {
                for(int itemQuality = 0; itemQuality < vectorRow; ++itemQuality, ++iter)
                {
                    config->SetMinBidPrice(ItemClass(itemClass), ItemQualities(itemQuality), uint32(atoi(iter->c_str())));
                    if (debugOut)
                        sLog.outDebug("AuctionHouseBot: LoadValues() - minBidPrice[%u][%u]       = %u", itemClass, itemQuality, config->GetMinBidPrice(ItemClass(itemClass), ItemQualities(itemQuality)));
                }
            }
            break;
        case AHB_VECTOR_ID_MAXBIDPRICE:
            for(uint8 itemClass = 0; itemClass < vectorColumn; ++itemClass, ++iter)
            {
                for(int itemQuality = 0; itemQuality < vectorRow; ++itemQuality, ++iter)
                {
                    config->SetMaxBidPrice(ItemClass(itemClass), ItemQualities(itemQuality), uint32(atoi(iter->c_str())));
                    if (debugOut)
                        sLog.outDebug("AuctionHouseBot: LoadValues() - maxBidPrice[%u][%u]       = %u", itemClass, itemQuality, config->GetMaxBidPrice(ItemClass(itemClass), ItemQualities(itemQuality)));
                }
            }
            break;
        case AHB_VECTOR_ID_MAXSTACK:
            for(uint8 itemClass = 0; itemClass < vectorColumn; ++itemClass, ++iter)
            {
                for(int itemQuality = 0; itemQuality < vectorRow; ++itemQuality, ++iter)
                {
                    config->SetMaxStack(ItemClass(itemClass), ItemQualities(itemQuality), uint32(atoi(iter->c_str())));
                    if (debugOut)
                        sLog.outDebug("AuctionHouseBot: LoadValues() - maxStack[%u][%u]          = %u", itemClass, itemQuality, config->GetMaxStack(ItemClass(itemClass), ItemQualities(itemQuality)));
                }
            }
            break;
        case AHB_VECTOR_ID_BUYERPRICE:
            for(uint8 itemClass = 0; itemClass < vectorColumn; ++itemClass, ++iter)
            {
                for(int itemQuality = 0; itemQuality < vectorRow; ++itemQuality, ++iter)
                {
                    config->SetBuyerPrice(ItemClass(itemClass), ItemQualities(itemQuality), uint32(atoi(iter->c_str())));
                    if (debugOut)
                        sLog.outDebug("AuctionHouseBot: LoadValues() - buyerPrice[%u][%u]        = %u", itemClass, itemQuality, config->GetBuyerPrice(ItemClass(itemClass), ItemQualities(itemQuality)));
                }
            }
            break;
        default:
            sLog.outError("AuctionHouseBot: LoadValues() - Error loading 'data', Unknown Vector ID");
            AHBSeller = FALSE;
            AHBBuyer = FALSE;
            return false;
        }
    }
    return true;
}

int AuctionHouseBot::lookupClass(char cClass[])
{
    const char* class_table[] =  { "consumable", "container", "weapon", "gem", "armor", "reagent", "projectile", "tradegoods", "generic", "recipe", "money", "quiver", "quest", "key", "permanent", "misc", "glyph" };

    for (int itemClass = 0; itemClass <= (sizeof class_table); ++itemClass)
        if (_strnicmp(cClass, class_table[itemClass], 10) == 0)
            return itemClass;
    return MAX_ITEM_CLASS;
}

const char* AuctionHouseBot::lookupClass(ItemClass itemClass)
{
    const char* class_table[] =  { "Consumable", "Container", "Weapon", "Gem", "Armor", "Reagent", "Projectile", "Tradegoods", "Generic", "Recipe", "Money", "Quiver", "Quest", "Key", "Permanent", "Misc", "Glyph" };
    const char* undefined[] =  { "Unknown" };

    if (itemClass <= (sizeof class_table))
        return class_table[itemClass];
    return undefined[0];
}

int AuctionHouseBot::lookupQuality(char quality[])
{
    const char* quality_table[] =  { "poor", "normal", "uncommon", "rare", "epic", "legendary", "artifact", "heirloom" };

    for (int itemQuality = 0; itemQuality <= (sizeof quality_table); ++itemQuality)
        if (_strnicmp(quality, quality_table[itemQuality], 10) == 0)
            return itemQuality;
    return MAX_ITEM_QUALITY;
}

const char* AuctionHouseBot::lookupQuality(ItemQualities itemQuality)
{
    const char* quality_table[] =  { "Poor", "Normal", "Uncommon", "Rare", "Epic", "Legendary", "Artifact", "Heirloom" };
    const char* undefined[] =  { "Unknown" };

    if (itemQuality <= (sizeof quality_table))
        return quality_table[itemQuality];
    return undefined[0];
}

bool AuctionHouseBot::isValidMapID(uint32 ahMapID)
{
    uint32 ahMapID_table[] = { 2, 6, 7 };

    for (int i = 0; i <= (sizeof ahMapID_table); ++i)
        if(ahMapID == ahMapID_table[i])
            return true;
    return false;
}

void AuctionHouseBot::Update()
{
    time_t _newrun = time(NULL);
    if ((!AHBSeller) && (!AHBBuyer))
        return;

    WorldSession _session(AHBplayerAccount, NULL, SEC_PLAYER, true, 0, LOCALE_enUS);
    Player _AHBplayer(&_session);
    _AHBplayer.MinimalLoadFromDB(NULL, AHBplayerGUID);
    ObjectAccessor::Instance().AddObject(&_AHBplayer);

    // Add New Bids
    if (!sWorld.getConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_AUCTION))
    {
        addNewAuctions(&_AHBplayer, &AllianceConfig);
        if (((_newrun - _lastrun_a) > (AllianceConfig.GetBiddingInterval() * 60)) && (AllianceConfig.GetBidsPerInterval() > 0))
        {
            addNewAuctionBuyerBotBid(&_AHBplayer, &AllianceConfig, &_session);
            _lastrun_a = _newrun;
        }

        addNewAuctions(&_AHBplayer, &HordeConfig);
        if (((_newrun - _lastrun_h) > (HordeConfig.GetBiddingInterval() *60)) && (HordeConfig.GetBidsPerInterval() > 0))
        {
            addNewAuctionBuyerBotBid(&_AHBplayer, &HordeConfig, &_session);
            _lastrun_h = _newrun;
        }
    }
    addNewAuctions(&_AHBplayer, &NeutralConfig);
    if (((_newrun - _lastrun_n) > (NeutralConfig.GetBiddingInterval() * 60)) && (NeutralConfig.GetBidsPerInterval() > 0))
    {
        addNewAuctionBuyerBotBid(&_AHBplayer, &NeutralConfig, &_session);
        _lastrun_n = _newrun;
    }

    ObjectAccessor::Instance().RemoveObject(&_AHBplayer);
}
