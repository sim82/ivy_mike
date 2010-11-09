bartype = ""

num = {}
time = {}
tmin = {}
tmax = {}

$stdin.each_line do |l|
	if l =~ /setup: barrier types:\s+(\S+)/
		bartype = $1
	elsif l =~ /master time: (\S+)s/
		if time[bartype] == nil
			time[bartype] = 0.0
			num[bartype] = 0
			tmin[bartype] = 100000
			tmax[bartype] = -100000
		end
		t = $1.to_f
		time[bartype] += t
		num[bartype] += 1
		tmin[bartype] = [tmin[bartype],t].min
		tmax[bartype] = [tmax[bartype],t].max
			
	end

end
#bs = ["mutex_spin_barrier"]
#bs = ["lockfree_spin_barrier<1>", "lockfree_spin_barrier<64>", "atomic_spin_barrier", "mutex_spin_barrier"]
bs = ["recursive_spin_barrier<1>", "recursive_spin_barrier<64>"]

#num.keys.each do |k|
class Float                                                                                                                     
    alias_method :round_orig, :round                                                                                            
    def round(n=3)                                                                                                              
        (self * (10.0 ** n)).round_orig * (10.0 ** (-n))                                                                        
    end                                                                                                                         
end 

minval = 1000000
mink = ""

bs.each do |k|
	if( (time[k] / num[k]).round < minval )
		minval = (time[k] / num[k]).round
		mink = k
	end
		
	
end


# Dr. Evil himself could not program this worse!
if ARGV[0] != nil
	mink = 0
end

bs.each do |k|
#	puts "#{k}: #{time[k] / num[k]} #{time[k]} #{num[k]}"

	if ARGV[1] != nil
		puts "#{k}: #{time[k] / num[k]} (#{tmin[k]};#{tmax[k]})"	
	else
		if k == mink
			puts "{\\bf #{(time[k] / num[k]).round}}"	
		else
			puts "#{(time[k] / num[k]).round}"	
			
		end
	end
end
