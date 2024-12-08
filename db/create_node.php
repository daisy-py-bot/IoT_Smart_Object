<?php
// display all errors
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);

// Include configuration file for database connection
include 'config.php';

// Include CORS headers for cross-origin requests
header("Access-Control-Allow-Origin: *");
header("Access-Control-Allow-Headers: access-control-allow-origin, content-type");
header("Access-Control-Allow-Methods: POST");
header("Content-Type: application/json; charset=UTF-8");

// Check if the request method is POST
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // Get the raw POST data from the request
    $data = json_decode(file_get_contents('php://input'), true);

    // Check if the required data is provided
    if (isset($data['node_id']) && isset($data['node_description']) && isset($data['location'])) {
        $node_id = $data['node_id'];
        $node_description = $data['node_description'];
        $location = $data['location'];

        // Prepare the SQL query to insert the new node into the database
        $sql = "INSERT INTO iot_smart_nodes (node_id, node_description, location) VALUES (?, ?, ?)";

        // Use prepared statements to prevent SQL injection
        if ($stmt = $conn->prepare($sql)) {
            // Bind the parameters to the prepared statement
            $stmt->bind_param("sss", $node_id, $node_description, $location);

            // Execute the statement
            if ($stmt->execute()) {
                // Return success response
                http_response_code(200);
                echo json_encode(array('status' => 'success', 'message' => 'Node added successfully', 'node_id' => $node_id));

            } else {
                // Return error response if the query fails
                http_response_code(400);
                echo json_encode(array('status' => 'error', 'message' => 'Error adding node'));
            }
        } else {
            // Return error response if the prepared statement fails
            http_response_code(500);
            echo json_encode(array('status' => 'error', 'message' => 'Failed to prepare the query'));
        }
    } else {
        // Return error response if any required field is missing
        http_response_code(400);
        echo json_encode(array('status' => 'error', 'message' => 'Missing required fields'));
    }
} else {
    // Return error response if the request method is not POST
    http_response_code(405);
    echo json_encode(array('status' => 'error', 'message' => 'Invalid request method'));
}

?>
