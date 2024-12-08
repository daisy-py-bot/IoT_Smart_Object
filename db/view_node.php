<?php

// Include configuration file for database connection
include 'config.php';

// Include CORS headers for cross-origin requests
header("Access-Control-Allow-Origin: *");
header("Access-Control-Allow-Headers: access-control-allow-origin, content-type");
header("Access-Control-Allow-Methods: GET");
header("Content-Type: application/json; charset=UTF-8");

// Check if the request method is GET
if ($_SERVER['REQUEST_METHOD'] === 'GET') {

    // Check if node_id is provided in the query string
    if (isset($_GET['node_id']) && !empty($_GET['node_id'])) {
        $node_id = $_GET['node_id'];

        // Prepare the SQL query to join the sensor readings with their respective sensor and node details
        $sql = "
            SELECT 
                sr.reading_id,
                sr.sensor_value,
                s.name AS sensor_name,
                sn.node_description,
                sn.location,
                sr.sensor_reading_timestamp
            FROM 
                iot_sensor_readings sr
            JOIN 
                iot_sensors s ON sr.sensor_id = s.sensor_id
            JOIN 
                iot_smart_nodes sn ON sr.node_id = sn.node_id
            WHERE 
                sr.node_id = ?
        ";

        // Prepare the SQL statement
        $stmt = $conn->prepare($sql);
        
        // Bind the node_id parameter
        $stmt->bind_param("s", $node_id);

        // Execute the statement
        $stmt->execute();

        // Get the result
        $result = $stmt->get_result();

        // Check if the query returned any results
        if ($result->num_rows > 0) {
            $sensor_readings = array();

            // Fetch the results and add them to an array
            while ($row = $result->fetch_assoc()) {
                $sensor_readings[] = array(
                    'reading_id' => $row['reading_id'],
                    'sensor_value' => $row['sensor_value'],
                    'sensor_name' => $row['sensor_name'],
                    'node_description' => $row['node_description'],
                    'location' => $row['location'],
                    'sensor_reading_timestamp' => $row['sensor_reading_timestamp']
                );
            }

            // Return the readings in JSON format
            echo json_encode(array('status' => 'success', 'data' => $sensor_readings));
        } else {
            // If no readings were found for the node, return a message indicating no data
            http_response_code(404);
            echo json_encode(array('status' => 'error', 'message' => 'No sensor readings found for this node'));
        }

        // Close the prepared statement
        $stmt->close();

    } else {
        // Return an error if node_id is not provided
        http_response_code(400);
        echo json_encode(array('status' => 'error', 'message' => 'Node ID is required'));
    }

} else {
    // Return error response if the request method is not GET
    http_response_code(405);
    echo json_encode(array('status' => 'error', 'message' => 'Invalid request method'));
}

?>
