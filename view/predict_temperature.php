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
    <script src="https://cdn.jsdelivr.net/npm/axios/dist/axios.min.js"></script>
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
            margin-top: 20px;
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
        /* h2{
            font-size: 16px;
        } */
         label {
            font-size:12px;
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
                    <a class="nav-link "  href="index.php">Home</a>
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
                    <a class="nav-link active" aria-current="page" href="view/predict_temperature.php">Model</a>
                </li>
            </ul>
        </div>
    </div>
</nav>


    <!-- Jumbotron for introduction -->
    <div class="jumbotron text-center">
        <h2 class="display-4" style="font-size: 20px;">Temperature Prediction</h2>
        <p class="lead" style="font-size: 12px;">Predict your temperature based on hour, humidity, and light intensity. 
            Simply fill in the form below to get started.</p>
        <!-- <hr class="my-4"> -->
        <!-- <p style="font-size: 12px;">Explore the data from sensors connected to smart objects, add new smart nodes, and view detailed reports on sensor readings.</p> -->
    </div>

    <!-- Main content cards -->
            <!-- Page Content -->
    <div class="container">
       
        <form id="predictionForm">
            <div class="mb-4">
                <label for="hour" class="form-label">Hour of the day:</label>
                <input type="number" class="form-control" id="hour" required min="0" max="23">
            </div>

            <div class="mb-4">
                <label for="humidity" class="form-label">Humidity:</label>
                <input type="number" class="form-control" id="humidity" required>
            </div>

            <div class="mb-4">
                <label for="light_intensity" class="form-label">Light Intensity:</label>
                <input type="number" class="form-control" id="light_intensity" required>
            </div>

            <div class="mb-4">
                <label for="location_indoor" class="form-label">Location:</label>
                <select id="location_indoor" class="form-select" required>
                    <option value="indoor">Indoor</option>
                    <option value="outdoor">Outdoor</option>
                </select>
            </div>

            <button type="submit" class="btn btn-primary">Get Prediction</button>
        </form>

        <!-- Prediction Result -->
        <div id="predictionResult" style="display: none; margin-bottom:80px;">
            <p>Predicted Temperature: <span id="temperature"></span>&#176;C</p>
        </div>
    </div>

        

    <!-- Footer -->
    <div class="footer">
        <p>&copy; 2024 IoT Smart Objects | Powered by Daisy & Shammah</p>
    </div>

    <!-- Bootstrap JS and dependencies -->
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/js/bootstrap.bundle.min.js"></script>

    <script>
        document.getElementById('predictionForm').addEventListener('submit', function (e) {
            e.preventDefault();
            
            // Get the values from the form
            const hour = document.getElementById('hour').value;
            const humidity = document.getElementById('humidity').value;
            const lightIntensity = document.getElementById('light_intensity').value;
            const locationIndoor = document.getElementById('location_indoor').value === "indoor" ? 1 : 0;



            // Send request to the server
            const url = 'http://127.0.0.1:5000/predict';
            axios.post(url, {
                hour: hour,
                humidity: humidity,
                light_intensity: lightIntensity,
                location_indoor: locationIndoor
            })
            .then(response => {
                // Display the prediction result
                document.getElementById('predictionResult').style.display = 'block';
                console.log(response.data);
                document.getElementById('temperature').textContent = parseFloat(response.data.prediction).toFixed(1);

            })
            .catch(error => {
                console.error('Error during prediction:', error);
            });
        });
    </script>
</body>

</html>
