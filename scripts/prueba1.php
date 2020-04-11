<?php
$f = fopen('php://stdin', 'r');
stream_set_blocking($f, false);

$line = fgets($f) 
list($user,$value) = split('=',$Line);
echo "Hola $value!";

fclose($f);
?>
