<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Sensor Readings History</title>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0-alpha1/dist/css/bootstrap.min.css" rel="stylesheet">

    <style>
        /* Apply a navy blue theme */
        body {
            background-color: #f8f9fa;
        }

        th, td {
            text-align: center;
            font-size: 12px;
        }

        th {
            background-color: #003366;
            color: black;
        }

        tbody tr:nth-child(even) {
            background-color: #e1f0ff; /* Lighter navy blue for even rows */
        }

        tbody tr:nth-child(odd) {
            background-color: #c5e1f2; /* Slightly darker navy blue for odd rows */
        }

        tbody tr:hover {
            background-color: #a1d0f7; /* Highlight row on hover */
        }

        .table-bordered {
            border: 2px solid #003366;
        }

        .table-bordered th, .table-bordered td {
            border: 1px solid #003366;
        }
    </style>
</head>
<body>

<!-- Navbar -->
<nav class="navbar navbar-expand-lg navbar-dark" style="background-color: #003366;">
    <div class="container-fluid">
        <a class="navbar-brand" href="../index.php">IoT</a>
        <button class="navbar-toggler" type="button" data-bs-toggle="collapse" data-bs-target="#navbarNav"
                aria-controls="navbarNav" aria-expanded="false" aria-label="Toggle navigation">
            <span class="navbar-toggler-icon"></span>
        </button>
        <div class="collapse navbar-collapse" id="navbarNav">
            <ul class="navbar-nav mx-auto">
                <li class="nav-item">
                    <a class="nav-link" href="../index.php">Home</a>
                </li>
                <li class="nav-item">
                    <a class="nav-link active" href="history.php">Historical</a>
                </li>
                <li class="nav-item">
                    <a class="nav-link" href="add_node.php">Add Node</a>
                </li>
                <li class="nav-item">
                    <a class="nav-link" href="node_history.php">View Node</a>
                </li>
                <li class="nav-item">
                    <a class="nav-link" href="predict_temperature.php">Model</a>
                </li>
            </ul>
        </div>
    </div>
</nav>

<!-- Main content -->
<div class="container mt-4">
    <h2  style="text-align:center">Sensor Readings History</h2>
    <p style="font-size: 12px; text-align:center">View historical data of sensor readings from various smart nodes.</p>
    
    <table class="table table-bordered" id="sensorReadingsTable">
        <thead>
            <tr>
                <th scope="col">Reading ID</th>
                <th scope="col">Sensor Name</th>
                <th scope="col">Sensor Value</th>
                <th scope="col">Node ID</th>
                <th scope="col">Node Description</th>
                <th scope="col">Location</th>
                <th scope="col">Timestamp</th>
            </tr>
        </thead>
        <tbody>
            <!-- Data rows will be added by AJAX -->
        </tbody>
    </table>
</div>

<!-- Script for AJAX and table update -->
<script src="https://code.jquery.com/jquery-3.6.0.min.js"></script>
<script>
    // Function to fetch sensor readings and update the table
    function fetchSensorReadings() {
        $.ajax({
            url: 'http://localhost/iot_smart_object/db/view_all_nodes.php', 
            method: 'GET',
            success: function(response) {
                // Check if the response is successful
                if (response.status === 'success') {
                    let data = response.data;
                    let tableBody = $('#sensorReadingsTable tbody');
                    tableBody.empty(); // Clear current rows

                    // Add new rows
                    data.forEach(function(reading) {
                        let row = `<tr>
                            <td>${reading.reading_id}</td>
                            <td>${reading.sensor_name}</td>
                            <td>${reading.sensor_value}</td>
                            <td>${reading.node_id}</td>
                            <td>${reading.node_description}</td>
                            <td>${reading.location}</td>
                            <td>${reading.sensor_reading_timestamp}</td>
                        </tr>`;
                        tableBody.append(row);
                    });
                }
            },
            error: function() {
                alert('Error fetching data.');
            }
        });
    }

    // Fetch data every 5 seconds
    setInterval(fetchSensorReadings, 5000);

    // Fetch initial data when the page loads
    $(document).ready(function() {
        fetchSensorReadings();
    });
</script>

</body>
</html>
