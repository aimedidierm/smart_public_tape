<?php
ini_set('display_errors',1);
ini_set('display_startup_errors',1);
error_reporting(E_ALL);
require 'php-includes/connect.php';
$newamount=0;
if(isset($_GET['kwiyaboneshaamount'])){
    $card = $_GET['card'];
    $kwiyaboneshaamount = $_GET['kwiyaboneshaamount'];
    $data = array('outml' =>'100'); 
    echo $response = json_encode($data);
}
if(isset($_GET['kwishyuraamount'])){
    $amount = $_GET['kwishyuraamount'];
    $card = $_GET['card'];

}
if(isset($_GET['phone'])){
    $phone = $_GET['phone'];
    $amount = $_GET['amount'];
    $data = array('outml' =>'1000'); 
    echo $response = json_encode($data);
}
?>