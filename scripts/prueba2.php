<?php
$f = fopen('php://stdin', 'r');
stream_set_blocking($f, false);

$line = fgets($f) 
list($celsius,$value) = split('=',$Line);
$farenheit = (int)$celsius * 1.8 + 32
echo "Temperatura correspondiente a $value C en Farenheit: $farenheit";

fclose($f);
?>
