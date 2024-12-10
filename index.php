<?php
// Include config for database connection
include 'config.php';
?>

<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>IoT Smart Objects Dashboard</title>
    <!-- Bootstrap CSS -->
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css" rel="stylesheet">
    <!-- Custom CSS for extra styling -->
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f8f9fa;
        }

        .jumbotron {
            background-color: #f1f1f1; /* Lighter background */
            color: #333; /* Darker text for contrast */
            padding: 40px 20px;
            border-radius: 8px;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1); /* Slight shadow for depth */
        }

        .navbar {
            margin-bottom: 20px;
        }

        .card {
            margin-bottom: 30px; /* Add space between cards */
            border: 1px solid #ddd; /* Subtle border for the cards */
            border-radius: 8px; /* Rounded corners */
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1); /* Soft shadow */
            border-radius: 10px;
            font-size: 12px;
        }

        .card-header {
            background-color:#003366;
            color: white;
        }

        .card-body {
            background-color: #ffffff;
        }

        .footer {
            background-color: #003366;
            color: white;
            text-align: center;
            padding: 10px 0;
            position: fixed;
            bottom: 0;
            width: 100%;
        }

        .container {
            margin-top: 20px;
            margin-bottom: 60px;
        }

        .card-title {
            font-weight: bold;
        }

        .btn-primary {
            background-color: rgb(25, 67, 112);
            border-color: rgb(25, 67, 112);
            font-size: 12px;
        }

        .btn-primary:hover {
            background-color: rgb(31, 115, 204);
            border-color: rgb(25, 67, 112);
        }
        h2{
            font-size: 24px;
        }
       
    </style>
</head>

<body>

   <!-- Navbar -->
<nav class="navbar navbar-expand-lg navbar-dark" style="background-color: #003366;">
    <div class="container-fluid">
        <a class="navbar-brand" href="index.php">IoT</a>
        <button class="navbar-toggler" type="button" data-bs-toggle="collapse" data-bs-target="#navbarNav"
            aria-controls="navbarNav" aria-expanded="false" aria-label="Toggle navigation">
            <span class="navbar-toggler-icon"></span>
        </button>
        <div class="collapse navbar-collapse" id="navbarNav">
            <ul class="navbar-nav mx-auto">
                <li class="nav-item">
                    <a class="nav-link active" aria-current="page" href="index.php">Home</a>
                </li>
                <li class="nav-item">
                    <a class="nav-link" href="view/history.php">Historical</a>
                </li>
                <li class="nav-item">
                    <a class="nav-link" href="view/add_node.php">Add Node</a>
                </li>
                <li class="nav-item">
                    <a class="nav-link" href="view/node_history.php">View Node</a>
                </li>
                <li class="nav-item">
                    <a class="nav-link" href="view/predict_temperature.php">Model</a>
                </li>
            </ul>
        </div>
    </div>
</nav>


    <!-- Jumbotron for introduction -->
    <div class="jumbotron text-center">
        <h2 class="display-4">Welcome!</h2>
        <p class="lead" style="font-size: 12px;">Monitor and manage your IoT-enabled smart objects and sensors from one place.</p>
        <!-- <hr class="my-4"> -->
        <!-- <p style="font-size: 12px;">Explore the data from sensors connected to smart objects, add new smart nodes, and view detailed reports on sensor readings.</p> -->
    </div>

    <!-- Main content cards -->
    <div class="container">
        <div class="row">
            <!-- Card 1: View Historical Data -->
            <div class="col-md-4">
                <div class="card">
                    <div class="card-header">
                        View Historical Data
                    </div>
                    <div class="card-body">
                        <p>Browse and explore the historical sensor readings collected from various smart objects from different envrionments over time.</p>
                        <a href="view/history.php" class="btn btn-primary">View Data</a>
                    </div>
                </div>
            </div>

            <!-- Card 2: Add Smart Node -->
            <div class="col-md-4">
                <div class="card">
                    <div class="card-header">
                        Add Smart Node
                    </div>
                    <div class="card-body">
                        <p>Add new smart nodes to your IoT network. 
                        Each node represents a smart object in your environment with sensors connected to it.</p>
                        <a href="view/add_node.php" class="btn btn-primary">Add Node</a>
                    </div>
                </div>
            </div>

            <!-- Card 3: View Data by Node -->
            <div class="col-md-4">
                <div class="card">
                    <div class="card-header">
                        View Data by Node
                    </div>
                    <div class="card-body">
                        <p>View all the sensor readings from a particular smart node and understand the sensor data it has recorded.</p>
                        <a href="view/node_history.php" class="btn btn-primary">View Data</a>
                    </div>
                </div>
            </div>

            <!-- <div class="col-md-4">
                <div class="card">
                    <div class="card-header">
                        Temperature Prediction
                    </div>
                    <div class="card-body">
                        <p>Get the temperature predictions based on the current time, location, humidity and light intensity</p>
                        <a href="view/node_history.php" class="btn btn-primary">View Data</a>
                    </div>
                </div>
            </div> -->
        </div>
    </div>

    <!-- Footer -->
    <div class="footer">
        <p>&copy; 2024 IoT Smart Objects | Powered by Daisy & Shammah</p>
    </div>

    <!-- Bootstrap JS and dependencies -->
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/js/bootstrap.bundle.min.js"></script>
</body>

</html>
