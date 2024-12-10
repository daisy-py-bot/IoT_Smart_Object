-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- Host: localhost
-- Generation Time: Dec 09, 2024 at 10:48 AM
-- Server version: 10.4.28-MariaDB
-- PHP Version: 8.2.4

DROP DATABASE IF EXISTS `iot_smart_object`;
CREATE DATABASE IF NOT EXISTS `iot_smart_object` DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;
USE `iot_smart_object`;

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `iot_smart_object`
--

-- --------------------------------------------------------

--
-- Table structure for table `iot_sensors`
--

CREATE TABLE `iot_sensors` (
  `sensor_id` varchar(36) NOT NULL,
  `name` varchar(100) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;


-- --------------------------------------------------------

--
-- Table structure for table `iot_sensor_readings`
--

CREATE TABLE `iot_sensor_readings` (
  `reading_id` bigint(20) NOT NULL,
  `node_id` varchar(36) DEFAULT NULL,
  `sensor_id` varchar(36) DEFAULT NULL,
  `sensor_value` decimal(10,2) NOT NULL,
  `sensor_reading_timestamp` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--


-- --------------------------------------------------------

--
-- Table structure for table `iot_smart_nodes`
--

CREATE TABLE `iot_smart_nodes` (
  `node_id` varchar(36) NOT NULL,
  `node_description` varchar(255) NOT NULL,
  `location` ENUM('Indoors', 'Outdoors') DEFAULT 'Indoors',
  `created_at` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;


--
-- Indexes for table `iot_sensors`
--
ALTER TABLE `iot_sensors`
  ADD PRIMARY KEY (`sensor_id`);

--
-- Indexes for table `iot_sensor_readings`
--
ALTER TABLE `iot_sensor_readings`
  ADD PRIMARY KEY (`reading_id`),
  ADD KEY `node_id` (`node_id`),
  ADD KEY `iot_sensor_readings_ibfk_2` (`sensor_id`);

--
-- Indexes for table `iot_smart_nodes`
--
ALTER TABLE `iot_smart_nodes`
  ADD PRIMARY KEY (`node_id`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `iot_sensor_readings`
--
ALTER TABLE `iot_sensor_readings`
  MODIFY `reading_id` bigint(20) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=22;

--
-- Constraints for dumped tables
--

--
-- Constraints for table `iot_sensor_readings`
--
ALTER TABLE `iot_sensor_readings`
  ADD CONSTRAINT `iot_sensor_readings_ibfk_1` FOREIGN KEY (`node_id`) REFERENCES `iot_smart_nodes` (`node_id`),
  ADD CONSTRAINT `iot_sensor_readings_ibfk_2` FOREIGN KEY (`sensor_id`) REFERENCES `iot_sensors` (`sensor_id`) ON DELETE CASCADE ON UPDATE CASCADE;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
