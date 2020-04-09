<?php
	$temperature = $_GET["temperature"];
	$humidity = $_GET["humidity"];
	$return = ($temperature + $humidity) / 2;
	echo $return;
?>
