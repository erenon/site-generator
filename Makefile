valgrind:
	valgrind  --log-file=/tmp/val.grd --leak-check=full --track-origins=yes ./Debug/site-generator Debug/