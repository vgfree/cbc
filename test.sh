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

function check_cbc_file()
{
	expression="$2,"
	cbc_test_file="/tmp/cbc_test_file.cb"
	echo -e "$expression" > $cbc_test_file
	cbc_result=`./cbc $cbc_test_file`
	rm $cbc_test_file
	check_equals $1 "$cbc_result" "$expression"
}

function check_cbc_text()
{
	cbc_result=`echo "$2" | ./cbc 2>&1`
	# compare strings, skip the line-number
	check_equals_text "$1" "${cbc_result:3}"
}

################################################################################


# print header
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
check_cbc	12345	"| foo | foo := 12345, foo"
check_cbc	0		"| bar | bar"
# checks (control-flow)
check_cbc	0		"| foo | foo := 10, while foo do foo := foo -1, end, foo"
check_cbc	5		"| foo | foo := 10, if foo then 5, else 1, endif"
check_cbc	1		"| foo | foo := 0, if foo then 5, else 1, endif"
check_cbc	1		"| foo | foo := 0, if foo = 1 then 5, else 1, endif"
check_cbc	5		"| foo | foo := 1, if foo = 1 then 5, else 1, endif"
check_cbc	5		"| foo | foo := 0, if foo <> 1 then 5, else 1, endif"
check_cbc	1		"| foo | foo := 1, if foo <> 1 then 5, else 1, endif"
check_cbc	5		"| foo | foo := 6, if foo > 5 then 5, else 1, endif"
check_cbc	1		"| foo | foo := 1, if foo > 5 then 5, else 1, endif"
check_cbc	1		"| foo | foo := 6, if foo < 5 then 5, else 1, endif"
check_cbc	5		"| foo | foo := 1, if foo < 5 then 5, else 1, endif"
check_cbc	5		"| foo | foo := 5, if foo >= 5 then 5, else 1, endif"
check_cbc	1		"| foo | foo := 1, if foo >= 5 then 5, else 1, endif"
check_cbc	5		"| foo | foo := 5, if foo <= 5 then 5, else 1, endif"
check_cbc	5		"| foo | foo := 1, if foo <= 5 then 5, else 1, endif"
check_cbc	5		"| foo, bar, baz | foo := 1, bar := 2, baz := 5, if foo < bar then baz, else 1, endif"
check_cbc	14		"| foo | function bar () foo := foo + 2, end, foo := 10, bar(), bar()"
check_cbc_file	118	"1400\t// line 1\n- 630\t// line 2\n- 13\t// line 3\n- 75\t// line 4\n- 50\t// line 5\n- 10\t// line 6\n- 35\t// line 7\n- 100\t// line 8\n- 100\t// line 9\n- 200\t// line 10\n- (60 + 4 + 5) // line 11\n,\t// line 12"
# error-checks
check_cbc_text	"error: syntax error"						","
check_cbc_text	"error: division by zero is not allowed!"	"3 / 0,"
check_cbc_text	"error: undefined symbol: foo"				"foo,"

echo "-------------------------"
echo "EXECUTED CHECKS: $checks"
echo "SUCCESSFUL:      $successes"
echo "FAILED:          $failures"
