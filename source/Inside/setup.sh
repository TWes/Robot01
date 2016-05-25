#! /bin/bash

sudo ~/Workspace/GPIO_ACCESS/GPIO_Server &

sleep 1

sudo ~/Workspace/STEERING/STEERING_Server &

sleep 1

sudo ~/Workspace/CAM/CAM_Server &


