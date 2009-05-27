-- MySQL dump 10.11
--
-- Host: localhost    Database: characters
-- ------------------------------------------------------
-- Server version	5.0.77-community-nt

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `auctionhousebot`
--

DROP TABLE IF EXISTS `auctionhousebot`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `auctionhousebot` (
  `auctionhouse` int(11) NOT NULL default '0' COMMENT 'mapID of the auctionhouse',
  `name` char(25) default NULL COMMENT 'Text name of the auctionhouse',
  `data` longtext COMMENT 'Internal data structure',
  `minitems` int(11) default '0' COMMENT 'Minimum number of items you want to keep in the auction house',
  `maxitems` int(11) default '0' COMMENT 'Maximum number of items you want to keep in the auction house',
  `mintime` int(11) default '8' COMMENT 'Minimum number of hours for an auction',
  `maxtime` int(11) default '24' COMMENT 'Maximum number of hours for an auction',
  `buyerbiddinginterval` int(11) default '1' COMMENT 'Interval for how frequently the AHB bids on auctions, value is in minutes',
  `buyerbidsperinterval` int(11) default '1' COMMENT 'Number of bids to put in per bidding interval',
  PRIMARY KEY  (`auctionhouse`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `auctionhousebot`
--

LOCK TABLES `auctionhousebot` WRITE;
/*!40000 ALTER TABLE `auctionhousebot` DISABLE KEYS */;
INSERT INTO `auctionhousebot` VALUES (2,'Alliance','0 8 0 27 12 10 1 0 0 0 1 8 0 10 30 8 2 0 0 0 2 8 100 150 800 1250 2250 3250 5250 0 3 8 150 250 1400 1750 4550 5550 6550 0 4 8 70 70 80 75 80 80 80 0 5 8 100 100 100 100 100 100 100 0 6 8 0 0 3 2 1 1 1 0 7 8 1 1 5 12 15 20 22 0 ',0,0,8,24,1,1),(6,'Horde','0 8 0 27 12 10 1 0 0 0 1 8 0 10 30 8 2 0 0 0 2 8 100 150 800 1250 2250 3250 5250 0 3 8 150 250 1400 1750 4550 5550 6550 0 4 8 70 70 80 75 80 80 80 0 5 8 100 100 100 100 100 100 100 0 6 8 0 0 3 2 1 1 1 0 7 8 1 1 5 12 15 20 22 0 ',0,0,8,24,1,1),(7,'Neutral','0 8 0 27 12 10 1 0 0 0 1 8 0 10 30 8 2 0 0 0 2 8 100 150 800 1250 2250 3250 5250 0 3 8 150 250 1400 1750 4550 5550 6550 0 4 8 70 70 80 75 80 80 80 0 5 8 100 100 100 100 100 100 100 0 6 8 0 0 3 2 1 1 1 0 7 8 1 1 5 12 15 20 22 0 ',0,0,8,24,1,1);
/*!40000 ALTER TABLE `auctionhousebot` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2009-05-27 17:22:40

