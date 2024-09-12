#!/bin/bash

. $(dirname "$0")/driver.sh

BMC_TEST=".test.bmc"

BMC_HOST=
BMC_USER=
BMC_PASS=

PDBG_ARM_BUILD=
PDBG_PATH=/tmp/pdbg
PDBG=${PDBG_PATH}/pdbg

function load_config()
{
    if [ ! -f "$BMC_TEST" ] ; then
        echo "Missing file $BMC_TEST, skipping tests"
        return 77
    fi

    fail=0
    . "$BMC_TEST"

    for var in "BMC_HOST" "BMC_USER" "BMC_PASS" "PDBG_ARM_BUILD"; do
        eval value="\$$var"
        if [ -z "$value" ] ; then
            echo "$var not defined in $BMC_TEST"
            fail=1
        fi
    done

    return $fail
}

function copy_pdbg()
{
    sshpass -p "$BMC_PASS" \
        rsync -Pav "${PDBG_ARM_BUILD}/.libs/"* \
        ${BMC_USER}@${BMC_HOST}:${PDBG_PATH}
}

function run_over_ssh()
{
    sshpass -p "$BMC_PASS" \
        ssh ${BMC_USER}@${BMC_HOST} \
        LD_LIBRARY_PATH="${PDBG_PATH}" \
        "$@"
}

test_setup load_config
test_setup copy_pdbg

test_group "BMC HW tests"

hw_state=0

function do_skip()
{
    if [ $hw_state -ne 1 ] ; then
        test_skip
    fi
}


echo -n "Checking if the host is up... "
output=$(run_over_ssh obmcutil state | grep CurrentHostState)
rc=$?
if [ $rc -ne 0 ] || \
    [ "$output" = "CurrentHostState    : xyz.openbmc_project.State.Host.HostState.Running" ] ; then
    echo "yes"
    hw_state=1
else
    echo "no"
    echo "$output"
fi

test_wrapper run_over_ssh

function result_filter()
{
    sed -E -e 's#0x[[:xdigit:]]{16}#HEX16#g' \
        -E -e 's#0x[[:xdigit:]]{8}#HEX8#g' \
        -E -e 's#/.*proc0/pib#PIB0PATH#'
}

test_result 0 <<EOF
p0: 0xc09 = HEX8
EOF

do_skip
test_run $PDBG -p0 getcfam 0xc09

test_result 0 <<EOF
p0: HEX16 = HEX16 (PIB0PATH)
EOF

do_skip
test_run $PDBG -p0 getscom 0xf000f

function result_filter()
{
    result_filter_default
}

test_result 0 <<EOF
Wrote 8 bytes starting at 0x0000000031000000
EOF

do_skip
echo -n "DEADBEEF" | test_run $PDBG -p0 putmem 0x31000000

test_result 0 <<EOF
DEADBEEF
EOF

do_skip
test_run $PDBG -p0 getmem --raw 0x31000000 0x8
