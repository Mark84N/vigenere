#! /bin/bash

INPUT="input/*"

TESTDIR="$(pwd)/testdir"
KEYS=("OPEN" "SAMSUNG" "INTELLIGENCE")

if [ ! -f ./vigenere ]; then
	echo "No executable."
	exit 1
fi

set -e

mkdir $TESTDIR

for src in $INPUT; do
	filename="$(basename $src)"
	printf "____________________________________\n\n"
	printf "File \"%s\": word count %d\n" $filename $(cat $src | wc -w)
	printf "____________________________________\n"

	for key in ${KEYS[@]}; do
		OUT="$TESTDIR/${filename}_${key}"

		printf "Encrypting with \"%s\" ...\n" $key

		./vigenere -e $src -k $key > $OUT

		echo "Encrypted"

		KEY="$(./vigenere -d $OUT | sed -n 's/Key: //p')"

		if [ "$KEY" == "$key" ]; then
			status="Success"
		else
			status="Fail"
		fi

		printf "Decryption finished. Derived key: %s. Status: %s\n\n" $KEY $status
	done
done

rm -rf "$TESTDIR"

set +e


