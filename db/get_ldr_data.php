<?php
header("Content-Type: application/json; charset=UTF-8");
include 'config.php'; 

try {
    // Query the last 15 LDR readings from the database
    $query = "SELECT sensor_value, sensor_reading_timestamp FROM iot_sensor_readings 
              WHERE sensor_id = 'light' 
              ORDER BY sensor_reading_timestamp DESC 
              LIMIT 15";
    $result = $conn->query($query);

    $ldrData = [];
    while ($row = $result->fetch_assoc()) {
        $ldrData[] = [
            "value" => $row['sensor_value'],
            "timestamp" => $row['sensor_reading_timestamp']
        ];
    }

    echo json_encode($ldrData); 
} catch (Exception $e) {
    http_response_code(500); 
    echo json_encode(["error" => "Failed to fetch LDR data: " . $e->getMessage()]);
}
?>
