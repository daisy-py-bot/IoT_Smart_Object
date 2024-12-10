<?php
// dispaly errors
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);

include 'config.php';

// check if the request method is POST
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // get the raw POST data
    $data = json_decode(file_get_contents('php://input'), true);

    // check if the required data is provided
    if ($data && isset($data['location'], $data['humidity'], $data['light_intensity'], $data['timestamp'])) {
        // extract the data from the JSON
        $location = $data['location'];
        $humidity = $data['humidity'];
        $light_intensity = $data['light_intensity'];
        $timestamp = $data['timestamp'];

        // convert location to binary value (1 for 'Indoor', 0 for 'Outdoor')
        $location_indoor = ($location == 'Indoor') ? 1 : 0;

        // log the command to make sure it's correct
        $command = escapeshellcmd("python3 models/predict.py {$timestamp} {$humidity} {$light_intensity} {$location_indoor}");

        // output the command to check if it's correct
        // file_put_contents('log.txt', "Command: {$command}\n", FILE_APPEND);  // Log the command

        $output = shell_exec($command);

        // log the output of the Python script to see if it's executing
        // file_put_contents('log.txt', "Output: {$output}\n", FILE_APPEND);  // Log the output

        // decode the JSON response from the Python script
        $result = json_decode($output, true);

        // Check for decoding errors
        if (json_last_error() !== JSON_ERROR_NONE) {
            error_log("JSON decode error: " . json_last_error_msg());
        }


        echo json_encode($result);

        // check if the result is valid
        if ($result && isset($result['prediction'])) {
            // return the prediction result as JSON response
            // echo all the results



            echo json_encode(['prediction' => $result['prediction']]);
        } else {
            // log error if prediction is not found
            file_put_contents('log.txt', "Error: No prediction returned\n", FILE_APPEND);  // Log error
            echo json_encode(['error' => 'Prediction not found']);
        }
    } else {
        // if the required data is not provided, return an error message
        echo json_encode(['error' => 'Invalid input data']);
    }
} else {
    // if the request method is not POST, return an error message
    echo json_encode(['error' => 'Invalid request method']);
}

// Close the database connection
$conn->close();
?>
