
DROP DATABASE IF EXISTS iot_smart_object;
CREATE DATABASE iot_smart_object;
USE iot_smart_object;


-- Table for smart nodes
CREATE TABLE iot_smart_nodes (
    node_id VARCHAR(36) PRIMARY KEY, -- Use UUID for uniqueness
    node_description VARCHAR(255) NOT NULL,
    location VARCHAR(100) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Table for sensors
CREATE TABLE iot_sensors (
    sensor_id VARCHAR(36) PRIMARY KEY,
    name VARCHAR(100) NOT NULL
);

-- Table for sensor readings
CREATE TABLE iot_sensor_readings (
    reading_id BIGINT AUTO_INCREMENT PRIMARY KEY,
    node_id VARCHAR(36),
    sensor_id VARCHAR(36),
    sensor_value DECIMAL(10, 2) NOT NULL,
    sensor_reading_timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (node_id) REFERENCES iot_smart_nodes(node_id),
    FOREIGN KEY (sensor_id) REFERENCES iot_sensors(sensor_id)
);
