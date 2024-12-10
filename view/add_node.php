<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Add New Node</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 0;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            background-color: #f7f7f7;
        }
        h1 {
            color: #333;
        }
        form {
            width: 100%;
            max-width: 400px;
            padding: 20px;
            background: #fff;
            border: 1px solid #ccc;
            border-radius: 8px;
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.1);
        }
        label {
            display: block;
            margin-bottom: 8px;
            font-weight: bold;
        }
        input, select, textarea, button {
            width: 100%;
            margin-bottom: 15px;
            padding: 8px;
            border: 1px solid #ccc;
            border-radius: 4px;
        }
        button {
            background-color: #003366;
            color: white;
            font-size: 16px;
            cursor: pointer;
        }
        button:hover {
            background-color: #002245;
        }
        .feedback {
            margin-top: 15px;
            padding: 10px;
            border-radius: 5px;
            display: none;
        }
        .success {
            background-color: #d4edda;
            color: #155724;
            border: 1px solid #c3e6cb;
        }
        .error {
            background-color: #f8d7da;
            color: #721c24;
            border: 1px solid #f5c6cb;
        }
        .back-button {
            background-color: gray;
            color: white;
            border: none;
            text-align: center;
            padding: 8px;
            cursor: pointer;
            /* width: auto; */
            margin-top: 5px;
        }
        .back-button:hover {
            background-color: #333;
        }
    </style>
</head>
<body>
    <form id="addNodeForm">
        <h1>Add New Node</h1>
        <label for="node_id">Node ID:</label>
        <input type="text" id="node_id" name="node_id" placeholder="Enter Node ID" required />

        <label for="node_description">Node Description:</label>
        <textarea id="node_description" name="node_description" placeholder="Enter Node Description" rows="3" required></textarea>

        <label for="location">Location:</label>
        <select id="location" name="location" required>
            <option value="" disabled selected>Select Location</option>
            <option value="Indoors">Indoors</option>
            <option value="Outdoors">Outdoors</option>
        </select>

        <button type="submit">Add Node</button>
        <button type="button" class="back-button" onclick="goBack()">Back to Index</button>

        <div id="feedback" class="feedback"></div>
    </form>

    <script>
        document.getElementById('addNodeForm').addEventListener('submit', async function(event) {
            event.preventDefault();

            // Gather form data
            const nodeId = document.getElementById('node_id').value.trim();
            const nodeDescription = document.getElementById('node_description').value.trim();
            const location = document.getElementById('location').value.trim();

            // Validation check
            if (!nodeId || !nodeDescription || !location) {
                showFeedback('Please fill in all fields.', 'error');
                return;
            }

            // Data to send to the server
            const data = {
                node_id: nodeId,
                node_description: nodeDescription,
                location: location
            };

            try {
                // Send POST request to the server
                const response = await fetch('http://localhost/iot_smart_object/db/create_node.php', {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/json'
                    },
                    body: JSON.stringify(data)
                });

                const result = await response.json();

                // Handle response
                if (result.status === 'success') {
                    showFeedback(`Node added successfully: ${result.node_id}`, 'success');
                    clearForm();
                } else {
                    showFeedback(`Error: ${result.message}`, 'error');
                }
            } catch (error) {
                console.error('Error:', error);
                showFeedback('An unexpected error occurred.', 'error');
            }
        });

        function showFeedback(message, type) {
            const feedback = document.getElementById('feedback');
            feedback.className = `feedback ${type}`;
            feedback.textContent = message;
            feedback.style.display = 'block';
        }

        function clearForm() {
            document.getElementById('node_id').value = '';
            document.getElementById('node_description').value = '';
            document.getElementById('location').value = '';
        }

        function goBack() {
            window.location.href = '../index.php'; // Redirects to index page
        }
    </script>
</body>
</html>
