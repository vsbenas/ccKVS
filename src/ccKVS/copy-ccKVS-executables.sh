#!/usr/bin/env bash
HOSTS=( "philly" "indianapolis" "houston" "sanantonio" ) #"austin" "atlanta")
#HOSTS=( "austin" "houston" "sanantonio")
#HOSTS=( "austin" "houston" "sanantonio" "indianapolis" "philly" )
#HOSTS=( "austin" "houston" "sanantonio" "indianapolis" "philly" "baltimore" "chicago" "atlanta" "detroit")
LOCAL_HOST=`hostname`
EXECUTABLES=("ccKVS-sc" "run-ccKVS.sh")
HOME_FOLDER="/home/s1513031/ccKVS/src/ccKVS"
DEST_FOLDER="/home/s1513031/ccKVS/src/ccKVS"
if test "$#" -eq 2; then
	echo "building program"
	cd $HOME_FOLDER
	make
	cd -
fi
if test "$#" -eq 0; then
	echo "usage ./copy-executables password (make)"
	exit
fi

for EXEC in "${EXECUTABLES[@]}"
do
	for HOST in "${HOSTS[@]}"
	do
		#echo "making directories"
		#sshpass -p "$1" ssh ${HOST}
		#mkdir ${DEST_FOLDER}
		#exit
		echo "${EXEC} copying to ${HOST}:/${DEST_FOLDER} "
		sshpass -p "$1" scp ${HOME_FOLDER}/${EXEC} ${HOST}:${DEST_FOLDER}/${EXEC}
		#echo "${EXEC} copied to {${HOSTS[@]/$LOCAL_HOST}}"

	done
done
