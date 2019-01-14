-- --------------------------------------------------------
-- 主机:                           192.168.0.68
-- 服务器版本:                        5.7.24-0ubuntu0.18.04.1 - (Ubuntu)
-- 服务器操作系统:                      Linux
-- HeidiSQL 版本:                  9.4.0.5125
-- --------------------------------------------------------

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET NAMES utf8 */;
/*!50503 SET NAMES utf8mb4 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;


-- 导出 smart_contract 的数据库结构
CREATE DATABASE IF NOT EXISTS `smart_contract` /*!40100 DEFAULT CHARACTER SET latin1 */;
USE `smart_contract`;

-- 导出  表 smart_contract.txinfo 结构
CREATE TABLE IF NOT EXISTS `txinfo` (
  `hight` bigint(20) NOT NULL COMMENT '高度',
  `txid` varchar(100) NOT NULL COMMENT '交易哈希',
  PRIMARY KEY (`txid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 导出  表 smart_contract.txinfo_vin 结构
CREATE TABLE IF NOT EXISTS `txinfo_vin` (
  `address` varchar(100) NOT NULL COMMENT '地址',
  `n` int(11) NOT NULL COMMENT 'vin引用的上一笔交易的vout的位置',
  `txid` varchar(100) NOT NULL COMMENT '交易哈希',
  PRIMARY KEY (`n`,`txid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 导出  表 smart_contract.txinfo_vout 结构
CREATE TABLE IF NOT EXISTS `txinfo_vout` (
  `content` varchar(1024) NOT NULL COMMENT 'op_return 内容或者vout地址',
  `n` tinyint(4) NOT NULL COMMENT 'vout在当前交易的下标',
  `txid` varchar(100) NOT NULL COMMENT '交易哈希',
  `flag` tinyint(4) DEFAULT NULL COMMENT 'content flag： 0 表示address，1表示message',
  PRIMARY KEY (`n`,`txid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `utxo` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT COMMENT '主键',
  `txid` varchar(200) NOT NULL COMMENT '交易ID',
  `vout` bigint(20) NOT NULL COMMENT 'vout',
  `address` varchar(200) NOT NULL COMMENT '地址',
  `value` varchar(100) NOT NULL COMMENT '数量',
  `height` bigint(20) NOT NULL COMMENT '块高度',
  `coinbase` smallint(10) NOT NULL COMMENT '是否是挖矿所得',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 导出  表 smart_contract.wallet_info 结构
CREATE TABLE IF NOT EXISTS `wallet_info` (
  `contract` varchar(100) DEFAULT NULL COMMENT '合约地址',
  `asset_id` varchar(100) DEFAULT NULL COMMENT '资产id',
  `qty` bigint(20) DEFAULT NULL COMMENT 'token数量，不同type有不同意义',
  `address` varchar(100) DEFAULT NULL COMMENT '用户地址',
  `txid` varchar(100) DEFAULT NULL COMMENT '交易id',
  `type` tinyint(4) DEFAULT NULL COMMENT '标记合约动作',
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 数据导出被取消选择。
/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
