CURR_DIR_PATH=$(shell pwd)

GCC:=gcc
INC+=-I${CURR_DIR_PATH}/Menu/inc
INC+=-I${CURR_DIR_PATH}/Demo/hmi/inc

SRC+=${CURR_DIR_PATH}/Menu/src/menu.c
SRC+=${CURR_DIR_PATH}/Demo/demo.c
SRC+=${CURR_DIR_PATH}/Demo/hmi/src/mainhmi.c

SRC+=${CURR_DIR_PATH}/Demo/hmi/src/hmi_camera.c
SRC+=${CURR_DIR_PATH}/Demo/hmi/src/hmi_music.c
SRC+=${CURR_DIR_PATH}/Demo/hmi/src/hmi_set.c
SRC+=${CURR_DIR_PATH}/Demo/hmi/src/hmi_more_set.c
SRC+=${CURR_DIR_PATH}/Demo/hmi/src/hmi_video.c


AIM_NAME:=demo

.PHONY:all
all:
	${GCC} ${INC} ${SRC} -o ${CURR_DIR_PATH}/${AIM_NAME}

.PHONY:clean
clean:
	rm -rf ${CURR_DIR_PATH}/${AIM_NAME}