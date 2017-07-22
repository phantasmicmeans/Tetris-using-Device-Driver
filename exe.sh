#!/bin/bash
sudo mknod /dev/csemad_push_switch_driver c 265 0
sudo mknod /dev/csemad_dot_matrix c 262 0
sudo mknod /dev/fpga_fnd c 261 0
make
sudo insmod push_switch_driver.ko
sudo insmod dot_matrix_driver.ko
sudo insmod fpga_fnd_driver.ko
sudo ./test_dot_tetris

