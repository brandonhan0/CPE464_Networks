#!/bin/bash

echo "Running tests..."
echo "=========================="

run_test () {
    PCAP=$1
    EXPECTED=$2
    OUTPUT="tmp_output.txt"

    echo "Test: $PCAP"

    ./trace "$PCAP" > "$OUTPUT"

    if diff --ignore-all-space --ignore-blank-lines "$OUTPUT" "$EXPECTED" > /dev/null
    then
        echo "PASS"
    else
        echo "FAIL"
        echo "Differences:"
        diff --ignore-all-space --ignore-blank-lines "$OUTPUT" "$EXPECTED" | grep -E '^[<>]'
    fi

    echo "--------------------------"
}

# ===== TEST CASES =====
#run_test IP_bad_checksum.pcap IP_bad_checksum.out # fail
#run_test TCP_bad_checksum.pcap TCP_bad_checksum.out


#run_test UDPfile.pcap UDPfile.out # pass
#run_test PingTest.pcap PingTest.out # pass
#run_test mix_withIPoptions.pcap mix_withIPoptions.out # pass
#run_test smallTCP.pcap smallTCP.out # pass
#run_test Http.pcap Http.out # pass
#run_test largeMix.pcap largeMix.out # pass
#run_test largeMix2.pcap largeMix2.out # pass


echo "Done."