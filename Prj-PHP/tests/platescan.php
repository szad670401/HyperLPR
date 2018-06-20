<?php
//
// Created by Coleflowers on 20/06/2018.
// 

/**
 * 车牌识别PHP扩展程序测试
 */
$path = realpath("demo.png"); 
$res = platescan($path);
var_dump($res);

?>
