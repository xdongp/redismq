for($i=1;$i<3600;$i++){	 	 print  `./redis-cli qpop 116` ;		 print "  ";           	 unless ($i%50){                     print "\n";                  }}
