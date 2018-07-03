<?php
//
// Created by Coleflowers on 20/06/2018.
// Updated by JustID on 03/07/2018.
// 

/**
 * 车牌识别PHP扩展程序测试
 */
$res = platescan(realpath("demo.png"),realpath("model"),0.8);
var_dump($res);

?>
