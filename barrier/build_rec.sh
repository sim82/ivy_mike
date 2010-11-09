function build {
	N_THR=$1
	N=$2
	M=$3
	PIN=$4
	OPT=$5	
	
	suf=T"$N_THR"_N"$N"_M"$M"_PIN"$PIN"_OPT"$OPT"
	echo $suf	

	g++ -Wall -static $OPT -DBAR_N_THR=$N_THR -DBAR_N=$N -DBAR_M=$M -DBAR_PIN=$PIN -o barrier_RS1_$suf -DBAR_RS1 barrier.cpp -I ../IvyMike/src/ -lboost_thread-mt -lpthread -L ../build_rel/IvyMike/ -livymike	

	g++ -Wall -static $OPT -DBAR_N_THR=$N_THR -DBAR_N=$N -DBAR_M=$M -DBAR_PIN=$PIN -o barrier_RS64_$suf -DBAR_RS64 barrier.cpp -I ../IvyMike/src/ -lboost_thread-mt -lpthread -L ../build_rel/IvyMike/ -livymike

}


#build 16 50000000 0 1 -O0
#build 8 50000000 0 1 -O0
#build 2 50000000 0 1 -O0
#build 4 50000000 0 1 -O0
#build 32 50000000 0 1 -O0


build 16 10000000 0 1 -O0
build 8 10000000 0 1 -O0
build 2 10000000 0 1 -O0
build 4 10000000 0 1 -O0
build 32 10000000 0 1 -O0
build 24 10000000 0 1 -O0


build 16 100000 10000 1 -O0
build 8 100000 10000 1 -O0
build 2 100000 10000 1 -O0
build 4 100000 10000 1 -O0
build 32 100000 10000 1 -O0
build 24 100000 10000 1 -O0


