ET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL';

-- Add new column to database
ALTER TABLE `auctionhousebot` ADD COLUMN `data` LONGTEXT NULL DEFAULT NULL COMMENT 'Internal data structure'  AFTER `name` ;

-- Modify existing column comments
ALTER TABLE `auctionhousebot` MODIFY COLUMN `auctionhouse` INT(11) NOT NULL DEFAULT 0 COMMENT 'mapID of the auctionhouse',
 MODIFY COLUMN `name` CHAR(25) CHARACTER SET utf8 COLLATE utf8_general_ci DEFAULT NULL COMMENT 'Text name of the auctionhouse',
 MODIFY COLUMN `minitems` INT(11) DEFAULT 0 COMMENT 'Minimum number of items you want to keep in the auction house',
 MODIFY COLUMN `maxitems` INT(11) DEFAULT 0 COMMENT 'Maximum number of items you want to keep in the auction house',
 MODIFY COLUMN `mintime` INT(11) DEFAULT 8 COMMENT 'Minimum number of hours for an auction',
 MODIFY COLUMN `maxtime` INT(11) DEFAULT 24 COMMENT 'Maximum number of hours for an auction',
 MODIFY COLUMN `buyerbiddinginterval` INT(11) DEFAULT 1 COMMENT 'Interval for how frequently the AHB bids on auctions, value is in minutes',
 MODIFY COLUMN `buyerbidsperinterval` INT(11) DEFAULT 1 COMMENT 'Number of bids to put in per bidding interval';


-- Upgrade existing data into new data column
UPDATE `auctionhousebot` SET `data` = CONCAT_WS(' ',
 '0','8',percentgreytradegoods,percentwhitetradegoods,percentgreentradegoods,percentbluetradegoods,percentpurpletradegoods,percentorangetradegoods,percentyellowtradegoods,'0',
 '1','8',percentgreyitems,percentwhiteitems,percentgreenitems,percentblueitems,percentpurpleitems,percentorangeitems,percentyellowitems,'0',
 '2','8',minpricegrey,minpricewhite,minpricegreen,minpriceblue,minpricepurple,minpriceorange,minpriceyellow,'0',
 '3','8',maxpricegrey,maxpricewhite,maxpricegreen,maxpriceblue,maxpricepurple,maxpriceorange,maxpriceyellow,'0',
 '4','8',minbidpricegrey,minbidpricewhite,minbidpricegreen,minbidpriceblue,minbidpricepurple,minbidpriceorange,minbidpriceyellow,'0',
 '5','8',maxbidpricegrey,maxbidpricewhite,maxbidpricegreen,maxbidpriceblue,maxbidpricepurple,maxbidpriceorange,maxbidpriceyellow,'0',
 '6','8',maxstackgrey,maxstackwhite,maxstackgreen,maxstackblue,maxstackpurple,maxstackorange,maxstackyellow,'0',
 '7','8',buyerpricegrey,buyerpricewhite,buyerpricegreen,buyerpriceblue,buyerpricepurple,buyerpriceorange,buyerpriceyellow,'0 ');

-- Remove old unused columns from database
ALTER TABLE `auctionhousebot` DROP COLUMN `buyerpriceblue` ,
 DROP COLUMN `buyerpricegreen` ,
 DROP COLUMN `buyerpricegrey` ,
 DROP COLUMN `buyerpriceorange` ,
 DROP COLUMN `buyerpricepurple` ,
 DROP COLUMN `buyerpricewhite` ,
 DROP COLUMN `buyerpriceyellow` ,
 DROP COLUMN `maxbidpriceblue` ,
 DROP COLUMN `maxbidpricegreen` ,
 DROP COLUMN `maxbidpricegrey` ,
 DROP COLUMN `maxbidpriceorange` ,
 DROP COLUMN `maxbidpricepurple` ,
 DROP COLUMN `maxbidpricewhite` ,
 DROP COLUMN `maxbidpriceyellow` ,
 DROP COLUMN `maxpriceblue` ,
 DROP COLUMN `maxpricegreen` ,
 DROP COLUMN `maxpricegrey` ,
 DROP COLUMN `maxpriceorange` ,
 DROP COLUMN `maxpricepurple` ,
 DROP COLUMN `maxpricewhite` ,
 DROP COLUMN `maxpriceyellow` ,
 DROP COLUMN `maxstackblue` ,
 DROP COLUMN `maxstackgreen` ,
 DROP COLUMN `maxstackgrey` ,
 DROP COLUMN `maxstackorange` ,
 DROP COLUMN `maxstackpurple` ,
 DROP COLUMN `maxstackwhite` ,
 DROP COLUMN `maxstackyellow` ,
 DROP COLUMN `minbidpriceblue` ,
 DROP COLUMN `minbidpricegreen` ,
 DROP COLUMN `minbidpricegrey` ,
 DROP COLUMN `minbidpriceorange` ,
 DROP COLUMN `minbidpricepurple` ,
 DROP COLUMN `minbidpricewhite` ,
 DROP COLUMN `minbidpriceyellow` ,
 DROP COLUMN `minpriceblue` ,
 DROP COLUMN `minpricegreen` ,
 DROP COLUMN `minpricegrey` ,
 DROP COLUMN `minpriceorange` ,
 DROP COLUMN `minpricepurple` ,
 DROP COLUMN `minpricewhite` ,
 DROP COLUMN `minpriceyellow` ,
 DROP COLUMN `percentblueitems` ,
 DROP COLUMN `percentbluetradegoods` ,
 DROP COLUMN `percentgreenitems` ,
 DROP COLUMN `percentgreentradegoods` ,
 DROP COLUMN `percentgreyitems` ,
 DROP COLUMN `percentgreytradegoods` ,
 DROP COLUMN `percentorangeitems` ,
 DROP COLUMN `percentorangetradegoods` ,
 DROP COLUMN `percentpurpleitems` ,
 DROP COLUMN `percentpurpletradegoods` ,
 DROP COLUMN `percentwhiteitems` ,
 DROP COLUMN `percentwhitetradegoods` ,
 DROP COLUMN `percentyellowitems` ,
 DROP COLUMN `percentyellowtradegoods` ;

SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;

