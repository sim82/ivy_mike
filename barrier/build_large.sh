function build {
	N_THR=$1
	N=$2
	M=$3
	PIN=$4
	OPT=$5	
	
	suf=T"$N_THR"_N"$N"_M"$M"_PIN"$PIN"_OPT"$OPT"
	echo $suf	

	g++ -Wall -static $OPT -DBAR_N_THR=$N_THR -DBAR_N=$N -DBAR_M=$M -DBAR_PIN=$PIN -o barrier_AS_$suf -DBAR_AS barrier.cpp -I ../IvyMike/src/ -lboost_thread-mt -lpthread -L ../build_rel/IvyMike/ -livymike	

	g++ -Wall -static $OPT -DBAR_N_THR=$N_THR -DBAR_N=$N -DBAR_M=$M -DBAR_PIN=$PIN -o barrier_LS1_$suf -DBAR_LS1 barrier.cpp -I ../IvyMike/src/ -lboost_thread-mt -lpthread -L ../build_rel/IvyMike/ -livymike

	g++ -Wall -static $OPT -DBAR_N_THR=$N_THR -DBAR_N=$N -DBAR_M=$M -DBAR_PIN=$PIN -o barrier_LS64_$suf -DBAR_LS64 barrier.cpp -I ../IvyMike/src/ -lboost_thread-mt -lpthread -L ../build_rel/IvyMike/ -livymike

	g++ -Wall -static $OPT -DBAR_N_THR=$N_THR -DBAR_N=$N -DBAR_M=$M -DBAR_PIN=$PIN -o barrier_MS_$suf -DBAR_MS barrier.cpp -I ../IvyMike/src/ -lboost_thread-mt -lpthread -L ../build_rel/IvyMike/ -livymike
	
	g++ -Wall -static $OPT -DBAR_N_THR=$N_THR -DBAR_N=$N -DBAR_M=$M -DBAR_PIN=$PIN -o barrier_RS1_$suf -DBAR_RS1 barrier.cpp -I ../IvyMike/src/ -lboost_thread-mt -lpthread -L ../build_rel/IvyMike/ -livymike	

	g++ -Wall -static $OPT -DBAR_N_THR=$N_THR -DBAR_N=$N -DBAR_M=$M -DBAR_PIN=$PIN -o barrier_RS64_$suf -DBAR_RS64 barrier.cpp -I ../IvyMike/src/ -lboost_thread-mt -lpthread -L ../build_rel/IvyMike/ -livymike

}


#build 2 1000000 1000000 1 -O0
build 32 1000000 1000000 1 -O0
