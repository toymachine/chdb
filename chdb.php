<?php

$chdb = chdb_open("hash.chdb");

$value = $chdb->get($argv[1]);
if($value === false) {
    echo "!not found!", PHP_EOL;
}
else {
    echo $value, PHP_EOL;
}

?>
