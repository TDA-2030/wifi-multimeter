#!/bin/bash

set -e
set -u

CHECK_FILE_NAME=$1
TEMP_FILE=$2/is_audio_update.log
RET=0

die() {
    echo "${1:-"Unknown Error"}" 1>&2
    exit 1
}

[ -z ${CHECK_FILE_NAME} ] && die "CHECK_FILE_NAME is not set"

if [ ! -e ${TEMP_FILE} ]
then
    echo "not found TEMP_FILE file"
    echo "0" > $TEMP_FILE
fi

CURRENT_MODIFY_TIMESTAMP=$(stat -c %Y  $CHECK_FILE_NAME)
LAST_MODIFY_TIMESTAMP=$(cat $TEMP_FILE)

echo $CURRENT_MODIFY_TIMESTAMP > $TEMP_FILE
# echo "LAST_MODIFY_TIMESTAMP=$LAST_MODIFY_TIMESTAMP"
# echo "CURRENT_MODIFY_TIMESTAMP=$CURRENT_MODIFY_TIMESTAMP"

if [[ $CURRENT_MODIFY_TIMESTAMP -gt $LAST_MODIFY_TIMESTAMP ]]
then
    echo "audio file has updated"
    RET=27
else
    echo "audio file don't need to flash"
fi

exit $RET