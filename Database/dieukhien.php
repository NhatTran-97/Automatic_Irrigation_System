<?php
    $servername = "localhost"; 
    $username = "root";
    $password = "";        
    $dbname = "awater1";

/*Cập nhật vào database*/
    $SETTIME =  $_POST['SETTIME'];
/*
    $MON =  $_POST['MON']; $TUE = $_POST['TUE']; $WED =  $_POST['WED']; $THUR =  $_POST['THUR']; 
    $SAT =  $_POST['SAT']; $FRI =  $_POST['FRI'];$MON =  $_POST['SUN'];
    $AUTO1 =  $_POST['AUTO1']; $AUTO2 =  $_POST['AUTO2'];
*/


    $VAN01 =  $_POST['VAN01'];
    $VAN02 =  $_POST['VAN02'];
    $VAN03 =  $_POST['VAN03'];
    $VAN04 =  $_POST['VAN04'];
    $VAN05 =  $_POST['VAN05'];
    $VAN06 =  $_POST['VAN06'];
    $VAN07 =  $_POST['VAN07'];
    $VAN08 =  $_POST['VAN08'];
    $AUTO1 =  $_POST['AUTO1'];
    $AUTO2 =  $_POST['AUTO2'];

    $THU2 =  $_POST['THU2'];
    $THU3 =  $_POST['THU3'];
    $THU4 =  $_POST['THU4'];
    $THU5 =  $_POST['THU5'];
    $THU6 =  $_POST['THU6'];
    $THU7 =  $_POST['THU7'];
    $THU8 =  $_POST['THU8'];

   


    //echo $date;
    


    /*
    
    $C1 = $_POST['C1'];
    $C1 = "22222333";
    $C2 = $_POST['C2'];
    $RL1 = $_POST['RL1'];
    $RL2 = $_POST['RL2'];
*/
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
        echo "Connect Database OK123!!!";

        if($SETTIME != null)
        {
            $sql = "UPDATE db_control SET setting_time = '$SETTIME'  WHERE id=1";
        }

        else if($VAN01 != null)
        {
            $sql = "UPDATE db_control SET van01 = '$VAN01'  WHERE id=1";
        }
        else if($VAN02 != null)
        {
            $sql = "UPDATE db_control SET van02 = '$VAN02'  WHERE id=1";
        }
        else if($VAN03 != null)
        {
            $sql = "UPDATE db_control SET van03 = '$VAN03'  WHERE id=1";
        }
        else if($VAN04 != null)
        {
            $sql = "UPDATE db_control SET van04 = '$VAN04'  WHERE id=1";
        }
        else if($VAN05 != null)
        {
            $sql = "UPDATE db_control SET van05 = '$VAN05'  WHERE id=1";
        }
        else if($VAN06 != null)
        {
            $sql = "UPDATE db_control SET van06 = '$VAN06'  WHERE id=1";
        }
        else if($VAN07 != null)
        {
            $sql = "UPDATE db_control SET van07 = '$VAN07'  WHERE id=1";
        }
        else if($VAN08 != null)
        {
            $sql = "UPDATE db_control SET van08 = '$VAN08'  WHERE id=1";
        }
        else if($AUTO1 != null)
        {
            $sql = "UPDATE db_control SET auto1 = '$AUTO1'  WHERE id=1";
        }
        else if($AUTO2 != null)
        {
            $sql = "UPDATE db_control SET auto2 = '$AUTO2'  WHERE id=1";
        }
        else if($THU2 != null)
        {
            $sql = "UPDATE db_control SET mon = '$THU2'  WHERE id=1";
        }
        else if($THU3 != null)
        {
            $sql = "UPDATE db_control SET tue = '$THU3'  WHERE id=1";
        }
        else if($THU4 != null)
        {
            $sql = "UPDATE db_control SET wed = '$THU4'  WHERE id=1";
        }
        else if($THU5 != null)
        {
            $sql = "UPDATE db_control SET thur = '$THU5'  WHERE id=1";
        }
        else if($THU6 != null)
        {
            $sql = "UPDATE db_control SET sat = '$THU6'  WHERE id=1";
        }
        else if($THU7 != null)
        {
            $sql = "UPDATE db_control SET fri = '$THU7'  WHERE id=1";
        }
        else if($THU8 != null)
        {
            $sql = "UPDATE db_control SET sun = '$THU8'  WHERE id=1";
        }

      
        /*
        else if($C2 != null)
        {
            $sql = "UPDATE dbdieukhien SET cai2 = '$C2'  WHERE id=1";
        }
        else if($RL1 != null)
        {
            $sql = "UPDATE dbdieukhien SET tb1 = '$RL1'  WHERE id=1";
        }
        else if($RL2 != null)
        {
            $sql = "UPDATE dbdieukhien SET tb2 = '$RL2'  WHERE id=1";
        }

*/

        if ($conn->query($sql) === TRUE) 
        {
            echo "Save OK!!!";  
        } 
        else 
        {
            echo "Save errror !!!";  
        }
        
        
        
     }
    $conn->close();




?>