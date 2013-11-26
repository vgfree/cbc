#!/bin/bash

checks=0
failures=0
successes=0

################################################################################

function check_equals()
{
	# increase check-count
	(( checks += 1 ))
	
	# perform check
	if [ $1 -eq $2 ]; then
		echo "-> CHECK: OK"
		(( successes += 1 ))
		return 0;
	else
		echo "-> CHECK: FAILED"
		(( failures += 1 ))
		echo "   exp: $1, act: $2"
		return 1
	fi
}

function check_cbc()
{
	cbc_result=`echo "$2," | ./cbc`
	check_equals $1 $cbc_result
}

################################################################################


#print header
echo "running cbc unit-test ..."
echo "-------------------------"
# checks
check_cbc 5		"2 + 3"
check_cbc 17	"50 - 33"
check_cbc -1	"2 - 3"
check_cbc 6		"2 * 3"
check_cbc 259	"777 / 3"
check_cbc 0		""
check_cbc 13	"3 + 4 * 3 - 8 / 4"
check_cbc -7	"(3 + 4) * (3 - 8) / 5"
check_cbc 15	"-3 * -5"
echo "-------------------------"
echo "EXECUTED CHECKS: $checks"
echo "SUCCESSFUL:      $successes"
echo "FAILED:          $failures"
