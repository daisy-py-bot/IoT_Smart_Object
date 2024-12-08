<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Sensor History - Select Node</title>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0-alpha1/dist/css/bootstrap.min.css" rel="stylesheet">
    <style>
        .navbar {
            margin-bottom: 20px;
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
        <a class="navbar-brand" href="index.php">IoT</a>
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
                    <a class="nav-link" href="history.php">Historical</a>
                </li>
                <li class="nav-item">
                    <a class="nav-link" href="add_node.php">Add Node</a>
                </li>
                <li class="nav-item">
                    <a class="nav-link active" href="node_history.php">View Node</a>
                </li>
            </ul>
        </div>
    </div>
</nav>

<!-- Main content -->
<div class="container mt-4">
    <h2 style="text-align: center;">Monitor each environment individually</h2>
    <p style="text-align: center; font-size:12px">Select a node to view its sensor history.</p>

    <!-- Dropdown to select node -->
    <div class="mb-3">
        <!-- <label for="nodeSelect" class="form-label">Select Node</label> -->
        <select class="form-select" id="nodeSelect">
            <option value="" disabled selected>Select a Node</option>
            <!-- Node options will be populated dynamically -->
        </select>
    </div>

    <!-- Table to display sensor readings -->
    <table class="table table-bordered" id="sensorReadingsTable">
        <thead>
            <tr>
                <th scope="col">Reading ID</th>
                <th scope="col">Sensor Name</th>
                <th scope="col">Sensor Value</th>
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

<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/js/bootstrap.bundle.min.js"></script>

<!-- Script for AJAX and table update -->
<script src="https://code.jquery.com/jquery-3.6.0.min.js"></script>
<script>
    // Function to fetch available node IDs and populate the dropdown
    function fetchNodeIds() {
        $.ajax({
            url: 'http://localhost/iot_smart_object/db/get_node_ids.php', // Endpoint to get all node IDs
            method: 'GET',
            dataType: 'json',
            success: function(response) {
                // Check if the response is successful
                if (response.status === 'success') {
                    let nodeSelect = $('#nodeSelect');
                    nodeSelect.empty(); // Clear current options
                    nodeSelect.append('<option value="" disabled selected>Select a Node</option>'); // Add default option
                    
                    // Add each node ID to the dropdown
                    response.data.forEach(function(node) {
                        nodeSelect.append(`<option value="${node.node_id}">${node.node_id} - ${node.node_description}</option>`);
                    });
                }
            },
            error: function() {
                alert('Error fetching node IDs.');
            }
        });
    }

    // Function to fetch sensor readings for the selected node
    function fetchSensorReadings(nodeId) {
        $.ajax({
            url: 'http://localhost/iot_smart_object/db/view_node.php', // Endpoint to fetch sensor readings
            method: 'GET',
            data: { node_id: nodeId },
            dataType: 'json',
            success: function(response) {
                // Check if the response is successful
                if (response.status === 'success') {
                    let data = response.data;
                    let tableBody = $('#sensorReadingsTable tbody');
                    tableBody.empty(); // Clear current rows

                    // Add new rows for each sensor reading
                    data.forEach(function(reading) {
                        let row = `<tr>
                            <td>${reading.reading_id}</td>
                            <td>${reading.sensor_name}</td>
                            <td>${reading.sensor_value}</td>
                            <td>${reading.node_description}</td>
                            <td>${reading.location}</td>
                            <td>${reading.sensor_reading_timestamp}</td>
                        </tr>`;
                        tableBody.append(row);
                    });
                }
            },
            error: function() {
                alert('Error fetching sensor readings.');
            }
        });
    }

    // Trigger fetching sensor readings when a node is selected
    $('#nodeSelect').change(function() {
        let nodeId = $(this).val();
        if (nodeId) {
            fetchSensorReadings(nodeId); // Fetch readings for selected node
        }
    });

    // Fetch node IDs when the page loads
    $(document).ready(function() {
        fetchNodeIds();
    });
</script>

</body>
</html>
