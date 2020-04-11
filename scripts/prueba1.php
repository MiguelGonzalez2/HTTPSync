<?php
$f = fopen('php://stdin', 'r');
stream_set_blocking($f, false);

$line = fgets($f); 
list($user,$value) = explode('=',$line);

if($user != "user"){
	echo "Introduce un parametro 'user' para convertir\n";
	echo "Has introducido: $user\r\n";
	fclose($f);
	exit();
}

fclose($f);

echo "Hola $value!\r\n";
?>
