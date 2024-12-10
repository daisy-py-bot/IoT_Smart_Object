SELECT 
    main_readings.node_id,
    main_readings.sensor_reading_timestamp AS timestamp,
    nodes.location,
    temp_readings.sensor_value AS temperature,
    humidity_readings.sensor_value AS humidity,
    light_readings.sensor_value AS light_intensity
FROM 
    iot_sensor_readings main_readings
JOIN 
    iot_smart_nodes nodes ON main_readings.node_id = nodes.node_id
LEFT JOIN 
    iot_sensor_readings temp_readings 
    ON main_readings.node_id = temp_readings.node_id 
    AND main_readings.sensor_reading_timestamp = temp_readings.sensor_reading_timestamp 
    AND temp_readings.sensor_id = 'temp'
LEFT JOIN 
    iot_sensor_readings humidity_readings 
    ON main_readings.node_id = humidity_readings.node_id 
    AND main_readings.sensor_reading_timestamp = humidity_readings.sensor_reading_timestamp 
    AND humidity_readings.sensor_id = 'humidity'
LEFT JOIN 
    iot_sensor_readings light_readings 
    ON main_readings.node_id = light_readings.node_id 
    AND main_readings.sensor_reading_timestamp = light_readings.sensor_reading_timestamp 
    AND light_readings.sensor_id = 'light'
GROUP BY 
    main_readings.node_id, main_readings.sensor_reading_timestamp, nodes.location
ORDER BY 
    main_readings.sensor_reading_timestamp
INTO OUTFILE '/Users/daisytsenesa/Desktop/iot_data/data.csv'
FIELDS TERMINATED BY ',' 
ENCLOSED BY '"'
LINES TERMINATED BY '\n';
