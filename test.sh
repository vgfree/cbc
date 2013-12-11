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
		echo "-> CHECK: OK      ($checks)"
		(( successes += 1 ))
		return 0
	else
		echo "-> CHECK: FAILED  ($checks)"
		(( failures += 1 ))
		echo "   exp: $1, act: $2"
		echo "   expression: \`$3'"
		echo ""
		return 1
	fi
}
function check_equals_text()
{
	# increase check-count
	(( checks += 1 ))
	
	# perform check
	if [ "$1" == "$2" ]; then
		echo "-> CHECK: OK      ($checks)"
		(( successes += 1 ))
		return 0
	else
		echo "-> CHECK: FAILED  ($checks)"
		(( failures += 1 ))
		echo "   exp: '$1', act: '$2'"
		echo ""
		return 1
	fi
}

function check_cbc()
{
	expression="$2,"
	cbc_result=`echo "$expression" | ./cbc`
	check_equals $1 "$cbc_result" "$expression"
}

function check_cbc_text()
{
	cbc_result=`echo "$2" | ./cbc 2>&1`
	check_equals_text "$1" "$cbc_result"
}

################################################################################


#print header
echo "running cbc unit-test ..."
echo "-------------------------"

# checks (basic maths-operations)
check_cbc	5		"2 + 3"
check_cbc	17		"50 - 33"
check_cbc	-1		"2 - 3"
check_cbc	6		"2 * 3"
check_cbc	259		"777 / 3"
check_cbc	13		"3 + 4 * 3 - 8 / 4"
check_cbc	-7		"(3 + 4) * (3 - 8) / 5"
check_cbc	15		"-3 * -5"
check_cbc	-100	"-100"
check_cbc	130		"3 + 5 * 11 - (80 - 14) / 33 * 2 - 50 + 3 * 7 * (66 / 11)"
# checks (symbols and identifiers)
check_cbc	12345	"declare foo, foo := 12345, foo"
check_cbc	0		"declare bar, bar"
# error-checks
check_cbc_text	"1: error: syntax error"						","
check_cbc_text	"1: error: division by zero is not allowed!"	"3 / 0,"
check_cbc_text	"1: error: undefined symbol: foo"				"foo,"

echo "-------------------------"
echo "EXECUTED CHECKS: $checks"
echo "SUCCESSFUL:      $successes"
echo "FAILED:          $failures"
