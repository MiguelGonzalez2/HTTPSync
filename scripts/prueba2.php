<?php
$f = fopen('php://stdin', 'r');
stream_set_blocking($f, false);

$line = fgets($f); 
list($celsius,$value) = explode('=',$line);
if($celsius != "celsius"){
	echo "Introduce un parametro 'celsius' para convertir\n";
	echo "Has introducido: $celsius\r\n";
	fclose($f);
	exit();
}
$farenheit = floatval($value) * 1.8 + 32;

fclose($f);

echo "Temperatura correspondiente a $value C en Farenheit: $farenheit\r\n";
?>
