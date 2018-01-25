#!/usr/bin/env php 
<?php
    print("CGI is Running\n");
#    $name = $_POST['name'];
#    $text = $_POST['text'];

    $file = "/tmp/pelmanism/data.txt";//ファイル読み込み
#    $data = $name + "\n";
    $data = "This is sample data.\n";
    // FILE_APPEND フラグはファイルの最後に追記することを表し、
    // LOCK_EX フラグは他の人が同時にファイルに書き込めないことを表します。
    file_put_contents($file, $data, FILE_APPEND | LOCK_EX);
?>
