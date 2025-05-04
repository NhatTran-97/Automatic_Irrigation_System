<?php

$servername = "localhost";
$username = "root";
$password = "";        
$dbname = "awater1";

$day_of = 0; 
 
        $conn = new mysqli($servername, $username, $password, $dbname);
        // Check connection
        if(!$conn)
        {
          die('Could not Connect MySql Server:' .mysql_error());
        }
         
        //$sql = "SELECT  id, auto_1, auto_2, thoigian, thietlap FROM db_control1";
        date_default_timezone_set('Asia/Jakarta');
        
        $day_of_weeks=date('l');

        if($day_of_weeks == 'Monday')
        {
            $day_of = 2;
        }
        elseif ($day_of_weeks == 'Tuesday') {
            $day_of = 3;
        }
        elseif($day_of_weeks == 'Wednesday')
        {
           $day_of = 4; 
        }
        elseif($day_of_weeks == 'Thursday')
        {
           $day_of = 5; 
        }
        elseif($day_of_weeks == 'Friday')
        {
           $day_of = 6; 
        }
        elseif($day_of_weeks == 'Saturday')
        {
           $day_of = 7; 
        }
        elseif($day_of_weeks == 'Sunday')
        {
           $day_of = 8; 
        }

        $hour = date('H');
        $minute = date('i');
        $second = date('s');
        $day = date('d');
        $month = date('m');
        $year = date('Y');

        $result = mysqli_query($conn,"SELECT * FROM db_control");

        if (mysqli_num_rows($result) > 0) {
            while($row = mysqli_fetch_array($result)) 
            {
                 echo  $row["setting_time"]. ','. $row["mon"].','. $row["tue"] .','. $row["wed"]. ','.
                  $row["thur"].',' . $row["sat"].',' . $row["fri"].',' . $row["sun"].',' . $row["auto1"] . ','.
                  $row["auto2"] .','. $row["van01"].','. $row["van02"].','. $row["van03"] .','. $row["van04"] .','. $row["van05"] .','. $row["van06"] .','. $row["van07"] .','. $row["van08"]   .','. $hour.','.$minute.','.$day .','.$month.','. $year.','.$day_of.','.$second .','.$row["vanstate1"].','.$row["vanstate2"].','.$row["vanstate3"].','.$row["vanstate4"].','.$row["vanstate5"].','.$row["vanstate6"].','.$row["vanstate7"].','.$row["vanstate8"].','.$row["config_val"].','.$row["nhietdo"];
            }
        }

        $conn->close();      

    ?>

