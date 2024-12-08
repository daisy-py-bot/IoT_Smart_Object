<?php
// display all errors
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);
// Include database connection
include "config.php"; // Update with your actual DB connection file

// include CORS headers
header("Access-Control-Allow-Origin: *");
header("Access-Control-Allow-Methods: GET");


// Set header for JSON response
header('Content-Type: application/json');

// check the request method
if ($_SERVER['REQUEST_METHOD'] !== 'GET') {
    // Return error response if the request method is not GET
    echo json_encode([
        'status' => 'error',
        'message' => 'Invalid request method.'
    ]);
    exit;
}

// Query to get node IDs and descriptions
$query = "SELECT node_id, node_description FROM iot_smart_nodes"; // Adjust the query based on your database structure
$result = mysqli_query($conn, $query);

if ($result) {
    $nodes = [];
    while ($row = mysqli_fetch_assoc($result)) {
        $nodes[] = $row;
    }

    // Return success response with node data
    echo json_encode([
        'status' => 'success',
        'data' => $nodes
    ]);
} else {
    // Return error response if query fails
    echo json_encode([
        'status' => 'error',
        'message' => 'Failed to fetch node IDs.'
    ]);
}
?>
