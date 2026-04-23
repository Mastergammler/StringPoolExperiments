#/usr/bin/time -f "Time: %e s" 
#TIMEFORMAT='%3R s'; time tcc src/compile.c -g -o .build/main -lm && echo "Main was built!"
ninja -v

