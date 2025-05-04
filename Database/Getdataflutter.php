<?php
    //include './connection.php';
	
	$servername = "localhost";
	$username = "root";
	$password = "";        
	$dbname = "awater1";

    // connect mysql php
    $conn = mysqli_connect($servername, $username, $password, $dbname);
    mysqli_set_charset($conn, 'UTF8');

    // Kiểm tra kết nối
    if ($conn->connect_error) 
	{
        die("Không thể kết nối tới Database" . $conn->connect_error);
    }
	else
	{
		$sql = "SELECT id, nhietdo, doam, van1, van2, van3, van4, van5, van6, van7 FROM phpdata ORDER BY id DESC LIMIT 1";
		$response = array();
		$result = $conn->query($sql);
        if ($result->num_rows >= 0) 
        {
            // output data of each row
            while($row = $result->fetch_assoc())
             {
              //echo "id" . $row["id"]. "- nhietdo:" . $row["nhietdo"]. "- doam:" . $row["doam"]. "N";
              //$row_nhietdo = $row["nhietdo"];
              //$row_doam = $row["doam"];
				$response['id'] = $row['id'];
                $response['nhietdo'] = $row['nhietdo'];
                $response['doam'] = $row['doam'];
                $response['van1'] = $row['van1'];
                $response['van2'] = $row['van2'];
                $response['van3'] = $row['van3'];    
                $response['van4'] = $row['van4']; 
                $response['van5'] = $row['van5']; 
                $response['van6'] = $row['van6'];   
                $response['van7'] = $row['van7'];           
             }
             echo json_encode($response);
            $result->free();
        } 
        else 
        {
            echo "0 results";
        }
	}
    
        //$conn->close();     
?>
