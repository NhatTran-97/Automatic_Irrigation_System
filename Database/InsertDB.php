<?php
	$servername = "localhost"; // 
    $username = "root";
    $password = "";        
    $dbname = "awater1";

    // connect mysql php
    $conn = mysqli_connect($servername, $username, $password, $dbname);
    mysqli_set_charset($conn, 'UTF8');

    // Kiểm tra kết nối
    if ($conn->connect_error) {
        die("Không thể kết nối tới Database" . $conn->connect_error);
      }
	  else
	  {
		 echo "Đã Kết nối Database" ; // println , 
		
		// date_default_timezone_set('Asia/Jakarta');
		// $d = date("Y-m-d");
		// $t = date("H:i:s");
		// $nhietdo = "111111";
		// $doam = "22222222";
		// $van1 = "33333";
		// $van2 = "4444";
		// $van3 = "5555";
		// $van4 = "33333";
		// $van5 = "4444";
		// $van6 = "5555";
		// $van7 = "5555";
	 //    $sql = "INSERT INTO phpdata (nhietdo, doam, van1,van2,van3,van4,van5,van6,van7 , Date, Time) VALUES ('".$nhietdo."', '".$doam."' , '".$van1."' , '".$van2."' , '".$van3."', '".$van4."' , '".$van5."', '".$van6."', '".$van7."','".$d."', '".$t."')"; 

		// if ($conn->query($sql) === TRUE) 
		// {
		//     echo "OK";
		// } 
		// else 
		// {
		//     echo "Error: " . $sql . "<br>" . $conn->error;
		// }
		
		
		
		
			//Get current date and time
			date_default_timezone_set('Asia/Jakarta');
			$d = date("Y-m-d");
			$t = date("H:i:s");

			if(!empty($_POST['nhietdo']))
			{
					$nhietdo = $_POST['nhietdo'];
					$doam = $_POST['doam'];
					$van1 = $_POST['van1'];
					$van2 = $_POST['van2'];
					$van3 = $_POST['van3'];
					$van4 = $_POST['van4'];
					$van5 = $_POST['van5'];
					$van6 = $_POST['van6'];
					$van7 = $_POST['van7'];
					$van8 = $_POST['van8'];

					$sql = "INSERT INTO phpdata (nhietdo, doam, van1,van2,van3,van4,van5,van6,van7,van8 , Date, Time) VALUES ('".$nhietdo."', '".$doam."' , '".$van1."' , '".$van2."' , '".$van3."', '".$van4."' , '".$van5."', '".$van6."', '".$van7."','".$van8."','".$d."', '".$t."')";    

				if ($conn->query($sql) === TRUE) 
				{
					echo "OK";
				} 
				else 
				{
					echo "Error: " . $sql . "<br>" . $conn->error;
				}
			}
			
			
	  }
	$conn->close();

?>




