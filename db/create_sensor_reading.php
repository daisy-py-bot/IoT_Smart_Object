<?php
// display all errors on the browser
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);

include 'config.php';

// Include CORS headers
header("Access-Control-Allow-Origin: *");
header("Access-Control-Allow-Headers: access-control-allow-origin, content-type");
header("Access-Control-Allow-Methods: POST");
header("Content-Type: application/json; charset=UTF-8");

// Check the request method
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // Get the data from the request
    $data = json_decode(file_get_contents('php://input'), true);

    // Validate input
    if (!empty($data) && isset($data['node_id'], $data['sensor_id'], $data['sensor_value'])) {
        $node_id = $data['node_id'];         // String
        $sensor_id = $data['sensor_id'];     // String
        $sensor_value = $data['sensor_value']; // Assume string or float

        // Prepare the SQL query
        $stmt = $conn->prepare("INSERT INTO iot_sensor_readings (sensor_id, node_id, sensor_value) VALUES (?, ?, ?)");
        
        if ($stmt) {
            // Bind parameters: `s` = string
            $stmt->bind_param("sss", $sensor_id, $node_id, $sensor_value);

            // Execute the statement
            if ($stmt->execute()) {
                // Return success response
                http_response_code(201); // 201 Created
                echo json_encode(['status' => 'success', 'message' => 'Sensor reading created successfully']);
            } else {
                // Return error response for execution failure
                http_response_code(500); // 500 Internal Server Error
                echo json_encode(['status' => 'error', 'message' => 'Database error: ' . $stmt->error]);
            }

            // Close the statement
            $stmt->close();
        } else {
            // Return error response for statement preparation failure
            http_response_code(500); // 500 Internal Server Error
            echo json_encode(['status' => 'error', 'message' => 'Failed to prepare the SQL statement']);
        }
    } else {
        // Return error response for missing data
        http_response_code(400); // 400 Bad Request
        echo json_encode(['status' => 'error', 'message' => 'Invalid or missing data']);
    }
} else {
    // Return error response for invalid request method
    http_response_code(405); // 405 Method Not Allowed
    echo json_encode(['status' => 'error', 'message' => 'Invalid request method']);
}

?>
